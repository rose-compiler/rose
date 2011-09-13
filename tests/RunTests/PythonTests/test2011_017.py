# test function decorators

def foo(fn):
    print "foo"
    return fn

def bar(fn):
    print "bar"
    return fn

@foo
@bar
def quux():
    print "quux"

quux()
