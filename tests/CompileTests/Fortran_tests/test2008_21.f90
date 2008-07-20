    subroutine ss
     type nums
      integer scalarStart
      integer array1(2)
      integer array2(2)
      integer scalarEnd
    end type nums

     type nested_nums
      integer i(2)
      integer rr(2)
      type (nums) j
    end type nested_nums

!    type (nums),parameter::con21=nums((/1,2/),(/1.2,2.3/))
!    type (nums),parameter::con22=nums((/1,2/))
!    type (nums),parameter::con22=nums((/1,2/),(/3,4/))
     type (nums),parameter::con22=nums(0,(/1,2/),(/3,4/),5)
!    type (nums),parameter::con22
!    type (nums),parameter::con22=nums((/1,2/))

!    type (nested_nums),parameter::nested=nested_nums((/1,2/),(/3,4/),nums((/5,6/),(/7,8/)))

     integer,parameter::testParameter=1
!    integer,dimension(2),parameter::testParameterArray=(/7,8/)
    end
