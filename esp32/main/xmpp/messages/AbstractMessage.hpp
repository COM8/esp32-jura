#pragma once

#include <tinyxml2.h>

#include <string>

#include "xmpp/XmppUtils.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp::messages {
//---------------------------------------------------------------------------
enum class Restart { NO_RESTART = 0, HARD_RESTART = 1, SOFT_RESTART = 2 };

enum class MessageType { IQMessageType = 0, BindResourceMessageType = 1 };

class AbstractMessage {
   public:
    const std::string id;
    Restart restart{Restart::NO_RESTART};

   public:
    AbstractMessage(std::string&& id = generate_uuid_v4());
    ~AbstractMessage() = default;

   protected:
    [[nodiscard]] virtual tinyxml2::XMLElement* to_xml_elem(tinyxml2::XMLDocument& doc) const = 0;

   public:
    std::string to_xml_str() const;
    virtual MessageType get_type() = 0;
};
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp::messages
//---------------------------------------------------------------------------
