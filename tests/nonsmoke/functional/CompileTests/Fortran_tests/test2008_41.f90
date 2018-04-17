   MODULE test2008_41_A
     REAL :: X=5.0
   END MODULE test2008_41_A
   MODULE test2008_41_B
     USE test2008_41_A
     PRIVATE :: X               !  X cannot be accessed through module B
     REAL :: C=80, D=50
   END MODULE test2008_41_B
   PROGRAM TEST
     INTEGER :: TX=7
     CALL SUB
     CONTAINS

     SUBROUTINE SUB
     USE test2008_41_B, ONLY : C
     USE test2008_41_B, T1 => C
     USE test2008_41_B, TX => C             !  C is given another local name
     USE test2008_41_A
     PRINT *, TX                !  Value written is 80 because use-associated
                                !  entity overrides host entity
     END SUBROUTINE
   END
