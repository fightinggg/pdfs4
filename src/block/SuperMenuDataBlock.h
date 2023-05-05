#include <memory>
#include <vector>

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
    return {};
}

std::vector<SuperMenuDataBlock::DataBlock> decodeDataBlocks(const std::string &data) {
    return {};
}


int64_t SuperMenuDataBlock::remainSize() {
    // TODO
    int64_t res = 0;
    for (const auto &item: data.dataNodes) {
        res += (int64_t) item.data.size();
    }
    return res;
}


