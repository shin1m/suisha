#include <deque>
#include <iostream>
#include <map>
#include <suisha/loop.h>

using namespace std::literals;
using namespace suisha;

int main(int argc, char* argv[])
{
	t_loop loop;
	loop.v_wait = [wait = std::move(loop.v_wait)]
	{
		std::cout << "before wait"sv << std::endl;
		wait();
		std::cout << "after wait"sv << std::endl;
	};
	std::deque<std::shared_ptr<t_timer>> timers;
	std::map<std::string, std::function<void()>> commands = {
		{"quit"s, [&]
		{
			loop.f_exit();
		}},
		{"echo"s, []
		{
			std::string value;
			std::cin >> value;
			std::cout << value << std::endl;
		}},
		{"post"s, [&]
		{
			std::string value;
			std::cin >> value;
			loop.f_post([value]
			{
				std::cout << "posted: "sv << value << std::endl;
			});
		}},
		{"in"s, [&]
		{
			size_t interval;
			std::string value;
			std::cin >> interval >> value;
			loop.f_timer([value]
			{
				std::cout << "single: "sv << value << std::endl;
			}, std::chrono::milliseconds(interval), true);
		}},
		{"every"s, [&]
		{
			size_t interval;
			std::string value;
			std::cin >> interval >> value;
			timers.push_back(loop.f_timer([value]
			{
				std::cout << "repeat: "sv << value << std::endl;
			}, std::chrono::milliseconds(interval)));
		}},
		{"stop"s, [&]
		{
			timers.front()->f_stop();
			timers.pop_front();
		}}
	};
	try {
		loop.f_poll(0, true, false, [&](bool a_readable, bool a_writable)
		{
			if (!a_readable) return;
			std::string command;
			std::cin >> command;
			auto i = commands.find(command);
			if (i == commands.end())
				std::cerr << "unknown command: "sv << command << std::endl;
			else
				i->second();
		});
		loop.f_run();
		return EXIT_SUCCESS;
	} catch (std::exception& e) {
		std::cerr << "Error: "sv << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}
