#pragma once

#include "AbstractNode.hpp"
#include "xmpp/Jid.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp::messages::xep_iot {
//---------------------------------------------------------------------------
class ActuatorNode : public AbstractNode {
   public:
    static const std::string XMPP_IOT_ACTUATORS;

   public:
    ActuatorNode(const std::string&& id, const std::string&& fieldType);
    ~ActuatorNode() override = default;

    void gen_actuator_node(tinyxml2::XMLDocument& doc, const xmpp::Jid& from, const std::string& iot_ns) const;
    virtual const std::string get_value_str() const = 0;

   protected:
    NodeType get_node_type() const override;
};
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp::messages::xep_iot
//---------------------------------------------------------------------------
