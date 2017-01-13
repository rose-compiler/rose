def foo0(*args):
    print args
def bar0(**kwargs):
    print kwargs
def baz0(*args, **kwargs):
    print args, kwargs
def foo1(x, *args):
    print x, args
def bar1(x, **kwargs):
    print x, kwargs
def baz1(x, *args, **kwargs):
    print x, args, kwargs
def foo2(x, y, *args):
    print x, y, args
def bar2(x, y, **kwargs):
    print x, y, kwargs
def baz2(x, y, *args, **kwargs):
    print x, y, args, kwargs

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

args = (1, 2, 3)
kwargs = {'four': 4,
          'five': 5,
          'six': 6}

foo0(*args)
bar0(**kwargs)
baz0(*args, **kwargs)

foo1(8, *args)
bar1(9, **kwargs)
baz1(10, *args, **kwargs)

foo2(11, 115, *args)
bar2(12, 125, **kwargs)
baz2(13, 135, *args, **kwargs)
