/*
 * Viscount Pedalboard to MIDI for Hauptwerk
 * Arduino Uno R3 - Serial MIDI Bridge Approach
 *
 * Scans a matrix pedalboard and outputs MIDI Note On/Off over USB serial.
 * Requires a serial-to-MIDI bridge on the host computer (SerialMidiBridge, Hairless, etc.)
 *
 * Based on wiring diagram:
 * - Columns (MD0-MD7): Digital pins 2-9 (outputs, driven LOW to scan)
 * - Rows: Analog pins A0-A3 (inputs with pullups, read HIGH when not pressed)
 * - Diodes prevent ghosting in the matrix
 */

// ============ CONFIGURATION ============
const uint8_t BASE_NOTE = 36;      // MIDI note 36 = C2, standard AGO pedalboard start
const uint8_t MIDI_CHANNEL = 2;    // Channel 2 for pedals (convention)
const uint8_t VELOCITY = 100;      // Note-on velocity
const unsigned long DEBOUNCE_MS = 5; // Debounce time in milliseconds

// ============ PIN CONFIGURATION ============
// Column pins (outputs) - MD0 to MD7
const uint8_t colPins[] = {2, 3, 4, 5, 6, 7, 8, 9};
const uint8_t NUM_COLS = 8;

// Row pins (inputs with pullups) - A0 to A3
const uint8_t rowPins[] = {A0, A1, A2, A3};
const uint8_t NUM_ROWS = 4;

// ============ STATE TRACKING ============
const uint8_t TOTAL_KEYS = NUM_COLS * NUM_ROWS;
bool keyState[TOTAL_KEYS];           // Current state of each key
unsigned long lastDebounceTime[TOTAL_KEYS]; // Last time key state changed

// ============ SETUP ============
void setup() {
  // Initialize serial at 115200 baud for bridge software
  Serial.begin(115200);

  // Configure column pins as outputs, set HIGH (inactive)
  for (uint8_t i = 0; i < NUM_COLS; i++) {
    pinMode(colPins[i], OUTPUT);
    digitalWrite(colPins[i], HIGH);
  }

  // Configure row pins as inputs with pullups
  for (uint8_t i = 0; i < NUM_ROWS; i++) {
    pinMode(rowPins[i], INPUT_PULLUP);
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
  for (uint8_t col = 0; col < NUM_COLS; col++) {
    // Drive current column LOW
    digitalWrite(colPins[col], LOW);
    delayMicroseconds(10); // Small delay for signal to settle

    // Read all rows
    for (uint8_t row = 0; row < NUM_ROWS; row++) {
      uint8_t keyIndex = col * NUM_ROWS + row;
      bool currentState = !digitalRead(rowPins[row]); // Inverted: LOW = pressed

      // Debouncing: only process if state has been stable
      if (currentState != keyState[keyIndex]) {
        unsigned long now = millis();
        if (now - lastDebounceTime[keyIndex] > DEBOUNCE_MS) {
          keyState[keyIndex] = currentState;
          lastDebounceTime[keyIndex] = now;

          // Send MIDI event
          uint8_t note = BASE_NOTE + keyIndex;
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

    // Restore column to HIGH (inactive)
    digitalWrite(colPins[col], HIGH);
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
