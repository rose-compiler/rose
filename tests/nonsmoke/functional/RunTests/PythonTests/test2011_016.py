try:
  print 1
except:
  print 4
else:
  print 5

def foo():
  print "start foo"
  try:
    print 6
  except:
    print 7
  else:
    print 8
  print "done foo"

foo()

try:
    print 9
except:
    print 10
finally:
    print 11

try:
    print 12
    try:
        print 13
    except:
        print 14
finally:
    print 15

print "done"
