! Test procedure-stmt
!      procedure-stmt  is  [ MODULE ] PROCEDURE procedure-name-list
!
! Not tested here: procedure-name-list.
!
! procedure-stmt tested as part of an interface-specification.
interface
   ! First without the optional MODULE
   procedure a
   procedure a, b
   
   ! Now with the MODULE
   module procedure c
   module procedure d, e
end interface

end


