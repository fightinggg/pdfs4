#include "../lib/cpp-httplib/httplib.h"
#include "pdfs.h"

namespace pdfs {
    void main() {
        httplib::Server svr;


        svr.Get("/", [](const httplib::Request &, httplib::Response &res) {
            res.set_redirect("/static");
        });

        svr.set_mount_point("/static/", "D:\\src\\pdfs4\\webstatic");

        svr.listen("0.0.0.0", 8080);
    }
}