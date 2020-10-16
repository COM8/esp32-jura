#include "ActuatorNode.hpp"

#include "xmpp/helpers/PubSubHelper.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp::messages::xep_iot {
//---------------------------------------------------------------------------
const std::string ActuatorNode::XMPP_IOT_ACTUATORS = "xmpp.iot.actuators";

ActuatorNode::ActuatorNode(std::string&& id, std::string&& fieldType) : AbstractNode(std::move(id), std::move(fieldType)) {}

NodeType ActuatorNode::get_node_type() const { return NodeType::ACTUATOR; }

void ActuatorNode::gen_actuator_node(tinyxml2::XMLDocument& doc, const xmpp::Jid& from, const std::string& iot_ns) const {
    tinyxml2::XMLElement* itemNode = helpers::PubSubHelper::gen_publish_item_node(doc, from, XMPP_IOT_ACTUATORS.c_str(), id.c_str());
    tinyxml2::XMLElement* valNode = doc.NewElement("val");
    valNode->SetAttribute("xmlns", iot_ns.c_str());
    valNode->SetAttribute("type", fieldType.c_str());
    // valNode->SetAttribute("unit", "");  // Not needed here
    valNode->SetText(get_value_str().c_str());
    itemNode->InsertEndChild(valNode);
}
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp::messages::xep_iot
//---------------------------------------------------------------------------
