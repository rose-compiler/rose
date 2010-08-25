!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module constants

!BOP
! !MODULE: constants
!
! !DESCRIPTION:
!  This module defines a variety of physical and numerical constants
!  used throughout the Parallel Ocean Program.
!
! !REVISION HISTORY:
!  CVS:$Id: constants.F90,v 1.12 2002/12/11 17:09:08 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $

! !USES:

   use kinds_mod

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:

   public :: init_constants

! !DEFINED PARAMETERS:

   ! numbers

   real (r8), parameter, public :: &
      c0     =    0.0_r8   ,&
      c1     =    1.0_r8   ,&
      c2     =    2.0_r8   ,&
      c3     =    3.0_r8   ,&
      c4     =    4.0_r8   ,&
      c5     =    5.0_r8   ,&
      c8     =    8.0_r8   ,&
      c10    =   10.0_r8   ,&
      c16    =   16.0_r8   ,&
      c1000  = 1000.0_r8   ,&
      c10000 =10000.0_r8   ,&
      c1p5   =    1.5_r8   ,&
!     p33    = c1/c3       ,&
      p33    = c1          ,&
      p5     = 0.500_r8    ,&
      p25    = 0.250_r8    ,&
      p125   = 0.125_r8    ,&
      p001   = 0.001_r8    ,&
      eps    = 1.0e-10_r8  ,&
      eps2   = 1.0e-20_r8  ,&
      bignum = 1.0e+30_r8

   real (r4), parameter, public :: &
      undefined = -12345._r4

   real (r8), public :: &
      pi, pih, pi2            ! pi, pi/2 and 2pi

   !*** location of fields for staggered grids

   integer (int_kind), parameter, public ::   &
      field_loc_unknown  =  0, &
      field_loc_noupdate = -1, &
      field_loc_center   =  1, &
      field_loc_NEcorner =  2, &
      field_loc_Nface    =  3, &
      field_loc_Eface    =  4

   !*** field type attribute - necessary for handling
   !*** changes of direction across tripole boundary

   integer (int_kind), parameter, public ::   &
      field_type_unknown  =  0, &
      field_type_noupdate = -1, &
      field_type_scalar   =  1, &
      field_type_vector   =  2, &
      field_type_angle    =  3

   !  common formats for formatted output

   character (1), parameter, public :: &
      char_delim = ','

   character (9), parameter, public :: &
      delim_fmt = "(72('-'))"

   character (5), parameter, public :: &
      blank_fmt = "(' ')"

!  !PUBLIC DATA MEMBERS:

   character (char_len), public ::  &
      char_blank          ! empty character string

   ! physical constants
   ! note that most internal ocean constants are in cgs units
   !  while atmosphere and surface flux constants are sometimes
   !  in MKS units
   ! these constants are defined in an init routine to allow
   !  CSM shared constants to over-ride

   real (r8), public ::   &
      grav               ,&! gravit. accel. (cm/s^2)
      omega              ,&! angular vel. of Earth 1/s
      radius             ,&! radius of Earth (cm)
      rho_air            ,&! ambient air density (kg/m^3)
      rho_fw             ,&! density of fresh water (g/cm^3)
      rho_sw             ,&! density of salt water (g/cm^3)
      cp_sw              ,&! specific heat salt water
      cp_air             ,&! heat capacity of air (J/kg/K)
      sound              ,&! speed of sound (cm/s)
      vonkar             ,&! von Karman constant
      emissivity         ,&!
      stefan_boltzmann   ,&! W/m^2/K^4
      latent_heat_vapor  ,&! lat heat of vaporization (erg/g)
      latent_heat_fusion ,&! lat heat of fusion (erg/g)
      sea_ice_salinity   ,&! salinity of sea ice formed (psu)
      ocn_ref_salinity     ! ocean reference salinity (psu)

   real (r8), public :: &
      radian                           ! degree-radian conversion

   !  conversion factors

   real (r8), public :: &
      T0_Kelvin        ,&! zero point for Celcius
      mpercm           ,&! meters per cm
      cmperm           ,&! cm per meter
      salt_to_ppt      ,&! salt (g/g) to ppt
      ppt_to_salt      ,&! salt ppt to g/g
      mass_to_Sv       ,&! mass flux to Sverdrups
      heat_to_PW       ,&! heat flux to Petawatts
      salt_to_Svppt    ,&! salt flux to Sv*ppt
      salt_to_mmday    ,&! salt to water (mm/day)
      momentum_factor  ,&! wind stress (N/m^2) to vel flux (cm^2/s^2)
      hflux_factor     ,&! heat flux (W/m^2) to temp flux (C*cm/s)
      fwflux_factor    ,&! fw flux (kg/m^2/s) to salt((msu/psu)*cm/s)
      salinity_factor  ,&! fw flux (kg/m^2/s) to salt flux (msu*cm/s)
      sflux_factor     ,&! salt flux (kg/m^2/s) to salt flux (msu*cm/s)
      fwmass_to_fwflux   ! fw flux (kg/m^2/s) to fw flux (cm/s)

!EOP
!BOC
!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: init_constants
! !INTERFACE:

 subroutine init_constants

! !DESCRIPTION:
!  This subroutine initializes constants that are best defined
!  at run time (e.g. pi).
!
! !REVISION HISTORY:

!EOP
!BOC
!-----------------------------------------------------------------------

   integer (int_kind) :: n

!-----------------------------------------------------------------------
!
!  more numbers and character constants
!
!-----------------------------------------------------------------------

   pi  = c4*atan(c1)
   pi2 = c2*pi
   pih = p5*pi

   radian = 180.0_r8/pi

   do n=1,char_len
     char_blank(n:n) = ' '
   end do

!-----------------------------------------------------------------------
!
!  physical constants
!  note that most internal ocean constants are in cgs units
!   while atmosphere and surface flux constants are sometimes
!   in MKS units
!
!  some of these constants may be over-ridden by CSM-defined
!  constants if the CSM shared constants are available
!
!-----------------------------------------------------------------------

   T0_Kelvin = 273.16_r8             ! zero point for Celcius
   grav      = 980.6_r8              ! gravit. accel. (cm/s^2)
   omega     = 7.292123625e-5_r8     ! angular vel. of Earth 1/s
   radius    = 6370.0e5_r8           ! radius of Earth (cm)
   rho_air   = 1.2_r8                ! ambient air density (kg/m^3)
   rho_sw    = 4.1_r8/3.996_r8       ! density of salt water (g/cm^3)
   rho_fw    = 1.0_r8                ! avg. water density (g/cm^3)
   cp_sw     = 3.996e7_r8            ! specific heat salt water
   cp_air    = 1005.0_r8             ! heat capacity of air (J/kg/K)
   sound     = 1.5e5_r8              ! speed of sound (cm/s)
   vonkar    = 0.4_r8                ! von Karman constant
   emissivity         = 1.0_r8       !
   stefan_boltzmann   = 567.0e-10_r8 !  W/m^2/K^4
   latent_heat_vapor  = 2.5e6_r8     ! lat heat of vaporization (erg/g)
   latent_heat_fusion = 3.34e9_r8    ! lat heat of fusion (erg/g)
   sea_ice_salinity   =  4.0_r8      ! (psu)
   ocn_ref_salinity   = 34.7_r8      ! (psu)

!-----------------------------------------------------------------------
!
!  conversion factors
!
!-----------------------------------------------------------------------

   mpercm        = .01_r8          ! meters per cm
   cmperm        = 100._r8         ! cm per meter
   salt_to_ppt   = 1000._r8        ! salt (g/g) to ppt
   ppt_to_salt   = 1.e-3_r8        ! salt ppt to g/g
   mass_to_Sv    = 1.0e-12_r8      ! mass flux to Sverdrups
   heat_to_PW    = 4.186e-15_r8    ! heat flux to Petawatts
   salt_to_Svppt = 1.0e-9_r8       ! salt flux to Sv*ppt
   salt_to_mmday = 3.1536e+5_r8    ! salt to water (mm/day)

!-----------------------------------------------------------------------
!
!  convert windstress (N/m^2) to velocity flux (cm^2/s^2):
!  -------------------------------------------------------
!    windstress in (N/m^2) = (kg/s^2/m) = 10(g/s^2/cm) = 10(dyn/cm^2)
!    assume here that density of seawater rho = 1 (g/cm^3)
!
!    vel_flux   = windstress / rho
!    vel_flux (cm^2/s^2) = windstress (N/m^2)*10 (g/s^2/cm)/(N/m^2)
!                          / [1 (g/cm^3)]
!                        = windstress (N/m^2)
!                          * momentum_factor ((cm^2/s^2)/N/m^2)
!    ==>  momentum_factor = 10
!
!-----------------------------------------------------------------------

   momentum_factor = 10.0_r8

!-----------------------------------------------------------------------
!
!  convert heat, solar flux (W/m^2) to temperature flux (C*cm/s):
!  --------------------------------------------------------------
!    heat_flux in (W/m^2) = (J/s/m^2) = 1000(g/s^3)
!    density of seawater rho_sw in (g/cm^3)
!    specific heat of seawater cp_sw in (erg/g/C) = (cm^2/s^2/C)
!
!    temp_flux          = heat_flux / (rho_sw*cp_sw)
!    temp_flux (C*cm/s) = heat_flux (W/m^2)
!                         * 1000 (g/s^3)/(W/m^2)
!                         / [(rho_sw*cp_sw) (g/cm/s^2/C)]
!
!                       = heat_flux (W/m^2)
!                         * hflux_factor (C*cm/s)/(W/m^2)
!
!    ==>  hflux_factor = 1000/(rho_sw*cp_sw)
!
!-----------------------------------------------------------------------

   hflux_factor = 1000.0_r8/(rho_sw*cp_sw)

!-----------------------------------------------------------------------
!
!  convert fresh water flux (kg/m^2/s) to virtual salt flux (msu*cm/s):
!  --------------------------------------------------------------------
!    ocean reference salinity in (o/oo=psu)
!    density of freshwater rho_fw = 1.0 (g/cm^3)
!    h2o_flux in (kg/m^2/s) = 0.1 (g/cm^2/s)
!
!    salt_flux            = - h2o_flux * ocn_ref_salinity / rho_fw
!    salt_flux (msu*cm/s) = - h2o_flux (kg/m^2/s)
!                           * ocn_ref_salinity (psu)
!                           * 1.e-3 (msu/psu)
!                           * 0.1 (g/cm^2/s)/(kg/m^2/s)
!                           / 1.0 (g/cm^3)
!                         = - h2o_flux (kg/m^2/s)
!                           * ocn_ref_salinity (psu)
!                           * fwflux_factor (cm/s)(msu/psu)/(kg/m^2/s)
!
!    ==>  fwflux_factor = 1.e-4
!
!    salt_flux(msu*cm/s) = h2oflux(kg/m^2/s) * salinity_factor
!
!    ==> salinity_factor = - ocn_ref_salinity(psu) * fwflux_factor
!
!-----------------------------------------------------------------------

   fwflux_factor   = 1.e-4_r8
   salinity_factor = -ocn_ref_salinity*fwflux_factor

!-----------------------------------------------------------------------
!
!  convert salt flux (kg/m^2/s) to salt flux (msu*cm/s):
!  -----------------------------------------------------
!    density of freshwater rho_fw = 1.0 (g/cm^3)
!    salt_flux_kg in (kg/m^2/s) = 0.1 (g/cm^2/s)
!
!    salt_flux            = - h2o_flux * ocn_ref_salinity / rho_fw
!    salt_flux (msu*cm/s) = salt_flux_kg (kg/m^2/s)
!                           * 0.1 (g/cm^2/s)/(kg/m^2/s)
!                           / 1.0 (g/cm^3)
!                         = salt_flux_kg (kg/m^2/s)
!                           * sflux_factor (msu*cm/s)/(kg/m^2/s)
!
!    ==>  sflux_factor = 0.1
!
!-----------------------------------------------------------------------

   sflux_factor = 0.1_r8

!-----------------------------------------------------------------------
!
!  convert fresh water mass flux (kg/m^2/s) to fresh water flux (cm/s):
!  --------------------------------------------------------------------
!    density of freshwater rho_fw = 1.0 (g/cm^3)
!    h2o_flux in (kg/m^2/s) = 0.1 (g/cm^2/s)
!
!    fw_flux  = h2o_flux / rho_fw
!    fw_flux (cm/s) = h2o_flux (kg/m^2/s)
!                     * 0.1 (g/cm^2/s)/(kg/m^2/s)
!                     / 1.0 (g/cm^3)
!                   = h2o_flux (kg/m^2/s)
!                     * fwmass_to_fwflux (cm/s)/(kg/m^2/s)
!
!    ==>  fwmass_to_fwflux = 0.1
!
!-----------------------------------------------------------------------

   fwmass_to_fwflux = 0.1_r8

!EOC
!-----------------------------------------------------------------------

 end subroutine init_constants

!***********************************************************************

 end module constants

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
