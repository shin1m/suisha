system = Module("system"
print = system.out.write_line
io = Module("io"
os = Module("os"
assert = @(x) x || throw Throwable("Assertion failed."
suisha = Module("suisha"

assert(suisha.loop() === null
suisha.main(@(loop)
	assert(suisha.loop() === loop
assert(suisha.loop() === null

setup = @(loop)
	log = [
	wait = loop.wait
	loop.wait = @
		log.push("wait"
		wait(
	log

suisha.main(@(loop)
	log = setup(loop
	loop.post(@
		log.push("post"
		loop.exit(
	loop.run(
	assert(log == [
		"wait"
		"post"
suisha.main(@(loop)
	log = setup(loop
	t = Thread(@
		loop.post(@
			log.push("post"
			loop.exit(
	loop.run(
	t.join(
	assert(log == [
		"wait"
		"post"
suisha.main(@(loop)
	log = setup(loop
	fds = os.pipe(
	reader = io.Reader(io.File(fds[0], "r"), "utf-8"
	try
		loop.poll(fds[0], true, false, @(readable, writable) if readable
			log.push(reader.read_line(
			loop.exit(
		t = Thread(@
			writer = io.Writer(io.File(fds[1], "w"), "utf-8"
			try
				writer.write_line("poll"
			finally
				writer.close(
		loop.run(
		t.join(
	finally
		reader.close(
	assert(log == [
		"wait"
		"poll\n"
suisha.main(@(loop)
	log = setup(loop
	fds = os.pipe(
	writer = io.Writer(io.File(fds[1], "w"), "utf-8"
	try
		loop.poll(fds[1], false, true, @(readable, writable) if writable
			writer.write_line("poll"
			loop.exit(
		log.share(
		t = Thread(@
			file = io.File(fds[0], "r"
			file.blocking__(false
			reader = io.Reader(file, "utf-8"
			try
				log.push(reader.read_line(
			finally
				reader.close(
		loop.run(
		t.join(
	finally
		writer.close(
	assert(log == [
		"wait"
		"poll\n"
suisha.main(@(loop)
	log = setup(loop
	loop.timer(@
		log.push("timer"
		loop.exit(
	, 100, true
	loop.run(
	assert(log == [
		"wait"
		"timer"
suisha.main(@(loop)
	log = setup(loop
	n = 0
	loop.timer(@
		log.push("timer"
		:n = n + 1
		n < 3 || loop.exit(
	, 100
	loop.run(
	assert(log == [
		"wait"
		"timer"
		"wait"
		"timer"
		"wait"
		"timer"
