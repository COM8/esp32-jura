#include "CoffeeMakerTask.hpp"

#include <stdlib.h>

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "smooth/core/task_priorities.h"

//---------------------------------------------------------------------------
namespace esp32jura::jura {
//---------------------------------------------------------------------------
CoffeeMakerTask::CoffeeMakerTask() : Task("Coffee Task", 0, 1, std::chrono::seconds(2), 1) {}

void CoffeeMakerTask::init() {
    std::cout << "Initializing coffee maker...\n";
    testEncodeDecode();
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
    // writeToCoffeeMaker("TY:\r\n");
    std::string s;
    for (size_t i = 0; i < 8; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds{8});
        s += readFromCoffeeMaker();
    }
    if (!s.empty()) {
        std::cout << "-----Read " << s.length() << " bytes-----\n";
        std::cout << s;
        std::cout << "\n----------\n";
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
    decData |= (encData[3] & B5_MASK) >> 2;
    return decData;
}

std::array<uint8_t, 4> CoffeeMakerTask::encode(uint8_t decData) {
    // The base bit layout for all send bytes:
    constexpr uint8_t BASE = 0b01011011;

    std::array<uint8_t, 4> encData;
    encData[0] = BASE | ((decData & 0b10000000) >> 2);
    encData[0] |= ((decData & 0b01000000) >> 4);

    encData[1] = BASE | (decData & 0b00100000);
    encData[1] |= ((decData & 0b00010000) >> 2);

    encData[2] = BASE | ((decData & 0b00001000) << 2);
    encData[2] |= (decData & 0b00000100);

    encData[3] = BASE | ((decData & 0b00000010) << 4);
    encData[3] |= ((decData & 0b00000001) << 2);

    return std::move(encData);
}

void CoffeeMakerTask::writeEncData(std::array<uint8_t, 4> encData) {
    uart_write_bytes(UART_PORT, reinterpret_cast<char*>(encData.data()), 4);
    std::this_thread::sleep_for(std::chrono::milliseconds{8});
}

std::optional<std::array<uint8_t, 4>> CoffeeMakerTask::readEncData() {
    size_t size = 0;
    ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_PORT, &size));
    if (size < 4) {
        return std::nullopt;
    }

    uint8_t buf[4];
    // Wait 8ms for the data to arrive:
    int len = uart_read_bytes(UART_PORT, buf, 4, 8 / portTICK_PERIOD_MS);
    if (len < 0) {
        std::cerr << "Failed to read from UART." << std::endl;
        return std::nullopt;
    }
    // std::cout << "Read: " << len << " bytes\n";
    return std::array<uint8_t, 4>{buf[0], buf[1], buf[2], buf[3]};
}

void CoffeeMakerTask::writeToCoffeeMaker(std::string s) {
    std::array<uint8_t, 4> data;
    for (char c : s) {
        data = encode(static_cast<uint8_t>(c));
        writeEncData(data);
    }
}

std::string CoffeeMakerTask::readFromCoffeeMaker() {
    // Wait 8 ms for the next bunch of data to arrive:
    std::this_thread::sleep_for(std::chrono::milliseconds{8});

    // Check if data is available:
    size_t size = 0;
    ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_PORT, &size));
    if (size <= 0) {
        return "";
    }

    // If less then 4 bytes are availabel wait again and then try again:
    if (size < 4) {
        std::this_thread::sleep_for(std::chrono::milliseconds{8});
        ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_PORT, &size));
        if (size < 4) {
            std::cerr << "Invalid amount of UART data found (" << size << " byte) - flushing.\n";
            uart_flush(UART_PORT);
            return "";
        }
        return readFromCoffeeMaker();
    }

    std::optional<std::array<uint8_t, 4>> data = readEncData();
    printByte(decode(*data));
    if (!data) {
        return "";
    }
    return std::string(1, static_cast<char>(decode(*data))) + readFromCoffeeMaker();
}

void CoffeeMakerTask::testEncodeDecode() {
    bool success = true;

    for (uint16_t i = 0b00000000; i <= 0b11111111; i++) {
        if (i != decode(encode(i))) {
            success = false;
            std::cout << "data: ";
            printByte(i);

            std::array<uint8_t, 4> dataEnc = encode(i);
            for (size_t i = 0; i < 4; i++) {
                std::cout << "dataEnc[" << i << "]: ";
                printByte(dataEnc[i]);
            }

            uint8_t dataDec = decode(dataEnc);
            std::cout << "dataDec: ";
            printByte(dataDec);
        }
    }
    std::cout << "Encode decode test: " << success << '\n';
}

void CoffeeMakerTask::printByte(const uint8_t& b) {
    for (size_t i = 0; i < 8; i++) {
        std::cout << ((b >> (7 - i)) & 0b00000001) << " ";
    }
    std::cout << '\n';
}

uint8_t CoffeeMakerTask::reverse(uint8_t b) {
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}

//---------------------------------------------------------------------------
}  // namespace esp32jura::jura
//---------------------------------------------------------------------------
