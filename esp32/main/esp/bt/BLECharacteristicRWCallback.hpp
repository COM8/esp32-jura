#pragma once

#include "BLECharacteristic.h"

//---------------------------------------------------------------------------
namespace esp32jura::esp::bt {
//---------------------------------------------------------------------------
class BLECharacteristicRWCallback : public BLECharacteristicCallbacks {
   private:
   public:
    BLECharacteristicRWCallback() = default;
    ~BLECharacteristicRWCallback() = default;

    void onRead(BLECharacteristic* characteristic) override;
    void onWrite(BLECharacteristic* characteristic) override;
};
//---------------------------------------------------------------------------
}  // namespace esp32jura::esp::bt
   //---------------------------------------------------------------------------