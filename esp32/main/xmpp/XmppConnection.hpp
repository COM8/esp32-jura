#pragma once

#include <smooth/core/Task.h>
#include <smooth/core/ipc/IEventListener.h>
#include <smooth/core/network/event/ConnectionStatusEvent.h>
#include <smooth/core/network/event/DataAvailableEvent.h>

#include "INonConstEventListener.hpp"
#include "XmppAccount.hpp"
#include "messages/Message.hpp"
#include "tcp/TcpConnection.hpp"
#include "tcp/XmppPacket.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp {
//---------------------------------------------------------------------------
enum XmppConnectionState { DISCONNECTED, CONNECTING, INITIAL_STREAM_HEADER_SEND, SASL_PLAIN_AUTH_SEND, SASL_SOFT_STREAM_RESET_SEND, RESOURCE_BINDING_REQUEST_SEND, CONNECTED, DISCONNECTING, ERROR };

class XmppConnection : public smooth::core::ipc::IEventListener<smooth::core::network::event::ConnectionStatusEvent>, public smooth::core::ipc::IEventListener<tcp::XmppPacket> {
   public:
    const XmppAccount* account;

   private:
    tcp::TcpConnection tcpConnection;
    smooth::core::Task& task;

    XmppConnectionState state;
    void setState(XmppConnectionState state);

   public:
    using StateChangedListener = smooth::core::ipc::IEventListener<XmppConnectionState>;
    StateChangedListener& stateChangedListener;
    using MessageListener = INonConstEventListener<messages::Message>;
    MessageListener& messageListener;

    XmppConnection(const XmppAccount* account, smooth::core::Task& task, StateChangedListener& stateChangedListener, MessageListener& messageListener);

    void send(const std::string& msg);
    void send(const std::wstring& msg);

    [[nodiscard]] XmppConnectionState getState() const;

    void connect();
    void disconnect();

    void event(const smooth::core::network::event::ConnectionStatusEvent& event) override;
    void event(const tcp::XmppPacket& event) override;

   private:
    std::string genInitialStreamHeader();
    std::string genPlainAuthMessage();
    std::string genResourceBindMessage();
    std::string genPresenceMessage();

    void onInitialStreamHeaderReply(const tcp::XmppPacket& packet);
    void onSaslAuthMessageReply(const tcp::XmppPacket& packet);
    void onSaslAuthRestartStreamHeaderReply(const tcp::XmppPacket& packet);
    void onResourceBindingReply(const tcp::XmppPacket& packet);
};
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp
   //---------------------------------------------------------------------------