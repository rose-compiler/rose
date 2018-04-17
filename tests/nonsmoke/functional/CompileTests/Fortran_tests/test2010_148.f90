      function random(ri,ra)
      implicit none
      double precision  random
      Real srandom
      integer ri
      double precision  ra(0:100-1)

      entry srandom(ri,ra)

      return
      end

    ! Error in parsing previous function causes additional parameters ri,ra in parameter list.
      subroutine random_array(y,n,ri,ra)
      implicit none
      integer n
      double precision  y(0:n-1)
      integer ri
      double precision  ra(0:100-1)

      end

