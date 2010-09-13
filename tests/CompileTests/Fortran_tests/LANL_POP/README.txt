The LANL POP code is a 75K line F90 Ocean climate simulation code that 
we have permission to use as part of tests of ROSE Fortran support.

There are currently a few locations in the code where code is
commented out becuase it was a problem for ROSE. This was done 
because the fix was not clear and so we moved on to identify
the scope of the problems associated with the Fortran 90 support 
in ROSE.

At present ROSE compiles about all of the POP code.  Specific tests include:
   1) Parsing (done and in place with the "make check" rule and thus part of commit tests)
   2) Compiling for analysis; this builds the AST and does any unparsing as required for 
      construction of *.rmod files to handle module support and use statements.  This
      step is the most recent accomplishment.  ROSE can compile POP for analysis
      with the exception of a dozen lines of code that are commented out (some code is 
      repeated multiple times); details below in point 1-9.
   3) Unparsing to generate new code (not tested yet)
   4) Compiling the generated (unparsed) code to verify that it will compile using the
      gfortran compiler (not tested yet).

Commented out parts of POP files required to compile using ROSE:

1) File: mpif.h
   ROSE calles the gfortran syntax checking which reports that REAL*8 is not standard for F90.
 This is not a ROSE specific issue, but since we use gfortran for syntax checking of input
code it is a problem to address at some point.  This is mostly a strictness issue in gfortran.

     ! DQ (8/5/2010): Bug in use of gfortran for syntax checking of input to ROSE.
     ! REAL*8 MPI_WTIME, MPI_WTICK
     ! REAL*8 PMPI_WTIME, PMPI_WTICK
       REAL MPI_WTIME, MPI_WTICK
       REAL PMPI_WTIME, PMPI_WTICK


2) Files: netcdf_eightbyte.f90, netcdf_expanded.f90, netcdf_text_variables.f90
This problem happend in 3 files and is the same problem in each file (in multiple
    locations in each file).  It is related to implicit do loops and I don't
currently understand the problem.

! DQ (9/11/2010): I don't understand this code well enough, so comment out for now!
!    localMap   (:numDims  ) = (/ 1, (product(localCount(:counter)), counter = 1, numDims - 1) /)
 

3) File: io_netcdf.F90
  I don't understand this error, but with these 5 locations in this file commented out the
whole file compiles cleanly with ROSE.  So this will be revisited later.

  This code fragment is commented out in only one location:
#if 0
! DQ ((9/12/2010): Comment this out as a test.
            iostat = NF90_INQ_DIMID(ncid=ncid,                         &
                                 name=trim(io_field%field_dim(n)%name),&
                                 dimid=dimid)
#endif

  This code fragment is commented out in 4 locations:
#if 0
! DQ (9/12/2010): Comment this out as a test.
               iostat = NF90_DEF_DIM (ncid=ncid,                    &
                             name=trim(io_field%field_dim(n)%name), &
                             len=io_field%field_dim(n)%length,      &
                             dimid=io_field%field_dim(n)%id)
#endif


4) File: io_binary.F90
   The newer inquire support is not finished yet.

! DQ (9/12/2010): Commented out incomplete handling of inqure support.
!     inquire(file=path, exist=header_exists)


5) File: ROSE file: src/frontend/SageIII/attachPreprocessingInfoTraversal.C
   Unclear what sort of error this might be, does not happen in C/C++ and only in this
   specific (large) fortran 90 file.

     // DQ (9/12/2010): This is something caught in compiling the Fortran LLNL_POP code file: prognostic.F90
     // ROSE_ASSERT(false);
        printf ("Skipping abort in processing a Fortran LLNL_POP code file: prognostic.F90 (unclear how to handle this error, if it is an error) \n");
        break;


6) File: forcing_shf.F90
   This code causes a symbol to be removed and is caught as an error.

! DQ (9/12/2010): Commented out because this is an error in ROSE.
!      call sw_absorb_frac(zw(k),sw_absorb(k))


7) File: vmix_kpp.F90
   This appears to be a problem similar as to that in #6 (above).
! DQ (9/12/2010): Commented out uses of "km" because this is an error in ROSE.
!      bckgrnd_vdc(k) = bckgrnd_vdc1 + bckgrnd_vdc2* &
!                       atan(bckgrnd_vdc_linv*       &
!                            (zw(k)-bckgrnd_vdc_dpth))

AND

! DQ (9/12/2010): Commented out uses of "km" because this is an error in ROSE.
!  zgrid(km+1) = -zw(km)


8) Another problem is that aliased symbols appear to be growing exponentially, so I need
   to check this soon.  I think that including them from multiple modules is causing them 
   to be redundentaly entered into the symbol tables and this is a mistake (but it does
   not fail; it just represents a performance issue in the compiling (especially with
   internal debugging tunred on).  This expoential problem is a serious issue and
   causes it to take an hour or so to compile the last third of the POP code.  So this 
   is the most important bug in ROSE currently and will get fixed next.
   We also can't include compiling POP into the "make check" rule until we fix this 
   performance problem.

   In general we should likely report the symbol table sizes to more easily track this
   problem in the generated reports from -rose:verbose level 2.

9) File: vertical_mix.F90
   This appears to be a problem similar as to that in #6 (above).
   
! DQ (9/12/2010): Commented out uses of "km" because this is an error in ROSE.
!            VTFB = merge( -bottom_heat_flx, VTFB,      &
!                         k == KMT(:,:,bid) .and.       &
!                         zw(k) >= bottom_heat_flx_depth)


