# Viscount Pedalboard MIDI Interface

Convert your Viscount organ pedalboard into a USB-MIDI input for Hauptwerk using an Arduino Uno R3—no firmware reflashing required.

## Project Goal

Turn an organ pedalboard with a matrix and diodes into a USB-MIDI input for Hauptwerk virtual pipe organ software. The Arduino Uno R3 speaks raw MIDI bytes over USB serial, and host computer software bridges serial to a virtual MIDI port that Hauptwerk can see.

## Why This Approach?

The Arduino Uno R3 does not enumerate as a native USB-MIDI device. Rather than reflashing the 16U2 chip (HIDUINO), we use a **serial-to-MIDI bridge** on the host computer, which is the recommended approach for Uno-class boards.

## Hardware Setup

### Components
- Arduino Uno R3
- Viscount pedalboard with matrix wiring and diodes
- USB cable (Arduino to computer)

### Wiring (Based on Diagram)
- **Column pins (MD0-MD7)**: Arduino digital pins 2-9 (outputs)
- **Row pins**: Arduino analog pins A0-A3 (inputs with internal pullups)
- **Diodes**: Already present in pedalboard matrix to prevent ghosting

### Wiring Table

**No resistors needed!** The Arduino's internal pullup resistors handle everything.

Simply connect the pedalboard connector wires directly to the Arduino pins:

| Connector Pin # | Pedalboard Wire | Arduino Pin | Type | Notes |
|-----------------|-----------------|-------------|------|-------|
| 1 | MD0 | Digital Pin 9 | OUTPUT | Column 0 (driven LOW to scan) |
| 2 | MD1 | Digital Pin 8 | OUTPUT | Column 1 |
| 3 | PS1 | Analog Pin A0 | INPUT_PULLUP | Row 1 (reads LOW when pressed) |
| 4 | MD2 | Digital Pin 7 | OUTPUT | Column 2 |
| 5 | PS2 | Analog Pin A1 | INPUT_PULLUP | Row 2 |
| 6 | MD3 | Digital Pin 6 | OUTPUT | Column 3 |
| 7 | PS3 | Analog Pin A2 | INPUT_PULLUP | Row 3 |
| 8 | MD4 | Digital Pin 5 | OUTPUT | Column 4 |
| 9 | PS4 | Analog Pin A3 | INPUT_PULLUP | Row 4 |
| 10 | MD5 | Digital Pin 4 | OUTPUT | Column 5 |
| 11 | *(empty)* | — | — | Not connected |
| 12 | MD6 | Digital Pin 3 | OUTPUT | Column 6 |
| 13 | *(empty)* | — | — | Not connected |
| 14 | MD7 | Digital Pin 2 | OUTPUT | Column 7 |

**Important:** The diodes in your pedalboard prevent ghosting and protect against shorts. Make sure they're all oriented the same direction.

## Software Setup

### Step 1: Upload Arduino Sketch
1. Open `pedalboard_midi/pedalboard_midi.ino` in Arduino IDE
2. Upload to your Arduino Uno R3
3. Close Arduino Serial Monitor (important - the MIDI bridge needs the serial port!)

### Step 2: Set Up MIDI Bridge

**OPTION A: Use the GUI App (Easiest!)**

**macOS:**
1. Download `Pedalboard-MIDI-Bridge-macOS.zip` from the releases
2. Unzip and drag the app to Applications
3. Double-click to launch
4. The app will auto-detect your Arduino and MIDI ports
5. Click "Start Bridge"

**Windows:**
1. Install Python dependencies: `pip install pyserial python-rtmidi`
2. Run the build script: `build_windows.bat`
3. The .exe will be created in the `dist` folder
4. Double-click to launch

**OPTION B: Run Python Script (Advanced)**

If you prefer command-line or the GUI doesn't work:

**With GUI:**
```bash
python3 midi_bridge_gui.py
```

**Without GUI (command-line):**
```bash
python3 midi_bridge.py
```

Both require: `pip install pyserial python-rtmidi`

### Step 3: Create Virtual MIDI Port

**macOS:**
1. Open **Audio MIDI Setup** (Applications → Utilities)
2. Window → **Show MIDI Studio**
3. Double-click **IAC Driver**
4. Check **"Device is online"**
5. Click **+** to add a port, name it **"Pedals"**
6. Click **Apply**

**Windows:**
1. Download and install **loopMIDI**: https://www.tobias-erichsen.de/software/loopmidi.html
2. Launch loopMIDI
3. Create a new port named **"Pedals"**

### Step 4: Test MIDI Output

**macOS:**
- Download **MIDI Monitor**: https://www.snoize.com/MIDIMonitor/
- Watch for Note On/Off from IAC: Pedals

**Windows:**
- Download **MIDI-OX**: http://www.midiox.com/
- Watch for Note On/Off from loopMIDI: Pedals

### Step 5: Connect to Hauptwerk (or GarageBand!)
1. Open Hauptwerk (or GarageBand for testing)
2. Go to MIDI settings
3. Enable input: **IAC: Pedals** (Mac) or **loopMIDI: Pedals** (Windows)
4. Use auto-detect to map the pedalboard
5. Play! 🎹

## Quick Start Guide

### TL;DR - Get Playing in 5 Steps:
1. **Wire** pedalboard to Arduino (see wiring table above)
2. **Upload** `pedalboard_midi.ino` to Arduino
3. **Create** virtual MIDI port (IAC Driver on Mac, loopMIDI on Windows)
4. **Run** the MIDI bridge app or Python script
5. **Connect** Hauptwerk/GarageBand to the virtual MIDI port

That's it! Press pedals = hear notes! 🎹

## MIDI Specification

- **Baud rate**: 115200 (for serial bridge)
- **Note On**: `0x90 | channel`, note, velocity 100
- **Note Off**: `0x80 | channel`, note, velocity 0
- **Channel**: 2 (standard for pedals)
- **Note range**: 32 notes starting at MIDI 36 (C2), adjustable via `BASE_NOTE`
- **Debounce**: 5-10ms per key

## Troubleshooting

### No Serial Port Showing Up
- Check USB cable connection
- Try a different USB port
- On Windows, check Device Manager for COM ports
- On macOS, look for `/dev/cu.usbmodem*` entries

### Ghost Notes (Multiple Notes from One Pedal)
- Verify diodes are all oriented the same direction in the matrix
- Check that you're using `Serial.write()` not `Serial.println()` in code

### Notes Not Appearing in Hauptwerk
1. Verify MIDI messages in MIDI Monitor or MIDI-OX first
2. Ensure IAC/loopMIDI port is enabled in Hauptwerk settings
3. Try using Hauptwerk's auto-detect feature for the pedalboard

### Stuck Notes
- Increase `DEBOUNCE_MS` value in sketch (try 10ms)
- Check for loose wiring connections

## Project Structure

```
organ-pedal-board/
├── README.md                              # This file
├── wiring-diagram.jpeg                    # Physical wiring reference
├── pedalboard_midi/
│   └── pedalboard_midi.ino                # Main Arduino MIDI sketch
├── pedal_test/
│   └── pedal_test.ino                     # Test sketch with Serial Monitor output
├── midi_bridge.py                         # Command-line MIDI bridge
├── midi_bridge_gui.py                     # GUI MIDI bridge (Mac/Windows)
├── Pedalboard-MIDI-Bridge-macOS.zip       # Standalone Mac app
├── build_windows.bat                      # Windows build script
├── blink/
│   └── blink.ino                          # Test sketch (heartbeat LED)
└── reset/
    └── reset.ino                          # Blank sketch to reset Arduino
```

## Future: Native USB-MIDI

If you want true plug-and-play USB-MIDI (no bridge software):
- **Arduino Leonardo/Micro/Teensy**: Use a USB-MIDI library with the same matrix code
- **Uno 16U2 reflashing**: Flash HIDUINO firmware via DFU mode (advanced)

For now, the serial bridge approach is simpler and requires no firmware modifications.

## Resources

- [Control-Surface Docs](https://tttapa.github.io/Control-Surface-doc/Doxygen/index.html) - Arduino MIDI library
- [Hairless MIDI-Serial Bridge](https://projectgus.github.io/hairless-midiserial/)
- [loopMIDI for Windows](https://www.tobias-erichsen.de/software/loopmidi.html)
- [Apple IAC Driver Setup](https://support.apple.com/guide/audio-midi-setup/)
- [MIDI Monitor (macOS)](https://www.snoize.com/MIDIMonitor/)
- [MIDI-OX (Windows)](http://www.midiox.com/)
- [QMK Matrix Scanning](https://docs.qmk.fm/#/how_a_matrix_works) - Diode matrix explanation
- [Hauptwerk](https://www.hauptwerk.com/)

## License

This project is open source. Feel free to modify and share!
