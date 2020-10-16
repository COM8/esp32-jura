#pragma once

#include <tinyxml2.h>

#include <string>

#include "xmpp/INonConstEventListener.hpp"
#include "xmpp/Jid.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp::messages::xep_iot {
//---------------------------------------------------------------------------
enum NodeType { SENSOR = 0, ACTUATOR = 1 };

enum Type { ButtonNodeType = 0, HeaderNodeType = 1, TextSingleNodeType = 2, SliderNodeType = 3 };

class AbstractNode {
   protected:
    const std::string id;
    const std::string fieldType;

   public:
    AbstractNode(std::string&& id, std::string&& fieldType);
    virtual ~AbstractNode() = default;

    virtual NodeType get_node_type() const = 0;
    virtual Type get_type() const = 0;
    virtual void to_ui_field(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* xNode) const = 0;
    /**
     * Called once the value of the node changed.
     * Returns true in case a new value should be published afterwards, false otherwise.
     **/
    virtual bool on_value_changed(const tinyxml2::XMLElement* valNode) = 0;

    const std::string& get_id() const;

   protected:
    tinyxml2::XMLElement* gen_field_node(tinyxml2::XMLDocument& doc, const char* value, const char* label) const;
};
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp::messages::xep_iot
//---------------------------------------------------------------------------
