#include "XmppAccount.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp {
//---------------------------------------------------------------------------
XmppAccount::XmppAccount(const Jid&& jid, const std::string&& password, const InetAddress_sp server) : jid(jid), password(password), server(server) {}
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp
   //---------------------------------------------------------------------------