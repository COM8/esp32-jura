#pragma once

#include <string>
#include <vector>

#include "esp/serial/SerialConnection.hpp"
#include "jura/JuraConnection.hpp"
#include "smooth/core/Task.h"

//---------------------------------------------------------------------------
namespace esp32jura::utils {
//---------------------------------------------------------------------------
class SerialJuraBridgeTask : public smooth::core::Task {
   private:
    esp::serial::SerialConnection serial_connection;

    static constexpr uart_port_t UART_PORT = UART_NUM_1;
    static constexpr gpio_num_t UART_TX = GPIO_NUM_14;
    static constexpr gpio_num_t UART_RX = GPIO_NUM_27;
    jura::JuraConnection jura_connection;

   public:
    SerialJuraBridgeTask();

    void init() override;
    void tick() override;

   private:
    std::string to_binaryString(const std::vector<uint8_t>& buffer);
};
//---------------------------------------------------------------------------
}  // namespace esp32jura::utils
//---------------------------------------------------------------------------
