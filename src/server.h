//#include "../lib/cpp-httplib/httplib.h"
#include "block/SuperMenuDataFs.h"
#include "pdfs.h"
#include "storage/MemoryStorage.h"
#include "errors.h"

namespace pdfs {
    void test() {
        // 10MB
        auto storagePtr = StoragePtr(new MemoryStorage(10, 1 << 20));
        auto fs = PdfsPtr(new SuperMenuDataFs(storagePtr));

//        fs->mkdir("/home");

        {
            std::string data = "你好\n";
            std::string filename = "/home/a.txt";
            auto write = fs->write(filename, 0, data.size(), stream::fromString(data));
            if (write != data.length()) {
                puts("write failed");
                return;
            }

            try {
                data = fs->read(filename, 0, data.size())->readAll();
                printf("%s", data.data());
            } catch (const NotFoundError &err) {
                puts(("找不到" + filename).data());
            }
        }

        {
            std::string data(3000000, 'b');
            std::string filename = "/home/b.txt";

            for (int i = 0; i < data.size(); ) {
                auto remainLine = data.size() - i;
                auto sub = std::string(data.data(), data.data() + remainLine);
                auto write = fs->write(filename, i, remainLine, stream::fromString(sub));
                if (write == 0) {
                    puts("write failed");
                    return;
                }
                i+=write;
            }

            try {
                data = fs->read(filename, 0, data.size())->readAll();
                printf("%s", data.data());
            } catch (const NotFoundError &err) {
                puts(("找不到" + filename).data());
            }
        }

    }

    void main() {
        _test_deserialization_vector();
        test();
//        return;
//        httplib::Server svr;
//
//
//        svr.Get("/", [](const httplib::Request &, httplib::Response &res) {
//            res.set_redirect("/static");
//        });
//
//        svr.set_mount_point("/static/", "D:\\src\\pdfs4\\webstatic");
//
//        std::cout << "http listen at port 8080" << std::endl;
//        svr.listen("0.0.0.0", 8080);
    }

}