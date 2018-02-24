system = Module("system"
print = system.out.write_line
suisha = Module("suisha"
text = Module("text"

read = @
	xs = [
	text.parse(system.in.read_line(), @(x) x <= 0x20, xs.push
	xs

suisha.main(@(loop)
	timers = [
	commands = {
		"quit": @(arguments)
			loop.exit(
		"echo": @(arguments)
			print(arguments
		"post": @(arguments)
			loop.post(@
				print("posted: " + arguments
		"in": @(arguments)
			loop.timer(@
				print("single: " + arguments
			, Integer(arguments.shift()), true
		"every": @(arguments)
			timers.push(loop.timer(@
				print("repeat: " + arguments
			, Integer(arguments.shift(
		"stop": @(arguments)
			timers.shift().stop(
	wait = loop.wait
	loop.wait = @
		print("before wait"
		wait(
		print("after wait"
	loop.poll(0, true, false, @(readable, writable) if readable
		line = read(
		if line.size() > 0
			command = line.shift(
			if commands.has(command)
				commands[command](line
			else
				print("unknown command: " + command
		else
			loop.exit(
	system.native_in.blocking__(false
	loop.run(
