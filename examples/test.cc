#include <deque>
#include <iostream>
#include <map>
#include <suisha/loop.h>

using namespace suisha;

int main(int argc, char* argv[])
{
	t_loop loop;
	loop.v_wait = [wait = std::move(loop.v_wait)]
	{
		std::cout << "before wait" << std::endl;
		wait();
		std::cout << "after wait" << std::endl;
	};
	std::deque<std::shared_ptr<t_timer>> timers;
	std::map<std::string, std::function<void()>> commands = {
		{"quit", [&]
		{
			loop.f_exit();
		}},
		{"echo", []
		{
			std::string value;
			std::cin >> value;
			std::cout << value << std::endl;
		}},
		{"post", [&]
		{
			std::string value;
			std::cin >> value;
			loop.f_post([value]
			{
				std::cout << "posted: " << value << std::endl;
			});
		}},
		{"in", [&]
		{
			size_t interval;
			std::string value;
			std::cin >> interval >> value;
			loop.f_timer([value]
			{
				std::cout << "single: " << value << std::endl;
			}, interval, true);
		}},
		{"every", [&]
		{
			size_t interval;
			std::string value;
			std::cin >> interval >> value;
			timers.push_back(loop.f_timer([value]
			{
				std::cout << "repeat: " << value << std::endl;
			}, interval));
		}},
		{"stop", [&]
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
				std::cerr << "unknown command: " << command << std::endl;
			else
				i->second();
		});
		loop.f_run();
		return EXIT_SUCCESS;
	} catch (std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}
