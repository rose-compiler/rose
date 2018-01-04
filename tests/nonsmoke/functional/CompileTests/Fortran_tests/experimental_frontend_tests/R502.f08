!! R502 attr-spec
!                 is  access-spec 
!                 or  ALLOCATABLE
!                 or  ASYNCHRONOUS
!                 or  CODIMENSION [ coarray-spec ]
!                 or  CONTIGUOUS
!                 or  dimension-spec
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
! Tested separately are: R507 (access-spec), R509 (dimension-spec), 
! R523 (intent-spec), and R508 (language-binding-spec).
!

integer, private :: a       
integer, allocatable :: b   
integer, asynchronous :: c  
integer, codimension[*] :: co
integer, CONTIGUOUS :: cont
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

