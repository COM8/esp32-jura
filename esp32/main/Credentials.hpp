#pragma once

#include <string>

//---------------------------------------------------------------------------
namespace esp32jura {
//---------------------------------------------------------------------------
/**
 * This file contains credentials for debugging the XMPP and WIFI connection,
 * without having to go through the usual setup procedure.
 * To apply these settings uncomment the "initWithDummyValues()" call in the "Esp32Jura.cpp" file.
 *
 * !!NEVER COMMIT THIS FILE!!
 **/
const std::string SSID = "Your WIFI SSID";
const std::string PASSWORD = "Your WIFI Password";
const std::string JID = "JID of this device";
const std::string JID_PASSWORD = "Password for the JID of this device";
const std::string JID_SENDER = "The owner JID of this device";
//---------------------------------------------------------------------------
}  // namespace esp32jura
//---------------------------------------------------------------------------
