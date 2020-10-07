#include "CoffeeMakerTask.hpp"

#include <iomanip>
#include <iostream>

#include "smooth/core/task_priorities.h"

//---------------------------------------------------------------------------
namespace esp32jura::jura {
//---------------------------------------------------------------------------
CoffeeMakerTask::CoffeeMakerTask() : Task("Coffee Task", 0, smooth::core::APPLICATION_BASE_PRIO, std::chrono::milliseconds(100), 1), button(BUTTON_SIGNAL) {}

void CoffeeMakerTask::init() {
    std::cout << "Initializing coffee maker task...\n";
    coffeeMaker.init();
    std::cout << "Coffee maker task initialized.\n";
}

void CoffeeMakerTask::tick() {
    // std::cout << "Coffee maker tick...\n";
    write();
    read();
    read();
    read();
    read();
}

void CoffeeMakerTask::write() {
    if (button.isPressed()) {
        if (!buttonPressed) {
            buttonPressed = true;

            /*std::stringstream stream;
            stream << std::setfill('0') << std::setw(2) << std::uppercase;
            stream << std::hex << buttonCounter;
            // std::string msg = "RT:" + stream.str();
            std::string msg = "TY:";
            if (msg == "AN:0A") {
                std::cout << "Skipping: " << msg << '\n';
            } else {
                std::cout << "Sending: " << msg << '\n';
                msg += +"\r\n";
                coffeeMaker.connection.write_decoded(msg);
            }
            ++buttonCounter;*/
            coffeeMaker.brew_custom_coffee();
        }
    } else {
        buttonPressed = false;
    }
}

void CoffeeMakerTask::read() {
    std::vector<uint8_t> buffer;
    if (coffeeMaker.connection.read_decoded(buffer)) {
        coffeeMaker.connection.print_bytes(buffer);
    }
}

//---------------------------------------------------------------------------
}  // namespace esp32jura::jura
//---------------------------------------------------------------------------
