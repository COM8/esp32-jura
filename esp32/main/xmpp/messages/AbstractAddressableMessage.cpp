#include "AbstractAddressableMessage.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp::messages {
//---------------------------------------------------------------------------
AbstractAddressableMessage::AbstractAddressableMessage(const Jid from, const Jid to) : AbstractMessage(), from(from), to(to) {}
AbstractAddressableMessage::AbstractAddressableMessage(const Jid from, const Jid to, const std::string&& id) : AbstractMessage(std::move(id)), from(std::move(from)), to(std::move(to)) {}
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp::messages
//---------------------------------------------------------------------------
