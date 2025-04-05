#include <Arduino.h>
#include <TFT_eSPI.h>
#include <memory>
#include "TrainDisplay.h"
#include "MenuPage.h"
#include "PageManager.h"
#include "Config.h"


// Create display instances
TFT_eSPI tft;         // TFT display instance
TrainDisplay trainDisplay(tft); // Pass tft to TrainDisplay constructor


// Function to read angle from potentiometer
int16_t readAngle(pin_size_t pin);

void setup() {
  Serial1.begin(115200); // Debug only

  pinMode(POTENTIOMETER_PIN, INPUT);
  pinMode(TC_BTN_UP, INPUT_PULLUP);
  pinMode(TC_BTN_DOWN, INPUT_PULLUP);
  pinMode(TC_BTN_LEFT, INPUT_PULLUP);
  pinMode(TC_BTN_RIGHT, INPUT_PULLUP);
  pinMode(TC_BTN_OK, INPUT_PULLUP);

  // Initialize TFT display
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  auto mainMenu = std::make_unique<MenuPage>(&tft);
  auto wifiSubMenu = std::make_unique<MenuPage>(&tft, mainMenu.get());

  // Setup menus
  wifiSubMenu->addItem("SSID", nullptr, []() {
    std::vector<ListItem> choices = {
        {"Apple", 101},
        {"Banana", 102},
        {"Orange", 103}
    };
    
    PageManager::showListDialog("Pick a fruit", choices,
      [](bool accepted, ListItem selected) {
        delay(200);
        if (accepted) {
          String message = "You picked: " + selected.label + " (value = " + String(selected.value) + ")";
          PageManager::showPopup(message.c_str());
        } else {
          PageManager::showPopup("Cancelled.");
        }
    });
  });


  wifiSubMenu->addItem("Password", nullptr, []() {
    PageManager::showInput("Enter password:", ALPHANUMERIC, [](String input, bool ok) {
      if (ok) {
        String message = "Password: " + input;
        PageManager::showPopup(message.c_str());
      } else {
        PageManager::showPopup("Input cancelled");
      }
    });
  });

  

  mainMenu->addItem("Option 1", nullptr, []() {
        PageManager::showPopup("Goodbye!");
    });
  mainMenu->addItem("Configure Wifi", std::move(wifiSubMenu));
  mainMenu->addItem("Option 3", nullptr, []() {
        PageManager::showInput("Enter value:", ALPHANUMERIC, [](String input, bool ok) {
            if (ok) {
                Serial1.printf("Input: %s\n", input.c_str());
            } else {
                Serial1.println("Input cancelled");
            }
        });
    });
  mainMenu->addItem("Option 4");
  mainMenu->addItem("Option 5");
  mainMenu->addItem("Option 6");
  

  PageManager::pushPage(std::move(mainMenu));  // Start the UI

  // Initialize train display
  // trainDisplay.begin();

  delay(2000);
}

void loop() {
  PageManager::handleInput();
}

int16_t readAngle(pin_size_t pin) {
  // Read the potentiometer and convert to angle
  int16_t angle = analogRead(pin);
  angle = map(angle, 0, 1023, 0, 240);
  Serial1.printf("Angle: %d\n", angle);
  return angle;
}