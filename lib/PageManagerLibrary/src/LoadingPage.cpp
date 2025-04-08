#include "LoadingPage.h"
#include "LibraryConfig.h"
#include "ThreadSafeTFT.h"


LoadingPage::LoadingPage(const String& msg)
    : message(msg) {
    draw(); // Draw initial frame
    xTaskCreate(animationTask, "LoadingSpinner", 1024, this, 1, &animationTaskHandle);
}

LoadingPage::~LoadingPage() {
    stop(); // ensure task cleanup
}

void LoadingPage::stop() {
    running = false;
    if (animationTaskHandle) {
        vTaskDelete(animationTaskHandle);
        animationTaskHandle = nullptr;
    }
}


void LoadingPage::draw() {
    ThreadSafeTFT::withLock([this](TFT_eSPI& tft) {
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_WHITE);
        tft.drawCentreString(message, PAGE_LIBRARY_SCREEN_WIDTH / 2, 100, 2);
    });
}

void LoadingPage::animationTask(void* param) {
    LoadingPage* self = static_cast<LoadingPage*>(param);
    
    const int lineWidth = PAGE_LIBRARY_SCREEN_WIDTH * 0.5;
    const int cx = (PAGE_LIBRARY_SCREEN_WIDTH - lineWidth) / 2;
    const int cy = 160;
    const int r1 = 4;

    int circlePosition = 0; 
    int increment = 10; // pixels per step

    while (self->running) {
        
        ThreadSafeTFT::withLock([&circlePosition, &increment, cx, cy, r1, lineWidth](TFT_eSPI& tft) {
            // Clear the previous dot
            tft.fillCircle(cx + circlePosition - increment, cy, r1 +1, TFT_BLACK);
            tft.fillCircle(cx + circlePosition, cy, r1 +1, TFT_BLACK);
            tft.fillCircle(cx + circlePosition, cy, r1/2, TFT_BLUE);

            // Increment the position along the horizontal line
            if ((circlePosition + increment > lineWidth) ||
                (circlePosition + increment < 0)) {
                increment *= -1; // invert movement
            }
            circlePosition = (circlePosition + increment);

            // Draw the current dot
            tft.fillCircle(cx + circlePosition, cy, r1, TFT_BLUE);
        });

        vTaskDelay(pdMS_TO_TICKS(100));
    }
    
}

void LoadingPage::handleInput(IKeyboard* keyboard) {
    // Ignore input
}
