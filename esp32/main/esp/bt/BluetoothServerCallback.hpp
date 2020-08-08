#pragma once

#include <string>

//---------------------------------------------------------------------------
namespace esp32jura::esp::bt {
//---------------------------------------------------------------------------
class BluetoothServerCallback {
   public:
    virtual ~BluetoothServerCallback() = default;

    virtual void onConfigurationDone(std::string& wifiSsid, std::string& wifiPassword, std::string& jid, std::string& jidPassword, std::string& jidSender) = 0;
};
//---------------------------------------------------------------------------
}  // namespace esp32jura::esp::bt
   //---------------------------------------------------------------------------