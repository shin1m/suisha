#ifndef XEMMAIX__SUISHA__LOOP_H
#define XEMMAIX__SUISHA__LOOP_H

#include "suisha.h"
#include <xemmai/thread.h>

namespace xemmaix::suisha
{

struct t_wait
{
	std::function<void()> v_wait;
};

struct t_timer
{
	t_slot v_callable;
	std::shared_ptr<::suisha::t_timer> v_timer;

	t_timer(t_loop& a_loop, t_scoped&& a_callable, size_t a_interval, bool a_single) : v_callable(std::move(a_callable)), v_timer(a_loop.f_timer([this]
	{
		v_callable();
	}, a_interval, a_single))
	{
	}
	void f_stop()
	{
		v_timer->f_stop();
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
	static size_t f_do_call(t_object* a_this, t_stacked* a_stack, size_t a_n);
};

template<>
struct t_type_of<xemmaix::suisha::t_timer> : t_uninstantiatable<t_underivable<t_holds<xemmaix::suisha::t_timer>>>
{
	typedef xemmaix::suisha::t_extension t_extension;

	static t_scoped f_transfer(const t_extension* a_extension, xemmaix::suisha::t_timer* a_value)
	{
		auto object = t_object::f_allocate(a_extension->f_type<xemmaix::suisha::t_timer>());
		object.f_pointer__(a_value);
		return object;
	}
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan);
};

template<>
struct t_type_of<suisha::t_loop> : t_uninstantiatable<t_underivable<t_with_traits<t_bears<suisha::t_loop>, false, true>>>
{
	template<typename T>
	struct t_as;
	template<typename T0>
	struct t_as<T0*>
	{
		template<typename T1>
		static T0* f_call(T1&& a_object)
		{
			auto p = static_cast<T0*>(f_object(std::forward<T1>(a_object))->f_pointer());
			if (!p) f_throw(L"already destroyed.");
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

	static void f_post(suisha::t_loop& a_self, t_scoped&& a_callable)
	{
		t_thread::f_cache_release();
		a_self.f_post([callable = std::move(a_callable)]
		{
			t_thread::f_cache_acquire();
			callable();
		});
	}
	static void f_poll(suisha::t_loop& a_self, int a_descriptor, bool a_read, bool a_write, t_scoped&& a_callable)
	{
		a_self.f_poll(a_descriptor, a_read, a_write, [callable = std::move(a_callable)](bool a_readable, bool a_writable)
		{
			callable(f_global()->f_as(a_readable), f_global()->f_as(a_writable));
		});
	}
	static xemmaix::suisha::t_timer* f_timer(suisha::t_loop& a_self, t_scoped&& a_callable, size_t a_interval, bool a_single)
	{
		return new xemmaix::suisha::t_timer(a_self, std::move(a_callable), a_interval, a_single);
	}
	static xemmaix::suisha::t_timer* f_timer(suisha::t_loop& a_self, t_scoped&& a_callable, size_t a_interval)
	{
		return new xemmaix::suisha::t_timer(a_self, std::move(a_callable), a_interval, false);
	}
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
};

}

#endif
