# test list comprehensions

print [x**2 for x in (1, 2, 3, 4, 5)]

print [x+y for x in (6, 7, 8) for y in (10, 20, 30)]

print [x+y+z for x in (2, 3, 4) for y in (10, 20, 30) for z in (100, 200, 300)]

print [x**2 for x in (0, 1, 2, 3) if x]

print [x**2 for x in (0, 1, 2, 3, 4) if x % 2]

print [x**2 for x in (0, 1, 2, 3, 4, 5, 6, 7) if x % 2 if x % 3]

print [(x+y)**2 for x in (0, 1, 2, 3, 4, 5, 6, 7) for y in (10, 20, 30, 40, 50) if x % 2 if x % 3]
