class num():
    def __init__(self, n):
        self.n = n
        print "init", self.n
    def __enter__(self):
        print "enter", self.n
        return (self.n, self.n**2)
    def __exit__(self, type, value, traceback):
        print "exit", self.n
        pass

# simple (non-targetted) with-stmt
with num(0):
    print "simple"

# targetted with-stmt
with num(1) as (one, one_squared):
    print one, one_squared

# multiple, targetted with-stmt
with num(2) as (two, two_squared), num(3) as three:
    print two, two_squared, three

# mixed with-stmt
with num(4) as four, num(5), num(6) as six:
    print four, six
