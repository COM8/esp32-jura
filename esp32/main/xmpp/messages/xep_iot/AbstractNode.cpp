#include "AbstractNode.hpp"

#include "xmpp/helpers/PubSubHelper.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp::messages::xep_iot {
//---------------------------------------------------------------------------
AbstractNode::AbstractNode(std::string&& id, std::string&& fieldType) : id(std::move(id)), fieldType(std::move(fieldType)) {}

tinyxml2::XMLElement* AbstractNode::gen_field_node(tinyxml2::XMLDocument& doc, const char* value, const char* label) const {
    return helpers::PubSubHelper::gen_field_node(doc, id.c_str(), fieldType.c_str(), value, label);
}

const std::string& AbstractNode::get_id() const { return id; }
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp::messages::xep_iot
//---------------------------------------------------------------------------
