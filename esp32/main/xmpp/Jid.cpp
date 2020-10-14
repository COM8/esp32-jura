#include "Jid.hpp"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp {
//---------------------------------------------------------------------------
Jid::Jid(std::string&& userPart, std::string&& domainPart, std::string&& resourcePart)
    : isValid(!userPart.empty() && domainPart.empty()), userPart(std::move(userPart)), domainPart(std::move(domainPart)), resourcePart(std::move(resourcePart)) {}

std::string Jid::get_bare() const { return userPart + "@" + domainPart; }

std::string Jid::get_full() const { return get_bare() + "/" + resourcePart; }

void Jid::split_bare_jid(const std::string& bareJid, std::string* userPart, std::string* domainPart) {
    std::size_t pos = bareJid.find('@');
    *userPart = bareJid.substr(0, pos);
    *domainPart = bareJid.substr(pos + 1, bareJid.length() - pos);
}

void Jid::split_full_jid(const std::string& fullJid, std::string* userPart, std::string* domainPart, std::string* resourcePart) {
    std::string tmp;
    split_bare_jid(fullJid, userPart, &tmp);

    std::size_t pos = tmp.find('/');
    *domainPart = tmp.substr(0, pos);
    *resourcePart = tmp.substr(pos + 1, tmp.length() - pos);
}

Jid&& Jid::from_bare_jid(const std::string& bareJid) {
    std::string userPart;
    std::string domainPart;
    std::string resourcePart;
    split_bare_jid(bareJid, &userPart, &domainPart);

    return std::move(Jid(std::move(userPart), std::move(domainPart), std::move(resourcePart)));
}

Jid&& Jid::from_full_jid(const std::string& fullJid) {
    std::string userPart;
    std::string domainPart;
    std::string resourcePart;
    split_full_jid(fullJid, &userPart, &domainPart, &resourcePart);

    return std::move(Jid(std::move(userPart), std::move(domainPart), std::move(resourcePart)));
}

Jid&& Jid::from_jid(const std::string& jid) { return from_full_jid(jid); }

void Jid::print() const {
    std::cout << "JID:\n";
    std::cout << "User part: " << userPart << "\n";
    std::cout << "Domain part: " << domainPart << "\n";
    std::cout << "Resource part: " << resourcePart << "\n";
    std::cout << get_bare() << std::endl;
}

bool Jid::is_valid() const { return isValid; }

bool Jid::is_full() const { return isValid && !resourcePart.empty(); }

bool Jid::is_bare() const { return isValid && resourcePart.empty(); }
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp
//---------------------------------------------------------------------------
