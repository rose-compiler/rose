! include file using double quoted string
include "test2007_46_a.h"

! include file using single quoted string
include 'test2007_46_b.h'

subroutine foobar()
   common /globals/ x

   integer :: y 
end subroutine foobar

#include "test2007_46_c.h"

#line 10 "foobar_header_file"
subroutine foobar_cpp_include()
   integer :: z 
end subroutine foobar_cpp_include

