#pragma once

#include <tinyxml2.h>

#include "xmpp/Jid.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp::helpers {
//---------------------------------------------------------------------------
class PubSubHelper {
   public:
    /**
     * Generates a new PEP node.
     **/
    static tinyxml2::XMLElement* gen_publish_item_node(tinyxml2::XMLDocument& doc, const xmpp::Jid& from, const char* nodeName, const char* itemId);
    static tinyxml2::XMLElement* gen_field_node(tinyxml2::XMLDocument& doc, const char* var, const char* type, const char* value, const char* label);
};
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp::helpers
   //---------------------------------------------------------------------------