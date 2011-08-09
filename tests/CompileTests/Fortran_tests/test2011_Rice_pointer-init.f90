PROGRAM pointer_init
INTEGER, PARAMETER :: SHORT = SELECTED_INT_KIND (4) 
  REAL, POINTER :: gp => NULL() , x
  x => gp
  x => NULL()
END
