#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino_Eye.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Глаза: (x, y, радиус_глаза, радиус_зрачка)
Eye leftEye(40, 32, 20, 8);
Eye rightEye(88, 32, 20, 8);

void setup() {
    Serial.begin(115200);
    
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("OLED not found!");
        for (;;); // зависнуть
    }

    display.clearDisplay();
    display.display();
    display.setTextColor(SSD1306_WHITE);
}

void loop() {
    display.clearDisplay();

    leftEye.draw(&display);
    rightEye.draw(&display);

    display.display();

    delay(50);

    // Моргаем раз в 3 секунды
    static uint32_t lastBlink = 0;
    if (millis() - lastBlink > 3000) {
        leftEye.blink();
        rightEye.blink();
        lastBlink = millis();
    }
}