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


        SuperMenuDataBlockPtr readAndParseBlock(int block) {
            if (block == 0) {
                return firstBlock;
            } else {
                return SuperMenuDataBlockPtr(SuperMenuDataBlock::decode(storagePtr->read(block)->readAll()));
            }
        }

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


        stream::InputStreamPtr read(std::string filename, int64_t start, int len) override {
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

            if (root->dataBlock.dataInWhichBlock != -1 && root->dataBlock.dataInWhichIndex != -1) {
                // found here
                if (root->status != SuperMenuDataBlock::Menu::MenuNode::valid) {
                    puts("not valid file");
                    exit(-1);
                }
                return read(root->dataBlock.dataInWhichBlock, root->dataBlock.dataInWhichIndex);

            }
            throw NotFoundError("");
        }

        void findSpaceForNewFile(SuperMenuDataBlock::Menu::MenuNode *pNode, int size) {
            // simple little file , storage in a block

            // big file, split data and storage in multi block
            auto &remainSizes = firstBlock->super.remainSize;
            std::vector<SuperMenuDataBlock::DataBlock> spaceSplit;
            for (int i = 0; i < remainSizes.size(); i++) {
                if (remainSizes[i] > 0) {
                    SuperMenuDataBlock::DataBlock block;

                    int need = std::min(size, remainSizes[i]);

                    block.size = need;
                    block.dataInWhichIndex = -1;
                    block.dataInWhichBlock = i;

                    spaceSplit.push_back(block);
                    remainSizes[i] -= need;
                    size -= need;
                }
                if (size == 0) {
                    break;
                }
            }
            if (size != 0) {
                puts("OOD");
                exit(-1);
            }

            auto data = encodeDataBlocks(spaceSplit);
//            spaceSplit = decodeDataBlocks(data);
            createNewFile(pNode, stream::fromString(data), size, false);
        }

        int64_t createNewFile(SuperMenuDataBlock::Menu::MenuNode *pNode, const stream::InputStreamPtr &ptr, int size,
                              bool simple) {
            auto &remainSize = firstBlock->super.remainSize;
            for (int i = 0; i < remainSize.size(); i++) {
                if (size <= remainSize[i]) {
                    auto block = readAndParseBlock(i);
//                    remainSize[i] = block->remainSize();

                    if (size <= remainSize[i]) {
                        // put all file into the first block
                        SuperMenuDataBlock::Data::DataNode item;
                        if (simple) {
                            item.simpleData = true;
                            item.status = SuperMenuDataBlock::Data::DataNode::valid;
                            item.data = ptr->readAll();
                            pNode->status = SuperMenuDataBlock::Menu::MenuNode::MenuStatus::valid;
                        } else {
                            item.simpleData = false;
                            item.status = SuperMenuDataBlock::Data::DataNode::uploading;
                            item.data = ptr->readAll();
                            pNode->status = SuperMenuDataBlock::Menu::MenuNode::MenuStatus::uploading;
                        }

                        block->data.dataNodes.push_back(item);

                        pNode->dataBlock.dataInWhichIndex = block->data.dataNodes.size() - 1;
                        pNode->dataBlock.dataInWhichBlock = i;
                        return size;
                    }
                }
            }

            if (size > storagePtr->everyBlockBytes()) {
                puts("UNSUPPORT");
                exit(-1);
            }

            puts("OOD");
            exit(-1);
        }

        int64_t updateOldFile(SuperMenuDataBlock::Menu::MenuNode *pNode, int64_t start, int len,
                              const stream::InputStreamPtr &ptr) {
            auto block = readAndParseBlock(pNode->dataBlock.dataInWhichBlock);
            auto dataPtr = &block->data.dataNodes[pNode->dataBlock.dataInWhichIndex];
            if (dataPtr->simpleData) {
                dataPtr->data = ptr->readAll();
            } else {
                std::vector<SuperMenuDataBlock::DataBlock> blocks = decodeDataBlocks(dataPtr->data);
                int sum = 0;
                for (auto &bk: blocks) {
                    // this block's is range [sum,sum+blocks[bk].size)
                    if (start >= sum && start < sum + bk.size) { // if this block need to write something
                        if (start + len > sum + bk.size) { // if this update want to update not only one block
                            // only write substring
                            len = sum + bk.size - start;
                        }
                        block = readAndParseBlock(bk.dataInWhichBlock);
                        if (bk.dataInWhichIndex != -1) {
                            dataPtr = &block->data.dataNodes[bk.dataInWhichIndex];
                            if (dataPtr->simpleData) {
                                dataPtr->data = ptr->readAll();
                                // TODO write to disk
                                return len;
                            } else {
                                puts("dataPtr not simpleData error");
                                exit(-1);
                            }
                        } else {
                            // need write new

                            // TODO write to disk
                            bk.dataInWhichIndex = block->data.dataNodes.size();
                            dataPtr->data = encodeDataBlocks(blocks);

                            SuperMenuDataBlock::Data::DataNode node;
                            node.data = ptr->readAll();
                            node.simpleData = true;
                            node.status = SuperMenuDataBlock::Data::DataNode::valid;
                            block->data.dataNodes.push_back(node);
                            return len;

                        }

                    }
                    sum += bk.size;
                }

            }
            puts("updateOldFile ERROR");
            return 0;

        }

        int64_t write(std::string filename, int64_t start, int len, stream::InputStreamPtr ptr) override {
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

                        if (len > storagePtr->everyBlockBytes()) {
                            findSpaceForNewFile(root, len);
                            return updateOldFile(root, start, len, ptr);
                        } else {
                            return createNewFile(root, ptr, len, true);
                        }
                    }
                }
            }
            return updateOldFile(root, start, len, ptr);
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
