! ROSE testTranslator drops the pointer attribute if it is declared after the DIMENSION attribute
MODULE pointerAttrBug
  REAL, DIMENSION(:), POINTER :: Y 
! translated code: REAL, DIMENSION(:): Y
END MODULE
 