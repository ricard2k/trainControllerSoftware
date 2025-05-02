#include "LocoDriverPage.h"
#include "ThreadSafeTFT.h"
#include "PageManager.h"
#include "ExtendedKeys.h"

LocoDriverPage::LocoDriverPage(LocoCommandManager* manager) : locoManager(manager) {
    // Initialize with default values
    currentSpeed = 0;
    currentBrake = 0;
}

void LocoDriverPage::draw() {
    ThreadSafeTFT::withLock([this](TFT_eSPI& tft) {
        // Clear screen
        tft.fillScreen(TFT_BLACK);
        
        // Draw title
        tft.setTextColor(TFT_WHITE);
        tft.drawCentreString("Train Controls", 160, 20, 4);
        
        // Draw the gauges
        drawSpeedGauge(tft);
        drawBrakeGauge(tft);
        
        // Draw labels for the gauges
        tft.setTextColor(TFT_WHITE);
        tft.drawCentreString("Speed", speedGaugeX, speedGaugeY + gaugeRadius + 10, 2);
        tft.drawCentreString("Brake", brakeGaugeX, brakeGaugeY + gaugeRadius + 10, 2);
        
        // Draw current values
        tft.setTextColor(TFT_YELLOW);
        tft.drawCentreString(String(currentSpeed) + " km/h", speedGaugeX, speedGaugeY + gaugeRadius + 30, 2);
        tft.drawCentreString(String(currentBrake) + " psi", brakeGaugeX, brakeGaugeY + gaugeRadius + 30, 2);
        
        // Draw key instructions at bottom of screen
        tft.setTextColor(TFT_CYAN);
        tft.drawString("UP/DOWN: Speed", 10, 220, 2);
        tft.drawString("B1/B2: Brake", 220, 220, 2);
    });
}

void LocoDriverPage::drawSpeedGauge(TFT_eSPI& tft) {
    // Draw gauge background
    tft.fillCircle(speedGaugeX, speedGaugeY, gaugeRadius, TFT_DARKGREY);
    tft.fillCircle(speedGaugeX, speedGaugeY, gaugeRadius - 5, TFT_BLACK);
    
    // Draw gauge labels
    drawGaugeLabels(tft, speedGaugeX, speedGaugeY, 100, gaugeRadius);
    
    // Draw the needle
    drawNeedle(tft, speedGaugeX, speedGaugeY, currentSpeed, 100, gaugeRadius - 10, TFT_RED);
}

void LocoDriverPage::drawBrakeGauge(TFT_eSPI& tft) {
    // Draw gauge background
    tft.fillCircle(brakeGaugeX, brakeGaugeY, gaugeRadius, TFT_DARKGREY);
    tft.fillCircle(brakeGaugeX, brakeGaugeY, gaugeRadius - 5, TFT_BLACK);
    
    // Draw gauge labels
    drawGaugeLabels(tft, brakeGaugeX, brakeGaugeY, 100, gaugeRadius);
    
    // Draw the needle
    drawNeedle(tft, brakeGaugeX, brakeGaugeY, currentBrake, 100, gaugeRadius - 10, TFT_GREEN);
}

void LocoDriverPage::drawNeedle(TFT_eSPI& tft, int centerX, int centerY, int value, int maxValue, int radius, uint16_t color) {
    // Calculate angle based on value (0-270 degrees, where 0 is at 9 o'clock position)
    float angle = map(value, 0, maxValue, 0, 270) - 135;
    angle = angle * PI / 180.0; // Convert to radians
    
    // Calculate needle endpoint
    int endX = centerX + radius * cos(angle);
    int endY = centerY + radius * sin(angle);
    
    // Draw the needle
    tft.drawLine(centerX, centerY, endX, endY, color);
    tft.drawLine(centerX-1, centerY, endX, endY, color); // Make needle thicker
    tft.drawLine(centerX+1, centerY, endX, endY, color);
    
    // Draw center point
    tft.fillCircle(centerX, centerY, 5, color);
}

void LocoDriverPage::drawGaugeLabels(TFT_eSPI& tft, int centerX, int centerY, int maxValue, int radius) {
    tft.setTextColor(TFT_WHITE);
    
    // Draw major tick marks and labels at 0, 25, 50, 75, 100%
    for (int i = 0; i <= 4; i++) {
        int value = i * (maxValue / 4);
        float angle = map(value, 0, maxValue, 0, 270) - 135;
        angle = angle * PI / 180.0; // Convert to radians
        
        // Calculate tick mark positions
        int innerX = centerX + (radius - 15) * cos(angle);
        int innerY = centerY + (radius - 15) * sin(angle);
        int outerX = centerX + radius * cos(angle);
        int outerY = centerY + radius * sin(angle);
        
        // Draw tick mark
        tft.drawLine(innerX, innerY, outerX, outerY, TFT_WHITE);
        
        // Draw label
        int labelX = centerX + (radius - 25) * cos(angle);
        int labelY = centerY + (radius - 25) * sin(angle);
        tft.drawString(String(value), labelX, labelY, 1);
    }
}

void LocoDriverPage::handleInput(IKeyboard* keyboard) {
    uint16_t keys = keyboard->getPressedKeys();
    
    bool needsRedraw = false;
    
    // Check for brake control keys
    if (keys & ExtendedKeys::KEY_TIGHT_BRAKE) {
        // Increase brake pressure (max 100)
        currentBrake = min(currentBrake + 5, 100);
        locoManager->setBrake(currentBrake);
        needsRedraw = true;
    }
    
    if (keys & ExtendedKeys::KEY_RELEASE_BRAKE) {
        // Decrease brake pressure (min 0)
        currentBrake = max(currentBrake - 5, 0);
        locoManager->setBrake(currentBrake);
        needsRedraw = true;
    }
    
    // Normal navigation keys can be used for speed control
    if (keys & KEY_UP) {
        // Increase speed (max 100)
        currentSpeed = min(currentSpeed + 5, 100);
        locoManager->setSpeed(currentSpeed);
        needsRedraw = true;
    }
    
    if (keys & KEY_DOWN) {
        // Decrease speed (min 0)
        currentSpeed = max(currentSpeed - 5, 0);
        locoManager->setSpeed(currentSpeed);
        needsRedraw = true;
    }
    
    // Go back to main menu with OK button
    if (keys & KEY_OK) {
        PageManager::popPage();
        return;
    }
    
    // Only redraw if values changed
    if (needsRedraw) {
        draw();
    }
}

void LocoDriverPage::updateSpeed(int speed) {
    if (currentSpeed != speed) {
        currentSpeed = speed;
        draw();
    }
}

void LocoDriverPage::updateBrake(int brake) {
    if (currentBrake != brake) {
        currentBrake = brake;
        draw();
    }
}