globals = {'foo': 1337, 'baz': 65536}
locals = {'bar': 42}

exec "print 31415"
exec "print foo" in globals
exec "print bar, baz" in globals, locals
