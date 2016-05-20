LOGICAL :: a, b
a = .TRUE. .OR. .FALSE.
a = a .OR. b .OR. .NOT. .FALSE.
END PROGRAM
