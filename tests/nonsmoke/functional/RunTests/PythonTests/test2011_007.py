# test 'continue' stmt

for x in (4, 5, 6):
    print x
    continue
    print x

y = 3
while y:
    y -= 1
    print y
    continue
    print y
