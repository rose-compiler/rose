
      subroutine getnextln()
!     get line line_num from the buffered input file

      implicit none
      integer main
               if (main .gt. 0) then
                  rewind(main)
               endif
      return

 20   format (a120)

      end subroutine getnextln
