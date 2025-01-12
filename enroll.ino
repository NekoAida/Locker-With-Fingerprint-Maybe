#include <Adafruit_Fingerprint.h>

// Define serial connections for each fingerprint sensor
SoftwareSerial fingerSerial1(2, 3);
SoftwareSerial fingerSerial2(4, 5);

Adafruit_Fingerprint finger[2] = { Adafruit_Fingerprint(&fingerSerial1), Adafruit_Fingerprint(&fingerSerial2) };

void setup() {
  Serial.begin(9600);
  while (!Serial);

  for (uint8_t i = 0; i < 2; i++) {
    finger[i].begin(57600);
    if (finger[i].verifyPassword()) {
      Serial.print("Found fingerprint sensor ");
      Serial.print(i);
      Serial.println("!");
    } else {
      Serial.println("Did not find fingerprint sensor 1 :(");
      while (1);
    }
  }
  Serial.println("Fingerprint sensors are ready!");
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
  Serial.println("Ready to enroll a fingerprint!");
  Serial.println("Please type in the sensor ID to enroll a new fingerprint (1-2): ");
  uint8_t sid = readnumber();
  if (sid != 1 && sid != 2) {
    Serial.println("Sensor ID should be either 1 or 2. Try again.");
    return;
  }
  Serial.println("Please type in the ID to save this fingerprint as (1-127)...");
  uint8_t fid = readnumber();
  if (fid < 1 || fid > 127) {
    Serial.println("Fingerprint ID should be in range 1 - 127. Try again.");
    return;
  }
  if (enrollFingerprint(sid, fid))
    Serial.println("Fingerprint successfully enrolled in both sensors!");
  else
    Serial.println("Enrollment failed. Try again.");
}

bool enrollFingerprint(uint8_t sid, uint8_t fid) {
  Serial.println("Waiting for valid finger to enroll.");
  while (finger.getImage() != FINGERPRINT_OK);
  if (finger.image2Tz(1) != FINGERPRINT_OK) {
    Serial.println("Failed to process image.");
    return false;
  }

  Serial.println("Remove finger");
  delay(1000);
  while (finger.getImage() != FINGERPRINT_NOFINGER);

  Serial.println("Place the same finger again.");
  while (finger.getImage() != FINGERPRINT_OK);
  if (finger.image2Tz(2) != FINGERPRINT_OK) {
    Serial.println("Failed to process second image.");
    return false;
  }

  if (finger.createModel() != FINGERPRINT_OK) {
    Serial.println("Fingerprints did not match.");
    return false;
  }

  if (finger.storeModel(id) != FINGERPRINT_OK) {
    Serial.println("Failed to store fingerprint.");
    return false;
  }

  return true;
}
