! Contributed by Jean Utke
module m_11_58
  type, public :: t1
      integer         :: m1
   end type t1
   type, public :: t2
      type      (t1)   :: t1  ! reuse typename as member name not forbidden in Fortran
   end type t2
end module 

program p 
  use m_11_58
  type(t2) :: anInstance
  print *,'OK'
end program

