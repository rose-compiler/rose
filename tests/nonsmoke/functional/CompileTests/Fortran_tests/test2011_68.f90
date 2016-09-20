      ZERO: SELECT CASE(N)

        CASE DEFAULT ZERO
             OTHER: SELECT CASE(N) ! start of CASE construct OTHER
                CASE(:-1)
                   SIGNUM = -1     ! this statement executed when n<=-1
                CASE(1:) OTHER
                    SIGNUM = 1
             END SELECT OTHER      ! end of CASE construct OTHER
        CASE (0)
          SIGNUM = 0

      END SELECT ZERO
      END

