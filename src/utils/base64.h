#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <algorithm>

namespace base64 {

    const std::string base64_chars =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789+/";

    inline bool is_base64(unsigned char c) {
        return (isalnum(c) || (c == '+') || (c == '/'));
    }

    std::string encode(const char *data, size_t length) {
        std::stringstream ss;

        for (size_t i = 0; i < length; i += 3) {
            unsigned char buffer[3] = {0};
            size_t remaining = std::min((size_t) 3, length - i);
            memcpy(buffer, data + i, remaining);

            ss << base64_chars[(buffer[0] & 0xfc) >> 2];
            ss << (base64_chars[((buffer[0] & 0x03) << 4) | ((buffer[1] & 0xf0) >> 4)]);
            ss << (remaining > 1 ? base64_chars[((buffer[1] & 0x0f) << 2) | ((buffer[2] & 0xc0) >> 6)] : '=');
            ss << (remaining > 2 ? base64_chars[buffer[2] & 0x3f] : '=');
        }

        return ss.str();
    }

    std::string decode(const std::string &encoded) {
        size_t length = encoded.length();
        size_t padding = std::count(encoded.begin(), encoded.end(), '=');

        if (padding > 2) {
            return "";
        }

        size_t output_length = 3 * length / 4 - padding;
        std::string decoded(output_length, '\0');

        size_t out_pos = 0;

        for (size_t i = 0; i < length; i += 4) {
            unsigned char buffer[4] = {0};
            for (size_t j = 0; j < 4 && i + j < length; ++j) {
                buffer[j] = static_cast<unsigned char>(base64_chars.find(encoded[i + j]));
            }
            unsigned char data[3] = {static_cast<unsigned char>(((buffer[0] << 2) & 0xfc) | ((buffer[1] >> 4) & 0x03))};
            if (buffer[2] < 64) {
                data[1] = static_cast<unsigned char>(((buffer[1] << 4) & 0xf0) | ((buffer[2] >> 2) & 0x0f));
            }
            if (buffer[3] < 64) {
                data[2] = static_cast<unsigned char>(((buffer[2] << 6) & 0xc0) | (buffer[3] & 0x3f));
            }
            memcpy(&decoded[out_pos], data, std::min(output_length - out_pos, static_cast<size_t>(3)));
            out_pos += 3;
        }

        return decoded;
    }

//    int main() {
//        std::string test_string = "Hello, World!";
//        std::string encoded = base64_encode(reinterpret_cast<const unsigned char *>(test_string.c_str()),
//                                            test_string.length());
//        std::string decoded = base64_decode(encoded);
//        std::cout << "String: " << test_string << std::endl;
//        std::cout << "Encoded: " << encoded << std::endl;
//        std::cout << "Decoded: " << decoded << std::endl;
//        return 0;
//    }
}