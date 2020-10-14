#pragma once

#include "SensorNode.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp::messages::xep_iot {
//---------------------------------------------------------------------------
class TextSingleNode : public SensorNode {
   private:
    std::string text;

   public:
    TextSingleNode(const std::string&& id, std::string&& text);
    ~TextSingleNode() override = default;

    bool on_value_changed(const tinyxml2::XMLElement* valNode) override;

   protected:
    virtual Type get_type() const override;
    void to_ui_field(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* xNode) const override;
    const std::string get_value_str() const override;
};
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp::messages::xep_iot
//---------------------------------------------------------------------------
