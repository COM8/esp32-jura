#pragma once

#include "AbstractNode.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp::messages::xep_iot {
//---------------------------------------------------------------------------
class SensorNode : public AbstractNode {
   protected:
    const std::string unit;

   public:
    static const std::string XMPP_IOT_SENSORS;

   public:
    SensorNode(const std::string&& id, const std::string&& fieldType, const std::string&& unit);
    ~SensorNode() override = default;

    void gen_sensor_node(tinyxml2::XMLDocument& doc, const xmpp::Jid& from, const std::string& iot_ns) const;
    const std::string get_unit() const;

   protected:
    NodeType get_node_type() const override;
    virtual const std::string get_value_str() const = 0;
};
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp::messages::xep_iot
//---------------------------------------------------------------------------
