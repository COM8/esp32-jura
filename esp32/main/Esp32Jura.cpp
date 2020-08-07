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

Esp32Jura::Esp32Jura() : Application(smooth::core::APPLICATION_BASE_PRIO, std::chrono::seconds(60)) {}

void Esp32Jura::init() {
    std::cout << "ESP32 Jura initializing...\n";
#ifdef MODE_SNOOPER
    snooper.start();
#endif  // MODE_SNOOPER
#ifdef MODE_COFFEE_MAKER
    coffeeMaker.start();
#endif  // MODE_COFFEE_MAKER
#ifdef MODE_BRIDGE
    bridge.start();
#endif  // MODE_BRIDGE
#ifdef MODE_SERIAL
    serialConnection.start();
#endif  // MODE_SERIAL
    std::cout << "ESP32 Jura initialized.\n";
}

void Esp32Jura::tick() {}

//---------------------------------------------------------------------------
}  // namespace esp32jura
//---------------------------------------------------------------------------
