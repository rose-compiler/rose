      integer M,I
      IF( M .LT. 4 ) THEN
         I = 3
      ELSE IF( M .LT. 2 ) THEN
         I = 4
         I = 4
      ELSE IF( M .LT. 0 ) THEN
         I = 5
         I = 5
      ELSE
         I = 5
         I = 5
      END IF

      IF( M .LT. 4 ) I = 3

      IF( M .LT. 4 ) I = 3; I = 4

      IF( M .LT. 4 ) THEN
         I = 3
         I = 3
         I = 3
         I = 3
      END IF

      END
