// InputPopupPage.cpp
#include "InputPopupPage.h"
#include "PageManager.h"
#include "ThreadSafeTFT.h"

InputPopupPage::InputPopupPage(const String &prompt, InputMode mode,
                               std::function<void(String, bool)> onComplete)
    : prompt(prompt), mode(mode), onComplete(onComplete)
{
  buildKeyboard();
  rows = (keys.size() + cols - 1) / cols;
}

void InputPopupPage::buildKeyboard()
{
  if (mode == NUMERIC)
  {
    keys = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "<", "OK", "Cancel"};
    cols = 3;
  }
  else
  {
    for (char c = 'A'; c <= 'Z'; ++c)
      keys.push_back(String(c));
    for (char c = '0'; c <= '9'; ++c)
      keys.push_back(String(c));
    keys.push_back(" ");
    keys.push_back("<");
    keys.push_back("OK");
    keys.push_back("Cancel");
    cols = 8;
  }
  keyWidth = (PAGE_LIBRARY_SCREEN_WIDTH - (cols + 1) * keyPadding) / cols;
  rows = (keys.size() + cols - 1) / cols;
  keyHeight = (PAGE_LIBRARY_SCREEN_HEIGHT - startY - keyPadding * (rows + 1)) / rows;
}

void InputPopupPage::toggleCursor()
{
  unsigned long now = millis();
  if (now - lastCursorToggle >= cursorInterval)
  {
    showCursor = !showCursor;
    lastCursorToggle = now;
    drawInputBox();
  }
}

void InputPopupPage::draw()
{
  ThreadSafeTFT::withLock([this](TFT_eSPI& tft) {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.drawCentreString(prompt, PAGE_LIBRARY_SCREEN_WIDTH / 2, 20, 2);
  });
  drawInputBox();
  drawKeyboard();
}

void InputPopupPage::drawInputBox()
{

  ThreadSafeTFT::withLock([&](TFT_eSPI& tft) {
    int boxWidth = PAGE_LIBRARY_SCREEN_WIDTH - 20;
    int boxHeight = 30;
    int x = 10;
    int y = 60;

    tft.drawRect(x, y, boxWidth, boxHeight, TFT_WHITE);
    tft.setTextColor(TFT_WHITE);
    tft.fillRect(x + 1, y + 1, boxWidth - 2, boxHeight - 2, TFT_BLACK);

    tft.setFreeFont(&FreeSans9pt7b); // Use a proportional font
    tft.setTextDatum(TL_DATUM);
    tft.drawString(inputBuffer, x + 5, y + 5);

    if (showCursor) {
      int cursorX = x + 6 + tft.textWidth(inputBuffer);
      int cursorY = y + 5;
      int cursorH = 16; // Approx height for FreeSans9pt7b
      tft.drawLine(cursorX, cursorY, cursorX, cursorY + cursorH, TFT_WHITE);
    }

    tft.setTextFont(2); // Restore default font for other elements
  });

}

void InputPopupPage::drawKeyboard()
{
  for (size_t i = 0; i < keys.size(); ++i)
  {
    ThreadSafeTFT::withLock([&](TFT_eSPI& tft) {
      int x = startX + (i % cols) * (keyWidth + keyPadding);
      int y = startY + (i / cols) * (keyHeight + keyPadding);

      uint16_t fillColor = TFT_BLUE;
      if ((int)i == selectedIndex)
        fillColor = TFT_WHITE;

      tft.fillRect(x, y, keyWidth, keyHeight, fillColor);
      tft.setTextColor(fillColor == TFT_WHITE ? TFT_BLACK : TFT_WHITE);

      int fontHeight = tft.fontHeight(2);
      int textY = y + (keyHeight - fontHeight) / 2;
      tft.drawCentreString(keys[i], x + keyWidth / 2, textY, 2);
    });
  }
}

void InputPopupPage::moveSelection(int dx, int dy)
{
  int col = selectedIndex % cols;
  int row = selectedIndex / cols;
  col = constrain(col + dx, 0, cols - 1);
  row = constrain(row + dy, 0, rows - 1);
  int newIndex = row * cols + col;
  if (newIndex < (int)keys.size())
    selectedIndex = newIndex;
  draw();
}

void InputPopupPage::handleKeyPress()
{
  String key = keys[selectedIndex];

  int x = startX + (selectedIndex % cols) * (keyWidth + keyPadding);
  int y = startY + (selectedIndex / cols) * (keyHeight + keyPadding);

  ThreadSafeTFT::withLock([this, key, x, y](TFT_eSPI& tft) {
    tft.fillRect(x, y, this->keyWidth, this->keyHeight, TFT_DARKGREY);
    tft.setTextColor(TFT_WHITE);
    int fontHeight = tft.fontHeight(2);
    int textY = y + (this->keyHeight - fontHeight) / 2;
    tft.drawCentreString(key, x + this->keyWidth / 2, textY, 2);
  });
  delay(100);

  if (key == "<")
  {
    if (!inputBuffer.isEmpty())
      inputBuffer.remove(inputBuffer.length() - 1);
  }
  else if (key == "OK")
  {
    PageManager::popPage();
    if (onComplete)
      onComplete(inputBuffer, true);
    return;
  }
  else if (key == "Cancel")
  {
    PageManager::popPage();
    if (onComplete)
      onComplete("", false);
    return;
  }
  else
  {
    inputBuffer += key;
  }
  draw();
}

void InputPopupPage::handleInput(IKeyboard* keyboard)
{
  uint16_t pressedKeys = keyboard->getPressedKeys();
  toggleCursor();
  if (pressedKeys & KEY_UP)
  {
    moveSelection(0, -1);
    delay(200);
  }
  else if (pressedKeys & KEY_DOWN)
  {
    moveSelection(0, 1);
    delay(200);
  }
  else if (pressedKeys & KEY_LEFT)
  {
    moveSelection(-1, 0);
    delay(200);
  }
  else if (pressedKeys & KEY_RIGHT)
  {
    moveSelection(1, 0);
    delay(200);
  }
  else if (pressedKeys & KEY_OK)
  {
    handleKeyPress();
    delay(200);
  }
}