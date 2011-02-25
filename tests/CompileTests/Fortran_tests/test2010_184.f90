  implicit none
  type :: state
!    integer               :: index
!    real, dimension(7:10) :: array
     character(4)          :: string
!    real                  :: floatingPointValue
  end type state

  type(state) :: stateArray(42)

  integer x,y,z
!  stateArray(x)%index = y
!  stateArray(x)%array(y) = z
  stateArray(x)%string = 'A'
!  stateArray(x)%floatingPointValue = 3.14
end

