10 IF (x == y) THEN
  x = 1
11 ELSE IF (x < y) THEN
  x = 0
12 ELSE
  x = -1
13 END IF
IF (x == y) THEN
  x = 1
ELSE
  x = -1
END IF
IF (x == y) THEN
  x = 1
END IF
my_if: IF (x == y) THEN
  x = 1
ELSE IF (x < y) THEN my_if
  x = 0
ELSE my_if
  x = -1
END IF my_if
END PROGRAM
