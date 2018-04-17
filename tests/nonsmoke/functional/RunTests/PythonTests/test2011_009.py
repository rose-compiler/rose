# test 'yield'

def foo():
    for y in (1, 2, 3):
        yield y

for x in foo():
    print x

print "done1"

# test yield atoms
def bar(value=None):
    while True:
        value = (yield value)

b = bar()
print b.next()
for x in (1, 2, 3, 4, 5):
    print b.send(x**2)

print "done2"
