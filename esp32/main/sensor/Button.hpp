#pragma once

#include <smooth/core/io/Input.h>

#include "driver/gpio.h"

//---------------------------------------------------------------------------
namespace esp32jura::sensor {
//---------------------------------------------------------------------------
class Button {
   public:
    smooth::core::io::Input signal;

    explicit Button(gpio_num_t signal);

    bool isPressed();
};
//---------------------------------------------------------------------------
}  // namespace esp32jura::sensor
   //---------------------------------------------------------------------------