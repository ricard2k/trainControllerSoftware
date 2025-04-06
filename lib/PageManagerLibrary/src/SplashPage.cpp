#include "SplashPage.h"
#include "PageManager.h"
#include "ThreadSafeTFT.h"

SplashPage::SplashPage(const uint16_t* img16bit, const uint8_t* img8bit, bool bpp8, const uint16_t* colmap, int w, int h, unsigned long durationMs)
    : image16bits(img16bit), image8bits(img8bit), bpp8(bpp8), colmap(colmap), imgWidth(w), imgHeight(h), duration(durationMs), startTime(millis()) {}


void SplashPage::draw() {
    ThreadSafeTFT::withLock([this](TFT_eSPI& tft) {
        tft.fillScreen(TFT_BLACK);
        int x = (PAGE_LIBRARY_SCREEN_WIDTH - this->imgWidth) / 2;
        int y = (PAGE_LIBRARY_SCREEN_HEIGHT - this->imgHeight) / 2;

        if (image8bits == nullptr) {
            tft.pushImage(x, y, this->imgWidth, this->imgHeight, this->image16bits);
        } else {
            tft.pushImage(x, y, this->imgWidth, this->imgHeight, const_cast<uint8_t*>(this->image8bits), bpp8, const_cast<uint16_t*>(this->colmap));
        }
    });
}

void SplashPage::handleInput() {
    if (millis() - startTime > duration) {
        PageManager::popPage();
    }
}