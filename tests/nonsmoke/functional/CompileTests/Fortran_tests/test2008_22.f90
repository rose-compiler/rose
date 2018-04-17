subroutine testFunction

     type nums
!     integer array1(2)
      integer array2(2)
    end type nums

     type nested_nums
!     integer arrayA(2)
      integer arrayB(2)
      type (nums) anotherType
    end type nested_nums

!   type (nested_nums),parameter::nested = nested_nums((/1,2/),(/3,4/),nums((/5,6/),(/7,8/)))
    type (nested_nums),parameter::nested = nested_nums((/3,4/),nums((/7,8/)))

end subroutine testFunction
