#pragma once

#include "storage.h"
#include "../../lib/stream/InputStream.h"
#include "../../lib/stream/StringInputStream.h"

namespace pdfs {
    class MemoryStorage : public Storage {
        std::shared_ptr<char> buf;
        int blockNumbersSize;
        int everyBlockBytesSize;

    public:
        MemoryStorage(int blockNumbers, int everyBlockBytes) {
            int total = blockNumbers * everyBlockBytes;
            buf = std::shared_ptr<char>(new char[total]);
            blockNumbersSize = blockNumbers;
            everyBlockBytesSize = everyBlockBytes;
        }

        stream::InputStreamPtr read(int index) override {
            std::string res;
            for (int i = 0; i < everyBlockBytesSize; i++) {
                res.push_back(buf.get()[index * everyBlockBytesSize + i]);
            }
            return stream::InputStreamPtr(new stream::StringInputStream(res));
        }

        void write(int index, std::string s) override {
            if (s.length() != everyBlockBytesSize) {
                puts("s.length() != everyBlockBytesSize");
                exit(-1);
            }
            for (int i = 0; i < s.length(); i++) {
                buf.get()[index * everyBlockBytesSize + i] = s[i];
            }
        }

        int blockNumbers() override {
            return blockNumbersSize;
        }

        int everyBlockBytes() override {
            return everyBlockBytesSize;
        }
    };
};