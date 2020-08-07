#include "SerialConnection.hpp"

#include <cassert>
#include <iostream>
#include <sstream>

//---------------------------------------------------------------------------
namespace esp32jura::serial {
//---------------------------------------------------------------------------
SerialConnection::SerialConnection() { read_buffer.reserve(BUF_SIZE); }

void SerialConnection::init() {
    assert(state == SC_DISABLED || state == SC_ERROR);
    std::cout << "Initializing serial connection on port (Port: " << UART_PORT << " , TX: " << UART_TX << " , RX: " << UART_RX << ")...\n";
    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uartConfig));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT, UART_TX, UART_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, BUF_SIZE * 2, 0, 0, nullptr, 0));
    std::cout << "Serial connection on port (Port: " << UART_PORT << " , TX: " << UART_TX << " , RX: " << UART_RX << ") has been initialized successfully.\n";
    state = SC_READY;
}

bool SerialConnection::read_byte() {
    uint8_t c;
    int len = uart_read_bytes(UART_PORT, &c, 1, 1000 / portTICK_PERIOD_MS);
    if (len <= 0) {
        std::cerr << "Failed to read from UART." << std::endl;
        return false;
    }
    if (c != '\n' && c != '\r') {
        read_buffer.push_back(c);
        return false;
    }
    return true;
}

bool SerialConnection::data_available() {
    size_t size = 0;
    ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_PORT, &size));
    return size > 0;
}

std::string SerialConnection::read() {
    assert(state == SC_READY);
    while (data_available()) {
        if (read_byte()) {
            std::string result = vec_to_string(read_buffer);
            read_buffer.clear();
            return result;
        }
    }
    return "";
}

std::string SerialConnection::vec_to_string(const std::vector<uint8_t>& data) {
    if (data.size() <= 0) {
        return "";
    }

    std::ostringstream sstream;
    for (size_t i = 0; i < data.size(); i++) {
        sstream << static_cast<char>(data[i]);
    }
    return sstream.str();
}

size_t SerialConnection::write(const std::vector<uint8_t>& data) {
    assert(state == SC_READY);
    int count = uart_write_bytes(UART_PORT, reinterpret_cast<const char*>(data.data()), data.size());
    return static_cast<size_t>(count);
}

size_t SerialConnection::write(const std::string& data) { return write(std::vector<uint8_t>(data.begin(), data.end())); }

//---------------------------------------------------------------------------
}  // namespace esp32jura::serial
//---------------------------------------------------------------------------
