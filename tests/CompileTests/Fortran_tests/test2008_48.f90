module assign_pointer_class
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
end module assign_pointer_class

