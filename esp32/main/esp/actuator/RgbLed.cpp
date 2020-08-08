#include "RgbLed.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::esp::actuators {
//---------------------------------------------------------------------------
RgbLed::RgbLed(gpio_num_t r, gpio_num_t g, gpio_num_t b) : r(r, false, false, false), g(g, false, false, false), b(b, false, false, false) {}

void RgbLed::turnOnOnly(smooth::core::io::Output& led) {
    r.set(led == r);
    g.set(led == g);
    b.set(led == b);
}

void RgbLed::turnOffOnly(smooth::core::io::Output& led) {
    r.set(led != r);
    g.set(led != g);
    b.set(led != b);
}

void RgbLed::turnOn(smooth::core::io::Output& led) { led.set(true); }

void RgbLed::turnOff(smooth::core::io::Output& led) { led.set(false); }
//---------------------------------------------------------------------------
}  // namespace esp32jura::esp::actuators
   //---------------------------------------------------------------------------