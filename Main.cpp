#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <Keypad.h>

// --------- Fingerprint (R307) ----------
#define FP_RX 2
#define FP_TX 3
#define FP_BAUD 57600   // Try 9600 if you face connectivity issues

SoftwareSerial mySerial(FP_RX, FP_TX);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// --------- LEDs & Buzzer ----------
const int GREEN_LED_PIN = 4;
const int RED_LED_PIN   = 5;
const int BUZZER_PIN    = 6;

// --------- LCD I2C ----------
LiquidCrystal_I2C lcd(0x27, 16, 2); // Try 0x3F if no text appears

// --------- Keypad 4x4 ----------
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {7, 8, 9, 10};        // R1..R4
byte colPins[COLS] = {11, 12, 13, A0};     // C1..C4

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// --------- Admin & Menu ---------
const char ADMIN_PIN[] = "1234";   // Admin password
char pinBuffer[5];
byte pinIndex = 0;

char idBuffer[4];   // ID up to 3 digits + '\0'
byte idIndex = 0;
uint8_t currentID = 0;

enum Mode {
  MODE_NORMAL,
  MODE_ADMIN_PIN,
  MODE_ADMIN_MENU,
  MODE_ADMIN_ENROLL_ID,
  MODE_ADMIN_DELETE_ID
};

Mode mode = MODE_NORMAL;

// ---------- Helpers ----------
void lcdMsg(const char* l1, const char* l2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(l1);
  lcd.setCursor(0, 1);
  lcd.print(l2);
}

void resetLedsBuzzer() {
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
}

void beepShort() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(150);
  digitalWrite(BUZZER_PIN, LOW);
}

// ---------- Fingerprint Enroll ----------
uint8_t enrollFingerprint(uint8_t id) {
  int p = -1;

  lcdMsg("Enroll ID:", "Place finger");
  Serial.print(F("Enrolling ID #")); Serial.println(id);

  // step 1
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    if (p == FINGERPRINT_NOFINGER) {
      delay(50);
      continue;
    } else if (p == FINGERPRINT_OK) {
      Serial.println(F("Image 1 OK"));
    } else {
      Serial.println(F("Image 1 error"));
      beepShort();
    }
  }

  p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) {
    Serial.println(F("image2Tz(1) error"));
    return p;
  }

  lcdMsg("Remove finger", "");
  delay(2000);
  while (finger.getImage() != FINGERPRINT_NOFINGER);

  // step 2
  lcdMsg("Same finger", "again");
  p = -1;
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    if (p == FINGERPRINT_NOFINGER) {
      delay(50);
      continue;
    } else if (p == FINGERPRINT_OK) {
      Serial.println(F("Image 2 OK"));
    } else {
      Serial.println(F("Image 2 error"));
      beepShort();
    }
  }

  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) {
    Serial.println(F("image2Tz(2) error"));
    return p;
  }

  p = finger.createModel();
  if (p != FINGERPRINT_OK) {
    Serial.println(F("createModel error"));
    return p;
  }

  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println(F("Enroll success"));
    lcdMsg("Enroll OK", "");
  } else {
    Serial.println(F("storeModel error"));
  }

  delay(1500);
  return p;
}

// ---------- Fingerprint Delete ----------
uint8_t deleteFingerprint(uint8_t id) {
  Serial.print(F("Deleting ID #")); Serial.println(id);
  uint8_t p = finger.deleteModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println(F("Delete OK"));
    lcdMsg("Delete OK", "");
  } else {
    Serial.println(F("Delete error"));
    lcdMsg("Delete error", "");
  }
  delay(1500);
  return p;
}

// ---------- Normal Mode Fingerprint Check ----------
void normalModeFingerprint() {
  uint8_t p = finger.getImage();
  if (p == FINGERPRINT_NOFINGER) {
    return;
  }

  if (p != FINGERPRINT_OK) {
    Serial.println(F("Image error"));
    beepShort();
    resetLedsBuzzer();
    lcdMsg("Error", "Try again");
    delay(800);
    lcdMsg("Place finger", "");
    return;
  }

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) {
    Serial.println(F("image2Tz error"));
    beepShort();
    resetLedsBuzzer();
    lcdMsg("Error", "Try again");
    delay(800);
    lcdMsg("Place finger", "");
    return;
  }

  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    // ✅ Fingerprint match success
    Serial.print(F("Match ID: "));
    Serial.println(finger.fingerID);

    // >> THIS IS THE MESSAGE PYTHON LISTENS FOR <<
    Serial.print("LOGIN:");
    Serial.println(finger.fingerID);
    // ----------------------------------------------------

    digitalWrite(GREEN_LED_PIN, HIGH);
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);

    lcdMsg("Access Granted", "ID: ");
    lcd.setCursor(4, 1);
    lcd.print(finger.fingerID);

    delay(1500);
    resetLedsBuzzer();
    lcdMsg("Place finger", "");
  } else {
    // ❌ No match found
    Serial.println(F("No match"));
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);

    lcdMsg("Access Denied", "Wrong finger");
    delay(1000);

    resetLedsBuzzer();
    lcdMsg("Place finger", "");
  }
}

// ---------- Keypad Helpers ----------
uint8_t parseIDBuffer() {
  if (idIndex == 0) return 0;
  int val = 0;
  for (byte i = 0; i < idIndex; i++) {
    val = val * 10 + (idBuffer[i] - '0');
  }
  if (val < 1 || val > 127) return 0;
  return (uint8_t)val;
}

void enterAdminPinMode() {
  mode = MODE_ADMIN_PIN;
  pinIndex = 0;
  memset(pinBuffer, 0, sizeof(pinBuffer));
  lcdMsg("Enter PIN:", "");
}

void enterAdminMenu() {
  mode = MODE_ADMIN_MENU;
  lcdMsg("1:Enroll 2:Del", "0:Exit");
}

void handleKeypad() {
  char key = keypad.getKey();
  if (!key) return;

  if (mode == MODE_NORMAL) {
    if (key == '#') {      // Enter Admin mode
      enterAdminPinMode();
    }
    return;
  }

  if (mode == MODE_ADMIN_PIN) {
    if (key >= '0' && key <= '9') {
      if (pinIndex < 4) {
        pinBuffer[pinIndex++] = key;
        lcd.setCursor(pinIndex - 1, 1);
        lcd.print("*");
      }
    } else if (key == '*') {
      pinBuffer[pinIndex] = '\0';
      if (strcmp(pinBuffer, ADMIN_PIN) == 0) {
        lcdMsg("PIN OK", "");
        delay(800);
        enterAdminMenu();
      } else {
        lcdMsg("Wrong PIN", "");
        beepShort();
        delay(1000);
        mode = MODE_NORMAL;
        lcdMsg("Place finger", "");
      }
    } else if (key == '#') {
      mode = MODE_NORMAL;
      lcdMsg("Place finger", "");
    }
    return;
  }

  if (mode == MODE_ADMIN_MENU) {
    if (key == '1') {
      mode = MODE_ADMIN_ENROLL_ID;
      idIndex = 0;
      memset(idBuffer, 0, sizeof(idBuffer));
      lcdMsg("Enroll ID:", "Type & *");
    } else if (key == '2') {
      mode = MODE_ADMIN_DELETE_ID;
      idIndex = 0;
      memset(idBuffer, 0, sizeof(idBuffer));
      lcdMsg("Delete ID:", "Type & *");
    } else if (key == '0') {
      mode = MODE_NORMAL;
      lcdMsg("Place finger", "");
    }
    return;
  }

  if (mode == MODE_ADMIN_ENROLL_ID || mode == MODE_ADMIN_DELETE_ID) {
    if (key >= '0' && key <= '9') {
      if (idIndex < 3) {
        idBuffer[idIndex++] = key;
        lcd.setCursor(idIndex - 1, 1);
        lcd.print(key);
      }
    } else if (key == '*') {
      currentID = parseIDBuffer();
      if (currentID == 0) {
        lcdMsg("Bad ID", "1..127");
        beepShort();
        delay(1000);
        enterAdminMenu();
        return;
      }

      if (mode == MODE_ADMIN_ENROLL_ID) {
        enrollFingerprint(currentID);
      } else {
        deleteFingerprint(currentID);
      }
      enterAdminMenu();
    } else if (key == '#') {
      enterAdminMenu();
    }
  }
}

// ---------- setup & loop ----------
void setup() {
  Serial.begin(9600);   // Baud rate for Python/PC communication
  while (!Serial);
  delay(200);

  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  resetLedsBuzzer();

  lcd.init();
  lcd.backlight();
  lcdMsg("Fingerprint", "Starting...");

  mySerial.begin(FP_BAUD);
  delay(100);
  finger.begin(FP_BAUD);

  if (finger.verifyPassword()) {
    Serial.println(F("Sensor OK"));
    lcdMsg("Sensor OK", "Ready...");
    delay(1000);
  } else {
    Serial.println(F("Sensor NOT found"));
    lcdMsg("Sensor error", "Check wiring");
    while (1) { delay(1); }
  }

  mode = MODE_NORMAL;
  lcdMsg("Place finger", "");
}

void loop() {
  handleKeypad();

  if (mode == MODE_NORMAL) {
    normalModeFingerprint();
  }
}