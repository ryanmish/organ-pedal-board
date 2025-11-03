#!/usr/bin/env python3
"""
Organ Pedalboard MIDI Bridge - GUI Version
Cross-platform GUI for bridging Arduino serial to MIDI
Works on macOS and Windows
"""

import tkinter as tk
from tkinter import ttk, scrolledtext
import serial
import serial.tools.list_ports
import rtmidi
import threading
import time
import sys

class MIDIBridgeGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("Organ Pedalboard MIDI Bridge")
        self.root.geometry("600x500")

        self.serial_conn = None
        self.midi_out = None
        self.bridge_thread = None
        self.running = False

        self.setup_ui()
        self.refresh_ports()

    def setup_ui(self):
        # Main container
        main_frame = ttk.Frame(self.root, padding="10")
        main_frame.grid(row=0, column=0, sticky=(tk.W, tk.E, tk.N, tk.S))

        # Title
        title_label = ttk.Label(main_frame, text="🎹 Organ Pedalboard MIDI Bridge",
                                font=('Arial', 16, 'bold'))
        title_label.grid(row=0, column=0, columnspan=3, pady=10)

        # Serial Port Selection
        ttk.Label(main_frame, text="Serial Port:").grid(row=1, column=0, sticky=tk.W, pady=5)
        self.serial_var = tk.StringVar()
        self.serial_combo = ttk.Combobox(main_frame, textvariable=self.serial_var,
                                         width=40, state='readonly')
        self.serial_combo.grid(row=1, column=1, pady=5, padx=5)

        # MIDI Port Selection
        ttk.Label(main_frame, text="MIDI Output:").grid(row=2, column=0, sticky=tk.W, pady=5)
        self.midi_var = tk.StringVar()
        self.midi_combo = ttk.Combobox(main_frame, textvariable=self.midi_var,
                                       width=40, state='readonly')
        self.midi_combo.grid(row=2, column=1, pady=5, padx=5)

        # Refresh Button
        refresh_btn = ttk.Button(main_frame, text="🔄 Refresh Ports", command=self.refresh_ports)
        refresh_btn.grid(row=1, column=2, rowspan=2, padx=5)

        # Baud rate is fixed at 115200
        self.baud_rate = 115200

        # Start/Stop Button
        self.start_btn = ttk.Button(main_frame, text="▶ Start Bridge",
                                    command=self.toggle_bridge, style='Accent.TButton')
        self.start_btn.grid(row=3, column=0, columnspan=3, pady=20)

        # Status Label
        self.status_label = ttk.Label(main_frame, text="Status: Stopped",
                                     font=('Arial', 10, 'bold'))
        self.status_label.grid(row=4, column=0, columnspan=3, pady=5)

        # Log Output
        ttk.Label(main_frame, text="Activity Log:").grid(row=5, column=0,
                                                         columnspan=3, sticky=tk.W, pady=5)
        self.log_text = scrolledtext.ScrolledText(main_frame, height=15, width=70,
                                                   state='disabled', wrap=tk.WORD)
        self.log_text.grid(row=6, column=0, columnspan=3, pady=5)

        # Configure grid weights
        self.root.columnconfigure(0, weight=1)
        self.root.rowconfigure(0, weight=1)
        main_frame.columnconfigure(1, weight=1)
        main_frame.rowconfigure(6, weight=1)

    def log(self, message):
        """Add message to log"""
        self.log_text.config(state='normal')
        self.log_text.insert(tk.END, f"{message}\n")
        self.log_text.see(tk.END)
        self.log_text.config(state='disabled')

    def refresh_ports(self):
        """Refresh available serial and MIDI ports"""
        # Get serial ports
        ports = serial.tools.list_ports.comports()
        serial_ports = []
        arduino_port = None

        for port in ports:
            port_str = f"{port.device} - {port.description}"
            serial_ports.append(port_str)
            # Auto-select Arduino
            if 'usbmodem' in port.device.lower() or 'arduino' in port.description.lower():
                arduino_port = port_str

        self.serial_combo['values'] = serial_ports
        if arduino_port:
            self.serial_var.set(arduino_port)
        elif serial_ports:
            self.serial_var.set(serial_ports[0])

        # Get MIDI ports
        midiout = rtmidi.MidiOut()
        midi_ports = midiout.get_ports()
        self.midi_combo['values'] = midi_ports

        # Auto-select IAC Driver or loopMIDI
        for port in midi_ports:
            if 'iac' in port.lower() or 'pedals' in port.lower() or 'loopmidi' in port.lower():
                self.midi_var.set(port)
                break

        if not self.midi_var.get() and midi_ports:
            self.midi_var.set(midi_ports[0])

        self.log(f"Found {len(serial_ports)} serial ports and {len(midi_ports)} MIDI ports")

    def toggle_bridge(self):
        """Start or stop the MIDI bridge"""
        if not self.running:
            self.start_bridge()
        else:
            self.stop_bridge()

    def start_bridge(self):
        """Start the MIDI bridge"""
        # Get selected ports
        serial_selection = self.serial_var.get()
        if not serial_selection:
            self.log("❌ ERROR: No serial port selected!")
            return

        serial_port = serial_selection.split(' - ')[0]
        baud_rate = self.baud_rate

        midi_port_name = self.midi_var.get()
        if not midi_port_name:
            self.log("❌ ERROR: No MIDI port selected!")
            return

        # Open serial connection
        try:
            self.serial_conn = serial.Serial(serial_port, baud_rate, timeout=0.001)
            self.log(f"✅ Opened serial port: {serial_port} at {baud_rate} baud")
        except Exception as e:
            self.log(f"❌ ERROR: Could not open serial port: {e}")
            return

        # Open MIDI connection
        try:
            self.midi_out = rtmidi.MidiOut()
            midi_ports = self.midi_out.get_ports()
            midi_idx = midi_ports.index(midi_port_name)
            self.midi_out.open_port(midi_idx)
            self.log(f"✅ Opened MIDI port: {midi_port_name}")
        except Exception as e:
            self.log(f"❌ ERROR: Could not open MIDI port: {e}")
            self.serial_conn.close()
            return

        # Start bridge thread
        self.running = True
        self.bridge_thread = threading.Thread(target=self.bridge_loop, daemon=True)
        self.bridge_thread.start()

        # Update UI
        self.start_btn.config(text="⏹ Stop Bridge")
        self.status_label.config(text="Status: Running ✅")
        self.serial_combo.config(state='disabled')
        self.midi_combo.config(state='disabled')
        self.log("🎹 Bridge is running! Press pedals to send MIDI...")

    def stop_bridge(self):
        """Stop the MIDI bridge"""
        self.running = False

        if self.bridge_thread:
            self.bridge_thread.join(timeout=1)

        if self.serial_conn:
            self.serial_conn.close()
            self.log("✅ Closed serial port")

        if self.midi_out:
            self.midi_out.close_port()
            self.log("✅ Closed MIDI port")

        # Update UI
        self.start_btn.config(text="▶ Start Bridge")
        self.status_label.config(text="Status: Stopped")
        self.serial_combo.config(state='readonly')
        self.midi_combo.config(state='readonly')
        self.log("⏹ Bridge stopped")

    def bridge_loop(self):
        """Main bridge loop (runs in separate thread)"""
        buffer = bytearray()
        note_count = 0

        while self.running:
            try:
                # Read serial data
                if self.serial_conn.in_waiting > 0:
                    data = self.serial_conn.read(self.serial_conn.in_waiting)
                    buffer.extend(data)

                    # Process MIDI messages (3 bytes each)
                    while len(buffer) >= 3:
                        status = buffer[0]

                        # Check for valid MIDI status byte
                        if status & 0x80:
                            midi_msg = [buffer[0], buffer[1], buffer[2]]

                            # Send to MIDI output
                            self.midi_out.send_message(midi_msg)

                            # Log (but not too often to avoid GUI lag)
                            note_count += 1
                            if note_count % 5 == 0:  # Log every 5th note
                                msg_type = "Note On " if (status & 0xF0) == 0x90 else "Note Off"
                                note = midi_msg[1]
                                velocity = midi_msg[2]
                                self.root.after(0, self.log,
                                              f"♪ {msg_type}: Note={note} Vel={velocity}")

                            buffer = buffer[3:]
                        else:
                            # Invalid status byte, skip it
                            buffer = buffer[1:]

                time.sleep(0.001)

            except Exception as e:
                self.root.after(0, self.log, f"❌ ERROR in bridge loop: {e}")
                break

def main():
    root = tk.Tk()
    app = MIDIBridgeGUI(root)

    # Handle window close
    def on_closing():
        if app.running:
            app.stop_bridge()
        root.destroy()

    root.protocol("WM_DELETE_WINDOW", on_closing)
    root.mainloop()

if __name__ == "__main__":
    main()
