#pragma once

#include <vector>

#include "JuraConnection.hpp"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "smooth/core/Task.h"

//---------------------------------------------------------------------------
namespace esp32jura::jura {
//---------------------------------------------------------------------------
class JuraSnooperTask : public smooth::core::Task {
   private:
    static constexpr uart_port_t UART_PORT_COFFEE_MAKER = UART_NUM_1;
    static constexpr gpio_num_t UART_TX_COFFEE_MAKER = GPIO_NUM_23;
    static constexpr gpio_num_t UART_RX_COFFEE_MAKER = GPIO_NUM_25;

    static constexpr uart_port_t UART_PORT_DONGLE = UART_NUM_2;
    static constexpr gpio_num_t UART_TX_DONGLE = GPIO_NUM_14;
    static constexpr gpio_num_t UART_RX_DONGLE = GPIO_NUM_27;

    JuraConnection conCoffeeMaker;
    JuraConnection conDongle;

   public:
    JuraSnooperTask();

    void init() override;
    void tick() override;

   private:
    bool read_from_coffee_maker(std::vector<uint8_t>& buffer, bool inDongleReadMode);
    bool read_from_dongle(std::vector<uint8_t>& buffer, bool inDongleReadMode);
};
//---------------------------------------------------------------------------
}  // namespace esp32jura::jura
//---------------------------------------------------------------------------
