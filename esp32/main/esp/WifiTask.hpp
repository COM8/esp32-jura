#pragma once

#include <smooth/core/Task.h>
#include <smooth/core/io/Output.h>
#include <smooth/core/ipc/IEventListener.h>
#include <smooth/core/ipc/SubscribingTaskEventQueue.h>
#include <smooth/core/network/NetworkStatus.h>
#include <smooth/core/network/Wifi.h>

#include <memory>

#include "actuator/RgbLed.hpp"
#include "esp/Storage.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::esp {
//---------------------------------------------------------------------------
class WifiTask : public smooth::core::Task, public smooth::core::ipc::IEventListener<smooth::core::network::NetworkStatus> {
   private:
    std::shared_ptr<smooth::core::network::Wifi> wifi;
    std::shared_ptr<actuator::RgbLed> rgbLed;
    std::shared_ptr<esp::Storage> storage;

    using NetworkStatusQueue = smooth::core::ipc::SubscribingTaskEventQueue<smooth::core::network::NetworkStatus>;
    std::shared_ptr<NetworkStatusQueue> net_status;

   public:
    WifiTask(std::shared_ptr<smooth::core::network::Wifi> wifi, std::shared_ptr<actuator::RgbLed> rgbLed, std::shared_ptr<esp::Storage> storage);

    void init() override;

    void event(const smooth::core::network::NetworkStatus& event) override;
};
//---------------------------------------------------------------------------
}  // namespace esp32jura::esp
   //---------------------------------------------------------------------------