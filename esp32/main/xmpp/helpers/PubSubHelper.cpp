#include "PubSubHelper.hpp"

#include <iostream>
#include <vector>

#include "xmpp/XmppUtils.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp::helpers {
//---------------------------------------------------------------------------
tinyxml2::XMLElement* PubSubHelper::gen_publish_item_node(tinyxml2::XMLDocument& doc, const xmpp::Jid& from, const char* nodeName, const char* itemId) {
    tinyxml2::XMLElement* iqNode = doc.NewElement("iq");
    iqNode->SetAttribute("type", "set");
    iqNode->SetAttribute("from", from.get_full().c_str());
    iqNode->SetAttribute("id", generate_uuid_v4().c_str());
    doc.InsertEndChild(iqNode);

    tinyxml2::XMLElement* pubsubNode = doc.NewElement("pubsub");
    pubsubNode->SetAttribute("xmlns", "http://jabber.org/protocol/pubsub");
    iqNode->InsertEndChild(pubsubNode);

    // Publish node:
    tinyxml2::XMLElement* publishNode = doc.NewElement("publish");
    publishNode->SetAttribute("node", nodeName);
    pubsubNode->InsertEndChild(publishNode);

    tinyxml2::XMLElement* itemNode = doc.NewElement("item");
    itemNode->SetAttribute("id", itemId);
    publishNode->InsertEndChild(itemNode);

    // Publish options node:
    // tinyxml2::XMLElement* configureNode = doc.NewElement("configure");

    return itemNode;
}

tinyxml2::XMLElement* PubSubHelper::gen_field_node(tinyxml2::XMLDocument& doc, const char* var, const char* type, const char* value, const char* label) {
    tinyxml2::XMLElement* fieldNode = doc.NewElement("field");
    fieldNode->SetAttribute("var", var);
    if (type) {
        fieldNode->SetAttribute("type", type);
    }
    if (label) {
        fieldNode->SetAttribute("label", label);
    }
    if (value) {
        tinyxml2::XMLElement* valueNode = doc.NewElement("value");
        valueNode->SetText(value);
        fieldNode->InsertEndChild(valueNode);
    }
    return fieldNode;
}
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp::helpers
   //---------------------------------------------------------------------------