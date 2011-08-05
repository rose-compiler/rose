x, y, z = 1, 2, 3

def foo():
    global x
    print 1, x
    x += 10
    print 2, x

def bar():
    global x, y, z
    print 3, y, z
    z *= y
    print 4, y, z

class baz:
    def __init__(self):
        global y
        print 5, y
        y **= 2
        print 6, y

    global x
    y = 1
    print 7, x, y
    y += 1
    print 8, x, y

print 9, x, y, z
foo()
print 10, x, y, z
bar()
print 11, x, y, z
baz()
print 12, x, y, z
