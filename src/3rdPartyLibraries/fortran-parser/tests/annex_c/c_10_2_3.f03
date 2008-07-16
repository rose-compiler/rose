use, intrinsic :: iso_c_binding

interface
   subroutine process_buffer(buffer, n_bytes) bind(c,name="ProcessBuffer")
     import :: c_ptr, c_int
     type(c_ptr), value :: buffer ! The "C address" of the array buffer
     integer(c_int), value :: n_bytes ! Number of bytes in buffer
   end subroutine process_buffer
end interface

real(r_quad), dimension(:), allocatable, target :: quad_array

call process_buffer(c_loc(quad_array), int(16*size(quad_array),c_int))
! One quead real takes 16 bytes on this processor

end 
