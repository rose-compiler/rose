module openacc

contains

   function acc_is_present (a, len)
      logical :: acc_is_present
      !GCC$ ATTRIBUTES NO_ARG_CHECK :: a                                                                            
      real :: a(*)
      integer :: len
   end function

end module

