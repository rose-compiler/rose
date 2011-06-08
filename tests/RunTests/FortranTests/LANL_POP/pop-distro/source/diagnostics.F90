!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module diagnostics

!BOP
! !MODULE: diagnostics
! !DESCRIPTION:
!  This module contains routines and data for producing diagnostic
!  output, including global diagnostics, cfl checks and transport
!  diagnostics.
!
! !REVISION HISTORY:
!  CVS:$Id: diagnostics.F90,v 1.17 2003/12/23 22:11:40 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $
!
! !USES:

   use domain
   use constants
   use prognostic
   use time_management
   use io
   use broadcast
   use global_reductions
   use grid
   use solvers
   use forcing
   use timers
   use exit_mod

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:

   public :: init_diagnostics,        &
             diag_init_sums,          &
             diag_global_preupdate,   &
             diag_global_afterupdate, &
             diag_print,              &
             diag_transport,          &
             cfl_advect,              &
             cfl_vdiff,               &
             cfl_hdiff,               &
             cfl_check,               &
             check_KE

! !PUBLIC DATA MEMBERS:

   logical (log_kind), public :: &
      ldiag_global,          &! time to compute global diagnostics
      ldiag_cfl,             &! time to compute cfl diagnostics
      ldiag_transport,       &! time to compute transport diagnostics
      cfl_all_levels,        &! writes cfl  diags for all vert levels
      diag_all_levels         ! writes some diags for all vert levels

   !***  arrays for holding various diagnostic results
   !***  public for now as they are modified directly in baroclinic

   real (r8), dimension(nx_block,ny_block,max_blocks_clinic), &
      public :: &
      DIAG_KE_ADV_2D,   &!
      DIAG_KE_HMIX_2D,  &!
      DIAG_KE_VMIX_2D,  &!
      DIAG_KE_PRESS_2D, &!
      DIAG_PE_2D

   real (r8), dimension(nx_block,ny_block,nt,max_blocks_clinic), &
      public :: &
      DIAG_TRACER_ADV_2D,    &!
      DIAG_TRACER_HDIFF_2D,  &!
      DIAG_TRACER_VDIFF_2D,  &!
      DIAG_TRACER_SOURCE_2D, &!
      DIAG_TRACER_SFC_FLX

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  variables for controlling frequency and output of diagnostics
!
!-----------------------------------------------------------------------

   integer (int_kind) ::     &
      diag_unit,             &! i/o unit for output diagnostic file
      trans_unit              ! i/o unit for output transport file

   integer (int_kind) ::     &
      diag_global_flag,      &! time flag id for global diags
      diag_cfl_flag,         &! time flag id for cfl    diags
      diag_transp_flag        ! time flag id for transp diags

   character (char_len) ::   &
      diag_outfile,          &! filename for diagnostic output
      diag_transport_outfile  ! filename for transport output

!-----------------------------------------------------------------------
!
!  global diagnostics
!
!-----------------------------------------------------------------------

   real (r8) ::        &
      diag_ke,         &! mean KE at new time
      diag_ke_adv,     &! KE change due to advection
      diag_ke_free,    &! advective KE change due to free surface
      diag_ke_hmix,    &! KE change due to horizontal diffusion
      diag_ke_vmix,    &! KE change due to vertical   diffusion
      diag_ke_press,   &! KE change due to pressure gradient
      diag_pe,         &! change in potential energy
      diag_press_free, &! pressure work contribution from free surface
      diag_ke_psfc,    &! KE change due to surface pressure
      diag_ws,         &! KE change due to wind stress
      diag_sealevel     ! global mean seal level

   real (r8), dimension(nt) :: &
      dtracer_avg,             &! avg tracer change for this time step
      dtracer_abs,             &! abs tracer change for this time step
      diag_tracer_adv,         &! tracer change due to advection
      diag_tracer_hdiff,       &! tracer change due to horiz diffusion
      diag_tracer_vdiff,       &! tracer change due to vert  diffusion
      diag_tracer_source,      &! tracer change due to source terms
      avg_tracer,              &! global average tracer at new time
      sfc_tracer_flux           ! sfc tracer flux (Fw*tracer) or
                                !   resid sfc flux (w*tracer)

   real (r8), dimension(km,nt) :: &
      avg_tracer_k                ! mean tracer at every level

!-----------------------------------------------------------------------
!
!  cfl diagnostics
!
!-----------------------------------------------------------------------

   real (r8), dimension(max_blocks_clinic,km) :: &
      cfl_advuk_block,     &! zonal advective cfl number for level k
      cfl_advvk_block,     &! merid advective cfl number for level k
      cfl_advwk_block,     &! vert  advective cfl number for level k
      cfl_advtk_block,     &! total advective cfl number for level k
      cfl_vdifftk_block,   &! tracer   vert  diff cfl num for level k
      cfl_vdiffuk_block,   &! momentum vert  diff cfl num for level k
      cfl_hdifftk_block,   &! tracer   horiz diff cfl num for level k
      cfl_hdiffuk_block     ! momentum horiz diff cfl num for level k

   integer (int_kind), dimension(2,max_blocks_clinic,km) :: &
      cfladd_advuk_block,       &! horiz addresses for max cfl numbers
      cfladd_advvk_block,       &!   at level k
      cfladd_advwk_block,       &
      cfladd_advtk_block,       &
      cfladd_vdifftk_block,     &
      cfladd_vdiffuk_block,     &
      cfladd_hdifftk_block,     &
      cfladd_hdiffuk_block

!-----------------------------------------------------------------------
!
!  transport diagnostics
!
!-----------------------------------------------------------------------

   type :: transport
      character(char_len) :: name   ! name for transport
      integer (int_kind)  :: type   ! type (meridional or zonal)
      integer (int_kind), dimension(6,max_blocks_clinic) :: &
                             add    ! address range for computing transp
      real (r8)           :: mass, &! mass transport
                             heat, &! heat transport
                             salt   ! salt transport
   end type

   integer (int_kind), parameter :: &
      transport_type_zonal = 1, &! zonal or meridional transport type
      transport_type_merid = 2

   integer (int_kind) :: &
      num_transports      ! number of transport diagnostics computed

   type (transport), dimension(:), allocatable :: &
      transports          ! transport info for all requested transports

!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: init_diagnostics
! !INTERFACE:

 subroutine init_diagnostics

! !DESCRIPTION:
!  Initializes diagnostic module quantities.
!
! !REVISION HISTORY:
!  same as module

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      nu,                &! unit for contents input file
      iblock, n,         &! dummy loop indices
      nml_error,         &! namelist i/o error flag
      ib,ie,jb,je         ! beg,end indices for block physical domain

   integer (int_kind), dimension(6) :: &
      tmp_add             ! transport global addresses
                          ! (ibeg,iend,jbeg,jend,kbeg,kend)

   character (char_len) ::  &
      diag_global_freq_opt, &! choice for freq of global diagnostics
      diag_cfl_freq_opt,    &! choice for freq of cfl diagnostics
      diag_transp_freq_opt, &! choice for freq of transport diagnostics
      diag_transport_file,  &! filename for choosing fields for output
      transport_ctype,      &! type of transport (zonal,merid)
      outfile_tmp            ! temp for appending to outfile name

   integer (int_kind) ::     &
      diag_global_freq_iopt, &! freq option for computing global diags
      diag_global_freq,      &! freq for computing global diagnostics
      diag_cfl_freq_iopt,    &! freq option for computing cfl diags
      diag_cfl_freq,         &! freq for computing cfl diagnostics
      diag_transp_freq_iopt, &! freq option for comp transport diags
      diag_transp_freq        ! freq for computing transp diagnostics

   character (10) ::        &
      cdate                 ! character date

   character (47), parameter :: &! output formats for freq options
      gfreq_fmt = "('Global    diagnostics computed every ',i8,a8)", &
      cfreq_fmt = "('CFL       diagnostics computed every ',i8,a8)", &
      tfreq_fmt = "('Transport diagnostics computed every ',i8,a8)"

   namelist /diagnostics_nml/diag_global_freq_opt, diag_global_freq, &
                             diag_cfl_freq_opt,    diag_cfl_freq,    &
                             diag_transp_freq_opt, diag_transp_freq, &
                             diag_transport_file,                    &
                             diag_all_levels, cfl_all_levels,        &
                             diag_outfile, diag_transport_outfile

   type (block) ::         &
      this_block           ! block information for current block

!-----------------------------------------------------------------------
!
!  read diagnostic file output frequency and filenames from namelist
!
!-----------------------------------------------------------------------

   !*** set default values

   diag_global_freq_opt   = 'never'
   diag_cfl_freq_opt      = 'never'
   diag_transp_freq_opt   = 'never'
   diag_transport_file    = 'unknown_transport_file'
   diag_outfile           = 'unknown_diag_outfile'
   diag_transport_outfile = 'unknown_transport_outfile'
   diag_all_levels        = .false.
   cfl_all_levels         = .false.

   if (my_task == master_task) then
      open (nml_in, file=nml_filename, status='old',iostat=nml_error)
      if (nml_error /= 0) then
         nml_error = -1
      else
         nml_error =  1
      endif
      do while (nml_error > 0)
         read(nml_in, nml=diagnostics_nml,iostat=nml_error)
      end do
      if (nml_error == 0) close(nml_in)
   endif

   call broadcast_scalar(nml_error, master_task)
   if (nml_error /= 0) then
      call exit_POP(sigAbort,'ERROR reading diagnostics_nml')
   endif

!-----------------------------------------------------------------------
!
!  append runid, initial date to output file names
!  concatenation operator must be split across lines to avoid problems
!    with preprocessors
!
!-----------------------------------------------------------------------

   if (date_separator == ' ') then
      cdate(1:4) = cyear
      cdate(5:6) = cmonth
      cdate(7:8) = cday
      cdate(9:10)= '  '
   else
      cdate(1:4) = cyear
      cdate(5:5) = date_separator
      cdate(6:7) = cmonth
      cdate(8:8) = date_separator
      cdate(9:10) = cday
   endif

   outfile_tmp = char_blank
   outfile_tmp = trim(diag_outfile)/&
                                    &/'.'/&
                                    &/trim(runid)/&
                                    &/'.'/&
                                    &/trim(cdate)
   diag_outfile = char_blank
   diag_outfile = trim(outfile_tmp)

   outfile_tmp = char_blank
   outfile_tmp = trim(diag_transport_outfile)/&
                                              &/'.'/&
                                              &/trim(runid)/&
                                              &/'.'/&
                                              &/trim(cdate)
   diag_transport_outfile = char_blank
   diag_transport_outfile = trim(outfile_tmp)

!-----------------------------------------------------------------------
!
!  set, broadcast and print options to log file
!
!-----------------------------------------------------------------------

   if (my_task == master_task) then
      write(stdout,blank_fmt)
      write(stdout,delim_fmt)
      write(stdout,blank_fmt)
      write(stdout,'(a18)') 'Diagnostic options'
      write(stdout,blank_fmt)
      write(stdout,delim_fmt)

      select case (diag_global_freq_opt)
      case ('never')
         diag_global_freq_iopt = freq_opt_never
         write(stdout,'(a31)') 'Global diagnostics not computed'
      case ('nyear')
         diag_global_freq_iopt = freq_opt_nyear
         write(stdout,gfreq_fmt) diag_global_freq, ' years  '
      case ('nmonth')
         diag_global_freq_iopt = freq_opt_nmonth
         write(stdout,gfreq_fmt) diag_global_freq, ' months '
      case ('nday')
         diag_global_freq_iopt = freq_opt_nday
         write(stdout,gfreq_fmt) diag_global_freq, ' days   '
      case ('nhour')
         diag_global_freq_iopt = freq_opt_nhour
         write(stdout,gfreq_fmt) diag_global_freq, ' hours  '
      case ('nsecond')
         diag_global_freq_iopt = freq_opt_nsecond
         write(stdout,gfreq_fmt) diag_global_freq, ' seconds'
      case ('nstep')
         diag_global_freq_iopt = freq_opt_nstep
         write(stdout,gfreq_fmt) diag_global_freq, ' steps  '
      case default
         diag_global_freq_iopt = -1000
      end select

      if (diag_global_freq_iopt /= freq_opt_never) then
         write(stdout,'(a36,a)') &
            'Global diagnostics written to file: ', trim(diag_outfile)
         if (diag_all_levels)     &
            write(stdout,'(a42)') &
               'Diagnostics output for all vertical levels'
      endif

      select case (diag_cfl_freq_opt)
      case ('never')
         diag_cfl_freq_iopt = freq_opt_never
         write(stdout,'(a28)') 'CFL diagnostics not computed'
      case ('nyear')
         diag_cfl_freq_iopt = freq_opt_nyear
         write(stdout,cfreq_fmt) diag_cfl_freq, ' years  '
      case ('nmonth')
         diag_cfl_freq_iopt = freq_opt_nmonth
         write(stdout,cfreq_fmt) diag_cfl_freq, ' months '
      case ('nday')
         diag_cfl_freq_iopt = freq_opt_nday
         write(stdout,cfreq_fmt) diag_cfl_freq, ' days   '
      case ('nhour')
         diag_cfl_freq_iopt = freq_opt_nhour
         write(stdout,cfreq_fmt) diag_cfl_freq, ' hours  '
      case ('nsecond')
         diag_cfl_freq_iopt = freq_opt_nsecond
         write(stdout,cfreq_fmt) diag_cfl_freq, ' seconds'
      case ('nstep')
         diag_cfl_freq_iopt = freq_opt_nstep
         write(stdout,cfreq_fmt) diag_cfl_freq, ' steps  '
      case default
         diag_cfl_freq_iopt = -1000
      end select

      if (diag_cfl_freq_iopt /= freq_opt_never) then
         write(stdout,'(a33,a)') &
            'CFL diagnostics written to file: ',trim(diag_outfile)
         if (cfl_all_levels) then
            write(stdout,'(a46)') &
               'CFL diagnostics output for all vertical levels'
         endif
      endif

      select case (diag_transp_freq_opt)
      case ('never')
         diag_transp_freq_iopt = freq_opt_never
         write(stdout,'(a34)') 'Transport diagnostics not computed'
      case ('nyear')
         diag_transp_freq_iopt = freq_opt_nyear
         write(stdout,tfreq_fmt) diag_transp_freq, ' years  '
      case ('nmonth')
         diag_transp_freq_iopt = freq_opt_nmonth
         write(stdout,tfreq_fmt) diag_transp_freq, ' months '
      case ('nday')
         diag_transp_freq_iopt = freq_opt_nday
         write(stdout,tfreq_fmt) diag_transp_freq, ' days   '
      case ('nhour')
         diag_transp_freq_iopt = freq_opt_nhour
         write(stdout,tfreq_fmt) diag_transp_freq, ' hours  '
      case ('nsecond')
         diag_transp_freq_iopt = freq_opt_nsecond
         write(stdout,tfreq_fmt) diag_transp_freq, ' seconds'
      case ('nstep')
         diag_transp_freq_iopt = freq_opt_nstep
         write(stdout,tfreq_fmt) diag_transp_freq, ' steps  '
      case default
         diag_transp_freq_iopt = -1000
      end select

   endif

   call broadcast_scalar(diag_global_freq_iopt, master_task)
   call broadcast_scalar(diag_global_freq,      master_task)
   call broadcast_scalar(diag_cfl_freq_iopt,    master_task)
   call broadcast_scalar(diag_cfl_freq,         master_task)
   call broadcast_scalar(diag_transp_freq_iopt, master_task)
   call broadcast_scalar(diag_transp_freq,      master_task)
   call broadcast_scalar(diag_all_levels,       master_task)
   call broadcast_scalar(cfl_all_levels,        master_task)

   if (diag_global_freq_iopt == -1000) then
      call exit_POP(sigAbort, &
                    'ERROR: unknown global diag frequency option')
   endif
   if (diag_cfl_freq_iopt == -1000) then
      call exit_POP(sigAbort, &
                    'ERROR: unknown cfl diagnostic frequency option')
   endif
   if (diag_transp_freq_iopt == -1000) then
      call exit_POP(sigAbort, &
                    'ERROR: unknown transport diag frequency option')
   endif

!-----------------------------------------------------------------------
!
!  create time flags for computing diagnostics
!
!-----------------------------------------------------------------------

   diag_global_flag = init_time_flag('diag_global', default=.false.,   &
                                     freq_opt = diag_global_freq_iopt, &
                                     freq     = diag_global_freq)

   diag_cfl_flag    = init_time_flag('diag_cfl', default=.false.,   &
                                     freq_opt = diag_cfl_freq_iopt, &
                                     freq     = diag_cfl_freq)

   diag_transp_flag = init_time_flag('diag_transp', default=.false.,   &
                                     freq_opt = diag_transp_freq_iopt, &
                                     freq     = diag_transp_freq)

!-----------------------------------------------------------------------
!
!  read transports file to determine which transports to compute
!
!-----------------------------------------------------------------------

   if (diag_transp_freq_iopt /= freq_opt_never) then

      call get_unit(nu)
      if (my_task == master_task) then
         write(stdout,'(a39,a)') &
            'Transport diagnostics written to file: ', &
            trim(diag_transport_outfile)
         write(stdout,blank_fmt)

         open(nu, file=diag_transport_file, status='old')
         read(nu,*) num_transports

         write(stdout,'(a14,i4,a27)') 'The following ',num_transports, &
                                    'transports will be computed'
      endif

      call broadcast_scalar(num_transports, master_task)

      allocate( transports(num_transports) )

      do n=1,num_transports
         if (my_task == master_task) then
            read(nu,'(6(i4,1x),a5,2x,a80)') tmp_add, &
                                 transport_ctype, transports(n)%name
            write(stdout,'(a2,a)') '  ',trim(transports(n)%name)

            select case (transport_ctype(1:5))
            case ('zonal')
               transports(n)%type = transport_type_zonal
            case ('merid')
               transports(n)%type = transport_type_merid
            case default
               transports(n)%type = -1000
            end select
         endif

         call broadcast_scalar(transports(n)%type,master_task)
         call broadcast_array(tmp_add,master_task)
         if (transports(n)%type == -1000) then
            call exit_POP(sigAbort,'ERROR: unknown transport type')
         endif

         !***
         !*** compute local addresses for transport
         !***

         !$OMP PARALLEL DO PRIVATE(this_block,ib,ie,jb,je)

         do iblock=1,nblocks_clinic

            this_block = get_block(blocks_clinic(iblock),iblock)  

            ib = this_block%ib
            ie = this_block%ie
            jb = this_block%jb
            je = this_block%je

            transports(n)%add(1,iblock) = nx_block
            transports(n)%add(2,iblock) = 1
            transports(n)%add(3,iblock) = ny_block
            transports(n)%add(4,iblock) = 1
            transports(n)%add(5,iblock) = tmp_add(5)  ! beg k index
            transports(n)%add(6,iblock) = tmp_add(6)  ! end k index

            if (tmp_add(1) <= this_block%i_glob(ie) .and. &
                tmp_add(2) >= this_block%i_glob(ib) ) then

               if (tmp_add(1) >= this_block%i_glob(ib)) then
                  transports(n)%add(1,iblock) = &
                              tmp_add(1) - this_block%i_glob(ib) + ib
               else
                  transports(n)%add(1,iblock) = ib
               endif
               if (tmp_add(2) <= this_block%i_glob(ie)) then
                  transports(n)%add(2,iblock) = &
                              tmp_add(2) - this_block%i_glob(ib) + ib
               else
                  transports(n)%add(2,iblock) = ie
               endif
            endif

            if (tmp_add(3) <= this_block%j_glob(je) .and. &
                tmp_add(4) >= this_block%j_glob(jb) ) then
               if (tmp_add(3) >= this_block%j_glob(jb)) then
                  transports(n)%add(3,iblock) = &
                              tmp_add(3) - this_block%j_glob(jb) + jb
               else
                  transports(n)%add(3,iblock) = jb
               endif
              if (tmp_add(4) <= this_block%j_glob(je)) then
                 transports(n)%add(4,iblock) = &
                              tmp_add(4) - this_block%j_glob(jb) + jb
              else
                 transports(n)%add(4,iblock) = je
              endif
            endif

         end do ! block loop

         !$OMP END PARALLEL DO

      end do

      if (my_task == master_task) close(nu)
      call release_unit(nu)
   endif

!-----------------------------------------------------------------------
!
!  open output files if required, then write a blank and close.
!
!-----------------------------------------------------------------------

   if (diag_global_freq_iopt /= freq_opt_never .or. &
       diag_cfl_freq_iopt    /= freq_opt_never) then

      call get_unit(diag_unit)
      if (my_task == master_task) then
          open(diag_unit, file=diag_outfile, status='unknown')
          write(diag_unit,*)' '
          close(diag_unit)
      endif
   endif

   if (diag_transp_freq_iopt /= freq_opt_never) then
      call get_unit(trans_unit)
      if (my_task == master_task) then
         open(trans_unit, file=diag_transport_outfile,status='unknown')
         write(trans_unit,*)' '
         close(trans_unit)
      endif
   endif

!-----------------------------------------------------------------------
!
!  initialize cfl arrays if required
!
!-----------------------------------------------------------------------

   if (diag_cfl_freq_iopt /= freq_opt_never) then
      cfl_advuk_block       = c0
      cfl_advvk_block       = c0
      cfl_advwk_block       = c0
      cfl_advtk_block       = c0
      cfl_vdifftk_block     = c0
      cfl_vdiffuk_block     = c0
      cfl_hdifftk_block     = c0
      cfl_hdiffuk_block     = c0
      cfladd_advuk_block    = 0
      cfladd_advvk_block    = 0
      cfladd_advwk_block    = 0
      cfladd_advtk_block    = 0
      cfladd_vdifftk_block  = 0
      cfladd_vdiffuk_block  = 0
      cfladd_hdifftk_block  = 0
      cfladd_hdiffuk_block  = 0
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine init_diagnostics

!***********************************************************************
!BOP
! !IROUTINE: diag_init_sums
! !INTERFACE:

 subroutine diag_init_sums

! !DESCRIPTION:
!  Sets diagnostic flags and initializes diagnostic sums at beginning
!  of each step.
!
! !REVISION HISTORY:
!  same as module

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  output formats
!
!-----------------------------------------------------------------------

   character (8), parameter :: &
      time_fmt1 = '(a15,i8)'

   character (11), parameter :: &
      time_fmt2 = '(a15,f14.6)'

   character (20), parameter :: &
      date_fmt1 = '(a15,a2,1x,a3,1x,a4)'

!-----------------------------------------------------------------------
!
!  set logical switches for diagnostic timesteps
!  do not compute on first pass of Matsuno
!
!-----------------------------------------------------------------------

   ldiag_global    = check_time_flag(diag_global_flag)
   ldiag_cfl       = check_time_flag(diag_cfl_flag)
   ldiag_transport = check_time_flag(diag_transp_flag)

   if (mix_pass == 1) then
      ldiag_global    = .false.
      ldiag_cfl       = .false.
      ldiag_transport = .false.
   endif

!-----------------------------------------------------------------------
!
!  initialize arrays for global diagnostic sums.
!
!-----------------------------------------------------------------------

   if (ldiag_global) then
      DIAG_KE_ADV_2D  = c0
      DIAG_KE_HMIX_2D = c0
      DIAG_KE_VMIX_2D = c0
      DIAG_KE_PRESS_2D = c0
      DIAG_PE_2D = c0
      DIAG_TRACER_ADV_2D = c0
      DIAG_TRACER_HDIFF_2D = c0
      DIAG_TRACER_VDIFF_2D = c0
      DIAG_TRACER_SOURCE_2D = c0
      DIAG_TRACER_SFC_FLX   = c0
   endif

!-----------------------------------------------------------------------
!
!  write some stuff to log file (stdout)
!
!-----------------------------------------------------------------------

   if ((ldiag_global .or. ldiag_cfl) .and. &
       my_task == master_task) then
      write(stdout,blank_fmt)
      write(stdout,time_fmt1) 'Step number  : ',nsteps_total
      write(stdout,date_fmt1) 'Date         : ',cday,cmonth3,cyear
      write(stdout,time_fmt1) 'Hour         : ',ihour
      write(stdout,time_fmt1) 'Minute       : ',iminute
      write(stdout,time_fmt1) 'Second       : ',isecond

      if (tsecond < seconds_in_hour) then
         write(stdout,time_fmt2) 'Time(seconds): ', tsecond
      elseif (tsecond < seconds_in_day) then
         write(stdout,time_fmt2) 'Time(hours)  : ', &
                                             tsecond/seconds_in_hour
      elseif (tsecond < 31536000._r8) then
         write(stdout,time_fmt2) 'Time(days)   : ', tday
      else
         write(stdout,time_fmt2) 'Time(years)  : ', tyear
      endif
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine diag_init_sums

!***********************************************************************
!BOP
! !IROUTINE: diag_global_preupdate
! !INTERFACE:

 subroutine diag_global_preupdate(DH,DHU)

! !DESCRIPTION:
!  Computes diagnostic sums for those diagnostics computed from
!  current time prognostic variables (before updating to new time)
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,max_blocks_clinic), &
      intent(in) :: &
      DH, DHU            ! change in surface height (-Fw) at T,U pts

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      i, k,              & ! horiz, vertical level index
      n ,                & ! tracer index
      iblock ,           & ! block index
      dcount ,           & ! diag counter
      ib,ie,jb,je

   real (r8), dimension(max_blocks_clinic,6*nt+9) :: &
      local_sums           ! array for holding block sums
                           ! of each diagnostic

   real (r8) ::            &
      sum_tmp              ! temp for local sum

   real (r8), dimension(nx_block,ny_block) :: &
      WORK1,              &! local work space
      TFACT,              &! factor for normalizing sums
      UFACT                ! factor for normalizing sums

   type (block) ::         &
      this_block           ! block information for current block

!-----------------------------------------------------------------------
!
!  compute these diagnostics only if it is time
!
!-----------------------------------------------------------------------

   if (ldiag_global) then

!-----------------------------------------------------------------------
!
!     compute local sums of each diagnostic
!     do all block sums first for better OpenMP performance
!
!-----------------------------------------------------------------------

      local_sums = c0

      !$OMP PARALLEL DO PRIVATE(this_block, ib, ie, jb, je, i, &
      !$OMP                     k, n, dcount, WORK1, UFACT, TFACT)

      do iblock = 1,nblocks_clinic

         this_block = get_block(blocks_clinic(iblock),iblock)  

         ib = this_block%ib
         ie = this_block%ie
         jb = this_block%jb
         je = this_block%je

         TFACT = TAREA(:,:,iblock)*RCALCT(:,:,iblock)
         UFACT = UAREA(:,:,iblock)*RCALCU(:,:,iblock)
         if (ltripole_grid .and. this_block%jblock == nblocks_y) then
            do i=1,nx_block
               if (this_block%i_glob(i) > nx_global/2) UFACT(i,je) = c0
            end do
         endif

         dcount = 0

         dcount = dcount + 1
         WORK1 = (UVEL(:,:,1,curtime,iblock)*SMF(:,:,1,iblock)   &
                 +VVEL(:,:,1,curtime,iblock)*SMF(:,:,2,iblock))*UFACT
         local_sums(iblock,dcount) = sum(WORK1(ib:ie,jb:je))
 
         dcount = dcount + 1
         WORK1 = DIAG_KE_ADV_2D(:,:,iblock)*UFACT
         local_sums(iblock,dcount) = sum(WORK1(ib:ie,jb:je))

         dcount = dcount + 1
         WORK1 = DIAG_KE_HMIX_2D(:,:,iblock)*UFACT
         local_sums(iblock,dcount) = sum(WORK1(ib:ie,jb:je))

         dcount = dcount + 1
         WORK1 = DIAG_KE_VMIX_2D(:,:,iblock)*UFACT
         local_sums(iblock,dcount) = sum(WORK1(ib:ie,jb:je))

         dcount = dcount + 1
         WORK1 = DIAG_KE_PRESS_2D(:,:,iblock)*UFACT
         local_sums(iblock,dcount) = sum(WORK1(ib:ie,jb:je))

         dcount = dcount + 1
         WORK1 = DIAG_PE_2D(:,:,iblock)*TFACT
         local_sums(iblock,dcount) = sum(WORK1(ib:ie,jb:je))

         dcount = dcount + 1
         WORK1 = -(UBTROP(:,:,curtime,iblock)*  &
                   GRADPX(:,:,curtime,iblock) + &
                   VBTROP(:,:,curtime,iblock)*  &
                   GRADPY(:,:,curtime,iblock))* &
                   HU(:,:,iblock)*UFACT
         local_sums(iblock,dcount) = sum(WORK1(ib:ie,jb:je))

         dcount = dcount + 1
         WORK1 = p5*(UVEL(:,:,1,curtime,iblock)**2 + &
                     VVEL(:,:,1,curtime,iblock)**2)* &
                 DHU(:,:,iblock)*UFACT
         local_sums(iblock,dcount) = sum(WORK1(ib:ie,jb:je))

         dcount = dcount + 1
         WORK1 = PSURF(:,:,curtime,iblock)*DH(:,:,iblock)*TFACT
         local_sums(iblock,dcount) = sum(WORK1(ib:ie,jb:je))

         do n=1,nt
            dcount = dcount + 1
            WORK1 = DIAG_TRACER_ADV_2D(:,:,n,iblock)*TFACT
            local_sums(iblock,dcount) = sum(WORK1(ib:ie,jb:je))

            dcount = dcount + 1
            WORK1 = DIAG_TRACER_HDIFF_2D(:,:,n,iblock)*TFACT
            local_sums(iblock,dcount) = sum(WORK1(ib:ie,jb:je))

            dcount = dcount + 1
            WORK1 = DIAG_TRACER_VDIFF_2D(:,:,n,iblock)*TFACT
            local_sums(iblock,dcount) = sum(WORK1(ib:ie,jb:je))

            dcount = dcount + 1
            WORK1 = DIAG_TRACER_SOURCE_2D(:,:,n,iblock)*TFACT
            local_sums(iblock,dcount) = sum(WORK1(ib:ie,jb:je))

            dcount = dcount + 1
            do k=1,km
               if (sfc_layer_type == sfc_layer_varthick .and. &
                   k == 1) then
                  WORK1 = merge(TAREA(:,:,iblock)*(                    &
                            (dz(1) + PSURF(:,:,newtime,iblock)/grav)*  &
                                TRACER(:,:,k,n,newtime,iblock) -       &
                            (dz(1) + PSURF(:,:,mixtime,iblock)/grav)*  &
                                TRACER(:,:,k,n,oldtime,iblock)),       &
                                c0, k <= KMT(:,:,iblock))
               else
                  if (partial_bottom_cells) then
                     WORK1 = merge(DZT(:,:,k,iblock)*TAREA(:,:,iblock)*&
                                   (TRACER(:,:,k,n,newtime,iblock) -   &
                                    TRACER(:,:,k,n,oldtime,iblock)),   &
                                   c0, k <= KMT(:,:,iblock))
                  else
                     WORK1 = merge(dz(k)*TAREA(:,:,iblock)*            &
                                   (TRACER(:,:,k,n,newtime,iblock) -   &
                                    TRACER(:,:,k,n,oldtime,iblock)),   &
                                   c0, k <= KMT(:,:,iblock))
                  endif
               endif

               local_sums(iblock,dcount  ) = &
               local_sums(iblock,dcount  ) + &
                                      sum(WORK1(ib:ie,jb:je))/c2dtt(k)
               local_sums(iblock,dcount+1) = &
               local_sums(iblock,dcount+1) + &
                                 sum(abs(WORK1(ib:ie,jb:je)))/c2dtt(k)
            end do  !k loop
            dcount = dcount + 1
         end do !tracer loop
      end do ! block loop

      !$OMP END PARALLEL DO 

!-----------------------------------------------------------------------
!
!     finish up scalar diagnostics
!
!-----------------------------------------------------------------------

      dcount = 1
      sum_tmp = sum(local_sums(:,dcount))
      diag_ws         = global_sum(sum_tmp,distrb_clinic)/volume_t
      dcount = dcount + 1
      sum_tmp = sum(local_sums(:,dcount))
      diag_ke_adv     = global_sum(sum_tmp,distrb_clinic)/volume_t
      dcount = dcount + 1
      sum_tmp = sum(local_sums(:,dcount))
      diag_ke_hmix    = global_sum(sum_tmp,distrb_clinic)/volume_t
      dcount = dcount + 1
      sum_tmp = sum(local_sums(:,dcount))
      diag_ke_vmix    = global_sum(sum_tmp,distrb_clinic)/volume_t
      dcount = dcount + 1
      sum_tmp = sum(local_sums(:,dcount))
      diag_ke_press   = global_sum(sum_tmp,distrb_clinic)/volume_t
      dcount = dcount + 1
      sum_tmp = sum(local_sums(:,dcount))
      diag_pe         = global_sum(sum_tmp,distrb_clinic)/volume_t
      dcount = dcount + 1
      sum_tmp = sum(local_sums(:,dcount))
      diag_press_free = global_sum(sum_tmp,distrb_clinic)/volume_t
      dcount = dcount + 1
      sum_tmp = sum(local_sums(:,dcount))
      diag_ke_free    = global_sum(sum_tmp,distrb_clinic)/volume_t
      dcount = dcount + 1
      sum_tmp = sum(local_sums(:,dcount))
      diag_ke_psfc    = global_sum(sum_tmp,distrb_clinic)/volume_t
      do n=1,nt
        dcount = dcount + 1
        sum_tmp = sum(local_sums(:,dcount))
        diag_tracer_adv(n)   = &
                         global_sum(sum_tmp,distrb_clinic)/volume_t
        dcount = dcount + 1
        sum_tmp = sum(local_sums(:,dcount))
        diag_tracer_hdiff(n) = &
                         global_sum(sum_tmp,distrb_clinic)/volume_t
        dcount = dcount + 1
        sum_tmp = sum(local_sums(:,dcount))
        diag_tracer_vdiff(n) = & 
                         global_sum(sum_tmp,distrb_clinic)/volume_t
        dcount = dcount + 1
        sum_tmp = sum(local_sums(:,dcount))
        diag_tracer_source(n) = & 
                         global_sum(sum_tmp,distrb_clinic)/volume_t
        dcount = dcount + 1
        sum_tmp = sum(local_sums(:,dcount))
        dtracer_avg(n) = global_sum(sum_tmp,distrb_clinic)/volume_t
        dcount = dcount + 1
        sum_tmp = sum(local_sums(:,dcount))
        dtracer_abs(n) = global_sum(sum_tmp,distrb_clinic)/volume_t
      end do

!-----------------------------------------------------------------------

   endif ! ldiag_global

!-----------------------------------------------------------------------
!EOC

 end subroutine diag_global_preupdate

!***********************************************************************
!BOP
! !IROUTINE: diag_global_afterupdate
! !INTERFACE:

 subroutine diag_global_afterupdate

! !DESCRIPTION:
!  Computes diagnostic sums for those diagnostics computed from
!  updated prognostic variables (after updating to new time).
!
! !REVISION HISTORY:
!  same as module

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      i, k,              & ! horiz, vertical level index
      n ,                & ! tracer index
      iblock ,           & ! block index
      dcount ,           & ! diag counter
      ib,ie,jb,je

   real (r8), dimension(max_blocks_clinic,2*nt+2) :: &
      local_sums           ! array for holding block sums
                           ! of each diagnostic

   real (r8), dimension(max_blocks_clinic,km,nt) :: &
      local_sums_k         ! array for holding block sums
                           ! of each diagnostic at each level

   real (r8) ::            &
      sum_tmp              ! temp for local sum

   real (r8), dimension(nx_block,ny_block) :: &
      WORK1,              &! local work space
      TFACT,              &! factor for normalizing sums
      UFACT                ! factor for normalizing sums

   type (block) ::         &
      this_block           ! block information for current block

!-----------------------------------------------------------------------
!
!  compute these diagnostics only if it is time
!
!-----------------------------------------------------------------------

   if (ldiag_global) then

!-----------------------------------------------------------------------
!
!     compute local block sums of each diagnostic
!
!-----------------------------------------------------------------------

      local_sums = c0
      if (diag_all_levels) local_sums_k = c0

      !$OMP PARALLEL DO &
      !$OMP PRIVATE(this_block, ib, ie, jb, je, i, dcount, &
      !$OMP         WORK1, TFACT, UFACT)

      do iblock = 1,nblocks_clinic

         this_block = get_block(blocks_clinic(iblock),iblock)  

         ib = this_block%ib
         ie = this_block%ie
         jb = this_block%jb
         je = this_block%je

         TFACT = TAREA(:,:,iblock)*RCALCT(:,:,iblock)
         UFACT = UAREA(:,:,iblock)*RCALCU(:,:,iblock)
         if (ltripole_grid .and. this_block%jblock == nblocks_y) then
            do i=1,nx_block
               if (this_block%i_glob(i) > nx_global/2) UFACT(i,je) = c0
            end do
         endif

         dcount = 0

         !*** global mean sea level

         dcount = dcount + 1
         WORK1 = PSURF(:,:,curtime,iblock)/grav*TFACT
         local_sums(iblock,dcount) = sum(WORK1(ib:ie,jb:je))

         !*** avg horiz KE at new time

         dcount = dcount + 1
         do k = 1,km
            if (partial_bottom_cells) then
               WORK1 = merge(p5*(UVEL(:,:,k,curtime,iblock)**2 +     &
                                 VVEL(:,:,k,curtime,iblock)**2)*     &
                             UFACT*DZU(:,:,k,iblock),                &
                             c0, KMU(:,:,iblock) >= k)
            else
               WORK1 = merge(p5*(UVEL(:,:,k,curtime,iblock)**2 +     &
                                 VVEL(:,:,k,curtime,iblock)**2)*     &
                             UFACT*dz(k),                            &
                             c0, KMU(:,:,iblock) >= k)
            endif

            local_sums(iblock,dcount) = local_sums(iblock,dcount) + & 
                                        sum(WORK1(ib:ie,jb:je))

         enddo ! k loop

         !*** average tracers at new time

         do n=1,nt
            dcount = dcount + 1
            do k = 1,km
               if (sfc_layer_type == sfc_layer_varthick .and. &
                   k == 1) then
                  WORK1 = merge(TRACER(:,:,k,n,curtime,iblock)*        &
                                TAREA(:,:,iblock)*                     &
                                (c1 + PSURF(:,:,curtime,iblock)/       &
                                (dz(1)*grav))*dz(1)                    &
                               ,c0,KMT(:,:,iblock) >= k)
               else
                  if (partial_bottom_cells) then
                     WORK1 = merge(TRACER(:,:,k,n,curtime,iblock)*     &
                                   TAREA(:,:,iblock)*DZT(:,:,k,iblock),&
                                   c0,KMT(:,:,iblock) >= k)
                  else
                     WORK1 = merge(TRACER(:,:,k,n,curtime,iblock)*  &
                                   TAREA(:,:,iblock)*dz(k),         &
                                   c0,KMT(:,:,iblock) >= k)
                  endif
               endif

               local_sums(iblock,dcount) = local_sums(iblock,dcount) + & 
                                           sum(WORK1(ib:ie,jb:je))

               if (diag_all_levels) then
                  local_sums_k(iblock,k,n) = sum(WORK1(ib:ie,jb:je))
               endif
            end do ! k loop
         end do ! tracer loop

         !*** surface tracer fluxes

         do n=1,nt
            dcount = dcount + 1

            WORK1 = DIAG_TRACER_SFC_FLX(:,:,n,iblock)*TFACT

            local_sums(iblock,dcount) = sum(WORK1(ib:ie,jb:je))

         end do ! tracer loop
      end do ! block loop

      !$OMP END PARALLEL DO

!-----------------------------------------------------------------------
!
!     finish up by computing global sums of local sums
!
!-----------------------------------------------------------------------

      dcount= 1
      sum_tmp = sum(local_sums(:,dcount))
      diag_sealevel = global_sum(sum_tmp,distrb_clinic)/area_t
      dcount = dcount + 1
      sum_tmp = sum(local_sums(:,dcount))
      diag_ke       = global_sum(sum_tmp,distrb_clinic)/volume_u
      do n=1,nt
        dcount = dcount + 1
        sum_tmp = sum(local_sums(:,dcount))
        avg_tracer(n) = global_sum(sum_tmp,distrb_clinic)/volume_t
        if (diag_all_levels) then
           do k=1,km
              sum_tmp = sum(local_sums_k(:,k,n))
              avg_tracer_k(k,n) = global_sum(sum_tmp,distrb_clinic)/ &
                                  area_t_k(k)
           end do
        endif
      end do
      do n=1,nt
        dcount = dcount + 1
        sum_tmp = sum(local_sums(:,dcount))
        sfc_tracer_flux(n) = global_sum(sum_tmp,distrb_clinic)/area_t
      end do

!-----------------------------------------------------------------------
!
!     convert some tracer diagnostics to different units
!
!-----------------------------------------------------------------------

      avg_tracer(2) = avg_tracer(2)*salt_to_ppt ! salinity to ppt
      if (diag_all_levels) then
        do k=1,km
          avg_tracer_k(k,2) = avg_tracer_k(k,2)*salt_to_ppt
        end do
      endif

      sfc_tracer_flux(1) = sfc_tracer_flux(1)/hflux_factor ! flux in W/m2
      sfc_tracer_flux(2) = sfc_tracer_flux(2)/salinity_factor ! FW flux

!-----------------------------------------------------------------------

   endif ! ldiag_global

!-----------------------------------------------------------------------
!EOC

 end subroutine diag_global_afterupdate

!***********************************************************************
!BOP
! !IROUTINE: diag_print
! !INTERFACE:

 subroutine diag_print

! !DESCRIPTION:
!  Writes scalar diagnostic info to both stdout and diagnostic output
!  file.
!
! !REVISION HISTORY:
!  same as module

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variable
!
!-----------------------------------------------------------------------

   integer (int_kind) :: k,n ! dummy indices

   character (25) :: diag_name  ! name of output diagnostic

   character (29), parameter :: &
      diag_fmt = '(1pe22.15,1x,1pe22.15,2x,a25)'

   character (14), parameter :: &
      stdo_fmt = '(a32,1pe22.15)'

   character (21), parameter :: &
      trcr_fmt = '(a24,i3,a19,1pe22.15)'

!-----------------------------------------------------------------------
!
!  print diagnostics if it is time
!
!-----------------------------------------------------------------------

   if (ldiag_global) then
      if (my_task == master_task) then

         !*** append diagnostic output to end of diag output file

         open(diag_unit, file=diag_outfile, status='old', &
                         position='append')

!-----------------------------------------------------------------------
!
!        print global diagnostics to stdout
!
!-----------------------------------------------------------------------

         write(stdout,blank_fmt)
         write(stdout,'(a17)') 'K.E. diagnostics:'
         write(stdout,stdo_fmt) &
         ' mean K.E.                     :', diag_ke
         write(stdout,stdo_fmt) &
         ' K.E. change from horiz. visc. :', diag_ke_hmix
         write(stdout,stdo_fmt) &
         ' K.E. change from vert.  visc. :', diag_ke_vmix
         write(stdout,stdo_fmt) &
         ' K.E. change from wind stress  :', diag_ws

         write(stdout,blank_fmt)
         write(stdout,stdo_fmt) &
         ' advective K.E. balance        :', diag_ke_adv
         write(stdout,stdo_fmt) &
         '                               :', diag_ke_free
         write(stdout,stdo_fmt) &
         ' P.E. verses pressure work     :', diag_pe
         write(stdout,stdo_fmt) &
         '                               :', diag_ke_press
         write(stdout,stdo_fmt) &
         ' surface-pressure contribution :', diag_ke_psfc
         write(stdout,stdo_fmt) &
         '                               :', diag_press_free

         write(stdout,blank_fmt)
         write(stdout,*) 'Tracer diagnostics:'
         do n=1,nt
            write(stdout,blank_fmt)
            write(stdout,trcr_fmt) &
            ' mean value  of tracer  ',n, &
            '                  : ',    avg_tracer(n)
            write(stdout,trcr_fmt) &
            ' abs  change in tracer  ',n, &
            '                  : ',    dtracer_abs(n)
            write(stdout,trcr_fmt) &
            ' mean change in tracer  ',n, &
            '                  : ',    dtracer_avg(n)
            write(stdout,trcr_fmt) &
            ' mean change in tracer  ',n, &
            ' from advection   : ',    diag_tracer_adv(n)
            write(stdout,trcr_fmt) &
            ' mean change in tracer  ',n, &
            ' from horiz. diff.: ',    diag_tracer_hdiff(n)
            write(stdout,trcr_fmt) &
            ' mean change in tracer  ',n, &
            ' from vert.  diff.: ',    diag_tracer_vdiff(n)
            write(stdout,trcr_fmt) &
            ' mean change in tracer  ',n, &
            ' from sources     : ',    diag_tracer_source(n)
            write(stdout,trcr_fmt) &
            ' surface flux of tracer ',n, &
            '                  : ',    sfc_tracer_flux(n)

            if (diag_all_levels) then
               do k=1,km
                   write(stdout,'(a22,i3,a10,i3,a3,1pe22.15)') &
                         ' mean value of tracer ', n,          &
                         ' at level ',             k,          &
                         ' : ',                    avg_tracer_k(k,n)
               end do
            endif
         end do

         write(stdout,blank_fmt)
         write(stdout,'(a18)') 'Other diagnostics:'
         write(stdout,'(a24,1pe22.15)') ' global mean sea level: ', &
                                          diag_sealevel
         write(stdout,'(a19,1pe22.15,2x,i4)') ' residual, scans : ', &
                                          rms_residual, solv_sum_iters

!-----------------------------------------------------------------------
!
!        print global diagnostics to output file
!
!-----------------------------------------------------------------------

         diag_name = 'mean KE'
         write(diag_unit,diag_fmt) tday, diag_ke, diag_name

         do n=1,nt
            write(diag_name,'(a12,i2)') 'mean tracer ',n
            write(diag_unit,diag_fmt) tday, avg_tracer(n), diag_name
            if (diag_all_levels) then
               do k=1,km
                  write(diag_name,'(a12,i2,a8,i2)') &
                        'mean tracer ',n,' at lvl ',k
                  write(diag_unit,diag_fmt) tday, avg_tracer_k(k,n), &
                                            diag_name
               end do
            endif
         end do

         diag_name = 'mean sea level'
         write(diag_unit,diag_fmt) tday, diag_sealevel, diag_name
         diag_name = 'solver residual'
         write(diag_unit,diag_fmt) tday, rms_residual, diag_name
         diag_name = 'solver iterations'
         write(diag_unit,diag_fmt) tday, real(solv_sum_iters), diag_name

         do n=1,nt
            write(diag_name,'(a7,i3,a11)') 'Tracer ',n,' change avg'
            write(diag_unit,diag_fmt) tday, dtracer_avg(n), diag_name
            write(diag_name,'(a7,i3,a11)') 'Tracer ',n,' change abs'
            write(diag_unit,diag_fmt) tday, dtracer_abs(n), diag_name

            write(diag_name,'(a7,i3,a14)') 'Tracer ',n,' change advect'
            write(diag_unit,diag_fmt) tday, diag_tracer_adv(n), diag_name
            write(diag_name,'(a7,i3,a13)') 'Tracer ',n,' change hdiff'
            write(diag_unit,diag_fmt) tday, diag_tracer_hdiff(n), diag_name
            write(diag_name,'(a7,i3,a13)') 'Tracer ',n,' change vdiff'
            write(diag_unit,diag_fmt) tday, diag_tracer_vdiff(n), diag_name
            write(diag_name,'(a7,i3,a14)') 'Tracer ',n,' change source'
            write(diag_unit,diag_fmt) tday, diag_tracer_source(n), diag_name

            write(diag_name,'(a7,i3,a14)') 'Tracer ',n,' surface flux'
            write(diag_unit,diag_fmt) tday, sfc_tracer_flux(n), diag_name
         end do

         diag_name = 'KE change horiz visc'
         write(diag_unit,diag_fmt) tday, diag_ke_hmix, diag_name
         diag_name = 'KE change vert  visc'
         write(diag_unit,diag_fmt) tday, diag_ke_vmix, diag_name
         diag_name = 'KE change wind stress'
         write(diag_unit,diag_fmt) tday, diag_ws, diag_name

         diag_name = 'advect KE balance'
         write(diag_unit,diag_fmt) tday, diag_ke_adv, diag_name
         diag_name = 'advect KE bal free sfc'
         write(diag_unit,diag_fmt) tday, diag_ke_free, diag_name
         diag_name = 'PE change'
         write(diag_unit,diag_fmt) tday, diag_pe, diag_name
         diag_name = 'pressure work'
         write(diag_unit,diag_fmt) tday, diag_ke_press, diag_name
         diag_name = 'KE change sfc-pressure'
         write(diag_unit,diag_fmt) tday, diag_ke_psfc, diag_name
         diag_name = 'press work free sfc'
         write(diag_unit,diag_fmt) tday, diag_press_free, diag_name

!-----------------------------------------------------------------------
!
!        close file and print timer info
!
!-----------------------------------------------------------------------

         close(diag_unit)

         endif

      call timer_print_all()

   endif ! ldiag_global


!-----------------------------------------------------------------------
!EOC

 end subroutine diag_print

!***********************************************************************
!BOP
! !IROUTINE: diag_transport
! !INTERFACE:

 subroutine diag_transport

! !DESCRIPTION:
!  Calculates and prints various user-defined transports.
!
! !REVISION HISTORY:
!  same as module

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      i,j,k,n,iblock,    &! dummy loop indices
      ib,ie,jb,je         ! beg,end indices for block physical domain

   real (r8), dimension(nx_block,ny_block) :: &
      MASS_Z,              &! zonal transport of mass
      HEAT_Z,              &! zonal transport of heat
      SALT_Z,              &! zonal transport of salt
      MASS_M,              &! merid transport of mass
      HEAT_M,              &! merid transport of heat
      SALT_M,              &! merid transport of salt
      WORK1, WORK2          ! local temp space

   real (r8) ::            &
      sum_tmp               ! temp for computing block sums

   real (r8), dimension(:,:), allocatable ::            &
      mass_tran,           &! local sum of mass transport
      heat_tran,           &! local sum of heat transport
      salt_tran             ! local sum of salt transport

   type (block) ::         &
      this_block           ! block information for current block

!-----------------------------------------------------------------------
!
!  only compute if it is time
!  initialize transport sums
!
!-----------------------------------------------------------------------

   if (ldiag_transport) then

      if (my_task == master_task) then
         !*** re-open file and append current values to previous
         !*** outputs from this run

         open(trans_unit, file=diag_transport_outfile, &
              status='old', position='append')
      endif

!-----------------------------------------------------------------------
!
!     calculate transports for each block
!
!-----------------------------------------------------------------------

      allocate(mass_tran(nblocks_clinic,num_transports), &
               heat_tran(nblocks_clinic,num_transports), &
               salt_tran(nblocks_clinic,num_transports))

      !$OMP PARALLEL DO PRIVATE(WORK1, WORK2, MASS_M, SALT_M, HEAT_M, &
      !$OMP                                   MASS_Z, SALT_Z, HEAT_Z, &
      !$OMP                     this_block, i, j, k, ib, ie, jb, je)

      do iblock = 1,nblocks_clinic

         this_block = get_block(blocks_clinic(iblock),iblock)  

         ib = this_block%ib
         ie = this_block%ie
         jb = this_block%jb
         je = this_block%je

         mass_tran(iblock,:) = c0
         heat_tran(iblock,:) = c0
         salt_tran(iblock,:) = c0

         do k = 1,km

            if (partial_bottom_cells) then
               do j=jb,je
               do i=ib,ie
                  MASS_M(i,j) = p5*(                          &
                                UVEL(i,j  ,k,curtime,iblock)* &
                                DYU(i,j  ,iblock)*            &
                                DZU(i,j  ,k,iblock) +         &
                                UVEL(i,j-1,k,curtime,iblock)* &
                                DYU(i,j-1,iblock)*            &
                                DZU(i,j-1,k,iblock) )

                  MASS_Z(i,j) = p5*(                             &
                                VVEL(i  ,j,k,curtime,iblock)*    &
                                DXU(i  ,j,iblock)*               &
                                DZU(i  ,j,k,iblock) +            &
                                VVEL(i-1,j,k,curtime,iblock)*    &
                                DXU(i-1,j,iblock)*               &
                                DZU(i-1,j,k,iblock) )

               end do
               end do
            else
               do j=jb,je
               do i=ib,ie
                  MASS_M(i,j) = p5*(                                   &
                                UVEL(i,j  ,k,curtime,iblock)*          &
                                DYU(i,j  ,iblock)*dz(k)                &
                              + UVEL(i,j-1,k,curtime,iblock)*          &
                                DYU(i,j-1,iblock)*dz(k) )

                  MASS_Z(i,j) = p5*(                                   &
                                VVEL(i  ,j,k,curtime,iblock)*          &
                                DXU(i  ,j,iblock)*dz(k)                &
                              + VVEL(i-1,j,k,curtime,iblock)*          &
                                DXU(i-1,j,iblock)*dz(k) )
               end do
               end do
            endif

            do j=jb,je
            do i=ib,ie
               HEAT_M(i,j) = p5*MASS_M(i,j)*                     &
                             (TRACER(i+1,j,k,1,curtime,iblock) + &
                              TRACER(i  ,j,k,1,curtime,iblock))

               SALT_M(i,j) = p5*MASS_M(i,j)*                     &
                             (TRACER(i+1,j,k,2,curtime,iblock) + &
                              TRACER(i  ,j,k,2,curtime,iblock))

               HEAT_Z(i,j) = p5*MASS_Z(i,j)*                     &
                             (TRACER(i,j+1,k,1,curtime,iblock) + &
                              TRACER(i,j  ,k,1,curtime,iblock))

               SALT_Z(i,j) = p5*MASS_Z(i,j)*                     &
                             (TRACER(i,j+1,k,2,curtime,iblock) + &
                              TRACER(i,j  ,k,2,curtime,iblock))
            end do
            end do

!-----------------------------------------------------------------------
!
!           compute transport sums for each defined transport
!
!-----------------------------------------------------------------------

            do n=1,num_transports
               if (k >= transports(n)%add(5,iblock) .and.  &
                   k <= transports(n)%add(6,iblock) ) then

                  !*** local sum

                  select case (transports(n)%type)

                  case (transport_type_zonal)

                     do j=transports(n)%add(3,iblock), &
                          transports(n)%add(4,iblock)
                     do i=transports(n)%add(1,iblock), &
                          transports(n)%add(2,iblock)
                        mass_tran(iblock,n) = mass_tran(iblock,n) + &
                                              MASS_Z(i,j)
                        heat_tran(iblock,n) = heat_tran(iblock,n) + &
                                              HEAT_Z(i,j)
                        salt_tran(iblock,n) = salt_tran(iblock,n) + &
                                              SALT_Z(i,j)
                     end do
                     end do

                  case (transport_type_merid)

                     do j=transports(n)%add(3,iblock), &
                          transports(n)%add(4,iblock)
                     do i=transports(n)%add(1,iblock), &
                          transports(n)%add(2,iblock)
                        mass_tran(iblock,n) = mass_tran(iblock,n) + &
                                              MASS_M(i,j)
                        heat_tran(iblock,n) = heat_tran(iblock,n) + &
                                              HEAT_M(i,j)
                        salt_tran(iblock,n) = salt_tran(iblock,n) + &
                                              SALT_M(i,j)
                     end do
                     end do

                  end select

               endif
            end do  ! transport loop
         end do ! k loop
      end do ! block loop

      !$OMP END PARALLEL DO

!-----------------------------------------------------------------------
!
!     finish global sums for each transport, convert to useful units
!     and write to output file
!
!-----------------------------------------------------------------------

      do n=1,num_transports

         sum_tmp = sum(mass_tran(:,n))
         transports(n)%mass = global_sum(sum_tmp,distrb_clinic)* &
                              mass_to_Sv

         sum_tmp = sum(heat_tran(:,n))
         transports(n)%heat = global_sum(sum_tmp,distrb_clinic)* &
                              heat_to_PW

         sum_tmp = sum(salt_tran(:,n))
         transports(n)%salt = global_sum(sum_tmp,distrb_clinic)* &
                              salt_to_Svppt

         if (my_task == master_task) then
            write(trans_unit,'(4(1pe13.6,2x), a)')           &
                                   tday, transports(n)%mass, &
                                         transports(n)%heat, &
                                         transports(n)%salt, &
                                         trim(transports(n)%name)
         endif
      end do ! transport loop

      if (my_task == master_task) close(trans_unit)

   endif ! ldiag_transport

!-----------------------------------------------------------------------
!EOC

 end subroutine diag_transport

!***********************************************************************
!BOP
! !IROUTINE: cfl_advect
! !INTERFACE:

 subroutine cfl_advect(k,iblock,UTE,UTW,VTN,VTS,WTK,WTKB,this_block)

! !DESCRIPTION:
!  Calculates maximum advective cfl number for a particular block.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: &
      k,          &! vertical level index
      iblock       ! local block index in baroclinic distribution

   real (r8), dimension(nx_block,ny_block), intent(in) :: &
      UTE, UTW,          &! advective fluxes through east, west  faces
      VTN, VTS,          &! advective fluxes through north,south faces
      WTK, WTKB           ! advective fluxes through top, bottom faces

   type (block), intent(in) ::     &
      this_block           ! block information for current block

!EOP
!BOC
!-----------------------------------------------------------------------
!
!     local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      i,j                ! dummy loop indices

   real (r8), dimension(nx_block,ny_block) :: &
      CFL_E, CFL_W, &! cfl numbers due to advective fluxes through
      CFL_N, CFL_S, &! each face
      CFL_T, CFL_B, &! 
      CFL_TOT        ! sum for total cfl number

!-----------------------------------------------------------------------
!
!  only compute cfl numbers if it is time
!
!-----------------------------------------------------------------------

   if (ldiag_cfl) then

!-----------------------------------------------------------------------
!
!     advective fluxes through east,west,north,south,top,bottom faces
!
!-----------------------------------------------------------------------

      if (partial_bottom_cells) then

         CFL_E = abs(UTE*TAREA_R(:,:,iblock)/DZU(:,:,k,iblock))*dt(k)
         CFL_W = abs(UTW*TAREA_R(:,:,iblock)/DZU(:,:,k,iblock))*dt(k)
         CFL_N = abs(VTN*TAREA_R(:,:,iblock)/DZU(:,:,k,iblock))*dt(k)
         CFL_S = abs(VTS*TAREA_R(:,:,iblock)/DZU(:,:,k,iblock))*dt(k)
         CFL_T = abs(WTK /DZT(:,:,k,iblock))*dt(k)
         CFL_B = abs(WTKB/DZT(:,:,k,iblock))*dt(k)

      else

         CFL_E = abs(UTE*TAREA_R(:,:,iblock))*dt(k)
         CFL_W = abs(UTW*TAREA_R(:,:,iblock))*dt(k)
         CFL_N = abs(VTN*TAREA_R(:,:,iblock))*dt(k)
         CFL_S = abs(VTS*TAREA_R(:,:,iblock))*dt(k)
         CFL_T = abs(WTK /dz(k))*dt(k)
         CFL_B = abs(WTKB/dz(k))*dt(k)

      endif

      CFL_TOT = p5*(CFL_E + CFL_W + CFL_N + CFL_S + CFL_T + CFL_B)

!-----------------------------------------------------------------------
!
!     find max cfl numbers and locations for this block
!
!-----------------------------------------------------------------------

      cfl_advuk_block(iblock,k) = -c1
      cfl_advvk_block(iblock,k) = -c1
      cfl_advwk_block(iblock,k) = -c1
      cfl_advtk_block(iblock,k) = -c1
      cfladd_advuk_block(:,iblock,k) = 0
      cfladd_advvk_block(:,iblock,k) = 0
      cfladd_advwk_block(:,iblock,k) = 0
      cfladd_advtk_block(:,iblock,k) = 0

      do j=this_block%jb,this_block%je
      do i=this_block%ib,this_block%ie

         !*** zonal cfl number east flux

         if (CFL_E(i,j) > cfl_advuk_block(iblock,k)) then
            cfl_advuk_block(iblock,k) = CFL_E(i,j)
            cfladd_advuk_block(1,iblock,k) = this_block%i_glob(i)
            cfladd_advuk_block(2,iblock,k) = this_block%j_glob(j)
         endif

         !*** zonal cfl number west flux

         if (CFL_W(i,j) > cfl_advuk_block(iblock,k)) then
            cfl_advuk_block(iblock,k) = CFL_W(i,j)
            cfladd_advuk_block(1,iblock,k) = this_block%i_glob(i)
            cfladd_advuk_block(2,iblock,k) = this_block%j_glob(j)
         endif

         !*** meridional cfl number north flux

         if (CFL_N(i,j) > cfl_advvk_block(iblock,k)) then
            cfl_advvk_block(iblock,k) = CFL_N(i,j)
            cfladd_advvk_block(1,iblock,k) = this_block%i_glob(i)
            cfladd_advvk_block(2,iblock,k) = this_block%j_glob(j)
         endif

         !*** meridional cfl number south flux

         if (CFL_S(i,j) > cfl_advvk_block(iblock,k)) then
            cfl_advvk_block(iblock,k) = CFL_S(i,j)
            cfladd_advvk_block(1,iblock,k) = this_block%i_glob(i)
            cfladd_advvk_block(2,iblock,k) = this_block%j_glob(j)
         endif

         !*** vertical cfl number top flux

         if (CFL_T(i,j) > cfl_advwk_block(iblock,k)) then
            cfl_advwk_block(iblock,k) = CFL_T(i,j)
            cfladd_advwk_block(1,iblock,k) = this_block%i_glob(i)
            cfladd_advwk_block(2,iblock,k) = this_block%j_glob(j)
         endif

         !*** vertical cfl number bottom flux

         if (CFL_B(i,j) > cfl_advwk_block(iblock,k)) then
            cfl_advwk_block(iblock,k) = CFL_B(i,j)
            cfladd_advwk_block(1,iblock,k) = this_block%i_glob(i)
            cfladd_advwk_block(2,iblock,k) = this_block%j_glob(j)
         endif

         !*** total advective CFL number

         if (CFL_TOT(i,j) > cfl_advtk_block(iblock,k)) then
            cfl_advtk_block(iblock,k) = CFL_TOT(i,j)
            cfladd_advtk_block(1,iblock,k) = this_block%i_glob(i)
            cfladd_advtk_block(2,iblock,k) = this_block%j_glob(j)
         endif

      end do
      end do

!-----------------------------------------------------------------------
!
!     finished computing advective cfl numbers
!
!-----------------------------------------------------------------------

   endif ! ldiag_cfl

!-----------------------------------------------------------------------
!EOC

 end subroutine cfl_advect

!***********************************************************************
!BOP
! !IROUTINE: cfl_vdiff
! !INTERFACE:

 subroutine cfl_vdiff(k,VDC,VVC,this_block)

! !DESCRIPTION:
!  Calculates maximum vertical diffusion cfl numbers and their
!  locations in i,j,k directions.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: &
      k              ! vertical level index

   real (r8), dimension(:,:,:,:), intent(in) :: &
      VDC                 ! tracer diffusivity

   real (r8), dimension(:,:,:), intent(in) :: &
      VVC                 ! viscosity

   type (block), intent(in) ::     &
      this_block           ! block information for current block

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) ::   &
      kmax, klvl, km1,     &! vertical level indices
      i,j,n,               &! dummy index
      bid                   ! local block id

   real (r8) :: &
      local_max             ! temp for holding local block max

   real (r8), dimension(nx_block,ny_block,max_blocks_clinic), save :: &
      VDCM1, VVCM1        ! save k-1 values for next k level

   real (r8), dimension(nx_block,ny_block) :: &
      WORK1,             &! local work space
      DZ1, DZ2, DZ3       ! grid arrays

!-----------------------------------------------------------------------
!
!  only compute cfl numbers if it is time
!
!-----------------------------------------------------------------------

   if (ldiag_cfl) then

!-----------------------------------------------------------------------
!
!     determine size of VDC arrays and set up some grid arrays
!
!-----------------------------------------------------------------------

      bid = this_block%local_id
      kmax = size(VDC, DIM=3)

      if (kmax > km) then  ! KPP uses VDC(0:km+1)
         klvl = k+1
         km1  = k
      else
         klvl = k
         km1  = k-1
      endif

      if (partial_bottom_cells) then
         if (k < km) then
            DZ1 = c2/(DZT(:,:,k,bid) + DZT(:,:,k+1,bid))
         else
            DZ1 = c2/DZT(:,:,k,bid)
         endif
         if (k == 1) then
            DZ2 = c2/DZT(:,:,k,bid)
         else
            DZ2 = c2/(DZT(:,:,k,bid) + DZT(:,:,k-1,bid))
         endif
         DZ3 = c1/DZT(:,:,k,bid)
      else
         DZ1 = dzwr(k  )
         DZ2 = dzwr(k-1)
         DZ3 = dzr (k  )
      endif

!-----------------------------------------------------------------------
!
!     if VDC a 2d array, compute cfl number for this level and save
!     for next level
!
!-----------------------------------------------------------------------

      if (kmax == 1) then

         if (k == 1) VDCM1(:,:,bid) = c0

         WORK1 = abs(c2*(VDC(:,:,1,1)*DZ1 +  &
                         VDCM1(:,:,bid)*DZ2)*DZ3)

         !*** swap to prepare for next k level
         VDCM1(:,:,bid) = VDC(:,:,1,1)

!-----------------------------------------------------------------------
!
!     if VDC a 3d array, use k-1 value directly from array
!
!-----------------------------------------------------------------------

      else

         if (k == 1) then   !*** k=1 case (k-1 values = 0)
            WORK1 = c0
            do n=1,size(VDC,DIM=4)
               WORK1 = max(WORK1, &
                           abs(c2*VDC(:,:,klvl,n)*dzwr(1)*dzr(1)))
            end do

         else   !*** all other k levels

            WORK1 = c0
            do n=1,size(VDC,DIM=4)
               WORK1 = max(WORK1,abs(c2*(             &
                           VDC(:,:,klvl,n)*DZ1 +      &
                           VDC(:,:,km1 ,n)*DZ2)*DZ3))
            end do

         endif ! k=1

      endif ! kmax = 1

!-----------------------------------------------------------------------
!
!     find max tracer cfl for this block
!
!-----------------------------------------------------------------------

      cfl_vdifftk_block(bid,k) = c0
      local_max = -c1

      do j=this_block%jb,this_block%je
      do i=this_block%ib,this_block%ie

         if (WORK1(i,j) > local_max) then
            local_max = WORK1(i,j)
            cfladd_vdifftk_block(1,bid,k) = this_block%i_glob(i)
            cfladd_vdifftk_block(2,bid,k) = this_block%j_glob(j)
         endif

      end do
      end do

      cfl_vdifftk_block(bid,k) = local_max*dt(k)

!-----------------------------------------------------------------------
!
!     repeat for momentum diffusion
!
!-----------------------------------------------------------------------

      kmax = size(VVC, DIM=3)

      if (partial_bottom_cells) then
         if (k < km) then
            DZ1 = c2/(DZU(:,:,k,bid) + DZU(:,:,k+1,bid))
         else
            DZ1 = c2/DZU(:,:,k,bid)
         endif
         if (k == 1) then
            DZ2 = c2/DZU(:,:,k,bid)
         else
            DZ2 = c2/(DZU(:,:,k,bid) + DZU(:,:,k-1,bid))
         endif
         DZ3 = c1/DZU(:,:,k,bid)
      else
         DZ1 = dzwr(k  )
         DZ2 = dzwr(k-1)
         DZ3 = dzr (k  )
      endif

!-----------------------------------------------------------------------
!
!     if VVC a 2d array, compute cfl number for this level and save
!     for next level
!
!-----------------------------------------------------------------------

      if (kmax == 1) then

         if (k == 1) then
            VVCM1(:,:,bid) = c0
         endif

         WORK1 = abs(c2*(VVC(:,:,1)  *DZ1 + &
                         VVCM1(:,:,bid)*DZ2)*DZ3)

         VVCM1(:,:,bid) = VVC(:,:,1)    ! swap to prepare for next k level

!-----------------------------------------------------------------------
!
!     if VVC a 3d array, use k-1 value directly from array
!
!-----------------------------------------------------------------------

      else

         if (k == 1) then  !*** k=1 case (k-1 values = 0)

            WORK1 = abs(c2*VVC(:,:,1)*dzwr(1)*dzr(1))

         else  !*** all other k levels

            WORK1 = abs(c2*(VVC(:,:,k  )*DZ1 + &
                            VVC(:,:,k-1)*DZ2)*DZ3)

         endif ! k=1

      endif ! kmax=1

!-----------------------------------------------------------------------
!
!     find max momentum cfl for this block
!
!-----------------------------------------------------------------------

      cfl_vdiffuk_block(bid,k) = c0
      local_max = -c1

      do j=this_block%jb,this_block%je
      do i=this_block%ib,this_block%ie

         if (WORK1(i,j) > local_max) then
            local_max = WORK1(i,j)
            cfladd_vdiffuk_block(1,bid,k) = this_block%i_glob(i)
            cfladd_vdiffuk_block(2,bid,k) = this_block%j_glob(j)
         endif

      end do
      end do

      cfl_vdiffuk_block(bid,k) = local_max*dtu

!-----------------------------------------------------------------------
!
!  finished computing vertical diffusion cfl numbers
!
!-----------------------------------------------------------------------

   endif ! ldiag_cfl

!-----------------------------------------------------------------------
!EOC

 end subroutine cfl_vdiff

!***********************************************************************
!BOP
! !IROUTINE: cfl_hdiff
! !INTERFACE:

 subroutine cfl_hdiff(k,iblock,HDIFFCFL,t_or_u,this_block)

! !DESCRIPTION:
!  Calculates maximum horizontal diffusion cfl numbers and their
!     locations in i,j,k directions
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: &
      k,          &! vertical level index
      iblock,     &! local block index in baroclinic distribution
      t_or_u       ! called from hdifft(1) or hdiffu(2)

   real (r8), dimension(nx_block,ny_block), intent(in) :: &
      HDIFFCFL            ! hdiff cfl number at grid points in block

   type (block), intent(in) ::     &
      this_block           ! block information for current block

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      i,j                ! dummy loop indices

   real (r8) :: cfltemp  ! temporary for holding local maximum

   integer (int_kind), dimension(2) :: &
      cfltemp_add        ! temporary for holding address of local max

!-----------------------------------------------------------------------
!
!  compute max cfl number for this block and level
!
!-----------------------------------------------------------------------

   cfltemp = c0
   cfltemp_add(:) = 0

   do j=this_block%jb,this_block%je
   do i=this_block%ib,this_block%ie
      if (HDIFFCFL(i,j) > cfltemp) then
         cfltemp = HDIFFCFL(i,j)
         cfltemp_add(1) = this_block%i_glob(i)
         cfltemp_add(2) = this_block%j_glob(j)
      endif
   end do
   end do

   select case (t_or_u)

   case(1)
      cfl_hdifftk_block(iblock,k) = cfltemp*dt(k)
      cfladd_hdifftk_block(:,iblock,k) = cfltemp_add(:) 

   case(2)
      cfl_hdiffuk_block(iblock,k) = cfltemp*dtu
      cfladd_hdiffuk_block(:,iblock,k) = cfltemp_add(:) 

   end select

!-----------------------------------------------------------------------
!EOC

 end subroutine cfl_hdiff

!***********************************************************************
!BOP
! !IROUTINE: cfl_check
! !INTERFACE:

 subroutine cfl_check

! !DESCRIPTION:
!  Finishes the calculation of all cfl numbers by finding the
!  global maxima and locations from the local block maxima.
!  The final results are written to stdout (or log file)
!  and diagnostic output file.
!
! !REVISION HISTORY:
!  same as module

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      k,iblock            ! loop indices

   integer (int_kind), dimension(1) :: &
      imax                ! max block location

   real (r8) ::         &
      cfl_temp,         &! temp for holding local max
      cfl_advu,         &! max total cfl number for zonal advection
      cfl_advv,         &! max total cfl number for merid advection
      cfl_advw,         &! max total cfl number for vert  advection
      cfl_advt,         &! max total cfl number for total advection
      cfl_vdifft,       &! max cfl num for vert tracer diffusion
      cfl_vdiffu,       &! max cfl num for vert momentum diffusion
      cfl_hdifft,       &! max cfl num for horiz tracer diffusion
      cfl_hdiffu         ! max cfl num for horiz momentum diffusion

   real (r8), dimension(km) ::         & ! max for each level
      cfl_advuk,        &! max total cfl number for zonal advection
      cfl_advvk,        &! max total cfl number for merid advection
      cfl_advwk,        &! max total cfl number for vert  advection
      cfl_advtk,        &! max total cfl number for total advection
      cfl_vdifftk,      &! max cfl num for vert tracer diffusion
      cfl_vdiffuk,      &! max cfl num for vert momentum diffusion
      cfl_hdifftk,      &! max cfl num for horiz tracer diffusion
      cfl_hdiffuk        ! max cfl num for horiz momentum diffusion

   integer (int_kind), dimension(3) :: &
      cfladd_temp,      &! temp for holding local max address
      cfladd_advu,      &! address of max zonal advection cfl number
      cfladd_advv,      &! address of max merid advection cfl number
      cfladd_advw,      &! address of max vert  advection cfl number
      cfladd_advt,      &! address of max total advection cfl number
      cfladd_vdifft,    &! address of max vertical tracer diff cfl num
      cfladd_vdiffu,    &! address of max vertical moment diff cfl num
      cfladd_hdifft,    &! address of max horiz tracer diff cfl num
      cfladd_hdiffu      ! address of max horiz momentum diff cfl num

   integer (int_kind), dimension(2,km) :: & ! addresses for each level
      cfladd_advuk,     &! address of max zonal advection cfl number
      cfladd_advvk,     &! address of max merid advection cfl number
      cfladd_advwk,     &! address of max vert  advection cfl number
      cfladd_advtk,     &! address of max total advection cfl number
      cfladd_vdifftk,   &! address of max vertical tracer diff cfl num
      cfladd_vdiffuk,   &! address of max vertical moment diff cfl num
      cfladd_hdifftk,   &! address of max horiz tracer diff cfl num
      cfladd_hdiffuk     ! address of max horiz momentum diff cfl num

   character (25) :: &
      name_temp1, name_temp2  ! temporaries for writing names

   character (*), parameter :: &
      cfl_fmt1 =  "(' max. cfl number for ',a25,': ',1pe12.4,4x,3i5)", &
      cfl_fmt2 = '(1pe22.15,1x,1pe22.15,2x,a25)'

!-----------------------------------------------------------------------
!
!  compute cfl diagnostics only if it is time
!
!-----------------------------------------------------------------------

   if (ldiag_cfl) then

!-----------------------------------------------------------------------
!
!     initialize max cfl numbers
!
!-----------------------------------------------------------------------

      cfl_advu   = -c1
      cfl_advv   = -c1
      cfl_advw   = -c1
      cfl_advt   = -c1
      cfl_vdifft = -c1
      cfl_vdiffu = -c1
      cfl_hdifft = -c1
      cfl_hdiffu = -c1

      cfl_advuk   = -c1
      cfl_advvk   = -c1
      cfl_advwk   = -c1
      cfl_advtk   = -c1
      cfl_vdifftk = -c1
      cfl_vdiffuk = -c1
      cfl_hdifftk = -c1
      cfl_hdiffuk = -c1

      cfladd_advu   = 0
      cfladd_advv   = 0
      cfladd_advw   = 0
      cfladd_advt   = 0
      cfladd_vdifft = 0
      cfladd_vdiffu = 0
      cfladd_hdifft = 0
      cfladd_hdiffu = 0

      cfladd_advuk   = 0
      cfladd_advvk   = 0
      cfladd_advwk   = 0
      cfladd_advtk   = 0
      cfladd_vdifftk = 0
      cfladd_vdiffuk = 0
      cfladd_hdifftk = 0
      cfladd_hdiffuk = 0

!-----------------------------------------------------------------------
!
!     compute level maximum cfl numbers from block results
!
!-----------------------------------------------------------------------

      do k=1,km

         !*** zonal advection

         imax = maxloc(cfl_advuk_block(:,k))
         cfl_temp = cfl_advuk_block(imax(1),k)
         cfl_advuk(k) = global_maxval(cfl_temp)

         cfladd_temp = 0
         if (cfl_temp == cfl_advuk(k)) then
            cfladd_temp(1:2) = cfladd_advuk_block(:,imax(1),k)
         endif
         cfladd_advuk(1,k) = global_maxval(cfladd_temp(1))
         cfladd_advuk(2,k) = global_maxval(cfladd_temp(2))

         !*** meridional advection

         imax = maxloc(cfl_advvk_block(:,k))
         cfl_temp = cfl_advvk_block(imax(1),k)
         cfl_advvk(k) = global_maxval(cfl_temp)

         cfladd_temp = 0
         if (cfl_temp == cfl_advvk(k)) then
            cfladd_temp(1:2) = cfladd_advvk_block(:,imax(1),k)
         endif
         cfladd_advvk(1,k) = global_maxval(cfladd_temp(1))
         cfladd_advvk(2,k) = global_maxval(cfladd_temp(2))

         !*** vertical advection

         imax = maxloc(cfl_advwk_block(:,k))
         cfl_temp = cfl_advwk_block(imax(1),k)
         cfl_advwk(k) = global_maxval(cfl_temp)

         cfladd_temp = 0
         if (cfl_temp == cfl_advwk(k)) then
            cfladd_temp(1:2) = cfladd_advwk_block(:,imax(1),k)
         endif
         cfladd_advwk(1,k) = global_maxval(cfladd_temp(1))
         cfladd_advwk(2,k) = global_maxval(cfladd_temp(2))

         !*** total advection

         imax = maxloc(cfl_advtk_block(:,k))
         cfl_temp = cfl_advtk_block(imax(1),k)
         cfl_advtk(k) = global_maxval(cfl_temp)

         cfladd_temp = 0
         if (cfl_temp == cfl_advtk(k)) then
            cfladd_temp(1:2) = cfladd_advtk_block(:,imax(1),k)
         endif
         cfladd_advtk(1,k) = global_maxval(cfladd_temp(1))
         cfladd_advtk(2,k) = global_maxval(cfladd_temp(2))

         !*** vertical tracer diffusion

         imax = maxloc(cfl_vdifftk_block(:,k))
         cfl_temp = cfl_vdifftk_block(imax(1),k)
         cfl_vdifftk(k) = global_maxval(cfl_temp)

         cfladd_temp = 0
         if (cfl_temp == cfl_vdifftk(k)) then
            cfladd_temp(1:2) = cfladd_vdifftk_block(:,imax(1),k)
         endif
         cfladd_vdifftk(1,k) = global_maxval(cfladd_temp(1))
         cfladd_vdifftk(2,k) = global_maxval(cfladd_temp(2))

         !*** vertical momentum diffusion

         imax = maxloc(cfl_vdiffuk_block(:,k))
         cfl_temp = cfl_vdiffuk_block(imax(1),k)
         cfl_vdiffuk(k) = global_maxval(cfl_temp)

         cfladd_temp = 0
         if (cfl_temp == cfl_vdiffuk(k)) then
            cfladd_temp(1:2) = cfladd_vdiffuk_block(:,imax(1),k)
         endif
         cfladd_vdiffuk(1,k) = global_maxval(cfladd_temp(1))
         cfladd_vdiffuk(2,k) = global_maxval(cfladd_temp(2))

         !*** horizontal tracer diffusion

         imax = maxloc(cfl_hdifftk_block(:,k))
         cfl_temp = cfl_hdifftk_block(imax(1),k)
         cfl_hdifftk(k) = global_maxval(cfl_temp)

         cfladd_temp = 0
         if (cfl_temp == cfl_hdifftk(k)) then
            cfladd_temp(1:2) = cfladd_hdifftk_block(:,imax(1),k)
         endif
         cfladd_hdifftk(1,k) = global_maxval(cfladd_temp(1))
         cfladd_hdifftk(2,k) = global_maxval(cfladd_temp(2))

         !*** horizontal momentum diffusion

         imax = maxloc(cfl_hdiffuk_block(:,k))
         cfl_temp = cfl_hdiffuk_block(imax(1),k)
         cfl_hdiffuk(k) = global_maxval(cfl_temp)

         cfladd_temp = 0
         if (cfl_temp == cfl_hdiffuk(k)) then
            cfladd_temp(1:2) = cfladd_hdiffuk_block(:,imax(1),k)
         endif
         cfladd_hdiffuk(1,k) = global_maxval(cfladd_temp(1))
         cfladd_hdiffuk(2,k) = global_maxval(cfladd_temp(2))

      end do

!-----------------------------------------------------------------------
!
!     compute global maximum cfl numbers from level maxima
!
!-----------------------------------------------------------------------

      do k = 1,km

         !*** advective cfl numbers

         if (cfl_advuk(k) > cfl_advu) then
            cfl_advu = cfl_advuk(k)
            cfladd_advu(1) = cfladd_advuk(1,k)
            cfladd_advu(2) = cfladd_advuk(2,k)
            cfladd_advu(3) = k
         endif

         if (cfl_advvk(k) > cfl_advv) then
            cfl_advv = cfl_advvk(k)
            cfladd_advv(1) = cfladd_advvk(1,k)
            cfladd_advv(2) = cfladd_advvk(2,k)
            cfladd_advv(3) = k
         endif

         if (cfl_advwk(k) > cfl_advw) then
            cfl_advw = cfl_advwk(k)
            cfladd_advw(1) = cfladd_advwk(1,k)
            cfladd_advw(2) = cfladd_advwk(2,k)
            cfladd_advw(3) = k
         endif

         if (cfl_advtk(k) > cfl_advt) then
            cfl_advt = cfl_advtk(k)
            cfladd_advt(1) = cfladd_advtk(1,k)
            cfladd_advt(2) = cfladd_advtk(2,k)
            cfladd_advt(3) = k
         endif

         !*** vertical diffusion cfl numbers

         if (cfl_vdifftk(k) > cfl_vdifft) then
            cfl_vdifft = cfl_vdifftk(k)
            cfladd_vdifft(1) = cfladd_vdifftk(1,k)
            cfladd_vdifft(2) = cfladd_vdifftk(2,k)
            cfladd_vdifft(3) = k
         endif

         if (cfl_vdiffuk(k) > cfl_vdiffu) then
            cfl_vdiffu = cfl_vdiffuk(k)
            cfladd_vdiffu(1) = cfladd_vdiffuk(1,k)
            cfladd_vdiffu(2) = cfladd_vdiffuk(2,k)
            cfladd_vdiffu(3) = k
         endif

         !*** horizontal diffusion cfl numbers

         if (cfl_hdifftk(k) > cfl_hdifft) then
            cfl_hdifft = cfl_hdifftk(k)
            cfladd_hdifft(1) = cfladd_hdifftk(1,k)
            cfladd_hdifft(2) = cfladd_hdifftk(2,k)
            cfladd_hdifft(3) = k
         endif

         if (cfl_hdiffuk(k) > cfl_hdiffu) then
            cfl_hdiffu = cfl_hdiffuk(k)
            cfladd_hdiffu(1) = cfladd_hdiffuk(1,k)
            cfladd_hdiffu(2) = cfladd_hdiffuk(2,k)
            cfladd_hdiffu(3) = k
         endif

      end do

!-----------------------------------------------------------------------
!
!     write results to stdout
!
!-----------------------------------------------------------------------

      if (my_task == master_task) then
         write(stdout,blank_fmt)

         name_temp1 = 'zonal advection          '
         write(stdout,cfl_fmt1) name_temp1, cfl_advu, cfladd_advu(:)
         name_temp1 = 'meridional advection     '
         write(stdout,cfl_fmt1) name_temp1, cfl_advv, cfladd_advv(:)
         name_temp1 = 'vertical advection       '
         write(stdout,cfl_fmt1) name_temp1, cfl_advw, cfladd_advw(:)
         name_temp1 = 'total advection          '
         write(stdout,cfl_fmt1) name_temp1, cfl_advt, cfladd_advt(:)
 
         if (cfl_vdifft /= c0) then
            name_temp1 = 'vertical tracer mixing   '
            write(stdout,cfl_fmt1) name_temp1, cfl_vdifft, &
                                               cfladd_vdifft(:)
         endif
         if (cfl_vdifft /= c0) then
            name_temp1 = 'vertical moment. mixing  '
            write(stdout,cfl_fmt1) name_temp1, cfl_vdiffu, &
                                               cfladd_vdiffu(:)
         endif
 
         name_temp1 = 'horizontal tracer mixing '
         write(stdout,cfl_fmt1) name_temp1, cfl_hdifft, cfladd_hdifft(:)
         name_temp1 = 'horiz. momentum mixing   '
         write(stdout,cfl_fmt1) name_temp1, cfl_hdiffu, cfladd_hdiffu(:)
 
         if (cfl_all_levels) then
            write(stdout,'(a27)') 'CFL numbers at all levels: '
            do k=1,km
               name_temp1 = 'total advection          '
               write(stdout,cfl_fmt1) name_temp1, cfl_advtk(k), &
                                      cfladd_advtk(:,k), k
               name_temp1 = 'horizontal tracer mixing '
               write(stdout,cfl_fmt1) name_temp1, cfl_hdifftk(k), &
                                      cfladd_hdifftk(:,k), k
            end do
         endif
 
!-----------------------------------------------------------------------
!
!        write results to output file
!
!-----------------------------------------------------------------------

         open(diag_unit, file=diag_outfile, status='old', &
              position='append')

         name_temp1 = 'cfl_advu'
         name_temp2 = 'cfl_advv'
         write(diag_unit,cfl_fmt2) tday, cfl_advu, name_temp1
         write(diag_unit,cfl_fmt2) tday, cfl_advv, name_temp2
         name_temp1 = 'cfl_advw'
         name_temp2 = 'cfl_advt'
         write(diag_unit,cfl_fmt2) tday, cfl_advw, name_temp1
         write(diag_unit,cfl_fmt2) tday, cfl_advt, name_temp2

         if (cfl_vdifft /= c0) then
            name_temp1 = 'cfl_vdifft'
            name_temp2 = 'cfl_vdiffu'
            write(diag_unit,cfl_fmt2) tday, cfl_vdifft, name_temp1
            write(diag_unit,cfl_fmt2) tday, cfl_vdiffu, name_temp2
         endif

         name_temp1 = 'cfl_hdifft'
         name_temp2 = 'cfl_hdiffu'
         write(diag_unit,cfl_fmt2) tday, cfl_hdifft, name_temp1
         write(diag_unit,cfl_fmt2) tday, cfl_hdiffu, name_temp2

         if (cfl_all_levels) then
            do k=1,km
               write(name_temp1,"('cfl_advtk ',i3)") k
               write(name_temp2,"('cfl_advuk ',i3)") k
               write(diag_unit,cfl_fmt2) tday,cfl_advtk(k),  name_temp1
               write(diag_unit,cfl_fmt2) tday,cfl_hdifftk(k),name_temp2
            end do
         endif

!-----------------------------------------------------------------------
!
!        finished writing max cfl numbers
!
!-----------------------------------------------------------------------

         close(diag_unit)

      endif ! master task

!-----------------------------------------------------------------------
!
!     all done
!
!-----------------------------------------------------------------------

   endif ! ldiag_cfl

!-----------------------------------------------------------------------
!EOC

 end subroutine cfl_check

!***********************************************************************
!BOP
! !IROUTINE: check_KE
! !INTERFACE:

 function check_KE(ke_thresh)

! !DESCRIPTION:
!  Checks the kinetic energy diagnostic to determine whether it
!  exceeds a threshold value.  Also checks for negative values.
!  Exceeding these ranges results in a true value returned.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   real (r8), intent(in) :: &
      ke_thresh             ! threshold value for kinetic energy

! !OUTPUT PARAMETERS:

   logical (log_kind) :: &
      check_KE           ! true if k.e. exceeds threshold or < 0

!EOP
!BOC
!-----------------------------------------------------------------------

   check_KE = .false.  ! default value
   if (diag_ke > ke_thresh .or. diag_ke < 0) check_KE = .true.

!-----------------------------------------------------------------------
!EOC

 end function check_KE

!***********************************************************************

 end module diagnostics

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
