LOGICAL :: a, b, c, d
a = b .EQV. c
a = b .NEQV. c
a = a .NEQV. b .EQV. c .AND. d
a = b .AND. c .EQV. .NOT. d
END PROGRAM
