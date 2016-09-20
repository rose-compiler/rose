program main
  implicit none

! DQ (9/19/2011): Changed "real*PS" to "real*8" to simplify use as test code in
! ROSE and included contents of include file to simplify testing to a single file.
! include "headers.inc"
! include "test2011_imperial_crayPointers.h"
! common / pointers /  p_x(10)

 integer*8 p_x

 real*8 x

	! cray pointer (used for aliasing x with single levels of multigrid)
	pointer ( px, x )

	integer(4) :: i, j
	integer(4), parameter :: nlevel = 10

	! sizes of each level of the multigrid
	integer(4), dimension(nlevel) :: sizes
	
	! set multigrid sizes
	sizes(1) = 10
	sizes(2) = 100
	sizes(3) = 145
	sizes(4) = 290
	sizes(5) = 3300
	sizes(6) = 240
	sizes(7) = 25
	sizes(8) = 72
	sizes(9) = 99000
	sizes(10) = 10000	
	
	! fill up all levels of p_x and all positions (via C malloc)
	do i = 1, nlevel
		call allocate ( p_x(i), sizes(i) )
	end do

	do i = 1, nlevel
	
		! alias x to level i-th of p_x
		px = p_x ( i )
	
		! now x is an alias for level i-th of multilevel grid represented by p_x
		call fun ( x, sizes(i) )
	
	end do

! the following tests the actual modification of the output
!	do i = 1, nlevel

		! alias x to level i-th of p_x
!		px = p_x ( i )
		
!		call myprint ( x, sizes(i) )
		
!	end do


end program main

subroutine allocate ( ptr, len )

	implicit none

	! reference to C malloc wrapper (see mymalloc.c)
  external    mymalloc
  integer*8 mymalloc

	! formal parameters
  integer*8 ptr

	integer(4) len
	
	integer*8 bytenum


	bytenum = 8 * len

	! allocates via C malloc a real array of length len
	ptr = mymalloc ( bytenum )

end subroutine allocate


! simple assignment
subroutine fun ( x, len )

	implicit none

	! formal parameters
	integer(4) :: len
	real*8 :: x(len)
	
	! local counter
	integer(4) :: i
	
	! dummy assignment
	do i = 1, len
		x(i) = i
	end do

end subroutine fun

subroutine myprint ( x, len )

	! formal parameters
	integer(4) :: len
	real*8 :: x(len)
	
	! local counter
	integer(4) :: i
	
	! dummy assignment
	do i = 1, len
	  print *, x(i)
	end do

end subroutine myprint
