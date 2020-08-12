#pragma once

#include <string>

//---------------------------------------------------------------------------
namespace esp32jura::jura {
//---------------------------------------------------------------------------
const std::string JURA_POWER_OFF = "AN:01\r\n";
const std::string JURA_TEST_MODE_ON = "AN:20\r\n";
const std::string JURA_TEST_MODE_OFF = "AN:21\r\n";

const std::string JURA_GET_TYPE = "TY:\r\n";

const std::string JURA_BUTTON_1 = "FA:04\r\n";
const std::string JURA_BUTTON_2 = "FA:05\r\n";
const std::string JURA_BUTTON_3 = "FA:06\r\n";
const std::string JURA_BUTTON_4 = "FA:07\r\n";
const std::string JURA_BUTTON_5 = "FA:08\r\n";
const std::string JURA_BUTTON_6 = "FA:09\r\n";
//---------------------------------------------------------------------------
}  // namespace esp32jura::jura
//---------------------------------------------------------------------------
