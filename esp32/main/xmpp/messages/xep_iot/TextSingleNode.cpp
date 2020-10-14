#include "TextSingleNode.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp::messages::xep_iot {
//---------------------------------------------------------------------------
TextSingleNode::TextSingleNode(const std::string&& id, std::string&& text) : SensorNode(std::move(id), "text-single", ""), text(std::move(text)) {}

Type TextSingleNode::get_type() const { return Type::TextSingleNodeType; }

void TextSingleNode::to_ui_field(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* xNode) const {
    tinyxml2::XMLElement* node = gen_field_node(doc, nullptr, text.c_str());
    xNode->InsertEndChild(node);
}

const std::string TextSingleNode::get_value_str() const { return text; }

bool TextSingleNode::on_value_changed(const tinyxml2::XMLElement* valNode) {
    text = std::string{valNode->GetText()};
    return false;
}
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp::messages::xep_iot
//---------------------------------------------------------------------------
