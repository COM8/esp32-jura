#pragma once

#include <string>
#include <functional>

#include "ActuatorNode.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp::messages::xep_iot {
//---------------------------------------------------------------------------
class ButtonNode : public ActuatorNode {
   private:
    const std::string title;
    const std::function<void(const std::string& id)> onButtonPressed;

   public:
    ButtonNode(std::string&& id, std::string&& title, std::function<void(const std::string& id)>&& onButtonPressed);
    ~ButtonNode() override = default;

    const std::string get_value_str() const override;
    bool on_value_changed(const tinyxml2::XMLElement* valNode) override;

   protected:
    Type get_type() const override;
    void to_ui_field(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* xNode) const override;
};
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp::messages::xep_iot
//---------------------------------------------------------------------------
