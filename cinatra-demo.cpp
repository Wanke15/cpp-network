#include <string>
#include "cinatra.hpp"
#include "json.hpp"
#include <thread>
#include <chrono>
#include <pthread.h>

using namespace cinatra;


// 日志切面
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

class Monitor
{

public:
	std::string last_modified = "";
	std::string content = "";
	std::string uri = "http://127.0.0.1:9000/custom_ner";

	std::string get_data();

};

std::string Monitor::get_data()
{
	return this->content;
}


void ner_monitor(Monitor& m)
{
	std::shared_ptr<http_client> client = client_factory::instance().new_client();

	while (true)
	{
		try {
			auto resp = client->get(m.uri);

			auto resp_data = resp.resp_body.data();

			nlohmann::json body = nlohmann::json::parse(resp_data);

			std::string last_modified;
			if (body.contains("last_modified"))
				last_modified = body["last_modified"];

			if (m.last_modified != last_modified)
			{
				std::string new_content;
				if (body.contains("content"))
					new_content = body["content"];
				std::cout << "检测到数据发生了变化，进行更新... " << std::endl;
				m.content = new_content;
				std::cout << "更新完成，最新数据为：" + new_content << std::endl;
				//std::cout << m.last_modified << last_modified << std::endl;
				m.last_modified = last_modified;
			}
		}
		catch (...) {
			std::cout << "catch (...)" << std::endl;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10000));
	}

}

int main() {
	Monitor monitor;

	std::thread gamma_thread(ner_monitor, std::ref(monitor));

	auto client = cinatra::client_factory::instance().new_client();
	int max_thread_num = std::thread::hardware_concurrency();
	http_server server(max_thread_num);
	server.listen("0.0.0.0", "8080");

	server.set_http_handler<GET>("/ner", [&](request& req, response& res) {
		std::string content = monitor.get_data();
		res.set_status_and_content(status_type::ok, content.c_str(), req_content_type::string);
		}
	);

	server.set_http_handler<GET>("/", [](request& req, response& res) {
		res.set_status_and_content(status_type::ok, "hello world");
		}
	);

	server.set_http_handler<GET>("/jieba", [](request& req, response& res) {
		auto text = req.get_query_value("text");
		std::vector<std::string> words;
		nlohmann::json result;

		result["text"] = text;
		result["words"] = words;

		res.set_status_and_content(status_type::ok, result.dump(), req_content_type::json);

		});

	server.set_http_handler<POST>("/hello", [](request& req, response& res) {
		// 解析 post body 数据
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
