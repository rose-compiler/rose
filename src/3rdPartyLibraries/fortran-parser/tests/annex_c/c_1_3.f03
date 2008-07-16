module types
  type base_type
     private               !-- Sets default accessibility
     integer :: i          !-- a private component
     integer, private :: j !-- another private component
     integer, public :: k  !-- a public component
  end type base_type

  type, extends(base_type) :: my_type
     private               !-- Sets default for components declared in my_type
     integer :: l          !-- A private component.
     integer, public :: m  !-- A public component.
  end type my_type

end module types

subroutine sub
  use types
  type(my_type) :: x

  ! this multi-line causes the parser to fail because of the comments 
  ! following the '&'.  the new-line character after the comment is left in 
  ! the stream and is not ignored, so the lexical prepass assumes that is the 
  ! T_EOS.  the newline needs to be ignored in this case, and it probably has 
  ! to be done in FortranStream or FortranLexer, but certainly before the 
  ! FortranLexicalPrepass.
  call another_sub(   &
       x%base_type,   & !-- ok because base_type is a public subobject of x
       x%base_type%k, & !-- ok because x%base_type is ok and has k as a 
                        !-- public component.
       x%k,           & !-- ok because it is a shorthand for x%base_type%k
       x%base_type%i, & !-- Invalid because i is private.
       x%i)             !-- Invalid because it is shorthand for x%base_type%i
end subroutine sub


       
