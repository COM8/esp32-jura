#include "XmppTask.hpp"

#include <smooth/core/network/event/ConnectionStatusEvent.h>
#include <smooth/core/task_priorities.h>
#include <tinyxml2.h>

#include <cassert>
#include <iostream>
#include <string>
#include <unordered_map>
#include <chrono>
#include <cmath>

#include "xmpp/Jid.hpp"
#include "xmpp/XmppAccount.hpp"
#include "xmpp/messages/xep_iot/AbstractNode.hpp"
#include "xmpp/messages/xep_iot/ButtonNode.hpp"
#include "xmpp/messages/xep_iot/HeaderNode.hpp"
#include "xmpp/messages/xep_iot/SliderNode.hpp"
#include "xmpp/messages/xep_iot/TextSingleNode.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp {
//---------------------------------------------------------------------------
using namespace smooth::core;
//---------------------------------------------------------------------------
const std::string INITIAL_HELLO_MESSAGE = "Hi from the ESP32. Please mirror this message!";

const std::string ID_SENSOR_TEXT_GENERAL = "xmpp.iot.sensor.text.general";
const std::string ID_SENSOR_TEXT_CUSTOM = "xmpp.iot.sensor.text.custom";
const std::string ID_ACTUATOR_SLIDER_WATER = "xmpp.iot.actuator.slider.water";
const std::string ID_ACTUATOR_SLIDER_BEANS = "xmpp.iot.actuator.slider.beans";
const std::string ID_SENSOR_TEXT_INFO = "xmpp.iot.sensor.text.info";
const std::string ID_SENSOR_TEXT_MACHINE = "xmpp.iot.sensor.text.machine";
const std::string ID_SENSOR_TEXT_STATUS = "xmpp.iot.sensor.text.status";

const std::string ID_ACTUATOR_BUTTON_COFFEE = "xmpp.iot.actuator.button.coffee";
const std::string ID_ACTUATOR_BUTTON_ESPRESSO = "xmpp.iot.actuator.button.espresso";
const std::string ID_ACTUATOR_BUTTON_CAPPUCCINO = "xmpp.iot.actuator.button.cappuccino";
const std::string ID_ACTUATOR_BUTTON_MILK_FOAM = "xmpp.iot.actuator.button.milk_foam";
const std::string ID_ACTUATOR_BUTTON_CAFFE_BARISTA = "xmpp.iot.actuator.button.caffe_barista";
const std::string ID_ACTUATOR_BUTTON_LUNGO_BARISTA = "xmpp.iot.actuator.button.lungo_barista";
const std::string ID_ACTUATOR_BUTTON_ESPRESSO_DOPPIO = "xmpp.iot.actuator.button.espresso_doppio";
const std::string ID_ACTUATOR_BUTTON_MACCIATO = "xmpp.iot.actuator.button.macciato";
const std::string ID_ACTUATOR_BUTTON_BREW = "xmpp.iot.actuator.button.brew";

const std::unordered_map<std::string, jura::CoffeeMaker::coffee_t> ID_TYPE_MAP{{ID_ACTUATOR_BUTTON_COFFEE, jura::CoffeeMaker::coffee_t::COFFEE},
                                                                               {ID_ACTUATOR_BUTTON_ESPRESSO, jura::CoffeeMaker::coffee_t::ESPRESSO},
                                                                               {ID_ACTUATOR_BUTTON_CAPPUCCINO, jura::CoffeeMaker::coffee_t::CAPPUCCINO},
                                                                               {ID_ACTUATOR_BUTTON_MILK_FOAM, jura::CoffeeMaker::coffee_t::MILK_FOAM},
                                                                               {ID_ACTUATOR_BUTTON_CAFFE_BARISTA, jura::CoffeeMaker::coffee_t::CAFFE_BARISTA},
                                                                               {ID_ACTUATOR_BUTTON_LUNGO_BARISTA, jura::CoffeeMaker::coffee_t::LUNGO_BARISTA},
                                                                               {ID_ACTUATOR_BUTTON_ESPRESSO_DOPPIO, jura::CoffeeMaker::coffee_t::ESPRESSO_DOPPIO},
                                                                               {ID_ACTUATOR_BUTTON_MACCIATO, jura::CoffeeMaker::coffee_t::MACCHIATO}};

XmppTask::XmppTask(std::shared_ptr<esp::Storage> storage)
    : Task("XMPP Task", 4096, smooth::core::APPLICATION_BASE_PRIO, std::chrono::seconds(3), 1),
      net_status(NetworkStatusQueue::create(2, *this, *this)),
      storage(std::move(storage)),
      client(nullptr),
      iotDevice(nullptr) {}

XmppTask::~XmppTask() {
    if (client) {
        client->unsubscribeFromMessagesListener(this);
    }
}

void XmppTask::init() {
    // Coffee Maker:
    coffeeMaker.init();

    // XMPP:
    std::string jidString = storage->readString(esp::Storage::JID);
    xmpp::Jid jid = xmpp::Jid::from_full_jid(jidString);
    assert(jid.is_full());

    std::string password = storage->readString(esp::Storage::JID_PASSWORD);
    xmpp::XmppAccount account(std::move(jid), std::move(password), std::make_shared<smooth::core::network::IPv4>(jid.domainPart, 5222));
    client = std::make_unique<xmpp::XmppClient>(std::move(account), *this, *this);
    client->subscribeToMessagesListener(this);

    // IoT-Device:
    iotDevice = std::make_unique<messages::xep_iot::Device>(client);
    add_header(ID_SENSOR_TEXT_GENERAL, "General:");
    add_button(ID_ACTUATOR_BUTTON_COFFEE, "Coffee");
    add_button(ID_ACTUATOR_BUTTON_ESPRESSO, "Espresso");
    add_button(ID_ACTUATOR_BUTTON_CAPPUCCINO, "Cappuccino");
    add_button(ID_ACTUATOR_BUTTON_MILK_FOAM, "Milk foam");
    add_button(ID_ACTUATOR_BUTTON_CAFFE_BARISTA, "Caffe Barista");
    add_button(ID_ACTUATOR_BUTTON_LUNGO_BARISTA, "Lungo Barista");
    add_button(ID_ACTUATOR_BUTTON_ESPRESSO_DOPPIO, "Espresso doppio");
    add_button(ID_ACTUATOR_BUTTON_MACCIATO, "Macciato");
    add_header(ID_SENSOR_TEXT_CUSTOM, "Custom:");
    add_slider(ID_ACTUATOR_SLIDER_WATER, "Water:", 0, 200, 100, 1);
    add_slider(ID_ACTUATOR_SLIDER_BEANS, "Beans:", 0, 200, 100, 1);
    add_button(ID_ACTUATOR_BUTTON_BREW, "Brew");
    add_header(ID_SENSOR_TEXT_INFO, "Info:");
    add_text_single(ID_SENSOR_TEXT_MACHINE, "Machine:", "Jura E6", true);
    add_text_single(ID_SENSOR_TEXT_STATUS, "Status:", "ready", true);
}

void XmppTask::add_button(const std::string& id, std::string&& label) {
    std::function<void(const std::string&)> fp = std::bind(&XmppTask::on_button_pressed, this, std::placeholders::_1);
    std::unique_ptr<messages::xep_iot::ButtonNode> button = std::make_unique<messages::xep_iot::ButtonNode>(std::string(id), std::move(label), std::move(fp));
    iotDevice->add_node(std::move(button));
}

void XmppTask::add_text_single(const std::string& id, std::string&& label, std::string&& text, bool readonly) {
    std::unique_ptr<messages::xep_iot::TextSingleNode> textSingle = std::make_unique<messages::xep_iot::TextSingleNode>(std::string(id), std::move(label), std::move(text), readonly);
    iotDevice->add_node(std::move(textSingle));
}

void XmppTask::add_header(const std::string& id, std::string&& text) {
    std::unique_ptr<messages::xep_iot::HeaderNode> header = std::make_unique<messages::xep_iot::HeaderNode>(std::string(id), std::move(text));
    iotDevice->add_node(std::move(header));
}

void XmppTask::add_slider(const std::string& id, std::string&& label, double min, double max, double value, double steps) {
    std::unique_ptr<messages::xep_iot::SliderNode> slider = std::make_unique<messages::xep_iot::SliderNode>(std::string(id), std::move(label), min, max, value, steps);
    iotDevice->add_node(std::move(slider));
}

void XmppTask::tick() {
    if (client->isConnected()) {
        // pubSubHelper->requestNodeConfigMessage(pubSubHelper->XMPP_IOT_ACTUATORS);
    }
}

void XmppTask::event(const network::NetworkStatus& event) {
    switch (event.get_event()) {
        case network::NetworkEvent::GOT_IP:
            if (client) {
                client->connect();
            }
            break;

        case network::NetworkEvent::DISCONNECTED:
            if (client) {
                client->disconnect();
            }
            break;

        default:
            break;
    }
}

void XmppTask::onReady() { iotDevice->init(); }

void XmppTask::handlePresenceMessages(const tinyxml2::XMLElement* elem) {
    const tinyxml2::XMLAttribute* attrib = elem->FindAttribute("type");
    // For now approve all presence subscription requests:
    if (attrib && !strcmp(attrib->Value(), "subscribe")) {
        attrib = elem->FindAttribute("from");
        std::string bareJid = attrib->Value();
        client->approvePresenceSubscription(bareJid);
    }
}

void XmppTask::handleMessageMessages(const tinyxml2::XMLElement* elem) {
    const tinyxml2::XMLElement* tmp = elem->FirstChildElement("body");
    if (tmp) {
        if (!storage->readBool(esp::Storage::SETUP_DONE)) {
            if (!strcmp(INITIAL_HELLO_MESSAGE.c_str(), tmp->GetText())) {
                std::string to = storage->readString(esp::Storage::JID_SENDER);

                // Add to roster:
                client->addToRoster(to);

                // Send setup done message:
                std::string body = "Setup done!";
                client->sendMessage(to, body);
                storage->writeBool(esp::Storage::SETUP_DONE, true);
                std::cout << "Setup done!\n";
                onReady();
            }
        } else {
            handleIoTMessageMessage(tmp->GetText());
        }
    } else if ((tmp = elem->FirstChildElement("event"))) {
        if ((tmp = tmp->FirstChildElement("items"))) {
            iotDevice->on_pub_sub_event_message(tmp);
        }
    }
}

void XmppTask::handleIoTMessageMessage(const char* msg) {
    /*#ifdef SPEAKER
        if (!strcmp("set speaker 1", msg)) {
            // speaker.set(1000);
        } else if (!strcmp("set speaker 0", msg)) {
            // speaker.set(0);
        }
    #endif  // SPEAKER*/
}

void XmppTask::event(const XmppClientConnectionState& event) {
    if (event == CLIENT_CONNECTED) {
        if (!storage->readBool(esp::Storage::SETUP_DONE)) {
            std::string to = storage->readString(esp::Storage::JID_SENDER);
            client->sendMessage(to, INITIAL_HELLO_MESSAGE);
        } else {
            onReady();
        }
    }
}

void XmppTask::event(messages::Message& event) {
    const tinyxml2::XMLDocument& doc = event.toXmlDoc();
    const tinyxml2::XMLElement* elem = nullptr;
    if ((elem = doc.FirstChildElement("message"))) {
        handleMessageMessages(elem);
    } else if ((elem = doc.FirstChildElement("presence"))) {
        handlePresenceMessages(elem);
    } else {
        iotDevice->event(event);
    }
}

void XmppTask::on_button_pressed(const std::string& id) {
    if(coffeeMaker.is_locked()) {
        update_status("Unable to brew coffee. Currently brewing one...");
        return;
    }

    if (id == ID_ACTUATOR_BUTTON_BREW) {
        messages::xep_iot::SliderNode* beansNode = iotDevice->get_node<messages::xep_iot::SliderNode>(ID_ACTUATOR_SLIDER_BEANS);
        assert(beansNode);
        int grindTime = static_cast<int>(std::round(3600.0 * (beansNode->get_value() / 100.0)));

        messages::xep_iot::SliderNode* waterNode = iotDevice->get_node<messages::xep_iot::SliderNode>(ID_ACTUATOR_SLIDER_WATER);
        assert(waterNode);
        int waterTime = static_cast<int>(std::round(40000.0 * (waterNode->get_value() / 100.0)));
        
        update_status("Brewing a custom coffee with [" + std::to_string(grindTime) + ", " + std::to_string(waterTime) + "].");
        coffeeMaker.brew_custom_coffee(std::chrono::milliseconds(grindTime), std::chrono::milliseconds(waterTime));
        update_status("Brewing custom coffee done. Enjoy!");
    } else {
        update_status("Brewing: " + id);
        coffeeMaker.brew_coffee(ID_TYPE_MAP.at(id));
        update_status( "Brewing "  + id + " done. Enjoy!");
    }
}

void XmppTask::update_status(std::string&& msg) {
    std::cout << msg << '\n';

    messages::xep_iot::TextSingleNode* statusNode = iotDevice->get_node<messages::xep_iot::TextSingleNode>(ID_SENSOR_TEXT_STATUS);
    statusNode->set_text(std::move(msg));
    iotDevice->publish_node(statusNode);
}
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp
//---------------------------------------------------------------------------
