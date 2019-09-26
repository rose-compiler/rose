 MODULE patch_mod
!===================================================================================================  
  IMPLICIT NONE
  
  TYPE patch_type
    INTEGER  :: color
    CONTAINS
     PROCEDURE :: setup => setup_patch
  END TYPE patch_type
  
!===================================================================================================
  CONTAINS
!=================================================================================================== 

   SUBROUTINE setup_patch(patch_data,color)
    IMPLICIT NONE
    CLASS(patch_type),      INTENT(OUT) :: patch_data
    INTEGER,                INTENT(IN)  :: color
   END SUBROUTINE setup_patch
      
!===================================================================================================
 END MODULE patch_mod
!===================================================================================================
