#include <suisha/loop.h>
#include <system_error>

namespace suisha
{

void t_timer::f_stop()
{
	if (!v_loop) return;
	v_loop->f_check();
	auto p = &v_loop->v_timer;
	while (p->get() != this) p = &(*p)->v_next;
	*p = std::move(v_next);
	v_loop = nullptr;
}

thread_local t_loop* t_loop::v_instance;

std::function<void()> t_loop::f_unpost()
{
	std::function<void()>* p;
	size_t m = 0;
	do m += read(v_pollfds[0].fd, reinterpret_cast<char*>(&p) + m, sizeof(p) - m); while (m < sizeof(p));
	auto q = std::move(*p);
	delete p;
	return q;
}

void t_loop::f_queue(const std::shared_ptr<t_timer>& a_timer)
{
	auto time = &a_timer->v_time;
	gettimeofday(time, NULL);
	time->tv_usec += a_timer->v_interval * 1000;
	time->tv_sec += time->tv_usec / 1000000;
	time->tv_usec %= 1000000;
	auto p = &v_timer;
	while (*p && timercmp(&(*p)->v_time, time, <=)) p = &(*p)->v_next;
	a_timer->v_next = std::move(*p);
	*p = a_timer;
}

t_loop::t_loop()
{
	if (v_instance) throw std::runtime_error("loop already exists.");
	int fds[2];
	pipe(fds);
	v_pollfds[0].fd = fds[0];
	v_pollfds[0].events = POLLIN;
	v_post = fds[1];
	v_wait = [this]
	{
		f_check();
		while (true) {
			int timeout = -1;
			if (v_more) {
				v_more = false;
				timeout = 0;
			} else if (v_timer) {
				timeval tv;
				gettimeofday(&tv, NULL);
				timersub(&v_timer->v_time, &tv, &tv);
				timeout = std::max<int>(tv.tv_sec * 1000 + (tv.tv_usec + 999) / 1000, 0);
			}
			if (poll(&v_pollfds[0], v_pollfds.size(), timeout) != -1) break;
			if (errno != EINTR) throw std::system_error(errno, std::generic_category());
		}
	};
	v_instance = this;
}

t_loop::~t_loop()
{
	v_instance = nullptr;
	while (true) {
		while (poll(&v_pollfds[0], 1, 0) == -1) if (errno != EINTR) throw std::system_error(errno, std::generic_category());
		if (v_pollfds[0].revents != POLLIN) break;
		f_unpost();
	}
	close(v_pollfds[0].fd);
	close(v_post);
	while (v_timer) {
		v_timer->v_loop = nullptr;
		v_timer = std::move(v_timer->v_next);
	}
}

void t_loop::f_run()
{
	f_check();
	size_t current = ++v_loop;
	while (true) {
		v_wait();
		if (v_loop < current) return;
		while (true) {
			if (v_pollfds[0].revents == POLLIN) {
				f_unpost()();
				if (v_loop < current) return;
				v_more = true;
			}
			while (v_notifier < v_listeners.size()) {
				short revents = v_pollfds[v_notifier + 1].revents;
				v_listeners[v_notifier]((revents & POLLIN) != 0, (revents & POLLOUT) != 0);
				if (v_loop < current) break;
				++v_notifier;
			}
			v_notifier = 0;
			if (v_loop < current) return;
			while (v_timer) {
				timeval tv;
				gettimeofday(&tv, NULL);
				if (timercmp(&v_timer->v_time, &tv, >)) break;
				auto p = std::move(v_timer);
				v_timer = std::move(p->v_next);
				if (p->v_single)
					p->v_loop = nullptr;
				else
					f_queue(p);
				p->v_function();
				if (v_loop < current) return;
			}
			if (!v_more) break;
			v_more = false;
			while (poll(v_pollfds.data(), v_pollfds.size(), 0) == -1) if (errno != EINTR) throw std::system_error(errno, std::generic_category());
			if (v_loop < current) return;
		}
	}
}

void t_loop::f_poll(int a_descriptor, bool a_read, bool a_write, std::function<void(bool, bool)>&& a_listener)
{
	f_check();
	size_t i = 1;
	while (i < v_pollfds.size() && v_pollfds[i].fd != a_descriptor) ++i;
	if (i < v_pollfds.size()) {
		v_listeners[i - 1] = std::move(a_listener);
	} else {
		struct pollfd fd;
		fd.fd = a_descriptor;
		v_pollfds.push_back(fd);
		v_listeners.emplace_back(std::move(a_listener));
	}
	v_pollfds[i].events = v_pollfds[i].revents = 0;
	if (a_read) v_pollfds[i].events |= POLLIN;
	if (a_write) v_pollfds[i].events |= POLLOUT;
}

void t_loop::f_unpoll(int a_descriptor)
{
	f_check();
	size_t i = 1;
	while (i < v_pollfds.size() && v_pollfds[i].fd != a_descriptor) ++i;
	if (i >= v_pollfds.size()) return;
	v_pollfds.erase(v_pollfds.begin() + i);
	--i;
	v_listeners.erase(v_listeners.begin() + i);
	if (v_notifier > i) --v_notifier;
}

}
