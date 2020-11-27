#pragma once

#include <smooth/core/Task.h>
#include <smooth/core/ipc/IEventListener.h>
#include <smooth/core/ipc/SubscribingTaskEventQueue.h>
#include <smooth/core/network/IPv4.h>
#include <smooth/core/network/NetworkStatus.h>

#include <memory>

#include "INonConstEventListener.hpp"
#include "esp/Storage.hpp"
#include "jura/CoffeeMaker.hpp"
#include "messages/Message.hpp"
#include "xmpp/XmppClient.hpp"
#include "xmpp/messages/xep_iot/Device.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp {
//---------------------------------------------------------------------------
class XmppTask : public smooth::core::Task,
                 public smooth::core::ipc::IEventListener<smooth::core::network::NetworkStatus>,
                 public smooth::core::ipc::IEventListener<XmppClientConnectionState>,
                 public INonConstEventListener<messages::Message> {
   private:
    using NetworkStatusQueue = smooth::core::ipc::SubscribingTaskEventQueue<smooth::core::network::NetworkStatus>;
    std::shared_ptr<NetworkStatusQueue> net_status;

    std::shared_ptr<esp::Storage> storage;

    std::shared_ptr<xmpp::XmppClient> client;
    std::unique_ptr<messages::xep_iot::Device> iotDevice;

    jura::CoffeeMaker coffeeMaker{};

   public:
    XmppTask(std::shared_ptr<esp::Storage> storage);
    ~XmppTask();

    void init() override;
    void tick() override;
    void handlePresenceMessages(const tinyxml2::XMLElement* elem);
    void handleMessageMessages(const tinyxml2::XMLElement* elem);

    void event(const smooth::core::network::NetworkStatus& event) override;
    void event(const XmppClientConnectionState& event) override;
    void event(messages::Message& event) override;

   private:
    void onReady();
    void handleIoTMessageMessage(const char* msg);

    void add_button(const std::string& id, std::string&& label);
    void add_text_single(const std::string& id, std::string&& label, std::string&& text, bool readonly);
    void add_header(const std::string& id, std::string&& text);
    void add_slider(const std::string& id, std::string&& label, double min, double max, double value, double steps);

    void on_button_pressed(const std::string& id);
    void update_status(const std::string& msg);
};
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp
   //---------------------------------------------------------------------------