#pragma once

#include <string>

#include "ActuatorNode.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp::messages::xep_iot {
//---------------------------------------------------------------------------
class SliderNode : public ActuatorNode {
   private:
    const std::string title;
    const double min;
    const double max;
    const double steps;
    double value;

   public:
    SliderNode(std::string&& id, std::string&& title, double min, double max, double value, double steps);
    ~SliderNode() override = default;

    const std::string get_value_str() const override;
    bool on_value_changed(const tinyxml2::XMLElement* valNode) override;
    double get_value() const;

   protected:
    Type get_type() const override;
    void to_ui_field(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* xNode) const override;
};
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp::messages::xep_iot
//---------------------------------------------------------------------------
