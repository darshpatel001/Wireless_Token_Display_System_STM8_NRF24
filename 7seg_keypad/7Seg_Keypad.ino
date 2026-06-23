// 7-segment display pins
const int DS_pin = 4;     // Serial Data Input
const int STCP_pin = 14;  // Latch Pin
const int SHCP_pin = 12;  // Clock Pin
const int digitPins[3] = {17, 33, 26}; // Digit enable pins

// Keypad pins
const byte ROWS = 4;
const byte COLS = 4;
byte rowPins[ROWS] = {16, 5, 19, 21}; // Keypad row pins
byte colPins[COLS] = {27, 25, 32, 18}; // Keypad column pins (changed 34 to 18)

// Keypad matrix
const char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

// 7-segment patterns for digits 0-9 (common cathode)
const byte digitPatterns[10] = {
  0x3F, // 0
  0x06, // 1
  0x5B, // 2
  0x4F, // 3
  0x66, // 4
  0x6D, // 5
  0x7D, // 6
  0x07, // 7
  0x7F, // 8
  0x6F  // 9
};

// Variables
static int storedNumber = 0;
int displayDigits[3]; // Digits to display

void setup() {
  Serial.begin(9600);
  while (!Serial);

  validatePins();

  pinMode(DS_pin, OUTPUT);
  pinMode(STCP_pin, OUTPUT);
  pinMode(SHCP_pin, OUTPUT);

  for (int i = 0; i < 3; i++) {
    pinMode(digitPins[i], OUTPUT);
    digitalWrite(digitPins[i], HIGH);
  }

  for (byte r = 0; r < ROWS; r++) {
    pinMode(rowPins[r], INPUT_PULLUP);
  }

  for (byte c = 0; c < COLS; c++) {
    pinMode(colPins[c], OUTPUT);
    digitalWrite(colPins[c], HIGH);
  }

  updateDisplayDigits();
  Serial.println("System initialized");
}

void validatePins() {
  const int allPins[] = {
    DS_pin, STCP_pin, SHCP_pin,
    digitPins[0], digitPins[1], digitPins[2],
    rowPins[0], rowPins[1], rowPins[2], rowPins[3],
    colPins[0], colPins[1], colPins[2], colPins[3]
  };

  for (size_t i = 0; i < sizeof(allPins) / sizeof(allPins[0]); i++) {
    if (allPins[i] < 0 || allPins[i] > 39) {
      Serial.print("FATAL: Invalid GPIO pin ");
      Serial.println(allPins[i]);
      while (1);
    }
  }
}

// Improved keypad scanning with debounce and release wait
char getKey() {
  for (byte c = 0; c < COLS; c++) {
    digitalWrite(colPins[c], LOW); // Pull column LOW

    for (byte r = 0; r < ROWS; r++) {
      if (digitalRead(rowPins[r]) == LOW) { // Key pressed
        delay(10); // debounce delay
        if (digitalRead(rowPins[r]) == LOW) {
          char key = keys[r][c];
          while (digitalRead(rowPins[r]) == LOW); // Wait for release
          delay(10); // debounce release
          digitalWrite(colPins[c], HIGH); // Restore column
          return key;
        }
      }
    }

    digitalWrite(colPins[c], HIGH); // Restore column
  }

  return 0; // No key pressed
}

void updateDisplayDigits() {
  int num = storedNumber % 1000;
  displayDigits[0] = num / 100;
  displayDigits[1] = (num / 10) % 10;
  displayDigits[2] = num % 10;
}

void refreshDisplay() {
  static uint8_t currentDigit = 0;

  for (int i = 0; i < 3; i++) {
    digitalWrite(digitPins[i], HIGH);
  }

  digitalWrite(STCP_pin, LOW);
  shiftOut(DS_pin, SHCP_pin, MSBFIRST, digitPatterns[displayDigits[currentDigit]]);
  digitalWrite(STCP_pin, HIGH);

  digitalWrite(digitPins[currentDigit], LOW);
  currentDigit = (currentDigit + 1) % 3;
}

void handleKeyPress(char key) {
  if (key >= '0' && key <= '9') {
    storedNumber = storedNumber * 10 + (key - '0');
    if (storedNumber > 999) storedNumber %= 1000;
    Serial.print("Number updated: ");
    Serial.println(storedNumber);
    updateDisplayDigits();
  }
  else if (key == '#') {
    storedNumber = 0;
    updateDisplayDigits();
    Serial.println("Display cleared");
  }
  else {
    Serial.print("Unhandled key: ");
    Serial.println(key);
  }
}

void loop() {
  char key = getKey();
  if (key) handleKeyPress(key);

  refreshDisplay();
  delay(2); // Controls refresh rate
}
