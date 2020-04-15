#include "CoffeeMakerTask.hpp"

#include <chrono>
#include <iostream>
#include <string>

#include "smooth/core/task_priorities.h"

//---------------------------------------------------------------------------
namespace esp32jura::jura {
//---------------------------------------------------------------------------
CoffeeMakerTask::CoffeeMakerTask() : Task("Coffee Task", 0, 1, std::chrono::seconds(1), 1) {}

void CoffeeMakerTask::init() {
    std::cout << "Initializing coffee maker...\n";
    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uartConfig));

    // Set UART pins(TX: IO16 (UART2 default), RX: IO17 (UART2 default), RTS:
    // IO18, CTS: IO19)
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT, UART_TX, UART_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    // Install UART driver using an event queue here
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, BUF_SIZE * 2, 0, 0, nullptr, 0));
    std::cout << "Coffee maker initialized.\n";
}

void CoffeeMakerTask::tick() {
    std::cout << "Coffee maker tick...\n";
    while (true) {
        uartWriteTest();
        uartReadTest();
    }
}

void CoffeeMakerTask::uartWriteTest() {
    std::string s = "Hallo";
    int len = uart_write_bytes(UART_PORT, s.c_str(), s.length());
}

void CoffeeMakerTask::uartReadTest() {
    size_t size = 0;
    ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_PORT, &size));
    if (size <= 0) {
        return;
    }

    uint8_t data[128];
    int len = uart_read_bytes(UART_PORT, data, size, 100);
    if (len < 0) {
        std::cerr << "Failed to read from UART." << std::endl;
    } else if (len > 0) {
        std::cout << "Read: " << len << " bytes\n";
    }
}

uint8_t CoffeeMakerTask::decode(std::array<uint8_t, 4> encData) {
    // Bit mask for the 2. bit from the left:
    constexpr uint8_t B2_MASK = (0b10000000 >> 2);
    // Bit mask for the 5. bit from the left:
    constexpr uint8_t B5_MASK = (0b10000000 >> 5);

    uint8_t decData = 0;
    decData |= (encData[0] & B2_MASK) << 2;
    decData |= (encData[0] & B5_MASK) << 4;

    decData |= (encData[1] & B2_MASK);
    decData |= (encData[1] & B5_MASK) << 2;

    decData |= (encData[2] & B2_MASK) >> 2;
    decData |= (encData[2] & B5_MASK);

    decData |= (encData[3] & B2_MASK) >> 4;
    decData |= (encData[3] & B5_MASK) << 2;
    return decData;
}

std::array<uint8_t, 4> CoffeeMakerTask::encode(uint8_t decData) {
    // The base bit layout for all send bytes:
    constexpr uint8_t BASE = 0b01111111;

    std::array<uint8_t, 4> encData{0, 0, 0, 0};
    encData[0] = BASE & ((decData & 0b10000000) >> 2);
    encData[0] = BASE & ((decData & 0b01000000) >> 4);

    encData[1] = BASE & (decData & 0b00100000);
    encData[1] = BASE & ((decData & 0b00010000) >> 1);

    encData[2] = BASE & ((decData & 0b00001000) << 2);
    encData[2] = BASE & (decData & 0b00000100);

    encData[3] = BASE & ((decData & 0b00000010) << 4);
    encData[3] = BASE & ((decData & 0b00000001) << 2);
    return std::move(encData);
}

void CoffeeMakerTask::writeEncData(std::array<uint8_t, 4> encData) { uart_write_bytes(UART_PORT, reinterpret_cast<char*>(encData.data()), 4); }

std::array<uint8_t, 4> CoffeeMakerTask::readEncData() { return std::array<uint8_t, 4>{0, 0, 0, 0}; }

//---------------------------------------------------------------------------
}  // namespace esp32jura::jura
//---------------------------------------------------------------------------
