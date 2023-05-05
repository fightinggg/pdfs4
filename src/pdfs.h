#pragma once

#include "../lib/stream/InputStream.h"

namespace pdfs {
    struct FileInfo {
        int64_t size;
        std::string name;
        int64_t updateTime;
    };

    class Pdfs {
    public:
        virtual stream::InputStreamPtr read(std::string filename, int64_t start, int len) = 0;

        virtual bool write(std::string filename, int64_t start, int len, stream::InputStreamPtr) = 0;

        virtual bool deleteF(std::string filename) = 0;

        virtual std::vector<FileInfo> ls(std::string filename) = 0;

        virtual bool mkdir(std::string filename) = 0;
    };

    using PdfsPtr = std::shared_ptr<Pdfs>;
}