#include "AbstractMessage.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp::messages {
//---------------------------------------------------------------------------
AbstractMessage::AbstractMessage(std::string&& id) : id(std::move(id)) {}

std::string AbstractMessage::to_xml_str() const {
    tinyxml2::XMLPrinter printer;
    tinyxml2::XMLDocument doc;
    to_xml_elem(doc)->Accept(&printer);
    return printer.CStr();
}
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp::messages
//---------------------------------------------------------------------------
