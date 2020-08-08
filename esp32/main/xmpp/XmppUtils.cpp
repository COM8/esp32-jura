#include "XmppUtils.hpp"

#include <codecvt>
#include <locale>

#include "esp_system.h"

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

std::string randUuid() { return randHexString(8) + '-' + randHexString(4) + '-' + randHexString(4) + '-' + randHexString(4) + '-' + randHexString(12); }

std::string randFakeUuid() {
    return std::to_string(esp_random()) + "-" + std::to_string(esp_random()) + "-" + std::to_string(esp_random()) + "-" + std::to_string(esp_random()) + "-" + std::to_string(esp_random());
}

std::string randHexString(std::size_t len) {
    size_t bytes = len / 2;
    uint8_t* buff = static_cast<uint8_t*>(malloc(bytes));
    esp_fill_random(buff, bytes);
    char* outBuff = static_cast<char*>(malloc(sizeof(char) * (len + 1)));
    for (size_t i = 0; i < bytes; i++) {
        sprintf(outBuff + i, "%02X", buff[i]);
    }
    outBuff[len] = '\0';
    std::string result(outBuff, len);
    if (len % 2 != 0) {
        result.substr(1, result.length() - 1);
    }
    return result;
}
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp
   //---------------------------------------------------------------------------