! Test primary, which is
!      primary  is  constant
!               or  designator
!               or  array-constructor
!               or  structure-constructor
!               or  function-reference
!               or  type-param-inquiry
!               or  type-param-name
!               or  ( expr )
!
! Each alternative is tested separately in the corresponding rule tests.
! primary is tested as an rhs.
a = 3
a = a
a = (/3,3/)
a = b(3,2,1)
a = func(1)
a = b%dim
a = (3)

end

