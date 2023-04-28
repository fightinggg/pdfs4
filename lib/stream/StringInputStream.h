#pragma once
#include "InputStream.h"

namespace stream {
    class StringInputStream : public InputStream {
        std::string data;
        int index;

    public:
        StringInputStream(std::string s) {
            data = s;
            index = 0;
        }

        bool read(char &ch) {
            if (index == data.size()) {
                return false;
            }
            ch = data[index++];
            return true;
        }
    };

    InputStreamPtr fromString(std::string s) {
        return InputStreamPtr(new StringInputStream(std::move(s)));
    }
}