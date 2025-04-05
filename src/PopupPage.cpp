#include "PopupPage.h"
#include "PageManager.h"
#include "Config.h"

PopupPage::PopupPage(TFT_eSPI* tft, String msg, std::function<void()> cb)
    : tft(tft), message(msg), onClose(cb) {}

void PopupPage::draw() {
    // Dim background
    int popupWidth = 260;
    int popupHeight = 60;

    int x = (TC_SCREEN_WIDTH - popupWidth) / 2;
    int y = (TC_SCREEN_HEIGHT - popupHeight) / 2;

    tft->fillRect(x, y, popupWidth, popupHeight, TFT_DARKGREY);
    tft->drawRect(x, y, popupWidth, popupHeight, TFT_WHITE);
    tft->setTextColor(TFT_WHITE);
    tft->drawCentreString(message, TC_SCREEN_WIDTH / 2, y + 20, 2);
}

void PopupPage::handleInput() {
    // Dismiss on any button press
    if (digitalRead(2) == LOW || digitalRead(TC_BTN_LEFT) == LOW ||
        digitalRead(4) == LOW || digitalRead(TC_BTN_RIGHT) == LOW ||
        digitalRead(TC_BTN_OK) == LOW) {
        PageManager::popPage();
        if (onClose) onClose();
        delay(200);  // debounce
    }
}

TFT_eSPI* PopupPage::getDisplay() const {
    return tft;
}
