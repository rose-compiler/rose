! Computed goto
      INTEGER NEXT
         
      GO TO (100,200) NEXT
10    PRINT *,'Control transfers here if NEXT does not equal 1 or 2'
         

100   PRINT *,'Control transfers here if NEXT = 1'
         

200   PRINT *,'Control transfers here if NEXT = 2'

    ! This second use of the GOTO fails in the new version of OFP/ROSE
      GO TO (100,200) NEXT

      END


