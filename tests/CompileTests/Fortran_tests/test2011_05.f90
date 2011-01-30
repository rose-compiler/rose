   type :: t1
    ! integer :: c
    ! This is a forward reference to a type that needs to be resolved before processing "v%f(1)%c"
      type (t2), pointer, dimension(:)  :: f
    ! type (t2), pointer :: t2_pointer
    !  real, pointer, dimension(:)  :: g
   end type

 ! Interesting that this is not allowed!
 ! type (t2), pointer, dimension(:)  :: f
 ! real, pointer, dimension(:)  :: g

   type :: t2
       integer :: c
       integer :: d
   end type

end
