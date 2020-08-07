#include "CoffeeMakerTask.hpp"

#include <iomanip>
#include <iostream>

#include "smooth/core/task_priorities.h"

//---------------------------------------------------------------------------
namespace esp32jura::jura {
//---------------------------------------------------------------------------
CoffeeMakerTask::CoffeeMakerTask() : Task("Coffee Task", 0, smooth::core::APPLICATION_BASE_PRIO, std::chrono::milliseconds(100), 1), connection(UART_PORT, UART_TX, UART_RX), button(BUTTON_SIGNAL) {}

void CoffeeMakerTask::init() {
    std::cout << "Initializing coffee maker...\n";
    connection.init();
    std::cout << "Coffee maker initialized.\n";
}

void CoffeeMakerTask::tick() {
    // std::cout << "Coffee maker tick...\n";

    if (button.isPressed()) {
        if (!buttonPressed) {
            buttonPressed = true;

            std::stringstream stream;
            stream << std::setfill('0') << std::setw(2) << std::uppercase;
            stream << std::hex << buttonCounter;
            std::string msg = "RT:" + stream.str();
            if (msg == "AN:0A") {
                std::cout << "Skipping: " << msg << '\n';
            } else {
                std::cout << "Sending: " << msg << '\n';
                msg += +"\r\n";
                connection.write_decoded(msg);
            }
            ++buttonCounter;
        }
    } else {
        buttonPressed = false;
    }

    read();
    read();
    read();
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
