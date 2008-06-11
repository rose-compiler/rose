! This test code for the include mechanism still fails becuase OFP does
! not provide any information about where the include mechnaism is used
! in the code.  I desperately need a c_action function to be called!

! include file using double quoted string
include "test2007_46_a.h"

! include file using single quoted string
include 'test2007_46_b.h'

subroutine foobar()
   common /globals/ x

   integer :: y 
end subroutine foobar

#line 10 "foobar_header_file"
subroutine foobar_cpp_include()
   integer :: z 
end subroutine foobar_cpp_include


