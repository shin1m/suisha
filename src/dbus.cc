#include <suisha/dbus.h>

namespace suisha
{

namespace dbus
{

void t_bridge::f_enable(DBusConnection* a_connection, DBusWatch* a_watch)
{
	unsigned int flags = dbus_watch_get_flags(a_watch);
	f_loop()->f_poll(dbus_watch_get_unix_fd(a_watch), (flags & DBUS_WATCH_READABLE) != 0, (flags & DBUS_WATCH_WRITABLE) != 0, [a_connection, a_watch](bool a_readable, bool a_writable)
	{
		unsigned int flags = 0;
		if (a_readable) flags |= DBUS_WATCH_READABLE;
		if (a_writable) flags |= DBUS_WATCH_WRITABLE;
		if (flags == 0) return;
		while (true) {
			bool b = dbus_watch_handle(a_watch, flags) != FALSE;
			while (dbus_connection_dispatch(a_connection) == DBUS_DISPATCH_DATA_REMAINS);
			if (b) break;
		}
	});
}

void t_bridge::f_enable(DBusTimeout* a_timeout)
{
	dbus_timeout_set_data(a_timeout, new std::shared_ptr<t_timer>(f_loop()->f_timer([a_timeout]
	{
		dbus_timeout_handle(a_timeout);
	}, dbus_timeout_get_interval(a_timeout))), f_free<std::shared_ptr<t_timer>>);
}

void t_bridge::f_disable(DBusTimeout* a_timeout)
{
	(*static_cast<std::shared_ptr<t_timer>*>(dbus_timeout_get_data(a_timeout)))->f_stop();
	dbus_timeout_set_data(a_timeout, NULL, NULL);
}

thread_local dbus_int32_t t_bridge::v_slot = -1;

t_bridge::t_bridge(DBusConnection* a_connection) : v_connection(a_connection)
{
	if (dbus_connection_allocate_data_slot(&v_slot) == FALSE) throw std::runtime_error("dbus_connection_allocate_data_slot failed.");
	if (dbus_connection_set_data(v_connection, v_slot, this, f_free<t_bridge>) == FALSE) throw std::runtime_error("dbus_connection_set_data failed.");
	if (dbus_connection_set_watch_functions(v_connection, [](auto a_watch, auto a_data) -> dbus_bool_t
	{
		if (dbus_watch_get_enabled(a_watch) != FALSE) f_enable(static_cast<t_bridge*>(a_data)->v_connection, a_watch);
		return TRUE;
	}, [](auto a_watch, auto a_data)
	{
		f_loop()->f_unpoll(dbus_watch_get_unix_fd(a_watch));
	}, [](auto a_watch, auto a_data)
	{
		if (dbus_watch_get_enabled(a_watch) == FALSE)
			f_loop()->f_unpoll(dbus_watch_get_unix_fd(a_watch));
		else
			f_enable(static_cast<t_bridge*>(a_data)->v_connection, a_watch);
	}, this, NULL) == FALSE) throw std::runtime_error("dbus_connection_set_watch_functions failed.");
	if (dbus_connection_set_timeout_functions(v_connection, [](auto a_timeout, auto a_data) -> dbus_bool_t
	{
		if (dbus_timeout_get_enabled(a_timeout) != FALSE) f_enable(a_timeout);
		return TRUE;
	}, [](auto a_timeout, auto a_data)
	{
		f_disable(a_timeout);
	}, [](auto a_timeout, auto a_data)
	{
		if (dbus_timeout_get_enabled(a_timeout) == FALSE)
			f_disable(a_timeout);
		else
			f_enable(a_timeout);
	}, this, NULL) == FALSE) throw std::runtime_error("dbus_connection_set_timeout_functions failed.");
}

}

}
