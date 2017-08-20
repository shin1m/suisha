#ifndef SUISHA__EXAMPLES__DBUS_H
#define SUISHA__EXAMPLES__DBUS_H

#include <functional>
#include <map>
#include <set>
#include <string>
#include <suisha/dbus.h>

namespace suisha
{

namespace dbus
{

class t_connection;

struct t_own
{
};

class t_message
{
	DBusMessage* v_p;

public:
	t_message(DBusMessage* a_p = 0) : v_p(a_p)
	{
	}
	t_message(DBusMessage* a_p, const t_own&) : v_p(a_p)
	{
		if (v_p != NULL) dbus_message_ref(v_p);
	}
	t_message(const t_message& a_message) : v_p(a_message.v_p)
	{
		if (v_p != NULL) dbus_message_ref(v_p);
	}
	~t_message()
	{
		if (v_p != NULL) dbus_message_unref(v_p);
	}
	t_message& operator=(const t_message& a_message)
	{
		if (a_message.v_p != NULL) dbus_message_ref(a_message.v_p);
		DBusMessage* p = v_p;
		v_p = a_message.v_p;
		if (p != NULL) dbus_message_unref(p);
		return *this;
	}
	operator DBusMessage*() const
	{
		return v_p;
	}
	void f_get(int a_type, ...) const;
};

class t_reply
{
	static t_message f_steal(DBusPendingCall* a_p)
	{
		t_message message = dbus_pending_call_steal_reply(a_p);
		if (!message) throw std::runtime_error("dbus_pending_call_steal_reply failed.");
		return message;
	}

	DBusPendingCall* v_p = NULL;
	DBusConnection* v_connection = NULL;

public:
	t_reply() = default;
	t_reply(DBusPendingCall* a_p, DBusConnection* a_connection) : v_p(a_p), v_connection(a_connection)
	{
		if (v_connection != NULL) dbus_connection_ref(v_connection);
	}
	t_reply(const t_reply& a_reply) : v_p(a_reply.v_p), v_connection(a_reply.v_connection)
	{
		if (v_p != NULL) dbus_pending_call_ref(v_p);
		if (v_connection != NULL) dbus_connection_ref(v_connection);
	}
	~t_reply()
	{
		if (v_p != NULL) dbus_pending_call_unref(v_p);
		if (v_connection != NULL) dbus_connection_unref(v_connection);
	}
	t_reply& operator=(const t_reply& a_reply)
	{
		if (a_reply.v_p != NULL) dbus_pending_call_ref(a_reply.v_p);
		if (a_reply.v_connection != NULL) dbus_connection_ref(a_reply.v_connection);
		DBusPendingCall* p = v_p;
		DBusConnection* connection = v_connection;
		v_p = a_reply.v_p;
		v_connection = a_reply.v_connection;
		if (p != NULL) dbus_pending_call_unref(p);
		if (connection != NULL) dbus_connection_ref(connection);
		return *this;
	}
	t_message operator()();
	void operator()(std::function<void (const t_message&)>&& a_function);
};

class t_connection
{
	struct t_match
	{
		int v_type;
		std::string v_path;
		std::string v_interface;
		std::string v_member;

		bool operator<(const t_match& a_match) const
		{
			if (v_type != a_match.v_type) return v_type < a_match.v_type;
			if (v_path < a_match.v_path) return true;
			if (v_path > a_match.v_path) return false;
			if (v_interface < a_match.v_interface) return true;
			if (v_interface > a_match.v_interface) return false;
			return v_member < a_match.v_member;
		}
	};
	template<typename T_function>
	struct t_slot
	{
		void* v_this;
		T_function v_function;

		bool operator<(const t_slot& a_x) const
		{
			if (v_this != a_x.v_this) return v_this < a_x.v_this;
			return v_function < a_x.v_function;
		}
	};
	typedef t_slot<void (*)(void*)> t_slot_void;
	typedef t_slot<void (*)(void*, t_message&)> t_slot_message;

	static std::string f_s(const char* a_p)
	{
		return a_p ? std::string(a_p) : std::string();
	}
	static DBusHandlerResult f_filter(DBusConnection* a_connection, DBusMessage* a_message, void* a_data);

	DBusConnection* v_p = NULL;
	std::set<t_slot_void> v_disconnecteds;
	std::map<t_match, t_slot_message> v_matches;

	void f_reset()
	{
		if (v_p != NULL) {
			dbus_connection_unref(v_p);
			v_p = NULL;
		}
		v_disconnecteds.clear();
		v_matches.clear();
	}

public:
	~t_connection()
	{
		f_reset();
	}
	operator DBusConnection*() const
	{
		return v_p;
	}
	void f_open(const char* a_address);
	void f_connect(DBusBusType a_type);
	t_reply f_send(const char* a_destination, const char* a_path, const char* a_interface, const char* a_method, int a_type, ...)
	{
		va_list list;
		va_start(list, a_type);
		t_reply reply = f_send_valist(a_destination, a_path, a_interface, a_method, a_type, list);
		va_end(list);
		return reply;
	}
	t_reply f_send_valist(const char* a_destination, const char* a_path, const char* a_interface, const char* a_method, int a_type, va_list a_list);
	void f_reply(DBusMessage* a_call, int a_type, ...)
	{
		va_list list;
		va_start(list, a_type);
		f_reply_valist(a_call, a_type, list);
		va_end(list);
	}
	void f_reply_valist(DBusMessage* a_call, int a_type, va_list a_list);
	void f_emit(const char* a_path, const char* a_interface, const char* a_name, int a_type, ...)
	{
		va_list list;
		va_start(list, a_type);
		f_emit_valist(a_path, a_interface, a_name, a_type, list);
		va_end(list);
	}
	void f_emit_valist(const char* a_path, const char* a_interface, const char* a_name, int a_type, va_list a_list);
	void f_add_disconnected(void* a_this, void (*a_function)(void*))
	{
		v_disconnecteds.insert(t_slot_void{a_this, a_function});
	}
	void f_remove_disconnected(void* a_this, void (*a_function)(void*))
	{
		v_disconnecteds.erase(t_slot_void{a_this, a_function});
	}
	void f_add_match(void* a_this, void (*a_function)(void*, t_message&), int a_type, const std::string& a_path, const std::string& a_interface, const std::string& a_member);
	void f_remove_match(int a_type, const std::string& a_path, const std::string& a_interface, const std::string& a_member);
};

template<typename T_this, void (T_this::*A_function)()>
void f_slot_member(void* a_this)
{
	(static_cast<T_this*>(a_this)->*A_function)();
}

template<typename T_this, void (T_this::*A_function)(t_message&)>
void f_slot_member(void* a_this, t_message& a_message)
{
	(static_cast<T_this*>(a_this)->*A_function)(a_message);
}

class t_container_builder
{
	DBusMessageIter& v_parent;
	DBusMessageIter v_i;

public:
	t_container_builder(DBusMessageIter& a_parent, int a_type, const char* a_signature) : v_parent(a_parent)
	{
		dbus_message_iter_open_container(&v_parent, a_type, a_signature, &v_i);
	}
	~t_container_builder()
	{
		dbus_message_iter_close_container(&v_parent, &v_i);
	}
	operator DBusMessageIter&()
	{
		return v_i;
	}
};

}

}

inline DBusMessageIter& operator>>(DBusMessageIter& a_i, bool& a_value)
{
	if (dbus_message_iter_get_arg_type(&a_i) != DBUS_TYPE_BOOLEAN) throw std::runtime_error("dbus_message_iter_get_arg_type must be DBUS_TYPE_BOOLEAN.");
	dbus_bool_t value;
	dbus_message_iter_get_basic(&a_i, &value);
	a_value = value != FALSE;
	dbus_message_iter_next(&a_i);
	return a_i;
}

inline DBusMessageIter& operator>>(DBusMessageIter& a_i, dbus_int32_t& a_value)
{
	if (dbus_message_iter_get_arg_type(&a_i) != DBUS_TYPE_INT32) throw std::runtime_error("dbus_message_iter_get_arg_type must be DBUS_TYPE_INT32.");
	dbus_message_iter_get_basic(&a_i, &a_value);
	dbus_message_iter_next(&a_i);
	return a_i;
}

inline DBusMessageIter& operator>>(DBusMessageIter& a_i, dbus_uint32_t& a_value)
{
	if (dbus_message_iter_get_arg_type(&a_i) != DBUS_TYPE_UINT32) throw std::runtime_error("dbus_message_iter_get_arg_type must be DBUS_TYPE_UINT32.");
	dbus_message_iter_get_basic(&a_i, &a_value);
	dbus_message_iter_next(&a_i);
	return a_i;
}

inline DBusMessageIter& operator>>(DBusMessageIter& a_i, const char*& a_value)
{
	if (dbus_message_iter_get_arg_type(&a_i) != DBUS_TYPE_STRING) throw std::runtime_error("dbus_message_iter_get_arg_type must be DBUS_TYPE_STRING.");
	dbus_message_iter_get_basic(&a_i, &a_value);
	dbus_message_iter_next(&a_i);
	return a_i;
}

inline DBusMessageIter& operator<<(DBusMessageIter& a_i, bool a_value)
{
	dbus_bool_t value = a_value ? TRUE : FALSE;
	if (dbus_message_iter_append_basic(&a_i, DBUS_TYPE_BOOLEAN, &value) == FALSE) throw std::runtime_error("dbus_message_iter_append_basic failed.");
	return a_i;
}

inline DBusMessageIter& operator<<(DBusMessageIter& a_i, dbus_int32_t a_value)
{
	if (dbus_message_iter_append_basic(&a_i, DBUS_TYPE_INT32, &a_value) == FALSE) throw std::runtime_error("dbus_message_iter_append_basic failed.");
	return a_i;
}

inline DBusMessageIter& operator<<(DBusMessageIter& a_i, dbus_uint32_t a_value)
{
	if (dbus_message_iter_append_basic(&a_i, DBUS_TYPE_UINT32, &a_value) == FALSE) throw std::runtime_error("dbus_message_iter_append_basic failed.");
	return a_i;
}

inline DBusMessageIter& operator<<(DBusMessageIter& a_i, const char* a_value)
{
	if (dbus_message_iter_append_basic(&a_i, DBUS_TYPE_STRING, &a_value) == FALSE) throw std::runtime_error("dbus_message_iter_append_basic failed.");
	return a_i;
}

#endif
