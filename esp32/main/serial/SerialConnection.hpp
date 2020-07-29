#pragma once

#include <array>
#include <string>
#include <vector>

#include "driver/gpio.h"
#include "driver/uart.h"
#include "smooth/core/Task.h"

//---------------------------------------------------------------------------
namespace esp32jura::serial {
//---------------------------------------------------------------------------
class SerialConnection : public smooth::core::Task {
   public:
    enum SerialConnectionState { SC_DISABLED = 0, SC_INIT = 1, SC_READY = 2, SC_ERROR = 3 };

   private:
    uart_config_t uartConfig{
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
        .use_ref_tick = false,
    };
    static constexpr int BUF_SIZE = 2048;
    static constexpr uart_port_t UART_PORT = UART_NUM_0;
    static constexpr gpio_num_t UART_TX = GPIO_NUM_1;
    static constexpr gpio_num_t UART_RX = GPIO_NUM_3;
    std::array<uint8_t, BUF_SIZE> read_buffer{};

    SerialConnectionState state{SC_DISABLED};

   public:
    SerialConnection();

    void init() override;
    void tick() override;

   private:
    std::string read();
    size_t write(const std::vector<uint8_t>& data);
    size_t write(const std::string& data);
};
//---------------------------------------------------------------------------
}  // namespace esp32jura::serial
//---------------------------------------------------------------------------
