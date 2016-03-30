LOGICAL :: a, b, c
a = .TRUE.
a = .TRUE._4
b = .FALSE.
b = .FALSE._1
a = .NOT. b
a = b .AND. c
a = a .AND. b .AND. c
a = .NOT. a .AND. b .AND. .NOT. c
END PROGRAM
