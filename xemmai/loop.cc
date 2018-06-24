#include "loop.h"

namespace xemmai
{

void t_type_of<xemmaix::suisha::t_wait>::f_define(t_extension* a_extension)
{
	t_define<xemmaix::suisha::t_wait, t_object>(a_extension, L"Wait");
}

t_type* t_type_of<xemmaix::suisha::t_wait>::f_derive()
{
	return nullptr;
}

void t_type_of<xemmaix::suisha::t_wait>::f_finalize(t_object* a_this)
{
	delete &f_as<xemmaix::suisha::t_wait&>(a_this);
}

t_scoped t_type_of<xemmaix::suisha::t_wait>::f_construct(t_stacked* a_stack, size_t a_n)
{
	t_throwable::f_throw(L"uninstantiatable.");
}

size_t t_type_of<xemmaix::suisha::t_wait>::f_call(t_object* a_this, t_stacked* a_stack, size_t a_n)
{
	if (a_n > 0) t_throwable::f_throw(a_stack, a_n, L"must be called without an argument.");
	a_stack[1].f_destruct();
	f_as<xemmaix::suisha::t_wait&>(a_this).v_wait();
	a_stack[0].f_construct();
	return -1;
}

void t_type_of<xemmaix::suisha::t_timer>::f_define(t_extension* a_extension)
{
	using xemmaix::suisha::t_timer;
	t_define<t_timer, t_object>(a_extension, L"Timer")
		(L"stop", t_member<void(t_timer::*)(), &t_timer::f_stop>())
	;
}

t_type* t_type_of<xemmaix::suisha::t_timer>::f_derive()
{
	return nullptr;
}

void t_type_of<xemmaix::suisha::t_timer>::f_scan(t_object* a_this, t_scan a_scan)
{
	a_scan(f_as<xemmaix::suisha::t_timer&>(a_this).v_callable);
}

void t_type_of<xemmaix::suisha::t_timer>::f_finalize(t_object* a_this)
{
	delete &f_as<xemmaix::suisha::t_timer&>(a_this);
}

t_scoped t_type_of<xemmaix::suisha::t_timer>::f_construct(t_stacked* a_stack, size_t a_n)
{
	t_throwable::f_throw(L"uninstantiatable.");
}

void t_type_of<suisha::t_loop>::f_define(t_extension* a_extension)
{
	using namespace suisha;
	t_define<t_loop, t_object>(a_extension, L"Loop")
		(L"run", t_member<void(t_loop::*)(), &t_loop::f_run>())
		(L"exit", t_member<void(t_loop::*)(), &t_loop::f_exit>())
		(L"terminate", t_member<void(t_loop::*)(), &t_loop::f_terminate>())
		(L"more", t_member<void(t_loop::*)(), &t_loop::f_more>())
		(L"post", t_member<void(*)(t_loop&, t_scoped&&), f_post, t_with_lock_for_write>())
		(L"poll", t_member<void(*)(t_loop&, int, bool, bool, t_scoped&&), f_poll>())
		(L"unpoll", t_member<void(t_loop::*)(int), &t_loop::f_unpoll>())
		(L"timer",
			t_member<xemmaix::suisha::t_timer*(*)(t_loop&, t_scoped&&, size_t), f_timer>(),
			t_member<xemmaix::suisha::t_timer*(*)(t_loop&, t_scoped&&, size_t, bool), f_timer>()
		)
	;
}

t_type* t_type_of<suisha::t_loop>::f_derive()
{
	return nullptr;
}

t_scoped t_type_of<suisha::t_loop>::f_construct(t_stacked* a_stack, size_t a_n)
{
	t_throwable::f_throw(L"uninstantiatable.");
}

}
