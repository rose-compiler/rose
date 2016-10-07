subroutine example
    logical :: a(9)
    real    :: b(9)

  ! where statement with a numeric label is not possible, but a string label works fine.
    MY_LABEL: where (a)
       b = 0
    end where MY_LABEL

end subroutine example
