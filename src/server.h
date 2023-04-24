#include "../lib/cpp-httplib/httplib.h"
#include "block/SuperMenuDataBlock.h"
#include "pdfs.h"

namespace pdfs {
    void main() {
        httplib::Server svr;


        svr.Get("/", [](const httplib::Request &, httplib::Response &res) {
            res.set_redirect("/static");
        });

        svr.set_mount_point("/static/", "D:\\src\\pdfs4\\webstatic");

        std::cout << "http listen at port 8080" << std::endl;
        svr.listen("0.0.0.0", 8080);
    }
}