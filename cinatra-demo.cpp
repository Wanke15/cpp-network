#include <string>
#include "cinatra.hpp"
#include "json.hpp"
using namespace cinatra;

// log aspect
struct log_t
{
	bool before(request& req, response& res) {
		std::cout << "before log" << std::endl;
		return true;
	}

	bool after(request& req, response& res) {
		std::cout << "after log" << std::endl;
		return true;
	}
};

int main() {
	int max_thread_num = std::thread::hardware_concurrency();
	http_server server(max_thread_num);
	server.listen("0.0.0.0", "8080");
	server.set_http_handler<GET>("/", [](request& req, response& res) {
		res.set_status_and_content(status_type::ok, "hello world");
		});

	server.set_http_handler<POST>("/hello", [](request& req, response& res) {
		// parse post body data
		nlohmann::json body = nlohmann::json::parse(req.body());

		std::string name;
		if (body.contains("name"))
			name = body["name"];
		else
			name = "nobody";
		int age;
		if (body.contains("age"))
			age = body["age"];
		else
			age = -1;

		nlohmann::json result;

		nlohmann::json data;
		data["name"] = name;
		data["age"] = age;

		result["data"] = data;
		result["msg"] = "success";
		result["code"] = 200;

		res.set_status_and_content(status_type::ok, result.dump(), req_content_type::json);
		}, log_t{});

	server.run();
	return 0;
}
