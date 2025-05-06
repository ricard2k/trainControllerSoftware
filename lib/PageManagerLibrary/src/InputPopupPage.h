#pragma once

#include "IPage.h"
#include "LibraryConfig.h"
#include <Arduino.h>
#include <vector>
#include <functional>

enum InputMode {
    NUMERIC,
    ALPHANUMERIC,
    NUMERIC_IP
};

class InputPopupPage : public IPage {
public:
    InputPopupPage(const String& prompt, InputMode mode,
                   std::function<void(String, bool)> onComplete);
    // New constructor with initial value parameter
    InputPopupPage(const String& prompt, InputMode mode, const String& initialValue,
                   std::function<void(String, bool)> onComplete);
    void handleInput(IKeyboard* keyboard) override;
    void draw() override;

private:
    void drawKeyboard();
    void drawInputBox();
    void handleKeyPress();
    void moveSelection(int dx, int dy);
    void buildKeyboard();
    void toggleCursor();
    bool isValidIPAddress(const String& ip);

    String prompt;
    InputMode mode;
    String inputBuffer;
    std::function<void(String, bool)> onComplete;

    std::vector<String> keys;
    int selectedIndex = 0;
    int cols = 6;
    int rows = 0;

    int keyPadding = PAGE_LIBRARY_SCREEN_WIDTH / 64;
    int keyWidth = (PAGE_LIBRARY_SCREEN_WIDTH - (cols + 1) * keyPadding) / cols;
    int keyHeight = PAGE_LIBRARY_SCREEN_HEIGHT / 12;
    int startX = keyPadding;
    int startY = PAGE_LIBRARY_SCREEN_HEIGHT / 2;

    bool showCursor = true;
    unsigned long lastCursorToggle = 0;
    unsigned long cursorInterval = 500;
};

