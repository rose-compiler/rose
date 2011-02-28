! Contributed by Jean Utke to the bug tracker (1/31/2011)
! The unparsed output  has only
! INTERFACE countarray
!    MODULE PROCEDURE c1
! END INTERFACE

interface countarray 
   module procedure c1, c2, cn                                                   
end interface          
