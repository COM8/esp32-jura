#include "ButtonNode.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp::messages::xep_iot {
//---------------------------------------------------------------------------
ButtonNode::ButtonNode(std::string&& id, std::string&& title, std::function<void(const std::string& id)>&& onButtonPressed)
    : ActuatorNode(std::move(id), "button"), title(std::move(title)), onButtonPressed(std::move(onButtonPressed)) {}

Type ButtonNode::get_type() const { return Type::ButtonNodeType; }

void ButtonNode::to_ui_field(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* xNode) const {
    tinyxml2::XMLElement* node = gen_field_node(doc, nullptr, title.c_str());
    xNode->InsertEndChild(node);
}

const std::string ButtonNode::get_value_str() const { return "0"; }

// NOLINTNEXTLINE (readability-convert-member-functions-to-static)
bool ButtonNode::on_value_changed(const tinyxml2::XMLElement* valNode) {
    if (std::string{valNode->GetText()} != "0") {
        onButtonPressed(id);
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp::messages::xep_iot
//---------------------------------------------------------------------------
