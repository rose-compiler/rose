subroutine init_io

   integer ::  nml_error     ! namelist i/o error flag
   character (8) :: logdate
   character (10) :: logtime ! wallclock time at model startup

   character :: char_tmp     ! temp character string for filenames

   character (6), parameter :: nml_filename = 'pop_in'  ! namelist input file name

   namelist /io_nml/ num_iotasks,                          &
                     lredirect_stdout, log_filename,       &
                     luse_pointer_files, pointer_filename

!-----------------------------------------------------------------------
!
!  initialize io unit manager
!
!-----------------------------------------------------------------------

!   in_use = .false.                  ! no unit in use

!   in_use(stdin) = .true.           ! reserved units
!   in_use(stdout) = .true.
!   in_use(stderr) = .true.
!   in_use(nml_in) = .true.

!-----------------------------------------------------------------------
!
!  read and define namelist inputs
!
!-----------------------------------------------------------------------

!   lredirect_stdout = .false.
!   log_filename = 'pop.out'
!   luse_pointer_files = .false.
!   pointer_filename = 'pop_pointer'
!   num_iotasks = 1         ! set default num io tasks

 !  if (my_task == master_task) then
      open (nml_in, file=nml_filename, status='old',iostat=nml_error)
 !     if (nml_error /= 0) then
 !        nml_error = -1
 !     else
 !        nml_error =  1
 !     endif
      do while (nml_error > 0)
         read(nml_in, nml=io_nml,iostat=nml_error)
      end do
      if (nml_error == 0) close(nml_in)
 !  endif

 end subroutine init_io
