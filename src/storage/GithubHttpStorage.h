#pragma once

#include "storage.h"
#include "../../lib/stream/InputStream.h"
#include "../../lib/stream/StringInputStream.h"
#include "../../lib/cpp-httplib/httplib.h"
#include "../utils/https.h"
#include "../utils/base64.h"
#include "../utils/sha.h"
#include "../utils/hex.h"

#include <cstdlib>
#include <iostream>
#include <string>
#include <curl/curl.h>

namespace pdfs {
    class GithubHttpStorage : public Storage {
        using string = std::string;


    public:
        GithubHttpStorage(const string &githubOwner, const string &githubRepo, const string &githubToken) {

        }

        stream::InputStreamPtr read(int index) override {
            if (index < 0 || index >= 1000) {
                puts("index < 0 || index >= 1000");
                exit(-1);
            }

            string githubUsername = "fightinggg";
            string githubRepoName = "pdfs-data-githubapi";
            char fileNameBuf[20];
            sprintf(fileNameBuf, "%03d.bin", index);
            string fileName = fileNameBuf;
            string githubToken = getenv("pdfsGithubToken");

            if (githubToken.empty()) {
                puts("githubToken == \"\"");
                exit(-1);
            }

            string url = "https://api.github.com/repos";
            url += "/" + githubUsername;
            url += "/" + githubRepoName;
            url += "/contents";
            url += "/" + fileName;

            map<string, string> headers;
            headers["Authorization"] = "Bearer " + githubToken;
            headers["Accept"] = "application/vnd.github.v3.raw";
            headers["X-GitHub-Api-Version"] = "2022-11-28";
            headers["User-Agent"] = "CURL";


            auto rsp = https::request(https::HttpReq{
                    "GET",
                    url,
                    headers
            });
            if (rsp.success && rsp.status == 200) {
                auto res = rsp.body;
                auto res404 = "{\"message\":\"Not Found\",\"documentation_url\":\""
                              "https://docs.github.com/rest/reference/r"
                              "epos#get-repository-content\"}";
                if (res != res404) {
                    return stream::fromString(rsp.body);
                } else {
                    throw NotFoundError("not found!");
                }
            }
            puts(rsp.body.data());
//            exit(-1);
            return stream::fromString(string(1 << 20, '\0'));

        }

        void write(int index, std::string s) override {
            if (index < 0 || index >= 1000) {
                puts("index < 0 || index >= 1000");
                exit(-1);
            }

            string githubUsername = "fightinggg";
            string githubRepoName = "pdfs-data-githubapi";
            char fileNameBuf[20];
            sprintf(fileNameBuf, "%03d.bin", index);
            string fileName = fileNameBuf;
            string githubToken = getenv("pdfsGithubToken");

            if (githubToken.empty()) {
                puts("githubToken == \"\"");
                exit(-1);
            }

            string url = "https://api.github.com/repos";
//            string url = "http://localhost:9999";
            url += "/" + githubUsername;
            url += "/" + githubRepoName;
            url += "/contents";
            url += "/" + fileName;

            map<string, string> headers;
            headers["Authorization"] = "Bearer " + githubToken;
            headers["Accept"] = "application/vnd.github+json";
            headers["X-GitHub-Api-Version"] = "2022-11-28";
            headers["Content-Type"] = "application/json";
            headers["User-Agent"] = "CURL";

            map<string, string> params;
            params["message"] = "commit by pdfs v4.0";
            params["content"] = base64::encode(s.data(), s.length());
            /**
            byte[] head = String.format("blob %d\0", old.getFileSize()).getBytes();
            byte[] sum = new byte[(int) (old.getFileSize() + head.length)];
            System.arraycopy(head, 0, sum, 0, head.length);
            System.arraycopy(old, 0, sum, head.length, (int) old.getFileSize());
            params.put("sha", Hex.encode(SHA.encode(sum)));
             */
            /**
             * hex(sha('blob %d\0%s'))
             */
            try {
                string old = read(index)->readAll();
//            params["sha"] = hex::hex(sha::sha1("blob " + std::to_string(old.length()) + '\0' + old));
                params["sha"] = sha::sha1("blob " + std::to_string(old.length()) + '\0' + old);
            } catch (const NotFoundError &ignore) {
                int a = 1; // for debug
            }


            vector<string> jsonList;
            for (const auto &item: params) {
                string row;

                row += '"';
                row += item.first;
                row += '"';

                row += ':';

                row += '"';
                row += item.second;
                row += '"';

                jsonList.push_back(row);
            }
            string body = "{" + joinString(jsonList, ',') + "}";


            auto rsp = https::request(https::HttpReq{
                    "PUT",
                    url,
                    headers,
                    body
            });
            if (rsp.success && rsp.status == 200) {
                auto res = rsp.body;
                auto res404 = "{\"message\":\"Not Found\",\"documentation_url\":\""
                              "https://docs.github.com/rest/reference/r"
                              "epos#get-repository-content\"}";
                if (res != res404) {
                    return;
                } else {
                    puts("404");
                    exit(-1);
                }
            }
            puts(rsp.body.data());
            exit(-1);
        }

        int blockNumbers() override {
            return 10;
        }

        int everyBlockBytes() override {
            return 1 << 20;
        }
    };

    namespace test {
        void testGithubHttpStorage() {
            Storage *storage = new GithubHttpStorage("", "", "");
            try {
//                storage->read(1);
            } catch (NotFoundError e) {

            }
            delete storage;

        }
    }
};