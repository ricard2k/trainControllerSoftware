#pragma once
#include "IPage.h"
#include <TFT_eSPI.h>
#include <Arduino.h>
#include <FreeRTOS.h>
#include <FreeRTOSConfig.h>
#include <task.h>

class LoadingPage : public IPage {
public:
    LoadingPage(const String& message);
    ~LoadingPage(); // needed to clean up the task

    void handleInput(IKeyboard* keyboard) override;
    void draw() override;
    void stop(); // signal task to terminate

private:
    static void animationTask(void* param);

    String message;

    TaskHandle_t animationTaskHandle = nullptr;
    volatile bool running = true;
};
