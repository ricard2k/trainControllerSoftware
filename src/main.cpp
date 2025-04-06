#include <Arduino.h>
#include "UIManager.h"

// Create UIManager instance
UIManager uiManager;

void setup() {
  Serial1.begin(115200); // Debug only


  // Initialize the UI
  uiManager.init();

  // Start the UI task
  uiManager.startTask();
}

void loop() {
    // No UI logic here; handled by the UIManager task
}
