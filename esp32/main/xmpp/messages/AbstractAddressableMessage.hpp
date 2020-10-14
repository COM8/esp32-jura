#pragma once

#include <string>

#include "AbstractMessage.hpp"
#include "xmpp/Jid.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp::messages {
//---------------------------------------------------------------------------
class AbstractAddressableMessage : public AbstractMessage {
   public:
    const Jid from;
    const Jid to;

   public:
    AbstractAddressableMessage(const Jid from, const Jid to);
    AbstractAddressableMessage(const Jid from, const Jid to, const std::string&& id);
    ~AbstractAddressableMessage() = default;
};
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp::messages
//---------------------------------------------------------------------------
