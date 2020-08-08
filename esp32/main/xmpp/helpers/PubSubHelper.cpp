#include "PubSubHelper.hpp"

#include <iostream>
#include <vector>

#include "../XmppUtils.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp::helpers {
//---------------------------------------------------------------------------
const std::string PubSubHelper::XMPP_IOT_SENSOR_TEMP = "xmpp.iot.sensor.temp";
const std::string PubSubHelper::XMPP_IOT_SENSOR_BAR = "xmpp.iot.sensor.bar";
const std::string PubSubHelper::XMPP_IOT_SENSOR_MQ2 = "xmpp.iot.sensor.mq2";
const std::string PubSubHelper::XMPP_IOT_SENSOR_PHOTORESISTOR = "xmpp.iot.sensor.photoresistor";
const std::string PubSubHelper::XMPP_IOT_SENSORS = "xmpp.iot.sensors";
const std::string PubSubHelper::XMPP_IOT_ACTUATORS = "xmpp.iot.actuator";
const std::string PubSubHelper::XMPP_IOT_ACTUATOR_LED = "xmpp.iot.actuator.led";
const std::string PubSubHelper::XMPP_IOT_ACTUATOR_SPEAKER = "xmpp.iot.actuator.speaker";
const std::string PubSubHelper::XMPP_IOT_ACTUATOR_RELAY = "xmpp.iot.actuator.relay";
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
    titleNode->SetText("ESP32 XMPP");
    xNode->InsertEndChild(titleNode);

#if defined(BMP180) || defined(MQ2) || defined(PHOTORESISTOR)
    tinyxml2::XMLElement* node;
#endif

    // Sensors:
#ifdef BMP180
    node = genFieldNode(doc, XMPP_IOT_SENSOR_TEMP.c_str(), "text-single", nullptr, "Temperature:");
    node->InsertEndChild(doc.NewElement("xdd:readOnly"));
    xNode->InsertEndChild(node);
    node = genFieldNode(doc, XMPP_IOT_SENSOR_BAR.c_str(), "text-single", nullptr, "Pressure:");
    node->InsertEndChild(doc.NewElement("xdd:readOnly"));
    xNode->InsertEndChild(node);
#endif  // BMP180
#ifdef MQ2
    node = genFieldNode(doc, XMPP_IOT_SENSOR_MQ2.c_str(), "text-single", nullptr, "MQ2:");
    node->InsertEndChild(doc.NewElement("xdd:readOnly"));
    xNode->InsertEndChild(node);
#endif  // MQ2
#ifdef PHOTORESISTOR
    node = genFieldNode(doc, XMPP_IOT_SENSOR_PHOTORESISTOR.c_str(), "text-single", nullptr, "Photoresistor:");
    node->InsertEndChild(doc.NewElement("xdd:readOnly"));
    xNode->InsertEndChild(node);
#endif  // PHOTORESISTOR

    // Actuators:
#ifdef SPEAKER
    xNode->InsertEndChild(genFieldNode(doc, XMPP_IOT_ACTUATOR_SPEAKER.c_str(), "boolean", nullptr, "Speaker on:"));
#endif  // SPEAKER
#ifdef RELAY
    xNode->InsertEndChild(genFieldNode(doc, XMPP_IOT_ACTUATOR_RELAY.c_str(), "boolean", nullptr, "Relay on:"));
#endif  // RELAY

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

void PubSubHelper::publishTempNode(double temp) {
    const std::string value = std::to_string(temp);
    const std::string unit = "celsius";
    const std::string type = "double";
    publishSensorNode(XMPP_IOT_SENSOR_TEMP, value, unit, type);
}

void PubSubHelper::publishPressureNode(int32_t pressure) {
    const std::string value = std::to_string(pressure);
    const std::string unit = "bar";
    const std::string type = "uint";
    publishSensorNode(XMPP_IOT_SENSOR_BAR, value, unit, type);
}

void PubSubHelper::publishLedNode(bool on) {
    const std::string value = std::to_string(on);
    const std::string unit = "";
    const std::string type = "bool";
    publishActuatorNode(XMPP_IOT_ACTUATOR_LED, value, unit, type);
}

void PubSubHelper::publishSpeakerNode(bool on) {
    const std::string value = std::to_string(on);
    const std::string unit = "";
    const std::string type = "bool";
    publishActuatorNode(XMPP_IOT_ACTUATOR_SPEAKER, value, unit, type);
}

void PubSubHelper::publishMq2Node(int32_t val) {
    const std::string value = std::to_string(val);
    const std::string unit = "";
    const std::string type = "uint";
    publishSensorNode(XMPP_IOT_SENSOR_MQ2, value, unit, type);
}

void PubSubHelper::publishPhotoresistorNode(int32_t val) {
    const std::string value = std::to_string(val);
    const std::string unit = "";
    const std::string type = "uint";
    publishSensorNode(XMPP_IOT_SENSOR_PHOTORESISTOR, value, unit, type);
}

void PubSubHelper::publishRelayNode(bool on) {
    const std::string value = std::to_string(on);
    const std::string unit = "";
    const std::string type = "bool";
    publishActuatorNode(XMPP_IOT_ACTUATOR_RELAY, value, unit, type);
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
#ifdef BMP180
    publishTempNode(0);
    publishPressureNode(0);
#endif  // BMP180
#ifdef MQ2
    publishMq2Node(0);
#endif  // MQ2
#ifdef PHOTORESISTOR
    publishPhotoresistorNode(0);
#endif  // PHOTORESISTOR

    // Actuators:
#ifdef SPEAKER
    publishSpeakerNode(0);
#endif  // SPEAKER
#ifdef RELAY
    publishRelayNode(0);
#endif  // RELAY

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