
struct SuperMenuDataBlock {
    struct Super {
        std::vector<int> perhapsUsedBytes;
    };

    struct Menu {
        struct MenuNode {
            std::string name;
            int size;
            int updateTime;
            int dataInWhichBlock;
            int dataInWhichIndex;

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


};