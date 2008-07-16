! Test attr-spec, which is:
!      attr-spec  is  access-spec 
!                 or  ALLOCATABLE
!                 or  ASYNCHRONOUS
!                 or  DIMENSION ( array-spec )
!                 or  EXTERNAL
!                 or  INTENT ( intent-spec )
!                 or  INTRINSIC
!                 or  language-binding-spec
!                 or  OPTIONAL
!                 or  PARAMETER
!                 or  POINTER
!                 or  PROTECTED
!                 or  SAVE
!                 or  TARGET
!                 or  VALUE
!                 or  VOLATILE
! 
! Tested separately are: R508 (access-spec), R510 (array-spec), 
! R517 (intent-spec), and R509 (language-binding-spec).
!
! attr-spec is tested as part of a type-declaration-stmt (R501).
integer, private :: a       
integer, allocatable :: b   
integer, asynchronous :: c  
integer, dimension(:) :: d  
integer, external :: e      
integer, intent(in) :: f    
integer, intrinsic :: g     
integer, bind(c) :: h
integer, optional :: i
integer, parameter :: j
integer, pointer :: k
integer, protected :: l
integer, save :: m
integer, target :: n
integer, value :: o
integer, volatile :: p

end

