dbus = Module("dbus"

$Proxy = Class() :: @
	$__initialize = @(connection, destination, path, interface)
		$connection = connection
		$destination = destination
		$path = path
		$interface = interface
	$add_match = @(signal, callable) $connection.add_match(dbus.MESSAGE_TYPE_SIGNAL, $path, $interface, signal, callable
	$remove_match = @(signal) $connection.remove_match(dbus.MESSAGE_TYPE_SIGNAL, $path, $interface, signal
	$add_properties_changed = @(callable) $connection.add_match(dbus.MESSAGE_TYPE_SIGNAL, $path, dbus.INTERFACE_PROPERTIES, "PropertiesChanged", callable
	$remove_properties_changed = @ $connection.remove_match(dbus.MESSAGE_TYPE_SIGNAL, $path, dbus.INTERFACE_PROPERTIES, "PropertiesChanged"
	$method = @(method) dbus.Message($destination, $path, $interface, method
	$send = @(message, callable)
		try
			$connection.send_with_reply(message)(callable
		finally
			message.release(
	$call = @(message)
		try
			reply = $connection.send_with_reply(message
			result = reply(
			result.get_type() == dbus.MESSAGE_TYPE_ERROR && throw Throwable(result.get().__string(
			a = result.get(
			a.size() > 0 ? a[0] : null
		finally
			message.release(
			reply && reply.release(
			result && result.release(
	$properties = @(method, name) dbus.Message($destination, $path, dbus.INTERFACE_PROPERTIES, method).append($interface).append(name
	$get = @(name) $call($properties("Get", name
	$set = @(name, value) $call($properties("Set", name).append(value

$Service = Class() :: @
	$__initialize = @(connection, path, interface)
		$connection = connection
		$path = path
		$interface = interface
	$add_match = @(method, callable) $connection.add_match(dbus.MESSAGE_TYPE_METHOD_CALL, $path, $interface, method, callable
	$remove_match = @(method) $connection.remove_match(dbus.MESSAGE_TYPE_METHOD_CALL, $path, $interface, method
	$signal = @(name) dbus.Message($path, $interface, name
	$send = @(message)
		try
			$connection.send(message
		finally
			message.release(
