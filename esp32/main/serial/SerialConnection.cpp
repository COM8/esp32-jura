#include "SerialConnection.hpp"

#include <cassert>
#include <sstream>

#include "smooth/core/task_priorities.h"

//---------------------------------------------------------------------------
namespace esp32jura::serial {
//---------------------------------------------------------------------------
SerialConnection::SerialConnection() : Task("Serial Task", 0, 1, std::chrono::milliseconds(100), 1) {}

void SerialConnection::init() {
    assert(state != SC_DISABLED && state != SC_ERROR);
    std::cout << "Initializing serial connection on port (Port: " << UART_PORT << " , TX: " << UART_TX << " , RX: " << UART_RX << ")...\n";
    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uartConfig));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT, UART_TX, UART_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, BUF_SIZE * 2, 0, 0, nullptr, 0));
    std::cout << "Serial connection on port (Port: " << UART_PORT << " , TX: " << UART_TX << " , RX: " << UART_RX << ") has been initialized successfully.\n";
    state = SC_READY;
}

void SerialConnection::tick() {
    assert(state == SC_READY);
    while (true) {
        std::string s = read();
        s = "Thanks: " + s;
        write(s);
    }
}

std::string SerialConnection::read() {
    size_t size = 0;
    ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_PORT, &size));
    if (size < 4) {
        return "";
    }

    // Wait 8ms for the data to arrive + 4ms for fail save reasons:
    int len = uart_read_bytes(UART_PORT, read_buffer.data(), read_buffer.size(), (8 + 4) / portTICK_PERIOD_MS);
    if (len < 0) {
        std::cerr << "Failed to read from UART." << std::endl;
        return "";
    }

    std::ostringstream convert;
    for (size_t i = 0; i < len; i++) {
        convert << (int)read_buffer[i];
    }
    return "";
}

size_t SerialConnection::write(const std::vector<uint8_t>& data) {
    int count = uart_write_bytes(UART_PORT, reinterpret_cast<const char*>(data.data()), data.size());
    return static_cast<size_t>(count);
}

size_t SerialConnection::write(const std::string& data) { return write(std::vector<uint8_t>(data.begin(), data.end())); }

//---------------------------------------------------------------------------
}  // namespace esp32jura::serial
//---------------------------------------------------------------------------
