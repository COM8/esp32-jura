#include "SerialJuraBridgeTask.hpp"

#include <iostream>
#include <vector>

#include "smooth/core/task_priorities.h"

//---------------------------------------------------------------------------
namespace esp32jura {
//---------------------------------------------------------------------------
SerialJuraBridgeTask::SerialJuraBridgeTask() : Task("Bridge Task", 0, smooth::core::APPLICATION_BASE_PRIO, std::chrono::milliseconds(100), 1), jura_connection(UART_PORT, UART_TX, UART_RX) {}

void SerialJuraBridgeTask::init() {
    std::cout << "Initializing bridge task...\n";
    serial_connection.init();
    jura_connection.init();
    std::cout << "Bridge task initialized.\n";
}

void SerialJuraBridgeTask::tick() {
    std::vector<uint8_t> buffer;

    if (serial_connection.read(buffer) && buffer.size() > 0) {
        buffer.push_back(static_cast<uint8_t>('\r'));
        buffer.push_back(static_cast<uint8_t>('\n'));
        std::cout << "[S->C]: Sending " << buffer.size() << " byte...\n";
        jura_connection.write_decoded(buffer);
        std::cout << "[S->C]: Done.\n";
        buffer.clear();
    }

    if (jura_connection.read_decoded(buffer) && buffer.size() > 0) {
        std::cout << "[C->S]: Sending " << buffer.size() << " byte...\n";
        serial_connection.write(buffer);
        std::cout << "[C->S]: Done.\n";
        buffer.clear();
    }
}
//---------------------------------------------------------------------------
}  // namespace esp32jura
//---------------------------------------------------------------------------
