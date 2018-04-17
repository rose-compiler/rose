
 module netcdf

  implicit none
  private

  ! Dimension routines
  public :: nf90_def_dim
  
  ! Dimension routines   nf_inq_dim
  integer,              external :: nf_def_dim

contains

! This error only happens when using the Fortran include mechanism.
  include "test2010_31_header.f90"

end module netcdf
