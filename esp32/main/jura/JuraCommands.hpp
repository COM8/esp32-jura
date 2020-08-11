#pragma once

#include <string>

//---------------------------------------------------------------------------
namespace esp32jura::jura {
//---------------------------------------------------------------------------
const std::string JURA_POWER_OFF = "AN:01";
const std::string JURA_TEST_MODE_ON = "AN:20";
const std::string JURA_TEST_MODE_OFF = "AN:21";

const std::string JURA_GET_TYPE = "TY:";

const std::string JURA_BUTTON_1 = "FA:04";
const std::string JURA_BUTTON_2 = "FA:05";
const std::string JURA_BUTTON_3 = "FA:06";
const std::string JURA_BUTTON_4 = "FA:07";
const std::string JURA_BUTTON_5 = "FA:08";
const std::string JURA_BUTTON_6 = "FA:09";
//---------------------------------------------------------------------------
}  // namespace esp32jura::jura
//---------------------------------------------------------------------------
