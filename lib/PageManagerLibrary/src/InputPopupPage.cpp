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

// New constructor implementation that accepts initial value
InputPopupPage::InputPopupPage(const String &prompt, InputMode mode, 
                               const String &initialValue,
                               std::function<void(String, bool)> onComplete)
    : prompt(prompt), mode(mode), inputBuffer(initialValue), onComplete(onComplete)
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
  else if (mode == NUMERIC_IP)
  {
    keys = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0", ".", "<", "OK", "Cancel"};
    cols = 5;  // 5 columns for a more compact layout
  }
  else // ALPHANUMERIC
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
    // For IP addresses, validate before accepting
    if (mode == NUMERIC_IP && !isValidIPAddress(inputBuffer))
    {
      // Show error message
      ThreadSafeTFT::withLock([](TFT_eSPI& tft) {
        tft.setTextColor(TFT_RED, TFT_BLACK);
        tft.drawString("Invalid IP format", 10, 100, 2);
      });
      delay(1500); // Show error for 1.5 seconds
      draw(); // Redraw to clear error
      return;
    }
    
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
  else if (mode == NUMERIC_IP)
  {
    // Special handling for IP address input
    if (key == ".")
    {
      // Don't allow consecutive dots
      if (inputBuffer.isEmpty() || inputBuffer.endsWith("."))
        return;
        
      // Count existing dots to ensure we don't exceed 3
      int dotCount = 0;
      for (unsigned int i = 0; i < inputBuffer.length(); i++) {
        if (inputBuffer.charAt(i) == '.')
          dotCount++;
      }
      
      if (dotCount >= 3)
        return; // Already have 3 dots (xxx.xxx.xxx.xxx)
        
      inputBuffer += key;
    }
    else // Number key
    {
      // Check current octet value
      int lastDotPos = inputBuffer.lastIndexOf('.');
      String currentOctet = lastDotPos >= 0 ? 
                           inputBuffer.substring(lastDotPos + 1) : 
                           inputBuffer;
                           
      // Determine value if new digit is added
      String newOctet = currentOctet + key;
      int newValue = newOctet.toInt();
      
      // Only add digit if octet remains valid (0-255)
      if (newValue >= 0 && newValue <= 255 && newOctet.length() <= 3)
        inputBuffer += key;
    }
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

// Check if the input string is a valid IP address
bool InputPopupPage::isValidIPAddress(const String& ip)
{
  // Check if empty
  if (ip.isEmpty())
    return false;
    
  // IP must have exactly 3 dots
  int dotCount = 0;
  for (unsigned int i = 0; i < ip.length(); i++) {
    if (ip.charAt(i) == '.')
      dotCount++;
  }
  
  if (dotCount != 3)
    return false;
    
  // Split into octets and validate each one
  int lastPos = 0;
  for (int i = 0; i < 4; i++) {
    int pos = (i < 3) ? ip.indexOf('.', lastPos) : ip.length();
    
    // Check if we have data between dots
    if (pos <= lastPos)
      return false;
      
    String octet = ip.substring(lastPos, pos);
    
    // Check if octet is a valid number between 0 and 255
    int value = octet.toInt();
    if (value < 0 || value > 255)
      return false;
      
    // Avoid leading zeros (except for 0 itself)
    if (octet != "0" && octet.charAt(0) == '0')
      return false;
      
    lastPos = pos + 1;
  }
  
  return true;
}