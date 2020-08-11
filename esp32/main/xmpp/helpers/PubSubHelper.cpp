#include "PubSubHelper.hpp"

#include <iostream>
#include <vector>

#include "../XmppUtils.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp::helpers {
//---------------------------------------------------------------------------
const std::string PubSubHelper::XMPP_IOT_SENSORS = "xmpp.iot.sensors";
const std::string PubSubHelper::XMPP_IOT_SENSOR_STATUS = "xmpp.iot.sensor.status";
const std::string PubSubHelper::XMPP_IOT_ACTUATORS = "xmpp.iot.actuators";
const std::string PubSubHelper::XMPP_IOT_ACTUATOR_ESPRESSO = "xmpp.iot.actuator.espresso";
const std::string PubSubHelper::XMPP_IOT_ACTUATOR_COFFEE = "xmpp.iot.actuator.coffee";
const std::string PubSubHelper::XMPP_IOT_ACTUATOR_CAPPUCCINO = "xmpp.iot.actuator.cappuccino";
const std::string PubSubHelper::XMPP_IOT_ACTUATOR_MILK_FOAM = "xmpp.iot.actuator.mild_foam";
const std::string PubSubHelper::XMPP_IOT_ACTUATOR_CAFFE_BARISTA = "xmpp.iot.actuator.caffe_barista";
const std::string PubSubHelper::XMPP_IOT_ACTUATOR_LUNGO_BARISTA = "xmpp.iot.actuator.lungo_barista";
const std::string PubSubHelper::XMPP_IOT_ACTUATOR_ESPRESSO_DOPPIO = "xmpp.iot.actuator.espresso_doppio";
const std::string PubSubHelper::XMPP_IOT_ACTUATOR_MACCHIATO = "xmpp.iot.actuator.macchiato";
const std::string PubSubHelper::XMPP_IOT_UI = "xmpp.iot.ui";
const std::string PubSubHelper::XMPP_IOT_NAMESPACE = "urn:xmpp:uwpx:iot";

PubSubHelper::PubSubHelper(std::shared_ptr<xmpp::XmppClient> client) : client(client), state(PUB_SUB_HELPER_NOT_STARTED) {}

void PubSubHelper::setState(PubSubHelperState state) {
    if (this->state != state) {
        this->state = state;
        // TODO trigger some kind of event
    }
}

PubSubHelperState PubSubHelper::getState() { return state; }

void PubSubHelper::start() {
    client->subscribeToMessagesListener(this);
    discoverNodes();
}

void PubSubHelper::discoverNodes() {
    std::string msg = genDiscoverNodesMessage();
    setState(PUB_SUB_HELPER_REQUESTING_NODES);
    client->send(msg);
}

void PubSubHelper::createNodes() {}

std::string PubSubHelper::genDiscoverNodesMessage() {
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* iqNode = doc.NewElement("iq");
    iqNode->SetAttribute("type", "get");
    iqNode->SetAttribute("from", client->account.jid.getFull().c_str());
    iqNode->SetAttribute("id", randFakeUuid().c_str());

    tinyxml2::XMLElement* queryNode = doc.NewElement("query");
    queryNode->SetAttribute("xmlns", "http://jabber.org/protocol/disco#items");
    iqNode->InsertEndChild(queryNode);

    tinyxml2::XMLPrinter printer;
    iqNode->Accept(&printer);
    return printer.CStr();
}

std::string PubSubHelper::genPublishUiNodeMessage() {
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* itemNode = genPublishItemNode(doc, XMPP_IOT_UI.c_str(), "current");

    tinyxml2::XMLElement* xNode = doc.NewElement("x");
    xNode->SetAttribute("xmlns", "jabber:x:data");
    xNode->SetAttribute("xmlns:xdd", "urn:xmpp:xdata:dynamic");
    xNode->SetAttribute("type", "form");
    itemNode->InsertEndChild(xNode);

    tinyxml2::XMLElement* titleNode = doc.NewElement("title");
    titleNode->SetText("JURA E6 Coffee Maker");
    xNode->InsertEndChild(titleNode);

    // Actuators:
    xNode->InsertEndChild(genFieldNode(doc, XMPP_IOT_ACTUATOR_ESPRESSO.c_str(), "boolean", nullptr, "Espresso:"));
    xNode->InsertEndChild(genFieldNode(doc, XMPP_IOT_ACTUATOR_COFFEE.c_str(), "boolean", nullptr, "Coffee:"));
    xNode->InsertEndChild(genFieldNode(doc, XMPP_IOT_ACTUATOR_CAPPUCCINO.c_str(), "boolean", nullptr, "Cappuccino:"));
    xNode->InsertEndChild(genFieldNode(doc, XMPP_IOT_ACTUATOR_MILK_FOAM.c_str(), "boolean", nullptr, "Milk foam:"));
    xNode->InsertEndChild(genFieldNode(doc, XMPP_IOT_ACTUATOR_CAFFE_BARISTA.c_str(), "boolean", nullptr, "Caffe Barista:"));
    xNode->InsertEndChild(genFieldNode(doc, XMPP_IOT_ACTUATOR_LUNGO_BARISTA.c_str(), "boolean", nullptr, "Lungo Barista:"));
    xNode->InsertEndChild(genFieldNode(doc, XMPP_IOT_ACTUATOR_ESPRESSO_DOPPIO.c_str(), "boolean", nullptr, "Espresso doppio:"));
    xNode->InsertEndChild(genFieldNode(doc, XMPP_IOT_ACTUATOR_MACCHIATO.c_str(), "boolean", nullptr, "Macchiato:"));

    // Sensors:
    tinyxml2::XMLElement* node = genFieldNode(doc, XMPP_IOT_SENSOR_STATUS.c_str(), "text-single", nullptr, "Status:");
    node->InsertEndChild(doc.NewElement("xdd:readOnly"));
    xNode->InsertEndChild(node);

    tinyxml2::XMLPrinter printer;
    doc.FirstChild()->Accept(&printer);
    return printer.CStr();
}

tinyxml2::XMLElement* PubSubHelper::genPublishItemNode(tinyxml2::XMLDocument& doc, const char* nodeName, const char* itemId) {
    tinyxml2::XMLElement* iqNode = doc.NewElement("iq");
    iqNode->SetAttribute("type", "set");
    iqNode->SetAttribute("from", client->account.jid.getFull().c_str());
    iqNode->SetAttribute("id", randFakeUuid().c_str());
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

void PubSubHelper::publishCoffeeNode(const std::string& node, bool on) {
    const std::string value = std::to_string(on);
    const std::string unit = "";
    const std::string type = "bool";
    publishActuatorNode(node, value, unit, type);
}

void PubSubHelper::publishStatusNode(const std::string& status) {
    const std::string unit = "";
    const std::string type = "text";
    publishSensorNode(XMPP_IOT_SENSOR_STATUS, status, unit, type);
}

tinyxml2::XMLElement* PubSubHelper::genFieldNode(tinyxml2::XMLDocument& doc, const char* var, const char* type, const char* value, const char* label) {
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

void PubSubHelper::requestNodeConfigMessage(const std::string& nodeName) {
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* iqNode = doc.NewElement("iq");
    iqNode->SetAttribute("type", "get");
    iqNode->SetAttribute("from", client->account.jid.getFull().c_str());
    iqNode->SetAttribute("id", randFakeUuid().c_str());

    tinyxml2::XMLElement* pubSubNode = doc.NewElement("pubsub");
    pubSubNode->SetAttribute("xmlns", "http://jabber.org/protocol/pubsub#owner");
    iqNode->InsertEndChild(pubSubNode);

    tinyxml2::XMLElement* configureNode = doc.NewElement("configure");
    configureNode->SetAttribute("node", nodeName.c_str());
    pubSubNode->InsertEndChild(configureNode);

    tinyxml2::XMLPrinter printer;
    iqNode->Accept(&printer);
    std::string msg = printer.CStr();
    client->send(msg);
}

void PubSubHelper::subscribeToNode(const std::string& nodeName) {
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* iqNode = doc.NewElement("iq");
    iqNode->SetAttribute("type", "set");
    iqNode->SetAttribute("from", client->account.jid.getFull().c_str());
    iqNode->SetAttribute("id", randFakeUuid().c_str());

    tinyxml2::XMLElement* pubSubNode = doc.NewElement("pubsub");
    pubSubNode->SetAttribute("xmlns", "http://jabber.org/protocol/pubsub");
    iqNode->InsertEndChild(pubSubNode);

    tinyxml2::XMLElement* subscribeNode = doc.NewElement("subscribe");
    subscribeNode->SetAttribute("node", nodeName.c_str());
    subscribeNode->SetAttribute("jid", client->account.jid.getFull().c_str());
    pubSubNode->InsertEndChild(subscribeNode);

    tinyxml2::XMLPrinter printer;
    iqNode->Accept(&printer);
    std::string msg = printer.CStr();
    client->send(msg);
}

tinyxml2::XMLElement* PubSubHelper::genNodePublishConfig(tinyxml2::XMLDocument& doc) {
    // https://xmpp.org/extensions/xep-0223.html
    tinyxml2::XMLElement* publishOptionsNode = doc.NewElement("publish-options");
    publishOptionsNode->InsertEndChild(genNodeConfig(doc));
    return publishOptionsNode;
}

tinyxml2::XMLElement* PubSubHelper::genNodeConfig(tinyxml2::XMLDocument& doc) {
    tinyxml2::XMLElement* xNode = doc.NewElement("x");
    xNode->SetAttribute("xmlns", "jabber:x:data");
    xNode->SetAttribute("type", "submit");

    xNode->InsertEndChild(genFieldNode(doc, "FORM_TYPE", "hidden", "http://jabber.org/protocol/pubsub#publish-options", nullptr));
    xNode->InsertEndChild(genFieldNode(doc, "pubsub#persist_items", nullptr, "true", nullptr));
    xNode->InsertEndChild(genFieldNode(doc, "pubsub#access_model", nullptr, "open", nullptr));   // Perhaps replace with "presence"
    xNode->InsertEndChild(genFieldNode(doc, "pubsub#publish_model", nullptr, "open", nullptr));  // Perhaps replace with "subscribers"
    xNode->InsertEndChild(genFieldNode(doc, "pubsub#notification_type", nullptr, "normal", nullptr));

    return xNode;
}

void PubSubHelper::publishSensorNode(const std::string& node, const std::string& value, const std::string& unit, const std::string& type) {
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* itemNode = genPublishItemNode(doc, XMPP_IOT_SENSORS.c_str(), node.c_str());
    tinyxml2::XMLElement* valNode = doc.NewElement("val");
    valNode->SetAttribute("xmlns", XMPP_IOT_NAMESPACE.c_str());
    valNode->SetAttribute("type", type.c_str());
    valNode->SetAttribute("unit", unit.c_str());
    valNode->SetText(value.c_str());
    itemNode->InsertEndChild(valNode);

    tinyxml2::XMLPrinter printer;
    doc.FirstChild()->Accept(&printer);
    std::string msg = printer.CStr();
    client->send(msg);
}

void PubSubHelper::createNode(const std::string& nodeName) {
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* iqNode = doc.NewElement("iq");
    iqNode->SetAttribute("type", "set");
    iqNode->SetAttribute("from", client->account.jid.getFull().c_str());
    iqNode->SetAttribute("id", randFakeUuid().c_str());

    tinyxml2::XMLElement* pubSubNode = doc.NewElement("pubsub");
    pubSubNode->SetAttribute("xmlns", "http://jabber.org/protocol/pubsub");
    iqNode->InsertEndChild(pubSubNode);

    tinyxml2::XMLElement* createNode = doc.NewElement("create");
    createNode->SetAttribute("node", nodeName.c_str());
    pubSubNode->InsertEndChild(createNode);

    tinyxml2::XMLElement* configureNode = doc.NewElement("configure");
    configureNode->InsertEndChild(genNodeConfig(doc));
    pubSubNode->InsertEndChild(configureNode);

    tinyxml2::XMLPrinter printer;
    iqNode->Accept(&printer);
    std::string msg = printer.CStr();
    client->send(msg);
}

void PubSubHelper::deleteNode(const std::string& nodeName) {
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* iqNode = doc.NewElement("iq");
    iqNode->SetAttribute("type", "set");
    iqNode->SetAttribute("from", client->account.jid.getFull().c_str());
    iqNode->SetAttribute("id", randFakeUuid().c_str());

    tinyxml2::XMLElement* pubSubNode = doc.NewElement("pubsub");
    pubSubNode->SetAttribute("xmlns", "http://jabber.org/protocol/pubsub#owner");
    iqNode->InsertEndChild(pubSubNode);

    tinyxml2::XMLElement* createNode = doc.NewElement("delete");
    createNode->SetAttribute("node", nodeName.c_str());
    pubSubNode->InsertEndChild(createNode);

    tinyxml2::XMLPrinter printer;
    iqNode->Accept(&printer);
    std::string msg = printer.CStr();
    client->send(msg);
}

void PubSubHelper::publishActuatorNode(const std::string& node, const std::string& value, const std::string& unit, const std::string& type) {
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* itemNode = genPublishItemNode(doc, XMPP_IOT_ACTUATORS.c_str(), node.c_str());
    tinyxml2::XMLElement* valNode = doc.NewElement("val");
    valNode->SetAttribute("xmlns", XMPP_IOT_NAMESPACE.c_str());
    valNode->SetAttribute("type", type.c_str());
    valNode->SetAttribute("unit", unit.c_str());
    valNode->SetText(value.c_str());
    itemNode->InsertEndChild(valNode);

    tinyxml2::XMLPrinter printer;
    doc.FirstChild()->Accept(&printer);
    std::string msg = printer.CStr();
    client->send(msg);
}

void PubSubHelper::onDiscoverNodesReply(messages::Message& event) {
    const tinyxml2::XMLDocument& doc = event.toXmlDoc();
    const tinyxml2::XMLElement* elem = doc.FirstChildElement("iq");
    if (!elem) {
        return;
    }
    elem = elem->FirstChildElement("query");
    if (!elem) {
        return;
    }
    std::vector<std::string> nodes;
    for (const tinyxml2::XMLElement* e = elem->FirstChildElement("item"); e != nullptr; e = e->NextSiblingElement("item")) {
        const char* val = e->Attribute("node");
        if (val) {
            nodes.push_back(std::string(val));
        }
    }
    // Publish nodes anyway.
    // Don't care if they exist:

    // Prepare nodes:
    deleteNode(XMPP_IOT_UI);
    createNode(XMPP_IOT_UI);
    subscribeToNode(XMPP_IOT_UI);
    deleteNode(XMPP_IOT_SENSORS);
    createNode(XMPP_IOT_SENSORS);
    subscribeToNode(XMPP_IOT_SENSORS);
    deleteNode(XMPP_IOT_ACTUATORS);
    createNode(XMPP_IOT_ACTUATORS);
    subscribeToNode(XMPP_IOT_ACTUATORS);

    // UI:
    std::string msg = genPublishUiNodeMessage();
    client->send(msg);

    // Sensors:
    publishStatusNode("Ready for some coffee.");

    // Actuators:
    publishCoffeeNode(XMPP_IOT_ACTUATOR_ESPRESSO, false);
    publishCoffeeNode(XMPP_IOT_ACTUATOR_COFFEE, false);
    publishCoffeeNode(XMPP_IOT_ACTUATOR_CAPPUCCINO, false);
    publishCoffeeNode(XMPP_IOT_ACTUATOR_MILK_FOAM, false);
    publishCoffeeNode(XMPP_IOT_ACTUATOR_CAFFE_BARISTA, false);
    publishCoffeeNode(XMPP_IOT_ACTUATOR_LUNGO_BARISTA, false);
    publishCoffeeNode(XMPP_IOT_ACTUATOR_ESPRESSO_DOPPIO, false);
    publishCoffeeNode(XMPP_IOT_ACTUATOR_MACCHIATO, false);

    /*if (std::find(nodes.begin(), nodes.end(), XMPP_IOT_UI) == nodes.end()) {
        std::string msg = genPublishUiNodeMessage();
        client->send(msg);
    }

    if (std::find(nodes.begin(), nodes.end(), XMPP_IOT_SENSORS) == nodes.end()) {
        std::string msg = genPublishSensorsNodeMessage(22.34, 42);
        client->send(msg);
    }

    if (std::find(nodes.begin(), nodes.end(), XMPP_IOT_ACTUATORS) == nodes.end()) {
        std::string msg = genPublishActuatorsNodeMessage();
        client->send(msg);
    }*/
}

void PubSubHelper::onCreateNodeReply(messages::Message& event) {}

void PubSubHelper::event(messages::Message& event) {
    switch (state) {
        case PUB_SUB_HELPER_REQUESTING_NODES:
            onDiscoverNodesReply(event);
            break;

        case PUB_SUB_HELPER_CREATING_NODES:
            onCreateNodeReply(event);
            break;

        default:
            break;
    }
}
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp::helpers
   //---------------------------------------------------------------------------