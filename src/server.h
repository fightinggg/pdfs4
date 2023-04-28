#include "../lib/cpp-httplib/httplib.h"
#include "block/SuperMenuDataFs.h"
#include "pdfs.h"
#include "storage/MemoryStorage.h"

namespace pdfs {
    void test() {
        // 10MB
        auto storagePtr = StoragePtr(new MemoryStorage(10, 1 << 20));
        auto fs = PdfsPtr(new SuperMenuDataFs(storagePtr));
        std::string data = "你好";
        if (!fs->write("a.txt", 0, data.size(), stream::fromString(data))) {
            puts("write failed");
            return;
        }

        data = fs->read("a.txt", 0, data.size())->readAll();

        printf("%s", data.data());
    }

    void main() {
        test();
        return;
        httplib::Server svr;


        svr.Get("/", [](const httplib::Request &, httplib::Response &res) {
            res.set_redirect("/static");
        });

        svr.set_mount_point("/static/", "D:\\src\\pdfs4\\webstatic");

        std::cout << "http listen at port 8080" << std::endl;
        svr.listen("0.0.0.0", 8080);
    }

}