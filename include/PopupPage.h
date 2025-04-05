#pragma once
#include "IPage.h"
#include <TFT_eSPI.h>
#include <functional>

class PopupPage : public IPage {
public:
    PopupPage(TFT_eSPI* tft, String message, std::function<void()> onClose = nullptr);

    void handleInput() override;
    void draw() override;
    TFT_eSPI* getDisplay() const override;

private:
    TFT_eSPI* tft;
    String message;
    std::function<void()> onClose;
};
