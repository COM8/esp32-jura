#include "Esp32Jura.hpp"

#include <smooth/core/task_priorities.h>

#include <iostream>

#include "Credentials.hpp"
#include "esp_system.h"

//---------------------------------------------------------------------------
namespace esp32jura {
//---------------------------------------------------------------------------
void app_main(void) {
    Esp32Jura espJura;
    espJura.start();
}

Esp32Jura::Esp32Jura() : Application(smooth::core::APPLICATION_BASE_PRIO, std::chrono::seconds(60)) {
    storage = std::make_shared<esp::Storage>();
#ifdef MODE_XMPP
    rgbLed = std::make_shared<esp::actuator::RgbLed>(GPIO_NUM_23, GPIO_NUM_25, GPIO_NUM_26);
    xmpp = std::make_unique<xmpp::XmppTask>(storage);
    wifiTask = std::make_shared<esp::WifiTask>(&get_wifi(), rgbLed, storage);
#endif  // MODE_XMPP
}

void Esp32Jura::init() {
    std::cout << "ESP32 Jura initializing...\n";
    storage->init();
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
    initWithDummyValues();

    wifiTask->start();
    xmpp->start();
    rgbLed->turnOnOnly(rgbLed->r);

    // Check if reset button is pressen:
    if (resetButton.isPressed()) {
        std::cout << "Reset button pressed. Discarding initialization.\n";
        storage->writeBool(esp::Storage::INITIALIZED, false);
        storage->writeBool(esp::Storage::SETUP_DONE, false);
    }
#endif  // MODE_XMPP
    std::cout << "ESP32 Jura initialized.\n";
}

void Esp32Jura::tick() {
    // Check if reset button is pressen:
    if (resetButton.isPressed()) {
        std::cout << "Reset button pressed. Discarding initialization.\n";
        storage->writeBool(esp::Storage::INITIALIZED, false);
        storage->writeBool(esp::Storage::SETUP_DONE, false);
        // Restart
        esp_restart();
    }
}

void Esp32Jura::initWithDummyValues() {
    storage->writeString(esp::Storage::JID, JID);
    storage->writeString(esp::Storage::JID_PASSWORD, JID_PASSWORD);
    storage->writeString(esp::Storage::JID_SENDER, JID_SENDER);
    storage->writeString(esp::Storage::WIFI_SSID, SSID);
    storage->writeString(esp::Storage::WIFI_PASSWORD, PASSWORD);
    storage->writeBool(esp::Storage::INITIALIZED, true);
    storage->writeBool(esp::Storage::SETUP_DONE, true);
    std::cout << "INITIALIZED WITH DUMMY VALUES!" << std::endl;
}

//---------------------------------------------------------------------------
}  // namespace esp32jura
//---------------------------------------------------------------------------
