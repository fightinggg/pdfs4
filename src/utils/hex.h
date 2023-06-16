#include <string>

namespace hex {
    std::string hex(std::string s) {
        std::string res;
        static auto chars = "0123456789abcdef";
        for (char ch: s) {
            res += chars[ch & 0xff];
            res += chars[(ch >> 4) & 0xff];
        }
        return res;
    }
}