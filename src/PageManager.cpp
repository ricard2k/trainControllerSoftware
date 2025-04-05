#include <Arduino.h>
#include "PageManager.h"
#include "PopupPage.h"
#include "InputPopupPage.h"
#include "DialogListPage.h"

std::stack<std::unique_ptr<IPage>> PageManager::pageStack;

void PageManager::pushPage(std::unique_ptr<IPage> page) {
    page->draw();
    pageStack.push(std::move(page));
}

void PageManager::showPopup(const String& message, std::function<void()> onClose) {
    IPage* current = currentPage();
    if (!current) return;

    auto* tft = current->getDisplay();
    if (!tft) return;

    auto popup = std::make_unique<PopupPage>(tft, message, onClose);
    pushPage(std::move(popup));
}



void PageManager::popPage() {
    if (!pageStack.empty()) {
        pageStack.pop(); // auto-deletes
        if (!pageStack.empty()) {
            pageStack.top()->draw();
        }
    }
}

IPage* PageManager::currentPage() {
    return pageStack.empty() ? nullptr : pageStack.top().get();
}

void PageManager::handleInput() {
    if (!pageStack.empty()) {
        pageStack.top()->handleInput();
    }
}

void PageManager::draw() {
    if (!pageStack.empty()) {
        pageStack.top()->draw();
    }
}

void PageManager::showInput(const String& prompt, InputMode mode,
    std::function<void(String, bool)> onComplete) {
    IPage* current = currentPage();
    if (!current) return;

    TFT_eSPI* tft = current->getDisplay();
    auto inputPage = std::make_unique<InputPopupPage>(tft, prompt, mode, onComplete);
    pushPage(std::move(inputPage));
}

void PageManager::showListDialog(
    const String& title,
    const std::vector<ListItem>& options,
    std::function<void(bool accepted, ListItem selected)> onResult) {

    IPage* current = currentPage();
    if (!current || options.empty()) return;

    auto dialog = std::make_unique<DialogListPage>(
        current->getDisplay(), title, options, onResult);

    pushPage(std::move(dialog));
}


