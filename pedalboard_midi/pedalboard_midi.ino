/*
 * Viscount Pedalboard to MIDI for Hauptwerk
 * Arduino Uno R3 - Serial MIDI Bridge Approach
 *
 * Scans a matrix pedalboard and outputs MIDI Note On/Off over USB serial.
 * Requires a serial-to-MIDI bridge on the host computer (SerialMidiBridge, Hairless, etc.)
 *
 * Based on wiring diagram:
 * - IMPORTANT: Diodes are oriented so we DRIVE ROWS and READ COLUMNS
 * - Rows (PS1-PS4): PS1→A0, PS2→A1, PS3→A2, PS4→A3 (outputs, driven LOW to scan)
 * - Columns (MD0-MD7): MD0→Pin 9, MD1→Pin 8, ..., MD7→Pin 2 (inputs with pullups)
 * - Diodes prevent ghosting in the matrix
 */

// ============ CONFIGURATION ============
const uint8_t MIDI_CHANNEL = 2;    // Channel 2 for pedals (convention)
const uint8_t VELOCITY = 100;      // Note-on velocity
const unsigned long DEBOUNCE_MS = 5; // Debounce time in milliseconds

// ============ PIN CONFIGURATION ============
// Row pins (outputs, driven LOW to scan) - PS1 to PS4
// PS1 → A0, PS2 → A1, PS3 → A2, PS4 → A3
const uint8_t rowPins[] = {A0, A1, A2, A3};
const uint8_t NUM_ROWS = 4;

// Column pins (inputs with pullups) - MD0 to MD7
// MD0 → Pin 9, MD1 → Pin 8, ..., MD7 → Pin 2
const uint8_t colPins[] = {9, 8, 7, 6, 5, 4, 3, 2};
const uint8_t NUM_COLS = 8;

// ============ MIDI NOTE MAPPING ============
// Maps matrix keyIndex to correct MIDI notes (C2=36 to G4=67)
// Physical left-to-right order: keyIndex 7,6,5,4,3,2,1,0, 15,14,13,12,11,10,9,8, 23,22,21,20,19,18,17,16, 31,30,29,28,27,26,25,24
const uint8_t pedalToMidi[32] = {
  43, 42, 41, 40, 39, 38, 37, 36,  // Row 0: keyIndex 0-7   (MD0→MD7 maps to G2,F#2,F2,E2,D#2,D2,C#2,C2)
  51, 50, 49, 48, 47, 46, 45, 44,  // Row 1: keyIndex 8-15  (MD0→MD7 maps to D#3,D3,C#3,C3,B2,A#2,A2,G#2)
  59, 58, 57, 56, 55, 54, 53, 52,  // Row 2: keyIndex 16-23 (MD0→MD7 maps to B3,A#3,A3,G#3,G3,F#3,F3,E3)
  67, 66, 65, 64, 63, 62, 61, 60   // Row 3: keyIndex 24-31 (MD0→MD7 maps to G4,F#4,F4,E4,D#4,D4,C#4,C4)
};

// ============ STATE TRACKING ============
const uint8_t TOTAL_KEYS = NUM_COLS * NUM_ROWS;
bool keyState[TOTAL_KEYS];           // Current state of each key
unsigned long lastDebounceTime[TOTAL_KEYS]; // Last time key state changed

// ============ SETUP ============
void setup() {
  // Initialize serial at 115200 baud for bridge software
  Serial.begin(115200);

  // Configure row pins as outputs, set HIGH (inactive)
  for (uint8_t i = 0; i < NUM_ROWS; i++) {
    pinMode(rowPins[i], OUTPUT);
    digitalWrite(rowPins[i], HIGH);
  }

  // Configure column pins as inputs with pullups
  for (uint8_t i = 0; i < NUM_COLS; i++) {
    pinMode(colPins[i], INPUT_PULLUP);
  }

  // Initialize key states
  for (uint8_t i = 0; i < TOTAL_KEYS; i++) {
    keyState[i] = false;
    lastDebounceTime[i] = 0;
  }
}

// ============ MAIN LOOP ============
void loop() {
  scanMatrix();
}

// ============ MATRIX SCANNING ============
void scanMatrix() {
  for (uint8_t row = 0; row < NUM_ROWS; row++) {
    // Drive current row LOW
    digitalWrite(rowPins[row], LOW);
    delayMicroseconds(100); // Delay for signal to settle

    // Read all columns
    for (uint8_t col = 0; col < NUM_COLS; col++) {
      uint8_t keyIndex = row * NUM_COLS + col;
      bool currentState = !digitalRead(colPins[col]); // Inverted: LOW = pressed

      // Debouncing: only process if state has been stable
      if (currentState != keyState[keyIndex]) {
        unsigned long now = millis();
        if (now - lastDebounceTime[keyIndex] > DEBOUNCE_MS) {
          keyState[keyIndex] = currentState;
          lastDebounceTime[keyIndex] = now;

          // Send MIDI event using mapping
          uint8_t note = pedalToMidi[keyIndex];
          if (currentState) {
            // Note On
            sendMidiNoteOn(MIDI_CHANNEL, note, VELOCITY);
          } else {
            // Note Off
            sendMidiNoteOff(MIDI_CHANNEL, note);
          }
        }
      }
    }

    // Restore row to HIGH (inactive)
    digitalWrite(rowPins[row], HIGH);
  }
}

// ============ MIDI FUNCTIONS ============
void sendMidiNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
  Serial.write(0x90 | (channel - 1)); // Note On status byte (channel 1-16 -> 0-15)
  Serial.write(note & 0x7F);          // Note number (0-127)
  Serial.write(velocity & 0x7F);      // Velocity (0-127)
}

void sendMidiNoteOff(uint8_t channel, uint8_t note) {
  Serial.write(0x80 | (channel - 1)); // Note Off status byte
  Serial.write(note & 0x7F);          // Note number
  Serial.write(0x00);                 // Velocity 0
}
