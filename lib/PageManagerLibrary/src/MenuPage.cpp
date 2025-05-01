#include <memory.h>
#include "MenuPage.h"
#include "PageManager.h"
#include "LibraryConfig.h"
#include "ThreadSafeTFT.h"

MenuItem::MenuItem(String l, std::unique_ptr<MenuPage> sub, std::function<void()> cb)
    : label(l), submenu(std::move(sub)), onSelect(cb) {}

std::unique_ptr<MenuPage> MenuItem::cloneSubmenu() const {
  if (!submenu) return nullptr;
  
  auto clone = std::make_unique<MenuPage>(nullptr);
  
  // Copy all the items
  for (const auto& item : submenu->items) {
      clone->addItem(item.label, item.cloneSubmenu(), item.onSelect);
  }
  
  return clone;
}

MenuPage::MenuPage(MenuPage *parent)
    : parentMenu(parent) {}

void MenuPage::addItem(String label, std::unique_ptr<MenuPage> submenu, std::function<void()> onSelect)
{
  items.emplace_back(label, std::move(submenu), onSelect);
}

void MenuPage::moveUp()
{
  if (selectedIndex > 0)
  {
    selectedIndex--;
    if (selectedIndex < scrollOffset)
    {
      scrollOffset--;
    }
    draw();
  }
}

void MenuPage::moveDown()
{
  if (selectedIndex < items.size() - 1)
  {
    selectedIndex++;
    if (selectedIndex >= scrollOffset + MAX_VISIBLE_ITEMS)
    {
      scrollOffset++;
    }
    draw();
  }
}

void MenuPage::enter()
{
  MenuItem &item = items[selectedIndex];
  if (item.submenu)
  {
    // Clone the submenu structure instead of moving it
    auto submenuClone = item.cloneSubmenu();
    PageManager::pushPage(std::move(submenuClone));
  }
  else if (item.onSelect)
  {
    item.onSelect();
  }
}

void MenuPage::back()
{
  PageManager::popPage();
}

void MenuPage::draw()
{
  ThreadSafeTFT::withLock([this](TFT_eSPI& tft) {
    tft.fillScreen(TFT_BLACK);
  });

  int visibleItems = std::min((int)items.size(), MAX_VISIBLE_ITEMS);

  for (int i = 0; i < visibleItems; ++i)
  {
    int itemIndex = scrollOffset + i;
      if (itemIndex >= items.size()) break;
    ThreadSafeTFT::withLock([this, i, itemIndex](TFT_eSPI& tft) {
      if (itemIndex == selectedIndex)
      {
        tft.setTextColor(TFT_BLACK, TFT_WHITE);
      }
      else
      {
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
      }

      tft.drawString(items[itemIndex].label, 10, 20 + i * ITEM_HEIGHT, 2);
    });
  }

  // Draw scrollbar if needed
  if (items.size() > MAX_VISIBLE_ITEMS)
  {
    ThreadSafeTFT::withLock([this](TFT_eSPI& tft) {
      int scrollbarHeight = MAX_VISIBLE_ITEMS * ITEM_HEIGHT;
      int scrollbarTop = 20;
      int thumbHeight = (MAX_VISIBLE_ITEMS * scrollbarHeight) / items.size();
      int thumbY = scrollbarTop + (selectedIndex * scrollbarHeight) / items.size();

      int scrollbarX = PAGE_LIBRARY_SCREEN_WIDTH - SCROLLBAR_WIDTH;
      tft.fillRect(scrollbarX, scrollbarTop, SCROLLBAR_WIDTH, scrollbarHeight, TFT_DARKGREY);
      tft.fillRect(scrollbarX, thumbY, SCROLLBAR_WIDTH, thumbHeight, TFT_WHITE);
    });
  }
}

void MenuPage::handleInput(IKeyboard* keyboard)
{
  uint16_t pressedKeys = keyboard->getPressedKeys();
  if (pressedKeys & KEY_UP)
  {
    moveUp();
    delay(200);
  }
  else if (pressedKeys & KEY_DOWN)
  {
    moveDown();
    delay(200);
  }
  else if ((pressedKeys & KEY_RIGHT) ||
           (pressedKeys & KEY_OK))
    {
      enter();
      delay(200);
    }
  else if (pressedKeys & KEY_LEFT)
  {
    back();
    delay(200);
  }
}

