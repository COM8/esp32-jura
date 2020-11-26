#include "Device.hpp"

#include <algorithm>
#include <iostream>

#include "xmpp/XmppUtils.hpp"
#include "xmpp/helpers/PubSubHelper.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp::messages::xep_iot {
//---------------------------------------------------------------------------
const std::string Device::XMPP_IOT_UI = "xmpp.iot.ui";
const std::string Device::XMPP_IOT_NAMESPACE = "urn:xmpp:uwpx:iot";

Device::Device(std::shared_ptr<xmpp::XmppClient> client) : client(std::move(client)) {}

void Device::add_node(std::unique_ptr<AbstractNode>&& node) { nodes.emplace_back(std::move(node)); }

void Device::init() { discover_nodes(); }

void Device::publish_nodes() {
    publish_ui_node();
    publish_sensor_nodes();
    publish_actuator_nodes();
}

void Device::publish_ui_node() {
    std::cout << "Publishing UI node...\n";

    tinyxml2::XMLDocument doc;
    gen_ui_node(doc);

    tinyxml2::XMLPrinter printer;
    doc.FirstChild()->Accept(&printer);
    std::string msg{printer.CStr()};
    client->send(msg);
    std::cout << "UI node published.\n";
}

void Device::publish_sensor_nodes() {
    std::cout << "Publishing sensor nodes...\n";
    for (const std::unique_ptr<AbstractNode>& node : nodes) {
        if (node->get_node_type() == NodeType::SENSOR) {
            SensorNode* senNode = static_cast<SensorNode*>(node.get());
            publish_node(senNode);
        }
    }
    std::cout << "Sensor nodes published.\n";
}

void Device::publish_actuator_nodes() {
    std::cout << "Publishing actuator nodes...\n";
    for (const std::unique_ptr<AbstractNode>& node : nodes) {
        if (node->get_node_type() == NodeType::ACTUATOR) {
            ActuatorNode* acNode = static_cast<ActuatorNode*>(node.get());
            publish_node(acNode);
        }
    }
    std::cout << "Actuator nodes published.\n";
}

void Device::publish_node(const std::unique_ptr<AbstractNode>& node) {
    if (node->get_node_type() == NodeType::ACTUATOR) {
        ActuatorNode* acNode = static_cast<ActuatorNode*>(node.get());
        publish_node(acNode);
    } else if (node->get_node_type() == NodeType::SENSOR) {
        SensorNode* senNode = static_cast<SensorNode*>(node.get());
        publish_node(senNode);
    } else {
        assert(false);  // Should not happen
    }
}

void Device::publish_node(const ActuatorNode* node) {
    tinyxml2::XMLDocument doc;
    node->gen_actuator_node(doc, client->account.jid, XMPP_IOT_NAMESPACE);

    tinyxml2::XMLPrinter printer;
    doc.FirstChild()->Accept(&printer);
    std::string msg{printer.CStr()};
    client->send(msg);
    std::cout << "Actuator node '" << node->get_id() << "' updated.\n";
}

void Device::publish_node(const SensorNode* node) {
    if (node->hasValue) {
        tinyxml2::XMLDocument doc;
        node->gen_sensor_node(doc, client->account.jid, XMPP_IOT_NAMESPACE);

        tinyxml2::XMLPrinter printer;
        doc.FirstChild()->Accept(&printer);
        std::string msg{printer.CStr()};
        client->send(msg);
        std::cout << "Sensor node '" << node->get_id() << "' updated.\n";
    }
}

void Device::gen_ui_node(tinyxml2::XMLDocument& doc) {
    tinyxml2::XMLElement* itemNode = helpers::PubSubHelper::gen_publish_item_node(doc, client->account.jid, XMPP_IOT_UI.c_str(), "current");

    tinyxml2::XMLElement* xNode = doc.NewElement("x");
    xNode->SetAttribute("xmlns", "jabber:x:data");
    xNode->SetAttribute("xmlns:xdd", "urn:xmpp:xdata:dynamic");
    xNode->SetAttribute("type", "form");
    itemNode->InsertEndChild(xNode);

    tinyxml2::XMLElement* titleNode = doc.NewElement("title");
    titleNode->SetText("Jutta E6 Coffee Maker");
    xNode->InsertEndChild(titleNode);

    // Add nodes:
    for (const std::unique_ptr<AbstractNode>& node : nodes) {
        node->to_ui_field(doc, xNode);
    }
}

void Device::event(messages::Message& event) {
    const tinyxml2::XMLDocument& doc = event.toXmlDoc();
    const tinyxml2::XMLElement* elem = doc.FirstChildElement("iq");
    if (!elem) {
        return;
    }
    elem = elem->FirstChildElement("query");
    if (!elem) {
        return;
    }
    std::unordered_set<std::string> nodes;
    for (const tinyxml2::XMLElement* e = elem->FirstChildElement("item"); e != nullptr; e = e->NextSiblingElement("item")) {
        const char* val = e->Attribute("node");
        if (val) {
            nodes.insert(std::string(val));
        }
    }

    // Prepare nodes:
    create_subscribe_iot_node(XMPP_IOT_UI, nodes);
    create_subscribe_iot_node(ActuatorNode::XMPP_IOT_ACTUATORS, nodes);
    create_subscribe_iot_node(SensorNode::XMPP_IOT_SENSORS, nodes);

    // Publish node values:
    publish_nodes();
}

void Device::create_subscribe_iot_node(const std::string& nodeName, const std::unordered_set<std::string>& discoNodeSet) {
    if (discoNodeSet.find(nodeName) == discoNodeSet.end()) {
        // delete_node(nodeName);
        create_node(nodeName);
    } else {
        std::cout << "Node: " << nodeName << " already published. Just subscribing.\n";
    }
    subscribe_to_node(nodeName);
}

void Device::delete_node(const std::string& nodeName) {
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* iqNode = doc.NewElement("iq");
    iqNode->SetAttribute("type", "set");
    iqNode->SetAttribute("from", client->account.jid.get_full().c_str());
    iqNode->SetAttribute("id", generate_uuid_v4().c_str());

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

void Device::create_node(const std::string& nodeName) {
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* iqNode = doc.NewElement("iq");
    iqNode->SetAttribute("type", "set");
    iqNode->SetAttribute("from", client->account.jid.get_full().c_str());
    iqNode->SetAttribute("id", generate_uuid_v4().c_str());

    tinyxml2::XMLElement* pubSubNode = doc.NewElement("pubsub");
    pubSubNode->SetAttribute("xmlns", "http://jabber.org/protocol/pubsub");
    iqNode->InsertEndChild(pubSubNode);

    tinyxml2::XMLElement* createNode = doc.NewElement("create");
    createNode->SetAttribute("node", nodeName.c_str());
    pubSubNode->InsertEndChild(createNode);

    tinyxml2::XMLElement* configureNode = doc.NewElement("configure");
    configureNode->InsertEndChild(gen_node_config(doc));
    pubSubNode->InsertEndChild(configureNode);

    tinyxml2::XMLPrinter printer;
    iqNode->Accept(&printer);
    std::string msg = printer.CStr();
    client->send(msg);
}

void Device::subscribe_to_node(const std::string& nodeName) {
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* iqNode = doc.NewElement("iq");
    iqNode->SetAttribute("type", "set");
    iqNode->SetAttribute("from", client->account.jid.get_full().c_str());
    iqNode->SetAttribute("id", generate_uuid_v4().c_str());

    tinyxml2::XMLElement* pubSubNode = doc.NewElement("pubsub");
    pubSubNode->SetAttribute("xmlns", "http://jabber.org/protocol/pubsub");
    iqNode->InsertEndChild(pubSubNode);

    tinyxml2::XMLElement* subscribeNode = doc.NewElement("subscribe");
    subscribeNode->SetAttribute("node", nodeName.c_str());
    subscribeNode->SetAttribute("jid", client->account.jid.get_full().c_str());
    pubSubNode->InsertEndChild(subscribeNode);

    tinyxml2::XMLPrinter printer;
    iqNode->Accept(&printer);
    std::string msg = printer.CStr();
    client->send(msg);
}

const std::string Device::gen_discover_nodes_message() const {
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* iqNode = doc.NewElement("iq");
    iqNode->SetAttribute("type", "get");
    iqNode->SetAttribute("from", client->account.jid.get_full().c_str());
    iqNode->SetAttribute("id", generate_uuid_v4().c_str());

    tinyxml2::XMLElement* queryNode = doc.NewElement("query");
    queryNode->SetAttribute("xmlns", "http://jabber.org/protocol/disco#items");
    iqNode->InsertEndChild(queryNode);

    tinyxml2::XMLPrinter printer;
    iqNode->Accept(&printer);
    return printer.CStr();
}

void Device::discover_nodes() {
    const std::string msg = gen_discover_nodes_message();
    client->send(msg);
}

void Device::on_pub_sub_event_message(const tinyxml2::XMLElement* elem) {
    const tinyxml2::XMLAttribute* nodeAttrib = elem->FindAttribute("node");
    if (nodeAttrib && (elem = elem->FirstChildElement("item"))) {
        const tinyxml2::XMLAttribute* idAttrib = elem->FindAttribute("id");
        if (idAttrib && (elem = elem->FirstChildElement("val"))) {
            if (!strcmp(nodeAttrib->Value(), ActuatorNode::XMPP_IOT_ACTUATORS.c_str())) {
                std::string nodeId = idAttrib->Value();
                for (const std::unique_ptr<AbstractNode>& node : nodes) {
                    if (node->get_id() == nodeId) {
                        if (node->on_value_changed(elem)) {
                            publish_node(node);
                        }
                        std::cout << "Value for node '" << nodeId << "' updated to: '" << elem->ToText() << "'\n";
                        return;
                    }
                }
            }
        }
    }
}

tinyxml2::XMLElement* Device::gen_node_config(tinyxml2::XMLDocument& doc) {
    tinyxml2::XMLElement* xNode = doc.NewElement("x");
    xNode->SetAttribute("xmlns", "jabber:x:data");
    xNode->SetAttribute("type", "submit");

    xNode->InsertEndChild(helpers::PubSubHelper::gen_field_node(doc, "FORM_TYPE", "hidden", "http://jabber.org/protocol/pubsub#node_config", nullptr));
    xNode->InsertEndChild(helpers::PubSubHelper::gen_field_node(doc, "pubsub#persist_items", nullptr, "true", nullptr));
    xNode->InsertEndChild(helpers::PubSubHelper::gen_field_node(doc, "pubsub#max_items", nullptr, "30", nullptr));
    xNode->InsertEndChild(helpers::PubSubHelper::gen_field_node(doc, "pubsub#access_model", nullptr, "open", nullptr));   // Perhaps replace with "presence"
    xNode->InsertEndChild(helpers::PubSubHelper::gen_field_node(doc, "pubsub#publish_model", nullptr, "open", nullptr));  // Perhaps replace with "subscribers"
    xNode->InsertEndChild(helpers::PubSubHelper::gen_field_node(doc, "pubsub#notification_type", nullptr, "normal", nullptr));
    xNode->InsertEndChild(helpers::PubSubHelper::gen_field_node(doc, "pubsub#deliver_payloads", nullptr, "true", nullptr));

    return xNode;
}
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp::messages::xep_iot
//---------------------------------------------------------------------------
