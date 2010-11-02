      program test
      integer j,k, jlast, klast
!$omp parallel
!$omp single
      print *, klast, jlast
!$omp end single copyprivate(jlast, klast)
!$omp end parallel
      end program test
