#include "XmppProtocol.hpp"

#include "../XmppUtils.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp::tcp {
//---------------------------------------------------------------------------
int XmppProtocol::get_wanted_amount(XmppPacket& packet) { return XmppPacket::PACKET_SIZE; }

void XmppProtocol::data_received(XmppPacket& packet, int length) {
    complete = length < XmppPacket::PACKET_SIZE;
    offset += length;

    // Resize for the next packet:
    if (!complete) {
        packet.resize(offset + XmppPacket::PACKET_SIZE);
    }
}

uint8_t* XmppProtocol::get_write_pos(XmppPacket& packet) {
    size_t maxSize = offset + XmppPacket::PACKET_SIZE;
    if (packet.size() < maxSize) {
        packet.resize(maxSize);
    }
    return packet.get_data_vec().data() + offset;
}

bool XmppProtocol::is_complete(XmppPacket& packet) const { return complete; }

bool XmppProtocol::is_error() { return false; }

void XmppProtocol::packet_consumed() {
    complete = false;
    offset = 0;
}

void XmppProtocol::reset() { packet_consumed(); }
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp::tcp
//---------------------------------------------------------------------------
