# 🧭 UI Page Architecture for Raspberry Pi Pico W + TFT

This document describes the architecture of a modular, stack-based UI system for a Raspberry Pi Pico W using a TFT display (via `TFT_eSPI`). The design is based around the concept of "pages" that encapsulate screen rendering and input logic.

---

## 🌐 Overview

The UI is driven by a `PageManager` that handles a stack of UI `Pages`. Each screen (menu, popup, input dialog, list dialog, etc.) implements the `IPage` interface. The user navigates using up/down/left/right buttons, and transitions between pages are handled by pushing and popping the page stack.

---

## 🧱 Core Components

| Component          | Description                                                                 |
|---------------------|-----------------------------------------------------------------------------|
| `IPage`            | Interface that defines `draw()`, `handleInput()`, and `getDisplay()`. All UI screens implement this. |
| `PageManager`      | Singleton-style controller that owns all pages using a `stack<unique_ptr<IPage>>`. Handles transitions, input routing, and popups. |
| `MenuPage`         | A concrete `IPage` that displays selectable items, each of which may trigger an action or open a submenu. |
| `MenuItem`         | An entry in a `MenuPage`, with an optional submenu and/or callback function. |
| `PopupPage`        | A modal page that displays a message overlay and dismisses on keypress. Optionally triggers a callback when closed. |
| `InputPopupPage`   | A modal page that allows the user to input text or numeric values. Supports callbacks for when input is completed or canceled. |
| `DialogListPage`   | A modal page that displays a list of selectable items. Supports callbacks for when an item is selected or the dialog is canceled. |

---

## 🔁 Navigation Flow

- The app starts by pushing a `MenuPage` onto the stack using `PageManager::pushPage()`.
- Input is routed to the current page (`PageManager::handleInput()` → `IPage::handleInput()`).
- Pages can push submenus or other pages via `PageManager::pushPage()`.
- `PageManager::popPage()` returns to the previous page.
- `PageManager::showPopup()` displays a temporary modal message page.
- `PageManager::showInput()` displays an input dialog for text or numeric input.
- `PageManager::showListDialog()` displays a list dialog for selecting an item.
- Any page can now call `getDisplay()` to pass the screen reference to a popup or dialog.

---

## 💡 Memory Management

- Pages are created with `std::make_unique<T>()` and passed to `PageManager`.
- `PageManager` owns and deletes pages automatically via `std::unique_ptr`.
- Pages do not manage their own lifetime — this is centralized.

---

## 🔄 Typical Flow

1. Start at root menu.
2. User selects a submenu item → calls `PageManager::pushPage()`.
3. User selects an action item → `MenuItem.onSelect()` runs.
4. Action calls `PageManager::showPopup("Done!")`.
5. Popup dismissed → optional `onClose()` runs → returns to the previous page.
6. For input or list dialogs, `PageManager::showInput()` or `PageManager::showListDialog()` is called, and the result is handled via callbacks.

---

## 📊 Class Diagram

```mermaid
classDiagram
    class IPage {
        <<interface>>
        +handleInput()
        +draw()
        +getDisplay(): TFT_eSPI*
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
    }

    class MenuPage {
        -tft: TFT_eSPI*
        -inputPins: MenuInputPins
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
        +getDisplay(): TFT_eSPI*
    }

    class MenuItem {
        -label: String
        -submenu: unique_ptr<MenuPage>
        -onSelect: function
    }

    class PopupPage {
        -tft: TFT_eSPI*
        -message: String
        -onClose: function
        +handleInput()
        +draw()
        +getDisplay(): TFT_eSPI*
    }

    class InputPopupPage {
        -tft: TFT_eSPI*
        -prompt: String
        -mode: InputMode
        -onComplete: function
        +handleInput()
        +draw()
        +getDisplay(): TFT_eSPI*
    }

    class DialogListPage {
        -tft: TFT_eSPI*
        -title: String
        -options: vector<ListItem>
        -onResult: function
        +handleInput()
        +draw()
        +getDisplay(): TFT_eSPI*
    }

    IPage <|.. MenuPage
    IPage <|.. PopupPage
    IPage <|.. InputPopupPage
    IPage <|.. DialogListPage
    PageManager --> IPage : manages
    MenuPage --> MenuItem : has
    MenuItem --> MenuPage : owns submenu
    MenuPage --> TFT_eSPI
    PopupPage --> TFT_eSPI
    InputPopupPage --> TFT_eSPI
    DialogListPage --> TFT_eSPI
