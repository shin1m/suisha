#include "loop.h"

namespace xemmai
{

void t_type_of<xemmaix::suisha::t_wait>::f_define(t_library* a_library)
{
	t_define{a_library}.f_derive<xemmaix::suisha::t_wait, t_object>();
}

size_t t_type_of<xemmaix::suisha::t_wait>::f_do_call(t_object* a_this, t_pvalue* a_stack, size_t a_n)
{
	if (a_n > 0) f_throw(L"must be called without an argument."sv);
	auto& wait = a_this->f_as<xemmaix::suisha::t_wait>();
	wait.f_owned_or_throw();
	wait.v_wait();
	a_stack[0] = nullptr;
	return -1;
}

void t_type_of<xemmaix::suisha::t_timer>::f_define(t_library* a_library)
{
	using xemmaix::suisha::t_timer;
	t_define{a_library}
		(L"stop"sv, t_member<void(t_timer::*)(), &t_timer::f_stop>())
	.f_derive<t_timer, t_object>();
}

void t_type_of<xemmaix::suisha::t_loop>::f_define(t_library* a_library)
{
	using namespace suisha;
	using xemmaix::suisha::t_loop;
	t_define{a_library}
		(L"wait"sv)
		(L"run"sv, t_member<void(t_loop::*)(), &t_loop::f_run>())
		(L"exit"sv, t_member<void(t_loop::*)(), &t_loop::f_exit>())
		(L"terminate"sv, t_member<void(t_loop::*)(), &t_loop::f_terminate>())
		(L"more"sv, t_member<void(t_loop::*)(), &t_loop::f_more>())
		(L"post"sv, t_member<void(t_loop::*)(const t_pvalue&), &t_loop::f_post>())
		(L"poll"sv, t_member<void(t_loop::*)(int, bool, bool, const t_pvalue&), &t_loop::f_poll>())
		(L"unpoll"sv, t_member<void(t_loop::*)(int), &t_loop::f_unpoll>())
		(L"timer"sv,
			t_member<t_pvalue(t_loop::*)(t_library*, const t_pvalue&, size_t), &t_loop::f_timer>(),
			t_member<t_pvalue(t_loop::*)(t_library*, const t_pvalue&, size_t, bool), &t_loop::f_timer>()
		)
	.f_derive<t_loop, t_object>();
}

}
