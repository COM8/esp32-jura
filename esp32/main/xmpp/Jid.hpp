#pragma once

#include <iostream>
#include <sstream>
#include <string>

//---------------------------------------------------------------------------
namespace esp32jura::xmpp {
//---------------------------------------------------------------------------
class Jid {
   private:
    const bool isValid{false};

   public:
    const std::string userPart{};
    const std::string domainPart{};
    const std::string resourcePart{};

    Jid(std::string&& userPart, std::string&& domainPart, std::string&& resourcePart);
    Jid(const Jid& other) = default;
    Jid() = default;
    Jid(Jid&& old) = default;
    ~Jid() = default;

    Jid& operator=(const Jid& other) = delete;
    Jid& operator=(Jid&& old) = delete;

    [[nodiscard]] std::string get_bare() const;
    [[nodiscard]] std::string get_full() const;

    static void split_bare_jid(const std::string& bareJid, std::string* userPart, std::string* domainPart);
    static void split_full_jid(const std::string& fullJid, std::string* userPart, std::string* domainPart, std::string* resourcePart);
    static Jid&& from_bare_jid(const std::string& bareJid);
    static Jid&& from_full_jid(const std::string& fullJid);
    static Jid&& from_jid(const std::string& jid);

    void print() const;
    bool is_valid() const;
    bool is_full() const;
    bool is_bare() const;
};
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp
//---------------------------------------------------------------------------
