#include "SerialConnectionTask.hpp"

#include "smooth/core/task_priorities.h"

//---------------------------------------------------------------------------
namespace esp32jura::esp::serial {
//---------------------------------------------------------------------------
SerialConnectionTask::SerialConnectionTask() : Task("Serial Task", 0, smooth::core::APPLICATION_BASE_PRIO, std::chrono::milliseconds(100), 1) {}

void SerialConnectionTask::init() { connection.init(); }

void SerialConnectionTask::tick() {
    std::string s = connection.read();
    if (!s.empty()) {
        s = "Thanks: " + s + "\r\n";
        connection.write(s);
    }
}
//---------------------------------------------------------------------------
}  // namespace esp32jura::esp::serial
//---------------------------------------------------------------------------
