#ifndef XEMMAIX__SUISHA__LOOP_H
#define XEMMAIX__SUISHA__LOOP_H

#include "suisha.h"
#include <xemmai/thread.h>

namespace xemmaix::suisha
{

struct t_wait : t_owned
{
	std::function<void()> v_wait;

	t_wait(std::function<void()>&& a_wait) : v_wait(std::move(a_wait))
	{
	}
};

struct t_timer : t_owned
{
	std::weak_ptr<::suisha::t_timer> v_timer;

	t_timer(::suisha::t_loop& a_loop, const t_pvalue& a_callable, size_t a_interval, bool a_single) : v_timer(a_loop.f_timer([this, callable = t_rvalue(a_callable)]
	{
		callable();
	}, a_interval, a_single))
	{
	}
	void f_stop()
	{
		f_owned_or_throw();
		if (auto p = v_timer.lock()) p->f_stop();
	}
};

struct t_loop : t_sharable
{
	::suisha::t_loop* v_loop;

	t_loop(::suisha::t_loop& a_loop) : v_loop(&a_loop)
	{
	}
	void f_check()
	{
		f_owned_or_throw();
		if (!v_loop) f_throw(L"already destroyed."sv);
	}
	void f_run()
	{
		f_check();
		v_loop->f_run();
	}
	void f_exit()
	{
		f_check();
		v_loop->f_exit();
	}
	void f_terminate()
	{
		f_check();
		v_loop->f_terminate();
	}
	void f_more()
	{
		f_check();
		v_loop->f_more();
	}
	void f_post(const t_pvalue& a_callable)
	{
		std::lock_guard lock(v_mutex);
		if (!v_loop) f_throw(L"already destroyed."sv);
		v_loop->f_post([callable = t_rvalue(a_callable)]
		{
			callable();
		});
	}
	void f_poll(int a_descriptor, bool a_read, bool a_write, const t_pvalue& a_callable)
	{
		f_check();
		v_loop->f_poll(a_descriptor, a_read, a_write, [callable = t_rvalue(a_callable)](bool a_readable, bool a_writable)
		{
			callable(f_global()->f_as(a_readable), f_global()->f_as(a_writable));
		});
	}
	void f_unpoll(int a_descriptor)
	{
		f_check();
		v_loop->f_unpoll(a_descriptor);
	}
	t_pvalue f_timer(t_library* a_library, const t_pvalue& a_callable, size_t a_interval, bool a_single)
	{
		f_check();
		return xemmai::f_new<t_timer>(a_library, *v_loop, a_callable, a_interval, a_single);
	}
	t_pvalue f_timer(t_library* a_library, const t_pvalue& a_callable, size_t a_interval)
	{
		return f_timer(a_library, a_callable, a_interval, false);
	}
};

}

namespace xemmai
{

template<>
struct t_type_of<xemmaix::suisha::t_wait> : t_uninstantiatable<t_holds<xemmaix::suisha::t_wait>>
{
	using t_library = xemmaix::suisha::t_library;

	static void f_define(t_library* a_library);

	using t_base::t_base;
	static size_t f_do_call(t_object* a_this, t_pvalue* a_stack, size_t a_n);
};

template<>
struct t_type_of<xemmaix::suisha::t_timer> : t_uninstantiatable<t_holds<xemmaix::suisha::t_timer>>
{
	using t_library = xemmaix::suisha::t_library;

	static void f_define(t_library* a_library);

	using t_base::t_base;
};

template<>
struct t_type_of<xemmaix::suisha::t_loop> : t_uninstantiatable<t_bears<xemmaix::suisha::t_loop>>
{
	using t_library = xemmaix::suisha::t_library;

	static void f_define(t_library* a_library);

	using t_base::t_base;
};

}

#endif
