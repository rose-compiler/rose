subroutine foo
	type mytype
    ! This does not generates a call to R510 so it is not clear how to know that this is a 2D array of undefined size size ":,:"
      integer, pointer:: pp(:,:)
   end type mytype
end subroutine foo
