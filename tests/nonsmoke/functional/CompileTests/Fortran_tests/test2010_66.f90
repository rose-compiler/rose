! DXN: ROSE incorrectly parses the array reference of a derived type component.
! See comment on lines 11-15 below.
program DotExpBug
	
	type :: panelptr
	  integer, pointer :: buff(:)
	end type

	type (panelptr) :: panels

	! ROSE parses panels%buff(2) as a dot expression 
	! whose left hand side is panels and 
	! whose right hand side is buff(2).
	! panels%buff(2) should be parsed as 
	! the array reference to the second element of the dot expression panels%buff.
    panels%buff(2) = 5

end program
