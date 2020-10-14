#include "XmppConnection.hpp"

#include "crypto/CryptoUtils.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp {
//---------------------------------------------------------------------------
XmppConnection::XmppConnection(const XmppAccount* account, smooth::core::Task& task, StateChangedListener& stateChangedListener, MessageListener& messageListener)
    : account(account), tcpConnection(account, task, *this, *this), task(task), state(DISCONNECTED), stateChangedListener(stateChangedListener), messageListener(messageListener) {}

void XmppConnection::connect() {
    if (state == DISCONNECTED) {
        setState(CONNECTING);
        tcpConnection.connect();
    }
}

void XmppConnection::disconnect() { tcpConnection.disconnect(); }

std::string XmppConnection::genInitialStreamHeader() {
    return "<?xml version='1.0'?><stream:stream from='" + account->jid.getBare() + "' to='" + account->jid.domainPart +
           "' version='1.0' xml:lang='en' xmlns='jabber:client' xmlns:stream='http://etherx.jabber.org/streams'>";
}

std::string XmppConnection::genPlainAuthMessage() {
    std::string nulStr("\0", 1);
    std::string passwordBase64 = crypto::toBase64(nulStr + account->jid.userPart + nulStr + account->password);
    return "<auth xmlns='urn:ietf:params:xml:ns:xmpp-sasl' mechanism='PLAIN'>" + passwordBase64 + "</auth>";
}

std::string XmppConnection::genResourceBindMessage() {
    return "<iq id='yhc13a95' type='set'><bind xmlns='urn:ietf:params:xml:ns:xmpp-bind'><resource>" + account->jid.resourcePart + "</resource></bind></iq>";
}

std::string XmppConnection::genPresenceMessage() { return "<presence/>"; }

void XmppConnection::send(const std::string& msg) { tcpConnection.send(msg); }

void XmppConnection::send(const std::wstring& msg) { tcpConnection.send(msg); }

void XmppConnection::setState(XmppConnectionState state) {
    if (state != this->state) {
        this->state = state;
        stateChangedListener.event(state);
    }
}

XmppConnectionState XmppConnection::getState() const { return state; }

void XmppConnection::onInitialStreamHeaderReply(const tcp::XmppPacket& packet) {
    std::string s = packet.to_string();
    if (s.find("stream:features") != std::string::npos) {
        std::string msg = genPlainAuthMessage();
        setState(SASL_PLAIN_AUTH_SEND);
        tcpConnection.send(msg);
    }
}

void XmppConnection::onSaslAuthMessageReply(const tcp::XmppPacket& packet) {
    std::string s = packet.to_string();
    if (s.find("success") != std::string::npos) {
        std::string msg = genInitialStreamHeader();
        setState(SASL_SOFT_STREAM_RESET_SEND);
        tcpConnection.send(msg);
    }
}

void XmppConnection::onSaslAuthRestartStreamHeaderReply(const tcp::XmppPacket& packet) {
    std::string s = packet.to_string();
    if (s.find("urn:ietf:params:xml:ns:xmpp-bind") != std::string::npos) {
        std::string msg = genResourceBindMessage();
        setState(RESOURCE_BINDING_REQUEST_SEND);
        tcpConnection.send(msg);
    }
}

void XmppConnection::onResourceBindingReply(const tcp::XmppPacket& packet) {
    std::string s = packet.to_string();
    if (s.find(account->jid.getFull()) != std::string::npos) {
        std::string msg = genPresenceMessage();
        tcpConnection.send(msg);
        setState(CONNECTED);
    }
}

void XmppConnection::event(const smooth::core::network::event::ConnectionStatusEvent& event) {
    if (event.is_connected()) {
        std::string msg = genInitialStreamHeader();
        setState(INITIAL_STREAM_HEADER_SEND);
        tcpConnection.send(msg);
    } else {
    }
}

void XmppConnection::event(const tcp::XmppPacket& event) {
    std::wcout << "Received: " << event.to_wstring() << "\n";

    switch (state) {
        case INITIAL_STREAM_HEADER_SEND:
            onInitialStreamHeaderReply(event);
            break;

        case SASL_PLAIN_AUTH_SEND:
            onSaslAuthMessageReply(event);
            break;

        case SASL_SOFT_STREAM_RESET_SEND:
            onSaslAuthRestartStreamHeaderReply(event);
            break;

        case RESOURCE_BINDING_REQUEST_SEND:
            onResourceBindingReply(event);
            break;

        default:
            break;
    }

    messages::Message msg(event.get_data_vec());
    messageListener.event(msg);
}
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp
   //---------------------------------------------------------------------------