# test set comprehensions

print { x for x in (1, 2, 3) }

print { x*y for x in (1, 2, 3) for y in (10, 20, 30) }

print { x*y for x in (1, 2, 3) for y in (10, 20, 30) if x % 3}

print { x*y for x in (1, 2, 3) for y in (10, 20, 30) if x % 3 if x % 4}
