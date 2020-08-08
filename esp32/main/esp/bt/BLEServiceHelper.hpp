#pragma once

#include <string>
#include <vector>

#include "BLE2902.h"
#include "BLE2904.h"
#include "BLECharacteristic.h"
#include "BLEServer.h"
#include "BLEUUID.h"

//---------------------------------------------------------------------------
namespace esp32jura::esp::bt {
//---------------------------------------------------------------------------
class BLEServiceHelper {
   private:
    std::string btMac;
    // Client Characteristic Configuration
    // Source: https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Descriptors/org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
    std::vector<BLE2902*> cCCDescriptors;
    // Characteristic User Description
    // Source: https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Descriptors/org.bluetooth.descriptor.gatt.characteristic_user_description.xml
    std::vector<BLEDescriptor*> cUDDescriptors;

    BLE2902* getNewCCCDescriptor(bool enableNotify, bool enableIndicate);
    BLEDescriptor* getNewCUDDescriptor(std::string description);
    void initDeviceInfoService(BLECharacteristicCallbacks* callback, BLEServer* server);
    void initDeviceSettings1Service(BLECharacteristicCallbacks* callback, BLEServer* server);
    void initDeviceSettings2Service(BLECharacteristicCallbacks* callback, BLEServer* server);

   public:
    //-------------------
    // Bluetooth characteristic UUIDs:
    // Source: https://www.bluetooth.com/specifications/gatt/characteristics/
    // Generation:
    // Base: 00000000-0000-1000-8000-00805F9B34FB + Assigned Number (e.g. 0x2A80 for Age)
    // = 00002A80-0000-1000-8000-00805F9B34FB
    //-------------------

    // Format: utf8s (The Language definition is based on ISO639-1.)
    // Source: https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.language.xml
    static const BLEUUID UUID_CHARACTERISTIC_LANGUAGE;
    // Format: utf8s
    // Source: https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.hardware_revision_string.xml
    static const BLEUUID UUID_CHARACTERISTIC_HARDWARE_REVISION;
    // Format: utf8s
    // Source: https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.software_revision_string.xml
    static const BLEUUID UUID_CHARACTERISTIC_SOFTWARE_REVISION;
    // Format: utf8s
    // Source: https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.serial_number_string.xml
    static const BLEUUID UUID_CHARACTERISTIC_SERIAL_NUMBER;
    // Format: utf8s
    // Source: https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.manufacturer_name_string.xml
    static const BLEUUID UUID_CHARACTERISTIC_MANUFACTURER_NAME;
    // Custom UUID
    static const BLEUUID UUID_CHARACTERISTIC_WIFI_SSID;
    // Custom UUID
    static const BLEUUID UUID_CHARACTERISTIC_WIFI_PASSWORD;
    // Custom UUID
    static const BLEUUID UUID_CHARACTERISTIC_JID;
    // Custom UUID
    static const BLEUUID UUID_CHARACTERISTIC_JID_PASSWORD;
    // Custom UUID
    static const BLEUUID UUID_CHARACTERISTIC_JID_SENDER;
    // Custom UUID
    static const BLEUUID UUID_CHARACTERISTIC_SETTINGS_DONE;
    // Custom UUID

    //-------------------
    // Bluetooth service UUIDs:
    // Source: https://www.bluetooth.com/specifications/gatt/services/
    // Generation:
    // Base: 00000000-0000-1000-8000-00805F9B34FB + Assigned Number (e.g. 0x1800 for Generic Access)
    // = 00001800-0000-1000-8000-00805F9B34FB
    //-------------------
    // Source: https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Services/org.bluetooth.service.device_information.xml
    static const BLEUUID UUID_SERVICE_DEVICE_INFORMATION;
    // Custom UUID
    static const BLEUUID UUID_SERVICE_DEVICE_SETTINGS_1;
    // Custom UUID
    static const BLEUUID UUID_SERVICE_DEVICE_SETTINGS_2;

    BLEServiceHelper(std::string btMac);
    ~BLEServiceHelper();

    void init(BLECharacteristicCallbacks* callback, BLEServer* server);
    void start(BLEServer* server);
    void stop(BLEServer* server);
};
//---------------------------------------------------------------------------
}  // namespace esp32jura::esp::bt
   //---------------------------------------------------------------------------