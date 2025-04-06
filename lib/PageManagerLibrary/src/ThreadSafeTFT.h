#pragma once

#include <TFT_eSPI.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <functional>

class ThreadSafeTFT {
public:
    // Initialize the TFT object and semaphore
    static void init(TFT_eSPI* tftInstance);

    // Lock the semaphore and access the TFT object
    static void withLock(std::function<void(TFT_eSPI&)> action);

private:
    static TFT_eSPI* tft;           // Pointer to the TFT object
    static SemaphoreHandle_t mutex; // Semaphore for thread safety
};