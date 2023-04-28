#pragma once

#include <utility>

#include "SuperMenuDataBlock.h"
#include "../pdfs.h"
#include "../storage/storage.h"
#include "../utils/StringUtils.h"
#include "../../lib/stream/StringInputStream.h"

namespace pdfs {
    class SuperMenuDataFs : public Pdfs {
        StoragePtr storagePtr;
        SuperMenuDataBlockPtr firstBlock; // this block storage all menu

        stream::InputStreamPtr read(int block, int index) {
            SuperMenuDataBlockPtr blockPtr;
            if (block == 0) {
                blockPtr = firstBlock;
            } else {
                auto data = storagePtr->read(index)->readAll();
                blockPtr = SuperMenuDataBlockPtr(SuperMenuDataBlock::decode(data));
            }

            if (index >= blockPtr->data.dataNodes.size()) {
                puts("index >= blockPtr->data.dataNodes.size()");
                exit(-1);
            } else {
                auto &node = blockPtr->data.dataNodes[index];
                if (node.simpleData) {
                    return stream::InputStreamPtr(new stream::StringInputStream(node.data));
                } else {
                    throw 1;
                }
            }
        }

        void lazy() {
            if (firstBlock == nullptr) {
                auto data = storagePtr->read(0)->readAll();
                firstBlock = SuperMenuDataBlockPtr(SuperMenuDataBlock::decode(data));
            }
        }

        void clear() {

        }

        stream::InputStreamPtr read(std::string filename, std::int64_t start, int len) override {
            lazy();

            auto filenames = splitString(filename, "/");
            auto root = &firstBlock->menu.root;
            for (const auto &pathNode: filenames) {
                bool find = false;
                for (auto &item: root->children) {
                    if (item.name == pathNode) {
                        root = &item;
                        find = true;
                        break;
                    }
                }
                if (!find) {
                    throw 1;
                }
            }

            if (root->dataInWhichBlock != -1 && root->dataInWhichIndex != -1) {
                // found here
                return read(root->dataInWhichBlock, root->dataInWhichIndex);

            }
            throw 1;

        }

        bool write(std::string filename, std::int64_t start, int len, stream::InputStreamPtr ptr) override {
            lazy();
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

    public:
        explicit SuperMenuDataFs(StoragePtr storagePtr) {
            this->storagePtr = std::move(storagePtr);
        }
    };
}
