#pragma once

#include <string>
#include <vector>

#include "jura/JuraConnection.hpp"
#include "serial/SerialConnection.hpp"
#include "smooth/core/Task.h"

//---------------------------------------------------------------------------
namespace esp32jura {
//---------------------------------------------------------------------------
class SerialJuraBridgeTask : public smooth::core::Task {
   private:
    serial::SerialConnection serial_connection;

    static constexpr uart_port_t UART_PORT = UART_NUM_1;
    static constexpr gpio_num_t UART_TX = GPIO_NUM_14;
    static constexpr gpio_num_t UART_RX = GPIO_NUM_27;
    jura::JuraConnection jura_connection;

   public:
    SerialJuraBridgeTask();

    void init() override;
    void tick() override;
};
//---------------------------------------------------------------------------
}  // namespace esp32jura
//---------------------------------------------------------------------------
