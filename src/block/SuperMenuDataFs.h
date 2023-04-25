#pragma once

#include "SuperMenuDataBlock.h"
#include "../pdfs.h"
#include "../storage/storage.h"

namespace pdfs {
    class SuperMenuDataFs : Pdfs {
        StoragePtr storagePtr;

        stream::InputStreamPtr read(std::string filename, std::int64_t start, int len) override {
            storagePtr->read(0);
            return stream::InputStreamPtr();
        }

        bool write(std::string filename, std::int64_t start, int len, stream::InputStreamPtr ptr) override {
            return false;
        }

        bool deleteF(std::string filename) override {
            return false;
        }

        std::vector<FileInfo> ls(std::string filename) override {
            return std::vector<FileInfo>();
        }

        bool mkdir(std::string filename) override {
            return false;
        }
    };
}
