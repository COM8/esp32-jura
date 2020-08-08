#pragma once

#include <smooth/core/network/InetAddress.h>

#include <memory>
#include <string>

#include "Jid.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp {
//---------------------------------------------------------------------------
class XmppAccount {
   public:
    using InetAddress_sp = std::shared_ptr<smooth::core::network::InetAddress>;

    const Jid jid;
    const std::string password;
    InetAddress_sp server;

    XmppAccount(const Jid&& jid, const std::string&& password, const InetAddress_sp server);
};
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp
   //---------------------------------------------------------------------------