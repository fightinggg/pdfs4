#pragma once
#include <string>

unsigned char ToHex(unsigned char x) {
    return  x > 9 ? x + 55 : x + 48;
}

unsigned char FromHex(unsigned char x) {
    unsigned char y;
    if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
    else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
    else if (x >= '0' && x <= '9') y = x - '0';
    else return 0;
    return y;
}

std::string url_decode(const std::string& str) {
    std::string dst;
    dst = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++) {
        if (str[i] == '+') {
            dst += ' ';
        } else if (i + 2 < length && str[i] == '%') {
            unsigned char high = FromHex((unsigned char)str[++i]);
            unsigned char low = FromHex((unsigned char)str[++i]);
            dst += high*16 + low;
        } else {
            dst += str[i];
        }
    }
    return dst;
}
