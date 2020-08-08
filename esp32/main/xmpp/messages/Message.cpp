#include "Message.hpp"

#include "../XmppUtils.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp::messages {
//---------------------------------------------------------------------------
Message::Message(const std::vector<uint8_t>& data) : data(data), wstringCache(nullptr), stringCache(nullptr), xmlDocCache(nullptr) {}

const std::wstring& Message::toWstring() {
    if (!wstringCache) {
        wstringCache = std::make_unique<std::wstring>(wstring_convert_from_bytes(data));
    }
    return *wstringCache;
}

const std::string& Message::toString() {
    if (!stringCache) {
        stringCache = std::make_unique<std::string>(data.begin(), data.end());
    }
    return *stringCache;
}

const tinyxml2::XMLDocument& Message::toXmlDoc() {
    if (!xmlDocCache) {
        xmlDocCache = std::make_unique<tinyxml2::XMLDocument>();
        xmlDocCache->Parse(toString().c_str());
    }
    return *xmlDocCache;
}

const std::vector<uint8_t>& Message::toVec() { return data; }
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp::messages
   //---------------------------------------------------------------------------