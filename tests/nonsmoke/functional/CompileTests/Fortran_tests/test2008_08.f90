	subroutine foo
   integer, pointer:: mm(:,:)
	type mytype
      integer i
      integer, pointer:: pp(:,:)
	end type mytype

	type (mytype) tt
	integer,allocatable:: aa(:,:)
   integer bb(4,4)
	tt%pp = aa
	end subroutine foo
