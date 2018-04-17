! example of IMPORT statement
use, intrinsic :: ISO_C_BINDING
interface
! It is an error to use a "," before the bind attribute.
!  subroutine process_buffer(buffer, n_bytes), bind(C,NAME="ProcessBuffer")
   subroutine process_buffer(buffer, n_bytes) bind(C,NAME="ProcessBuffer")
!    IMPORT :: C_PTR, C_INT
     type (C_PTR), value :: buffer
     integer (C_INT), value :: n_bytes
   end subroutine process_buffer
end interface
