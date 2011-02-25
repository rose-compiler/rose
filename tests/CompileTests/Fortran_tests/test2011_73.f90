! illegal-forward-ref-type.f90
! An illegal forward reference to a derived type causes the front end to crash.
! Specifically, the Java VM detects a SIGSEGV in libstdc++ in '__dynamic_cast'.

program p

  type(t2) :: y  ! illegal forward reference
 
  type :: t2
    real ::  x
  end type

end program
