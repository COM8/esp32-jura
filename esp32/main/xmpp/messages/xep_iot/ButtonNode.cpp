#include "ButtonNode.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp::messages::xep_iot {
//---------------------------------------------------------------------------
ButtonNode::ButtonNode(const std::string&& id, const std::string&& title) : ActuatorNode(std::move(id), "button"), title(std::move(title)) {}

Type ButtonNode::get_type() const { return Type::ButtonNode; }

void ButtonNode::to_ui_field(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* xNode) const {
    tinyxml2::XMLElement* node = gen_field_node(doc, nullptr, title.c_str());
    xNode->InsertEndChild(node);
}

const std::string ButtonNode::get_value_str() const { return "0"; }
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp::messages::xep_iot
//---------------------------------------------------------------------------
