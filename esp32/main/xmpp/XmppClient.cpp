#include "XmppClient.hpp"

#include <iostream>

#include "XmppUtils.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp {
//---------------------------------------------------------------------------
XmppClient::XmppClient(const XmppAccount&& account, smooth::core::Task& task, ConnectionStatusEventListener& connectionStatusChanged)
    : state(CLIENT_DISCONNECTED), account(account), connection(&(this->account), task, *this, *this), task(task), connectionStatusChanged(connectionStatusChanged), messageListener() {}
void XmppClient::connect() {
    if (state == CLIENT_DISCONNECTED) {
        std::cout << "XMPP Client connecting..." << std::endl;
        setState(CLIENT_CONNECTING);
        connection.connect();
    }
}

void XmppClient::disconnect() {
    if (state == CLIENT_CONNECTING || state == CLIENT_CONNECTED) {
        std::cout << "XMPP Client disconnecting..." << std::endl;
        setState(CLIENT_DISCONNECTING);
        connection.disconnect();
    }
}

bool XmppClient::isConnected() { return state == CLIENT_CONNECTED; }

void XmppClient::setState(XmppClientConnectionState state) {
    if (state != this->state) {
        this->state = state;
        connectionStatusChanged.event(state);
    }
}

XmppClientConnectionState XmppClient::getState() { return this->state; }

void XmppClient::send(const std::string& msg) { connection.send(msg); }

void XmppClient::send(const std::wstring& msg) { connection.send(msg); }

void XmppClient::sendMessage(const std::string& to, const std::string& body) {
    std::string msg = "<message from='" + account.jid.get_full() + "' id='" + generate_uuid_v4() + "' to='" + to + "' type='chat' xml:lang='en'><body>" + body + "</body></message>";
    send(msg);
}

void XmppClient::subscribeToMessagesListener(MessageListener* messageListener) { this->messageListener.push_back(messageListener); }

void XmppClient::unsubscribeFromMessagesListener(MessageListener* messageListener) {
    auto it = this->messageListener.begin();
    while (it != this->messageListener.end()) {
        if (*it == messageListener) {
            this->messageListener.erase(it);
            return;
        }
    }
}

void XmppClient::addToRoster(std::string& bareJid) {
    std::string msg = "<iq type='get' from='" + account.jid.get_full() + "' to='" + account.jid.domainPart + "' id='" + generate_uuid_v4() + "'><query xmlns='jabber:iq:roster'><item jid='" + bareJid +
                      "' name='Nurse'></item></query></iq>";
    send(msg);
}

void XmppClient::approvePresenceSubscription(std::string& bareJid) {
    std::string msg = "<presence to='" + bareJid + "' type='subscribed'/>";
    send(msg);
}

void XmppClient::refusePresenceSubscription(std::string& bareJid) {
    std::string msg = "<presence to='" + bareJid + "' type='unsubscribed'/>";
    send(msg);
}

void XmppClient::event(const XmppConnectionState& event) {
    if (event == CONNECTED) {
        std::cout << "XMPP client connected\n";
        setState(CLIENT_CONNECTED);
    } else if (event == DISCONNECTED) {
        std::cout << "XMPP client disconnected\n";
        setState(CLIENT_DISCONNECTED);
    }
}

void XmppClient::event(messages::Message& event) {
    for (MessageListener* item : messageListener) {
        item->event(event);
    }
}
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp
   //---------------------------------------------------------------------------