# 1 "test2008_05.F90"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "test2008_05.F90"
! include file using double quoted string
include "test2007_46_a.h"

! include file using single quoted string (note: generated code uses double quotes)
include 'test2007_46_b.h'

subroutine foobar()
   common /globals/ x
   integer :: y
end subroutine foobar

# 1 "test2007_46_c.h" 1
subroutine foobar_header_c()
   common /globals/ x
end subroutine foobar_header_c
# 14 "test2008_05.F90" 2
# 10 "foobar_header_file"
subroutine foobar_cpp_include()
   integer :: z
end subroutine foobar_cpp_include
