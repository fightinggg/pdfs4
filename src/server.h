//#define CPPHTTPLIB_OPENSSL_SUPPORT

#include "../lib/cpp-httplib/httplib.h"
#include "block/SuperMenuDataFs.h"
#include "pdfs.h"
#include "storage/MemoryStorage.h"
#include "errors.h"
#include "block/SuperBlockFs.h"
#include "utils/json.h"
#include "utils/http.h"
//#include "storage/GithubHttpStorage.h"
#include "storage/FileSystemStorage.h"

namespace pdfs {
    void addFile(PdfsPtr fs, string filename, string data) {
//        std::string data = "hello world\n";
//        std::string filename = "/home/a.txt";
        fs->createFile(filename, data.size());
        auto write = fs->write(filename, 0, data.size(), stream::fromString(data));
        if (write != data.length()) {
            puts("write failed");
            return;
        }
    }

    void test2() {
        // 10MB
        auto storagePtr = StoragePtr(new FileSystemStorage(10, 1 << 20));
        auto fs = PdfsPtr(new SuperBlockFs(storagePtr));

//        fs->mkdir("/home");

        if (false) {
            std::string data = "hello world\n";
            std::string filename = "/home/a.txt";
            fs->createFile(filename, data.size());
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

            fs->createFile(filename, data.size());
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
//        test::testGithubHttpStorage();
//        _test_deserialization_vector();
//        test();
//        return;
        int port = 8081;
        string staticPath = "/app/static";

//        auto storagePtr = StoragePtr(new MemoryStorage(10, 1 << 20));
//        auto storagePtr = StoragePtr(new GithubHttpStorage("", "", ""));
        auto storagePtr = StoragePtr(new FileSystemStorage(10, 1 << 20));
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

        auto parseRequests = [](const string &url) {
            auto split = splitString(url, '?');
            map<string, vector<string>> headers;
            if (split.size() == 2) {
                auto requestParamPairs = splitString(split[1], '&');
                for (const auto &requestParamPair: requestParamPairs) {
                    auto requestParamPairSplit = splitString(requestParamPair, '=');
                    if (requestParamPairSplit.size() == 2) {
                        headers[requestParamPairSplit[0]].push_back(url_decode(requestParamPairSplit[1]));
                    }
                }
            }
            return headers;
        };


        svr.set_mount_point("/", staticPath);
        svr.Get("/", [&](const httplib::Request &, httplib::Response &res) {
            res.set_redirect("/index.html");
        });

        svr.Get("/api/list", [&](const httplib::Request &req, httplib::Response &res) {
            alowCors(res);
            auto reqParams = parseRequests(req.target);
            auto path = url_decode(reqParams["path"][0]);
            res.body = ::toJson(fs->ls(path));
        });


        svr.Get("/api/file", [&](const httplib::Request &req, httplib::Response &res) {
            alowCors(res);
            auto reqParams = parseRequests(req.target);
            auto path = url_decode(reqParams["path"][0]);
            int size = 1024;
            if (!reqParams["size"].empty()) {
                size = std::stoll(reqParams["size"][0]);
            }
            res.body = fs->read(path, 0, size)->readAll();
            printf("bodylen=%d\n", res.body.size());
            fflush(stdout);
        });

        // create file
        svr.Options("/api/createFile", [&](const httplib::Request &req, httplib::Response &res) {
            alowCors(res);
            res.status = 200;
        });
        svr.Post("/api/createFile", [&](const httplib::Request &req, httplib::Response &res) {
            alowCors(res);
            auto reqParams = parseRequests(req.target);
            auto status = fs->createFile(reqParams["path"][0], std::stoll(reqParams["size"][0]));
            res.status = status ? 200 : 500;
        });

        // write file
        svr.Options("/api/writeFile", [&](const httplib::Request &req, httplib::Response &res) {
            alowCors(res);
            res.status = 200;
        });
        svr.Post("/api/writeFile", [&](const httplib::Request &req, httplib::Response &res) {
            alowCors(res);
            auto reqParams = parseRequests(req.target);
            auto start = std::stoll(reqParams["start"][0]);
            auto size = std::stoll(reqParams["size"][0]);
            auto status = fs->write(reqParams["path"][0], start, size, stream::fromString(req.body));
            res.status = 200;
            res.body = std::to_string(status);
        });


        std::cout << "http listen at port " << port << std::endl;
        svr.listen("0.0.0.0", port);
    }

}