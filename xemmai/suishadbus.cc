#include <xemmaix/dbus/connection.h>
#include <suisha/dbus.h>

namespace xemmaix
{

namespace suishadbus
{

using namespace xemmai;

void f_watch(xemmaix::dbus::t_connection& a_connection)
{
	new suisha::dbus::t_bridge(a_connection);
}

struct t_extension : xemmai::t_extension
{
	t_extension(t_object* a_module) : xemmai::t_extension(a_module)
	{
		f_define<void(*)(xemmaix::dbus::t_connection&), f_watch>(this, L"watch");
	}
	virtual void f_scan(t_scan a_scan)
	{
	}
};

}

}

XEMMAI__MODULE__FACTORY(xemmai::t_object* a_module)
{
	return new xemmaix::suishadbus::t_extension(a_module);
}
