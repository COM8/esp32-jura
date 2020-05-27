#pragma once

#include <array>

#include "JuraConnection.hpp"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "smooth/core/Task.h"

//---------------------------------------------------------------------------
namespace esp32jura::jura {
//---------------------------------------------------------------------------
class CoffeeMakerTask : public smooth::core::Task {
   private:
    static constexpr uart_port_t UART_PORT = UART_NUM_1;
    static constexpr gpio_num_t UART_TX = GPIO_NUM_14;
    static constexpr gpio_num_t UART_RX = GPIO_NUM_27;

    bool on{true};

    JuraConnection connection;

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
