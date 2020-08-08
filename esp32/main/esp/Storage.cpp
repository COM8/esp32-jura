#include "Storage.hpp"

#include <array>
#include <iostream>

#include "esp_system.h"
#include "nvs_flash.h"

//-------------------------------value--------------------------------------------
namespace esp32jura::esp {
//---------------------------------------------------------------------------
// Keys have a maximum length of 15 characters
const std::string Storage::SETUP_DONE = "setup_done";
const std::string Storage::INITIALIZED = "initialized";
const std::string Storage::WIFI_SSID = "wifi_ssid";
const std::string Storage::WIFI_PASSWORD = "wifi_pass";
const std::string Storage::JID = "jid";
const std::string Storage::JID_PASSWORD = "jid_pass";
const std::string Storage::JID_SENDER = "jid_sender";

void Storage::init() {
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        std::cout << "NVS partition was truncated and needs to be erased.\n";
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    std::cout << "\n";
    std::cout << "Opening Non-Volatile Storage (NVS) handle... ";
    err = nvs_open("storage", NVS_READWRITE, &handle);

    if (err != ESP_OK) {
        std::cerr << "Error " << esp_err_to_name(err) << " opening NVS handle!\n";
        ready = false;
    } else {
        std::cout << "Done\n";
        ready = true;
    }
}

bool Storage::readBool(const std::string& key) {
    if (!ready) {
        std::cerr << "Initialize storage first!\n";
        return false;
    }

    return readUInt8(key) == 1;
}

uint8_t Storage::readUInt8(const std::string& key) {
    if (!ready) {
        std::cerr << "Initialize storage first!\n";
        return false;
    }

    uint8_t val;
    esp_err_t err = nvs_get_u8(handle, key.c_str(), &val);
    switch (err) {
        case ESP_OK:
            return val;
        case ESP_ERR_NVS_NOT_FOUND:
            std::cerr << "The value for key \"" << key << "\" has not been initialized yet!\n";
            break;
        default:
            std::cerr << "Error reading the value for key \"" << key << "\"- " << esp_err_to_name(err) << "\n";
            break;
    }
    return 0;
}

uint16_t Storage::readUInt16(const std::string& key) {
    if (!ready) {
        std::cerr << "Initialize storage first!\n";
        return false;
    }

    uint16_t val;
    esp_err_t err = nvs_get_u16(handle, key.c_str(), &val);
    switch (err) {
        case ESP_OK:
            return val;
        case ESP_ERR_NVS_NOT_FOUND:
            std::cerr << "The value for key \"" << key << "\" has not been initialized yet!\n";
            break;
        default:
            std::cerr << "Error reading the value for key \"" << key << "\"- " << esp_err_to_name(err) << "\n";
            break;
    }
    return 0;
}

std::string Storage::readString(const std::string& key) {
    if (!ready) {
        std::cerr << "Initialize storage first!\n";
        return "";
    }

    std::string strLenKey = key + "_len";
    size_t strLen = static_cast<size_t>(readUInt16(strLenKey));
    if (strLen <= 0) {
        std::cout << "Read string with length 0 from key \"" << key << "\"\n";
        return "";
    }

    char* str = new char[strLen];
    esp_err_t err = nvs_get_str(handle, key.c_str(), str, &strLen);
    switch (err) {
        case ESP_OK: {
            std::string result = str;
            delete[] str;
            return result;
        }
        case ESP_ERR_NVS_NOT_FOUND:
            std::cerr << "The value for key \"" << key << "\" has not been initialized yet!\n";
            break;
        default:
            std::cerr << "Error reading the value for key \"" << key << "\"- " << esp_err_to_name(err) << "\n";
            break;
    }
    delete[] str;
    return "";
}

void Storage::writeBool(const std::string& key, bool value) { writeUInt8(key, static_cast<uint8_t>(value)); }

void Storage::writeUInt16(const std::string& key, uint16_t value) {
    if (!ready) {
        std::cerr << "Initialize storage first!\n";
        return;
    }

    esp_err_t err = nvs_set_u16(handle, key.c_str(), value);
    switch (err) {
        case ESP_OK:
            break;
        default:
            std::cerr << "Error writing the value for key \"" << key << "\" - " << esp_err_to_name(err) << "\n";
            break;
    }
}

void Storage::writeUInt8(const std::string& key, uint8_t value) {
    if (!ready) {
        std::cerr << "Initialize storage first!\n";
        return;
    }

    esp_err_t err = nvs_set_u8(handle, key.c_str(), value);
    switch (err) {
        case ESP_OK:
            break;
        default:
            std::cerr << "Error writing the value for key \"" << key << "\" - " << esp_err_to_name(err) << "\n";
            break;
    }
}

void Storage::writeString(const std::string& key, const std::string& value) {
    if (!ready) {
        std::cerr << "Initialize storage first!\n";
        return;
    }

    std::string strLenKey = key + "_len";
    size_t strLen = value.length() + 1;
    writeUInt16(strLenKey, static_cast<uint16_t>(strLen));

    esp_err_t err = nvs_set_str(handle, key.c_str(), value.c_str());
    switch (err) {
        case ESP_OK:
            break;
        default:
            std::cerr << "Error writing the value for key \"" << key << "\" - " << esp_err_to_name(err) << "\n";
            break;
    }
}
//---------------------------------------------------------------------------
}  // namespace esp32jura::esp
   //---------------------------------------------------------------------------