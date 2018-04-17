! Cycle lacks a label
program main
L1: DO I = 1, 10
   L2: DO J = 1, 10
          IF (I>J) CYCLE L1
       END DO L2
      END DO L1
end program
