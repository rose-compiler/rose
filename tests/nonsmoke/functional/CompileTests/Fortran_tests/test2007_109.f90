 
       SUBROUTINE LOOKUP(INDEX, RECORD)  
       INTEGER INDEX 
       REAL  RECORD 
       LOGICAL OPENED  
       SAVE OPENED 
       DATA OPENED / .FALSE. /  
     ! On first call OPENED is false so open the file. 
       IF(.NOT. OPENED) THEN 
            OPEN(UNIT=57, FILE='HIDDEN.DAT', STATUS='OLD', ACCESS='DIRECT', RECL=100) 
            OPENED = .TRUE. 
       END IF 
       READ(UNIT=57, REC=INDEX) RECORD 
       END

