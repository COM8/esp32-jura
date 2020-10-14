#pragma once

#include <tinyxml2.h>

#include <string>

#include "IQMessage.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp::messages {
//---------------------------------------------------------------------------
class BindResourceMessage : public IQMessage {
   public:
    const std::string resource;

   public:
    BindResourceMessage(const std::string&& resource);
    ~BindResourceMessage() = default;

    MessageType get_type() override;

   protected:
    [[nodiscard]] tinyxml2::XMLElement* get_query(tinyxml2::XMLDocument& doc) const override;
};
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp::messages
//---------------------------------------------------------------------------
