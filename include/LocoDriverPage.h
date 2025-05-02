#pragma once

#include <IPage.h>
#include <Arduino.h>
#include "LocoCommandManager.h"

class LocoDriverPage : public IPage {
private:
    LocoCommandManager* locoManager;
    
    // Current values
    int currentSpeed = 0;
    int currentBrake = 0;
    
    // UI positions and dimensions
    const int speedGaugeX = 80;
    const int speedGaugeY = 120;
    const int brakeGaugeX = 240;
    const int brakeGaugeY = 120;
    const int gaugeRadius = 70;
    
    // Helper methods for drawing
    void drawSpeedGauge(TFT_eSPI& tft);
    void drawBrakeGauge(TFT_eSPI& tft);
    void drawNeedle(TFT_eSPI& tft, int centerX, int centerY, int value, int maxValue, int radius, uint16_t color);
    void drawGaugeLabels(TFT_eSPI& tft, int centerX, int centerY, int maxValue, int radius);

public:
    LocoDriverPage(LocoCommandManager* manager);
    void draw() override;
    void handleInput(IKeyboard* keyboard) override;
    
    // Methods to update gauge values
    void updateSpeed(int speed);
    void updateBrake(int brake);
};