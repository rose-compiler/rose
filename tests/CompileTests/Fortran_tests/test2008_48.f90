! Note that the name "test2008_48_assign_pointer_class" 
! is too long for gfortran error checking (32 characters).
module test2008_48_assign_ptr_class
   type mytype
      private
         real, pointer :: pr
   end type mytype
   interface assignment (=)
      module procedure assign_pointer
   end interface
   contains
      subroutine assign_pointer(this, a)
         type (mytype), intent(out) :: this
         real, target, intent(in) :: a
         this%pr => a
      end subroutine assign_pointer
end module test2008_48_assign_ptr_class

