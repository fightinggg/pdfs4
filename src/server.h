#include "../lib/cpp-httplib/httplib.h"
#include "block/SuperMenuDataFs.h"
#include "pdfs.h"
#include "storage/MemoryStorage.h"
#include "errors.h"
#include "block/SuperBlockFs.h"
#include "utils/json.h"

namespace pdfs {
    void test() {
        // 10MB
        auto storagePtr = StoragePtr(new MemoryStorage(10, 1 << 20));
        auto fs = PdfsPtr(new SuperBlockFs(storagePtr));

//        fs->mkdir("/home");

        if (false) {
            std::string data = "hello world\n";
            std::string filename = "/home/a.txt";
            fs->createFile(filename, data.size(), data);
            auto write = fs->write(filename, 0, data.size(), stream::fromString(data));
            if (write != data.length()) {
                puts("write failed");
                return;
            }

            try {
                if (data != fs->read(filename, 0, data.size())->readAll()) {
                    puts("not eq");
                    return;
                }
            } catch (const NotFoundError &err) {
                puts(("找不到" + filename).data());
            }
        }

        {
            std::string data(3000000, 'b');
            std::string filename = "/home/b.txt";

            fs->createFile(filename, data.size(), data);
            for (int i = 0; i < data.size();) {
                auto remainLine = data.size() - i;
                auto sub = std::string(data.data(), data.data() + remainLine);
                auto write = fs->write(filename, i, remainLine, stream::fromString(sub));
                if (write == 0) {
                    puts("write failed");
                    return;
                }
                i += write;
            }

            try {
                auto newData = fs->read(filename, 0, data.size())->readAll();
                if (data != newData) {
                    printf("%d %d", data.size(), newData.size());
                    puts("not eq");
                    return;
                }
            } catch (const NotFoundError &err) {
                puts(("找不到" + filename).data());
            }
        }

    }

    void main() {
//        _test_deserialization_vector();
//        test();
//        return;
        int port = 8081;
        string staticPath = "D:\\src\\pdfs4\\webstatic";

        auto storagePtr = StoragePtr(new MemoryStorage(10, 1 << 20));
        auto fs = PdfsPtr(new SuperBlockFs(storagePtr));


        httplib::Server svr;

        auto alowCors = [](httplib::Response &res) {
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_header("Access-Control-Allow-Methods", "*");
            res.set_header("Access-Control-Allow-Headers", "*");
        };

        svr.Get("/", [](const httplib::Request &, httplib::Response &res) {
            res.set_redirect("/static");
        });

        svr.Get("/api/list", [&](const httplib::Request &, httplib::Response &res) {
            alowCors(res);
            res.body = ::toJson(fs->ls(""));

        });

        svr.set_mount_point("/static/", staticPath);

        std::cout << "http listen at port " << port << std::endl;
        svr.listen("0.0.0.0", port);
    }

}