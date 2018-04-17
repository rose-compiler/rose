DO i = 1, 10
  20 CYCLE 
END DO 
my_do: DO i = 1, 10
  CYCLE my_do
END DO my_do
END PROGRAM
