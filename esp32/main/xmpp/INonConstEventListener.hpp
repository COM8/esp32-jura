#pragma once

//---------------------------------------------------------------------------
namespace esp32jura::xmpp {
//---------------------------------------------------------------------------
template <typename EventType>
class INonConstEventListener {
   public:
    virtual ~INonConstEventListener() = default;

    virtual void event(EventType& event) = 0;
};
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp
//---------------------------------------------------------------------------
