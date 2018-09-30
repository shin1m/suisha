#include "loop.h"

namespace xemmai
{

void t_type_of<xemmaix::suisha::t_wait>::f_define(t_extension* a_extension)
{
	t_define<xemmaix::suisha::t_wait, t_object>(a_extension, L"Wait"sv);
}

size_t t_type_of<xemmaix::suisha::t_wait>::f_do_call(t_object* a_this, t_stacked* a_stack, size_t a_n)
{
	if (a_n > 0) f_throw(a_stack, a_n, L"must be called without an argument."sv);
	a_stack[1].f_destruct();
	f_as<xemmaix::suisha::t_wait&>(a_this).v_wait();
	a_stack[0].f_construct();
	return -1;
}

void t_type_of<xemmaix::suisha::t_timer>::f_define(t_extension* a_extension)
{
	using xemmaix::suisha::t_timer;
	t_define<t_timer, t_object>(a_extension, L"Timer"sv)
		(L"stop"sv, t_member<void(t_timer::*)(), &t_timer::f_stop>())
	;
}

void t_type_of<xemmaix::suisha::t_timer>::f_do_scan(t_object* a_this, t_scan a_scan)
{
	a_scan(f_as<xemmaix::suisha::t_timer&>(a_this).v_callable);
}

void t_type_of<suisha::t_loop>::f_define(t_extension* a_extension)
{
	using namespace suisha;
	t_define<t_loop, t_object>(a_extension, L"Loop"sv)
		(L"run"sv, t_member<void(t_loop::*)(), &t_loop::f_run>())
		(L"exit"sv, t_member<void(t_loop::*)(), &t_loop::f_exit>())
		(L"terminate"sv, t_member<void(t_loop::*)(), &t_loop::f_terminate>())
		(L"more"sv, t_member<void(t_loop::*)(), &t_loop::f_more>())
		(L"post"sv, t_member<void(*)(t_loop&, t_scoped&&), f_post, t_with_lock_for_write>())
		(L"poll"sv, t_member<void(*)(t_loop&, int, bool, bool, t_scoped&&), f_poll>())
		(L"unpoll"sv, t_member<void(t_loop::*)(int), &t_loop::f_unpoll>())
		(L"timer"sv,
			t_member<xemmaix::suisha::t_timer*(*)(t_loop&, t_scoped&&, size_t), f_timer>(),
			t_member<xemmaix::suisha::t_timer*(*)(t_loop&, t_scoped&&, size_t, bool), f_timer>()
		)
	;
}

}
