#include "DialogListPage.h"
#include "PageManager.h"

DialogListPage::DialogListPage(TFT_eSPI* tft, const String& title,
                               const std::vector<ListItem>& items,
                               std::function<void(bool, ListItem)> callback)
    : tft(tft), title(title), items(items), callback(callback) {}

TFT_eSPI* DialogListPage::getDisplay() const {
    return tft;
}

void DialogListPage::draw() {
    tft->fillScreen(TFT_BLACK);
    tft->setTextColor(TFT_WHITE);
    tft->drawCentreString(title, TC_SCREEN_WIDTH / 2, 10, 2);
    drawItems();
    drawButtons();
}

void DialogListPage::drawItems() {
    for (size_t i = 0; i < items.size(); ++i) {
        int y = listTopY + i * itemHeight;

        bool isSelected = (i == selectedIndex);
        uint16_t bg = isSelected && !focusOnButtons ? TFT_WHITE :
                  isSelected &&  focusOnButtons ? TFT_DARKGREY :
                                                 TFT_BLACK;
       
        tft->fillRect(0, y, TC_SCREEN_WIDTH, itemHeight, bg);
        tft->setTextColor(bg == TFT_WHITE ? TFT_BLACK : TFT_WHITE);

        tft->drawString(items[i].label, 10, y + 4, 2);
    }
}

void DialogListPage::drawButtons() {
    int y = TC_SCREEN_HEIGHT - 30;
    int btnW = 80;
    int spacing = 20;
    int xOk = (TC_SCREEN_WIDTH / 2) - btnW - spacing / 2;
    int xCancel = (TC_SCREEN_WIDTH / 2) + spacing / 2;

    // OK Button
    tft->fillRect(xOk, y, btnW, 24, focusOnButtons && selectedButton == 0 ? TFT_WHITE : TFT_BLUE);
    tft->setTextColor(focusOnButtons && selectedButton == 0 ? TFT_BLACK : TFT_WHITE);
    tft->drawCentreString("OK", xOk + btnW / 2, y + 4, 2);

    // Cancel Button
    tft->fillRect(xCancel, y, btnW, 24, focusOnButtons && selectedButton == 1 ? TFT_WHITE : TFT_BLUE);
    tft->setTextColor(focusOnButtons && selectedButton == 1 ? TFT_BLACK : TFT_WHITE);
    tft->drawCentreString("Cancel", xCancel + btnW / 2, y + 4, 2);
}

void DialogListPage::moveSelection(int delta) {
    if (!focusOnButtons) {
        selectedIndex = constrain(selectedIndex + delta, 0, items.size() - 1);
    } else {
        selectedButton = (selectedButton + 1) % 2;
    }
    draw();
}

void DialogListPage::handleInput() {
    if (digitalRead(TC_BTN_UP) == LOW) {
        if (!focusOnButtons) moveSelection(-1);
        delay(200);
    } else if (digitalRead(TC_BTN_DOWN) == LOW) {
        if (!focusOnButtons) moveSelection(1);
        delay(200);
    } else if (digitalRead(TC_BTN_LEFT) == LOW) {
        if (focusOnButtons) moveSelection(-1); 
        else focusOnButtons = true;
        draw();
        delay(200);
    } else if (digitalRead(TC_BTN_RIGHT) == LOW) {
        if (focusOnButtons) moveSelection(1);
        else focusOnButtons = true;
        draw();
        delay(200);
    } else if (digitalRead(TC_BTN_OK) == LOW) {
        if (focusOnButtons) {
            bool accepted = selectedButton == 0;
            PageManager::popPage();
            if (callback) callback(accepted, items[selectedIndex]);
        } else {
            focusOnButtons = true;
            draw();
            delay(200);
        }
    }   
}
