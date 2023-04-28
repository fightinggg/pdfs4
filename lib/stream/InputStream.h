#pragma once

#include <string>
#include <memory>

namespace stream {
    class InputStream {
    public:
        virtual bool read(char &ch) = 0;

        std::string readN(int n) {
            std::string res;
            char ch;
            while (n-- && read(ch)) {
                res += ch;
            }
            return res;
        }

        std::string readAll() {
            std::string res;
            char ch;
            while (read(ch)) {
                res += ch;
            }
            return res;
        }
    };

    using InputStreamPtr = std::shared_ptr<InputStream>;


}