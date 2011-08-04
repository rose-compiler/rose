def foo0(*args):
    print args
def bar0(**kwargs):
    print kwargs
def baz0(*args, **kwargs):
    print args, kwargs
def foo1(x, *args):
    print args
def bar1(x, **kwargs):
    print kwargs
def baz1(x, *args, **kwargs):
    print args, kwargs
def foo2(x, y, *args):
    print args
def bar2(x, y, **kwargs):
    print kwargs
def baz2(x, y, *args, **kwargs):
    print args, kwargs
(zero, one, two, three, four) = (None, 1, 2.0, 'three', ['four'])
foo0(one, two)
bar0(first=one, second=two)
baz0(one, two, third=three, fourth=four)

foo1(zero, one, two)
bar1(zero, first=one, second=two)
baz1(zero, one, two, third=three, fourth=four)

foo2(zero, four, one, two)
bar2(zero, four, first=one, second=two)
baz2(zero, four, one, two, third=three, fourth=four)
