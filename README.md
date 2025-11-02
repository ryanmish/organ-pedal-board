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

### Pin Configuration
```
Columns (drive LOW to scan):
- MD0 → Pin 2
- MD1 → Pin 3
- MD2 → Pin 4
- MD3 → Pin 5
- MD4 → Pin 6
- MD5 → Pin 7
- MD6 → Pin 8
- MD7 → Pin 9

Rows (read with pullups):
- Row 0 → A0 (Black wire)
- Row 1 → A1 (White wire)
- Row 2 → A2 (Yellow wire)
- Row 3 → A3 (Brown wire)
```

## Software Setup

### Arduino Sketch
1. Open `pedalboard_midi/pedalboard_midi.ino` in Arduino IDE
2. Upload to your Arduino Uno R3
3. Note the serial port (e.g., `/dev/cu.usbmodem14101` on macOS or `COM3` on Windows)

### Configuration Options (in sketch)
- `BASE_NOTE`: MIDI note for first pedal (default: 36 = C2, AGO standard)
- `MIDI_CHANNEL`: MIDI channel (default: 2 for pedals)
- `VELOCITY`: Note-on velocity (default: 100)
- `DEBOUNCE_MS`: Debounce time in milliseconds (default: 5ms)

## macOS Setup

### 1. Create Virtual MIDI Port
1. Open **Audio MIDI Setup** (Applications → Utilities)
2. Open **MIDI Studio** (Window → Show MIDI Studio)
3. Double-click **IAC Driver**
4. Check **"Device is online"**
5. Add a new bus, name it **"Pedals"**

### 2. Install Serial-to-MIDI Bridge

**Option A: SerialMidiBridge (Recommended)**
- Download from: [GitHub - SerialMidiBridge](https://github.com/gbevin/SerialMidiBridge)
- Launch the app
- Select your Arduino's serial port
- Set baud rate to **115200**
- Select output port: **IAC: Pedals**

**Option B: Python Script**
```bash
pip install pyserial python-rtmidi
# Run a simple serial-to-MIDI bridge script
# (search for "serialmidi.py" examples online)
```

**Option C: Hairless MIDI (deprecated, may not work on current macOS)**
- Only use if SerialMidiBridge doesn't work
- Download from: [Hairless MIDI](https://projectgus.github.io/hairless-midiserial/)

### 3. Verify MIDI
1. Download and install **MIDI Monitor**: [snoize.com](https://www.snoize.com/MIDIMonitor/)
2. Select source: **IAC: Pedals**
3. Press pedals and verify Note On/Off messages appear

### 4. Configure Hauptwerk
1. Open Hauptwerk
2. Go to MIDI settings
3. Enable input device: **IAC: Pedals**
4. Use organ's auto-detect for pedalboard, or map notes manually

## Windows Setup

### 1. Create Virtual MIDI Port
1. Download and install **loopMIDI**: [Tobias Erichsen](https://www.tobias-erichsen.de/software/loopmidi.html)
2. Launch loopMIDI
3. Create a new port named **"Pedals"**

### 2. Install Serial-to-MIDI Bridge

**Option A: SerialMidiBridge (Recommended)**
- Download from: [GitHub - SerialMidiBridge](https://github.com/gbevin/SerialMidiBridge)
- Launch the app
- Select your Arduino's COM port
- Set baud rate to **115200**
- Select output port: **loopMIDI: Pedals**

**Option B: Hairless MIDI (Works well on Windows)**
- Download from: [Hairless MIDI](https://projectgus.github.io/hairless-midiserial/)
- Select Arduino COM port at **115200 baud**
- Select MIDI output: **loopMIDI: Pedals**

### 3. Verify MIDI
1. Download and install **MIDI-OX**: [midiox.com](http://www.midiox.com/)
2. Configure to watch: **loopMIDI: Pedals**
3. Press pedals and verify Note On/Off messages appear

### 4. Configure Hauptwerk
1. Open Hauptwerk
2. Go to MIDI settings
3. Enable input device: **loopMIDI: Pedals**
4. Use organ's auto-detect for pedalboard, or map notes manually

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
├── README.md                    # This file
├── wiring-diagram.jpeg          # Physical wiring reference
├── pedalboard_midi/
│   └── pedalboard_midi.ino      # Main Arduino sketch
├── blink/
│   └── blink.ino                # Test sketch (heartbeat LED)
└── reset/
    └── reset.ino                # Blank sketch to stop all programs
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
