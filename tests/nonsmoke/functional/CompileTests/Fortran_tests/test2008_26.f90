! This test code demonstrates a user-defined type.
! It is important that we test that the "dataType"
! is properly built into a dirived data type IR node
! (it should not be a variable, for example).
subroutine testFunction
     type dataType
          integer scalarStart
          integer scalarEnd
     end type dataType

     type (dataType),parameter::variableOfdataType = dataType(0,5)
end subroutine testFunction

