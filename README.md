# 🔐 Bluetooth-Based Secure Locker System with Access Logging

<div align="center">

![Platform](https://img.shields.io/badge/Platform-LPC2148-blue)
![Language](https://img.shields.io/badge/Language-Embedded%20C-green)
![IDE](https://img.shields.io/badge/IDE-Keil%20uVision-red)
![Communication](https://img.shields.io/badge/UART-I2C-Bluetooth)
![License](https://img.shields.io/badge/Project-Embedded%20Systems-orange)

**A Secure Embedded Locker System using LPC2148 ARM7 Microcontroller with Two-Level Authentication, EEPROM Password Storage, RTC-Based Event Logging and Bluetooth Communication.**

</div>

---

# 📖 Project Overview

The **Bluetooth-Based Secure Locker System** is a real-time embedded security application developed using the **LPC2148 (ARM7TDMI-S)** microcontroller.

The system protects a locker using **two-factor authentication**.

1. **Bluetooth Authentication (HC-05 Module)**
2. **Keypad Password Authentication (4×4 Matrix Keypad)**

Only when both authentication levels are successful will the locker unlock using a **DC Motor controlled through the L293D Motor Driver**.

All important events such as system startup, successful login, failed authentication attempts, lock/unlock operations, tamper detection, and administrator actions are stored with **RTC timestamps** and transmitted to a PC through **UART** for real-time monitoring.

Passwords and configuration settings are securely stored inside an **AT24C256 EEPROM** using the **I2C protocol**, allowing data retention even after power loss.

---

# ✨ Features

- 🔒 Two-Level Authentication
  - Bluetooth Password
  - Keypad Password

- 📱 HC-05 Bluetooth Communication

- 🔑 4×4 Matrix Keypad Password Entry

- 💾 Secure Password Storage in AT24C256 EEPROM

- 🕒 RTC Timestamp-Based Event Logging

- 📟 16×2 LCD User Interface

- 🔊 Buzzer Alarm for Invalid Access

- 🚪 DC Motor Based Locker Lock/Unlock

- 📤 UART Terminal Logging to PC

- 👨‍💼 Administrator Mode

- 🔐 Failed Attempt Protection

- 🚨 Tamper Detection Support

- ⚡ Modular Embedded C Firmware

---

# 🏗 System Architecture

```
                    +----------------+
                    | Mobile Phone   |
                    | Bluetooth App  |
                    +--------+-------+
                             |
                          Bluetooth
                             |
                        HC-05 Module
                             |
                  UART1 (TXD1 / RXD1)
                             |
                    +----------------+
                    |    LPC2148     |
                    | ARM7 MCU       |
                    +----------------+
       |        |          |         |         |
      LCD    EEPROM     Keypad    L293D     Buzzer
     GPIO      I2C        GPIO      GPIO      GPIO
       |                                 |
       |                             DC Motor
       |                                 |
       +----------------------------> Locker
```

---

# 🔄 System Workflow

1. Power ON
2. Initialize peripherals
3. Display welcome message
4. Wait for Bluetooth password
5. Verify Bluetooth authentication
6. Request keypad password
7. Verify keypad authentication
8. Unlock locker
9. Log event with RTC timestamp
10. Lock locker automatically
11. Return to standby mode

If authentication fails:

- Increment failed attempt counter
- Activate buzzer
- Log failure
- Lock system after maximum attempts

---

# 🧩 Hardware Components

| Component | Description |
|-----------|-------------|
| LPC2148 | ARM7TDMI-S Microcontroller |
| HC-05 | Bluetooth Module |
| 16×2 LCD | User Interface |
| 4×4 Matrix Keypad | Password Input |
| L293D | Motor Driver |
| DC Motor | Locker Lock/Unlock |
| AT24C256 | External EEPROM |
| RTC | Internal Real-Time Clock |
| Buzzer | Alarm |
| Admin Button | Configuration Mode |
| Tamper Switch | Intrusion Detection |
| USB-UART | PC Logging |

---

# 🔌 Pin Connections (LPC2148)

## LCD (16×2)

| LCD Pin | LPC2148 |
|----------|----------|
| RS | P0.16 |
| EN | P0.17 |
| D4 | P0.18 |
| D5 | P0.19 |
| D6 | P0.20 |
| D7 | P0.21 |

---

## HC-05 Bluetooth

| HC-05 | LPC2148 |
|---------|----------|
| TXD | P0.9 (RXD1) |
| RXD | P0.8 (TXD1) |
| VCC | 5V |
| GND | GND |

---

## AT24C256 EEPROM

| EEPROM | LPC2148 |
|----------|----------|
| SDA | P0.3 |
| SCL | P0.2 |

---

## 4×4 Matrix Keypad

### Rows

| Row | LPC2148 |
|------|----------|
| R1 | P1.16 |
| R2 | P1.17 |
| R3 | P1.18 |
| R4 | P1.19 |

### Columns

| Column | LPC2148 |
|----------|----------|
| C1 | P1.20 |
| C2 | P1.21 |
| C3 | P1.22 |
| C4 | P1.23 |

---

## L293D Motor Driver

| Signal | LPC2148 |
|----------|----------|
| IN1 | P1.24 |
| IN2 | P1.25 |

---

## Other Connections

| Device | LPC2148 |
|----------|----------|
| Buzzer | P1.26 |
| Tamper Switch | P0.4 |
| Admin Button (EINT2) | P0.7 |
| UART0 TX | P0.0 |
| UART0 RX | P0.1 |

---

# 📁 Project Structure

```
SecureLocker_Commented/
│
├── main.c
├── lcd.c / lcd.h
├── keypad.c / keypad.h
├── bluetooth.c / bluetooth.h
├── uart.c / uart.h
├── rtc.c / rtc.h
├── eeprom.c / eeprom.h
├── motor.c / motor.h
├── buzzer.c / buzzer.h
├── menu.c / menu.h
├── defines.h
├── Startup.s
├── majorproject12.uvproj
└── README.md
```

---

# 📂 Software Modules

### LCD Driver
Displays menus, authentication messages and system status.

### Bluetooth Driver
Receives Bluetooth password from HC-05.

### Keypad Driver
Scans the 4×4 matrix keypad for user input.

### EEPROM Driver
Stores passwords and system configuration using I2C.

### RTC Driver
Maintains date and time for event logging.

### UART Driver
Transfers logs to PC terminal.

### Motor Driver
Controls locker lock/unlock operation.

### Buzzer Driver
Generates alarm during invalid authentication.

### Menu System
Provides administrator configuration menu.

---

# 🔒 Security Features

- Two-Level Authentication
- EEPROM Password Storage
- RTC Timestamp Logging
- Failed Attempt Counter
- Automatic Lockout
- Tamper Detection
- Administrator Mode
- Secure Password Update
- Real-Time UART Monitoring

---

# 🖥 Development Environment

| Software | Version |
|----------|---------|
| Keil uVision | IDE |
| Embedded C | Programming Language |
| Flash Magic | Programming Tool |
| Proteus | Circuit Simulation |
| LPC2148 | Target MCU |

---

# 📸 Project Images

## Hardware Prototype
<p align="center">
  <img src="Hardware image/WhatsApp Image 2026-08-06 at 9.44.22 PM.jpeg" width="600">
</p>
## System Block Diagram

```
images/block_diagram.png
```

## Software Architecture

```
images/software_architecture.png
```

## System Workflow

```
images/system_workflow.png
```

---

# 📈 Future Improvements

- Fingerprint Authentication
- RFID Authentication
- Wi-Fi Connectivity
- Cloud Event Logging
- Mobile Application
- OTP-Based Authentication
- Camera-Based Intrusion Detection
- IoT Dashboard

---

# 👨‍💻 Author

**Pranav Chopade**

Embedded Systems Engineer

### Technologies

- Embedded C
- LPC2148 ARM7
- UART
- I2C
- GPIO
- RTC
- EEPROM
- Bluetooth
- Motor Control
- Real-Time Embedded Systems

---

# ⭐ If you found this project useful, consider giving it a Star!
