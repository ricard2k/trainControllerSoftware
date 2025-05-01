#pragma once

#include <IKeyboard.h>

// Extend the Key enum with additional values
namespace ExtendedKeys {
    enum {
        KEY_TIGHT_BRAKE = 32,   // Weight: 32
        KEY_RELEASE_BRAKE = 64, // Weight: 64
        
        // You can add more keys if needed
        KEY_HORN = 128,         // Weight: 128
        KEY_BELL = 256          // Weight: 256
    };
}