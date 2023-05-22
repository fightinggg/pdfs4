#pragma once

#include <string>
#include <vector>

std::string toJson(const int64_t &t) {
    std::string res;
    auto v = t;
    for (int i = 0; i < 8; i++) {
        res.push_back(v & 0xff);
        v >>= 8;
    }
    return res;
}


void fromJson(const char *data, int len, int64_t &rhs) {
    static char debug[8];
    for (int i = 0; i < 8; i++)debug[i] = data[i];
    if (len != 8) {
        puts("len!=8");
        exit(-1);
    }
    rhs = 0;
    for (int i = 0; i < 8; i++) {
        rhs = rhs << 8 | (0xff & data[7 - i]);
    }
    return;
}


template<class T>
std::string toJson(const T &t) {
    return t.toJson();
}

template<class T>
void fromJson(const char *data, int len, int begin, T &rhs) {
    rhs.fromJson(data, begin);
}

template<class T>
std::string toJson(const std::vector<T> &t) {
    std::string res;
    res += toJson(int64_t(t.size()));
    for (const auto &item: t) {
        auto itemData = toJson(item);
        res += toJson(int64_t(itemData.size()));
        res += itemData;
    }
    return res;
}

template<class T>
void fromJson(const char *data, int len, std::vector<T> &rhs) {
    rhs.clear();
    int64_t size;
    int now = 0;
    fromJson(data + now, 8, size);
    now += 8;
    for (int i = 0; i < size; i++) {
        T t;
        int64_t itemLen;
        fromJson(data + now, 8, itemLen);
        now += 8;
        fromJson(data + now, itemLen, t);
        now += itemLen;
        rhs.push_back(t);
    }
}



