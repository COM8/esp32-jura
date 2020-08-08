#pragma once

#include <string>
#include <vector>

#include "SerialConnection.hpp"
#include "smooth/core/Task.h"

//---------------------------------------------------------------------------
namespace esp32jura::esp::serial {
//---------------------------------------------------------------------------
class SerialConnectionTask : public smooth::core::Task {
   private:
    SerialConnection connection;

   public:
    SerialConnectionTask();

    void init() override;
    void tick() override;
};
//---------------------------------------------------------------------------
}  // namespace esp32jura::esp::serial
//---------------------------------------------------------------------------
