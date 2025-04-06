# PageManagerLibrary

## Overview

The **PageManagerLibrary** is a modular UI framework designed for the **Raspberry Pi Pico** using the **Arduino framework** provided by the **earlephilhower core**. It leverages the **TFT_eSPI** library for driving TFT displays and includes enhancements to make the library thread-safe in an RTOS environment.

This library provides a stack-based page management system, allowing for the creation of dynamic and interactive user interfaces. It also includes a `ThreadSafeTFT` wrapper to ensure safe usage of the `TFT_eSPI` library in multitasking environments.

---

## Features

1. **Raspberry Pi Pico Support**:
   - Built for the **earlephilhower Arduino core**.

2. **TFT_eSPI Integration**:
   - Uses the **TFT_eSPI** library for high-performance TFT display rendering.

3. **RTOS Compatibility**:
   - Designed to work seamlessly with FreeRTOS included in earlephilhower core.
   - Includes a `ThreadSafeTFT` wrapper to make `TFT_eSPI` thread-safe using a FreeRTOS semaphore.

4. **Stack-Based Page Management**:
   - A `PageManager` handles navigation between different UI pages.
   - Includes prebuilt pages like `MenuPage`, `PopupPage`, `InputPopupPage`, `DialogListPage`, `SplashPage`, and `LoadingPage`.

5. **Customizable UI**:
   - Easily create custom pages by implementing the `IPage` interface.

---

## Prebuilt Pages

The library includes the following prebuilt pages:

1. **MenuPage**:
   - Displays a list of selectable items.
   - Supports hierarchical menus with submenus and actions.

2. **PopupPage**:
   - Displays a temporary modal message overlay.

3. **InputPopupPage**:
   - Allows the user to input text or numeric values.

4. **DialogListPage**:
   - Displays a list of selectable items in a modal dialog.

5. **SplashPage**:
   - Displays an image for a specified duration (e.g., branding or startup screens).

6. **LoadingPage**:
   - Displays a loading animation with an optional message.

---

## How to Use

1. **Include the Library**:
   - Add the library to your project via `lib_deps` in `platformio.ini`:
     ```ini
     lib_deps =
       lib/PageManagerLibrary
     ```

2. **Initialize the PageManager**:
   - Push the initial page (e.g., `MenuPage`) to the `PageManager` stack.

3. **Use Prebuilt Pages**:
   - Use the provided pages for common UI elements like menus, popups, and dialogs.

4. **Create Custom Pages**:
   - Implement the `IPage` interface to define custom behavior.

---

## Creating Custom Pages

You can extend the UI system by creating custom pages. Implement the `IPage` interface to define custom behavior for rendering and handling input.

### Example:
```cpp
class CustomPage : public IPage {
private:
    String message;

public:
    CustomPage(const String& msg) : message(msg) {}
    void handleInput() override {
        if (digitalRead(PAGE_LIBRARY_BTN_OK) == LOW) {
            PageManager::popPage();
        }
    }
    void draw() override {
        ThreadSafeTFT::withLock([this](TFT_eSPI& tft) {
            tft.fillScreen(TFT_BLACK);
            tft.setTextColor(TFT_WHITE);
            tft.drawCentreString(message, tft.width() / 2, tft.height() / 2, 2);
        });
    }
};
```

---

## Thread-Safe TFT Access

The `ThreadSafeTFT` class encapsulates the `TFT_eSPI` object and ensures thread-safe access using a FreeRTOS semaphore. This allows multiple tasks to safely interact with the display without causing race conditions.

### Example Usage:
```cpp
ThreadSafeTFT::withLock([](TFT_eSPI& tft) {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.drawCentreString("Hello, World!", 120, 160, 2);
});
```

---

## License

This library is licensed under the MIT License. See the `LICENSE` file for details.

---

## Acknowledgments

- **TFT_eSPI** by Bodmer for the excellent TFT display library.
- **earlephilhower** for the Raspberry Pi Pico Arduino core.

