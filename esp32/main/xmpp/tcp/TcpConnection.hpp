#pragma once

#include <smooth/application/network/http/IResponseOperation.h>
#include <smooth/core/ipc/IEventListener.h>
#include <smooth/core/network/BufferContainer.h>
#include <smooth/core/network/Socket.h>
#include <smooth/core/network/event/ConnectionStatusEvent.h>

#include <memory>
#include <string>

#include "../XmppAccount.hpp"
#include "XmppPacket.hpp"
#include "XmppProtocol.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp::tcp {
//---------------------------------------------------------------------------
class TcpConnection : public smooth::core::ipc::IEventListener<smooth::core::network::event::TransmitBufferEmptyEvent>,
                      public smooth::core::ipc::IEventListener<smooth::core::network::event::DataAvailableEvent<XmppProtocol>>,
                      public smooth::core::ipc::IEventListener<smooth::core::network::event::ConnectionStatusEvent>,
                      public smooth::core::ipc::IEventListener<smooth::core::network::NetworkStatus> {
   private:
    using ConnectionEventListener = smooth::core::ipc::IEventListener<smooth::core::network::event::ConnectionStatusEvent>;
    using XmppPacketAvailableListener = smooth::core::ipc::IEventListener<XmppPacket>;

    const XmppAccount* account;
    smooth::core::Task& task;
    ConnectionEventListener& connectionStatusChanged;
    XmppPacketAvailableListener& xmppPacketAvailable;

    std::shared_ptr<smooth::core::network::BufferContainer<XmppProtocol>> buffer;
    std::shared_ptr<smooth::core::network::Socket<XmppProtocol>> socket;

   public:
    TcpConnection(const XmppAccount* account, smooth::core::Task& task, ConnectionEventListener& connectionStatusChanged, XmppPacketAvailableListener& xmppPacketAvailable);

    void event(const smooth::core::network::event::TransmitBufferEmptyEvent&) override;
    void event(const smooth::core::network::event::DataAvailableEvent<XmppProtocol>&) override;
    void event(const smooth::core::network::event::ConnectionStatusEvent&) override;
    void event(const smooth::core::network::NetworkStatus& event) override;

    void connect();
    void disconnect();

    bool send(std::string& msg);
    bool send(std::wstring& msg);
};
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp::tcp
   //---------------------------------------------------------------------------