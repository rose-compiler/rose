module test2010_47_mod1
   ! The keyword "protected" is specific to F2003.
   ! integer, protected :: val
     integer :: val
     integer :: age

   ! The keyword "protected" is specific to F2003.
   ! protected :: age

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
     use test2010_47_mod1
     implicit none
     integer :: value, his_age
     call set_val(88)
     call set_age(38)

   ! If these variables are not setup at "protected" then they can be accessed
     value = val
     his_age = age

     print *, value, his_age
end program
