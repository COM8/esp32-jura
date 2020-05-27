#include "Esp32Jura.hpp"

#include <smooth/core/task_priorities.h>

#include <iostream>

//---------------------------------------------------------------------------
namespace esp32jura {
//---------------------------------------------------------------------------
void app_main(void) {
    Esp32Jura espJura;
    espJura.start();
}

Esp32Jura::Esp32Jura() : Application(smooth::core::APPLICATION_BASE_PRIO, std::chrono::seconds(1)), snooper() {}

void Esp32Jura::init() {
    std::cout << "ESP32 Jura initializing...\n";
    // snooper.start();
    coffeeMaker.start();
    std::cout << "ESP32 Jura initialized.\n";
}

void Esp32Jura::tick() {}

//---------------------------------------------------------------------------
}  // namespace esp32jura
//---------------------------------------------------------------------------
