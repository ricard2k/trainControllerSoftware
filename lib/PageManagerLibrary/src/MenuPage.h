#pragma once
#include <TFT_eSPI.h>
#include <vector>
#include <functional>
#include "IPage.h"

#ifndef MAX_VISIBLE_ITEMS
#define MAX_VISIBLE_ITEMS 5     // How many items to show on screen
#endif

#ifndef ITEM_HEIGHT
#define ITEM_HEIGHT 20          // Pixel height per menu item
#endif

#ifndef SCROLLBAR_WIDTH
#define SCROLLBAR_WIDTH 6       // Width of scrollbar in pixels
#endif

class MenuPage;

struct MenuItem {
    String label;
    std::unique_ptr<MenuPage> submenu;
    std::function<void()> onSelect = nullptr;

    MenuItem(String l, std::unique_ptr<MenuPage> sub = nullptr, std::function<void()> onSelect = nullptr);

};

class MenuPage : public IPage {
public:
    MenuPage(MenuPage* parent = nullptr);
    void addItem(String label, std::unique_ptr<MenuPage> submenu = nullptr, std::function<void()> onSelect = nullptr);
    static IPage* activePage;
    void handleInput(IKeyboard* keyboard);
    void draw();

private:
    std::vector<MenuItem> items;
    int selectedIndex = 0;
    MenuPage* parentMenu;
    int scrollOffset = 0;
    void moveUp();
    void moveDown();
    void enter();
    void back();

};
