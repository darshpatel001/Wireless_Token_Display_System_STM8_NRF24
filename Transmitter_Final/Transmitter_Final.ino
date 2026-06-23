
#include <SPI.h>

// --- Constants ---
//#define PCF8574_ADDR 0x20 // I2C address of PCF8574 (check A0–A2 wiring)

//Pins
#define NUM_ROWS 4
#define NUM_COLS 4
#define CE_PIN   5
#define CSN_PIN  6

//Shift registers
const int STCP_pin = 15;  // Latch Pin
int displayDigits[3] = {0, 0, 0};

uint8_t rowPins[NUM_ROWS] = {0, 1, 4, 3};
uint8_t colPins[NUM_COLS] = {11, 12, 13, 14};
// nRF24L01+ Register definitions
#define CONFIG      0x00
#define EN_AA       0x01
#define EN_RXADDR   0x02
#define SETUP_RETR  0x04
#define RF_CH       0x05
#define RF_SETUP    0x06
#define STATUS_REG  0x07
#define RX_ADDR_P0  0x0A
#define TX_ADDR     0x10
#define RX_PW_P0    0x11
#define FIFO_STATUS 0x17

#define W_REGISTER    0x20
#define R_REGISTER    0x00
#define FLUSH_TX      0xE1
#define FLUSH_RX      0xE2
#define W_TX_PAYLOAD  0xA0
#define R_RX_PAYLOAD  0x61

byte tx_address[5] = {'A', 'B', 'C', 'D', 'E'};
byte rx_address[5] = {'A', 'B', 'C', 'D', 'E'};

// --- Global Variables ---
int storedNumber = 0;
char Data;
char specialKey = '\0';

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

char keys[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// Function declarations
void SevenSegDisplaySetup();
void csnLow();
void csnHigh();
void ceLow();
void ceHigh();
void writeRegister(uint8_t reg, const byte* data, uint8_t len);
void writeRegisterM(uint8_t reg, byte value);
void flushTX();
void writePayload(byte *data, uint8_t len);
void senddataNRF(int data);
void handleKeypad();
char scanKeypad();
uint8_t readPCF8574();
void F1(char digit);
void F2(char special);
void splitNumber(int num);
void refreshDisplay();

// --- Setup ---
void setup() {

  SPI_begin();
  pinMode(CE_PIN, OUTPUT);
  pinMode(CSN_PIN, OUTPUT);
  ceLow();
  csnHigh();

  pinMode(colPins[0], OUTPUT);

  pinMode(colPins[1], OUTPUT);
  pinMode(colPins[2], OUTPUT);
  pinMode(colPins[3], OUTPUT);

  digitalWrite(colPins[0], HIGH);
  digitalWrite(colPins[1], HIGH);
  digitalWrite(colPins[2], HIGH);
  digitalWrite(colPins[3], HIGH);

  // Setup 7-segment display
  SevenSegDisplaySetup();

  SPI_setBitOrder(MSBFIRST);
  SPI_setDataMode(SPI_MODE0);
  SPI_setClockDivider(SPI_CLOCK_DIV4);
  delay(100);

  // Setup nRF24L01+ TX mode

  // Setup TX mode
  writeRegisterM(CONFIG, 0x0E); // PWR_UP=1, PRIM_RX=0
  writeRegisterM(EN_AA, 0x00);  // Disable auto-ack
  writeRegisterM(EN_RXADDR, 0x01); // Enable data pipe 0
  writeRegisterM(SETUP_RETR, 0x00); // Disable retransmission
  writeRegisterM(RF_CH, 76);    // Channel 76
  writeRegisterM(RF_SETUP, 0x06); // 1Mbps, 0dBm

  writeRegister(RX_ADDR_P0, rx_address, 5);
  writeRegister(TX_ADDR, tx_address, 5);
  writeRegisterM(RX_PW_P0, 4); // Payload width = 10

  flushTX();
  delay(100);
  Data = storedNumber + '0';
}
unsigned long lastmilli = 0 ;
// --- Main Loop ---
void loop() {
  if ((millis() - lastmilli) > 300 )
    handleKeypad();

  splitNumber(storedNumber);

  // Refresh display
  for (int i = 0; i < 50; i++) {
    refreshDisplay();
    delayMicroseconds(1);
    delay(1);
  }
  //  delay(3);

}

void writeRegister(uint8_t reg, const byte* data, uint8_t len) {
  csnLow();
  SPI_transfer(W_REGISTER | (reg & 0x1F));
  for (uint8_t i = 0; i < len; i++) {
    SPI_transfer(data[i]);
  }
  csnHigh();
}

void writeRegisterM(uint8_t reg, byte value) {
  writeRegister(reg, &value, 1);
}

void flushTX() {
  csnLow();
  SPI_transfer(FLUSH_TX);
  csnHigh();
}

void writePayload(byte *data, uint8_t len) {
  csnLow();
  SPI_transfer(W_TX_PAYLOAD);
  for (uint8_t i = 0; i < len; i++) {
    SPI_transfer(data[i]);
  }
  csnHigh();
}

void senddataNRF(int data) {
  data = constrain(data, 0, 2000);
  byte payload[4] = {
    (byte)((data / 1000) %10),        // Thousand Digit
    (byte)((data / 100) %10),         // Hundreds digit
    (byte)((data / 10) % 10),   // Tens digit
    (byte)(data % 10)           // Units digit
  };

  //  digitalWrite(STCP_pin, HIGH);  // This line seems unnecessary here unless used elsewhere

  flushTX();                     // Clear previous TX buffer (optional, if needed before every send)
  writePayload(payload, 4);      // Correct CSN handling happens inside

  ceHigh();
  delayMicroseconds(70);
  ceLow();
}

void SevenSegDisplaySetup() {
  // Set digit control pins
  pinMode(STCP_pin, OUTPUT);
  digitalWrite(STCP_pin, HIGH);
}

void csnLow() {
  digitalWrite(CSN_PIN, LOW);
}

void csnHigh() {
  digitalWrite(CSN_PIN, HIGH);
}

void ceLow() {
  digitalWrite(CE_PIN, LOW);
}

void ceHigh() {
  digitalWrite(CE_PIN, HIGH);
}

// --- Handle keypad input ---
void handleKeypad() {
  char key = scanKeypad();

  if (key != '\0') {
    lastmilli = millis();
    if (key >= '0' && key <= '9') {
      F1(key); // Digit entered
    }
    else {
      F2(key); // Special key
      switch (specialKey) {
        case 'A':
          storedNumber += 1;
          break;
        case 'B':
          storedNumber -= 1;
          break;
        case 'C':
          storedNumber = 0;
          break;
        case 'D':
          senddataNRF(storedNumber);
          break;
        case '#':
          senddataNRF(1000);
      }
      specialKey = '\0';
    }
  }
}

char scanKeypad() {
  for (int c = 0; c < NUM_COLS; c++) {
    digitalWrite(colPins[c], LOW);

    for (int r = 0; r < NUM_ROWS; r++) {
      if (digitalRead(rowPins[r]) == LOW) {
        delay(10);

        if (digitalRead(rowPins[r]) == LOW) {
          digitalWrite(colPins[c], HIGH);
          return keys[c][r];
        }
      }
    }

    digitalWrite(colPins[c], HIGH);
  }

  return '\0';
}



// --- Append and live update digits ---
void F1(char digit) {
  int d = digit - '0';
  // Keep only last 3 digits
  storedNumber = ((storedNumber % 100) * 10) + d;
}

// --- Handle special key ---
void F2(char special) {
  specialKey = special;
}

// --- Split number into 3 digits ---
void splitNumber(int num) {
  num = abs(num) % 1000;
  displayDigits[0] = num / 100;
  displayDigits[1] = (num / 10) % 10;
  displayDigits[2] = num % 10;
}



void refreshDisplay() {
  static uint8_t currentDigit = 0;

  // Digit control byte (active LOW)
  byte digitControl = 0xFF;
  bitClear(digitControl, currentDigit);  // Enable only current digit

  byte segmentData = digitPatterns[displayDigits[currentDigit]];

  digitalWrite(STCP_pin, LOW);
  SPI_transfer(digitControl);  // Second byte: digit control
  SPI_transfer(segmentData);   // First byte: segment pattern
  //  SPI_transfer(digitControl);  // Second byte: digit control
  digitalWrite(STCP_pin, HIGH);

  currentDigit = (currentDigit + 1) % 3;
}
