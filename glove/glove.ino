#include <Servo.h>
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>

Servo servo1;
Servo servo2;
Servo servo3;

SoftwareSerial XBee(2, 3); // RX, TX

Adafruit_NeoPixel leds = Adafruit_NeoPixel(3, 5, NEO_GRB + NEO_KHZ800);

int a0;
int a1;
int a2;

void setup() {
  setupSerialPorts();
  setupVibrationMotors();
  setupLEDs();
  setupFlexSensors();
}

void setupSerialPorts() {
  Serial.begin(19200);
  XBee.begin(19200);

  // Set the timeout so we don't wait too long for a force sensor packet
  XBee.setTimeout(50);
}

void setupVibrationMotors() {
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
}

void setupLEDs() {
  leds.begin();
  leds.show();
}

void setupFlexSensors() {
  pinMode(A0, INPUT); // 80 - 215
  pinMode(A1, INPUT); // 130 - (258)
  pinMode(A2, INPUT); // 85 - (285)
}

void loop() {
    updateVibrationMotors();
    updateFlexWeightedAverage();

    refreshLEDs();
    
    constrainAndSendFlexValues();
}

void updateVibrationMotors() {
    // Read the force values
    String first  = XBee.readStringUntil(',');
    XBee.read(); //next character is comma, so skip it using this
    String second = XBee.readStringUntil(',');
    XBee.read();
    String third  = XBee.readStringUntil('\n');

    double a = first.toInt();
    double b = second.toInt();
    double c = third.toInt();

    if (a > 1) digitalWrite(9, HIGH);
    else digitalWrite(9, LOW);
    
    if (b > 1) digitalWrite(10, HIGH);
    else digitalWrite(10, LOW);

    if (c > 1) digitalWrite(11, HIGH);
    else digitalWrite(11, LOW);
}

void updateFlexWeightedAverage() {
    a1 = map(analogRead(A0),225,120,0,180) * .1 + a1 * .9;
    a0 = map(analogRead(A1),265,150,0,180) * .1 + a0 * .9;
    a2 = map(analogRead(A2),210,120,0,180) * .1 + a2 * .9;
}

void refreshLEDs() {
    leds.setPixelColor(1, Wheel(a1/5 + 20));
    leds.setPixelColor(2, Wheel(a0/5 + 20));
    leds.setPixelColor(0, Wheel(a2/5 + 20));
    leds.show();
}

void constrainAndSendFlexValues() {
    a0 = constrain(a0, 20, 170);
    a1 = constrain(a1, 20, 170);
    a2 = constrain(a2, 20, 170);

    // Invert a2 because that motor is reversed
    XBee.println(String(a0) + "," + String(a1) + "," + String(180 - a2));
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) return leds.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  
  if (WheelPos < 170) {
    WheelPos -= 85;
    return leds.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  
  WheelPos -= 170;
  return leds.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
