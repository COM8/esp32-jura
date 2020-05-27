#include "CoffeeMakerTask.hpp"

#include <iostream>

#include "smooth/core/task_priorities.h"

//---------------------------------------------------------------------------
namespace esp32jura::jura {
//---------------------------------------------------------------------------
CoffeeMakerTask::CoffeeMakerTask() : Task("Coffee Task", 0, 1, std::chrono::seconds(5), 1), connection(UART_PORT, UART_TX, UART_RX) {}

void CoffeeMakerTask::init() {
    std::cout << "Initializing coffee maker...\n";
    connection.init();
    std::cout << "Coffee maker initialized.\n";
}

void CoffeeMakerTask::tick() {
    std::cout << "Coffee maker tick...\n";

    connection.write_decoded("FA:04\r\n");
    on = !on;

    read();
}

void CoffeeMakerTask::write() { connection.write_decoded("TY:\r\n"); }

void CoffeeMakerTask::read() {
    std::vector<uint8_t> buffer;
    if (connection.read_decoded(&buffer)) {
        connection.print_bytes(buffer);
    }
}

//---------------------------------------------------------------------------
}  // namespace esp32jura::jura
//---------------------------------------------------------------------------
