#pragma once

#include <string>
#include <vector>

std::string serialization(const int64_t &t) {
    std::string res;
    auto v = t;
    for (int i = 0; i < 8; i++) {
        res.push_back(v & 0xff);
        v >>= 8;
    }
    return res;
}


void deserialization(const char *data, int len, int64_t &rhs) {
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
std::string serialization(const T &t) {
    return t.serialization();
}

template<class T>
void deserialization(const char *data, int len, int begin, T &rhs) {
    rhs.deserialization(data, begin);
}

template<class T>
std::string serialization(const std::vector<T> &t) {
    std::string res;
    res += serialization(int64_t(t.size()));
    for (const auto &item: t) {
        auto itemData = serialization(item);
        res += serialization(int64_t(itemData.size()));
        res += itemData;
    }
    return res;
}

template<class T>
void deserialization(const char *data, int len, std::vector<T> &rhs) {
    rhs.clear();
    int64_t size;
    int now = 0;
    deserialization(data + now, 8, size);
    now += 8;
    for (int i = 0; i < size; i++) {
        T t;
        int64_t itemLen;
        deserialization(data + now, 8, itemLen);
        now += 8;
        deserialization(data + now, itemLen, t);
        now += itemLen;
        rhs.push_back(t);
    }
}

void _test_deserialization_vector() {
    {
        int64_t x = 902848;
        auto inttest = serialization(x);
        deserialization(inttest.data(), inttest.length(), x);
        if (x != 902848) {
            puts("ERR");
            exit(-1);
        }
    }

    std::vector<int64_t> a, b;
//    for (int i = 0; i < 3; i++)a.push_back(i);
//    a.push_back(2);
//    a.push_back(-1);
    a.push_back(902848);
    auto x = serialization(a);
    deserialization(x.data(), x.size(), b);
    if (a.size() != b.size()) {
        puts("ERR");
        exit(-1);
    }
    for (int i = 0; i < a.size(); i++) {
        if (a[i] != b[i]) {
            puts("ERR");
            exit(-1);
        }
    }


}
