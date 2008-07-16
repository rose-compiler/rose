module ftn_c_1
  use, intrinsic :: iso_c_binding
end module ftn_c_1

module ftn_c_2
  interface
     ! isn't this an error, since c_int won't be defined in time because it's
     ! not defined until the use ftn_c_1 below??
     integer(c_int) function c_library_function &
          (sendbuf, sendcount, recvcounts) &
          bind(c, name='C_Library_function')
       use ftn_c_1
       implicit none
       type(c_ptr), value :: sendbuf
       integer(c_int), value :: sendcount
       type(c_ptr), value :: recvcounts
     end function c_library_function
  end interface
end module ftn_c_2

use, intrinsic :: iso_c_binding, only: c_int, c_float, c_loc
use ftn_c_2

real(c_float), target :: send(100)
integer(c_int) :: sendcount
integer(c_int), allocatable, target :: recvcounts(100)

allocate( recvcounts(100) )

call c_library_function(c_loc(send), sendcoutn, &
     c_loc(recvcounts))

end
