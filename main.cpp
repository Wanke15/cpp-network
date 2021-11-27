
#include "include/httplib.h"
#include "include/hello.h"

Hello hello;

int main() {

    httplib::Server svr;

    svr.Get("/hi", [](const httplib::Request &req, httplib::Response &res) {
    // res.set_content("Hello World Jeff!", "text/plain");

    std::string name = "";

    if (req.has_param("name")) {
        name = req.get_param_value("name");
    }

    std::string result = hello.sayHello(name);

    res.set_content(result, "text/plain");

    });

    std::cout << "start..." << std::endl;
    svr.listen("0.0.0.0", 8080);

    return 0;
}