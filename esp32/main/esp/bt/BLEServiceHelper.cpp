#include "BLEServiceHelper.hpp"

#include <iostream>

//---------------------------------------------------------------------------
namespace esp32jura::esp::bt {
//---------------------------------------------------------------------------
const BLEUUID BLEServiceHelper::UUID_CHARACTERISTIC_LANGUAGE("00002AA2-0000-1000-8000-00805F9B34FB");
const BLEUUID BLEServiceHelper::UUID_CHARACTERISTIC_HARDWARE_REVISION("00002A27-0000-1000-8000-00805F9B34FB");
const BLEUUID BLEServiceHelper::UUID_CHARACTERISTIC_SOFTWARE_REVISION("00002A28-0000-1000-8000-00805F9B34FB");
const BLEUUID BLEServiceHelper::UUID_CHARACTERISTIC_SERIAL_NUMBER("00002A25-0000-1000-8000-00805F9B34FB");
const BLEUUID BLEServiceHelper::UUID_CHARACTERISTIC_MANUFACTURER_NAME("00002A29-0000-1000-8000-00805F9B34FB");
const BLEUUID BLEServiceHelper::UUID_CHARACTERISTIC_WIFI_SSID("00000001-0000-0000-0000-000000000002");
const BLEUUID BLEServiceHelper::UUID_CHARACTERISTIC_WIFI_PASSWORD("00000002-0000-0000-0000-000000000002");
const BLEUUID BLEServiceHelper::UUID_CHARACTERISTIC_JID("00000003-0000-0000-0000-000000000002");
const BLEUUID BLEServiceHelper::UUID_CHARACTERISTIC_JID_PASSWORD("00000004-0000-0000-0000-000000000002");
const BLEUUID BLEServiceHelper::UUID_CHARACTERISTIC_JID_SENDER("00000005-0000-0000-0000-000000000002");
const BLEUUID BLEServiceHelper::UUID_CHARACTERISTIC_SETTINGS_DONE("00000006-0000-0000-0000-000000000002");

const BLEUUID BLEServiceHelper::UUID_SERVICE_DEVICE_INFORMATION("0000180A-0000-1000-8000-00805F9B34FB");
const BLEUUID BLEServiceHelper::UUID_SERVICE_DEVICE_SETTINGS_1("00000001-0000-0000-0000-000000000001");
const BLEUUID BLEServiceHelper::UUID_SERVICE_DEVICE_SETTINGS_2("00000002-0000-0000-0000-000000000001");

BLEServiceHelper::BLEServiceHelper(std::string btMac) : btMac(btMac), cCCDescriptors(7), cUDDescriptors(7) {}

BLEServiceHelper::~BLEServiceHelper() {
    for (BLE2902* desc : cCCDescriptors) {
        delete desc;
    }

    for (BLEDescriptor* desc : cUDDescriptors) {
        delete desc;
    }
}

BLE2902* BLEServiceHelper::getNewCCCDescriptor(bool enableNotify, bool enableIndicate) {
    BLE2902* desc = new BLE2902();
    desc->setNotifications(enableNotify);
    desc->setIndications(enableIndicate);
    cCCDescriptors.push_back(desc);
    return desc;
}

BLEDescriptor* BLEServiceHelper::getNewCUDDescriptor(std::string description) {
    BLEDescriptor* desc = new BLEDescriptor(BLEUUID(static_cast<uint16_t>(0x2901)));
    desc->setValue(description);
    cUDDescriptors.push_back(desc);
    return desc;
}

void BLEServiceHelper::init(BLECharacteristicCallbacks* callback, BLEServer* server) {
    initDeviceInfoService(callback, server);
    initDeviceSettings1Service(callback, server);
    initDeviceSettings2Service(callback, server);
}

void BLEServiceHelper::start(BLEServer* server) {
    // Device information:
    BLEService* service = server->getServiceByUUID(UUID_SERVICE_DEVICE_INFORMATION);
    service->start();

    // Device settings:
    service = server->getServiceByUUID(UUID_SERVICE_DEVICE_SETTINGS_1);
    service->start();

    // Challenge response:
    service = server->getServiceByUUID(UUID_SERVICE_DEVICE_SETTINGS_2);
    service->start();
}

void BLEServiceHelper::stop(BLEServer* server) {
    // Device information:
    BLEService* service = server->getServiceByUUID(UUID_SERVICE_DEVICE_INFORMATION);
    service->stop();

    // Device settings:
    service = server->getServiceByUUID(UUID_SERVICE_DEVICE_SETTINGS_1);
    service->stop();

    // Challenge response:
    service = server->getServiceByUUID(UUID_SERVICE_DEVICE_SETTINGS_2);
    service->stop();
}

void BLEServiceHelper::initDeviceInfoService(BLECharacteristicCallbacks* callback, BLEServer* server) {
    BLEService* service = server->createService(UUID_SERVICE_DEVICE_INFORMATION);

    // Language:
    BLECharacteristic* characteristic = service->createCharacteristic(UUID_CHARACTERISTIC_LANGUAGE, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    characteristic->setValue("en");
    characteristic->addDescriptor(getNewCCCDescriptor(true, false));

    // Hardware Revision:
    characteristic = service->createCharacteristic(UUID_CHARACTERISTIC_HARDWARE_REVISION, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    characteristic->setValue("1.0");
    characteristic->addDescriptor(getNewCCCDescriptor(true, false));

    // Software Revision:
    characteristic = service->createCharacteristic(UUID_CHARACTERISTIC_SOFTWARE_REVISION, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    characteristic->setValue("1.0");
    characteristic->addDescriptor(getNewCCCDescriptor(true, false));

    // Serial Number:
    characteristic = service->createCharacteristic(UUID_CHARACTERISTIC_SERIAL_NUMBER, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    characteristic->setValue(btMac);
    characteristic->addDescriptor(getNewCCCDescriptor(true, false));

    // Manufacturer Name:
    characteristic = service->createCharacteristic(UUID_CHARACTERISTIC_MANUFACTURER_NAME, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    characteristic->setValue("TUM Garching");
    characteristic->addDescriptor(getNewCCCDescriptor(true, false));
}

void BLEServiceHelper::initDeviceSettings1Service(BLECharacteristicCallbacks* callback, BLEServer* server) {
    BLEService* service = server->createService(UUID_SERVICE_DEVICE_SETTINGS_1);

    // WiFi SSID:
    BLECharacteristic* characteristic = service->createCharacteristic(UUID_CHARACTERISTIC_WIFI_SSID, BLECharacteristic::PROPERTY_WRITE);
    characteristic->setCallbacks(callback);
    characteristic->addDescriptor(getNewCUDDescriptor("Wi-Fi SSID"));

    // WiFi Password:
    characteristic = service->createCharacteristic(UUID_CHARACTERISTIC_WIFI_PASSWORD, BLECharacteristic::PROPERTY_WRITE);
    characteristic->setCallbacks(callback);
    characteristic->addDescriptor(getNewCUDDescriptor("Wi-Fi Password"));

    // JID:
    characteristic = service->createCharacteristic(UUID_CHARACTERISTIC_JID, BLECharacteristic::PROPERTY_WRITE);
    characteristic->setCallbacks(callback);
    characteristic->addDescriptor(getNewCUDDescriptor("JID"));

    // JID Password:
    characteristic = service->createCharacteristic(UUID_CHARACTERISTIC_JID_PASSWORD, BLECharacteristic::PROPERTY_WRITE);
    characteristic->setCallbacks(callback);
    characteristic->addDescriptor(getNewCUDDescriptor("JID Password"));

    // JID Sender:
    characteristic = service->createCharacteristic(UUID_CHARACTERISTIC_JID_SENDER, BLECharacteristic::PROPERTY_WRITE);
    characteristic->setCallbacks(callback);
    characteristic->addDescriptor(getNewCUDDescriptor("JID Sender"));
}

void BLEServiceHelper::initDeviceSettings2Service(BLECharacteristicCallbacks* callback, BLEServer* server) {
    BLEService* service = server->createService(UUID_SERVICE_DEVICE_SETTINGS_2);

    // Settings done:
    // Has to be added here since it looks like there is a limit of 5 characteristics per service:
    BLECharacteristic* characteristic = service->createCharacteristic(UUID_CHARACTERISTIC_SETTINGS_DONE, BLECharacteristic::PROPERTY_WRITE);
    characteristic->setCallbacks(callback);
    characteristic->addDescriptor(getNewCUDDescriptor("Settings Done"));
}
//---------------------------------------------------------------------------
}  // namespace esp32jura::esp::bt
   //---------------------------------------------------------------------------