#pragma once
#include "IPage.h"
#include <stack>
#include <memory>
#include <functional>
#include "InputPopupPage.h"
#include "ListItem.h"
#include <vector>

class PageManager {
public:
    static void showSplash(const uint16_t* img16Bit, int w, int h, unsigned long duration = 3000);
    static void showSplash(const uint8_t* img8Bit, bool bpp8, const uint16_t* colmap, int w, int h, unsigned long duration = 3000);
    static void showLoading(const String& message);
    static void hideLoading();
    static void showListDialog(
        const String& title,
        const std::vector<ListItem>& options,
        std::function<void(bool accepted, ListItem selected)> onResult);
    // New method that accepts an initial selected index
    static void showListDialog(
        const String& title,
        const std::vector<ListItem>& options,
        int initialSelectedIndex,
        std::function<void(bool accepted, ListItem selected)> onResult);
    static void showInput(const String& prompt, InputMode mode,
        std::function<void(String, bool)> onComplete);
    // New method that accepts an initial value
    static void showInput(const String& prompt, InputMode mode,
        const String& initialValue,
        std::function<void(String, bool)> onComplete);
    static void showPopup(const String& message, std::function<void()> onClose = nullptr);
    static void pushPage(std::unique_ptr<IPage> page);
    static void popPage();
    static IPage* currentPage();
    static void handleInput(IKeyboard* keyboard);
    static void draw();

private:
    static std::stack<std::unique_ptr<IPage>> pageStack;
};
