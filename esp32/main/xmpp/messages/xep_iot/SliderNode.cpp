#include "SliderNode.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp::messages::xep_iot {
//---------------------------------------------------------------------------
SliderNode::SliderNode(std::string&& id, std::string&& title, double min, double max, double value, double steps)
    : ActuatorNode(std::move(id), "slider"), title(std::move(title)), min(min), max(max), steps(steps), value(value) {}

Type SliderNode::get_type() const { return Type::SliderNodeType; }

void SliderNode::to_ui_field(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* xNode) const {
    tinyxml2::XMLElement* node = gen_field_node(doc, nullptr, title.c_str());
    node->SetAttribute("min", min);
    node->SetAttribute("max", max);
    node->SetAttribute("steps", steps);
    xNode->InsertEndChild(node);
}

const std::string SliderNode::get_value_str() const { return std::to_string(value); }

bool SliderNode::on_value_changed(const tinyxml2::XMLElement* valNode) {
    value = valNode->DoubleText();
    return false;
}
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp::messages::xep_iot
//---------------------------------------------------------------------------
