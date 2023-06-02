#pragma once

#include <string>
#include <vector>
#include <sstream>

std::vector<std::string> splitString(const std::string &str, char delimiter) {
    std::vector<std::string> tokens;

    std::stringstream ss(str);
    std::string token;

    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

std::string joinString(const std::vector<std::string> &str, char delimiter) {
    if (str.empty()) {
        return "";
    } else {
        std::string res = str[0];
        for (int i = 1; i < str.size(); i++) {
            res += delimiter;
            res += str[i];
        }
        return res;
    }
}

std::string subString(const std::string &str, int start, int len) {
    std::string res;
    for (int i = start; i - start < len && i < str.size(); i++) {
        res += str[i];
    }
    return res;
}