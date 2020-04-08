#include "CoffeeMakerTask.hpp"

#include <smooth/core/task_priorities.h>

#include <chrono>
#include <iostream>

#include "driver/gpio.h"

//---------------------------------------------------------------------------
namespace esp32jura::jura {
//---------------------------------------------------------------------------
CoffeeMakerTask::CoffeeMakerTask()
    : Task("Coffee Task", 0, 1, std::chrono::seconds(1), 1) {}

void CoffeeMakerTask::init() {
    std::cout << "Initializing coffee maker...\n";
    uart_set_pin(UART_NUM_1, GPIO_NUM_4, GPIO_NUM_5, UART_PIN_NO_CHANGE,
                 UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_1, BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_1, &uartConfig);
    std::cout << "Coffee maker initialized.\n";
}

void CoffeeMakerTask::tick() {
    std::cout << "Coffee maker tick...\n";
    uint8_t buf[BUF_SIZE];
    int len = 0;
    while (true) {
        len = uart_read_bytes(UART_NUM_1, buf, BUF_SIZE, 20 / portTICK_RATE_MS);
        if (len < 0) {
            std::cerr << "Failed to read from UART." << std::endl;
        } else if (len > 0) {
            std::cout << "Read: " << len << " bytes\n";
        }
    }
}
//---------------------------------------------------------------------------
}  // namespace esp32jura::jura
//---------------------------------------------------------------------------
