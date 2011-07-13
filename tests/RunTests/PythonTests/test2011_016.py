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
print "done"
