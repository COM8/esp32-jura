#pragma once

#include <tinyxml2.h>

#include <memory>
#include <string>
#include <vector>

//---------------------------------------------------------------------------
namespace esp32jura::xmpp::messages {
//---------------------------------------------------------------------------
class Message {
   private:
    const std::vector<uint8_t> data;

    // Cache the following so we do not need to generate them every time:
    std::unique_ptr<std::wstring> wstringCache;
    std::unique_ptr<std::string> stringCache;
    std::unique_ptr<tinyxml2::XMLDocument> xmlDocCache;

   public:
    Message(const std::vector<uint8_t>& data);

    const std::wstring& toWstring();
    const std::string& toString();
    const tinyxml2::XMLDocument& toXmlDoc();
    const std::vector<uint8_t>& toVec();
};
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp::messages
   //---------------------------------------------------------------------------