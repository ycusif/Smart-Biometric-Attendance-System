# 🔒 Fingerprint Access and Attendance System (Arduino + Python)

## 🌟 Overview

This project implements a fully functional attendance and access control system using an Arduino microcontroller, an optical fingerprint sensor (like the R307/R503), a keypad, and an I²C LCD.

A critical feature of this system is its ability to communicate a successful authentication event to a connected PC via the serial port. The companion Python script, running on the PC, listens for this event and automatically triggers two actions:

1.  Logs the user's entry time into a local CSV file (`attendance_log.csv`).
2.  Sends an immediate notification (including the user ID and timestamp) via **WhatsApp** using the `pywhatkit` library.

## 🛠️ Hardware Requirements

| Component | Description |
| :--- | :--- |
| **Microcontroller** | Arduino Uno, Nano, or Mega. |
| **Fingerprint Sensor** | R307, R503, or similar optical module. |
| **Display** | 16x2 I²C LCD Display. |
| **Input** | 4x4 Matrix Keypad. |
| **Feedback** | 2x LEDs (Red/Green) and a Buzzer. |
| **Wiring** | Wires, Breadboard, and power supply. |
| **PC Connection** | USB cable for communication with the Python script. |

## 🔌 Wiring Diagram

Connect your components as specified in the Arduino code. 

### Arduino Pinout Summary:

| Component | Arduino Pin(s) | Function |
| :--- | :--- | :--- |
| **Fingerprint Sensor** | Pin 2 (RX) & Pin 3 (TX) | Software Serial Communication |
| **Green LED** | Pin 4 (OUTPUT) | Access Granted |
| **Red LED** | Pin 5 (OUTPUT) | Access Denied |
| **Buzzer** | Pin 6 (OUTPUT) | Audio Feedback |
| **Keypad Rows** | Pins 7, 8, 9, 10 | Input Scan |
| **Keypad Columns** | Pins 11, 12, 13, A0 | Input Scan |
| **LCD I²C** | SDA (A4) & SCL (A5) | Display Communication |

## 💻 Software & Libraries

### Arduino (C++)

* **IDE:** Arduino IDE
* **Libraries:**
    * `Wire.h` (Standard I²C library)
    * `LiquidCrystal_I2C` (For 16x2 I²C LCD)
    * `Adafruit_Fingerprint` (For R30x/R50x sensors)
    * `SoftwareSerial` (Standard Arduino Library)
    * `Keypad` (For 4x4 Keypad)

### PC (Python)

* **Version:** Python 3.x
* **Libraries (Install via pip):**
    * `pyserial` (`pip install pyserial`): To communicate with the Arduino's serial port.
    * `pywhatkit` (`pip install pywhatkit`): To automate sending WhatsApp messages.

## 🚀 Getting Started

### Step 1: Arduino Setup and Upload

1.  Install all required Arduino libraries listed above using the Library Manager.
2.  Open the C++ code file (`your_arduino_code.ino`) in the Arduino IDE.
3.  **Verify and Update Settings:** Ensure the `FP_BAUD` (Line 10) matches your sensor's setting, and if necessary, adjust the LCD I²C address (Line 21: `0x27` or `0x3F`).
4.  Compile and upload the code to your Arduino board.

### Step 2: Admin Enrollment

1.  On the running device, press the **`#`** key to enter Admin Mode.
2.  Enter the default PIN: **`1234`**, then press **`*`**.
3.  Select **`1`** (Enroll).
4.  Enter a new **ID** (e.g., `1`, `12`, etc. Max 127) and press **`*`**.
5.  Follow the LCD prompts to place, remove, and replace the finger for two successful scans.

### Step 3: Python Configuration and Run

1.  Open the Python script (`your_python_code.py`).
2.  **Configuration:** Update the following global variables:
    * `ARDUINO_PORT`: Change `'COM6'` to your Arduino's actual serial port (e.g., `'COM3'`, `/dev/ttyACM0`).
    * `WHATSAPP_NUMBER`: Enter the target WhatsApp number in **international format** (e.g., `+15551234567`).
3.  Ensure you have an active internet connection and are logged into **WhatsApp Web** in your default browser.
4.  Run the Python script from your terminal:
    ```bash
    python your_python_code.py
    ```

### Step 4: Testing and Validation

1.  The Arduino should display **"Place finger"**.
2.  The Python script should display **"Listening for LOGIN: events from Arduino..."**.
3.  Place one of the enrolled fingers on the sensor.
4.  **Success:**
    * Arduino displays **"Access Granted"** (Green LED on).
    * Python console logs **"FROM ARDUINO: LOGIN:ID"** and **"Sending WhatsApp message..."**.
    * A browser window opens briefly to send the notification.
    * The user's entry is added to `attendance_log.csv`.

## ⚙️ Core Communication Protocol

The entire system relies on a simple, consistent serial protocol:

| Sender | Event | Serial Output | Python Action |
| :--- | :--- | :--- | :--- |
| **Arduino** | Successful Fingerprint Match (ID 42) | `LOGIN:42` | Sends WhatsApp message and logs attendance. |

## 📂 Project Structure

project/
│
├── arduino_code.ino        // Arduino firmware for fingerprint, LCD, buzzer, serial output
├── python_code.py          // Python listener for serial, Google Sheets logging, WhatsApp automation
├── attendance_log.csv      // Auto-generated attendance log
└── README.md               // Project documentation