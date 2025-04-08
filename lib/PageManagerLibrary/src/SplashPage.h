#pragma once
#include "IPage.h"
#include <TFT_eSPI.h>
#include <Arduino.h>

class SplashPage : public IPage {
public:
    SplashPage(const uint16_t* img16bit, const uint8_t* img8bit, bool bpp8, const uint16_t* colmap, int w, int h, unsigned long durationMs);

    void handleInput(IKeyboard* keyboard) override;
    void draw() override;


private:
    const uint16_t* image16bits; // For 16-bit images
    const uint8_t* image8bits; // For 8-bit images
    bool bpp8; // for 8-bit images
    const uint16_t* colmap; // For 8-bit images, palete
    int imgWidth, imgHeight;
    unsigned long duration;
    unsigned long startTime;
};
