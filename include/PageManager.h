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
    static void showListDialog(
        const String& title,
        const std::vector<ListItem>& options,
        std::function<void(bool accepted, ListItem selected)> onResult);
    static void showInput(const String& prompt, InputMode mode,
        std::function<void(String, bool)> onComplete);
    static void showPopup(const String& message, std::function<void()> onClose = nullptr);
    static void pushPage(std::unique_ptr<IPage> page);
    static void popPage();
    static IPage* currentPage();
    static void handleInput();
    static void draw();

private:
    static std::stack<std::unique_ptr<IPage>> pageStack;

};
