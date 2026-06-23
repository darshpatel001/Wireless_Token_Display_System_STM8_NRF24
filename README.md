# Wireless Token Management System Using STM8 & NRF24L01

## Overview

The Wireless Token Management System is an embedded solution designed for queue management applications such as hospitals, banks, government offices, clinics, and customer service centers.

The system consists of a wireless transmitter and receiver communicating through NRF24L01 RF modules. Generated token numbers are transmitted wirelessly and displayed on both a 3-digit 7-segment display and a P10 LED display board.

This project demonstrates wireless embedded communication, real-time display management, and queue automation using STM8 microcontrollers.

---

## Features

* Wireless Token Transmission
* NRF24L01 RF Communication
* Real-Time Token Display
* P10 LED Display Interface
* 3-Digit 7-Segment Display
* Multi-Panel Display Support
* Token Increment Functionality
* Queue Management System
* Embedded Firmware Design
* Low-Cost Implementation

---

## Hardware Used

| Component                 | Purpose                     |
| ------------------------- | --------------------------- |
| STM8 Microcontroller      | Main Controller             |
| NRF24L01 (Transmitter)    | Wireless Token Transmission |
| NRF24L01 (Receiver)       | Wireless Token Reception    |
| P10 LED Display           | Large Token Display         |
| 3-Digit 7-Segment Display | Local Display               |
| Push Buttons              | Token Generation            |
| Power Supply              | System Power                |

---

## Technologies Used

* Embedded C
* STM8 Firmware Development
* NRF24L01 RF Communication
* SPI Communication
* LED Matrix Control
* Seven Segment Multiplexing
* Real-Time Display Management

---

## Project Structure

```text
.
├── Receiver_3_Panel/
│   ├── Receiver_3_Panel.ino
│   ├── P10_SPI_DISPLAY.c
│   └── P10_SPI_DISPLAY.h
│
├── Transmitter_Final/
│   ├── Transmitter_Final.ino
│   ├── 1105TX/
│   ├── 3d7Seg_SPI/
│   └── 7Seg_Keypad/
│
└── README.md
```

---

## System Architecture

```text
           +----------------+
           | Push Buttons   |
           +----------------+
                    |
                    v

           +----------------+
           | STM8 TX Node   |
           +----------------+
                    |
                    |
                NRF24L01
                    |
         Wireless RF Link
                    |
                NRF24L01
                    |
                    v

           +----------------+
           | STM8 RX Node   |
           +----------------+
              |         |
              |         |
              v         v

      +-------------+ +-------------+
      | P10 Display | | 7-Segment   |
      | Token Board | | Display     |
      +-------------+ +-------------+
```

---

## Working Principle

### Token Generation

When the operator presses the token button:

```text
Current Token = 101
```

Next token generated:

```text
Current Token = 102
```

---

### Wireless Transmission

The transmitter sends:

```text
TOKEN = 102
```

using NRF24L01 RF communication.

---

### Receiver Operation

The receiver:

1. Receives token data.
2. Validates received packet.
3. Updates local display.
4. Updates P10 display.

---

### Display Output

#### P10 Display

```text
NOW SERVING

TOKEN : 102
```

#### 7-Segment Display

```text
102
```

---

## Functional Modules

### Transmitter Module

#### generate_token()

* Increments token count.
* Handles user button press.

#### send_token()

* Transmits token through NRF24L01.

---

### NRF24 Communication Module

#### nrf24_init()

* Configures RF module.

#### nrf24_transmit()

* Sends token packets.

#### nrf24_receive()

* Receives token packets.

---

### Receiver Module

#### receive_token()

* Reads incoming token data.

#### update_display()

* Refreshes display output.

---

### P10 Display Module

#### P10_SPI_DISPLAY()

* Controls LED matrix display.

#### display_token()

* Shows current token number.

---

### Seven Segment Module

#### seven_segment_update()

* Updates multiplexed display.

#### display_number()

* Displays token value.

---

## Communication Flow

```text
1. Button Press
2. Generate Token
3. NRF24 Packet Creation
4. Wireless Transmission
5. Receiver Packet Processing
6. Display Update
7. Queue Information Display
```

---

## Build Instructions

### Compile

Using STM8 development environment / Arduino IDE (as applicable).

### Upload

1. Program Transmitter Firmware.
2. Program Receiver Firmware.
3. Connect NRF24 modules.
4. Power both systems.
5. Start token generation.

---

## Screenshots

Add screenshots of:

* Transmitter Hardware
* Receiver Hardware
* NRF24 Connections
* P10 Display Output
* 7-Segment Display Output
* Token Increment Operation

---

## Demo Video

If the video exists in repository root:


## 🎥 Demo Video

[Watch Demo_On_breadboard](./Token_system_bread_board.mp4)
[Watch Demo_On_PCB](./Token_system_bread_PCB_Enclosure.mp4)


---

## Concepts Demonstrated

* Embedded C
* STM8 Development
* NRF24L01 Communication
* SPI Protocol
* Wireless Data Transfer
* LED Matrix Control
* Seven Segment Multiplexing
* Queue Management Systems
* Embedded Firmware Design

---

## Applications

* Hospital Token Systems
* Bank Queue Systems
* Government Service Centers
* Retail Stores
* Customer Waiting Areas
* Industrial Production Tracking

---

## Future Improvements

* Voice Announcements
* Multiple Counters
* Mobile App Monitoring
* Wi-Fi Connectivity
* ESP32 Gateway
* MQTT Dashboard
* Database Logging

---

## Learning Outcomes

Through this project, I gained practical experience in:

* STM8 Firmware Development
* NRF24L01 RF Communication
* SPI Protocol
* P10 Display Control
* Seven Segment Multiplexing
* Wireless Embedded Systems
* Real-Time Display Applications

---

## Author

**Darsh Patel**

Electronics & Communication Engineer

Firmware Engineer | Embedded Systems | IoT Developer

GitHub: https://github.com/darshpatel001

---

## License

This project is licensed under the MIT License.
