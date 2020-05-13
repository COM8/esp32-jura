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
CoffeeMakerTask::CoffeeMakerTask() : Task("Coffee Task", 0, 1, std::chrono::seconds(2), 1), connection(UART_PORT, UART_TX, UART_RX) {}

void CoffeeMakerTask::init() {
    std::cout << "Initializing coffee maker...\n";
    connection.init();
    std::cout << "Coffee maker initialized.\n";
}

void CoffeeMakerTask::tick() {
    std::cout << "Coffee maker tick...\n";
    write();
    read();
}

void CoffeeMakerTask::write() {}

void CoffeeMakerTask::read() {}

//---------------------------------------------------------------------------
}  // namespace esp32jura::jura
//---------------------------------------------------------------------------
