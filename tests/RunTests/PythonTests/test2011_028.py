# test slicing

x = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h']

print x[1]

print x[ : ]   # 00
print x[ :4]   # 01
print x[1: ]   # 10
print x[1:4]   # 11

print x[ : : ] #000
print x[ : :2] #001
print x[ :6: ] #010
print x[ :6:2] #011
print x[1: : ] #100
print x[1: :2] #101
print x[1:6: ] #110
print x[1:6:2] #111
