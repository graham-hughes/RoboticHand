#include <Servo.h>

Servo servo1;
Servo servo2;
Servo servo3;

void setup() {
  setupServos();
  setupXBee();
  setupForceSensors();
}

void setupServos() {
  servo1.attach(10);
  servo2.attach(11);
  servo3.attach(12);
}

void setupXBee() {
  Serial.begin(19200);
  Serial.setTimeout(100);
}

void setupForceSensors() {
  pinMode(A0, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
}

void loop() {
  updateServos();
  sendForceValues();
}

void updateServos() {
  // Read the flex values
  String first  = Serial.readStringUntil(',');
  Serial.read(); //next character is comma, so skip it using this
  String second = Serial.readStringUntil(',');
  Serial.read();
  String third  = Serial.readStringUntil('\n');

  double a = first.toInt();
  double b = second.toInt();
  double c = third.toInt();

  // Write the flex values to the servos (pre-reverse on glove)
  servo1.write(a);
  servo2.write(b);
  servo3.write(c);
}

void sendForceValues() {
  // Read and exponentiate the force sensors
  byte a0 = pow(2, analogRead(A0) / 1024.0 * 6);
  byte a2 = pow(2, analogRead(A2) / 1024.0 * 6);
  byte a3 = pow(2, analogRead(A3) / 1024.0 * 6);.

  Serial.println(String(a0) + "," + String(a2) + "," + String(a3));
}

