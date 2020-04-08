#include "Esp32Jura.hpp"

#include <smooth/core/task_priorities.h>

//---------------------------------------------------------------------------
namespace esp32jura {
//---------------------------------------------------------------------------
void app_main(void) {
    Esp32Jura espJura;
    espJura.start();
}

Esp32Jura::Esp32Jura()
    : Application(smooth::core::APPLICATION_BASE_PRIO,
                  std::chrono::seconds(1)) {}

void Esp32Jura::init() {}

void Esp32Jura::tick() {}

//---------------------------------------------------------------------------
}  // namespace esp32jura
//---------------------------------------------------------------------------
