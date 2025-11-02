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
// Column pins (outputs) - MD0 to MD7
// MD0 → Pin 9, MD1 → Pin 8, ..., MD7 → Pin 2
const uint8_t colPins[] = {9, 8, 7, 6, 5, 4, 3, 2};
const uint8_t NUM_COLS = 8;

// Row pins (inputs with pullups) - PS1 to PS4
// PS1 → A0, PS2 → A1, PS3 → A2, PS4 → A3
const uint8_t rowPins[] = {A0, A1, A2, A3};
const uint8_t NUM_ROWS = 4;

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
  Serial.println("  Column 0-7 (left to right)");
  Serial.println("  Row 0-3 (varies by wiring)");
  Serial.println("  Pedal# = (Column × 4) + Row");
  Serial.println();
  Serial.println("Press pedals to test...");
  Serial.println("=================================");
  Serial.println();

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
    delayMicroseconds(10);

    // Read all rows
    for (uint8_t row = 0; row < NUM_ROWS; row++) {
      uint8_t keyIndex = col * NUM_ROWS + row;
      bool currentState = !digitalRead(rowPins[row]); // Inverted: LOW = pressed

      // Debouncing
      if (currentState != keyState[keyIndex]) {
        unsigned long now = millis();
        if (now - lastDebounceTime[keyIndex] > DEBOUNCE_MS) {
          keyState[keyIndex] = currentState;
          lastDebounceTime[keyIndex] = now;

          // Print event
          if (currentState) {
            printPedalPressed(keyIndex, col, row);
          } else {
            printPedalReleased(keyIndex, col, row);
          }
        }
      }
    }

    // Restore column to HIGH
    digitalWrite(colPins[col], HIGH);
  }
}

// ============ DISPLAY FUNCTIONS ============
void printPedalPressed(uint8_t keyIndex, uint8_t col, uint8_t row) {
  Serial.print("✓ PRESSED  | Pedal #");
  Serial.print(keyIndex);
  Serial.print("\t| Column ");
  Serial.print(col);
  Serial.print(" (MD");
  Serial.print(col);
  Serial.print(") | Row ");
  Serial.print(row);
  Serial.print(" (PS");
  Serial.print(row + 1);
  Serial.print(") | MIDI Note: ");
  Serial.println(36 + keyIndex);
}

void printPedalReleased(uint8_t keyIndex, uint8_t col, uint8_t row) {
  Serial.print("✗ RELEASED | Pedal #");
  Serial.print(keyIndex);
  Serial.print("\t| Column ");
  Serial.print(col);
  Serial.print(" (MD");
  Serial.print(col);
  Serial.print(") | Row ");
  Serial.print(row);
  Serial.print(" (PS");
  Serial.print(row + 1);
  Serial.println(")");
}
