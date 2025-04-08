#include "DialogListPage.h"
#include "PageManager.h"
#include "ThreadSafeTFT.h"
#include "LibraryConfig.h"

DialogListPage::DialogListPage(const String& title,
                               const std::vector<ListItem>& items,
                               std::function<void(bool, ListItem)> callback)
    : title(title), items(items), callback(callback) {}


void DialogListPage::draw() {
    ThreadSafeTFT::withLock([this](TFT_eSPI& tft) {
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_WHITE);
        tft.drawCentreString(title, PAGE_LIBRARY_SCREEN_WIDTH / 2, 10, 2);
    });
    drawItems();
    drawButtons();
}

void DialogListPage::drawItems() {
    for (size_t i = 0; i < items.size(); ++i) {
                
        ThreadSafeTFT::withLock([this, i](TFT_eSPI& tft) {
            int y = listTopY + i * itemHeight;

            bool isSelected = (i == selectedIndex);
            uint16_t bg = isSelected && !focusOnButtons ? TFT_WHITE :
                    isSelected &&  focusOnButtons ? TFT_DARKGREY :
                                                    TFT_BLACK;
            tft.fillRect(0, y, PAGE_LIBRARY_SCREEN_WIDTH, itemHeight, bg);
            tft.setTextColor(bg == TFT_WHITE ? TFT_BLACK : TFT_WHITE);

            tft.drawString(items[i].label, 10, y + 4, 2);
        });
    }
}

void DialogListPage::drawButtons() {
    
    ThreadSafeTFT::withLock([this](TFT_eSPI& tft) {
        int y = PAGE_LIBRARY_SCREEN_HEIGHT - 30;
        int btnW = 80;
        int spacing = 20;
        int xOk = (PAGE_LIBRARY_SCREEN_WIDTH / 2) - btnW - spacing / 2;
        int xCancel = (PAGE_LIBRARY_SCREEN_WIDTH / 2) + spacing / 2;
        // OK Button
        tft.fillRect(xOk, y, btnW, 24, focusOnButtons && selectedButton == 0 ? TFT_WHITE : TFT_BLUE);
        tft.setTextColor(focusOnButtons && selectedButton == 0 ? TFT_BLACK : TFT_WHITE);
        tft.drawCentreString("OK", xOk + btnW / 2, y + 4, 2);

        // Cancel Button
        tft.fillRect(xCancel, y, btnW, 24, focusOnButtons && selectedButton == 1 ? TFT_WHITE : TFT_BLUE);
        tft.setTextColor(focusOnButtons && selectedButton == 1 ? TFT_BLACK : TFT_WHITE);
        tft.drawCentreString("Cancel", xCancel + btnW / 2, y + 4, 2);
    });
}

void DialogListPage::moveSelection(int delta) {
    if (!focusOnButtons) {
        selectedIndex = constrain(selectedIndex + delta, 0, items.size() - 1);
    } else {
        selectedButton = (selectedButton + 1) % 2;
    }
    draw();
}

void DialogListPage::handleInput(IKeyboard* keyboard) {
    uint16_t pressedKeys = keyboard->getPressedKeys();
    if (pressedKeys & KEY_UP) {
        if (!focusOnButtons) moveSelection(-1);
        delay(200);
    } else if (pressedKeys & KEY_DOWN) {
        if (!focusOnButtons) moveSelection(1);
        delay(200);
    } else if (pressedKeys & KEY_LEFT) {
        if (focusOnButtons) moveSelection(-1); 
        else focusOnButtons = true;
        draw();
        delay(200);
    } else if (pressedKeys & KEY_RIGHT) {
        if (focusOnButtons) moveSelection(1);
        else focusOnButtons = true;
        draw();
        delay(200);
    } else if (pressedKeys & KEY_OK) {
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
