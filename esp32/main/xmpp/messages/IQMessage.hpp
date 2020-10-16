#pragma once

#include <tinyxml2.h>

#include <string>

#include "AbstractAddressableMessage.hpp"
#include "xmpp/Jid.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp::messages {
//---------------------------------------------------------------------------
enum class IQType { SET = 0, GET = 1, RESULT = 2, ERROR = 3 };

class IQMessage : public AbstractAddressableMessage {
   public:
    const IQType iqType;

   public:
    IQMessage(const Jid from, const Jid to, IQType iqType, std::string&& id);
    IQMessage(const Jid from, const Jid to, IQType iqType);
    IQMessage(std::string&& id, IQType iqType);
    IQMessage(IQType iqType);
    IQMessage(const tinyxml2::XMLElement* node);
    virtual ~IQMessage() = default;

    MessageType get_type() override;

   private:
    static Jid get_from(const tinyxml2::XMLElement* node);
    static Jid get_to(const tinyxml2::XMLElement* node);
    static std::string get_id(const tinyxml2::XMLElement* node);
    static IQType get_iq_type(const tinyxml2::XMLElement* node);

   protected:
    [[nodiscard]] tinyxml2::XMLElement* to_xml_elem(tinyxml2::XMLDocument& doc) const;
    [[nodiscard]] virtual tinyxml2::XMLElement* get_query(tinyxml2::XMLDocument& doc) const;
};
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp::messages
//---------------------------------------------------------------------------
