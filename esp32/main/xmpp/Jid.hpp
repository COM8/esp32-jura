#pragma once

#include <iostream>
#include <sstream>
#include <string>

//---------------------------------------------------------------------------
namespace esp32jura::xmpp {
//---------------------------------------------------------------------------
struct Jid {
    const std::string userPart;
    const std::string domainPart;
    const std::string resourcePart;

    Jid(std::string&& userPart, std::string&& domainPart, std::string&& resourcePart) : userPart(userPart), domainPart(domainPart), resourcePart(resourcePart) {}

    [[nodiscard]] std::string getBare() const { return userPart + "@" + domainPart; }

    [[nodiscard]] std::string getFull() const { return getBare() + "/" + resourcePart; }

    static void splitBareJid(const std::string& bareJid, std::string* userPart, std::string* domainPart) {
        std::size_t pos = bareJid.find('@');
        *userPart = bareJid.substr(0, pos);
        *domainPart = bareJid.substr(pos + 1, bareJid.length() - pos);
    }

    static void splitFullJid(const std::string& fullJid, std::string* userPart, std::string* domainPart, std::string* resourcePart) {
        std::string tmp = "";
        splitBareJid(fullJid, userPart, &tmp);

        std::size_t pos = tmp.find('/');
        *domainPart = tmp.substr(0, pos);
        *resourcePart = tmp.substr(pos + 1, tmp.length() - pos);
    }

    static Jid&& fromBareJid(const std::string& bareJid) {
        std::string userPart;
        std::string domainPart;
        std::string resourcePart = "";
        splitBareJid(bareJid, &userPart, &domainPart);

        return std::move(Jid(std::move(userPart), std::move(domainPart), std::move(resourcePart)));
    }

    static Jid&& fromFullJid(const std::string& fullJid) {
        std::string userPart;
        std::string domainPart;
        std::string resourcePart;
        splitFullJid(fullJid, &userPart, &domainPart, &resourcePart);

        return std::move(Jid(std::move(userPart), std::move(domainPart), std::move(resourcePart)));
    }

    void print() const {
        std::cout << "JID:\n";
        std::cout << "User part: " << userPart << "\n";
        std::cout << "Domain part: " << domainPart << "\n";
        std::cout << "Resource part: " << resourcePart << "\n";
        std::cout << getBare() << std::endl;
    }
};
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp
//---------------------------------------------------------------------------
