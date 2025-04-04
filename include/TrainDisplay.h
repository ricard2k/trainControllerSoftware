#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>

// Font definitions
#include "NotoSansBold36.h"
#define AA_FONT_LARGE NotoSansBold36

class TrainDisplay {
private:
    // Display parameters
    const int16_t NEEDLE_LENGTH;
    const int16_t NEEDLE_WIDTH;
    const int16_t NEEDLE_RADIUS;
    const uint16_t NEEDLE_COLOR1;
    const uint16_t NEEDLE_COLOR2;
    const int16_t DIAL_CENTRE_X;
    const int16_t DIAL_CENTRE_Y;
    
    // Display objects
    TFT_eSPI* tft;  // Pointer to externally initialized TFT object
    TFT_eSprite needle;
    TFT_eSprite spr;
    
    // Buffer for display
    uint16_t* tft_buffer;
    bool buffer_loaded;
    uint16_t spr_width;
    uint16_t bg_color;
    
    // Static callback for JPEG decoder
    static TrainDisplay* instance;
    static bool jpegCallback(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap);

    // Private methods
    void createNeedle();

public:
    TrainDisplay(TFT_eSPI& display);  // Constructor accepts TFT object
    ~TrainDisplay();
    
    void begin();  // No longer accepts TFT object
    void plotNeedle(int16_t angle, uint16_t ms_delay);
    bool tftOutput(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap);
};
