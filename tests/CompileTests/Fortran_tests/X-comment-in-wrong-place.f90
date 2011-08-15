! This f90 program compiles and run fine with gfortran/pgf90.
! However ROSE identity translator incorrectly translates the component of an array element of a derived type.
! See comment on line 18.
!
program derivedTypeArrayBug

	type :: ptr
		integer, pointer, dimension(:) :: a
	end type
	
	integer, allocatable, target :: d(:)		
	type (ptr) :: p(1:2)

	allocate(d(1:3))
	do i = 1, 3
	  d(i) = i
	end do 
	
! p(1)%a is incorrectly transformed into p%a(1)
	p(1)%a => d
	
! print 1 2 3:	
	print *, p(1)%a(1:3)
end program 
