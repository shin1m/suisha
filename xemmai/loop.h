#ifndef XEMMAIX__SUISHA__LOOP_H
#define XEMMAIX__SUISHA__LOOP_H

#include "suisha.h"
#include <xemmai/thread.h>

namespace xemmaix::suisha
{

struct t_wait
{
	std::function<void()> v_wait;

	t_wait(std::function<void()>&& a_wait) : v_wait(std::move(a_wait))
	{
	}
};

struct t_timer
{
	std::weak_ptr<::suisha::t_timer> v_timer;

	t_timer(t_loop& a_loop, const t_pvalue& a_callable, size_t a_interval, bool a_single) : v_timer(a_loop.f_timer([this, callable = t_rvalue(a_callable)]
	{
		callable();
	}, a_interval, a_single))
	{
	}
	void f_stop()
	{
		if (auto p = v_timer.lock()) p->f_stop();
	}
};

}

namespace xemmai
{

template<>
struct t_type_of<xemmaix::suisha::t_wait> : t_uninstantiatable<t_underivable<t_holds<xemmaix::suisha::t_wait>>>
{
	typedef xemmaix::suisha::t_extension t_extension;

	static void f_define(t_extension* a_extension);

	using t_base::t_base;
	static size_t f_do_call(t_object* a_this, t_pvalue* a_stack, size_t a_n);
};

template<>
struct t_type_of<xemmaix::suisha::t_timer> : t_uninstantiatable<t_underivable<t_holds<xemmaix::suisha::t_timer>>>
{
	typedef xemmaix::suisha::t_extension t_extension;

	static void f_define(t_extension* a_extension);

	using t_base::t_base;
};

template<>
struct t_type_of<suisha::t_loop> : t_uninstantiatable<t_underivable<t_bears<suisha::t_loop>>>
{
	template<typename T>
	struct t_as;
	template<typename T0>
	struct t_as<T0*>
	{
		template<typename T1>
		static T0* f_call(T1&& a_object)
		{
			auto p = f_object(std::forward<T1>(a_object))->template f_as<T0*>();
			if (!p) f_throw(L"already destroyed."sv);
			return p;
		}
	};
	template<typename T0>
	struct t_as<T0&>
	{
		template<typename T1>
		static T0& f_call(T1&& a_object)
		{
			return *t_as<T0*>::f_call(std::forward<T1>(a_object));
		}
	};
	typedef xemmaix::suisha::t_extension t_extension;

	static void f_post(suisha::t_loop& a_self, const t_pvalue& a_callable)
	{
		t_thread::f_cache_release();
		a_self.f_post([callable = t_rvalue(a_callable)]
		{
			t_thread::f_cache_acquire();
			callable();
		});
	}
	static void f_poll(suisha::t_loop& a_self, int a_descriptor, bool a_read, bool a_write, const t_pvalue& a_callable)
	{
		a_self.f_poll(a_descriptor, a_read, a_write, [callable = t_rvalue(a_callable)](bool a_readable, bool a_writable)
		{
			callable(f_global()->f_as(a_readable), f_global()->f_as(a_writable));
		});
	}
	static t_pvalue f_timer(t_extension* a_extension, suisha::t_loop& a_self, const t_pvalue& a_callable, size_t a_interval, bool a_single)
	{
		return xemmai::f_new<xemmaix::suisha::t_timer>(a_extension, false, a_self, a_callable, a_interval, a_single);
	}
	static t_pvalue f_timer(t_extension* a_extension, suisha::t_loop& a_self, const t_pvalue& a_callable, size_t a_interval)
	{
		return xemmai::f_new<xemmaix::suisha::t_timer>(a_extension, false, a_self, a_callable, a_interval, false);
	}
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
};

}

#endif
