   MODULE A
     REAL :: X=5.0
   END MODULE A
   MODULE B
     USE A
     PRIVATE :: X               !  X cannot be accessed through module B
     REAL :: C=80, D=50
   END MODULE B
   PROGRAM TEST
     INTEGER :: TX=7
     CALL SUB
     CONTAINS

     SUBROUTINE SUB
     USE B, ONLY : C
     USE B, T1 => C
     USE B, TX => C             !  C is given another local name
     USE A
     PRINT *, TX                !  Value written is 80 because use-associated
                                !  entity overrides host entity
     END SUBROUTINE
   END
