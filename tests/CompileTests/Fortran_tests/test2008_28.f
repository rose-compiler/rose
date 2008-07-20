      program cg
      integer nx
      parameter( nx = 7 )

      common / main_int_mem / colidx, rowstr
      integer colidx(nx), rowstr(8)

      call conj_grad ( colidx, nx )

      end program cg

!      subroutine conj_grad ( colidx, nx )
!          integer colidx(nx)
!          integer nx
!          return
!      end subroutine conj_grad

