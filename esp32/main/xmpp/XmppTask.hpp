#pragma once

#include <smooth/core/Task.h>
#include <smooth/core/ipc/IEventListener.h>
#include <smooth/core/ipc/SubscribingTaskEventQueue.h>
#include <smooth/core/network/IPv4.h>
#include <smooth/core/network/NetworkStatus.h>

#include <memory>

#include "INonConstEventListener.hpp"
#include "esp/Storage.hpp"
#include "helpers/PubSubHelper.hpp"
#include "messages/Message.hpp"
#include "xmpp/XmppClient.hpp"

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

    esp::Storage& storage;

    std::shared_ptr<xmpp::XmppClient> client;
    std::unique_ptr<helpers::PubSubHelper> pubSubHelper;

    static const std::string INITIAL_HELLO_MESSAGE;

    void onReady();
    void handleIoTMessageMessage(const char* msg);
    void handlePubSubEventMessage(const tinyxml2::XMLElement* elem);

   public:
    XmppTask(esp::Storage& storage);
    ~XmppTask();

    void init() override;
    void tick() override;
    void handlePresenceMessages(const tinyxml2::XMLElement* elem);
    void handleMessageMessages(const tinyxml2::XMLElement* elem);

    void event(const smooth::core::network::NetworkStatus& event) override;
    void event(const XmppClientConnectionState& event) override;
    void event(messages::Message& event) override;
};
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp
   //---------------------------------------------------------------------------