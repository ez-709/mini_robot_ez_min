#include <Arduino.h>

const int echoPin = 5;
const int trigPin = 6;


void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
}

void loop() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  float distance_mm = (duration * 0.343) / 2.0;
  Serial.println((int)distance_mm);

  delay(100); 
}