# test function calls

def foo():
    return 1

def bar(first):
    return first

def baz(first, second):
    return first + second

def quux(first, second=1):
    return first + second

def gorply(first=1, second=2):
    return first + second

foo()
bar(1)
baz(2, 3)
quux(4)
quux(4, 5)
gorply()
gorply(2)
gorply(3, 4)
