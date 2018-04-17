! This f90 program compiles and run fine with gfortran/pgf90.
! However ROSE identity translator incorrectly translates the component of an array element of a derived type.
! See comment on line 18.
!
program testptr
	integer, allocatable, target :: d(:)
	type :: ptr
		integer, pointer, dimension(:) :: a
	end type
	type (ptr) :: p(1:2)
	integer :: i

	allocate(d(1:10))
	do i = 1, 10
	  d(i) = i
	end do 
	
! p(1)%a is incorrectly transformed into p%a(1)
	p(1)%a => d
	
	print *, p(1)%a(1:10)
end program 
