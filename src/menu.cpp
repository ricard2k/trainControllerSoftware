#include "menu.h"
#include "Buttons.h"

Menu* Menu::activeMenu = nullptr;

MenuItem::MenuItem(String l, Menu* sub, std::function<void()> cb)
    : label(l), submenu(sub), onSelect(cb) {}


Menu::Menu(TFT_eSPI* display, Menu* parent)
    : tft(display), parentMenu(parent) {}

void Menu::addItem(String label, Menu* submenu, std::function<void()> onSelect) {
    items.emplace_back(label, submenu, onSelect);
}

void Menu::moveUp() {
    if (selectedIndex > 0) {
        selectedIndex--;
        if (selectedIndex < scrollOffset) {
            scrollOffset--;
        }
        draw();
    }
}

void Menu::moveDown() {
    if (selectedIndex < items.size() - 1) {
        selectedIndex++;
        if (selectedIndex >= scrollOffset + MAX_VISIBLE_ITEMS) {
            scrollOffset++;
        }
        draw();
    }
}

void Menu::enter() {
    MenuItem& item = items[selectedIndex];
    if (item.submenu) {
        item.submenu->draw();
        activeMenu = item.submenu;
    } else if (item.onSelect) {
        item.onSelect();
    }
}

void Menu::back() {
    if (parentMenu) {
        parentMenu->draw();
        activeMenu = parentMenu;
    }
}

void Menu::draw() {
    tft->fillScreen(TFT_BLACK);

    int visibleItems = std::min((int)items.size(), MAX_VISIBLE_ITEMS);

    for (int i = 0; i < visibleItems; ++i) {
        int itemIndex = scrollOffset + i;
        if (itemIndex >= items.size()) break;

        if (itemIndex == selectedIndex) {
            tft->setTextColor(TFT_BLACK, TFT_WHITE);
        } else {
            tft->setTextColor(TFT_WHITE, TFT_BLACK);
        }

        tft->drawString(items[itemIndex].label, 10, 20 + i * ITEM_HEIGHT, 2);
    }

    // Draw scrollbar if needed
    if (items.size() > MAX_VISIBLE_ITEMS) {
        int scrollbarHeight = MAX_VISIBLE_ITEMS * ITEM_HEIGHT;
        int scrollbarTop = 20;
        int thumbHeight = (MAX_VISIBLE_ITEMS * scrollbarHeight) / items.size();
        int thumbY = scrollbarTop + (selectedIndex * scrollbarHeight) / items.size();

        tft->fillRect(235, scrollbarTop, SCROLLBAR_WIDTH, scrollbarHeight, TFT_DARKGREY); // Track
        tft->fillRect(235, thumbY, SCROLLBAR_WIDTH, thumbHeight, TFT_WHITE);              // Thumb
    }
    activeMenu = this; 

}

void Menu::handleInput() {
    if (digitalRead(TC_BTN_UP) == LOW) {
        moveUp();
        delay(200);
    } else if (digitalRead(TC_BTN_DOWN) == LOW) {
        moveDown();
        delay(200);
    } else if (digitalRead(TC_BTN_RIGHT) == LOW) {
        enter();
        delay(200);
    } else if (digitalRead(TC_BTN_LEFT) == LOW) {
        back();
        delay(200);
    }
}

