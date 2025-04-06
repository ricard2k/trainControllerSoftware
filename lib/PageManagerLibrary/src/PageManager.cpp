#include <Arduino.h>
#include "PageManager.h"
#include "PopupPage.h"
#include "InputPopupPage.h"
#include "DialogListPage.h"
#include "LoadingPage.h"
#include "SplashPage.h"

std::stack<std::unique_ptr<IPage>> PageManager::pageStack;

void PageManager::pushPage(std::unique_ptr<IPage> page) {
    page->draw();
    pageStack.push(std::move(page));
}

void PageManager::showPopup(const String& message, std::function<void()> onClose) {
    auto popup = std::make_unique<PopupPage>(message, onClose);
    pushPage(std::move(popup));
}



void PageManager::popPage() {
    if (pageStack.size() > 1) { // Ensure at least one page remains
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
    auto inputPage = std::make_unique<InputPopupPage>(prompt, mode, onComplete);
    pushPage(std::move(inputPage));
}

void PageManager::showListDialog(
    const String& title,
    const std::vector<ListItem>& options,
    std::function<void(bool accepted, ListItem selected)> onResult) {

    auto dialog = std::make_unique<DialogListPage>(title, options, onResult);

    pushPage(std::move(dialog));
}

static std::unique_ptr<LoadingPage> loadingPageInstance;

void PageManager::showLoading(const String& message) {
    loadingPageInstance = std::make_unique<LoadingPage>(message);
    pushPage(std::move(loadingPageInstance));
}

void PageManager::hideLoading() {
    // Assumes the loading page is topmost
    popPage();
    loadingPageInstance.reset();
}

void PageManager::showSplash(const uint16_t* img16Bit, int w, int h, unsigned long duration) {
    
    auto splash = std::make_unique<SplashPage>(img16Bit, nullptr, false, nullptr, w, h, duration);
 
    pushPage(std::move(splash));
}

void PageManager::showSplash(const uint8_t* img8Bit, bool bpp8, const uint16_t* colmap, int w, int h, unsigned long duration) {
    auto splash = std::make_unique<SplashPage>(nullptr, img8Bit, bpp8, colmap, w, h, duration);
    pushPage(std::move(splash));
}


