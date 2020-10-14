#pragma once

#include <tinyxml2.h>

#include <memory>
#include <vector>

#include "AbstractNode.hpp"
#include "ActuatorNode.hpp"
#include "SensorNode.hpp"
#include "xmpp/XmppClient.hpp"
#include "xmpp/messages/Message.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp::messages::xep_iot {
//---------------------------------------------------------------------------
class Device : public INonConstEventListener<messages::Message> {
   private:
    static const std::string XMPP_IOT_UI;
    static const std::string XMPP_IOT_NAMESPACE;

    std::vector<std::unique_ptr<AbstractNode>> nodes{};
    std::shared_ptr<xmpp::XmppClient> client;

   public:
    Device(std::shared_ptr<xmpp::XmppClient> client);

    void add_node(std::unique_ptr<AbstractNode>&& node);
    void init();

    void publish_nodes();
    void publish_ui_node();
    void publish_sensor_nodes();
    void publish_actuator_nodes();
    void publish_node(const std::unique_ptr<AbstractNode>& node);
    void publish_node(const ActuatorNode* node);
    void publish_node(const SensorNode* node);

    void discover_nodes();
    void request_node_configuration(const std::string& nodeName);

    void create_node(const std::string& nodeName);
    void delete_node(const std::string& nodeName);
    void subscribe_to_node(const std::string& nodeName);

    void event(messages::Message& event) override;
    void on_pub_sub_event_message(const tinyxml2::XMLElement* elem);

   private:
    void gen_ui_node(tinyxml2::XMLDocument& doc);
    static tinyxml2::XMLElement* gen_node_config(tinyxml2::XMLDocument& doc);
    const std::string gen_discover_nodes_message() const;
};
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp::messages::xep_iot
   //---------------------------------------------------------------------------