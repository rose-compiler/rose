      subroutine dgbco(abd,lda)
      integer lda
      double precision abd(lda,*)

    ! Note that dasum should be a function.
      double precision anorm
!     double precision dasum
      double precision dasum,last
      integer is,j

      anorm = dmax1(anorm,dasum(l,abd(is,j),1))
!     anorm = dmax1(anorm,dasum(abd(is,j)))
!     anorm = dmax1(anorm,dasum(is,j))
      anorm = dmax1(anorm,dasum(is,j))
      anorm = dasum(is,j)

      return
      end
