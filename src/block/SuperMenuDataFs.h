#pragma once

#include <utility>

#include "SuperMenuDataBlock.h"
#include "../pdfs.h"
#include "../storage/storage.h"
#include "../utils/StringUtils.h"
#include "../../lib/stream/StringInputStream.h"
#include "../errors.h"

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

            auto filenames = parsePath(filename);
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
                    throw NotFoundError("");
                }
            }

            if (root->dataInWhichBlock != -1 && root->dataInWhichIndex != -1) {
                // found here
                return read(root->dataInWhichBlock, root->dataInWhichIndex);

            }
            throw NotFoundError("");
        }

        void createNewFile(SuperMenuDataBlock::Menu::MenuNode *pNode, const stream::InputStreamPtr &ptr) {
            // put all file in first
            // TODO
            SuperMenuDataBlock::Data::DataNode item;
            item.simpleData = true;
            item.status = SuperMenuDataBlock::Data::DataNode::valid;
            item.data = ptr->readAll();
            firstBlock->data.dataNodes.push_back(item);

            pNode->dataInWhichIndex = firstBlock->data.dataNodes.size() - 1;
            pNode->dataInWhichBlock = 0;
        }

        void updateOldFile(SuperMenuDataBlock::Menu::MenuNode *pNode, const stream::InputStreamPtr &ptr) {
            firstBlock->data.dataNodes[pNode->dataInWhichIndex].data = ptr->readAll();
        }

        bool write(std::string filename, std::int64_t start, int len, stream::InputStreamPtr ptr) override {
            lazy();
            auto filenames = parsePath(filename);

            auto root = &firstBlock->menu.root;

            for (int i = 0; i < filenames.size(); i++) {
                const auto &pathNode = filenames[i];
                bool find = false;
                for (auto &item: root->children) {
                    if (item.name == pathNode) {
                        root = &item;
                        find = true;
                        break;
                    }
                }
                if (!find) {
                    if (i != filenames.size() - 1) {
                        SuperMenuDataBlock::Menu::MenuNode node;
                        node.name = pathNode;
                        node.isDir = true;
                        root->children.push_back(node);

                        root = &root->children.back();
                    } else {
                        SuperMenuDataBlock::Menu::MenuNode node;
                        node.name = pathNode;
                        node.isDir = false;
                        root->children.push_back(node);

                        root = &root->children.back();
                        createNewFile(root, ptr);
                        return true;
                    }
                }
            }
            updateOldFile(root, ptr);
            return true;
        }

        bool deleteF(std::string filename) override {
            return false;
        }

        std::vector<FileInfo> ls(std::string filename) override {
            return std::vector<FileInfo>();
        }

        std::vector<std::string> parsePath(const std::string &path) {
            auto split = splitString(path, '/');
            std::vector<std::string> res;
            for (const auto &item: split) {
                if (!item.empty()) {
                    res.push_back(item);
                }
            }
            return res;
        }

        bool mkdir(std::string filename) override {
            lazy();
            auto filenames = parsePath(filename);

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
                    SuperMenuDataBlock::Menu::MenuNode node;
                    node.name = pathNode;
                    node.isDir = true;
                    root->children.push_back(node);

                    root = &root->children.back();
                }
            }
            return true;
        }

    public:
        explicit SuperMenuDataFs(StoragePtr storagePtr) {
            this->storagePtr = std::move(storagePtr);
        }
    };
}
