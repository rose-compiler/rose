      module mod1
          integer, protected :: val
          integer :: age
          protected :: age
          contains
              subroutine set_val(arg)
                  integer arg
                   val = arg
              end subroutine
               subroutine set_age(arg)
                  integer arg
                   age  = arg
              end subroutine
      end module
      program dt_init01
          use mod1
          implicit none
          integer :: value, his_age
          call set_val(88)
          call set_age(38)
          value = val
          his_age = age
          print *, value, his_age
      end program
