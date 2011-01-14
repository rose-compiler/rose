! Variations of how print and read can be specified
      PRINT 1, PI
      PRINT *, PI

! These statements are a problem!
      READ 1, PI
      READ (*,1) PI

    1 FORMAT (F9.2)
      END
