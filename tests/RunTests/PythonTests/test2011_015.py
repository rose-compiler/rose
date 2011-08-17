print 1 < 2, 2 < 1, 1 < 1
print 1 > 2, 2 > 1, 1 > 1
print 1 <= 2, 2 <= 1, 1 <= 1
print 1 >= 2, 2 >= 1, 1 >= 1
print 1 == 2, 2 == 1, 1 == 1
print 1 != 2, 2 != 1, 1 != 1

print 1 in (1, 2, 3)
print (2,) in [(1,), (2,)]
print 4 in (1, 2, 3)
print (4,) in [(1,), (2,)]

print 1 not in (1, 2, 3)
print (2,) not in [(1,), (2,)]
print 4 not in (1, 2, 3)
print (4,) not in [(1,), (2,)]

a = "someobj"
b = a
c = "someobj"

print a is a
print a is b
print a is c
print b is b
print b is c
print c is c

print a is not a
print a is not b
print a is not c
print b is not b
print b is not c
print c is not c

print a == a
print a == b
print a == c
print b == b
print b == c
print c == c

print a != a
print a != b
print a != c
print b != b
print b != c
print c != c
