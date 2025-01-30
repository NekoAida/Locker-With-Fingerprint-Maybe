#include <Adafruit_Fingerprint.h>

// ใช้พอร์ต Serial1 และ Serial2 สำหรับเซ็นเซอร์ลายนิ้วมือ
Adafruit_Fingerprint finger1 = Adafruit_Fingerprint(&Serial1);
Adafruit_Fingerprint finger2 = Adafruit_Fingerprint(&Serial2);

uint8_t id;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // กำหนดค่า baud rate สำหรับเซ็นเซอร์ลายนิ้วมือ
  Serial1.begin(57600); // Sensor 1
  Serial2.begin(57600); // Sensor 2

  // ตรวจสอบเซ็นเซอร์ 1
  finger1.begin(57600);
  if (finger1.verifyPassword()) {
    Serial.println("พบเซ็นเซอร์ลายนิ้วมือ 1!");
  } else {
    Serial.println("ไม่พบเซ็นเซอร์ลายนิ้วมือ 1 :(");
    while (1);
  }

  // ตรวจสอบเซ็นเซอร์ 2
  finger2.begin(57600);
  if (finger2.verifyPassword()) {
    Serial.println("พบเซ็นเซอร์ลายนิ้วมือ 2!");
  } else {
    Serial.println("ไม่พบเซ็นเซอร์ลายนิ้วมือ 2 :(");
    while (1);
  }

  Serial.println("เซ็นเซอร์ลายนิ้วมือพร้อมใช้งาน!");
}

uint8_t readnumber() {
  uint8_t num = 0;
  while (num == 0) {
    while (!Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

void loop() {
  Serial.println("พร้อมลงทะเบียนลายนิ้วมือ!");
  Serial.println("พิมพ์หมายเลข ID (1 ถึง 127) สำหรับบันทึกลายนิ้วมือ...");
  id = readnumber();
  if (id == 0) {
    Serial.println("ID #0 ไม่อนุญาต ลองใหม่!");
    return;
  }

  Serial.print("กำลังลงทะเบียน ID #");
  Serial.println(id);

  Serial.println("ใช้เซ็นเซอร์ 1 สำหรับลงทะเบียน...");
  if (!enrollFingerprint(finger1)) {
    Serial.println("ลงทะเบียนบนเซ็นเซอร์ 1 ไม่สำเร็จ.");
    return;
  }

  Serial.println("ใช้เซ็นเซอร์ 2 สำหรับลงทะเบียน...");
  if (!enrollFingerprint(finger2)) {
    Serial.println("ลงทะเบียนบนเซ็นเซอร์ 2 ไม่สำเร็จ.");
    return;
  }

  Serial.println("ลงทะเบียนลายนิ้วมือสำเร็จบนทั้งสองเซ็นเซอร์!");
}

bool enrollFingerprint(Adafruit_Fingerprint &finger) {
  int p = -1;

  Serial.print("รอให้วางนิ้วเพื่อลงทะเบียน ID #");
  Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    if (p == FINGERPRINT_NOFINGER) continue;
    if (p == FINGERPRINT_OK) {
      Serial.println("จับภาพลายนิ้วมือสำเร็จ");
      break;
    } else {
      Serial.println("เกิดข้อผิดพลาดในการจับภาพ");
      return false;
    }
  }

  p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) {
    Serial.println("ไม่สามารถแปลงภาพเป็นข้อมูล.");
    return false;
  }

  Serial.println("กรุณาถอนนิ้วออก");
  delay(2000);
  while (finger.getImage() != FINGERPRINT_NOFINGER);

  Serial.println("กรุณาวางนิ้วเดิมอีกครั้ง");
  while ((p = finger.getImage()) != FINGERPRINT_OK);

  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) {
    Serial.println("ไม่สามารถประมวลผลภาพที่สอง.");
    return false;
  }

  p = finger.createModel();
  if (p != FINGERPRINT_OK) {
    Serial.println("ลายนิ้วมือไม่ตรงกัน.");
    return false;
  }

  p = finger.storeModel(id);
  if (p != FINGERPRINT_OK) {
    Serial.println("ไม่สามารถบันทึกลายนิ้วมือ.");
    return false;
  }

  return true;
}
