#include "JuraConnection.hpp"

#include <stdlib.h>

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

//---------------------------------------------------------------------------
namespace esp32jura::jura {
//---------------------------------------------------------------------------
JuraConnection::JuraConnection(uart_port_t uart_port_num, gpio_num_t uart_tx, gpio_num_t uart_rx) : UART_PORT(uart_port_num), UART_TX(uart_tx), UART_RX(uart_rx) {}

void JuraConnection::init() {
    std::cout << "Initializing JURA connection on port (Port: " << UART_PORT << " , TX: " << UART_TX << " , RX: " << UART_RX << ")...\n";
    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uartConfig));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT, UART_TX, UART_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, BUF_SIZE * 2, 0, 0, nullptr, 0));
    std::cout << "JURA connection on port (Port: " << UART_PORT << " , TX: " << UART_TX << " , RX: " << UART_RX << ") has been initialized successfully.\n";
}

bool JuraConnection::read_decoded(uint8_t* byte) {
    std::array<uint8_t, 4> buffer;
    if (!read_encoded(&buffer)) {
        return false;
    }
    *byte = decode(buffer);
    return true;
}

bool JuraConnection::read_decoded(std::vector<uint8_t>* data) {
    // Read encoded data:
    std::vector<std::array<uint8_t, 4>> dataBuffer;
    if (read_encoded(&dataBuffer) <= 0) {
        return false;
    }

    // Decode all:
    for (const std::array<uint8_t, 4>& buffer : dataBuffer) {
        data->push_back(decode(buffer));
    }
    return true;
}

bool JuraConnection::write_decoded(const uint8_t& byte) { return write_encoded(encode(byte)); }

bool JuraConnection::write_decoded(const std::vector<uint8_t>& data) {
    for (const uint8_t& byte : data) {
        if (!write_decoded(byte)) {
            return false;
        }
    }
    return true;
}

void JuraConnection::print_byte(const uint8_t& byte) {
    for (size_t i = 0; i < 8; i++) {
        std::cout << ((byte >> (7 - i)) & 0b00000001) << " ";
    }
}

void JuraConnection::print_bytes(const std::vector<uint8_t>& data) {
    for (const uint8_t& byte : data) {
        print_byte(byte);
        std::cout << "\n";
    }
}

void JuraConnection::run_encode_decode_test() {
    bool success = true;

    for (uint16_t i = 0b00000000; i <= 0b11111111; i++) {
        if (i != decode(encode(i))) {
            success = false;
            std::cout << "data: ";
            print_byte(i);
            std::cout << "\n";

            std::array<uint8_t, 4> dataEnc = encode(i);
            for (size_t i = 0; i < 4; i++) {
                std::cout << "dataEnc[" << i << "]: ";
                print_byte(dataEnc[i]);
                std::cout << "\n";
            }

            uint8_t dataDec = decode(dataEnc);
            std::cout << "dataDec: ";
            print_byte(dataDec);
            std::cout << "\n";
        }
    }
    // Flush the stdout to ensure the result gets printed when assert(success) fails:
    std::cout << "Encode decode test: " << success << std::endl;
    assert(success);
}

std::array<uint8_t, 4> JuraConnection::encode(const uint8_t& decData) {
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

uint8_t JuraConnection::decode(const std::array<uint8_t, 4>& encData) {
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

bool JuraConnection::write_encoded(const std::array<uint8_t, 4>& encData) {
    int count = uart_write_bytes(UART_PORT, reinterpret_cast<const char*>(encData.data()), 4);
    std::this_thread::sleep_for(std::chrono::milliseconds{8});
    return count > 0;
}

bool JuraConnection::read_encoded(std::array<uint8_t, 4>* buffer) {
    size_t size = 0;
    ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_PORT, &size));
    if (size < 4) {
        return false;
    }

    // Wait 8ms for the data to arrive + 4ms for fail save reasons:
    int len = uart_read_bytes(UART_PORT, buffer->data(), 4, (8 + 4) / portTICK_PERIOD_MS);
    if (len < 0) {
        std::cerr << "Failed to read from UART." << std::endl;
        return false;
    }
    return true;
}

size_t JuraConnection::read_encoded(std::vector<std::array<uint8_t, 4>>* data) {
    // Wait 8 ms for the next bunch of data to arrive:
    std::this_thread::sleep_for(std::chrono::milliseconds{8});

    // Check if data is available:
    size_t size = 0;
    ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_PORT, &size));
    if (size <= 0) {
        return data->size();
    }

    // If less then 4 bytes are available wait again and then try again:
    if (size < 4) {
        std::this_thread::sleep_for(std::chrono::milliseconds{8});
        ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_PORT, &size));
        if (size < 4) {
            std::cerr << "Invalid amount of UART data found (" << size << " byte) - flushing.\n";
            uart_flush(UART_PORT);
            return data->size();
        }
        return read_encoded(data);
    }

    // Read all data available:
    for (size_t i = size; i >= 4; i -= 4) {
        std::array<uint8_t, 4> buffer;
        if (!read_encoded(&buffer)) {
            return data->size();
        }
        data->push_back(std::move(buffer));
    }

    // Try to read again to ensure there is no data available any more:
    return read_encoded(data);
}

uint8_t JuraConnection::reverse(const uint8_t& byte) {
    uint8_t result;
    result = (byte & 0xF0) >> 4 | (byte & 0x0F) << 4;
    result = (byte & 0xCC) >> 2 | (byte & 0x33) << 2;
    result = (byte & 0xAA) >> 1 | (byte & 0x55) << 1;
    return result;
}
//---------------------------------------------------------------------------
}  // namespace esp32jura::jura
//---------------------------------------------------------------------------