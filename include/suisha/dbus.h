#ifndef SUISHA__DBUS_H
#define SUISHA__DBUS_H

#include "loop.h"
#include <dbus/dbus.h>

namespace suisha
{

namespace dbus
{

class t_bridge
{
	template<typename T>
	static void f_free(void* a_p)
	{
		delete static_cast<T*>(a_p);
	}
	static void f_enable(DBusConnection* a_connection, DBusWatch* a_watch);
	static void f_enable(DBusTimeout* a_timeout);
	static void f_disable(DBusTimeout* a_timeout);

	static thread_local dbus_int32_t v_slot;

	DBusConnection* v_connection;

	~t_bridge()
	{
		dbus_connection_free_data_slot(&v_slot);
	}

public:
	t_bridge(DBusConnection* a_connection);
};

}

}

#endif
