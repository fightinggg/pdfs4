#include "../lib/cpp-httplib/httplib.h"
#include "block/SuperMenuDataFs.h"
#include "pdfs.h"
#include "storage/MemoryStorage.h"
#include "errors.h"
#include "block/SuperBlockFs.h"
#include "utils/json.h"
#include "utils/http.h"

namespace pdfs {
    void addFile(PdfsPtr fs, string filename, string data) {
//        std::string data = "hello world\n";
//        std::string filename = "/home/a.txt";
        fs->createFile(filename, data.size(), data);
        auto write = fs->write(filename, 0, data.size(), stream::fromString(data));
        if (write != data.length()) {
            puts("write failed");
            return;
        }
    }

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

        addFile(fs, "/a.txt", "123");
        addFile(fs, "/b.txt", "456");
        addFile(fs, "/c/c.txt", "789");
        addFile(fs, "/c/哥哥.txt", "我是小黑子");


        httplib::Server svr;

        auto alowCors = [](httplib::Response &res) {
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_header("Access-Control-Allow-Methods", "*");
            res.set_header("Access-Control-Allow-Headers", "*");
        };

        svr.Get("/", [](const httplib::Request &, httplib::Response &res) {
            res.set_redirect("/static");
        });

        svr.Get("/api/list", [&](const httplib::Request &req, httplib::Response &res) {
            alowCors(res);
            auto split = splitString(req.target, '?');
            if (split.size() == 2) {
                split = splitString(split[1], '&');
                if (split.size() == 1) {
                    split = splitString(split[0], '=');
                    if (split.size() == 2 && split[0] == "path") {
                        res.body = ::toJson(fs->ls(split[1]));
                        return;
                    }
                }
            }
            res.body = ::toJson(fs->ls("/"));

        });


        svr.Get("/api/file", [&](const httplib::Request &req, httplib::Response &res) {
            alowCors(res);
            auto split = splitString(req.target, '?');
            if (split.size() == 2) {
                split = splitString(split[1], '&');
                if (split.size() == 1) {
                    split = splitString(split[0], '=');
                    if (split.size() == 2 && split[0] == "path") {
                        auto path = url_decode(split[1]);
                        res.body = fs->read(path,0,1024)->readAll();
                        return;
                    }
                }
            }
            res.body = "404 NOT FOUND!!!";

        });

        svr.set_mount_point("/static/", staticPath);

        std::cout << "http listen at port " << port << std::endl;
        svr.listen("0.0.0.0", port);
    }

}