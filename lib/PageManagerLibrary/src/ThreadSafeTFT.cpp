#include "ThreadSafeTFT.h"

TFT_eSPI* ThreadSafeTFT::tft = nullptr;
SemaphoreHandle_t ThreadSafeTFT::mutex = nullptr;

void ThreadSafeTFT::init(TFT_eSPI* tftInstance) {
    tft = tftInstance;
    if (!mutex) {
        mutex = xSemaphoreCreateMutex();
    }
}

void ThreadSafeTFT::withLock(std::function<void(TFT_eSPI&)> action) {
    if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE) {
        if (tft) {
            action(*tft); // Execute the action with the TFT object
        }
        xSemaphoreGive(mutex);
    }
}