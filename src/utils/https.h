#include <curl/curl.h>
#include <string>
#include <map>
#include <iostream>

namespace https {

    struct HttpReq {
        std::string method;
        std::string url;
        std::map<std::string, std::string> header;
        std::string body;
    };

    struct HttpRsp {
        bool success;
        int status;
        std::map<std::string, std::string> header;
        std::string body;
    };

    size_t WriteCallback(char *contents, size_t size, size_t nmemb, void *userp) {
        ((std::string *) userp)->append((char *) contents, size * nmemb);
        return size * nmemb;
    }


    HttpRsp request(HttpReq req) {
        CURL *curl;
        CURLcode res;
        std::string readBuffer;

        curl = curl_easy_init();
        if (curl) {

            if (!req.header.empty()) {
                struct curl_slist *httpHeaders = nullptr;

                for (const auto &item: req.header) {
                    auto header = item.first + ": " + item.second;
                    httpHeaders = curl_slist_append(httpHeaders, header.data());
                }
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, httpHeaders);
            }

            /* First set the URL that is about to receive our POST. This URL can
               just as well be a https:// URL if that is what should receive the
               data. */
            curl_easy_setopt(curl, CURLOPT_URL, req.url.data());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

            // SUPPORT FOR PUT
            if (req.method != "GET") {
                curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, req.body.data());
            }

            /* Perform the request, res will get the return code */
            res = curl_easy_perform(curl);

            /* always cleanup */
            curl_easy_cleanup(curl);
        }
        return HttpRsp{
                true,
                200,
                {},
                readBuffer,
        };
    }


    void test() {

    }
}