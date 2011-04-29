! P05-complicated-test-3.f90
! The ROSE front end will fail an assertion under the following
! detailed conditions. All of these must hold to provoke the bug:
!
! (1) A derived type t2 must be declared after a derived type t1.
! (2) Type t2 must have a component f of type 'pointer to array of t1'.
! (3) Following the type declarations must be a statement referencing
!     a component of t2 by indexing t1's component f.

program p
  
   type :: t1
      integer ::  i
   end type

   type :: t2
      type (t1), pointer, dimension(:) :: f
   end type

   type (t2) :: v

   v % f(1) = t1(1)  ! assertion failure: declaration != null.
   
end program
