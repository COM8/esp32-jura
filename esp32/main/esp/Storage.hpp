#pragma once

#include <string>

#include "nvs.h"

//---------------------------------------------------------------------------
namespace esp32jura::esp {
//---------------------------------------------------------------------------
class Storage {
   public:
    static const std::string SETUP_DONE;
    static const std::string INITIALIZED;
    static const std::string WIFI_SSID;
    static const std::string WIFI_PASSWORD;
    static const std::string JID;
    static const std::string JID_PASSWORD;
    static const std::string JID_SENDER;

    Storage() = default;
    void init();

    bool readBool(const std::string& key);
    uint16_t readUInt16(const std::string& key);
    uint8_t readUInt8(const std::string& key);
    std::string readString(const std::string& key);

    void writeBool(const std::string& key, bool value);
    void writeUInt16(const std::string& key, uint16_t value);
    void writeUInt8(const std::string& key, uint8_t value);
    void writeString(const std::string& key, const std::string& value);

   private:
    bool ready{false};
    nvs_handle_t handle{0};
};

//---------------------------------------------------------------------------
}  // namespace esp32jura::esp
   //---------------------------------------------------------------------------