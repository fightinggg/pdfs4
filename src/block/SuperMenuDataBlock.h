#include <memory>
#include <vector>
#include "../utils/serialization.h"

#pragma once

struct SuperMenuDataBlock;

using SuperMenuDataBlockPtr = std::shared_ptr<SuperMenuDataBlock>;


struct SuperMenuDataBlock {
    struct Super {
        std::vector<int> remainSize;
    };

    struct DataBlock {
        int dataInWhichBlock{};
        int dataInWhichIndex{};
        int size{};

    };

    struct Menu {
        struct MenuNode {
            enum MenuStatus {
                valid,
                deleting,
                uploading
            };

            MenuStatus status;

            std::string name;
            int updateTime{};
            DataBlock dataBlock;
            bool isDir{};

            std::vector<MenuNode> children;
        } root;
    };

    struct Data {
        struct DataNode {
            enum DataStatus {
                valid,
                deleting,
                uploading
            };

            DataStatus status;

            bool simpleData;

            std::string data;

            std::vector<DataBlock> multiBlockData;
        };

        std::vector<DataNode> dataNodes;
    };

    int version{};

    bool hasSuper{};
    bool hasMenu{};
    bool hasData{};

    Super super;
    Menu menu;
    Data data;


    static std::string encode();

    static SuperMenuDataBlockPtr decode(const std::string &);

    int64_t remainSize();

};


std::string SuperMenuDataBlock::encode() {
    return {};
}

SuperMenuDataBlockPtr SuperMenuDataBlock::decode(const std::string &) {
    auto res = std::make_shared<SuperMenuDataBlock>();
    res->super.remainSize = std::vector<int>(10, 1 << 20);
    return res;
}

std::string encodeDataBlocks(const std::vector<SuperMenuDataBlock::DataBlock> &blocks) {
    return serialization(blocks);
}

std::vector<SuperMenuDataBlock::DataBlock> decodeDataBlocks(const std::string &data) {
    std::vector<SuperMenuDataBlock::DataBlock> res;
    deserialization(data.data(), data.size(), res);
    return res;
}

std::string serialization(const SuperMenuDataBlock::DataBlock &rhs) {
    std::vector<int64_t> data;
    data.push_back(rhs.size);
    data.push_back(rhs.dataInWhichBlock);
    data.push_back(rhs.dataInWhichIndex);
    return serialization(data);
}

void deserialization(const char *data, int len, SuperMenuDataBlock::DataBlock &rhs) {
    std::vector<int64_t> data2;
    deserialization(data, len, data2);
    rhs.size = data2[0];
    rhs.dataInWhichBlock = data2[1];
    rhs.dataInWhichIndex = data2[2];
}


int64_t SuperMenuDataBlock::remainSize() {
    // TODO
    int64_t res = 0;
    for (const auto &item: data.dataNodes) {
        res += (int64_t) item.data.size();
    }
    return res;
}


