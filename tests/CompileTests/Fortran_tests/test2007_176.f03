module test2007_176_mod1
     integer, protected :: val
   ! integer :: val
   ! integer, protected :: age
     integer :: age

   ! The bug here is that "protected" as a statement is not equivalent to its specification in the variable declaration.
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
     use test2007_176_mod1
     implicit none
     integer :: value, his_age
     call set_val(88)
     call set_age(38)

   ! This should fail (and does in ROSE) if the declaration used the "protected" attribute (but we don't have the semantic analysis in place).
     value = val

   ! This should fail if the "protected" statement if modified but not if the variable is read only.
     his_age = age

     print *, value, his_age
end program
