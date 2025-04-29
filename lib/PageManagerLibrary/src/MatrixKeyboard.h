#pragma once

#include <IKeyboard.h>
#include <vector>

class MatrixKeyboard : public IKeyboard {
private:
    std::vector<uint8_t> rowPins; // Pins connected to the rows
    std::vector<uint8_t> colPins; // Pins connected to the columns
    uint16_t keyMap[5][5];        // Map of keys in the matrix (adjust size as needed)

public:
    MatrixKeyboard(const std::vector<uint8_t>& rows, const std::vector<uint8_t>& cols);

    uint16_t getPressedKeys() override;

private:
    void initializePins();
    uint16_t scanMatrix();
};