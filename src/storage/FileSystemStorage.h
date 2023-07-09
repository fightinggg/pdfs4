#pragma once

#include "storage.h"
#include "../../lib/stream/InputStream.h"
#include "../../lib/stream/StringInputStream.h"
#include <fstream>

namespace pdfs {
    class FileSystemStorage : public Storage {
        int blockNumbersSize;
        int everyBlockBytesSize;

    public:
        FileSystemStorage(int blockNumbers, int everyBlockBytes) {
            blockNumbersSize = blockNumbers;
            everyBlockBytesSize = everyBlockBytes;
        }

        stream::InputStreamPtr read(int index) override {
            string filename = "tmp/" + std::to_string(index) + ".txt";
            std::ifstream fin(filename);
            std::istreambuf_iterator<char> begin(fin);
            std::istreambuf_iterator<char> end;
            string read = std::string(begin, end);
            return stream::fromString(read);
        }

        void write(int index, std::string s) override {
            string filename = "tmp/" + std::to_string(index) + ".txt";
            std::ofstream fout(filename);
            fout << s;
        }

        int blockNumbers() override {
            return blockNumbersSize;
        }

        int everyBlockBytes() override {
            return everyBlockBytesSize;
        }
    };
};