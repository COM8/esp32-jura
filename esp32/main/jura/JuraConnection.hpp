#pragma once

#include <array>
#include <vector>

#include "driver/gpio.h"
#include "driver/uart.h"

//---------------------------------------------------------------------------
namespace esp32jura::jura {
//---------------------------------------------------------------------------
class JuraConnection {
   private:
    uart_config_t uartConfig{
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0,
        .use_ref_tick = false,
    };
    static constexpr int BUF_SIZE = 1024;
    const uart_port_t UART_PORT;
    const gpio_num_t UART_TX;
    const gpio_num_t UART_RX;

   public:
    JuraConnection(uart_port_t uart_port_num, gpio_num_t uart_tx, gpio_num_t uart_rx);

    void init();

    bool read_decoded(uint8_t* byte);
    bool read_decoded(std::vector<uint8_t>* data);

    bool write_decoded(const uint8_t& byte);
    bool write_decoded(const std::vector<uint8_t>& data);

    static void print_byte(const uint8_t& byte);
    static void print_bytes(const std::vector<uint8_t>& data);

    static void run_encode_decode_test();

   private:
    /**
     * Encodes the given byte into four bytes that the coffee maker understands.
     * Based on: http://protocoljura.wiki-site.com/index.php/Protocol_to_coffeemaker
     **/
    static std::array<uint8_t, 4> encode(const uint8_t& decData);
    /**
     * Decodes the given four bytes read from the coffee maker into on byte.
     * Based on: http://protocoljura.wiki-site.com/index.php/Protocol_to_coffeemaker
     **/
    static uint8_t decode(const std::array<uint8_t, 4>& encData);
    /**
     * Writes four bytes of data to the coffee maker and then waits 8ms.
     **/
    bool write_encoded(const std::array<uint8_t, 4>& encData);
    /**
     * Reads four bytes of data which represent one byte of actual data.
     * Returns true on success.
     **/
    bool read_encoded(std::array<uint8_t, 4>* buffer);
    /**
     * Reads multiples of four bytes. Every four bytes represent one actual byte.
     * Returns the number of 4 byte tuples read.
     **/
    size_t read_encoded(std::vector<std::array<uint8_t, 4>>* data);

    /**
     * Reverses the bitorder of the given byte.
     * For example: 01010101 -> 10101010
     **/
    uint8_t reverse(const uint8_t& byte);
};
//---------------------------------------------------------------------------
}  // namespace esp32jura::jura
//---------------------------------------------------------------------------
