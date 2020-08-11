#pragma once

#include "CoffeeMaker.hpp"
#include "esp/sensor/Button.hpp"
#include "smooth/core/Task.h"

//---------------------------------------------------------------------------
namespace esp32jura::jura {
//---------------------------------------------------------------------------
class CoffeeMakerTask : public smooth::core::Task {
   private:
    CoffeeMaker coffeeMaker;

    static constexpr gpio_num_t BUTTON_SIGNAL = GPIO_NUM_13;
    esp::sensor::Button button;
    size_t buttonCounter{0x00};
    bool buttonPressed{false};

   public:
    CoffeeMakerTask();

    void init() override;
    void tick() override;

   private:
    void read();
    void write();
};
//---------------------------------------------------------------------------
}  // namespace esp32jura::jura
//---------------------------------------------------------------------------
