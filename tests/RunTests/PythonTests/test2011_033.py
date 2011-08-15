# test if stmts and exps

if 1:
    print "one"

if 2:
    print "two"
else:
    print "three"

if 0:
    print "four"
else:
    print "five"

if 1:
    print "six"
elif 2:
    print "seven"

if 0:
    print "eight"
elif 2:
    print "nine"

if 0+0:
    print "ten"
elif 0*0:
    print "eleven"
else:
    print "twelve"

print 1 if 1 else 2
print 3 if 1 and 2 else 4
print 5 if 0 else 6
print 6*7 if 0 and 1 else 8*9
print 10*11 if 1 and 2 else 12*13
