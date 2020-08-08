#include "TcpConnection.hpp"

#include <smooth/core/Task.h>

#include <chrono>
#include <iostream>

//---------------------------------------------------------------------------
namespace esp32jura::xmpp::tcp {
//---------------------------------------------------------------------------
TcpConnection::TcpConnection(const XmppAccount* account, smooth::core::Task& task, ConnectionEventListener& connectionStatusChanged, XmppPacketAvailableListener& xmppPacketAvailable)
    : account(account), task(task), connectionStatusChanged(connectionStatusChanged), xmppPacketAvailable(xmppPacketAvailable), buffer(nullptr), socket(nullptr) {}

void TcpConnection::connect() {
    if (!socket) {
        std::unique_ptr<XmppProtocol> protocol = std::make_unique<XmppProtocol>();
        buffer = std::make_shared<smooth::core::network::BufferContainer<XmppProtocol>>(task, *this, *this, *this, std::move(protocol));

        socket = smooth::core::network::Socket<XmppProtocol>::create(buffer, static_cast<std::chrono::milliseconds>(5000), static_cast<std::chrono::milliseconds>(-1));
        bool result = socket->start(account->server);
        if (!result) {
            std::cerr << "Socket start failed!" << std::endl;
        }
    }
}

bool TcpConnection::send(std::string& msg) {
    std::cout << "Send: " << msg << "\n";
    return socket->send(static_cast<XmppPacket>(msg));
}

bool TcpConnection::send(std::wstring& msg) { return socket->send(static_cast<XmppPacket>(msg)); }

void TcpConnection::disconnect() {}

void TcpConnection::event(const smooth::core::network::event::TransmitBufferEmptyEvent&) {}

void TcpConnection::event(const smooth::core::network::event::DataAvailableEvent<XmppProtocol>& event) {
    XmppPacket packet;
    event.get(packet);
    xmppPacketAvailable.event(packet);
}

void TcpConnection::event(const smooth::core::network::event::ConnectionStatusEvent& event) {
    connectionStatusChanged.event(event);
    if (event.is_connected()) {
        std::cout << "TcpConnection connected\n";
    } else {
        std::cout << "TcpConnection disconnected\n";
    }
}

void TcpConnection::event(const smooth::core::network::NetworkStatus& event) { std::cout << "EVENT NetworkStatus" << std::endl; }
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp::tcp
   //---------------------------------------------------------------------------