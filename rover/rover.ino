#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Параметры глаз
const int BASE_W = 20;
const int BASE_H = 12;
int eyeWidth  = BASE_W;
int eyeHeight = BASE_H;
int eyeY = 26;
int leftEyeX  = 34;
int rightEyeX = 74;

// Ультразвуковой датчик
const int trigPin = 3;
const int echoPin = 2;
const int SURPRISE_DISTANCE_MM = 80;

// Пины драйвера TB6612FNG
const int PWMA = 5;
const int AIN2 = 6;
const int AIN1 = 7;
const int BIN1 = 8;
const int BIN2 = 9;
const int PWMB = 10;

const int NORMAL_SPEED = 100;

// Флаг: моторы запущены?
bool motorsRunning = false;

// --- Функции глаз (без изменений) ---
void drawEyes(int w, int h, int offsetX = 0) {
  display.clearDisplay();
  display.fillRect(leftEyeX + offsetX, eyeY, w, h, SSD1306_WHITE);
  display.fillRect(rightEyeX + offsetX, eyeY, w, h, SSD1306_WHITE);
  display.display();
}

void animateSize(int fromW, int fromH, int toW, int toH, int steps, int delayMs) {
  for (int i = 0; i <= steps; i++) {
    int w = map(i, 0, steps, fromW, toW);
    int h = map(i, 0, steps, fromH, toH);
    drawEyes(w, h, 0);
    delay(delayMs);
  }
}

void animateLook(int from, int to, int steps, int delayMs) {
  for (int i = 0; i <= steps; i++) {
    int offset = map(i, 0, steps, from, to);
    drawEyes(eyeWidth, eyeHeight, offset);
    delay(delayMs);
  }
}

void blink() {
  animateSize(eyeWidth, eyeHeight, eyeWidth, 2, 4, 15);
  animateSize(eyeWidth, 2, eyeWidth, eyeHeight, 4, 15);
}

void surprise() {
  animateSize(eyeWidth, eyeHeight, 30, 18, 6, 15);
  eyeWidth = 30;
  eyeHeight = 18;

  animateLook(0, 20, 6, 15);
  animateLook(10, -20, 8, 15);
  animateLook(-10, 0, 6, 15);

  animateSize(eyeWidth, eyeHeight, BASE_W, BASE_H, 6, 15);
  eyeWidth  = BASE_W;
  eyeHeight = BASE_H;
  drawEyes(eyeWidth, eyeHeight, 0);
}

int getDistanceMM() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) return 9999;
  return (int)((duration * 0.343) / 2.0);
}


void motorForward(int speed) {
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, speed);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMB, speed);
}

void stopMotors() {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);
}

void avoidObstacle() {
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, NORMAL_SPEED);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMB, 0);
  delay(1000);
  stopMotors();
  delay(50);
}

void setup() {
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {

    Serial.println("OLED error!");
  }

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  stopMotors();

  unsigned long startTime = millis();
  while (millis() - startTime < 5000) {
    drawEyes(eyeWidth, eyeHeight, 0);
    delay(600);
    blink();
    delay(400);
  }

  motorsRunning = true;
  motorForward(NORMAL_SPEED);
  Serial.println("Моторы запущены!");
}

void loop() {
  if (!motorsRunning) {
    return; 
  }

  int distanceMM = getDistanceMM();
  Serial.print("Расстояние: ");
  Serial.print(distanceMM);
  Serial.println(" мм");

  if (distanceMM > 0 && distanceMM < SURPRISE_DISTANCE_MM) {
    stopMotors();
    surprise();
    avoidObstacle();
    motorForward(NORMAL_SPEED);
    delay(600);
  } else {
    drawEyes(eyeWidth, eyeHeight, 0);
    delay(800);
    blink();
    delay(600);
    motorForward(NORMAL_SPEED);
  }
}