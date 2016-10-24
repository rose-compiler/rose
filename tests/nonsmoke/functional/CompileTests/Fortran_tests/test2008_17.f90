! This appears to be a bug in OFP, the rule for the ":" operator 
! is not generated for an array that is in a type. Or is the point
! that it is communicated to be a 2D array and since it is a 
! pointer to an array the dimensions are required to be undefined?
! If this is true then the behavior between arrays declared in and
! out of a type are significantly different (and this inconsistance
! is a complicating factor in the use of OFP (but I think I can make 
! it work).

subroutine foo
 ! This generates a call to R510 (two calls) so that we know that there a 2D array of undefined size ":,:"
   integer, pointer:: mm(:,:)
	type mytype
    ! This does not generates a call to R510 so it is not clear how to know that this is a 2D array of undefined size size ":,:"
      integer, pointer:: pp(:,:)
   end type mytype
end subroutine foo
