#ifndef SUISHA__LOOP_H
#define SUISHA__LOOP_H

#include <chrono>
#include <functional>
#include <memory>
#include <stdexcept>
#include <vector>
#include <unistd.h>
#include <sys/poll.h>

namespace suisha
{

class t_loop;

class t_timer
{
	friend class t_loop;

	t_loop* v_loop;
	std::function<void()> v_function;
	std::chrono::milliseconds v_interval;
	bool v_single;
	std::shared_ptr<t_timer> v_next;
	std::chrono::steady_clock::time_point v_time;

public:
	t_timer(t_loop* a_loop, std::function<void()>&& a_function, const std::chrono::milliseconds& a_interval, bool a_single) : v_loop(a_loop), v_function(std::move(a_function)), v_interval(a_interval), v_single(a_single)
	{
	}
	void f_stop();
};

class t_loop
{
	friend class t_timer;
	friend t_loop& f_loop();

	static thread_local t_loop* v_instance;

	size_t v_loop = 0;
	bool v_more = false;
	std::vector<struct pollfd> v_pollfds{1};
	int v_post;
	std::vector<std::function<void(bool, bool)>> v_listeners;
	size_t v_notifier = 0;
	std::shared_ptr<t_timer> v_timer;

	void f_check() const
	{
		if (this != v_instance) throw std::runtime_error("no owner thread.");
	}
	std::function<void()> f_unpost();
	size_t f_find(int a_descriptor)
	{
		f_check();
		size_t i = 1;
		while (i < v_pollfds.size() && v_pollfds[i].fd != a_descriptor) ++i;
		return i;
	}
	void f_poll(size_t a_i, bool a_read, bool a_write)
	{
		v_pollfds[a_i].events = 0;
		if (a_read) v_pollfds[a_i].events |= POLLIN;
		if (a_write) v_pollfds[a_i].events |= POLLOUT;
	}
	void f_queue(const std::shared_ptr<t_timer>& a_timer);

public:
	std::function<void()> v_wait;

	t_loop();
	~t_loop() noexcept(false);
	void f_run();
	void f_exit()
	{
		f_check();
		if (v_loop > 0) --v_loop;
	}
	void f_terminate()
	{
		f_check();
		v_loop = 0;
	}
	void f_more()
	{
		f_check();
		v_more = true;
	}
	void f_post(std::function<void()>&& a_function)
	{
		auto p = new std::function<void()>(std::move(a_function));
		write(v_post, &p, sizeof(p));
	}
	void f_poll(int a_descriptor, bool a_read, bool a_write, std::function<void(bool, bool)>&& a_listener);
	void f_poll(int a_descriptor, bool a_read, bool a_write);
	void f_unpoll(int a_descriptor);
	std::shared_ptr<t_timer> f_timer(std::function<void()>&& a_function, const std::chrono::milliseconds& a_interval, bool a_single = false)
	{
		f_check();
		auto timer = std::make_shared<t_timer>(this, std::move(a_function), a_interval, a_single);
		f_queue(timer);
		return timer;
	}
};

inline t_loop& f_loop()
{
	if (!t_loop::v_instance) throw std::runtime_error("no loop.");
	return *t_loop::v_instance;
}

}

#endif
