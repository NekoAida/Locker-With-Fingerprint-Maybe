#include <Adafruit_Fingerprint.h>

// Define serial connections for each fingerprint sensor
SoftwareSerial fingerSerial1(2, 3); // Sensor 1
SoftwareSerial fingerSerial2(4, 5); // Sensor 2

Adafruit_Fingerprint finger1 = Adafruit_Fingerprint(&fingerSerial1);
Adafruit_Fingerprint finger2 = Adafruit_Fingerprint(&fingerSerial2);

uint8_t id;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Initialize sensor 1
  finger1.begin(57600);
  if (finger1.verifyPassword()) {
    Serial.println("Found fingerprint sensor 1!");
  } else {
    Serial.println("Did not find fingerprint sensor 1 :(");
    while (1);
  }

  // Initialize sensor 2
  finger2.begin(57600);
  if (finger2.verifyPassword()) {
    Serial.println("Found fingerprint sensor 2!");
  } else {
    Serial.println("Did not find fingerprint sensor 2 :(");
    while (1);
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
  Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
  id = readnumber();
  if (id == 0) {
    Serial.println("ID #0 not allowed, try again!");
    return;
  }

  Serial.print("Enrolling ID #");
  Serial.println(id);

  Serial.println("Using Sensor 1 for enrollment...");
  if (!enrollFingerprint(finger1)) {
    Serial.println("Failed to enroll on Sensor 1.");
    return;
  }

  Serial.println("Using Sensor 2 for enrollment...");
  if (!enrollFingerprint(finger2)) {
    Serial.println("Failed to enroll on Sensor 2.");
    return;
  }

  Serial.println("Fingerprint successfully enrolled in both sensors!");
}

bool enrollFingerprint(Adafruit_Fingerprint &finger) {
  int p = -1;

  Serial.print("Waiting for valid finger to enroll as #");
  Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    if (p == FINGERPRINT_NOFINGER) continue;
    if (p == FINGERPRINT_OK) {
      Serial.println("Image taken");
      break;
    } else {
      Serial.println("Error capturing image");
      return false;
    }
  }

  p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) {
    Serial.println("Failed to convert image.");
    return false;
  }

  Serial.println("Remove finger");
  delay(2000);
  while (finger.getImage() != FINGERPRINT_NOFINGER);

  Serial.println("Place same finger again");
  while ((p = finger.getImage()) != FINGERPRINT_OK);

  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) {
    Serial.println("Failed to process second image.");
    return false;
  }

  p = finger.createModel();
  if (p != FINGERPRINT_OK) {
    Serial.println("Fingerprints did not match.");
    return false;
  }

  p = finger.storeModel(id);
  if (p != FINGERPRINT_OK) {
    Serial.println("Failed to store fingerprint.");
    return false;
  }

  return true;
}
