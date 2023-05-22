#pragma once


#include <vector>
#include <string>
#include <map>
#include <set>
#include "../lib/stream/InputStream.h"

namespace pdfs {
    using int64 = long long;
    template<class T>
    using vector = std::vector<T>;
    using string = std::string;
    template<class K, class V>
    using map = std::map<K, V>;
    template<class T>
    using set = std::set<T>;


    struct FileInfo {
        int64_t size;
        std::string name;
        int64_t updateTime;
    };

    class Pdfs {
    public:
        virtual stream::InputStreamPtr read(std::string filename, int64_t start, int len) = 0;

        virtual int64_t write(std::string filename, int64_t start, int len, stream::InputStreamPtr) = 0;

        virtual bool deleteF(std::string filename) = 0;

        virtual std::vector<FileInfo> ls(std::string filename) = 0;

        virtual bool mkdir(std::string filename) = 0;

        virtual bool createFile(string filename, int64 size, string data) = 0;
    };

    using PdfsPtr = std::shared_ptr<Pdfs>;


    string toJson(const FileInfo &fileInfo) {
        return "";
    }
}