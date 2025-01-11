#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ตั้งค่าขา RX และ TX สำหรับเซ็นเซอร์ลายนิ้วมือ
SoftwareSerial fingerSerial1(2, 3); // Fingerprint 1
SoftwareSerial fingerSerial2(4, 5); // Fingerprint 2

Adafruit_Fingerprint finger1(&fingerSerial1);
Adafruit_Fingerprint finger2(&fingerSerial2);

// ตั้งค่าขารีเลย์สำหรับล็อกเกอร์
const int locker1RelayPin = 8;
const int locker2RelayPin = 9;

// ตั้งค่า LCD Address ที่ตรวจพบคือ 0x3F
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
    while (1); // หยุดโปรแกรมหากเซ็นเซอร์ไม่พร้อม
  }

  delay(2000); // รอ 2 วินาที
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
    while (1); // หยุดโปรแกรมหากเซ็นเซอร์ไม่พร้อม
  }

  delay(2000); // รอ 2 วินาที
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ready to Scan");
}

void loop() {
  lcd.setCursor(0, 1);
  lcd.print("Waiting Finger...");

  // ตรวจสอบเซ็นเซอร์ 1
  int id1 = getFingerprintID(finger1);
  if (id1 == 1) {
    Serial.println("Locker 1 Unlocked!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Locker 1 Unlocked");
    openLocker(locker1RelayPin);
    delay(2000);
  } else {
    Serial.println("No match for Finger 1");
  }

  // ตรวจสอบเซ็นเซอร์ 2
  int id2 = getFingerprintID(finger2);
  if (id2 == 2) {
    Serial.println("Locker 2 Unlocked!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Locker 2 Unlocked");
    openLocker(locker2RelayPin);
    delay(2000);
  } else {
    Serial.println("No match for Finger 2");
  }

  delay(1000); // รอ 1 วินาที
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
