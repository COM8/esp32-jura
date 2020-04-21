#pragma once

#include <array>
#include <optional>

#include "driver/gpio.h"
#include "driver/uart.h"
#include "smooth/core/Task.h"

//---------------------------------------------------------------------------
namespace esp32jura::jura {
//---------------------------------------------------------------------------
class CoffeeMakerTask : public smooth::core::Task {
   private:
    uart_config_t uartConfig{
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    static constexpr int BUF_SIZE = 1024;
    static constexpr uart_port_t UART_PORT = UART_NUM_1;
    static constexpr gpio_num_t UART_TX = GPIO_NUM_4;
    static constexpr gpio_num_t UART_RX = GPIO_NUM_5;

   public:
    CoffeeMakerTask();

    void init() override;
    void tick() override;

   private:
    void writeToCoffeeMaker(std::string s);
    void readFromCoffeeMaker();

    void testEncodeDecode();
    void printByte(const uint8_t& b);

    /**
     * Decodes the given four bytes read from the coffee maker into on byte.
     * Based on: http://protocoljura.wiki-site.com/index.php/Protocol_to_coffeemaker
     **/
    uint8_t decode(std::array<uint8_t, 4> encData);
    /**
     * Encodes the given byte into four bytes that the coffee maker understands.
     * Based on: http://protocoljura.wiki-site.com/index.php/Protocol_to_coffeemaker
     **/
    std::array<uint8_t, 4> encode(uint8_t decData);
    /**
     * Writes four bytes of data to the coffee maker and then waits 8ms.
     **/
    void writeEncData(std::array<uint8_t, 4> encData);
    /**
     * Reads four bytes of data from the coffee maker which represent one byte of actual data.
     **/
    std::optional<std::array<uint8_t, 4>> readEncData();

    uint8_t reverse(uint8_t b);
};
//---------------------------------------------------------------------------
}  // namespace esp32jura::jura
//---------------------------------------------------------------------------
