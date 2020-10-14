#include "BindResourceMessage.hpp"

#include "xmpp/Jid.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp::messages {
//---------------------------------------------------------------------------
BindResourceMessage::BindResourceMessage(const std::string&& resource) : IQMessage(IQType::SET), resource(std::move(resource)) {}

MessageType IQMessage::get_type() { return MessageType::BindResourceMessageType; }

tinyxml2::XMLElement* BindResourceMessage::get_query(tinyxml2::XMLDocument& doc) const {
    tinyxml2::XMLElement* bindNode = doc.NewElement("bind");
    bindNode->SetAttribute("xmlns", "urn:ietf:params:xml:ns:xmpp-bind");
    tinyxml2::XMLElement* resourceNode = doc.NewElement("resource");
    resourceNode->SetText(resource.c_str());
    bindNode->InsertEndChild(resourceNode);
    return bindNode;
}
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp::messages
//---------------------------------------------------------------------------
