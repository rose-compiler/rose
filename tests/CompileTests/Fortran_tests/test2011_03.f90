   type :: t1
      integer :: c
    ! This is a forward reference to a type that needs to be resolved before processing "v%f(1)%c"
      type (t2), pointer, dimension(:)  :: f
      type (t2), pointer :: t2_pointer
   end type

   type :: t2
      integer :: c
      integer :: d
   end type

   type (t1) :: v

   v % f(1) % c = 0    
end
