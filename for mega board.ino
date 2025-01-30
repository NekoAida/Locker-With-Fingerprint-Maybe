#include <Adafruit_Fingerprint.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ตั้งค่าพอร์ต Serial สำหรับเซ็นเซอร์ลายนิ้วมือ
Adafruit_Fingerprint finger1(&Serial1); // Fingerprint 1 (ต่อกับ TX1 และ RX1)
Adafruit_Fingerprint finger2(&Serial2); // Fingerprint 2 (ต่อกับ TX2 และ RX2)

// ตั้งค่าขารีเลย์สำหรับล็อกเกอร์
const int locker1RelayPin = 8;
const int locker2RelayPin = 9;

// ตั้งค่า LCD Address
LiquidCrystal_I2C lcd(0x3F, 16, 2);

void setup() {
  Serial.begin(9600);

  // เริ่มต้น LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("System Starting...");

  // ตั้งค่าขารีเลย์
  pinMode(locker1RelayPin, OUTPUT);
  pinMode(locker2RelayPin, OUTPUT);
  digitalWrite(locker1RelayPin, HIGH); // ล็อกเกอร์ 1 ปิดล็อก
  digitalWrite(locker2RelayPin, HIGH); // ล็อกเกอร์ 2 ปิดล็อก

  // เริ่มต้นเซ็นเซอร์ลายนิ้วมือ 1
  finger1.begin(57600);
  if (finger1.verifyPassword()) {
    Serial.println("Fingerprint sensor 1 ready.");
    lcd.setCursor(0, 1);
    lcd.print("Sensor 1 Ready");
  } else {
    Serial.println("Fingerprint 1 failed!");
    lcd.setCursor(0, 1);
    lcd.print("Sensor 1 Failed");
    while (1);
  }

  delay(2000);
  lcd.clear();

  // เริ่มต้นเซ็นเซอร์ลายนิ้วมือ 2
  finger2.begin(57600);
  if (finger2.verifyPassword()) {
    Serial.println("Fingerprint sensor 2 ready.");
    lcd.setCursor(0, 1);
    lcd.print("Sensor 2 Ready");
  } else {
    Serial.println("Fingerprint 2 failed!");
    lcd.setCursor(0, 1);
    lcd.print("Sensor 2 Failed");
    while (1);
  }

  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ready to Scan");
}

void loop() {
  lcd.setCursor(0, 1);
  lcd.print("Waiting Finger...");

  // ตรวจสอบเซ็นเซอร์ 1
  if (scanAndUnlock(finger1, locker1RelayPin, "Locker 1")) {
    delay(2000); // รอให้ปลดล็อกก่อน
  }

  // ตรวจสอบเซ็นเซอร์ 2
  if (scanAndUnlock(finger2, locker2RelayPin, "Locker 2")) {
    delay(2000); // รอให้ปลดล็อกก่อน
  }

  delay(1000); // รอ 1 วินาที
}

// ฟังก์ชันตรวจสอบและปลดล็อก
bool scanAndUnlock(Adafruit_Fingerprint &finger, int relayPin, const char *lockerName) {
  int id = getFingerprintID(finger);
  if (id >= 0) {
    Serial.print(lockerName);
    Serial.println(" Unlocked!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(lockerName);
    lcd.setCursor(0, 1);
    lcd.print("Unlocked");
    openLocker(relayPin);
    return true; // คืนค่า true ถ้าปลดล็อกสำเร็จ
  } else {
    Serial.print(lockerName);
    Serial.println(": No match.");
    return false; // คืนค่า false ถ้าหาลายนิ้วมือไม่เจอ
  }
}

// ฟังก์ชันสแกนลายนิ้วมือ
int getFingerprintID(Adafruit_Fingerprint &finger) {
  int p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) return -1;

  return finger.fingerID; // ส่งคืน ID ที่ตรวจพบ
}

// ฟังก์ชันเปิดล็อกเกอร์
void openLocker(int relayPin) {
  digitalWrite(relayPin, LOW); // ปลดล็อก
  delay(5000); // เปิดค้างไว้ 5 วินาที
  digitalWrite(relayPin, HIGH); // ล็อกใหม่
}
