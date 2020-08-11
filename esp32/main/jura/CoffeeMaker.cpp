#include "CoffeeMaker.hpp"

#include <cassert>
#include <iomanip>
#include <iostream>
#include <limits>
#include <thread>

#include "JuraCommands.hpp"
#include "smooth/core/task_priorities.h"

//---------------------------------------------------------------------------
namespace esp32jura::jura {
//---------------------------------------------------------------------------
CoffeeMaker::CoffeeMaker() : connection(UART_PORT, UART_TX, UART_RX) {}

void CoffeeMaker::init() {
    std::cout << "Initializing coffee maker...\n";
    connection.init();
    std::cout << "Coffee maker initialized.\n";
}

void CoffeeMaker::switch_page() {
    press_button(jura_button_t::BUTTON_6);
    if (++pageNum >= NUM_PAGES) {
        pageNum = 0;
    }
}

void CoffeeMaker::switch_page(size_t pageNum) {
    if (this->pageNum == pageNum) {
        return;
    }

    press_button(jura_button_t::BUTTON_6);
    if (++pageNum >= NUM_PAGES) {
        pageNum = 0;
    }
    switch_page(pageNum);
}

void CoffeeMaker::brew_coffee(coffee_t coffee) {
    size_t pageNum = get_page_num(coffee);
    switch_page(pageNum);
    jura_button_t button = get_button_num(coffee);
    press_button(button);
}

size_t CoffeeMaker::get_page_num(coffee_t coffee) {
    for (const std::pair<coffee_t, size_t>& c : coffee_page_map) {
        if (c.first == coffee) {
            return c.second;
        }
    }
    assert(false);  // Should not happen
    return std::numeric_limits<size_t>::max();
}

CoffeeMaker::jura_button_t CoffeeMaker::get_button_num(coffee_t coffee) {
    for (const std::pair<coffee_t, jura_button_t>& c : coffee_button_map) {
        if (c.first == coffee) {
            return c.second;
        }
    }
    assert(false);  // Should not happen
    return jura_button_t::BUTTON_6;
}

void CoffeeMaker::press_button(jura_button_t button) {
    switch (button) {
        case jura_button_t::BUTTON_1:
            connection.write_decoded(JURA_BUTTON_1);
            break;

        case jura_button_t::BUTTON_2:
            connection.write_decoded(JURA_BUTTON_2);
            break;

        case jura_button_t::BUTTON_3:
            connection.write_decoded(JURA_BUTTON_3);
            break;

        case jura_button_t::BUTTON_4:
            connection.write_decoded(JURA_BUTTON_4);
            break;

        case jura_button_t::BUTTON_5:
            connection.write_decoded(JURA_BUTTON_5);
            break;

        case jura_button_t::BUTTON_6:
            connection.write_decoded(JURA_BUTTON_6);
            break;

        default:
            assert(false);  // Should not happen
            break;
    }

    // Give the coffee maker to to react:
    std::this_thread::sleep_for(std::chrono::milliseconds{500});
}

//---------------------------------------------------------------------------
}  // namespace esp32jura::jura
//---------------------------------------------------------------------------
