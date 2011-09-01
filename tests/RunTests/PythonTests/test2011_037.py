# test inheritance lists

class foo:
    def f(self): print 'foo.f'
    def b(self): print 'foo.b'
    def x(self): print 'foo.x'

class bar:
    def f(self): print 'bar.f'
    def b(self): print 'bar.b'
    def y(self): print 'bar.y'

class animal(foo):
    pass

class baz(bar, foo):
    pass

class quux(foo, bar):
    pass

a = animal()
a.f()
a.b()
a.x()

b = baz()
b.f()
b.b()
b.x()
b.y()

q = quux()
q.f()
q.b()
q.x()
q.y()
