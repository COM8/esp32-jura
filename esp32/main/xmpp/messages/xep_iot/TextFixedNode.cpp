#include "TextFixedNode.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp::messages::xep_iot {
//---------------------------------------------------------------------------
TextFixedNode::TextFixedNode(const std::string&& id, std::string&& text) : SensorNode(std::move(id), "text-single", ""), text(std::move(text)) {}

Type TextFixedNode::get_type() const { return Type::TextFixedNodeType; }

void TextFixedNode::to_ui_field(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* xNode) const {
    tinyxml2::XMLElement* node = gen_field_node(doc, nullptr, "Status:");
    node->InsertEndChild(doc.NewElement("xdd:readOnly"));
    xNode->InsertEndChild(node);
}

const std::string TextFixedNode::get_value_str() const { return text; }

bool TextFixedNode::on_value_changed(const tinyxml2::XMLElement* valNode) {
    text = std::string{valNode->GetText()};
    return false;
}
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp::messages::xep_iot
//---------------------------------------------------------------------------
