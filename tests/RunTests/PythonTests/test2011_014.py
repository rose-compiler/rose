# test dictionary comprehensions

print {x: x**2 for x in (1, 2, 3)}
print {x: y**2 for x in (1, 2, 3) for y in (10, 11, 12)}
print {x: x**2 for x in (1, 2, 3, 4, 5, 6, 7, 8) if x % 3}
print {x: x**2 for x in (1, 2, 3, 4, 5, 6, 7, 8) if x % 3 if x % 4}
