/*
 * Pedalboard Connection Test
 *
 * This sketch helps verify your pedalboard wiring is correct.
 * Open Serial Monitor at 115200 baud to see which pedals are being pressed.
 *
 * Expected: 32 pedals total (8 columns × 4 rows)
 * Numbering: Pedal 0 is leftmost, Pedal 31 is rightmost
 */

// ============ PIN CONFIGURATION ============
// IMPORTANT: Diodes are oriented so we drive ROWS and read COLUMNS
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
bool keyState[TOTAL_KEYS];
unsigned long lastDebounceTime[TOTAL_KEYS];
const unsigned long DEBOUNCE_MS = 10;

// ============ SETUP ============
void setup() {
  // Initialize serial for test output
  Serial.begin(115200);

  // Wait for serial connection
  delay(2000);

  Serial.println("=================================");
  Serial.println("Pedalboard Connection Test");
  Serial.println("=================================");
  Serial.println();
  Serial.print("Testing ");
  Serial.print(TOTAL_KEYS);
  Serial.println(" pedals");
  Serial.println();
  Serial.println("Pedal numbering:");
  Serial.println("  Row 0-3 (PS1-PS4)");
  Serial.println("  Column 0-7 (MD0-MD7)");
  Serial.println("  Pedal# = (Row × 8) + Column");
  Serial.println();
  Serial.println("Press pedals to test...");
  Serial.println("=================================");
  Serial.println();

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
    delayMicroseconds(100); // Delay for signal settling

    // Read all columns
    for (uint8_t col = 0; col < NUM_COLS; col++) {
      uint8_t keyIndex = row * NUM_COLS + col;
      bool currentState = !digitalRead(colPins[col]); // Inverted: LOW = pressed

      // Debouncing
      if (currentState != keyState[keyIndex]) {
        unsigned long now = millis();
        if (now - lastDebounceTime[keyIndex] > DEBOUNCE_MS) {
          keyState[keyIndex] = currentState;
          lastDebounceTime[keyIndex] = now;

          // Print event
          if (currentState) {
            printPedalPressed(keyIndex, row, col);
          } else {
            printPedalReleased(keyIndex, row, col);
          }
        }
      }
    }

    // Restore row to HIGH
    digitalWrite(rowPins[row], HIGH);
  }
}

// ============ DISPLAY FUNCTIONS ============
void printPedalPressed(uint8_t keyIndex, uint8_t row, uint8_t col) {
  // Calculate expected note (C2=36 to G4=67, chromatic order left to right)
  // Physical order is: 7,6,5,4,3,2,1,0, 15,14,13,12,11,10,9,8, ...
  uint8_t physicalPosition = 0;
  if (keyIndex == 7) physicalPosition = 0;       // C2
  else if (keyIndex == 6) physicalPosition = 1;  // C#2
  else if (keyIndex == 5) physicalPosition = 2;  // D2
  else if (keyIndex == 4) physicalPosition = 3;  // D#2
  else if (keyIndex == 3) physicalPosition = 4;  // E2
  else if (keyIndex == 2) physicalPosition = 5;  // F2
  else if (keyIndex == 1) physicalPosition = 6;  // F#2
  else if (keyIndex == 0) physicalPosition = 7;  // G2
  else if (keyIndex == 15) physicalPosition = 8;  // G#2
  else if (keyIndex == 14) physicalPosition = 9;  // A2
  else if (keyIndex == 13) physicalPosition = 10; // A#2
  else if (keyIndex == 12) physicalPosition = 11; // B2
  else if (keyIndex == 11) physicalPosition = 12; // C3
  else if (keyIndex == 10) physicalPosition = 13; // C#3
  else if (keyIndex == 9) physicalPosition = 14;  // D3
  else if (keyIndex == 8) physicalPosition = 15;  // D#3
  else if (keyIndex == 23) physicalPosition = 16; // E3
  else if (keyIndex == 22) physicalPosition = 17; // F3
  else if (keyIndex == 21) physicalPosition = 18; // F#3
  else if (keyIndex == 20) physicalPosition = 19; // G3
  else if (keyIndex == 19) physicalPosition = 20; // G#3
  else if (keyIndex == 18) physicalPosition = 21; // A3
  else if (keyIndex == 17) physicalPosition = 22; // A#3
  else if (keyIndex == 16) physicalPosition = 23; // B3
  else if (keyIndex == 31) physicalPosition = 24; // C4
  else if (keyIndex == 30) physicalPosition = 25; // C#4
  else if (keyIndex == 29) physicalPosition = 26; // D4
  else if (keyIndex == 28) physicalPosition = 27; // D#4
  else if (keyIndex == 27) physicalPosition = 28; // E4
  else if (keyIndex == 26) physicalPosition = 29; // F4
  else if (keyIndex == 25) physicalPosition = 30; // F#4
  else if (keyIndex == 24) physicalPosition = 31; // G4

  uint8_t expectedNote = 36 + physicalPosition;

  // Note names array
  const char* noteNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
  uint8_t mappedNoteIndex = (pedalToMidi[keyIndex] - 36) % 12;
  uint8_t mappedOctave = (pedalToMidi[keyIndex] - 36) / 12 + 2;
  uint8_t expectedNoteIndex = (expectedNote - 36) % 12;
  uint8_t expectedOctave = (expectedNote - 36) / 12 + 2;

  Serial.print("✓ PRESSED  | Pedal #");
  Serial.print(keyIndex);
  Serial.print("\t| Row ");
  Serial.print(row);
  Serial.print(" (PS");
  Serial.print(row + 1);
  Serial.print(") | Column ");
  Serial.print(col);
  Serial.print(" (MD");
  Serial.print(col);
  Serial.print(") | Mapped: ");
  Serial.print(noteNames[mappedNoteIndex]);
  Serial.print(mappedOctave);
  Serial.print(" (");
  Serial.print(pedalToMidi[keyIndex]);
  Serial.print(") | Should be: ");
  Serial.print(noteNames[expectedNoteIndex]);
  Serial.print(expectedOctave);
  Serial.print(" (");
  Serial.print(expectedNote);
  Serial.println(")");
}

void printPedalReleased(uint8_t keyIndex, uint8_t row, uint8_t col) {
  Serial.print("✗ RELEASED | Pedal #");
  Serial.print(keyIndex);
  Serial.print("\t| Row ");
  Serial.print(row);
  Serial.print(" (PS");
  Serial.print(row + 1);
  Serial.print(") | Column ");
  Serial.print(col);
  Serial.print(" (MD");
  Serial.print(col);
  Serial.println(")");
}
