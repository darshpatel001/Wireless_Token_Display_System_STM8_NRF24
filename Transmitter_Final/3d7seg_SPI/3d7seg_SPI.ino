
#include <SPI.h>

const int STCP_pin = 8;  // Latch Pin
const int digitPins[3] = {5, 6, 7}; // Left, Middle, Right digits

byte digitPatterns[10] = {
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

int displayDigits[3] = {0, 0, 0};
void SevenSegDisplaySetup(){
   // Set digit control pins
  for (int i = 0; i < 3; i++) {
    pinMode(digitPins[i], OUTPUT);
    digitalWrite(digitPins[i], HIGH); // Disable all digits initially
  }

  pinMode(STCP_pin, OUTPUT);
  digitalWrite(STCP_pin, HIGH);

  // Initialize SPI
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0); // 74HC595 works in MODE0
  SPI.setClockDivider(SPI_CLOCK_DIV4); // Slow enough for stability
}
void setup() {
 SevenSegDisplaySetup();
}

void splitNumber(int num) {
  num = abs(num) % 1000;
  displayDigits[0] = num / 100;
  displayDigits[1] = (num / 10) % 10;
  displayDigits[2] = num % 10;
}

void refreshDisplay() {
  static uint8_t currentDigit = 0;

  // Disable all digits
  for (int i = 0; i < 3; i++) {
    digitalWrite(digitPins[i], HIGH);
  }

  // Send segment data via SPI
  digitalWrite(STCP_pin, LOW); // Begin latch
  SPI.transfer(digitPatterns[displayDigits[currentDigit]]);
  digitalWrite(STCP_pin, HIGH); // End latch (output update)

  // Enable current digit
  digitalWrite(digitPins[currentDigit], LOW);

  // Cycle through digits
  currentDigit = (currentDigit + 1) % 3;
}

void loop() {
  // Static display
  splitNumber(123);
  for (int i = 0; i < 300; i++) {
    refreshDisplay();
    delay(3);
  }

  // Count from 0 to 999
  for (int count = 0; count < 1000; count++) {
    splitNumber(count);
    for (int j = 0; j < 50; j++) {
      refreshDisplay();
      delay(3);
    }
  }
}
