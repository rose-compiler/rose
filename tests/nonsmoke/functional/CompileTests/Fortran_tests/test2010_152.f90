! P03-complicated-test-1.f90
! The ROSE front end will fail an assertion under the following
! detailed conditions. All of these must hold to provoke the bug:
!
! (1) A derived type t1 must be declared before a derived type t2.
! (2) Type t1 must have a component f of type 'pointer to array of t2'.
! (3) Types t1 and t2 must each have a component c with the same name,
!     where c != f.
! (4) Type t2 must have an additional component d.
! (5) Following the type declarations must be two statements, each
!     referencing a component of t2 by indexing t1's component f, where:
!     (a) the first data reference accesses t2's additional component d
!     (b) the second data reference accesses t2's same-named component c.
!
! NOTE: this is the same as P04, but with the data references in 5(a)
! and 5(b) interchanged.

program p

   type :: t1
      integer :: c                   
      type (t2), pointer, dimension(:)  :: f
   end type

   type :: t2
      integer :: c
      integer :: d
   end type
 
   type (t1) :: v   

   v % f(1) % d = 0
   v % f(1) % c  = 0    ! assertion failure: expression stack not empty

end program

