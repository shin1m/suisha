#include <xemmaix/dbus/connection.h>
#include <suisha/dbus.h>

namespace xemmaix::suishadbus
{

using namespace xemmai;

void f_watch(xemmaix::dbus::t_connection& a_connection)
{
	new suisha::dbus::t_bridge(a_connection);
}

struct t_library : xemmai::t_library
{
	using xemmai::t_library::t_library;
	virtual void f_scan(t_scan a_scan)
	{
	}
	virtual std::vector<std::pair<t_root, t_rvalue>> f_define()
	{
		return t_define(this)
			(L"watch"sv, t_static<void(*)(xemmaix::dbus::t_connection&), f_watch>())
		;
	}
};

}

XEMMAI__MODULE__FACTORY(xemmai::t_library::t_handle* a_handle)
{
	return xemmai::f_new<xemmaix::suishadbus::t_library>(a_handle);
}
