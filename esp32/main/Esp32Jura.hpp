#pragma once

#include <smooth/core/Application.h>

#include "SerialJuraBridgeTask.hpp"
#include "jura/CoffeeMakerTask.hpp"
#include "jura/JuraSnooper.hpp"
#include "serial/SerialConnectionTask.hpp"

// #define MODE_SNOOPER
// #define MODE_COFFEE_MAKER
#define MODE_BRIDGE
// #define MODE_SERIAL

//---------------------------------------------------------------------------
namespace esp32jura {
//---------------------------------------------------------------------------
#if defined(__cplusplus)
extern "C" {
#endif
void app_main(void);
#if defined(__cplusplus)
}
#endif
//---------------------------------------------------------------------------
class Esp32Jura : public smooth::core::Application {
   private:
#ifdef MODE_SNOOPER
    jura::JuraSnooper snooper;
#endif  // MODE_SNOOPER
#ifdef MODE_COFFEE_MAKER
    jura::CoffeeMakerTask coffeeMaker;
#endif  // MODE_COFFEE_MAKER
#ifdef MODE_BRIDGE
    SerialJuraBridgeTask bridge;
#endif  // MODE_BRIDGE
#ifdef MODE_SERIAL
    serial::SerialConnectionTask serialConnection;
#endif  // MODE_SERIAL

   public:
    Esp32Jura();

    void init() override;
    void tick() override;
};
//---------------------------------------------------------------------------
}  // namespace esp32jura
//---------------------------------------------------------------------------
