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

        std::string data = "你好";
        std::string filename = "/home/a.txt";
        if (!fs->write("/home/a.txt", 0, data.size(), stream::fromString(data))) {
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

    void main() {
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