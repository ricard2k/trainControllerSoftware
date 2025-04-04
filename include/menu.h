#pragma once
#include <TFT_eSPI.h>
#include <vector>
#include <functional>

#ifndef MAX_VISIBLE_ITEMS
#define MAX_VISIBLE_ITEMS 5     // How many items to show on screen
#endif

#ifndef ITEM_HEIGHT
#define ITEM_HEIGHT 20          // Pixel height per menu item
#endif

#ifndef SCROLLBAR_WIDTH
#define SCROLLBAR_WIDTH 6       // Width of scrollbar in pixels
#endif

class Menu;

struct MenuItem {
    String label;
    Menu* submenu = nullptr;
    std::function<void()> onSelect = nullptr;

    MenuItem(String l, Menu* sub = nullptr, std::function<void()> cb = nullptr);
};

class Menu {
public:
    Menu(TFT_eSPI* display, Menu* parent = nullptr);
    void addItem(String label, Menu* submenu = nullptr, std::function<void()> onSelect = nullptr);
    static Menu* activeMenu;
    void handleInput();
    void draw();

private:
    std::vector<MenuItem> items;
    int selectedIndex = 0;
    TFT_eSPI* tft;
    Menu* parentMenu;
    int scrollOffset = 0;
    void moveUp();
    void moveDown();
    void enter();
    void back();

};
