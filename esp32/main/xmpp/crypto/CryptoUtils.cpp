#include "CryptoUtils.hpp"

#include <iostream>

#include "mbedtls/base64.h"

//---------------------------------------------------------------------------
namespace esp32jura::xmpp::crypto {
//---------------------------------------------------------------------------
std::string toBase64(const std::string& in) {
    std::size_t olen = 0;
    const unsigned char* inStr = reinterpret_cast<const unsigned char*>(in.c_str());
    // Get the size of the output in bytes stored in olen:
    mbedtls_base64_encode(nullptr, 0, &olen, inStr, in.length());

    // Do the actual conversion:
    // We do not need to write "sizeof(unsigned char) * olen" since olen is the number of bytes:
    unsigned char* buffer = static_cast<unsigned char*>(malloc(olen));
    mbedtls_base64_encode(buffer, olen, &olen, inStr, in.length());

    char* tmp = reinterpret_cast<char*>(buffer);
    std::string out(tmp, tmp + olen);
    free(buffer);
    return out;
}

std::string fromBase64(const std::string& in) {
    std::size_t olen = 0;
    const unsigned char* inStr = reinterpret_cast<const unsigned char*>(in.c_str());
    // Get the size of the output in bytes stored in olen:
    mbedtls_base64_decode(nullptr, 0, &olen, inStr, in.length());

    // Do the actual conversion:
    // We do not need to write "sizeof(unsigned char) * olen" since olen is the number of bytes:
    unsigned char* buffer = static_cast<unsigned char*>(malloc(olen));
    mbedtls_base64_decode(buffer, olen, &olen, inStr, in.length());

    char* tmp = reinterpret_cast<char*>(buffer);
    std::string out(tmp, tmp + olen);
    free(buffer);
    return out;
}

void testBase64() {
    testBase64("Hallo Welt", "SGFsbG8gV2VsdA==");
    testBase64("abc123!?$*&()'-=@~", "YWJjMTIzIT8kKiYoKSctPUB+");
    testBase64(std::string("\0test1\0password", 15), "AHRlc3QxAHBhc3N3b3Jk");
}

void testBase64(std::string refIn, std::string refOut) {
    std::string out = toBase64(refIn);
    std::string in = fromBase64(out);

    if (refIn != in) {
        std::cout << "refIn != in\n";
        std::cout << "refIn: " << refIn << "\n";
        std::cout << "in: " << in << "\n";
    }

    if (refOut != out) {
        std::cout << "refOut != out\n";
        std::cout << "refOut: " << refOut << "\n";
        std::cout << "out: " << out << "\n";
    }
}
//---------------------------------------------------------------------------
}  // namespace esp32jura::xmpp::crypto
   //---------------------------------------------------------------------------