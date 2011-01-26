 module io_types_08

   type, public :: io_dim
      integer :: id
   end type

 ! Pointer to array of real
  real, pointer, dimension(:) :: g
 ! Array of pointer to real (not unparsed correctly)
 ! -- unparsed as: real, dimension(:) :: h
  type (io_dim), dimension(:), pointer :: h

  type, public :: io_field_desc
    ! integer, dimension(:), pointer :: int_h_in_type
      type (io_dim), dimension(:), pointer :: h_in_type
   end type

end
