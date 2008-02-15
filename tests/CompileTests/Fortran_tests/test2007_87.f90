! Example of failing code demonstrating an inconsistancy in the block 
! where label (it can be a string label, but not a numeric label).
subroutine example
    logical :: a(9)
    real    :: b(9)

  ! where statement with a numeric label is not possible, but a string label works fine.
 10 where (a)
       b = 0
  ! end where MY_LABEL
    end where

end subroutine example
