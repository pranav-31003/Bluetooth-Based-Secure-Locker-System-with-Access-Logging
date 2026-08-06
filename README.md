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

---

# 📸 Hardware Prototype

The hardware prototype demonstrates the complete implementation of the Bluetooth-Based Secure Locker System using the LPC2148 ARM7 microcontroller. It integrates multiple hardware peripherals to provide secure authentication, real-time monitoring, and automated locker control.

### Description

- LPC2148 ARM7 microcontroller acts as the central processing unit of the system.
- HC-05 Bluetooth module enables wireless communication with a mobile device.
- A 4×4 matrix keypad is used to enter the second-level authentication password.
- The 16×2 LCD displays authentication prompts, system status, and user messages.
- AT24C256 EEPROM securely stores passwords and configuration data using the I2C protocol.
- L293D motor driver controls the DC motor for locking and unlocking the locker.
- A buzzer generates alerts for invalid password attempts and security events.
- The hardware design follows a modular approach, making the system reliable and easy to maintain.

<p align="center">
  <img src="Hardware image/WhatsApp Image 2026-08-06 at 9.44.22 PM.jpeg" width="700"/>
</p>

<p align="center">
<b>Figure 1.</b> Complete hardware implementation of the Bluetooth-Based Secure Locker System.
</p>

---

# 🏗 System Block Diagram

The block diagram illustrates the overall hardware architecture and communication between the LPC2148 microcontroller and its peripherals. It shows how different interfaces work together to perform secure authentication, event logging, and locker control.

### Description

- LPC2148 ARM7 microcontroller coordinates all system operations.
- HC-05 Bluetooth module communicates through UART for wireless authentication.
- AT24C256 EEPROM communicates over the I2C bus for secure password storage.
- The 16×2 LCD provides real-time user interaction and system status.
- The keypad allows users to securely enter passwords.
- L293D motor driver controls the DC motor responsible for the locking mechanism.
- RTC provides accurate timestamps for all access events.
- UART transmits event logs to a PC terminal for monitoring and debugging.
- GPIO pins interface with the LCD, keypad, buzzer, and motor driver.
- The modular architecture simplifies debugging, testing, and future upgrades.

<p align="center">
  <img src="Hardware image/Project workflow/block diagram.png" width="900"/>
</p>

<p align="center">
<b>Figure 2.</b> Hardware block diagram showing the interconnection of all peripherals with the LPC2148.
</p>

---

# 🧩 Software Architecture

The software architecture is designed using a layered and modular approach. Each peripheral is managed by an independent driver while the application layer coordinates all system functionalities.

### Description

- Developed using Embedded C in Keil uVision.
- Driver layer manages LCD, Bluetooth, Keypad, UART, EEPROM, RTC, Buzzer, and Motor.
- Middleware layer handles authentication, event logging, and system management.
- Application layer controls the overall locker operation.
- Bluetooth driver receives credentials from the HC-05 module.
- Keypad driver scans and validates keypad input.
- EEPROM driver securely stores and retrieves passwords.
- RTC driver records timestamps for every system event.
- UART driver displays event logs on a serial terminal.
- Modular firmware improves readability, portability, and maintainability.

<p align="center">
  <img src="Hardware image/Project workflow/software architechture.png" width="900"/>
</p>

<p align="center">
<b>Figure 3.</b> Layered software architecture of the embedded firmware.
</p>

---

# 🔄 System Workflow

The workflow diagram illustrates the complete sequence of operations performed by the secure locker system, from power-up initialization to authentication, access control, and event logging.

### Description

- Initializes all peripherals after power-on.
- Displays the welcome screen on the LCD.
- Waits for Bluetooth authentication.
- Verifies the Bluetooth password received from the HC-05 module.
- Requests keypad password after successful Bluetooth authentication.
- Grants locker access only if both authentication stages are successful.
- Unlocks the locker by controlling the DC motor through the L293D driver.
- Stores every event with an RTC timestamp.
- Sends event logs to the UART terminal for monitoring.
- Activates the buzzer for invalid authentication attempts.
- Locks the locker automatically after the operation is complete.
- Returns to standby mode and waits for the next user.

<p align="center">
  <img src="Hardware image/Project workflow/system flow.png" width="900"/>
</p>

<p align="center">
<b>Figure 4.</b> System workflow of the Bluetooth-Based Secure Locker System.
</p>

---

# 🖥 UART Event Logging

The UART terminal provides real-time monitoring of all system activities. It records authentication events, locker operations, and system messages along with RTC timestamps for debugging and security auditing.

### Description

- Displays system startup messages.
- Records successful Bluetooth authentication.
- Records successful keypad authentication.
- Logs failed authentication attempts.
- Displays locker lock and unlock operations.
- Shows administrator activities such as password updates.
- Maintains RTC-based timestamps for every event.
- Helps developers debug firmware during testing.
- Provides a complete access history for security analysis.
- Enables real-time communication between the embedded system and a PC.

<p align="center">
  <img src="Images/uart_log_output.png" width="700"/>
</p>

<p align="center">
<b>Figure 5.</b> UART terminal displaying timestamped access logs generated by the secure locker system.
</p>

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
