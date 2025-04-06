# UI Page Architecture for Raspberry Pi Pico W + TFT

This document describes the architecture of a modular, stack-based UI system for a Raspberry Pi Pico W using a TFT display (via `TFT_eSPI`). The design is based around the concept of "pages" that encapsulate screen rendering and input logic.


## Overview

The UI is driven by a `PageManager` that handles a stack of UI `Pages`. Each screen (menu, popup, input dialog, list dialog, splash screen, loading screen, etc.) implements the `IPage` interface. The user navigates using up/down/left/right buttons, and transitions between pages are handled by pushing and popping the page stack.


## Core Components

| Component          | Description                                                                 |
|---------------------|-----------------------------------------------------------------------------|
| `IPage`            | Interface that defines `draw()`, `handleInput()`, and `getDisplay()`. All UI screens implement this. |
| `PageManager`      | Singleton-style controller that owns all pages using a `stack<unique_ptr<IPage>>`. Handles transitions, input routing, and popups. |
| `MenuPage`         | A concrete `IPage` that displays selectable items, each of which may trigger an action or open a submenu. |
| `MenuItem`         | An entry in a `MenuPage`, with an optional submenu and/or callback function. |
| `PopupPage`        | A modal page that displays a message overlay and dismisses on keypress. Optionally triggers a callback when closed. |
| `InputPopupPage`   | A modal page that allows the user to input text or numeric values. Supports callbacks for when input is completed or canceled. |
| `DialogListPage`   | A modal page that displays a list of selectable items. Supports callbacks for when an item is selected or the dialog is canceled. |
| `SplashPage`       | A page that displays an image (16-bit or 8-bit) for a specified duration. Used for branding or startup screens. |
| `LoadingPage`      | A page that displays a loading animation and optional message. Used for long-running operations. |
| `ThreadSafeTFT`    | A utility class that encapsulates the `TFT_eSPI` object and ensures thread-safe access using a FreeRTOS semaphore. |


## Navigation Flow

- The app starts by pushing a `MenuPage` onto the stack using `PageManager::pushPage()`.
- Input is routed to the current page (`PageManager::handleInput()` → `IPage::handleInput()`).
- Pages can push submenus or other pages via `PageManager::pushPage()`.
- `PageManager::popPage()` returns to the previous page.
- `PageManager::showPopup()` displays a temporary modal message page.
- `PageManager::showInput()` displays an input dialog for text or numeric input.
- `PageManager::showListDialog()` displays a list dialog for selecting an item.
- `PageManager::showSplash()` displays a splash screen for a specified duration.
- `PageManager::showLoading()` displays a loading animation with an optional message.


## Memory Management

- Pages are created with `std::make_unique<T>()` and passed to `PageManager`.
- `PageManager` owns and deletes pages automatically via `std::unique_ptr`.
- Pages do not manage their own lifetime — this is centralized.


## Typical Flow

1. Start at root menu.
2. User selects a submenu item → calls `PageManager::pushPage()`.
3. User selects an action item → `MenuItem.onSelect()` runs.
4. Action calls `PageManager::showPopup("Done!")`.
5. Popup dismissed → optional `onClose()` runs → returns to the previous page.
6. For input or list dialogs, `PageManager::showInput()` or `PageManager::showListDialog()` is called, and the result is handled via callbacks.
7. For splash screens, `PageManager::showSplash()` is called to display an image for a specified duration.
8. For loading animations, `PageManager::showLoading()` is called to display a loading screen, and `PageManager::hideLoading()` is called to dismiss it.


## ThreadSafeTFT: Encapsulating `TFT_eSPI` for Thread Safety

The `ThreadSafeTFT` class encapsulates the `TFT_eSPI` object and ensures that all interactions with it are thread-safe. This is achieved by protecting access to the `TFT_eSPI` object with a FreeRTOS semaphore.

### Key Features:
1. **Centralized Management**:
   - The `TFT_eSPI` object is managed centrally by `ThreadSafeTFT`, ensuring consistent access across the application.

2. **Thread Safety**:
   - All interactions with the `TFT_eSPI` object are protected by a FreeRTOS semaphore, preventing race conditions in a multitasking environment.

3. **Ease of Use**:
   - The `ThreadSafeTFT::withLock()` method allows users to safely execute operations on the `TFT_eSPI` object without worrying about thread safety.


## Class Diagram

```mermaid
classDiagram
    class IPage {
        <<interface>>
        +handleInput()
        +draw()
    }

    class PageManager {
        -pageStack: stack<unique_ptr<IPage>>
        +pushPage(page: unique_ptr<IPage>)
        +popPage()
        +currentPage(): IPage
        +handleInput()
        +draw()
        +showPopup(message: String, onClose: function)
        +showInput(prompt: String, mode: InputMode, onComplete: function)
        +showListDialog(title: String, options: vector<ListItem>, onResult: function)
        +showSplash(img16Bit: uint16_t*, w: int, h: int, duration: unsigned long)
        +showLoading(message: String)
        +hideLoading()
    }

    class MenuPage {
        -items: vector<MenuItem>
        -selectedIndex: int
        -scrollOffset: int
        -parentMenu: MenuPage*
        +addItem(label: String, submenu: unique_ptr<MenuPage>, onSelect: function)
        +moveUp()
        +moveDown()
        +enter()
        +back()
        +handleInput()
        +draw()
    }

    class MenuItem {
        -label: String
        -submenu: unique_ptr<MenuPage>
        -onSelect: function
    }

    class PopupPage {
        -message: String
        -onClose: function
        +handleInput()
        +draw()
    }

    class InputPopupPage {
        -prompt: String
        -mode: InputMode
        -onComplete: function
        +handleInput()
        +draw()
    }

    class DialogListPage {
        -title: String
        -options: vector<ListItem>
        -onResult: function
        +handleInput()
        +draw()
    }

    class SplashPage {
        -img16Bit: uint16_t*
        -img8Bit: uint8_t*
        -colmap: uint16_t*
        -bpp8: bool
        -w: int
        -h: int
        -duration: unsigned long
        +handleInput()
        +draw()
    }

    class LoadingPage {
        -message: String
        -angle: int
        -lastFrame: unsigned long
        -frameInterval: unsigned long
        +handleInput()
        +draw()
    }

    class ThreadSafeTFT {
        -tft: TFT_eSPI*
        -mutex: SemaphoreHandle_t
        +init(tftInstance: TFT_eSPI*)
        +withLock(action: function<void(TFT_eSPI&)>)
    }

    IPage <|.. MenuPage
    IPage <|.. PopupPage
    IPage <|.. InputPopupPage
    IPage <|.. DialogListPage
    IPage <|.. SplashPage
    IPage <|.. LoadingPage
    PageManager --> IPage : manages
    MenuPage --> MenuItem : has
    MenuItem --> MenuPage : owns submenu
    ThreadSafeTFT --> TFT_eSPI : encapsulates
    MenuPage --> ThreadSafeTFT : uses
    PopupPage --> ThreadSafeTFT : uses
    InputPopupPage --> ThreadSafeTFT : uses
    DialogListPage --> ThreadSafeTFT : uses
    SplashPage --> ThreadSafeTFT : uses
    LoadingPage --> ThreadSafeTFT : uses
