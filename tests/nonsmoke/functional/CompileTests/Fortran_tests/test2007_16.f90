subroutine foo(i)
   INTEGER :: I,J
   I=10
   TWO_DIGIT: DO WHILE (I.GE.10)
      J=J+I
!     READ (5,*) I
   END DO TWO_DIGIT
end subroutine


