MODULE precision_05
     EXTERNAL MPI_WTIME
     REAL MPI_WTIME
END MODULE precision_05

subroutine timer()
  use precision_05
  implicit none

  real t1wc

! This reference to the function MPI_WTIME() causes an error in ROSE.
  t1wc = MPI_WTIME()

end subroutine timer

