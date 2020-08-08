#pragma once

#include <smooth/core/io/Output.h>

#include "driver/gpio.h"

//---------------------------------------------------------------------------
namespace esp32jura::esp::actuators {
//---------------------------------------------------------------------------
class RgbLed {
   public:
    smooth::core::io::Output r;
    smooth::core::io::Output g;
    smooth::core::io::Output b;

    RgbLed(gpio_num_t r, gpio_num_t g, gpio_num_t b);

    void turnOnOnly(smooth::core::io::Output& led);
    void turnOffOnly(smooth::core::io::Output& led);

    void turnOn(smooth::core::io::Output& led);
    void turnOff(smooth::core::io::Output& led);
};

//---------------------------------------------------------------------------
}  // namespace esp32jura::esp::actuators
   //---------------------------------------------------------------------------