#include <Arduino.h>
#include <TFT_eSPI.h>
#include "TrainDisplay.h"
#include "menu.h"
#include "Buttons.h"

// Create display instances
TFT_eSPI tft;         // TFT display instance
TrainDisplay trainDisplay(tft); // Pass tft to TrainDisplay constructor
Menu mainMenu(&tft);

// Function to read angle from potentiometer
int16_t readAngle(pin_size_t pin);

void setup() {
  Serial1.begin(115200); // Debug only

  pinMode(POTENTIOMETER_PIN, INPUT);
  pinMode(TC_BTN_UP, INPUT_PULLUP);
  pinMode(TC_BTN_DOWN, INPUT_PULLUP);
  pinMode(TC_BTN_LEFT, INPUT_PULLUP);
  pinMode(TC_BTN_RIGHT, INPUT_PULLUP);

  // Initialize TFT display
  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);

  // Setup menus
  Menu* submenu = new Menu(&tft, &mainMenu);
  submenu->addItem("Sub Option 1");
  submenu->addItem("Sub Option 2");

  mainMenu.addItem("Option 1", nullptr, []() {
        Serial1.println("Lambda action works too!");
    });
  mainMenu.addItem("Option 2", submenu);
  mainMenu.addItem("Option 3");
  mainMenu.addItem("Option 4");
  mainMenu.addItem("Option 5");
  mainMenu.addItem("Option 6");
  mainMenu.draw();

  // Initialize train display
  // trainDisplay.begin();

  delay(2000);
}

void loop() {
  // uint16_t angle = readAngle(POTENTIOMETER_PIN);

  // // Plot needle at the measured angle
  // display.plotNeedle(angle, 30);

  if (Menu::activeMenu) {
    Menu::activeMenu->handleInput();
  }
}

int16_t readAngle(pin_size_t pin) {
  // Read the potentiometer and convert to angle
  int16_t angle = analogRead(pin);
  angle = map(angle, 0, 1023, 0, 240);
  Serial1.printf("Angle: %d\n", angle);
  return angle;
}