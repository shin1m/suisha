#include "loop.h"

namespace xemmaix::suisha
{

namespace
{

thread_local t_root v_loop;

t_pvalue f_loop()
{
	return static_cast<t_object*>(v_loop);
}

}

void t_library::f_main(t_library* a_library, const t_pvalue& a_callable)
{
	::suisha::t_loop loop;
	v_loop = f_new<t_loop>(a_library, loop);
	auto wait = f_new<t_wait>(a_library, [wait = std::move(loop.v_wait)]
	{
		t_safe_region region;
		wait();
	});
	v_loop->f_fields()[/*wait*/0] = wait;
	loop.v_wait = [&]
	{
		v_loop->f_fields()[/*wait*/0]();
	};
	auto finalize = [&]
	{
		auto& loop = v_loop->f_as<t_loop>();
		std::lock_guard lock(loop.v_mutex);
		loop.v_loop = nullptr;
		v_loop = nullptr;
		wait->f_as<t_wait>().v_wait = []
		{
		};
	};
	try {
		a_callable(v_loop);
		finalize();
	} catch (...) {
		finalize();
		throw;
	}
}

void t_library::f_scan(t_scan a_scan)
{
	a_scan(v_type_wait);
	a_scan(v_type_timer);
	a_scan(v_type_loop);
}

std::vector<std::pair<t_root, t_rvalue>> t_library::f_define()
{
	t_type_of<t_wait>::f_define(this);
	t_type_of<t_timer>::f_define(this);
	t_type_of<t_loop>::f_define(this);
	return t_define(this)
		(L"Wait"sv, static_cast<t_object*>(v_type_wait))
		(L"Timer"sv, static_cast<t_object*>(v_type_timer))
		(L"Loop"sv, static_cast<t_object*>(v_type_loop))
		(L"main"sv, t_static<void(*)(t_library*, const t_pvalue&), f_main>())
		(L"loop"sv, t_static<t_pvalue(*)(), f_loop>())
	;
}

}

XEMMAI__MODULE__FACTORY(xemmai::t_library::t_handle* a_handle)
{
	return xemmai::f_new<xemmaix::suisha::t_library>(a_handle);
}
