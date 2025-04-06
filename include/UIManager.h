#pragma once

#include <TFT_eSPI.h>
#include <memory>
#include <FreeRTOS.h>
#include <task.h>

class UIManager {
public:
    UIManager();
    ~UIManager();

    void init();      // Initialize the UI
    void startTask(); // Start the UI task

private:
    static void uiTask(void* param); // FreeRTOS task function
    void setupMenus();               // Setup the menus

    TFT_eSPI tft;          // Encapsulated TFT display object
    TaskHandle_t uiTaskHandle; // Handle for the UI task
};