#pragma once
namespace pdfs {
    class Storage {
    public:
        virtual stream::InputStreamPtr read(int index) = 0;

        virtual void write(int index, std::string s) = 0;

        virtual int blockNumbers() = 0;

        virtual int everyBlockBytes() = 0;
    };

    using StoragePtr = std::shared_ptr<Storage>;
}