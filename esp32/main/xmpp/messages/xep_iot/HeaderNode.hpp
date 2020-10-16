#pragma once

#include "SensorNode.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp::messages::xep_iot {
//---------------------------------------------------------------------------
class HeaderNode : public SensorNode {
   private:
    std::string text;

   public:
    HeaderNode(const std::string&& id, std::string&& text);
    ~HeaderNode() override = default;

    bool on_value_changed(const tinyxml2::XMLElement* valNode) override;

   protected:
    virtual Type get_type() const override;
    void to_ui_field(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* xNode) const override;
};
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp::messages::xep_iot
//---------------------------------------------------------------------------
