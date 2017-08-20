#include <cstdio>
#include <cstring>

#include "dbus.h"

using namespace suisha;

class t_container_parser
{
	DBusMessageIter v_i;

public:
	t_container_parser(DBusMessageIter& a_parent, int a_type)
	{
		if (dbus_message_iter_get_arg_type(&a_parent) != a_type) throw std::runtime_error("dbus_message_iter_get_arg_type must be " + std::string(1, a_type));
		dbus_message_iter_recurse(&a_parent, &v_i);
	}
	operator DBusMessageIter&()
	{
		return v_i;
	}
	operator DBusMessageIter*()
	{
		return &v_i;
	}
};

struct t_entry_parser : t_container_parser
{
	const char* v_key;

	t_entry_parser(DBusMessageIter& a_parent, int a_type) : t_container_parser(a_parent, DBUS_TYPE_DICT_ENTRY)
	{
		if (dbus_message_iter_get_arg_type(*this) != a_type) throw std::runtime_error("dbus_message_iter_get_arg_type must be " + std::string(1, a_type));
		dbus_message_iter_get_basic(*this, &v_key);
		dbus_message_iter_next(*this);
	}
};

void f_check(const dbus::t_message& a_result)
{
	if (dbus_message_get_type(a_result) != DBUS_MESSAGE_TYPE_ERROR) return;
	const char* p;
	a_result.f_get(DBUS_TYPE_STRING, &p, DBUS_TYPE_INVALID);
	throw std::runtime_error(std::string(dbus_message_get_error_name(a_result)) + ": " + p);
}

template<typename T_match>
void f_find(const dbus::t_message& a_message, const char* a_uuid, T_match a_match)
{
	DBusMessageIter i;
	dbus_message_iter_init(a_message, &i);
	for (t_container_parser paths(i, DBUS_TYPE_ARRAY); dbus_message_iter_get_arg_type(paths) != DBUS_TYPE_INVALID; dbus_message_iter_next(paths)) {
		t_entry_parser path(paths, DBUS_TYPE_OBJECT_PATH);
		for (t_container_parser interfaces(path, DBUS_TYPE_ARRAY); dbus_message_iter_get_arg_type(interfaces) != DBUS_TYPE_INVALID; dbus_message_iter_next(interfaces)) {
			t_entry_parser interface(interfaces, DBUS_TYPE_STRING);
			if (std::strcmp(interface.v_key, "org.bluez.Device1") != 0) continue;
			const char* address = nullptr;
			bool uuid_matched = false;
			for (t_container_parser properties(interface, DBUS_TYPE_ARRAY); dbus_message_iter_get_arg_type(properties) != DBUS_TYPE_INVALID; dbus_message_iter_next(properties)) {
				t_entry_parser property(properties, DBUS_TYPE_STRING);
				if (std::strcmp(property.v_key, "Address") == 0) {
					t_container_parser value(property, DBUS_TYPE_VARIANT);
					value >> address;
					if (uuid_matched) break;
				} else if (std::strcmp(property.v_key, "UUIDs") == 0) {
					t_container_parser value(property, DBUS_TYPE_VARIANT);
					for (t_container_parser uuids(value, DBUS_TYPE_ARRAY); dbus_message_iter_get_arg_type(uuids) != DBUS_TYPE_INVALID; dbus_message_iter_next(uuids)) {
						const char* uuid;
						uuids >> uuid;
						uuid_matched = std::strcmp(uuid, a_uuid) == 0;
						if (uuid_matched) break;
					}
					if (address || !uuid_matched) break;
				} else {
					continue;
				}
			}
			if (uuid_matched) a_match(path.v_key, address);
		}
	}
}

class t_main
{
	inline static const char* v_path = "/xraft/Foo";
	inline static const char* v_uuid_spp = "00001101-0000-1000-8000-00805f9b34fb";

	static void f_copy(int a_from, int a_to)
	{
		char buffer[256];
		ssize_t n = read(a_from, buffer, sizeof(buffer));
		switch (n) {
		case -1:
			throw std::runtime_error("read failed.");
		case 0:
			f_loop()->f_exit();
			break;
		default:
			if (a_to >= 0) do {
				ssize_t m = write(a_to, buffer, n);
				if (m == -1) throw std::runtime_error("write failed.");
				n -= m;
			} while (n > 0);
		}
	}

	dbus::t_connection v_connection;
	std::string v_device;
	int v_fd = -1;

	void f_fd(int a_fd)
	{
		if (v_fd >= 0) {
			f_loop()->f_unpoll(v_fd);
			close(v_fd);
		}
		v_fd = a_fd;
		if (v_fd >= 0) f_loop()->f_poll(v_fd, true, false, [this](bool a_readable, bool a_writable)
		{
			if (a_readable) f_copy(v_fd, 1);
		});
	}
	void f_release_profile(dbus::t_message& a_message)
	{
		std::printf("Release called\n");
		v_connection.f_reply(a_message, DBUS_TYPE_INVALID);
	}
	void f_new_connection(dbus::t_message& a_message)
	{
		const char* device;
		int fd;
		a_message.f_get(
			DBUS_TYPE_OBJECT_PATH, &device,
			DBUS_TYPE_UNIX_FD, &fd,
			DBUS_TYPE_INVALID
		);
		std::printf("NewConnection called: %s, %d\n", device, fd);
		f_fd(fd);
		v_connection.f_reply(a_message, DBUS_TYPE_INVALID);
	}
	void f_request_disconnection(dbus::t_message& a_message)
	{
		const char* device;
		a_message.f_get(
			DBUS_TYPE_OBJECT_PATH, &device,
			DBUS_TYPE_INVALID
		);
		std::printf("RequestDisconnection called: %s\n", device);
		f_loop()->f_exit();
		v_connection.f_reply(a_message, DBUS_TYPE_INVALID);
	}

public:
	t_main(const std::string& a_device)
	{
		v_connection.f_connect(DBUS_BUS_SYSTEM);
		new dbus::t_bridge(v_connection);
		auto result = v_connection.f_send(
			"org.bluez",
			"/",
			"org.freedesktop.DBus.ObjectManager",
			"GetManagedObjects",
			DBUS_TYPE_INVALID
		)();
		f_check(result);
		f_find(result, v_uuid_spp, [&](auto a_path, auto a_address)
		{
			std::printf("%s\n", a_address);
			if (a_address == a_device) v_device = a_path;
		});
		if (v_device.empty()) throw std::runtime_error("device path not found.");
		std::printf("device path found: %s\n", v_device.c_str());
		v_connection.f_add_match(this,
			dbus::f_slot_member<t_main, &t_main::f_release_profile>,
			DBUS_MESSAGE_TYPE_METHOD_CALL,
			v_path,
			"org.bluez.Profile1",
			"Release"
		);
		v_connection.f_add_match(this,
			dbus::f_slot_member<t_main, &t_main::f_new_connection>,
			DBUS_MESSAGE_TYPE_METHOD_CALL,
			v_path,
			"org.bluez.Profile1",
			"NewConnection"
		);
		v_connection.f_add_match(this,
			dbus::f_slot_member<t_main, &t_main::f_request_disconnection>,
			DBUS_MESSAGE_TYPE_METHOD_CALL,
			v_path,
			"org.bluez.Profile1",
			"RequestDisconnection"
		);
		std::printf("registering...\n");
		dbus::t_message message = dbus_message_new_method_call(
			"org.bluez",
			"/org/bluez",
			"org.bluez.ProfileManager1",
			"RegisterProfile"
		);
		if (!message) throw std::runtime_error("dbus_message_new_method_call failed.");
		if (dbus_message_append_args(message,
			DBUS_TYPE_OBJECT_PATH, &v_path,
			DBUS_TYPE_STRING, &v_uuid_spp,
			DBUS_TYPE_INVALID
		) == FALSE) throw std::runtime_error("dbus_message_append_args failed.");
		{
			DBusMessageIter i;
			dbus_message_iter_init_append(message, &i);
			dbus::t_container_builder b(i, DBUS_TYPE_ARRAY, "{sv}");
		}
		{
			DBusPendingCall* p;
			if (dbus_connection_send_with_reply(v_connection, message, &p, DBUS_TIMEOUT_USE_DEFAULT) == FALSE) throw std::runtime_error("dbus_connection_send_with_reply failed.");
			dbus::t_reply(p, v_connection)([this](auto a_result)
			{
				f_check(a_result);
				std::printf("connecting...\n");
				v_connection.f_send(
					"org.bluez",
					v_device.c_str(),
					"org.bluez.Device1",
					"ConnectProfile",
					DBUS_TYPE_STRING, &v_uuid_spp,
					DBUS_TYPE_INVALID
				)(f_check);
			});
		}
		f_loop()->f_poll(0, true, false, [this](bool a_readable, bool a_writable)
		{
			if (a_readable) f_copy(0, v_fd);
		});
	}
	~t_main()
	{
		f_fd(-1);
	}
};

int main(int argc, char* argv[])
{
	if (argc < 2) {
		std::fprintf(stderr, "Usage: %s <device>\n", argv[0]);
		return EXIT_FAILURE;
	}
	t_loop loop;
	try {
		t_main main(argv[1]);
		loop.f_run();
		return EXIT_SUCCESS;
	} catch (std::exception& e) {
		std::fprintf(stderr, "Error: %s\n", e.what());
		return EXIT_FAILURE;
	}
}
