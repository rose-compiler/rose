! Bug report from Rice: 10-procedure-component.f90
! A procedure component in a derived type does not work.

module m96
  type :: t
  contains
    procedure :: p  ! causes an assert(false)
  end type
end module
