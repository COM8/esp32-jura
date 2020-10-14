#include "XmppUtils.hpp"

#include <codecvt>
#include <locale>
#include <random>
#include <sstream>

//---------------------------------------------------------------------------
namespace esp32jura::xmpp {
//---------------------------------------------------------------------------
std::wstring wstring_convert_from_char(const char* str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.from_bytes(str);
}

std::string string_convert_from_wchar(const wchar_t* str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.to_bytes(str);
}

std::wstring wstring_convert_from_bytes(const std::vector<uint8_t>& v) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    const std::vector<char> tmp(v.begin(), v.end());
    return wstring_convert_from_bytes(tmp);
}

std::wstring wstring_convert_from_bytes(const std::vector<char>& v) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.from_bytes(v.data(), v.data() + v.size());
}

std::vector<uint8_t> wstring_convert_to_bytes(const wchar_t* str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    std::string string = converter.to_bytes(str);
    return std::vector<uint8_t>(string.begin(), string.end());
}

std::string generate_uuid_v4() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution dis(0, 15);
    static std::uniform_int_distribution dis2(8, 11);

    std::stringstream ss;
    int i;
    ss << std::hex;
    for (i = 0; i < 8; i++) {
        ss << dis(gen);
    }
    ss << "-";
    for (i = 0; i < 4; i++) {
        ss << dis(gen);
    }
    ss << "-4";
    for (i = 0; i < 3; i++) {
        ss << dis(gen);
    }
    ss << "-";
    ss << dis2(gen);
    for (i = 0; i < 3; i++) {
        ss << dis(gen);
    }
    ss << "-";
    for (i = 0; i < 12; i++) {
        ss << dis(gen);
    };
    return ss.str();
}
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp
   //---------------------------------------------------------------------------