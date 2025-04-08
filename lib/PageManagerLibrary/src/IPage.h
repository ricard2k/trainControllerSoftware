#pragma once
#include <memory>
#include <IKeyboard.h>


class TFT_eSPI;

class IPage {
public:
    virtual void handleInput(IKeyboard* keyboard) = 0;
    virtual void draw() = 0;
    virtual ~IPage() = default;
};
