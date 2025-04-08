#pragma once
#include "IPage.h"
#include <TFT_eSPI.h>
#include <functional>

class PopupPage : public IPage {
public:
    PopupPage(String message, std::function<void()> onClose = nullptr);

    void handleInput(IKeyboard* keyboard) override;
    void draw() override;

private:
    String message;
    std::function<void()> onClose;
};
