#pragma once

#include <smooth/core/Task.h>

#include "driver/uart.h"

//---------------------------------------------------------------------------
namespace esp32jura::jura {
//---------------------------------------------------------------------------
class CoffeeMakerTask : public smooth::core::Task {
   private:
    uart_config_t uartConfig{.baud_rate = 9600,
                             .data_bits = UART_DATA_8_BITS,
                             .parity = UART_PARITY_DISABLE,
                             .stop_bits = UART_STOP_BITS_1,
                             .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};
    static constexpr int BUF_SIZE = 1024;

   public:
    CoffeeMakerTask();

    void init() override;
    void tick() override;
};
//---------------------------------------------------------------------------
}  // namespace esp32jura::jura
//---------------------------------------------------------------------------
