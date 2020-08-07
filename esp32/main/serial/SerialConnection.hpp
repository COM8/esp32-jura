#pragma once

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
    static constexpr uart_port_t UART_PORT = UART_NUM_0;
    static constexpr gpio_num_t UART_TX = GPIO_NUM_1;
    static constexpr gpio_num_t UART_RX = GPIO_NUM_3;
    static constexpr int BUF_SIZE = 2048;
    /**
     * Use std::vector instead of std::array to save on stack space.
     * std::array allocates memory on the stack.
     **/
    std::vector<uint8_t> read_buffer;

    SerialConnectionState state{SC_DISABLED};

   public:
    SerialConnection();

    void init() override;
    void tick() override;

   private:
    std::string read();
    /**
     * Reads a single byte from the uart buffer and appends it to the "read_buffer".
     * Returns true if the byte is a new line character ('\n').
     **/
    bool read_byte();
    bool data_available();
    size_t write(const std::vector<uint8_t>& data);
    size_t write(const std::string& data);
    static std::string vec_to_string(const std::vector<uint8_t>& data);
};
//---------------------------------------------------------------------------
}  // namespace esp32jura::serial
//---------------------------------------------------------------------------
