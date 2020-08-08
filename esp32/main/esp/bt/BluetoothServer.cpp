#include "BluetoothServer.hpp"

#include <array>
#include <cstdio>
#include <string>

#include "BLEDevice.h"

//---------------------------------------------------------------------------
namespace esp32jura::esp::bt {
//---------------------------------------------------------------------------
BluetoothServer::BluetoothServer(std::shared_ptr<actuator::RgbLed> rgbLed, std::shared_ptr<Storage> storage)
    : rgbLed(std::move(rgbLed)), storage(std::move(storage)), running(false), server(nullptr), serviceHelper(getChipMacString()), advertising(nullptr), advertisingData(), serverCallback(nullptr) {}

bool BluetoothServer::isRunning() { return running; }

void BluetoothServer::init() {
    BLEDevice::init("ESP32 meets XMPP");
    server = BLEDevice::createServer();
    server->setCallbacks(this);

    // Generic Computer appearance
    // https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.gap.appearance.xml
    advertisingData.setAppearance(128);
    advertisingData.setManufacturerData("TUM Garching");
    advertisingData.setName("ESP32 meets XMPP");

    advertising = server->getAdvertising();
    advertising->setAdvertisementData(advertisingData);

    serviceHelper.init(this, server);
}

void BluetoothServer::start() {
    if (running) {
        return;
    }
    serviceHelper.start(server);
    advertising->start();
    rgbLed->turnOnOnly(rgbLed->b);
}

void BluetoothServer::stop() {
    if (!running) {
    }
    advertising->stop();
    serviceHelper.stop(server);
    rgbLed->turnOff(rgbLed->b);
}

void BluetoothServer::onRead(BLECharacteristic* characteristic) {}

void BluetoothServer::onWrite(BLECharacteristic* characteristic) {
    std::string value = characteristic->getValue();
    if (characteristic->getUUID().equals(BLEServiceHelper::UUID_CHARACTERISTIC_WIFI_SSID)) {
        storage->writeString(Storage::WIFI_SSID, value);
    } else if (characteristic->getUUID().equals(BLEServiceHelper::UUID_CHARACTERISTIC_WIFI_PASSWORD)) {
        storage->writeString(Storage::WIFI_PASSWORD, value);
    } else if (characteristic->getUUID().equals(BLEServiceHelper::UUID_CHARACTERISTIC_JID)) {
        storage->writeString(Storage::JID, value);
    } else if (characteristic->getUUID().equals(BLEServiceHelper::UUID_CHARACTERISTIC_JID_PASSWORD)) {
        storage->writeString(Storage::JID_PASSWORD, value);
    } else if (characteristic->getUUID().equals(BLEServiceHelper::UUID_CHARACTERISTIC_JID_SENDER)) {
        storage->writeString(Storage::JID_SENDER, value);
    } else if (characteristic->getUUID().equals(BLEServiceHelper::UUID_CHARACTERISTIC_SETTINGS_DONE)) {
        storage->writeBool(Storage::INITIALIZED, true);
        if (serverCallback) {
            std::string wifiSsid = storage->readString(Storage::WIFI_SSID);
            std::string wifiPassword = storage->readString(Storage::WIFI_PASSWORD);
            std::string jid = storage->readString(Storage::JID);
            std::string jidPassword = storage->readString(Storage::JID_PASSWORD);
            std::string jidSender = storage->readString(Storage::JID_SENDER);
            serverCallback->onConfigurationDone(wifiSsid, wifiPassword, jid, jidPassword, jidSender);
        }
    }
}

void BluetoothServer::onConnect(BLEServer* pServer) {}

void BluetoothServer::onDisconnect(BLEServer* pServer) {}

void BluetoothServer::registerCallback(BluetoothServerCallback* serverCallback) { this->serverCallback = serverCallback; }

std::string BluetoothServer::getChipMacString() {
    std::array<uint8_t, 6> chipMac;
    esp_efuse_mac_get_default(chipMac.data());
    std::array<char, 18> buff;
    sprintf(buff.data(), "%02X:%02X:%02X:%02X:%02X:%02X", chipMac[0], chipMac[1], chipMac[2], chipMac[3], chipMac[4], chipMac[5]);
    return std::string(buff.data());
}

//---------------------------------------------------------------------------
}  // namespace esp32jura::esp::bt
   //---------------------------------------------------------------------------