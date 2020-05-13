#include "JuraSnooper.hpp"

#include <iostream>

#include "smooth/core/task_priorities.h"

//---------------------------------------------------------------------------
namespace esp32jura::jura {
//---------------------------------------------------------------------------
JuraSnooper::JuraSnooper()
    : Task("Coffee Task", 0, 1, std::chrono::seconds(2), 1),
      conCoffeeMaker(UART_PORT_COFFEE_MAKER, UART_TX_COFFEE_MAKER, UART_RX_COFFEE_MAKER),
      conDongle(UART_PORT_DONGLE, UART_TX_DONGLE, UART_RX_DONGLE) {}

void JuraSnooper::init() {
    std::cout << "Initializing JURA snooper...\n";
    JuraConnection::run_encode_decode_test();
    conCoffeeMaker.init();
    conDongle.init();
    std::cout << "JURA snooper initialized.\n";
}

void JuraSnooper::tick() {
    std::vector<uint8_t> buffer;
    // Start in dongle read mode since usually the dongle starts the connection:
    bool inDongleReadMode = true;
    std::cout << "-------------------------Dongle:---------------------------\n";

    while (true) {
        if (read_from_dongle(&buffer, inDongleReadMode)) {
            inDongleReadMode = true;
        }

        if (read_from_coffee_maker(&buffer, inDongleReadMode)) {
            inDongleReadMode = false;
        }
    }
}

bool JuraSnooper::read_from_coffee_maker(std::vector<uint8_t>* buffer, bool inDongleReadMode) {
    buffer->clear();
    if (conCoffeeMaker.read_decoded(buffer)) {
        if (inDongleReadMode) {
            std::cout << "-----------------------Coffee-Maker:-----------------------\n";
        }
        conCoffeeMaker.print_bytes(*buffer);
        return true;
    }
    return false;
}

bool JuraSnooper::read_from_dongle(std::vector<uint8_t>* buffer, bool inDongleReadMode) {
    buffer->clear();
    if (conDongle.read_decoded(buffer)) {
        if (!inDongleReadMode) {
            std::cout << "-------------------------Dongle:---------------------------\n";
        }
        conDongle.print_bytes(*buffer);
        return true;
    }
    return false;
}

//---------------------------------------------------------------------------
}  // namespace esp32jura::jura
//---------------------------------------------------------------------------
