# test 'yield'

def foo():
    for y in (1, 2, 3):
        yield y

for x in foo():
    print x

print "done"
