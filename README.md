## 📁 Repository Structure and File Naming

To make your repository clean and easy to navigate, use the following structure and file names:

```
Smart-Biometric-Attendance-System/
├── Arduino-Code/
│   └── Biometric_Attendance_Arduino.ino   # Arduino C++ Code
├── Python-Automation/
│   └── biometric_automation.py            # Python Script
├── Docs/
│   └── biometric_presentation.pdf         # (Your uploaded presentation file)
├── logs/
│   └── attendance_log.csv                 # Attendance records (ignored by Git usually)
├── .gitignore                             # Recommended to exclude logs/
└── README.md                              # Main project documentation
```

-----

## ✨ Project Overview

This is an integrated biometric attendance solution combining hardware reliability with software automation. It utilizes an **Arduino** microcontroller and an **R307 Fingerprint Sensor** for authentication, paired with a **Python** script that sends real-time attendance notifications via **WhatsApp** and maintains a digital log.

This hybrid system aims to enhance security, reduce human error, and provide secure, automated logging, ideal for institutions monitoring attendance. (As noted in `biometric presentation.pdf`, Page 3)

### Key Features

  * **Biometric Authentication:** Secure user identification using the R307 Fingerprint Sensor.
  * **Real-time Notifications:** Automated WhatsApp messages sent to an administrator upon successful check-in.
  * **Admin Interface:** A PIN-protected (default: `1234`) 4x4 Keypad interface for enrolling and deleting user IDs.
  * **Digital Logging:** All check-in events are timestamped and saved in a local `attendance_log.csv` file.

## ⚙️ Hardware Components

| Component | Description | Connection (Arduino Uno) |
| :--- | :--- | :--- |
| **Arduino Uno** | Main Microcontroller | - |
| **R307 Fingerprint Sensor** | Authentication | D2 (RX), D3 (TX) via `SoftwareSerial` |
| **LCD 16x2 I2C** | User Display | I2C (A4/SDA, A5/SCL) |
| **4x4 Keypad** | Input Interface | D7-D13, A0 |
| **Green LED** | Access Granted Indicator | D4 |
| **Red LED / Buzzer** | Access Denied Indicator | D5 / D6 |

## 📸 Physical Prototype

Below is a photograph of the final, assembled attendance system, showing the Arduino, Keypad, LCD, and Fingerprint Sensor connected.
![Completed Biometric System Prototype](assets/prototype_image.jpg)
<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/21b6e27d-2b04-48fa-83ed-3c73474dfe29" />


## 🚀 Setup and Installation Guide

### 1\. Arduino Setup (Firmware)

1.  **Libraries:** Install the following libraries in your Arduino IDE:
      * `Adafruit Fingerprint Sensor Library`
      * `LiquidCrystal I2C`
      * `Keypad`
2.  **Upload Code:** Open `Arduino-Code/Biometric_Attendance_Arduino.ino` and upload it to your Arduino board.

### 2\. Python Setup (Automation Script)

The Python script runs on the PC connected via USB.

1.  **Install Dependencies:**
    ```bash
    pip install pyserial pywhatkit pyautogui
    ```
2.  **Configure Script:** Open `Python-Automation/biometric_automation.py` and modify the following constants:
      * `ARDUINO_PORT = 'COMX'`: **Change this** to your Arduino's serial port (e.g., `'COM3'` or `'/dev/ttyACM0'`).
      * `WHATSAPP_NUMBER = "+YYXXXXXXXXXX"`: Set the administrator's phone number (international format).
3.  **Run the Script:**
      * Ensure you are logged into **WhatsApp Web** in your default browser.
      * Run the script from your terminal:
        ```bash
        python Python-Automation/biometric_automation.py
        ```

## 🔄 Integration Protocol (Serial Communication)

The core functionality relies on the reliable communication between the two systems (as noted in `biometric presentation.pdf`, Page 9):

1.  **Success Event:** Upon a successful fingerprint match, the Arduino sends a coded string via the USB Serial Port.
2.  **Protocol Format:**
    ```
    LOGIN:ID
    ```
    (E.g., `LOGIN:7` if user ID 7 checks in).
3.  **Python Action:** The Python script monitors the port, parses the line, and triggers the automated WhatsApp messaging sequence using the extracted `ID`.

## 📝 Admin Operations (Keypad)

The system is managed via the 4x4 keypad:

| Action | Key Press | Description |
| :--- | :--- | :--- |
| **Enter Admin Mode** | Press **`#`** | Enters the PIN authentication screen. |
| **Authenticate** | Type `1234` then press **`*`** | Enters the Admin Menu (Default PIN is `1234`). |
| **Enroll Finger** | Press **`1`** | Prompts for a User ID, then guides the two-scan enrollment process. |
| **Delete Finger** | Press **`2`** | Prompts for a User ID to be deleted from memory. |
| **Exit Admin Mode** | Press **`0`** | Returns the system to `MODE_NORMAL` (listening for fingerprints). |

## 💡 Future Enhancements

Potential areas for future development (as suggested in `biometric presentation.pdf`, Page 10):

  * Integrate a database (e.g., SQLite or MySQL) for more robust data storage.
  * Develop automated daily/monthly attendance report generation.
  * Add door-lock automation using a relay module.

## ⚖️ License

This project is licensed under the **MIT License**.
