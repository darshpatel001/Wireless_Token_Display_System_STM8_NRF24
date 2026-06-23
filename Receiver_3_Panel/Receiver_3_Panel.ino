#include "P10_SPI_DISPLAY.h"

#define Bell_Relay 11
#define CE_PIN 5
#define CSN_PIN 6

#define CONFIG 0x00
#define EN_AA 0x01
#define EN_RXADDR 0x02
#define SETUP_RETR 0x04
#define RF_CH 0x05
#define RF_SETUP 0x06
#define STATUS_REG 0x07
#define RX_ADDR_P0 0x0A
#define TX_ADDR 0x10
#define RX_PW_P0 0x11
#define FIFO_STATUS 0x17

#define W_REGISTER 0x20
#define R_REGISTER 0x00
#define FLUSH_TX 0xE1
#define FLUSH_RX 0xE2
#define W_TX_PAYLOAD 0xA0
#define R_RX_PAYLOAD 0x61
byte rx_address[5] = { 'A', 'B', 'C', 'D', 'E' };

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

void writeRegisterM(uint8_t reg, const byte* data, uint8_t len) {
  csnLow();
  SPI_transfer(W_REGISTER | (reg & 0x1F));
  for (uint8_t i = 0; i < len; i++) {
    SPI_transfer(data[i]);
  }
  csnHigh();
}

void writeRegister(uint8_t reg, byte value) {
  writeRegisterM(reg, &value, 1);
}

void readPayload(byte* data, uint8_t len) {
  csnLow();
  SPI_transfer(R_RX_PAYLOAD);
  for (uint8_t i = 0; i < len; i++) {
    data[i] = SPI_transfer(0xFF);
  }
  csnHigh();
}

void flushRX() {
  csnLow();
  SPI_transfer(FLUSH_RX);
  csnHigh();
}

byte getStatus() {
  csnLow();
  byte status = SPI_transfer(R_REGISTER | STATUS_REG);
  csnHigh();
  return status;
}


void setup() {
  SPI_begin();
  SPI_setBitOrder(MSBFIRST);
  SPI_setDataMode(SPI_MODE0);
  SPI_setClockDivider(SPI_CLOCK_DIV4);  // Adjust based on your needs
  delay(10);
  pinMode(CE_PIN, OUTPUT);
  pinMode(CSN_PIN, OUTPUT);
  pinMode(Bell_Relay, OUTPUT);
  digitalWrite(Bell_Relay, LOW);
  ceLow();
  csnHigh();
  SPI_begin();
  SPI_setBitOrder(MSBFIRST);
  SPI_setDataMode(SPI_MODE0);
  SPI_setClockDivider(SPI_CLOCK_DIV4);
  delay(100);

  // Setup RX mode
  writeRegister(CONFIG, 0x0F);     // PWR_UP=1, PRIM_RX=1
  writeRegister(EN_AA, 0x00);      // Disable auto-ack
  writeRegister(EN_RXADDR, 0x01);  // Enable data pipe 0
  writeRegister(RF_CH, 76);
  writeRegister(RF_SETUP, 0x06);  // 1Mbps
  writeRegisterM(RX_ADDR_P0, rx_address, 5);
  writeRegister(RX_PW_P0, 4);  // 10-byte payload

  flushRX();
  ceHigh();  // Start listening
  P10_init();
  // Optional: disable P10 output during setup
  digitalWrite(P10_ENABLE, HIGH);
  //  receivedNumber = 0;

  P10_display_number_zone(0, 888);  // Left panel
  P10_display_number_zone(1, 456);  // Middle panel
  P10_display_number_zone(2, 888);  // Right panel
}
void loop() {
  P10_refresh();
}
