!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module restart

!BOP
! !MODULE: restart
! !DESCRIPTION:
!  This module contains routins for reading and writing data necessary
!  for restarting a POP simulation.
!
! !REVISION HISTORY:
!  CVS:$Id: restart.F90,v 1.16 2003/01/10 00:11:26 jfd Exp $
!  CVS:$Name: POP_2_0_1 $
!
! !USES:

   use kinds_mod, only: char_len, log_kind, int_kind, r8
   use domain, only: nblocks_clinic, blocks_clinic, bndy_clinic
   use constants, only: char_blank, field_loc_NEcorner, field_type_vector,  &
       field_loc_center, field_type_scalar, blank_fmt, c0, grav
   use blocks, only: nx_block, ny_block, block, get_block
   use prognostic, only: UBTROP, VBTROP, PSURF, GRADPX, GRADPY, UVEL, VVEL, &
       PGUESS, TRACER, nt, nx_global, ny_global, km, curtime, oldtime,      &
       tracer_d
   use broadcast, only: broadcast_scalar 
   use boundary, only: update_ghost_cells
   use communicate, only: my_task, master_task
   use operators, only: div
   use grid, only: sfc_layer_type, sfc_layer_varthick, CALCU, CALCT, KMU,   &
       KMT, HU, TAREA_R
   use io, only: data_set
   use io_types, only: io_field_desc, datafile, io_dim, luse_pointer_files, &
       pointer_filename, stdout, construct_io_field, construct_file,        &
       rec_type_dbl, construct_io_dim, nml_in, nml_filename, get_unit,      &
       release_unit, destroy_file, add_attrib_file, destroy_io_field,       &
       extract_attrib_file
   use time_management, only: cday, cmonth, init_time_flag, freq_opt_nstep, &
       freq_opt_nsecond, freq_opt_nhour, freq_opt_nday, freq_opt_nmonth,    &
       freq_opt_nyear, freq_opt_never, hours_in_year, seconds_in_year,      &
       days_in_prior_year, days_in_year, leapyear, newhour, newday,         &
       adjust_year_next, midnight_last, eoy_last, eoy, eom_next, eom_last,  &
       eom, eod_last, eod, nsteps_total, seconds_this_year_next,            &
       seconds_this_year, seconds_this_day_next, seconds_this_day,          &
       elapsed_days_this_year, elapsed_years, elapsed_months, elapsed_days, &
       iday_of_year_last, iday_of_year, dtt, isecond0, iminute0, ihour0,    &
       iday0, imonth0, iyear0, isecond, iminute, ihour, iday, imonth, iyear,&
       runid, dtp, init_time_flag, cmonth3, freq_opt_nstep,                 &
       freq_opt_nsecond, cyear, date_separator, freq_opt_nstep,             &
       freq_opt_nsecond, check_time_flag, month3_all, int_to_char,          &
       set_time_flag_last
   use ice, only: tlast_ice, liceform, AQICE
   use forcing, only: FW_OLD
   use forcing_ap, only: ap_interp_last
   use forcing_ws, only: ws_interp_last
   use forcing_shf, only: shf_interp_last
   use forcing_sfwf, only: sfwf_interp_last, sum_precip, precip_fact,       &
       ssh_initial, sal_initial
   use forcing_pt_interior, only: pt_interior_interp_last
   use forcing_s_interior, only: s_interior_interp_last
   use exit_mod, only: sigAbort, exit_pop

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:

   public :: init_restart,  &
             write_restart, &
             read_restart

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  module variables
!
!-----------------------------------------------------------------------

   character (char_len) :: &
      restart_outfile,     &! restart output filename root
      restart_fmt           ! format (bin or nc) of output restart

   logical (log_kind) ::   &
      pressure_correction, &! fix pressure for exact restart
      lrestart_on,         &! flag to turn restarts on/off
      leven_odd_on          ! flag to turn even_odd restarts on/off

   integer (int_kind) ::  &
      even_odd_freq,      &! even/odd restart files every freq steps
      last_even_odd,      &! last even/odd dump
      restart_flag,       &! time flag id for restarts
      evenodd_flag,       &! time flag id for even-odd restarts
      out_stop_now,       &! time flag id for stop_now flag
      restart_cpl_ts,     &! time flag id for coupled_ts time flag
      restart_freq_iopt,  &! restart frequency option
      restart_freq         ! restart frequency

   integer (int_kind), parameter :: &
      even = 0,           &! integer for which even/odd dump
      odd  = 1

   !*** field descriptors for all output fields

   type (io_field_desc) ::     &
      UBTROP_CUR, UBTROP_OLD,  &! barotropic U at current, old times
      VBTROP_CUR, VBTROP_OLD,  &! barotropic U at current, old times
      PSURF_CUR,  PSURF_OLD,   &! surface press at current, old times
      GRADPX_CUR, GRADPX_OLD,  &! sfc press gradient in x at both times
      GRADPY_CUR, GRADPY_OLD,  &! sfc press gradient in y at both times
      PGUESSd,                 &! guess for next surface pressure
      FW_OLDd,                 &! freshwater input at old time
      AQICEd,                  &! accumulated ice melt/freeze
      UVEL_CUR, UVEL_OLD,      &! U at current, old times
      VVEL_CUR, VVEL_OLD        ! V at current, old times

   type (io_field_desc), dimension(nt) :: &
      TRACER_CUR, TRACER_OLD    ! tracers at current, old times

!-----------------------------------------------------------------------
!
!  scalar data to be written/read from restart file
!
!     runid,
!     iyear,  imonth,  iday,  ihour,  iminute,  isecond
!     iyear0, imonth0, iday0, ihour0, iminute0, isecond0
!     dtt,    iday_of_year,   iday_of_year_last
!     elapsed_days, elapsed_months, elapsed_years
!     elapsed_days_this_year
!     seconds_this_day,  seconds_this_day_next
!     seconds_this_year, seconds_this_year_next
!     nsteps_total
!     eod, eod_last, eom, eom_last, eom_next, eoy, eoy_last
!     midnight_last, adjust_year_next, newday, newhour
!     leapyear, days_in_year, days_in_prior_year
!     seconds_in_year, hours_in_year
!     tlast_ice
!     lcoupled_ts
!     shf_interp_last, sfwf_interp_last, ws_interp_last
!     ap_interp_last, pt_interior_interp_last
!     s_interior_interp_last
!     sal_initial, sum_precip, precip_fact, ssh_initial
!
!-----------------------------------------------------------------------

!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: read_restart
! !INTERFACE:

 subroutine read_restart(in_filename,lbranch,in_restart_fmt)

! !DESCRIPTION:
!  This routine reads restart data from a file.
!
!  Prognostic fields read are:
!     UBTROP,VBTROP  : barotropic velocities
!     PSURF          : surface pressure
!     GRADPX,GRADPY  : surface pressure gradient
!     PGUESS         : next guess for pressure
!     UVEL,VVEL      : 3d velocities
!     TRACER         : 3d tracers
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   character (*), intent(in) :: &
      in_filename,              &! filename of restart file
      in_restart_fmt             ! format of restart file (bin,nc)

   logical (log_kind), intent(in) :: &
      lbranch        ! flag if branch initialization

!EOP
!BOC
!-----------------------------------------------------------------------
!
!     local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      n, k,              &! dummy counters
      nu,                &! i/o unit for pointer file reads
      iblock,            &! local block index
      cindx,cindx2        ! indices into character strings

   real (r8), dimension(nx_block,ny_block) :: &
      WORK1,WORK2        ! work space for pressure correction

   character (char_len) ::  &
      restart_filename,     &! modified file name for restart file
      restart_pointer_file, &! file name for restart pointer file
      short_name, long_name  ! tracer name temporaries

   logical (log_kind) ::   &
      lcoupled_ts           ! flag to check whether coupled time step

   type (block) ::         &
      this_block            ! block information for current block

   type (datafile) :: &
      restart_file    ! io file descriptor

   type (io_dim) :: &
      i_dim, j_dim, &! dimension descriptors for horiz dims
      k_dim          ! dimension descriptor  for vertical levels

!-----------------------------------------------------------------------
!
!  if pointer files are used, pointer file must be read to get
!  actual filenames - skip this for branch initialization
!
!-----------------------------------------------------------------------

   restart_filename = char_blank
   restart_pointer_file = char_blank

   if (luse_pointer_files .and. .not. lbranch) then
      call get_unit(nu)
      if (my_task == master_task) then
         restart_pointer_file = pointer_filename
         cindx = len_trim(pointer_filename) + 1
         cindx2= cindx + 7
         restart_pointer_file(cindx:cindx2) = '.restart'
         write(stdout,*) 'Reading pointer file: ', &
                          trim(restart_pointer_file)
         open(nu, file=trim(restart_pointer_file), form='formatted', &
                  status='old')
         read(nu,'(a80)') restart_filename
         close(nu)
      endif
      call release_unit(nu)

      call broadcast_scalar(restart_filename, master_task)

!-----------------------------------------------------------------------
!
!  otherwise use input filename
!
!-----------------------------------------------------------------------

   else
      cindx2 = len_trim(in_filename)
      restart_filename(1:cindx2) = trim(in_filename)
   endif

!-----------------------------------------------------------------------
!
!  create input file and define scalars with default values
!
!-----------------------------------------------------------------------

   restart_file =  construct_file(in_restart_fmt,                   &
                                  full_name=trim(restart_filename), &
                                  record_length=rec_type_dbl,       &
                                  recl_words=nx_global*ny_global)

   !*** set some defaults for namelist variables not initialized
   !*** under some options

   tlast_ice = c0
   lcoupled_ts = .false.

   !*** add defaults as file attributes

   call add_attrib_file(restart_file, 'runid',    runid   )
   call add_attrib_file(restart_file, 'iyear',    iyear   )
   call add_attrib_file(restart_file, 'imonth',   imonth  )
   call add_attrib_file(restart_file, 'iday',     iday    )
   call add_attrib_file(restart_file, 'ihour',    ihour   )
   call add_attrib_file(restart_file, 'iminute',  iminute )
   call add_attrib_file(restart_file, 'isecond',  isecond )
   call add_attrib_file(restart_file, 'iyear0',   iyear0  )
   call add_attrib_file(restart_file, 'imonth0',  imonth0 )
   call add_attrib_file(restart_file, 'iday0',    iday0   )
   call add_attrib_file(restart_file, 'ihour0',   ihour0  )
   call add_attrib_file(restart_file, 'iminute0', iminute0)
   call add_attrib_file(restart_file, 'isecond0', isecond0)
   call add_attrib_file(restart_file, 'dtt',      dtt     )
   call add_attrib_file(restart_file, 'iday_of_year', iday_of_year)
   call add_attrib_file(restart_file, 'iday_of_year_last',       &
                                       iday_of_year_last)
   call add_attrib_file(restart_file, 'elapsed_days',    elapsed_days)
   call add_attrib_file(restart_file, 'elapsed_months',  elapsed_months)
   call add_attrib_file(restart_file, 'elapsed_years',   elapsed_years)
   call add_attrib_file(restart_file, 'elapsed_days_this_year',  &
                                       elapsed_days_this_year)
   call add_attrib_file(restart_file, 'seconds_this_day',        &
                                       seconds_this_day)
   call add_attrib_file(restart_file, 'seconds_this_day_next',   &
                                       seconds_this_day_next)
   call add_attrib_file(restart_file, 'seconds_this_year',       &
                                       seconds_this_year)
   call add_attrib_file(restart_file, 'seconds_this_year_next',  &
                                       seconds_this_year_next)
   call add_attrib_file(restart_file, 'nsteps_total'  , nsteps_total)
   call add_attrib_file(restart_file, 'eod'     , eod     )
   call add_attrib_file(restart_file, 'eod_last', eod_last)
   call add_attrib_file(restart_file, 'eom'     , eom     )
   call add_attrib_file(restart_file, 'eom_last', eom_last)
   call add_attrib_file(restart_file, 'eom_next', eom_next)
   call add_attrib_file(restart_file, 'eoy'     , eoy     )
   call add_attrib_file(restart_file, 'eoy_last', eoy_last)
   call add_attrib_file(restart_file, 'midnight_last', midnight_last)
   call add_attrib_file(restart_file, 'adjust_year_next',        &
                                       adjust_year_next)
   call add_attrib_file(restart_file, 'newday' , newday  )
   call add_attrib_file(restart_file, 'newhour', newhour )
   call add_attrib_file(restart_file, 'leapyear',leapyear)
   call add_attrib_file(restart_file, 'days_in_year', days_in_year)
   call add_attrib_file(restart_file, 'days_in_prior_year',      &
                                       days_in_prior_year)
   call add_attrib_file(restart_file, 'seconds_in_year',         &
                                       seconds_in_year)
   call add_attrib_file(restart_file, 'hours_in_year', hours_in_year)
   call add_attrib_file(restart_file, 'tlast_ice',     tlast_ice  )
   call add_attrib_file(restart_file, 'lcoupled_ts',   lcoupled_ts)
   call add_attrib_file(restart_file, 'shf_interp_last',         &
                                       shf_interp_last)
   call add_attrib_file(restart_file, 'sfwf_interp_last',        &
                                       sfwf_interp_last)
   call add_attrib_file(restart_file, 'ws_interp_last',          &
                                       ws_interp_last)
   call add_attrib_file(restart_file, 'ap_interp_last',          &
                                       ap_interp_last)
   call add_attrib_file(restart_file, 'pt_interior_interp_last', &
                                       pt_interior_interp_last)
   call add_attrib_file(restart_file, 's_interior_interp_last',  &
                                       s_interior_interp_last)
   call add_attrib_file(restart_file, 'sum_precip' , sum_precip )
   call add_attrib_file(restart_file, 'precip_fact', precip_fact)
   call add_attrib_file(restart_file, 'ssh_initial', ssh_initial)

   short_name = char_blank
   do k=1,km
      write(short_name,'(a11,i3.3)') 'sal_initial',k
      call add_attrib_file(restart_file,trim(short_name),sal_initial(k))
   end do

!-----------------------------------------------------------------------
!
!  open a file and extract scalars as file attributes
!  do not extract if this is a branch - values are set elsewhere
!
!-----------------------------------------------------------------------

   !*** open file and read attributes

   call data_set(restart_file, 'open_read')

   !*** extract scalars if not a branch

   if (.not. lbranch) then

      call extract_attrib_file(restart_file, 'runid',    runid   )
      call extract_attrib_file(restart_file, 'iyear',    iyear   )
      call extract_attrib_file(restart_file, 'imonth',   imonth  )
      call extract_attrib_file(restart_file, 'iday',     iday    )
      call extract_attrib_file(restart_file, 'ihour',    ihour   )
      call extract_attrib_file(restart_file, 'iminute',  iminute )
      call extract_attrib_file(restart_file, 'isecond',  isecond )
      call extract_attrib_file(restart_file, 'iyear0',   iyear0  )
      call extract_attrib_file(restart_file, 'imonth0',  imonth0 )
      call extract_attrib_file(restart_file, 'iday0',    iday0   )
      call extract_attrib_file(restart_file, 'ihour0',   ihour0  )
      call extract_attrib_file(restart_file, 'iminute0', iminute0)
      call extract_attrib_file(restart_file, 'isecond0', isecond0)
      call extract_attrib_file(restart_file, 'dtt',      dtt     )
      call extract_attrib_file(restart_file, 'iday_of_year',     &
                                              iday_of_year)
      call extract_attrib_file(restart_file, 'iday_of_year_last',&
                                              iday_of_year_last)
      call extract_attrib_file(restart_file, 'elapsed_days',     &
                                              elapsed_days)
      call extract_attrib_file(restart_file, 'elapsed_months',   &
                                              elapsed_months)
      call extract_attrib_file(restart_file, 'elapsed_years',    &
                                              elapsed_years)
      call extract_attrib_file(restart_file, 'elapsed_days_this_year', &
                                              elapsed_days_this_year)
      call extract_attrib_file(restart_file, 'seconds_this_day',       &
                                              seconds_this_day)
      call extract_attrib_file(restart_file, 'seconds_this_day_next',  &
                                              seconds_this_day_next)
      call extract_attrib_file(restart_file, 'seconds_this_year',      &
                                              seconds_this_year)
      call extract_attrib_file(restart_file, 'seconds_this_year_next',  &
                                              seconds_this_year_next)
      call extract_attrib_file(restart_file, 'nsteps_total',     &
                                                    nsteps_total)
      call extract_attrib_file(restart_file, 'eod'     , eod     )
      call extract_attrib_file(restart_file, 'eod_last', eod_last)
      call extract_attrib_file(restart_file, 'eom'     , eom     )
      call extract_attrib_file(restart_file, 'eom_last', eom_last)
      call extract_attrib_file(restart_file, 'eom_next', eom_next)
      call extract_attrib_file(restart_file, 'eoy'     , eoy     )
      call extract_attrib_file(restart_file, 'eoy_last', eoy_last)
      call extract_attrib_file(restart_file, 'midnight_last',     &
                                              midnight_last)
      call extract_attrib_file(restart_file, 'adjust_year_next',  &
                                              adjust_year_next)
      call extract_attrib_file(restart_file, 'newday' , newday  )
      call extract_attrib_file(restart_file, 'newhour', newhour )
      call extract_attrib_file(restart_file, 'leapyear',leapyear)
      call extract_attrib_file(restart_file, 'days_in_year',      &
                                              days_in_year)
      call extract_attrib_file(restart_file, 'days_in_prior_year',&
                                              days_in_prior_year)
      call extract_attrib_file(restart_file, 'seconds_in_year',   &
                                              seconds_in_year)
      call extract_attrib_file(restart_file, 'hours_in_year',     &
                                              hours_in_year)
      call extract_attrib_file(restart_file, 'tlast_ice', tlast_ice  )
      call extract_attrib_file(restart_file, 'lcoupled_ts',       &
                                              lcoupled_ts)
      call extract_attrib_file(restart_file, 'shf_interp_last',   &
                                              shf_interp_last)
      call extract_attrib_file(restart_file, 'sfwf_interp_last',  &
                                              sfwf_interp_last)
      call extract_attrib_file(restart_file, 'ws_interp_last',    &
                                              ws_interp_last)
      call extract_attrib_file(restart_file, 'ap_interp_last',    &
                                              ap_interp_last)
      call extract_attrib_file(restart_file, 'pt_interior_interp_last',&
                                              pt_interior_interp_last)
      call extract_attrib_file(restart_file, 's_interior_interp_last', &
                                              s_interior_interp_last)
      call extract_attrib_file(restart_file, 'sum_precip' , sum_precip )
      call extract_attrib_file(restart_file, 'precip_fact', precip_fact)
      call extract_attrib_file(restart_file, 'ssh_initial', ssh_initial)

      short_name = char_blank
      do k=1,km
         write(short_name,'(a11,i3.3)') 'sal_initial',k
         call extract_attrib_file(restart_file, trim(short_name), &
                                                sal_initial(k))
      end do

      call int_to_char(4, iyear,cyear)
      call int_to_char(2, imonth,cmonth)
      call int_to_char(2, iday,cday)
      cmonth3 = month3_all(imonth)

      !*** set old value for the time flag 'coupled_ts'

      if (lcoupled_ts) then
         restart_cpl_ts = init_time_flag('coupled_ts')
         call set_time_flag_last (restart_cpl_ts, lcoupled_ts)
      endif

   endif ! lbranch

!-----------------------------------------------------------------------
!
!  define all fields to be read
!
!-----------------------------------------------------------------------

   !*** define dimensions

   i_dim = construct_io_dim('i', nx_global)
   j_dim = construct_io_dim('j', ny_global)
   k_dim = construct_io_dim('k', km)

   UBTROP_CUR = construct_io_field('UBTROP_CUR', i_dim, j_dim,        &
                   long_name='U barotropic velocity at current time', &
                   units    ='cm/s', grid_loc ='2220',                &
                   field_loc = field_loc_NEcorner,                    &
                   field_type = field_type_vector,                    &
                   d2d_array =UBTROP(:,:,curtime,:))
   call data_set (restart_file, 'define', UBTROP_CUR)

   UBTROP_OLD = construct_io_field('UBTROP_OLD', i_dim, j_dim,        &
                   long_name='U barotropic velocity at old time',     &
                   units    ='cm/s', grid_loc ='2220',                &
                   field_loc = field_loc_NEcorner,                    &
                   field_type = field_type_vector,                    &
                   d2d_array =UBTROP(:,:,oldtime,:))
   call data_set (restart_file, 'define', UBTROP_OLD)

   VBTROP_CUR = construct_io_field('VBTROP_CUR', i_dim, j_dim,        &
                   long_name='V barotropic velocity at current time', &
                   units    ='cm/s', grid_loc ='2220',                &
                   field_loc = field_loc_NEcorner,                    &
                   field_type = field_type_vector,                    &
                   d2d_array =VBTROP(:,:,curtime,:))
   call data_set (restart_file, 'define', VBTROP_CUR)

   VBTROP_OLD = construct_io_field('VBTROP_OLD', i_dim, j_dim,        &
                   long_name='V barotropic velocity at old time',     &
                   units    ='cm/s', grid_loc ='2220',                &
                   field_loc = field_loc_NEcorner,                    &
                   field_type = field_type_vector,                    &
                   d2d_array =VBTROP(:,:,oldtime,:))
   call data_set (restart_file, 'define', VBTROP_OLD)

   PSURF_CUR = construct_io_field('PSURF_CUR', i_dim, j_dim,          &
                   long_name='surface pressure at current time',      &
                   units    ='dyne/cm2', grid_loc ='2110',            &
                   field_loc = field_loc_center,                      &
                   field_type = field_type_scalar,                    &
                   d2d_array =PSURF(:,:,curtime,:))
   call data_set (restart_file, 'define', PSURF_CUR)

   PSURF_OLD = construct_io_field('PSURF_OLD', i_dim, j_dim,          &
                   long_name='surface pressure at old time',          &
                   units    ='dyne/cm2', grid_loc ='2110',            &
                   field_loc = field_loc_center,                      &
                   field_type = field_type_scalar,                    &
                   d2d_array =PSURF(:,:,oldtime,:))
   call data_set (restart_file, 'define', PSURF_OLD)

   GRADPX_CUR = construct_io_field('GRADPX_CUR', i_dim, j_dim,        &
                   long_name='sfc press gradient in x at current time',&
                   units    ='dyne/cm3', grid_loc ='2220',            &
                   field_loc = field_loc_NEcorner,                    &
                   field_type = field_type_vector,                    &
                   d2d_array =GRADPX(:,:,curtime,:))
   call data_set (restart_file, 'define', GRADPX_CUR)

   GRADPX_OLD = construct_io_field('GRADPX_OLD', i_dim, j_dim,        &
                   long_name='sfc press gradient in x at old time',   &
                   units    ='dyne/cm3', grid_loc ='2220',            &
                   field_loc = field_loc_NEcorner,                    &
                   field_type = field_type_vector,                    &
                   d2d_array =GRADPX(:,:,oldtime,:))
   call data_set (restart_file, 'define', GRADPX_OLD)

   GRADPY_CUR = construct_io_field('GRADPY_CUR', i_dim, j_dim,        &
                   long_name='sfc press gradient in y at current time',&
                   units    ='dyne/cm3', grid_loc ='2220',            &
                   field_loc = field_loc_NEcorner,                    &
                   field_type = field_type_vector,                    &
                   d2d_array =GRADPY(:,:,curtime,:))
   call data_set (restart_file, 'define', GRADPY_CUR)

   GRADPY_OLD = construct_io_field('GRADPY_OLD', i_dim, j_dim,        &
                   long_name='sfc press gradient in y at old time',   &
                   units    ='dyne/cm3', grid_loc ='2220',            &
                   field_loc = field_loc_NEcorner,                    &
                   field_type = field_type_vector,                    &
                   d2d_array =GRADPY(:,:,oldtime,:))
   call data_set (restart_file, 'define', GRADPY_OLD)

   PGUESSd = construct_io_field('PGUESS', i_dim, j_dim,               &
                   long_name='guess for sfc pressure at new time',    &
                   units    ='dyne/cm2', grid_loc ='2110',            &
                   field_loc = field_loc_center,                      &
                   field_type = field_type_scalar,                    &
                   d2d_array =PGUESS)
   call data_set (restart_file, 'define', PGUESSd)

   if (sfc_layer_type == sfc_layer_varthick) then
      FW_OLDd = construct_io_field('FW_OLD', i_dim, j_dim,            &
                   long_name='fresh water input at old time',         &
                   grid_loc ='2110',                                  &
                   field_loc = field_loc_center,                      &
                   field_type = field_type_scalar,                    &
                   d2d_array = FW_OLD)
      call data_set (restart_file, 'define', FW_OLDd)
   endif

   if (liceform) then
      AQICEd = construct_io_field('AQICE', i_dim, j_dim,              &
                   long_name='accumulated ice melt/heat',             &
                   grid_loc ='2220',                                  &
                   field_loc = field_loc_center,                      &
                   field_type = field_type_scalar,                    &
                   d2d_array = AQICE)
      call data_set (restart_file, 'define', AQICEd)
   endif

   UVEL_CUR = construct_io_field('UVEL_CUR', i_dim, j_dim, dim3=k_dim,&
                   long_name='U velocity at current time',            &
                   units    ='cm/s',                                  &
                   grid_loc ='3221',                                  &
                   field_loc = field_loc_NEcorner,                    &
                   field_type = field_type_vector,                    &
                   d3d_array = UVEL(:,:,:,curtime,:))
   call data_set (restart_file, 'define', UVEL_CUR)

   UVEL_OLD = construct_io_field('UVEL_OLD', i_dim, j_dim, dim3=k_dim,&
                   long_name='U velocity at old time',                &
                   units    ='cm/s',                                  &
                   grid_loc ='3221',                                  &
                   field_loc = field_loc_NEcorner,                    &
                   field_type = field_type_vector,                    &
                   d3d_array = UVEL(:,:,:,oldtime,:))
   call data_set (restart_file, 'define', UVEL_OLD)

   VVEL_CUR = construct_io_field('VVEL_CUR', i_dim, j_dim, dim3=k_dim,&
                   long_name='V velocity at current time',            &
                   units    ='cm/s',                                  &
                   grid_loc ='3221',                                  &
                   field_loc = field_loc_NEcorner,                    &
                   field_type = field_type_vector,                    &
                   d3d_array = VVEL(:,:,:,curtime,:))
   call data_set (restart_file, 'define', VVEL_CUR)

   VVEL_OLD = construct_io_field('VVEL_OLD', i_dim, j_dim, dim3=k_dim,&
                   long_name='V velocity at old time',                &
                   units    ='cm/s',                                  &
                   grid_loc ='3221',                                  &
                   field_loc = field_loc_NEcorner,                    &
                   field_type = field_type_vector,                    &
                   d3d_array = VVEL(:,:,:,oldtime,:))
   call data_set (restart_file, 'define', VVEL_OLD)

   do n=1,nt
      short_name = char_blank
      short_name = trim(tracer_d(n)%short_name)/&
                                                &/'_CUR'
      long_name = char_blank
      long_name = trim(tracer_d(n)%long_name)/&
                                              &/'at current time'

      TRACER_CUR(n) = construct_io_field(trim(short_name),            &
                   i_dim, j_dim, dim3=k_dim,                          &
                   long_name=trim(long_name),                         &
                   units    =trim(tracer_d(n)%units),                 &
                   grid_loc ='3111',                                  &
                   field_loc = field_loc_center,                      &
                   field_type = field_type_scalar,                    &
                   d3d_array = TRACER(:,:,:,n,curtime,:))
      call data_set (restart_file, 'define', TRACER_CUR(n))
   end do

   do n=1,nt
      short_name = char_blank
      short_name = trim(tracer_d(n)%short_name)/&
                                                &/'_OLD'
      long_name = char_blank
      long_name = trim(tracer_d(n)%long_name)/&
                                              &/'at old time'

      TRACER_OLD(n) = construct_io_field(trim(short_name),            &
                      i_dim, j_dim, dim3=k_dim,                       &
                      long_name=trim(long_name),                      &
                      units    =trim(tracer_d(n)%units),              &
                      grid_loc ='3111',                               &
                      field_loc = field_loc_center,                   &
                      field_type = field_type_scalar,                 &
                      d3d_array = TRACER(:,:,:,n,oldtime,:))

      call data_set (restart_file, 'define', TRACER_OLD(n))
   end do

!-----------------------------------------------------------------------
!
!  now we actually read each field
!  after reading, get rid of io field descriptors and close file
!
!-----------------------------------------------------------------------

   call data_set (restart_file, 'read', UBTROP_CUR)
   call data_set (restart_file, 'read', UBTROP_OLD)
   call data_set (restart_file, 'read', VBTROP_CUR)
   call data_set (restart_file, 'read', VBTROP_OLD)
   call data_set (restart_file, 'read', PSURF_CUR)
   call data_set (restart_file, 'read', PSURF_OLD)
   call data_set (restart_file, 'read', GRADPX_CUR)
   call data_set (restart_file, 'read', GRADPX_OLD)
   call data_set (restart_file, 'read', GRADPY_CUR)
   call data_set (restart_file, 'read', GRADPY_OLD)
   call data_set (restart_file, 'read', PGUESSd)

   if (sfc_layer_type == sfc_layer_varthick) then
      call data_set (restart_file, 'read', FW_OLDd)
   endif
   if (liceform) then
      call data_set (restart_file, 'read', AQICEd)
   endif

   call data_set (restart_file, 'read', UVEL_CUR)
   call data_set (restart_file, 'read', UVEL_OLD)
   call data_set (restart_file, 'read', VVEL_CUR)
   call data_set (restart_file, 'read', VVEL_OLD)
   do n=1,nt
      call data_set (restart_file, 'read', TRACER_CUR(n))
   end do
   do n=1,nt
      call data_set (restart_file, 'read', TRACER_OLD(n))
   end do

   call destroy_io_field (UBTROP_CUR)
   call destroy_io_field (UBTROP_OLD)
   call destroy_io_field (VBTROP_CUR)
   call destroy_io_field (VBTROP_OLD)
   call destroy_io_field (PSURF_CUR)
   call destroy_io_field (PSURF_OLD)
   call destroy_io_field (GRADPX_CUR)
   call destroy_io_field (GRADPX_OLD)
   call destroy_io_field (GRADPY_CUR)
   call destroy_io_field (GRADPY_OLD)
   call destroy_io_field (PGUESSd)

   if (sfc_layer_type == sfc_layer_varthick) then
      call destroy_io_field (FW_OLDd)
   endif
   if (liceform) then
      call destroy_io_field (AQICEd)
   endif

   call destroy_io_field (UVEL_CUR)
   call destroy_io_field (UVEL_OLD)
   call destroy_io_field (VVEL_CUR)
   call destroy_io_field (VVEL_OLD)
   do n=1,nt
      call destroy_io_field (TRACER_CUR(n))
   end do
   do n=1,nt
      call destroy_io_field (TRACER_OLD(n))
   end do

   call data_set (restart_file, 'close')

   if (my_task == master_task) then
     write(stdout,blank_fmt)
     write(stdout,*) ' file read: ', trim(restart_filename)
   endif

   call destroy_file(restart_file)

!-----------------------------------------------------------------------
!
!  zero prognostic variables at land points
!
!-----------------------------------------------------------------------

   do iblock = 1,nblocks_clinic

      this_block = get_block(blocks_clinic(iblock),iblock)

      where (.not. CALCU(:,:,iblock))
         UBTROP(:,:,curtime,iblock) = c0
         VBTROP(:,:,curtime,iblock) = c0
         GRADPX(:,:,curtime,iblock) = c0
         GRADPY(:,:,curtime,iblock) = c0
         UBTROP(:,:,oldtime,iblock) = c0
         VBTROP(:,:,oldtime,iblock) = c0
         GRADPX(:,:,oldtime,iblock) = c0
         GRADPY(:,:,oldtime,iblock) = c0
      endwhere

      where (.not. CALCT(:,:,iblock))
         PSURF(:,:,curtime,iblock) = c0
         PSURF(:,:,oldtime,iblock) = c0
         PGUESS(:,:,iblock) = c0
      endwhere

      if (sfc_layer_type == sfc_layer_varthick) then
         where (.not. CALCT(:,:,iblock)) FW_OLD(:,:,iblock) = c0
      endif

      do k = 1,km
         where (k > KMU(:,:,iblock))
            UVEL(:,:,k,curtime,iblock) = c0
            VVEL(:,:,k,curtime,iblock) = c0
         endwhere
      enddo

      do n = 1,nt
         do k = 1,km
            where (k > KMT(:,:,iblock))
               TRACER(:,:,k,n,curtime,iblock) = c0
               TRACER(:,:,k,n,oldtime,iblock) = c0
            endwhere
         enddo
      enddo

!-----------------------------------------------------------------------
!
!     reset PSURF(oldtime) to eliminate error in barotropic continuity
!     eqn due to (possible) use of different timestep after restart
!
!     NOTE: use pressure_correction = .false. for exact restart
!
!-----------------------------------------------------------------------

      if (pressure_correction) then

         WORK1 = HU(:,:,iblock)*UBTROP(:,:,curtime,iblock)
         WORK2 = HU(:,:,iblock)*VBTROP(:,:,curtime,iblock)

         !*** use PSURF(oldtime) as temp
         call div(1, PSURF(:,:,oldtime,iblock),WORK1,WORK2,this_block)

         PSURF(:,:,oldtime,iblock) = PSURF(:,:,curtime,iblock) +  &
                            grav*dtp*PSURF(:,:,oldtime,iblock)*   &
                            TAREA_R(:,:,iblock)

      endif
   end do !block loop

   if (pressure_correction) then
      call update_ghost_cells(PSURF(:,:,oldtime,:), bndy_clinic, &
                              field_loc_center, field_type_scalar)
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine read_restart

!***********************************************************************
!BOP
! !IROUTINE: write_restart
! !INTERFACE:

 subroutine write_restart(restart_type)

! !DESCRIPTION:
!  This routine writes all the data necessary for restarting a POP
!  simulation if it is determined that the time has come to write
!  the data.  It also returns the type of restart that was written
!  so that the tavg module can determine whether it need to write
!  a restart file for tavg fields.
!
! !REVISION HISTORY:
!  same as module
!
! !OUTPUT PARAMETERS:

   character(char_len), intent(out) :: &
      restart_type  ! type of restart file written if any
                    ! possible values are: none,restart,even,odd,end

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   logical (log_kind) :: &
      lrestart_write     ! flag to determine whether restart is written

   character (char_len) :: &
      file_suffix          ! suffix to append to root filename

   integer (int_kind) :: &
      k, n,              &! dummy counters
      nu                  ! i/o unit for pointer file writes

   character (char_len) ::  &
      restart_filename,     &! modified file name for restart file
      restart_pointer_file, &! file name for restart pointer file
      short_name,           &! temporary for short name for io fields
      long_name              ! temporary for long  name for io fields

   logical (log_kind) ::   &
      lcoupled_ts           ! flag to check whether coupled time step

   type (datafile) :: &
      restart_file    ! io file descriptor

   type (io_dim) :: &
      i_dim, j_dim, &! dimension descriptors for horiz dims
      k_dim          ! dimension descriptor  for vertical levels

!-----------------------------------------------------------------------
!
!  check to see whether it is time to write files
!
!-----------------------------------------------------------------------

   lrestart_write = .false.
   restart_type = char_blank
   restart_type = 'none'

   !*** write restart files if code is stopping for any reason

   if (check_time_flag(out_stop_now) .and. &
       (lrestart_on .or. leven_odd_on)) then

      lrestart_write = .true.
      restart_type = char_blank
      restart_type = 'end'
   endif

   !*** check if it is time for even/odd output
   !*** (but not if an end file is written)

   if (.not. lrestart_write .and. check_time_flag(evenodd_flag)) then

      lrestart_write = .true.
      restart_type = char_blank

      if (last_even_odd == even) then
         restart_type = 'odd'
         last_even_odd = odd
      else
         restart_type = 'even'
         last_even_odd = even
      endif

   endif

   !*** check if it is time for regularly-scheduled restart output
   !*** note that this option over-rides others

   if (check_time_flag(restart_flag)) then

      lrestart_write = .true.
      restart_type = char_blank
      restart_type = 'restart'

   endif

!-----------------------------------------------------------------------
!
!  the rest of this routine is only executed if it is time to write a
!  restart file
!
!-----------------------------------------------------------------------

   if (lrestart_write) then

!-----------------------------------------------------------------------
!
!  create filename for user-supplied root and append date
!
!-----------------------------------------------------------------------

   restart_filename = char_blank
   file_suffix = char_blank
   call create_restart_suffix(file_suffix, restart_type)

   !*** must split concatenation operator to avoid preprocessor mangling

   restart_filename = trim(restart_outfile)/&
                                            &/'.'/&
                                                  &/trim(file_suffix)

!-----------------------------------------------------------------------
!
!  create output file
!
!-----------------------------------------------------------------------

   restart_file =  construct_file(restart_fmt,                      &
                                  full_name=trim(restart_filename), &
                                  record_length=rec_type_dbl,       &
                                  recl_words=nx_global*ny_global)

!-----------------------------------------------------------------------
!
!  scalar variables in restart file are output as file attributes
!  so define them here
!
!-----------------------------------------------------------------------

   !*** query time_flag routine for present value of lcoupled_ts

   lcoupled_ts = check_time_flag (restart_cpl_ts)

   !*** add defaults as file attributes

   call add_attrib_file(restart_file, 'runid',    runid   )
   call add_attrib_file(restart_file, 'iyear',    iyear   )
   call add_attrib_file(restart_file, 'imonth',   imonth  )
   call add_attrib_file(restart_file, 'iday',     iday    )
   call add_attrib_file(restart_file, 'ihour',    ihour   )
   call add_attrib_file(restart_file, 'iminute',  iminute )
   call add_attrib_file(restart_file, 'isecond',  isecond )
   call add_attrib_file(restart_file, 'iyear0',   iyear0  )
   call add_attrib_file(restart_file, 'imonth0',  imonth0 )
   call add_attrib_file(restart_file, 'iday0',    iday0   )
   call add_attrib_file(restart_file, 'ihour0',   ihour0  )
   call add_attrib_file(restart_file, 'iminute0', iminute0)
   call add_attrib_file(restart_file, 'isecond0', isecond0)
   call add_attrib_file(restart_file, 'dtt',      dtt     )
   call add_attrib_file(restart_file, 'iday_of_year', iday_of_year)
   call add_attrib_file(restart_file, 'iday_of_year_last',       &
                                       iday_of_year_last)
   call add_attrib_file(restart_file, 'elapsed_days',   elapsed_days)
   call add_attrib_file(restart_file, 'elapsed_months', elapsed_months)
   call add_attrib_file(restart_file, 'elapsed_years',  elapsed_years)
   call add_attrib_file(restart_file, 'elapsed_days_this_year',  &
                                       elapsed_days_this_year)
   call add_attrib_file(restart_file, 'seconds_this_day',        &
                                       seconds_this_day)
   call add_attrib_file(restart_file, 'seconds_this_day_next',   &
                                       seconds_this_day_next)
   call add_attrib_file(restart_file, 'seconds_this_year',       &
                                       seconds_this_year)
   call add_attrib_file(restart_file, 'seconds_this_year_next',  &
                                       seconds_this_year_next)
   call add_attrib_file(restart_file, 'nsteps_total',            &
                                             nsteps_total)
   call add_attrib_file(restart_file, 'eod'     , eod     )
   call add_attrib_file(restart_file, 'eod_last', eod_last)
   call add_attrib_file(restart_file, 'eom'     , eom     )
   call add_attrib_file(restart_file, 'eom_last', eom_last)
   call add_attrib_file(restart_file, 'eom_next', eom_next)
   call add_attrib_file(restart_file, 'eoy'     , eoy     )
   call add_attrib_file(restart_file, 'eoy_last', eoy_last)
   call add_attrib_file(restart_file, 'midnight_last', midnight_last)
   call add_attrib_file(restart_file, 'adjust_year_next',        &
                                       adjust_year_next)
   call add_attrib_file(restart_file, 'newday' , newday  )
   call add_attrib_file(restart_file, 'newhour', newhour )
   call add_attrib_file(restart_file, 'leapyear',leapyear)
   call add_attrib_file(restart_file, 'days_in_year', days_in_year)
   call add_attrib_file(restart_file, 'days_in_prior_year',      &
                                       days_in_prior_year)
   call add_attrib_file(restart_file, 'seconds_in_year', &
                                       seconds_in_year)
   call add_attrib_file(restart_file, 'hours_in_year', hours_in_year)
   call add_attrib_file(restart_file, 'tlast_ice',     tlast_ice  )
   call add_attrib_file(restart_file, 'lcoupled_ts',   lcoupled_ts)
   call add_attrib_file(restart_file, 'shf_interp_last',         &
                                       shf_interp_last)
   call add_attrib_file(restart_file, 'sfwf_interp_last',        &
                                       sfwf_interp_last)
   call add_attrib_file(restart_file, 'ws_interp_last',          &
                                       ws_interp_last)
   call add_attrib_file(restart_file, 'ap_interp_last',          &
                                       ap_interp_last)
   call add_attrib_file(restart_file, 'pt_interior_interp_last', &
                                       pt_interior_interp_last)
   call add_attrib_file(restart_file, 's_interior_interp_last',  &
                                       s_interior_interp_last)
   call add_attrib_file(restart_file, 'sum_precip' , sum_precip )
   call add_attrib_file(restart_file, 'precip_fact', precip_fact)
   call add_attrib_file(restart_file, 'ssh_initial', ssh_initial)

   short_name = char_blank
   do k=1,km
      write(short_name,'(a11,i3.3)') 'sal_initial',k
      call add_attrib_file(restart_file,trim(short_name),sal_initial(k))
   end do

!-----------------------------------------------------------------------
!
!  open a file (also writes scalars as attributes to file)
!
!-----------------------------------------------------------------------

   call data_set(restart_file, 'open')

!-----------------------------------------------------------------------
!
!  construct all fields to be written
!
!-----------------------------------------------------------------------

   !*** define dimensions

   i_dim = construct_io_dim('i', nx_global)
   j_dim = construct_io_dim('j', ny_global)
   k_dim = construct_io_dim('k', km)

   UBTROP_CUR = construct_io_field('UBTROP_CUR', i_dim, j_dim,        &
                   long_name='U barotropic velocity at current time', &
                   units    ='cm/s',                                  &
                   grid_loc ='2220',                                  &
                   d2d_array =UBTROP(:,:,curtime,:))

   UBTROP_OLD = construct_io_field('UBTROP_OLD', i_dim, j_dim,        &
                   long_name='U barotropic velocity at old time',     &
                   units    ='cm/s',                                  &
                   grid_loc ='2220',                                  &
                   d2d_array =UBTROP(:,:,oldtime,:))

   VBTROP_CUR = construct_io_field('VBTROP_CUR', i_dim, j_dim,        &
                   long_name='V barotropic velocity at current time', &
                   units    ='cm/s',                                  &
                   grid_loc ='2220',                                  &
                   d2d_array =VBTROP(:,:,curtime,:))

   VBTROP_OLD = construct_io_field('VBTROP_OLD', i_dim, j_dim,        &
                   long_name='V barotropic velocity at old time',     &
                   units    ='cm/s',                                  &
                   grid_loc ='2220',                                  &
                   d2d_array =VBTROP(:,:,oldtime,:))

   PSURF_CUR = construct_io_field('PSURF_CUR', i_dim, j_dim,          &
                   long_name='surface pressure at current time',      &
                   units    ='dyne/cm2',                              &
                   grid_loc ='2110',                                  &
                   d2d_array =PSURF(:,:,curtime,:))

   PSURF_OLD = construct_io_field('PSURF_OLD', i_dim, j_dim,          &
                   long_name='surface pressure at old time',          &
                   units    ='dyne/cm2',                              &
                   grid_loc ='2110',                                  &
                   d2d_array =PSURF(:,:,oldtime,:))

   GRADPX_CUR = construct_io_field('GRADPX_CUR', i_dim, j_dim,        &
                   long_name='sfc press gradient in x at current time',&
                   units    ='dyne/cm3',                              &
                   grid_loc ='2220',                                  &
                   d2d_array =GRADPX(:,:,curtime,:))

   GRADPX_OLD = construct_io_field('GRADPX_OLD', i_dim, j_dim,        &
                   long_name='sfc press gradient in x at old time',   &
                   units    ='dyne/cm3',                              &
                   grid_loc ='2220',                                  &
                   d2d_array =GRADPX(:,:,oldtime,:))

   GRADPY_CUR = construct_io_field('GRADPY_CUR', i_dim, j_dim,        &
                   long_name='sfc press gradient in y at current time',&
                   units    ='dyne/cm3',                              &
                   grid_loc ='2220',                                  &
                   d2d_array =GRADPY(:,:,curtime,:))

   GRADPY_OLD = construct_io_field('GRADPY_OLD', i_dim, j_dim,        &
                   long_name='sfc press gradient in y at old time',   &
                   units    ='dyne/cm3',                              &
                   grid_loc ='2220',                                  &
                   d2d_array =GRADPY(:,:,oldtime,:))

   PGUESSd = construct_io_field('PGUESS', i_dim, j_dim,               &
                   long_name='guess for sfc pressure at new time',    &
                   units    ='dyne/cm2',                              &
                   grid_loc ='2110',                                  &
                   d2d_array =PGUESS)

   if (sfc_layer_type == sfc_layer_varthick) then
      FW_OLDd = construct_io_field('FW_OLD', i_dim, j_dim,            &
                   long_name='fresh water input at old time',         &
                   grid_loc ='2110',                                  &
                   d2d_array = FW_OLD)
   endif

   if (liceform) then
      AQICEd = construct_io_field('AQICE', i_dim, j_dim,              &
                   long_name='accumulated ice melt/heat',             &
                   grid_loc ='2220',                                  &
                   d2d_array = AQICE)
   endif

   UVEL_CUR = construct_io_field('UVEL_CUR', i_dim, j_dim, dim3=k_dim,&
                   long_name='U velocity at current time',            &
                   units    ='cm/s',                                  &
                   grid_loc ='3221',                                  &
                   d3d_array = UVEL(:,:,:,curtime,:))

   UVEL_OLD = construct_io_field('UVEL_OLD', i_dim, j_dim, dim3=k_dim,&
                   long_name='U velocity at old time',                &
                   units    ='cm/s',                                  &
                   grid_loc ='3221',                                  &
                   d3d_array = UVEL(:,:,:,oldtime,:))

   VVEL_CUR = construct_io_field('VVEL_CUR', i_dim, j_dim, dim3=k_dim,&
                   long_name='V velocity at current time',            &
                   units    ='cm/s',                                  &
                   grid_loc ='3221',                                  &
                   d3d_array = VVEL(:,:,:,curtime,:))

   VVEL_OLD = construct_io_field('VVEL_OLD', i_dim, j_dim, dim3=k_dim,&
                   long_name='V velocity at old time',                &
                   units    ='cm/s',                                  &
                   grid_loc ='3221',                                  &
                   d3d_array = VVEL(:,:,:,oldtime,:))

   do n=1,nt
      short_name = char_blank
      short_name = trim(tracer_d(n)%short_name)/&
                                                &/'_CUR'
      long_name = char_blank
      long_name = trim(tracer_d(n)%long_name)/&
                                              &/'at current time'

      TRACER_CUR(n) = construct_io_field(trim(short_name),            &
                   i_dim, j_dim, dim3=k_dim,                          &
                   long_name=trim(long_name),                         &
                   units    =trim(tracer_d(n)%units),                 &
                   grid_loc ='3111',                                  &
                   d3d_array = TRACER(:,:,:,n,curtime,:))
   end do

   do n=1,nt
      short_name = char_blank
      short_name = trim(tracer_d(n)%short_name)/&
                                                &/'_OLD'
      long_name = char_blank
      long_name = trim(tracer_d(n)%long_name)/&
                                              &/'at old time'

      TRACER_OLD(n) = construct_io_field(trim(short_name),            &
                   i_dim, j_dim, dim3=k_dim,                          &
                   long_name=trim(long_name),                         &
                   units    =trim(tracer_d(n)%units),                 &
                   grid_loc ='3111',                                  &
                   d3d_array = TRACER(:,:,:,n,oldtime,:))
   end do

!-----------------------------------------------------------------------
!
!  define all fields to be read
!
!-----------------------------------------------------------------------

   !*** must call in this order for back compatibility

   call data_set (restart_file, 'define', UBTROP_CUR)
   call data_set (restart_file, 'define', UBTROP_OLD)
   call data_set (restart_file, 'define', VBTROP_CUR)
   call data_set (restart_file, 'define', VBTROP_OLD)
   call data_set (restart_file, 'define', PSURF_CUR)
   call data_set (restart_file, 'define', PSURF_OLD)
   call data_set (restart_file, 'define', GRADPX_CUR)
   call data_set (restart_file, 'define', GRADPX_OLD)
   call data_set (restart_file, 'define', GRADPY_CUR)
   call data_set (restart_file, 'define', GRADPY_OLD)
   call data_set (restart_file, 'define', PGUESSd)

   if (sfc_layer_type == sfc_layer_varthick) then
      call data_set (restart_file, 'define', FW_OLDd)
   endif
   if (liceform) then
      call data_set (restart_file, 'define', AQICEd)
   endif

   call data_set (restart_file, 'define', UVEL_CUR)
   call data_set (restart_file, 'define', UVEL_OLD)
   call data_set (restart_file, 'define', VVEL_CUR)
   call data_set (restart_file, 'define', VVEL_OLD)

   do n=1,nt
      call data_set (restart_file, 'define', TRACER_CUR(n))
   end do
   do n=1,nt
      call data_set (restart_file, 'define', TRACER_OLD(n))
   end do

!-----------------------------------------------------------------------
!
!  now we actually write each field
!
!-----------------------------------------------------------------------

   call data_set (restart_file, 'write', UBTROP_CUR)
   call data_set (restart_file, 'write', UBTROP_OLD)
   call data_set (restart_file, 'write', VBTROP_CUR)
   call data_set (restart_file, 'write', VBTROP_OLD)
   call data_set (restart_file, 'write', PSURF_CUR)
   call data_set (restart_file, 'write', PSURF_OLD)
   call data_set (restart_file, 'write', GRADPX_CUR)
   call data_set (restart_file, 'write', GRADPX_OLD)
   call data_set (restart_file, 'write', GRADPY_CUR)
   call data_set (restart_file, 'write', GRADPY_OLD)
   call data_set (restart_file, 'write', PGUESSd)

   if (sfc_layer_type == sfc_layer_varthick) then
      call data_set (restart_file, 'write', FW_OLDd)
   endif
   if (liceform) then
      call data_set (restart_file, 'write', AQICEd)
   endif

   call data_set (restart_file, 'write', UVEL_CUR)
   call data_set (restart_file, 'write', UVEL_OLD)
   call data_set (restart_file, 'write', VVEL_CUR)
   call data_set (restart_file, 'write', VVEL_OLD)
   do n=1,nt
      call data_set (restart_file, 'write', TRACER_CUR(n))
   end do
   do n=1,nt
      call data_set (restart_file, 'write', TRACER_OLD(n))
   end do

!-----------------------------------------------------------------------
!
!  close and destroy file
!
!-----------------------------------------------------------------------

   call data_set (restart_file, 'close')

   if (my_task == master_task) then
     write(stdout,blank_fmt)
     write(stdout,*) ' file written: ', trim(restart_filename)
   endif

   call destroy_file(restart_file)

!-----------------------------------------------------------------------
!
!  if pointer files are used, write filename to pointer file
!
!-----------------------------------------------------------------------

   if (luse_pointer_files) then
      call get_unit(nu)
      if (my_task == master_task) then
         restart_pointer_file = trim(pointer_filename)/&
                                                       &/'.restart'

       open(nu, file=restart_pointer_file, form='formatted', &
                status='unknown')
       write(nu,'(a80)') restart_filename
       close(nu)
     endif
     call release_unit(nu)
   endif

!-----------------------------------------------------------------------
!
!  finished writing file
!
!-----------------------------------------------------------------------

   endif ! lrestart_write

!-----------------------------------------------------------------------
!EOC

 end subroutine write_restart

!***********************************************************************
!BOP
! !IROUTINE: init_restart
! !INTERFACE:

 subroutine init_restart

! !DESCRIPTION:
!  Initializes quantities associated with writing all the data
!  necessary for restarting a simulation.
!
! !REVISION HISTORY:
!  same as module

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables and input namelist
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      n,                 &! tracer loop index
      nml_error           ! namelist i/o error flag

   character (char_len) :: &
      restart_freq_opt      ! input option for freq of restart dumps

   namelist /restart_nml/ restart_freq_opt, restart_freq, &
                          restart_outfile, restart_fmt,   &
                          leven_odd_on, even_odd_freq,    &
                          pressure_correction

!-----------------------------------------------------------------------
!
!  read namelist input and broadcast variables
!
!-----------------------------------------------------------------------

   restart_outfile   = 'd'
   restart_fmt       = 'bin'
   restart_freq_iopt = freq_opt_never
   restart_freq      = 100000
   leven_odd_on      = .false.
   even_odd_freq     = 100000
   pressure_correction = .false.

   if (my_task == master_task) then
      open (nml_in, file=nml_filename, status='old',iostat=nml_error)
      if (nml_error /= 0) then
         nml_error = -1
      else
         nml_error =  1
      endif
      do while (nml_error > 0)
         read(nml_in, nml=restart_nml,iostat=nml_error)
      end do
      if (nml_error == 0) close(nml_in)
   endif

   call broadcast_scalar(nml_error, master_task)
   if (nml_error /= 0) then
      call exit_POP(sigAbort,'ERROR reading restart_nml')
   endif

   if (my_task == master_task) then
      select case (restart_freq_opt)
      case ('never')
         restart_freq_iopt = freq_opt_never
      case ('nyear')
         restart_freq_iopt = freq_opt_nyear
      case ('nmonth')
         restart_freq_iopt = freq_opt_nmonth
      case ('nday')
         restart_freq_iopt = freq_opt_nday
      case ('nhour')
         restart_freq_iopt = freq_opt_nhour
      case ('nsecond')
         restart_freq_iopt = freq_opt_nsecond
      case ('nstep')
         restart_freq_iopt = freq_opt_nstep
      case default
         restart_freq_iopt = -1000
      end select
   endif

   call broadcast_scalar (restart_outfile,      master_task)
   call broadcast_scalar (restart_freq_iopt,    master_task)
   call broadcast_scalar (restart_freq,         master_task)
   call broadcast_scalar (restart_fmt,          master_task)
   call broadcast_scalar (leven_odd_on,         master_task)
   call broadcast_scalar (even_odd_freq,        master_task)
   call broadcast_scalar (pressure_correction,  master_task)

   if (restart_freq_iopt == -1000) then
      call exit_POP(sigAbort,'unknown restart frequency option')
   else if (restart_freq_iopt == freq_opt_never) then
      lrestart_on = .false.
   else
      lrestart_on = .true.
   endif

!-----------------------------------------------------------------------
!
!  create some time flags
!
!-----------------------------------------------------------------------

   restart_flag   = init_time_flag('restart', default=.false.,   &
                                   freq_opt = restart_freq_iopt, &
                                   freq     = restart_freq)

   if (leven_odd_on) then
      last_even_odd = even
      evenodd_flag   = init_time_flag('evenodd', default=.false.,   &
                                      freq_opt = freq_opt_nstep,    &
                                      freq     = even_odd_freq)
   else
      evenodd_flag   = init_time_flag('evenodd', default=.false.,   &
                                      freq_opt = freq_opt_never,    &
                                      freq     = even_odd_freq)
   endif

   restart_cpl_ts = init_time_flag('coupled_ts')
   out_stop_now   = init_time_flag('stop_now') ! get id for stop flag

!-----------------------------------------------------------------------
!EOC

 end subroutine init_restart

!***********************************************************************
!BOP
! !IROUTINE: create_restart_suffix
! !INTERFACE:

 subroutine create_restart_suffix(file_suffix, restart_type)

! !DESCRIPTION:
!  Determines suffix to append to restart files based on restart type.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   character (*), intent(in) :: &
      restart_type           ! type of restart file to be written
                             ! (restart,even,odd,end)

! !OUTPUT PARAMETERS:

   character (char_len), intent(out) :: &
      file_suffix            ! suffix to append to root filename

!EOP
!BOC
!-----------------------------------------------------------------------
!
!     local variable
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      cindx, cindx2,     &! indices into character strings
      len_date            ! length of date string

   character (char_len) :: &
      char_temp            ! temp character space

   character (10) :: &
      cdate          ! date string

!-----------------------------------------------------------------------
!
!  clear character strings
!
!-----------------------------------------------------------------------

   file_suffix = char_blank
   char_temp   = char_blank

!-----------------------------------------------------------------------
!
!  for even, odd, or end, simply add the appropriate string
!
!-----------------------------------------------------------------------

   select case (trim(restart_type))
   case('end')
      file_suffix = trim(runid)/&
                                &/'.end'
   case('even')
      file_suffix = trim(runid)/&
                                &/'.even'
   case('odd')
      file_suffix = trim(runid)/&
                                &/'.odd'

!-----------------------------------------------------------------------
!
!  for a regular restart file, append a date/time string
!
!-----------------------------------------------------------------------

   case('restart')

      if (date_separator == ' ') then
         len_date = 8
         cdate(1:4) = cyear
         cdate(5:6) = cmonth
         cdate(7:8) = cday
         cdate(9:10)= '  '
      else
         len_date = 10
         cdate(1:4) = cyear
         cdate(5:5) = date_separator
         cdate(6:7) = cmonth
         cdate(8:8) = date_separator
         cdate(9:10) = cday
      endif

      select case (restart_freq_iopt)
      case (freq_opt_nyear, freq_opt_nmonth, freq_opt_nday)

         !*** append the date after the runid

         file_suffix = trim(runid)/&
                                   &/'.'/&
                                         &/trim(cdate)

      case (freq_opt_nhour)

         !*** append the date to runid and add hour

         write(file_suffix,'(i2)') ihour
         char_temp = adjustl(file_suffix)

         file_suffix = trim(runid)/&
                                   &/'.'/&
                                   &/trim(cdate)/&
                                   &/'.h'/&
                                   &/trim(char_temp)

      case (freq_opt_nsecond)

         !*** append the date to runid and the elapsed seconds in day

         write (file_suffix,'(i6)') isecond
         char_temp = adjustl(file_suffix)

         file_suffix = trim(runid)/&
                                   &/'.'/&
                                   &/trim(cdate)/&
                                   &/'.s'/&
                                   &/trim(char_temp)

      case (freq_opt_nstep)

         !*** append the step number

         write (file_suffix,'(i10)') nsteps_total
         char_temp = adjustl(file_suffix)

         file_suffix = trim(runid)/&
                                   &/'.'/&
                                         &/trim(char_temp)

      case default
         file_suffix = trim(runid)
      end select

   end select

!-----------------------------------------------------------------------
!EOC

 end subroutine create_restart_suffix

!***********************************************************************

 end module restart

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
