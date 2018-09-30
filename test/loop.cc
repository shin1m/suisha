#include <suisha/loop.h>
#include <numeric>
#include <thread>
#include <cassert>

using namespace std::literals;

template<typename T_container>
std::string f_join(const T_container& a_container)
{
	return std::accumulate(a_container.begin(), a_container.end(), R"(
)"s, [](auto a_value, auto a_x)
	{
		return a_value + a_x + R"(
)";
	});
}

template<typename T_container>
void f_log(suisha::t_loop& a_loop, T_container& a_container)
{
	a_container.clear();
	a_loop.v_wait = [&, wait = std::move(a_loop.v_wait)]
	{
		a_container.emplace_back("wait"sv);
		wait();
	};
}

int main(int argc, char* argv[])
{
	try {
		suisha::f_loop();
		assert(false);
	} catch (std::runtime_error& e) {
		assert(e.what() == "no loop."sv);
	}
	{
		suisha::t_loop loop;
		suisha::f_loop();
	}
	try {
		suisha::f_loop();
		assert(false);
	} catch (std::runtime_error& e) {
		assert(e.what() == "no loop."sv);
	}
	std::vector<std::string> log;
	{
		suisha::t_loop loop;
		f_log(loop, log);
		loop.f_post([&]
		{
			log.emplace_back("post"sv);
			loop.f_exit();
		});
		loop.f_run();
		assert(f_join(log) == R"(
wait
post
)"sv);
	}
	{
		suisha::t_loop loop;
		f_log(loop, log);
		std::thread t([&]
		{
			loop.f_post([&]
			{
				log.emplace_back("post"sv);
				loop.f_exit();
			});
		});
		loop.f_run();
		t.join();
		assert(f_join(log) == R"(
wait
post
)"sv);
	}
	{
		suisha::t_loop loop;
		f_log(loop, log);
		int fds[2];
		pipe(fds);
		char buffer[1024];
		loop.f_poll(fds[0], true, false, [&](bool a_readable, bool a_writable)
		{
			if (!a_readable) return;
			ssize_t n = read(fds[0], buffer, sizeof(buffer));
			log.emplace_back(buffer);
			close(fds[0]);
			loop.f_exit();
		});
		std::thread t([&]
		{
			write(fds[1], "poll", 5);
			close(fds[1]);
		});
		loop.f_run();
		t.join();
		assert(f_join(log) == R"(
wait
poll
)"sv);
	}
	{
		suisha::t_loop loop;
		f_log(loop, log);
		int fds[2];
		pipe(fds);
		char buffer[1024];
		loop.f_poll(fds[1], false, true, [&](bool a_readable, bool a_writable)
		{
			if (!a_writable) return;
			write(fds[1], "poll", 5);
			close(fds[1]);
			loop.f_exit();
		});
		std::thread t([&]
		{
			ssize_t n = read(fds[0], buffer, sizeof(buffer));
			log.emplace_back(buffer);
			close(fds[0]);
		});
		loop.f_run();
		t.join();
		assert(f_join(log) == R"(
wait
poll
)"sv);
	}
	{
		suisha::t_loop loop;
		f_log(loop, log);
		loop.f_timer([&]
		{
			log.emplace_back("timer"sv);
			loop.f_exit();
		}, 100, true);
		loop.f_run();
		assert(f_join(log) == R"(
wait
timer
)"sv);
	}
	{
		suisha::t_loop loop;
		f_log(loop, log);
		size_t n = 0;
		loop.f_timer([&]
		{
			log.emplace_back("timer"sv);
			if (++n >= 3) loop.f_exit();
		}, 100);
		loop.f_run();
		assert(f_join(log) == R"(
wait
timer
wait
timer
wait
timer
)"sv);
	}
	return 0;
}
