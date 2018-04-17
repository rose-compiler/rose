# test else clause for while stmts

while False:
    print "body"
else:
    print "else_body"

x = 3
while x > 0:
    x -= 1
    y = 0
    while y < x:
        print y
        y += 1
    else:
        print x, x, x
else:
    print x, x
