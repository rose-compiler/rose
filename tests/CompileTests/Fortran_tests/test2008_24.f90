subroutine testFunction
     type nums
          integer scalarStart
          integer array1(2)
          integer array2(2)
          integer scalarEnd
     end type nums

     type (nums),parameter::con22=nums(0,(/1,2/),(/3,4/),5)
end
