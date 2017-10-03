!! R526 allocatable-stmt
!   - F2008 addition
!   - allocatable component can be of recursive type
!
program recursive_type
  TYPE NODE        ! Define a "recursive" type
    INTEGER :: value = 0
    TYPE(NODE), POINTER :: next_node => NULL()
  END TYPE NODE

  type(NODE)  :: recursive_t
  allocatable :: recursive_t

end program
