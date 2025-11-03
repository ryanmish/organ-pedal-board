#!/usr/bin/env python3
"""
Simple Serial to MIDI Bridge
Bridges Arduino serial output to a virtual MIDI port for Hauptwerk
"""

import serial
import rtmidi
import sys
import time

# Configuration
SERIAL_PORT = '/dev/cu.usbmodem14101'  # Change this to your Arduino port
BAUD_RATE = 115200
MIDI_PORT_NAME = 'IAC Driver Pedals'  # Virtual MIDI port name

def find_serial_port():
    """List available serial ports"""
    import serial.tools.list_ports
    ports = serial.tools.list_ports.comports()
    print("\nAvailable serial ports:")
    for i, port in enumerate(ports):
        print(f"  {i}: {port.device} - {port.description}")
    return ports

def find_midi_port(midiout, port_name):
    """Find MIDI output port by name"""
    available_ports = midiout.get_ports()
    print("\nAvailable MIDI ports:")
    for i, port in enumerate(available_ports):
        print(f"  {i}: {port}")
        if port_name.lower() in port.lower():
            return i
    return None

def main():
    print("=" * 60)
    print("Arduino Pedalboard MIDI Bridge")
    print("=" * 60)

    # Find serial port
    ports = find_serial_port()
    if not ports:
        print("\nERROR: No serial ports found!")
        print("Make sure your Arduino is connected.")
        sys.exit(1)

    # Auto-select Arduino port (or use first available)
    arduino_port = None
    for port in ports:
        if 'usbmodem' in port.device or 'arduino' in port.description.lower():
            arduino_port = port.device
            break

    if not arduino_port:
        arduino_port = ports[0].device

    print(f"\nUsing serial port: {arduino_port}")

    # Setup MIDI output
    midiout = rtmidi.MidiOut()
    midi_port_idx = find_midi_port(midiout, MIDI_PORT_NAME)

    if midi_port_idx is None:
        print(f"\nERROR: Could not find MIDI port matching '{MIDI_PORT_NAME}'")
        print("\nPlease set up IAC Driver first:")
        print("  1. Open Audio MIDI Setup")
        print("  2. Window → Show MIDI Studio")
        print("  3. Double-click IAC Driver")
        print("  4. Check 'Device is online'")
        print("  5. Add a port named 'Pedals'")
        sys.exit(1)

    midiout.open_port(midi_port_idx)
    print(f"Opened MIDI port: {midiout.get_ports()[midi_port_idx]}")

    # Open serial connection
    try:
        ser = serial.Serial(arduino_port, BAUD_RATE, timeout=0.001)
        print(f"Opened serial connection at {BAUD_RATE} baud")
    except Exception as e:
        print(f"\nERROR: Could not open serial port: {e}")
        sys.exit(1)

    print("\n" + "=" * 60)
    print("Bridge is RUNNING! Press Ctrl+C to stop.")
    print("=" * 60)
    print("\nWaiting for MIDI messages from Arduino...")

    # Main bridge loop
    buffer = bytearray()
    try:
        while True:
            # Read available bytes
            if ser.in_waiting > 0:
                data = ser.read(ser.in_waiting)
                buffer.extend(data)

                # Process complete MIDI messages (3 bytes each)
                while len(buffer) >= 3:
                    # MIDI messages are 3 bytes: status, note, velocity
                    status = buffer[0]

                    # Check if this is a valid MIDI message (status byte has high bit set)
                    if status & 0x80:
                        midi_msg = [buffer[0], buffer[1], buffer[2]]

                        # Send to MIDI output
                        midiout.send_message(midi_msg)

                        # Print for debugging
                        msg_type = "Note On " if (status & 0xF0) == 0x90 else "Note Off"
                        note = midi_msg[1]
                        velocity = midi_msg[2]
                        print(f"{msg_type}: Note={note} Velocity={velocity}")

                        # Remove processed bytes
                        buffer = buffer[3:]
                    else:
                        # Invalid status byte, skip it
                        buffer = buffer[1:]

            time.sleep(0.001)  # Small delay to prevent CPU spinning

    except KeyboardInterrupt:
        print("\n\nStopping bridge...")
    finally:
        ser.close()
        midiout.close_port()
        print("Bridge stopped. Goodbye!")

if __name__ == "__main__":
    main()
