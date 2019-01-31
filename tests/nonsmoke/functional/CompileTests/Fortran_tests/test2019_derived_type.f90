module test_type_attr_spec

  type, bind(c) :: bind_c_struct
     integer :: ivar
  end type

  type :: default_struct
  end type

  type, private :: private_struct
  end type

  type, public :: public_struct
  end type

end module
