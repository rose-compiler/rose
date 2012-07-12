# test precedence

print 1 + 2 * 3 - 4 ** 5
print (1 + 2) * 3 - 4 ** 5
print (1 + 2 * 3) - 4 ** 5
print (1 + 2 * 3 - 4) ** 5
print (1 + 2 * 3 - 4 ** 5)
print (1 + 2) * (3 - 4) ** 5
print (1 + 2 * 3) - (4 ** 5)
print 1 + (2 * 3) - (4 ** 5)
print ((1 + 2) * 3 - 4) ** 5
print (((1 + 2) * 3) - 4) ** 5
print ((1 + 2) * 3) - 4 ** 5
print 1 + 2 * 3 - 4 ** (2 ** 2) ** 2
