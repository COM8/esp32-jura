#include "IQMessage.hpp"

#include <cassert>
#include <iostream>

//---------------------------------------------------------------------------
namespace esp32jura::xmpp::messages {
//---------------------------------------------------------------------------
IQMessage::IQMessage(const Jid from, const Jid to, IQType iqType, std::string&& id) : AbstractAddressableMessage(from, to, std::move(id)), iqType(iqType) {}

IQMessage::IQMessage(const Jid from, const Jid to, IQType iqType) : AbstractAddressableMessage(from, to), iqType(iqType) {}

IQMessage::IQMessage(std::string&& id, IQType iqType) : AbstractAddressableMessage(Jid(), Jid(), std::move(id)), iqType(iqType) {}

IQMessage::IQMessage(IQType iqType) : AbstractAddressableMessage(Jid(), Jid()), iqType(iqType) {}

IQMessage::IQMessage(const tinyxml2::XMLElement* node) : AbstractAddressableMessage(get_from(node), get_to(node), get_id(node)), iqType(get_iq_type(node)) {}

MessageType IQMessage::get_type() { return MessageType::IQMessageType; }

Jid IQMessage::get_from(const tinyxml2::XMLElement* node) {
    const tinyxml2::XMLAttribute* fromAttrib = node->FindAttribute("from");
    if (fromAttrib) {
        return Jid::from_jid(fromAttrib->Value());
    }
    return Jid();
}

Jid IQMessage::get_to(const tinyxml2::XMLElement* node) {
    const tinyxml2::XMLAttribute* toAttrib = node->FindAttribute("to");
    if (toAttrib) {
        return Jid::from_jid(toAttrib->Value());
    }
    return Jid();
}

std::string IQMessage::get_id(const tinyxml2::XMLElement* node) {
    const tinyxml2::XMLAttribute* idAttrib = node->FindAttribute("id");
    if (idAttrib) {
        return idAttrib->Value();
    }
    return "";
}

IQType IQMessage::get_iq_type(const tinyxml2::XMLElement* node) {
    const tinyxml2::XMLAttribute* typeAttrib = node->FindAttribute("type");
    if (typeAttrib) {
        std::string type = typeAttrib->Value();
        if (type == "set") {
            return IQType::SET;
        } else if (type == "get") {
            return IQType::GET;
        } else if (type == "result") {
            return IQType::RESULT;
        } else if (type == "error") {
            return IQType::ERROR;
        } else {
            std::cerr << "Failed to parse IQ type from: " << type << '\n';
            assert(false);  // Should not happen
        }
    } else {
        std::cerr << "Failed to parse IQ type. No 'type' attribute found\n";
        assert(false);  // Should not happen
    }
}

tinyxml2::XMLElement* IQMessage::to_xml_elem(tinyxml2::XMLDocument& doc) const {
    tinyxml2::XMLElement* iqNode = doc.NewElement("iq");
    std::string typeStr;
    switch (iqType) {
        case IQType::ERROR:
            typeStr = "error";
            break;

        case IQType::GET:
            typeStr = "get";
            break;

        case IQType::SET:
            typeStr = "set";
            break;

        case IQType::RESULT:
            typeStr = "result";
            break;

        default:
            assert(false);  // Should not happen
            break;
    }
    iqNode->SetAttribute("type", typeStr.c_str());
    iqNode->SetAttribute("id", id.c_str());
    if (from.is_valid()) {
        iqNode->SetAttribute("from", from.get_full().c_str());
    }
    if (to.is_valid()) {
        iqNode->SetAttribute("to", to.get_full().c_str());
    }
    tinyxml2::XMLElement* query = get_query(doc);
    if (query) {
        iqNode->InsertEndChild(query);
    }
    return iqNode;
}

tinyxml2::XMLElement* IQMessage::get_query(tinyxml2::XMLDocument& doc) const { return nullptr; }

//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp::messages
//---------------------------------------------------------------------------
