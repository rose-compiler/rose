! This example passes with gfortran (with the -fcray-pointer option)
! but fails with OFP.
subroutine example

     integer i_array(10)

   ! The size of ipt does not match the size required for it to be a pointer 
   ! so this causes a warning in gfortran. It is suggested that the declaration
   ! for the pointer not be explicit so that the compiler can set the type correctly.
   ! So do not explicitly declare "i_pointer"
   ! integer ipt
     pointer (i_pointer, i_array)

end subroutine example

