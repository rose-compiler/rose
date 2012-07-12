# test class decorators

def foo(fn):
    print "foo"
    return fn

def bar(fn):
    print "bar"
    return fn

@foo
@bar
class quux:
    print "quux"

#quux()
