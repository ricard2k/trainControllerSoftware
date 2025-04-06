#pragma once

class TFT_eSPI;

class IPage {
public:
    virtual void handleInput() = 0;
    virtual void draw() = 0;
    virtual ~IPage() = default;
};
