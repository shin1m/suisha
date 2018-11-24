#include "loop.h"

namespace xemmaix::suisha
{

namespace
{

thread_local t_scoped v_loop;

t_scoped f_loop()
{
	return v_loop;
}

}

void t_extension::f_main(t_extension* a_extension, const t_value& a_callable)
{
	t_loop loop;
	v_loop = t_object::f_allocate(a_extension->f_type<t_loop>(), true, sizeof(t_loop*));
	v_loop->f_as<t_loop*>() = &loop;
	auto wait = f_new<t_wait>(a_extension, false, [wait = std::move(loop.v_wait)]
	{
		t_safe_region region;
		wait();
	});
	auto symbol_wait = t_symbol::f_instantiate(L"wait"sv);
	v_loop.f_put(symbol_wait, std::move(wait));
	loop.v_wait = [&]
	{
		t_scoped_stack stack(2);
		stack[1].f_construct();
		v_loop.f_call(symbol_wait, stack, 0);
		stack.f_return();
	};
	auto finalize = [&]
	{
		t_with_lock_for_write lock(v_loop);
		v_loop->f_as<t_loop*>() = nullptr;
		v_loop = nullptr;
	};
	try {
		a_callable(v_loop);
		finalize();
	} catch (...) {
		finalize();
		throw;
	}
}

t_extension::t_extension(t_object* a_module) : xemmai::t_extension(a_module)
{
	t_type_of<t_wait>::f_define(this);
	t_type_of<t_timer>::f_define(this);
	t_type_of<t_loop>::f_define(this);
	f_define<void(*)(t_extension*, const t_value&), f_main>(this, L"main"sv);
	f_define<t_scoped(*)(), f_loop>(this, L"loop"sv);
}

void t_extension::f_scan(t_scan a_scan)
{
	a_scan(v_type_wait);
	a_scan(v_type_timer);
	a_scan(v_type_loop);
}

}

XEMMAI__MODULE__FACTORY(xemmai::t_object* a_module)
{
	return new xemmaix::suisha::t_extension(a_module);
}
