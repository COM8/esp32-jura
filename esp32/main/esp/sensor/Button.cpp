#include "Button.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::esp::sensor {
//---------------------------------------------------------------------------
Button::Button(gpio_num_t signal) : signal(signal, true, false) {}

bool Button::isPressed() { return not signal.read(); }
//---------------------------------------------------------------------------
}  // namespace esp32jura::esp::sensor
   //---------------------------------------------------------------------------