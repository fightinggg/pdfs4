#include <memory>
#include <vector>

#pragma once

struct SuperMenuDataBlock;

using SuperMenuDataBlockPtr = std::shared_ptr<SuperMenuDataBlock>;


struct SuperMenuDataBlock {
    struct Super {
        std::vector<int> perhapsUsedBytes;
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
            int size;
            int updateTime;
            int dataInWhichBlock;
            int dataInWhichIndex;
            bool isDir;

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

            struct MultiBlockData {
                struct MultiBlockPoint {
                    int dataInWhichBlock;
                    int dataInWhichIndex;
                };
                std::vector<MultiBlockPoint> datas;
            };
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

};


std::string SuperMenuDataBlock::encode() {
    return {};
}

SuperMenuDataBlockPtr SuperMenuDataBlock::decode(const std::string &) {
    return std::make_shared<SuperMenuDataBlock>();
}


