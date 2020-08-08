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

Esp32Jura::Esp32Jura() : Application(smooth::core::APPLICATION_BASE_PRIO, std::chrono::seconds(60)) {
#ifdef MODE_XMPP
    rgbLed = std::make_shared<esp::actuator::RgbLed>(GPIO_NUM_27, GPIO_NUM_26, GPIO_NUM_25);
    xmpp = std::make_unique<xmpp::XmppTask>(storage);
    wifiTask = std::make_shared<esp::WifiTask>(&get_wifi(), rgbLed, storage);
#endif  // MODE_XMPP
}

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
#ifdef MODE_XMPP
    xmpp->start();
#endif  // MODE_XMPP
    std::cout << "ESP32 Jura initialized.\n";
}

void Esp32Jura::tick() {}

//---------------------------------------------------------------------------
}  // namespace esp32jura
//---------------------------------------------------------------------------
