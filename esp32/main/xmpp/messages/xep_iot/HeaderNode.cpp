#include "HeaderNode.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp::messages::xep_iot {
//---------------------------------------------------------------------------
HeaderNode::HeaderNode(const std::string&& id, std::string&& text) : SensorNode(std::move(id), "header", "", false), text(std::move(text)) {}

Type HeaderNode::get_type() const { return Type::HeaderNodeType; }

void HeaderNode::to_ui_field(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* xNode) const {
    tinyxml2::XMLElement* node = gen_field_node(doc, nullptr, text.c_str());
    xNode->InsertEndChild(node);
}

bool HeaderNode::on_value_changed(const tinyxml2::XMLElement* valNode) { return false; }
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp::messages::xep_iot
//---------------------------------------------------------------------------
