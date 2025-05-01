#pragma once

#include <IKeyboard.h>
#include <Arduino.h>

class MatrixKeyboard : public IKeyboard {
private:
    // Matrix pins
    uint8_t rowPins[2];        // 2 rows
    uint8_t colPins[3];        // 3 columns
    uint8_t softPowerPin;      // Special pin for soft power button
    
    // Key mapping for matrix positions [row][column]
    uint16_t keyMap[2][3];

public:
    // Constructor - takes pin numbers for rows, columns, and soft power button
    MatrixKeyboard(const uint8_t rowPins[2], const uint8_t colPins[3], uint8_t softPowerPin);
    
    // Required by IKeyboard interface
    uint16_t getPressedKeys() override;
    
    // Soft power control methods
    void powerOff();
    
private:
    void initializePins();
    void scanRow(uint8_t row, uint16_t& pressedKeys);
};