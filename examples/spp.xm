system = Module("system"
print = system.out.write_line
io = Module("io"
suisha = Module("suisha"
dbus = Module("dbus"
suishadbus = Module("suishadbus"
dbusproxy = Module("dbusproxy"

UUID_SPP = "00001101-0000-1000-8000-00805f9b34fb"

suisha.main(@(loop) dbus.main(@
	if system.arguments.size() < 1
		print("Usage: <this> <device>"
		return
	address = system.arguments[0]
	connection = dbus.Connection(dbus.BusType.SYSTEM
	suishadbus.watch(connection
	root = dbusproxy.Proxy(connection
		"org.bluez"
		"/"
		"org.freedesktop.DBus.ObjectManager"
	objects = root.call(root.method("GetManagedObjects")
	try
		objects.each(@(path) path[1].each(@(interface)
			if interface[0] != "org.bluez.Device1": return
			address_matched = false
			uuid_matched = false
			try
				interface[1].each(@(property)
					if property[0] == "Address"
						print("Address: " + property[1]
						if property[1] != address: throw null
						if uuid_matched: throw path[0]
						:address_matched = true
					else if property[0] == "UUIDs"
						try
							property[1].each(@(uuid)
								if uuid == UUID_SPP: throw address_matched ? path[0] : true
							throw null
						catch Boolean x
							:uuid_matched = x
			catch Null _
		throw Throwable("Address " + address + " not found."
	catch String device_path
	remote = null
	remote__ = @(fd)
		if remote !== null
			loop.unpoll(remote.fd
			remote.file.close(
		if fd == -1
			:remote = null
		else
			:remote = Object(
			remote.fd = fd
			remote.file = io.File(fd, "r+"
			remote.writer = io.Writer(remote.file, "utf-8"
			reader = io.Reader(remote.file, "utf-8"
			loop.poll(fd, true, false, @(readable, writable)
				if !readable: return
				try
					system.out.write(reader.read_line(
					system.out.flush(
				catch Throwable e
					print(e
	profile = dbusproxy.Service(connection
		"/foo"
		"org.bluez.Profile1"
	profile.add_match("Release", @(message)
		print("Release called"
		profile.send(dbus.Message(message
	profile.add_match("NewConnection", @(message)
		arguments = message.get(
		print("NewConnection called: " + arguments
		remote__(arguments[1]
		profile.send(dbus.Message(message
	profile.add_match("RequestDisconnection", @(message)
		print("RequestDisconnection called: " + message.get(
		loop.exit(
		profile.send(dbus.Message(message
	manager = dbusproxy.Proxy(connection
		"org.bluez"
		"/org/bluez"
		"org.bluez.ProfileManager1"
	manager.call(manager.method("RegisterProfile")
		.append(dbus.TYPE_OBJECT_PATH, "/foo")
		.append(UUID_SPP)
		.append(dbus.TYPE_ARRAY, "{sv}", @(x)
	device = dbusproxy.Proxy(connection
		"org.bluez"
		device_path
		"org.bluez.Device1"
	device.send(device.method("ConnectProfile")
		.append(UUID_SPP
		@(x)
			# reply callback is not exception safe.
			#if x.get_type() == dbus.MESSAGE_TYPE_ERROR: throw Throwable(x.get().__string(
			if x.get_type() == dbus.MESSAGE_TYPE_ERROR
				print(x.get(
				loop.exit(
	loop.poll(0, true, false, @(readable, writable)
		if !readable: return
		line = system.in.read_line(
		if line == ""
			loop.exit(
		else if remote !== null
			try
				remote.writer.write(line
				remote.writer.flush(
			catch Throwable e
				print(e
	try
		loop.run(
	finally
		remote__(-1
