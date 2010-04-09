MODULE precision
     EXTERNAL MPI_WTIME
     REAL MPI_WTIME
END MODULE precision

subroutine timer()
  use precision
  implicit none

  real t1wc

! This reference to the function MPI_WTIME() causes an error in ROSE.
  t1wc = MPI_WTIME()

end subroutine timer

