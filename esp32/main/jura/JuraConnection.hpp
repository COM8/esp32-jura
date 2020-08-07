#pragma once

#include <array>
#include <string>
#include <vector>

#include "driver/gpio.h"
#include "driver/uart.h"

//---------------------------------------------------------------------------
namespace esp32jura::jura {
//---------------------------------------------------------------------------
class JuraConnection {
   private:
    /**
     * Default UART connection configuration.
     * More information can be found here:
     * https://github.com/COM8/esp32-jura#connecting-to-an-jura-coffee-maker
     **/
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
    /**
     * Initializes a new JURA (UART) connection.
     *
     * Suggested values for the UART pins are:
     * UART_PORT_NUM: UART_NUM_1
     * UART_TX: GPIO_NUM_23
     * UART_RX: GPIO_NUM_25
     *
     * or
     *
     * UART_PORT_NUM: UART_NUM_2
     * UART_TX: GPIO_NUM_14
     * UART_RX: GPIO_NUM_27
     **/
    JuraConnection(uart_port_t uart_port_num, gpio_num_t uart_tx, gpio_num_t uart_rx);

    /**
     * Initializes UART connection.
     **/
    void init();

    /**
     * Tries to read a single decoded byte.
     * This requires reading 4 JURA bytes and converting them to a single actual data byte.
     * The result will be stored in the given "byte" pointer.
     * Returns true on success.
     **/
    bool read_decoded(uint8_t* byte);
    /**
     * Reads as many data bytes, as there are availabel.
     * Each data byte consists of 4 JURA bytes which will be decoded into a single data byte.
     **/
    bool read_decoded(std::vector<uint8_t>& data);

    /**
     * Encodes the given byte into 4 JURA bytes and writes them to the coffee maker.
     **/
    bool write_decoded(const uint8_t& byte);
    /**
     * Encodes each byte of the given bytes into 4 JURA bytes and writes them to the coffee maker.
     **/
    bool write_decoded(const std::vector<uint8_t>& data);
    /**
     * Encodes each character into 4 JURA bytes and writes them to the coffee maker.
     *
     * An example call could look like: write_decoded("TY:\r\n");
     * This would request the device type from the coffee maker.
     **/
    bool write_decoded(const std::string& data);

    /**
     * Helper function used for debugging.
     * Prints the given byte in binary, hex and as a char.
     * Does not append a new line at the end!
     *
     * Example output:
     * 0 1 0 1 0 1 0 0 -> 84	54	T
     **/
    static void print_byte(const uint8_t& byte);
    /**
     * Prints each byte in the given vector in binary, hex and as a char
     *
     * Example output:
     * 0 1 0 1 0 1 0 0 -> 84	54	T
     * 0 1 0 1 1 0 0 1 -> 89	59	Y
     * 0 0 1 1 1 0 1 0 -> 58	3a	:
     * 0 0 0 0 1 1 0 1 -> 13	0d
     * 0 0 0 0 1 0 1 0 -> 10	0a
     **/
    static void print_bytes(const std::vector<uint8_t>& data);

    /**
     * Runs the encode and decode test.
     * Ensures encoding and decoding is reversable.
     * Should be run at least once per session to ensure proper functionality.
     **/
    static void run_encode_decode_test();

   private:
    /**
     * Encodes the given byte into four bytes that the coffee maker understands.
     * Based on: http://protocoljura.wiki-site.com/index.php/Protocol_to_coffeemaker
     *
     * A full documentation of the process can be found here:
     * https://github.com/COM8/esp32-jura#deobfuscating
     **/
    static std::array<uint8_t, 4> encode(const uint8_t& decData);
    /**
     * Decodes the given four bytes read from the coffee maker into on byte.
     * Based on: http://protocoljura.wiki-site.com/index.php/Protocol_to_coffeemaker
     *
     * A full documentation of the process can be found here:
     * https://github.com/COM8/esp32-jura#deobfuscating
     **/
    static uint8_t decode(const std::array<uint8_t, 4>& encData);
    /**
     * Writes four bytes of encoded data to the coffee maker and then waits 8ms.
     **/
    bool write_encoded(const std::array<uint8_t, 4>& encData);
    /**
     * Reads four bytes of encoded data which represent one byte of actual data.
     * Returns true on success.
     **/
    bool read_encoded(std::array<uint8_t, 4> buffer);
    /**
     * Reads multiples of four bytes. Every four bytes represent one actual byte.
     * Returns the number of 4 byte tuples read.
     **/
    size_t read_encoded(std::vector<std::array<uint8_t, 4>> data);
};
//---------------------------------------------------------------------------
}  // namespace esp32jura::jura
//---------------------------------------------------------------------------
