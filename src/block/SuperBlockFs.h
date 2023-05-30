#pragma once

#include <vector>
#include <string>
#include <map>
#include <set>
#include <algorithm>

#include "SuperMenuDataBlock.h"
#include "../pdfs.h"
#include "../storage/storage.h"
#include "../utils/StringUtils.h"
#include "../../lib/stream/StringInputStream.h"
#include "../errors.h"
#include "../utils/serialization.h"


namespace pdfs {
    using int64 = long long;
    template<class T>
    using vector = std::vector<T>;
    using string = std::string;
    template<class K, class V>
    using map = std::map<K, V>;
    template<class T>
    using set = std::set<T>;
    using Block = map<int64, string>;


    std::string serialization(const Block &t) {
        string s;
        s += ::serialization(int64(t.size()));
        for (const auto &item: t) {
            s += ::serialization(int64(item.first));
            s += ::serialization(int64(item.second.length()));
            s += item.second;
        }
        return s;
    }


    void deserialization(const char *data, int len, Block &rhs) {
        rhs.clear();

        int64 size, mark, dataSize;
        ::deserialization(data, 8, size);
        data += 8;

        for (int i = 0; i < size; i++) {
            ::deserialization(data, 8, mark);
            data += 8;

            ::deserialization(data, 8, dataSize);
            data += 8;

            rhs[mark] = string(data, data + dataSize);
            data += dataSize;
        }
    }

    class SuperBlockFs : public Pdfs {

        class FileBlock {
        public:
            int64 block;
            int64 mark;
            int64 size;
        };

        class File {
        public:
            string filename;
//            int updateTime;
            vector<FileBlock> fileBlocks;
        };


        using Files = vector<File>;

        Files allFiles;
        StoragePtr storagePtr;

    public:
        explicit SuperBlockFs(StoragePtr storagePtr) {
            this->storagePtr = std::move(storagePtr);
        }

        bool createFile(string filename, int64 size) override {
            if (filename.empty() || filename[0] != '/') {
                return false;
            }
            // check exist
            for (const auto &file: allFiles) {
                if (file.filename == filename) {
                    return false;
                }
            }

            int oneBlockMaxBytes = storagePtr->everyBlockBytes() - 8 - 16;
            int blockSize = size / oneBlockMaxBytes;
            int remainSize = size % oneBlockMaxBytes;

            vector<FileBlock> fileBlocks;
            for (int i = 0; i < blockSize; i++) {
                auto fileBlock = createInOneBlock(oneBlockMaxBytes, true, fileBlocks);
                if (fileBlock.block != -1) {
                    fileBlocks.push_back(fileBlock);
                } else {
                    return false;
                }
            }

            if (remainSize != 0) {
                auto fileBlock = createInOneBlock(remainSize, false, fileBlocks);
                if (fileBlock.block != -1) {
                    fileBlocks.push_back(fileBlock);
                } else {
                    fileBlock = createInOneBlock(remainSize, true, fileBlocks);
                    if (fileBlock.block != -1) {
                        fileBlocks.push_back(fileBlock);
                    } else {
                        return false;
                    }
                }
            }

            File file;
            file.filename = filename;
            file.fileBlocks = fileBlocks;
            allFiles.push_back(file);
            return true;
        }


        FileBlock createInOneBlock(int size, bool canPutInEmpty, vector<FileBlock> fileBlocks) {
            FileBlock fileBlock;
            fileBlock.size = size;
            fileBlock.mark = -1;
            fileBlock.block = -1;

            set<int64> used;
            for (const auto &item: fileBlocks) {
                used.insert(item.block);
            }

            // find any to write is ok
            for (int i = 1; i < storagePtr->blockNumbers(); i++) {
                if (used.contains(i)) {
                    continue;
                }
                int used = 8;
                set<int64> usedMarks;
                for (auto &file: allFiles) {
                    for (auto &block: file.fileBlocks) {
                        if (block.block == i) {
                            used += block.size + 16;
                            usedMarks.insert(block.mark);
                        }
                    }
                }
                // not enough space
                if (16 + size > storagePtr->everyBlockBytes() - used) {
                    continue;
                }

                // can not put in empty
                if (!canPutInEmpty && used == 8) {
                    continue;
                }


                fileBlock.block = i;
                return fileBlock;
            }
            return fileBlock;
        }

        void deleteFile(string filename) {

        }


        Files list(string path) {

        }


        void judge();


        stream::InputStreamPtr read(std::string filename, int64_t start, int len) {
            string data = readOneBlock(filename, start, len);
            if (len <= data.length()) {
                return stream::fromString(data);
            } else if (data.length() == 0) {
                return stream::fromString(data);
            } else {
                // TODO lazy
                return stream::fromString(data + read(filename, start + data.length(), len - data.length())->readAll());
            }
        }

        string readOneBlock(std::string filename, int64_t start, int64 len) {
            for (auto &file: allFiles) {
                if (file.filename == filename) {
                    int64 curStart = 0;
                    for (auto &fileBlock: file.fileBlocks) {
                        if (curStart == start && curStart <= start && start < curStart + fileBlock.size) {
                            // update...
                            string blockData = storagePtr->read(fileBlock.block)->readAll();
                            Block block;
                            deserialization(blockData.data(), blockData.size(), block);

                            if (fileBlock.mark == -1) {
                                return string(std::min(fileBlock.size, len), '0');
                            }
                            return block[fileBlock.mark];
                        }
                        curStart += fileBlock.size;
                    }
                }
            }
            return "";
        }

        int64_t write(std::string filename, int64_t start, int len, stream::InputStreamPtr dataPtr) {
            if (filename.empty() || filename[0] != '/') {
                return 0;
            }

            for (auto &file: allFiles) {
                if (file.filename == filename) {
                    int64 curStart = 0;
                    for (auto &fileBlock: file.fileBlocks) {
                        if (curStart == start && curStart <= start && start < curStart + fileBlock.size) {
                            // update...
                            string blockData = storagePtr->read(fileBlock.block)->readAll();
                            Block block;
                            deserialization(blockData.data(), blockData.size(), block);

                            if (fileBlock.mark == -1) {
                                fileBlock.mark = 0;
                                while (block.contains(fileBlock.mark)) {
                                    fileBlock.mark++;
                                }
                            }

                            auto old = block[fileBlock.mark];
                            auto add = dataPtr->readN(fileBlock.size - old.size());
                            block[fileBlock.mark] = old + add;

                            auto needToWrite = serialization(block);
                            int size = storagePtr->everyBlockBytes() - needToWrite.length();
                            needToWrite += string(size, '\0');
                            storagePtr->write(fileBlock.block, needToWrite);

                            return add.size();
                        }
                        curStart += fileBlock.size;
                    }
                }
            }
            return 0;
        }

        bool deleteF(std::string filename) {}

        std::vector<FileInfo> ls(std::string filename) {
            if (filename.back() != '/') {
                exit(-1);
                puts("filename.back()!='/'");
            }
            auto paths = splitString(filename, '/');
            std::vector<FileInfo> res;
            std::set<string> dir;
            for (auto &file: allFiles) {
                auto filePaths = splitString(file.filename, '/');
                if (filePaths.size() == paths.size() + 1) {
                    string back = filePaths.back();
                    filePaths.pop_back();
                    if (paths == filePaths) {
                        FileInfo tmp;
                        tmp.name = back;
                        tmp.path = filename;
                        tmp.type = "file";
                        tmp.size = 0;
                        for (const auto &item: file.fileBlocks) {
                            tmp.size += item.size;
                        }
                        // TODO
                        tmp.updateTime = -1;
                        res.push_back(tmp);
                    }
                } else if (filePaths.size() > paths.size() + 1) {
                    dir.insert(filePaths[paths.size()]);
                }
            }
            for (const auto &item: dir) {
                FileInfo tmp;
                tmp.name = item;
                tmp.path = filename;
                tmp.type = "folder";
                tmp.size = 0;
                // TODO
                tmp.updateTime = -1;
                res.push_back(tmp);
            }
            return res;
        }

        bool mkdir(std::string filename) {}


    };

}