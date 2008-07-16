module mytype_module
  type mytype
     private 
     complex value
     logical exact
  end type mytype

  interface mytype
     module procedure int_to_mytype
  end interface
  ! Operator definitions etc.
contains
  type(mytype) function int_to_mytype(i)
    integer, intent(in) :: i
    int_to_mytype%value = i
    int_to_mytype%exact = .true.
  end function int_to_mytype
  ! Procedures to support operators etc.

end

program example
  use mytype_module
  type(mytype) x
  x = mytype(17)
end
