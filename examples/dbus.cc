#include "dbus.h"

namespace suisha
{

namespace dbus
{

void t_message::f_get(int a_type, ...) const
{
	DBusError error;
	dbus_error_init(&error);
	va_list list;
	va_start(list, a_type);
	dbus_bool_t b = dbus_message_get_args_valist(v_p, &error, a_type, list);
	va_end(list);
	if (dbus_error_is_set(&error) != FALSE) {
		std::string s = error.message;
		dbus_error_free(&error);
		throw std::runtime_error("dbus_message_get_args_valist failed: " + s);
	}
	if (b == FALSE) throw std::runtime_error("dbus_message_get_args_valist failed.");
}

t_message t_reply::operator()()
{
	dbus_pending_call_block(v_p);
	return f_steal(v_p);
}

void t_reply::operator()(std::function<void (const t_message&)>&& a_function)
{
	auto p = new std::remove_reference_t<decltype(a_function)>(std::move(a_function));
	if (dbus_pending_call_set_notify(v_p, [](auto a_pending, auto a_data)
	{
		(*static_cast<decltype(p)>(a_data))(f_steal(a_pending));
	}, p, [](auto a_data)
	{
		delete static_cast<decltype(p)>(a_data);
	}) == FALSE) throw std::runtime_error("dbus_pending_call_set_notify failed.");
}

DBusHandlerResult t_connection::f_filter(DBusConnection* a_connection, DBusMessage* a_message, void* a_data)
{
//std::fprintf(stderr, "filter: %s, %s, %s, %s\n", dbus_message_type_to_string(dbus_message_get_type(a_message)), dbus_message_get_path(a_message), dbus_message_get_interface(a_message), dbus_message_get_member(a_message));
	auto p = static_cast<t_connection*>(a_data);
	auto i = p->v_matches.find(t_match{dbus_message_get_type(a_message), f_s(dbus_message_get_path(a_message)), f_s(dbus_message_get_interface(a_message)), f_s(dbus_message_get_member(a_message))});
	if (i != p->v_matches.end()) {
		t_message message(a_message, t_own());
		(*i->second.v_function)(i->second.v_this, message);
		return DBUS_HANDLER_RESULT_HANDLED;
	}
	if (dbus_message_is_signal(a_message, DBUS_INTERFACE_LOCAL, "Disconnected") != FALSE) {
		for (auto q : p->v_disconnecteds) (*q.v_function)(q.v_this);
		p->f_reset();
		return DBUS_HANDLER_RESULT_HANDLED;
	}
	return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

void t_connection::f_open(const char* a_address)
{
	DBusError error;
	dbus_error_init(&error);
	v_p = dbus_connection_open(a_address, &error);
	if (dbus_error_is_set(&error) != FALSE) {
		std::string s = error.message;
		dbus_error_free(&error);
		throw std::runtime_error("dbus_connection_open failed: " + s);
	}
	if (v_p == NULL) throw std::runtime_error("dbus_connection_open failed.");
	if (dbus_connection_add_filter(v_p, f_filter, this, NULL) == FALSE) throw std::runtime_error("dbus_connection_add_filter failed.");
}

void t_connection::f_connect(DBusBusType a_type)
{
	DBusError error;
	dbus_error_init(&error);
	v_p = dbus_bus_get(a_type, &error);
	if (dbus_error_is_set(&error) != FALSE) {
		std::string s = error.message;
		dbus_error_free(&error);
		throw std::runtime_error("dbus_bus_get failed: " + s);
	}
	if (v_p == NULL) throw std::runtime_error("dbus_bus_get failed.");
	dbus_connection_set_exit_on_disconnect(v_p, FALSE);
	if (dbus_connection_add_filter(v_p, f_filter, this, NULL) == FALSE) throw std::runtime_error("dbus_connection_add_filter failed.");
}

t_reply t_connection::f_send_valist(const char* a_destination, const char* a_path, const char* a_interface, const char* a_method, int a_type, va_list a_list)
{
	t_message message = dbus_message_new_method_call(a_destination, a_path, a_interface, a_method);
	if (!message) throw std::runtime_error("dbus_message_new_method_call failed.");
	if (dbus_message_append_args_valist(message, a_type, a_list) == FALSE) throw std::runtime_error("dbus_message_append_args_valist failed.");
	DBusPendingCall* p;
	if (dbus_connection_send_with_reply(v_p, message, &p, DBUS_TIMEOUT_USE_DEFAULT) == FALSE) throw std::runtime_error("dbus_connection_send_with_reply failed.");
	return t_reply(p, v_p);
}

void t_connection::f_reply_valist(DBusMessage* a_call, int a_type, va_list a_list)
{
	t_message message = dbus_message_new_method_return(a_call);
	if (!message) throw std::runtime_error("dbus_message_new_method_return failed.");
	if (dbus_message_append_args_valist(message, a_type, a_list) == FALSE) throw std::runtime_error("dbus_message_append_args_valist failed.");
	if (dbus_connection_send(v_p, message, NULL) == FALSE) throw std::runtime_error("dbus_connection_send failed.");
}

void t_connection::f_emit_valist(const char* a_path, const char* a_interface, const char* a_name, int a_type, va_list a_list)
{
	t_message message = dbus_message_new_signal(a_path, a_interface, a_name);
	if (!message) throw std::runtime_error("dbus_message_new_signal failed.");
	if (dbus_message_append_args_valist(message, a_type, a_list) == FALSE) throw std::runtime_error("dbus_message_append_args_valist failed.");
	if (dbus_connection_send(v_p, message, NULL) == FALSE) throw std::runtime_error("dbus_connection_send failed.");
}

void t_connection::f_add_match(void* a_this, void (*a_function)(void*, t_message&), int a_type, const std::string& a_path, const std::string& a_interface, const std::string& a_member)
{
	if (!v_matches.emplace(t_match{a_type, a_path, a_interface, a_member}, t_slot_message{a_this, a_function}).second) return;
	std::string type = dbus_message_type_to_string(a_type);
	DBusError error;
	dbus_error_init(&error);
	dbus_bus_add_match(v_p, ("type='" + type + "',path='" + a_path + "',interface='" + a_interface + "',member='" + a_member + "'").c_str(), &error);
	if (dbus_error_is_set(&error) != FALSE) {
		std::string s = error.message;
		dbus_error_free(&error);
		throw std::runtime_error("dbus_bus_add_match failed: " + s);
	}
}

void t_connection::f_remove_match(int a_type, const std::string& a_path, const std::string& a_interface, const std::string& a_member)
{
	if (v_matches.erase(t_match{a_type, a_path, a_interface, a_member}) <= 0) return;
	std::string type = dbus_message_type_to_string(a_type);
	DBusError error;
	dbus_error_init(&error);
	dbus_bus_remove_match(v_p, ("type='" + type + "',path='" + a_path + "',interface='" + a_interface + "',member='" + a_member + "'").c_str(), &error);
	if (dbus_error_is_set(&error) != FALSE) {
		std::string s = error.message;
		dbus_error_free(&error);
		throw std::runtime_error("dbus_bus_remove_match failed: " + s);
	}
}

}

}
