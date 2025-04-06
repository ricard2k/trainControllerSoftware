#include "PopupPage.h"
#include "PageManager.h"
#include "LibraryConfig.h"
#include "ThreadSafeTFT.h"

PopupPage::PopupPage(String msg, std::function<void()> cb)
    : message(msg), onClose(cb) {}

void PopupPage::draw() {
    ThreadSafeTFT::withLock([this](TFT_eSPI& tft) {
        int popupWidth = (TFT_WIDTH /16) *14;
        int popupHeight = (TFT_HEIGHT /3);

        int x = (PAGE_LIBRARY_SCREEN_WIDTH - popupWidth) / 2;
        int y = (PAGE_LIBRARY_SCREEN_HEIGHT - popupHeight) / 2;
        tft.fillRect(x, y, popupWidth, popupHeight, TFT_DARKGREY);
        tft.drawRect(x, y, popupWidth, popupHeight, TFT_WHITE);
        tft.setTextColor(TFT_WHITE);
        tft.drawCentreString(message, PAGE_LIBRARY_SCREEN_WIDTH / 2, y + 20, 2);
    });
}

void PopupPage::handleInput() {
    // Dismiss on any button press
    if (digitalRead(2) == LOW || digitalRead(PAGE_LIBRARY_BTN_LEFT) == LOW ||
        digitalRead(4) == LOW || digitalRead(PAGE_LIBRARY_BTN_RIGHT) == LOW ||
        digitalRead(PAGE_LIBRARY_BTN_OK) == LOW) {
        PageManager::popPage();
        if (onClose) onClose();
        delay(200);  // debounce
    }
}