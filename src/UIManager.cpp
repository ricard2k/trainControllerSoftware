#include "UIManager.h"
#include "TrainController360x240.h"
#include <PageManager.h>
#include <MenuPage.h>
#include <ThreadSafeTFT.h>
#include "Config.h"
#include "AnalogKeyboard.h"

UIManager::UIManager() : tft(), uiTaskHandle(nullptr) {}

UIManager::~UIManager() {
    if (uiTaskHandle) {
        vTaskDelete(uiTaskHandle);
    }
}

void UIManager::begin() {

    //create an instance of keyboard
    keyboard = new AnalogKeyboard(A0);
    
    // Create an instance of AnalogSwitch
    analogSwitch = new AnalogSwitch(D14, D15, D16);

    // Initialize the TFT display
    tft.begin();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);

    // Initialize ThreadSafeTFT
    ThreadSafeTFT::init(&tft);

    // Setup the menus, this is the root node
    setupMenus();
    // Show splash screen
    PageManager::showSplash(trainControllerImage, false, trainControllerPalette, 360, 240, 3000);
}

void UIManager::startTask() {
    // Create the UI task
    xTaskCreate(
        uiTask,         // Task function
        "UITask",       // Task name
        4096,           // Stack size (in bytes)
        this,           // Task parameter (pass the UIManager instance)
        1,              // Task priority
        &uiTaskHandle   // Task handle
    );
}

void UIManager::uiTask(void* param) {
    UIManager* self = static_cast<UIManager*>(param);

    while (true) {
        // Handle input and draw the current page
        self->analogSwitch->switchTo(0); // Switch to channel 0
        PageManager::handleInput(self->keyboard);
    }
}

void UIManager::setupMenus() {
    auto mainMenu = std::make_unique<MenuPage>();
    auto wifiSubMenu = std::make_unique<MenuPage>(mainMenu.get());

    // Setup WiFi submenu
    wifiSubMenu->addItem("SSID", nullptr, []() {
        std::vector<ListItem> choices = {
            {"Apple", 101},
            {"Banana", 102},
            {"Orange", 103}
        };

        PageManager::showListDialog("Pick a fruit", choices,
            [](bool accepted, ListItem selected) {
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

    // Setup main menu
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
    mainMenu->addItem("Loading example", nullptr, []() {
        PageManager::showLoading("Loading...");
        delay(5000);
        PageManager::hideLoading();
    });
    mainMenu->addItem("Option 5");
    mainMenu->addItem("Option 6");

    // Push the main menu to the PageManager
    PageManager::pushPage(std::move(mainMenu));
}