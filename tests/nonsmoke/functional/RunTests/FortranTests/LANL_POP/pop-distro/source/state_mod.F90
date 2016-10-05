!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module state_mod

!BOP
! !MODULE: state_mod
!
! !DESCRIPTION:
!  This module contains routines necessary for computing the density 
!  from model temperature and salinity using an equation of state.
!
!  The module supports four forms of EOS:
!  \begin{enumerate}
!     \item The UNESCO equation of state computed using the 
!           potential-temperature-based bulk modulus from Jackett and 
!           McDougall, JTECH, Vol.12, pp 381-389, April, 1995.
!     \item The faster and more accurate alternative to the UNESCO eos
!           of McDougall, Wright, Jackett and Feistel (hereafter 
!           MWJF, 2001 submission to JTECH).
!     \item a polynomial fit to the full UNESCO EOS
!     \item a simple linear EOS based on constant expansion coeffs
!  \end{enumerate}
!
! !REVISION HISTORY:
!  CVS:$Id: state_mod.F90,v 1.14 2003/02/11 23:33:15 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $

! !USES:

   use kinds_mod
   use blocks
   use distribution
   use domain
   use constants
   use grid
   use io
   use broadcast
   use time_management
   use exit_mod

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:

   public :: state,        &
             init_state,   &
             ref_pressure

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  valid ranges and pressure as function of depth
!
!-----------------------------------------------------------------------

   real (r8), dimension(km) :: & 
      tmin, tmax,        &! valid temperature range for level k
      smin, smax,        &! valid salinity    range for level k
      pressz              ! ref pressure (bars) at each level

!-----------------------------------------------------------------------
!
!  choices for eos type and valid range checks
!
!-----------------------------------------------------------------------

   integer (int_kind), parameter :: &
      state_type_jmcd       = 1,    &! integer ids for state choice
      state_type_mwjf       = 2,    &
      state_type_polynomial = 3,    &
      state_type_linear     = 4

   integer (int_kind) ::    &
      state_itype           ! input state type chosen

   integer (int_kind) ::    &
      state_range_iopt,     &! option for checking valid T,S range
      state_range_freq       ! freq (in steps) for checking T,S range

   integer (int_kind), parameter :: &
      state_range_ignore  = 1, &! do not check T,S range
      state_range_check   = 2, &! check T,S range and report invalid
      state_range_enforce = 3   ! force polynomial eval within range

!-----------------------------------------------------------------------
!
!  UNESCO EOS constants and JMcD bulk modulus constants
!
!-----------------------------------------------------------------------

   !*** for density of fresh water (standard UNESCO)

   real (r8), parameter ::              &
      unt0 =   999.842594_r8,           &
      unt1 =  6.793952e-2_r8,           &
      unt2 = -9.095290e-3_r8,           &
      unt3 =  1.001685e-4_r8,           &
      unt4 = -1.120083e-6_r8,           &
      unt5 =  6.536332e-9_r8

   !*** for dependence of surface density on salinity (UNESCO)

   real (r8), parameter ::              &
      uns1t0 =  0.824493_r8 ,           &
      uns1t1 = -4.0899e-3_r8,           &
      uns1t2 =  7.6438e-5_r8,           &
      uns1t3 = -8.2467e-7_r8,           &
      uns1t4 =  5.3875e-9_r8,           &
      unsqt0 = -5.72466e-3_r8,          &
      unsqt1 =  1.0227e-4_r8,           &
      unsqt2 = -1.6546e-6_r8,           &
      uns2t0 =  4.8314e-4_r8

   !*** from Table A1 of Jackett and McDougall

   real (r8), parameter ::              &
      bup0s0t0 =  1.965933e+4_r8,       &
      bup0s0t1 =  1.444304e+2_r8,       &
      bup0s0t2 = -1.706103_r8   ,       &
      bup0s0t3 =  9.648704e-3_r8,       &
      bup0s0t4 = -4.190253e-5_r8

   real (r8), parameter ::              &
      bup0s1t0 =  5.284855e+1_r8,       &
      bup0s1t1 = -3.101089e-1_r8,       &
      bup0s1t2 =  6.283263e-3_r8,       &
      bup0s1t3 = -5.084188e-5_r8

   real (r8), parameter ::              &
      bup0sqt0 =  3.886640e-1_r8,       &
      bup0sqt1 =  9.085835e-3_r8,       &
      bup0sqt2 = -4.619924e-4_r8

   real (r8), parameter ::              &
      bup1s0t0 =  3.186519_r8   ,       &
      bup1s0t1 =  2.212276e-2_r8,       &
      bup1s0t2 = -2.984642e-4_r8,       &
      bup1s0t3 =  1.956415e-6_r8 

   real (r8), parameter ::              &
      bup1s1t0 =  6.704388e-3_r8,       &
      bup1s1t1 = -1.847318e-4_r8,       &
      bup1s1t2 =  2.059331e-7_r8,       &
      bup1sqt0 =  1.480266e-4_r8 

   real (r8), parameter ::              &
      bup2s0t0 =  2.102898e-4_r8,       &
      bup2s0t1 = -1.202016e-5_r8,       &
      bup2s0t2 =  1.394680e-7_r8,       &
      bup2s1t0 = -2.040237e-6_r8,       &
      bup2s1t1 =  6.128773e-8_r8,       &
      bup2s1t2 =  6.207323e-10_r8

!-----------------------------------------------------------------------
!
!  MWJF EOS coefficients
!
!-----------------------------------------------------------------------

   !*** these constants will be used to construct the numerator
   !*** factor unit change (kg/m^3 -> g/cm^3) into numerator terms

   real (r8), parameter ::                     &
      mwjfnp0s0t0 =   9.99843699e+2_r8 * p001, &
      mwjfnp0s0t1 =   7.35212840e+0_r8 * p001, &
      mwjfnp0s0t2 =  -5.45928211e-2_r8 * p001, &
      mwjfnp0s0t3 =   3.98476704e-4_r8 * p001, &
      mwjfnp0s1t0 =   2.96938239e+0_r8 * p001, &
      mwjfnp0s1t1 =  -7.23268813e-3_r8 * p001, &
      mwjfnp0s2t0 =   2.12382341e-3_r8 * p001, &
      mwjfnp1s0t0 =   1.04004591e-2_r8 * p001, &
      mwjfnp1s0t2 =   1.03970529e-7_r8 * p001, &
      mwjfnp1s1t0 =   5.18761880e-6_r8 * p001, &
      mwjfnp2s0t0 =  -3.24041825e-8_r8 * p001, &
      mwjfnp2s0t2 =  -1.23869360e-11_r8* p001

   !*** these constants will be used to construct the denominator

   real (kind=r8), parameter ::          &
      mwjfdp0s0t0 =   1.0e+0_r8,         &
      mwjfdp0s0t1 =   7.28606739e-3_r8,  &
      mwjfdp0s0t2 =  -4.60835542e-5_r8,  &
      mwjfdp0s0t3 =   3.68390573e-7_r8,  &
      mwjfdp0s0t4 =   1.80809186e-10_r8, &
      mwjfdp0s1t0 =   2.14691708e-3_r8,  &
      mwjfdp0s1t1 =  -9.27062484e-6_r8,  &
      mwjfdp0s1t3 =  -1.78343643e-10_r8, &
      mwjfdp0sqt0 =   4.76534122e-6_r8,  &
      mwjfdp0sqt2 =   1.63410736e-9_r8,  &
      mwjfdp1s0t0 =   5.30848875e-6_r8,  &
      mwjfdp2s0t3 =  -3.03175128e-16_r8, &
      mwjfdp3s0t1 =  -1.27934137e-17_r8

   !*** MWJF numerator coefficients including pressure

   real (r8) ::                                                        &
      mwjfnums0t0, mwjfnums0t1, mwjfnums0t2, mwjfnums0t3,              &
      mwjfnums1t0, mwjfnums1t1, mwjfnums2t0,                           &
      mwjfdens0t0, mwjfdens0t1, mwjfdens0t2, mwjfdens0t3, mwjfdens0t4, &
      mwjfdens1t0, mwjfdens1t1, mwjfdens1t3,                           &
      mwjfdensqt0, mwjfdensqt2

!-----------------------------------------------------------------------
!
!  coeffs and reference values for polynomial eos
!
!-----------------------------------------------------------------------

   real (r8), dimension(:), allocatable :: &
      to,                &! reference temperature for level k
      so,                &! reference salinity    for level k
      sigo                ! reference density     for level k

   real (r8), dimension(:,:), allocatable :: & 
      state_coeffs        ! coefficients for polynomial eos

!-----------------------------------------------------------------------
!
!  parameters for linear eos
!
!-----------------------------------------------------------------------

   real (r8), parameter ::        & 
      T_leos_ref = 19.0_r8,       &! reference T for linear eos (deg C)
      S_leos_ref = 0.035_r8,      &! reference S for linear eos (msu)
      rho_leos_ref = 1.025022_r8, &! ref dens (g/cm3) at ref T,S and 0 bar
      alf = 2.55e-4_r8,           &! expansion coeff -(drho/dT) (gr/cm^3/K)
      bet = 7.64e-1_r8             ! expansion coeff (drho/dS) (gr/cm^3/msu)

!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: state
! !INTERFACE:

 subroutine state(k, kk, TEMPK, SALTK, this_block, &
                         RHOOUT, RHOFULL, DRHODT, DRHODS)

! !DESCRIPTION:
!  Returns the density of water at level k from equation of state
!  $\rho = \rho(d,\theta,S)$ where $d$ is depth, $\theta$ is
!  potential temperature, and $S$ is salinity. the density can be
!  returned as a perturbation (RHOOUT) or as the full density
!  (RHOFULL). Note that only the polynomial EOS choice will return
!  a perturbation density; in other cases the full density is returned
!  regardless of which argument is requested.
!
!  This routine also computes derivatives of density with respect
!  to temperature and salinity at level k from equation of state
!  if requested (ie the optional arguments are present).
!
!  If $k = kk$ are equal the density for level k is returned.
!  If $k \neq kk$ the density returned is that for a parcel
!  adiabatically displaced from level k to level kk.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: &
      k,                    &! depth level index
      kk                     ! level to which water is adiabatically 
                            ! displaced

   real (r8), dimension(nx_block,ny_block), intent(in) :: & 
      TEMPK,             &! temperature at level k
      SALTK               ! salinity    at level k

   type (block), intent(in) :: &
      this_block          ! block information for current block

! !OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block), optional, intent(out) :: & 
      RHOOUT,  &! perturbation density of water
      RHOFULL, &! full density of water
      DRHODT,  &! derivative of density with respect to temperature
      DRHODS    ! derivative of density with respect to salinity

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables:
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      ib,ie,jb,je,       &! extent of physical domain
      bid,               &! local block index
      out_of_range        ! counter for out-of-range T,S values

   real (r8), dimension(nx_block,ny_block) :: &
      TQ,SQ,             &! adjusted T,S
      BULK_MOD,          &! Bulk modulus
      RHO_S,             &! density at the surface
      DRDT0,             &! d(density)/d(temperature), for surface
      DRDS0,             &! d(density)/d(salinity   ), for surface
      DKDT,              &! d(bulk modulus)/d(pot. temp.)
      DKDS,              &! d(bulk modulus)/d(salinity  )
      SQR,DENOMK,        &! work arrays
      WORK1, WORK2, WORK3, WORK4, T2

   real (r8) :: p, p2 ! temporary pressure scalars

!-----------------------------------------------------------------------
!
!  first check for valid range if requested
!
!-----------------------------------------------------------------------

   bid = this_block%local_id

   select case (state_range_iopt)
   case (state_range_ignore)

      !*** prevent problems with garbage on land points or ghost cells

      TQ = min(TEMPK, c1000)
      TQ = max(TQ,   -c1000)
      SQ = min(SALTK, c1000)
      SQ = max(SALTK, c0)

   case (state_range_check)

      if (time_to_do(freq_opt_nstep, state_range_freq)) then

         ib = this_block%ib
         ie = this_block%ie
         jb = this_block%jb
         je = this_block%je

         out_of_range = count((TEMPK(ib:ie,jb:je) < tmin(kk) .or.   &
                               TEMPK(ib:ie,jb:je) > tmax(kk)) .and. &
                               k <= KMT(ib:ie,jb:je,bid))

         if (out_of_range /= 0)                                      &
            write(stdout,'(a9,i6,a44,i3)') 'WARNING: ',out_of_range, &
                  'points outside of valid temp range at level ',kk

         out_of_range = count((SALTK(ib:ie,jb:je) < smin(kk) .or.   &
                               SALTK(ib:ie,jb:je) > smax(kk)) .and. &
                               k <= KMT(ib:ie,jb:je,bid))

         if (out_of_range /= 0)                                      &
            write(stdout,'(a9,i6,a44,i3)') 'WARNING: ',out_of_range, &
                  'points outside of valid salt range at level ',kk

      endif

      TQ = TEMPK
      SQ = SALTK

   case (state_range_enforce)

      TQ = min(TEMPK,tmax(kk))
      TQ = max(TQ,tmin(kk))

      SQ = min(SALTK,smax(kk))
      SQ = max(SQ,smin(kk))

   end select

!-----------------------------------------------------------------------
!
!  now compute density or expansion coefficients
!
!-----------------------------------------------------------------------

   select case (state_itype)

!-----------------------------------------------------------------------
!
!  McDougall, Wright, Jackett, and Feistel EOS
!  test value : rho = 1.033213242 for
!  S = 35.0 PSU, theta = 20.0, pressz = 200.0
!
!-----------------------------------------------------------------------

   case (state_type_mwjf)

      p   = c10*pressz(kk)

      SQ  = c1000*SQ
      SQR = sqrt(SQ)

      !***
      !*** first calculate numerator of MWJF density [P_1(S,T,p)]
      !***

      mwjfnums0t0 = mwjfnp0s0t0 + p*(mwjfnp1s0t0 + p*mwjfnp2s0t0)
      mwjfnums0t1 = mwjfnp0s0t1 
      mwjfnums0t2 = mwjfnp0s0t2 + p*(mwjfnp1s0t2 + p*mwjfnp2s0t2)
      mwjfnums0t3 = mwjfnp0s0t3
      mwjfnums1t0 = mwjfnp0s1t0 + p*mwjfnp1s1t0
      mwjfnums1t1 = mwjfnp0s1t1
      mwjfnums2t0 = mwjfnp0s2t0

      WORK1 = mwjfnums0t0 + TQ * (mwjfnums0t1 + TQ * (mwjfnums0t2 + &
              mwjfnums0t3 * TQ)) + SQ * (mwjfnums1t0 +              &
              mwjfnums1t1 * TQ + mwjfnums2t0 * SQ)

      !***
      !*** now calculate denominator of MWJF density [P_2(S,T,p)]
      !***

      mwjfdens0t0 = mwjfdp0s0t0 + p*mwjfdp1s0t0
      mwjfdens0t1 = mwjfdp0s0t1 + p**3 * mwjfdp3s0t1
      mwjfdens0t2 = mwjfdp0s0t2
      mwjfdens0t3 = mwjfdp0s0t3 + p**2 * mwjfdp2s0t3
      mwjfdens0t4 = mwjfdp0s0t4
      mwjfdens1t0 = mwjfdp0s1t0
      mwjfdens1t1 = mwjfdp0s1t1
      mwjfdens1t3 = mwjfdp0s1t3
      mwjfdensqt0 = mwjfdp0sqt0
      mwjfdensqt2 = mwjfdp0sqt2

      WORK2 = mwjfdens0t0 + TQ * (mwjfdens0t1 + TQ * (mwjfdens0t2 +    &
           TQ * (mwjfdens0t3 + mwjfdens0t4 * TQ))) +                   &
           SQ * (mwjfdens1t0 + TQ * (mwjfdens1t1 + TQ*TQ*mwjfdens1t3)+ &
           SQR * (mwjfdensqt0 + TQ*TQ*mwjfdensqt2))

      DENOMK = c1/WORK2

      if (present(RHOOUT)) then
         RHOOUT  = WORK1*DENOMK
      endif

      if (present(RHOFULL)) then
         RHOFULL = WORK1*DENOMK
      endif

      if (present(DRHODT)) then
         WORK3 = &! dP_1/dT
                 mwjfnums0t1 + TQ * (c2*mwjfnums0t2 +    &
                 c3*mwjfnums0t3 * TQ) + mwjfnums1t1 * SQ

         WORK4 = &! dP_2/dT
                 mwjfdens0t1 + SQ * mwjfdens1t1 +               &
                 TQ * (c2*(mwjfdens0t2 + SQ*SQR*mwjfdensqt2) +  &
                 TQ * (c3*(mwjfdens0t3 + SQ * mwjfdens1t3) +    &
                 TQ *  c4*mwjfdens0t4))

         DRHODT = (WORK3 - WORK1*DENOMK*WORK4)*DENOMK
      endif

      if (present(DRHODS)) then
         WORK3 = &! dP_1/dS
                 mwjfnums1t0 + mwjfnums1t1 * TQ + c2*mwjfnums2t0 * SQ

         WORK4 = mwjfdens1t0 +   &! dP_2/dS
                 TQ * (mwjfdens1t1 + TQ*TQ*mwjfdens1t3) +   &
                 c1p5*SQR*(mwjfdensqt0 + TQ*TQ*mwjfdensqt2)

         DRHODS = (WORK3 - WORK1*DENOMK*WORK4)*DENOMK * c1000
      endif

!-----------------------------------------------------------------------
!
!  Jackett and McDougall EOS
!
!-----------------------------------------------------------------------

   case (state_type_jmcd)

      p   = pressz(kk)
      p2  = p*p

      SQ  = c1000*SQ
      SQR = sqrt(SQ)
      T2  = TQ*TQ


      !***
      !*** first calculate surface (p=0) values from UNESCO eqns.
      !***

      WORK1 = uns1t0 + uns1t1*TQ + & 
             (uns1t2 + uns1t3*TQ + uns1t4*T2)*T2
      WORK2 = SQR*(unsqt0 + unsqt1*TQ + unsqt2*T2)

      RHO_S = unt1*TQ + (unt2 + unt3*TQ + (unt4 + unt5*TQ)*T2)*T2 &
                      + (uns2t0*SQ + WORK1 + WORK2)*SQ


      !***
      !*** now calculate bulk modulus at pressure p from 
      !*** Jackett and McDougall formula
      !***

      WORK3 = bup0s1t0 + bup0s1t1*TQ +                    &
             (bup0s1t2 + bup0s1t3*TQ)*T2 +                &
              p *(bup1s1t0 + bup1s1t1*TQ + bup1s1t2*T2) + &
              p2*(bup2s1t0 + bup2s1t1*TQ + bup2s1t2*T2)
      WORK4 = SQR*(bup0sqt0 + bup0sqt1*TQ + bup0sqt2*T2 + &
                   bup1sqt0*p)

      BULK_MOD  = bup0s0t0 + bup0s0t1*TQ +                    &
                  (bup0s0t2 + bup0s0t3*TQ + bup0s0t4*T2)*T2 + &
                  p *(bup1s0t0 + bup1s0t1*TQ +                &
                     (bup1s0t2 + bup1s0t3*TQ)*T2) +           &
                  p2*(bup2s0t0 + bup2s0t1*TQ + bup2s0t2*T2) + &
                  SQ*(WORK3 + WORK4)

      DENOMK = c1/(BULK_MOD - p)

      !***
      !*** now calculate required fields
      !***

      if (present(RHOOUT)) then
         RHOOUT  = merge(((unt0 + RHO_S)*BULK_MOD*DENOMK)*p001, &
                         c0, KMT(:,:,bid) >= k)
      endif

      if (present(RHOFULL)) then
         RHOFULL = merge(((unt0 + RHO_S)*BULK_MOD*DENOMK)*p001, &
                         c0, KMT(:,:,bid) >= k)
      endif

      if (present(DRHODT)) then
         DRDT0 =  unt1 + c2*unt2*TQ +                      &
                  (c3*unt3 + c4*unt4*TQ + c5*unt5*T2)*T2 + &
                  (uns1t1 + c2*uns1t2*TQ +                 &
                   (c3*uns1t3 + c4*uns1t4*TQ)*T2 +         &
                   (unsqt1 + c2*unsqt2*TQ)*SQR )*SQ

         DKDT  = bup0s0t1 + c2*bup0s0t2*TQ +                       &
                 (c3*bup0s0t3 + c4*bup0s0t4*TQ)*T2 +               &
                 p *(bup1s0t1 + c2*bup1s0t2*TQ + c3*bup1s0t3*T2) + &
                 p2*(bup2s0t1 + c2*bup2s0t2*TQ) +                  &
                 SQ*(bup0s1t1 + c2*bup0s1t2*TQ + c3*bup0s1t3*T2 +  &
                     p  *(bup1s1t1 + c2*bup1s1t2*TQ) +             &
                     p2 *(bup2s1t1 + c2*bup2s1t2*TQ) +             &
                     SQR*(bup0sqt1 + c2*bup0sqt2*TQ))

         DRHODT = merge ((DENOMK*(DRDT0*BULK_MOD -                    &
                           pressz(kk)*(unt0+RHO_S)*DKDT*DENOMK))*p001, &
                         c0, KMT(:,:,bid) >= k)
      endif

      if (present(DRHODS)) then
         DRDS0  = c2*uns2t0*SQ + WORK1 + c1p5*WORK2
         DKDS = WORK3 + c1p5*WORK4

         DRHODS = merge (DENOMK*(DRDS0*BULK_MOD -                    &
                          pressz(kk)*(unt0+RHO_S)*DKDS*DENOMK),       &
                         c0, KMT(:,:,bid) >= k)

      endif
 
!-----------------------------------------------------------------------
!
!  polynomial EOS
!
!-----------------------------------------------------------------------

   case (state_type_polynomial)

      TQ = TQ - to(kk)
      SQ = SQ - so(kk) - 0.035_r8

      if (present(RHOOUT)) then

         !*** density as perturbation from a k-level reference

         RHOOUT = merge((state_coeffs(1,kk) +            &
                        (state_coeffs(4,kk) +            &
                         state_coeffs(7,kk)*SQ)*SQ +     &
                        (state_coeffs(3,kk) +            &
                         state_coeffs(8,kk)*SQ +         &
                         state_coeffs(6,kk)*TQ)*TQ)*TQ + &
                        (state_coeffs(2,kk) +            &
                        (state_coeffs(5,kk) +            &
                         state_coeffs(9,kk)*SQ)*SQ)*SQ   &
                     ,c0, KMT(:,:,bid) >= k)
      endif

      if (present(RHOFULL)) then

         !*** full density (adding the reference density)

         RHOFULL=merge(((state_coeffs(1,kk) +             &
                        (state_coeffs(4,kk) +             &
                         state_coeffs(7,kk)*SQ)*SQ +      &
                        (state_coeffs(3,kk) +             &
                         state_coeffs(8,kk)*SQ +          &
                         state_coeffs(6,kk)*TQ)*TQ)*TQ +  &
                        (state_coeffs(2,kk) +             &
                        (state_coeffs(5,kk) +             &
                         state_coeffs(9,kk)*SQ)*SQ)*SQ) + &
                         sigo(kk)*p001 + c1               &
                     ,c1, KMT(:,:,bid) >= k)
      endif

      if (present(DRHODT)) then
         DRHODT = merge (state_coeffs(1,kk) +        &
                        (state_coeffs(4,kk) +        &
                         state_coeffs(7,kk)*SQ)*SQ + &
                     (c2*state_coeffs(3,kk) +        &
                      c2*state_coeffs(8,kk)*SQ +     &
                      c3*state_coeffs(6,kk)*TQ)*TQ,  &
                  c0, KMT(:,:,bid) >= k)
      endif

      if (present(DRHODS)) then
         DRHODS = merge((state_coeffs(4,kk) +        &
                      c2*state_coeffs(7,kk)*SQ +     &
                         state_coeffs(8,kk)*TQ)*TQ + &
                         state_coeffs(2,kk) +        &
                     (c2*state_coeffs(5,kk) +        &
                      c3*state_coeffs(9,kk)*SQ)*SQ,  &
                  c0, KMT(:,:,bid) >= k)
      endif
 
!-----------------------------------------------------------------------
!
!  linear EOS
!
!-----------------------------------------------------------------------

   case (state_type_linear)

      if (present(RHOOUT )) RHOOUT  = bet*(SALTK - S_leos_ref) - &
                                      alf*(TEMPK - T_leos_ref)
      if (present(RHOFULL)) RHOFULL = rho_leos_ref + & 
                                      bet*(SALTK - S_leos_ref) - &
                                      alf*(TEMPK - T_leos_ref)
      if (present(DRHODT )) DRHODT  = -alf
      if (present(DRHODS )) DRHODS  =  bet

!-----------------------------------------------------------------------

   end select

!-----------------------------------------------------------------------
!EOC

 end subroutine state

!***********************************************************************
!BOP
! !IROUTINE: ref_pressure
! !INTERFACE:

 function ref_pressure(k)

! !DESCRIPTION:
!  This function returns a reference pressure at level k.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: &
      k                  ! vertical level index

! !OUTPUT PARAMETERS:

   real (r8) ::         &
      ref_pressure       ! reference pressure at level k

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  return pre-computed reference pressure at level k
!
!-----------------------------------------------------------------------

    ref_pressure = pressz(k)

!-----------------------------------------------------------------------
!EOC

 end function ref_pressure

!***********************************************************************
!BOP
! !IROUTINE: init_state
! !INTERFACE:

 subroutine init_state

! !DESCRIPTION:
!  Initializes eos choice and initializes coefficients for
!  equation of state.
!
! !REVISION HISTORY:
!  same as module

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  namelist for input choice and coefficient file
!
!-----------------------------------------------------------------------

   integer (int_kind) ::  & 
      k,                  &! vertical loop index
      nu,                 &! unit number for coeff input file
      m,                  &! dummy loop index
      rec_length,         &! record length for input polynomial file
      nml_error            ! namelist i/o error flag

   character (char_len) :: &
      state_choice,        &! choice of which EOS to use
      state_file,          &! file containing polynomial eos coeffs
      state_range_opt       ! option for checking valid T,S range

   namelist /state_nml/ state_choice, state_file,         & 
                        state_range_opt, state_range_freq

!-----------------------------------------------------------------------
!
!  read input choice for EOS type
!
!-----------------------------------------------------------------------

   state_itype = state_type_mwjf
   state_file  = 'internal'
   state_range_iopt = state_range_ignore
   state_range_freq = 100000

   if (my_task == master_task) then
      open (nml_in, file=nml_filename, status='old',iostat=nml_error)
      if (nml_error /= 0) then
         nml_error = -1
      else
         nml_error =  1
      endif
      do while (nml_error > 0)
         read(nml_in, nml=state_nml,iostat=nml_error)
      end do
      if (nml_error == 0) close(nml_in)
   endif

   call broadcast_scalar(nml_error, master_task)
   if (nml_error /= 0) then
      call exit_POP(sigAbort,'ERROR reading state_nml')
   endif

   if (my_task == master_task) then
      write(stdout,delim_fmt)
      write(stdout,blank_fmt)
      write(stdout,'(a25)') 'Equation of state options'
      write(stdout,blank_fmt)
      write(stdout,delim_fmt)

      select case (state_choice(1:4))
      case ('jmcd')
         state_itype = state_type_jmcd
         write(stdout,'(a29)') 'Using Jackett & McDougall EOS'
      case ('mwjf')
         state_itype = state_type_mwjf
         write(stdout,'(a48)') &
            'Using McDougall, Wright ,Jackett and Feistel EOS'
      case ('poly')
         state_itype = state_type_polynomial
         write(stdout,'(a20)') 'Using polynomial EOS'
      case ('line')
         state_itype = state_type_linear
         write(stdout,'(a16)') 'Using linear EOS'
      case default
         state_itype = -1000
      end select

      select case (state_range_opt)
      case ('ignore')
         state_range_iopt = state_range_ignore
      case ('check')
         state_range_iopt = state_range_check
      case ('enforce')
         state_range_iopt = state_range_enforce
      case default
         state_range_iopt = -1000
      end select

   endif

   call broadcast_scalar(state_itype,      master_task)
   call broadcast_scalar(state_file,       master_task)
   call broadcast_scalar(state_range_iopt, master_task)
   call broadcast_scalar(state_range_freq, master_task)

   if (state_itype == -1000) then
      call exit_POP(sigAbort, &
                    'unknown choice for equation of state type')
   endif
   if (state_range_iopt == -1000) then
      call exit_POP(sigAbort,'unknown choice for state range option')
   endif

!-----------------------------------------------------------------------
!
!  write eos options to stdout
!
!-----------------------------------------------------------------------

   if (my_task == master_task) then

      select case (state_range_iopt)
      case (state_range_ignore)
         write(stdout,'(a30)') 'No checking of valid T,S range'
      case (state_range_check)
         write(stdout,'(a30,i6,a7)') 'Valid T,S range checked every ', &
                         state_range_freq,' steps.'
      case (state_range_enforce)
         write(stdout,'(a37)') 'EOS computed as if in valid T,S range'
      end select

      if (state_itype == state_type_polynomial) then
         if (state_file == 'internal') then
            write(stdout,'(a39)') & 
              'Calculating EOS coefficients internally'
         else
            write(stdout,*) &
              'Reading EOS coefficients from file: ', trim(state_file)
         endif
      endif

   endif

!-----------------------------------------------------------------------
!
!  calculate pressure as function of depth
!
!-----------------------------------------------------------------------

   do k=1,km
      pressz(k) = pressure(zt(k)*mpercm)
   end do

!-----------------------------------------------------------------------
!
!  initialize various arrays, constants for each state type
!
!-----------------------------------------------------------------------

   select case (state_itype)

!-----------------------------------------------------------------------
!
!  McDougall, Wright, Jackett, and Feistel
!
!-----------------------------------------------------------------------

   case (state_type_mwjf)

      tmin =  -2.0_r8  ! limited   on the low  end
      tmax = 999.0_r8  ! unlimited on the high end
      smin =   0.0_r8  ! limited   on the low  end
      smax = 0.999_r8  ! unlimited on the high end

!-----------------------------------------------------------------------
!
!  Jackett and McDougall
!
!-----------------------------------------------------------------------

   case (state_type_jmcd)

      tmin = -2.0_r8  ! valid pot. temp. range for level k
      tmax = 40.0_r8 
      smin =  0.0_r8  ! valid salinity   range for level k
      smax = 0.042_r8 

!-----------------------------------------------------------------------
!
!  polynomial - initialize polynomial coefficients and ranges
!
!-----------------------------------------------------------------------

   case (state_type_polynomial)

      allocate (to(km), so(km), sigo(km))
      allocate (state_coeffs(9,km))

      if (state_file == 'internal') then
         call init_state_coeffs
      else
         call get_unit(nu)
         if (my_task == master_task) then
            inquire(iolength = rec_length) to
            open(nu,file=state_file,access='direct',form='unformatted',&
                    recl=rec_length,status='unknown')
            read(nu,rec=1) to
            read(nu,rec=2) so
            read(nu,rec=3) sigo
            do m = 1,9
               read(nu,rec=m+3) state_coeffs(m,:)
            enddo
            if (state_range_iopt /= state_range_ignore) then
               read(nu,rec=13) tmin
               read(nu,rec=14) tmax
               read(nu,rec=15) smin
               read(nu,rec=16) smax
            endif
            close (nu)
         endif
         call release_unit(nu)

         call broadcast_array(to,   master_task)
         call broadcast_array(so,   master_task)
         call broadcast_array(sigo, master_task)
         do m = 1,9
            call broadcast_array(state_coeffs(m,:), master_task)
         enddo
         if (state_range_iopt /= state_range_ignore) then
            call broadcast_array(tmin, master_task)
            call broadcast_array(tmax, master_task)
            call broadcast_array(smin, master_task)
            call broadcast_array(smax, master_task)
         endif
      endif

      !***
      !*** convert smin,smax to model units
      !***

      if (state_range_iopt /= state_range_ignore) then
         smin = smin*ppt_to_salt
         smax = smax*ppt_to_salt
      endif

!-----------------------------------------------------------------------
!
!  linear EOS - intialize valid range
!
!-----------------------------------------------------------------------

   case (state_type_linear)

      tmin = -2.0_r8  ! valid pot. temp. range for level k
      tmax = 40.0_r8 
      smin =  0.0_r8  ! valid salinity   range for level k
      smax = 0.042_r8 

   end select

!-----------------------------------------------------------------------
!EOC

 end subroutine init_state

!***********************************************************************
!BOP
! !IROUTINE: init_state_coeffs
! !INTERFACE:

 subroutine init_state_coeffs

! !DESCRIPTION:
!  This routine calculates coefficients for the polynomial equation of 
!  state option.  This routine calculates the 9 coefficients of a third
!  order (in temperature and salinity) polynomial approximation to the 
!  equation of state for sea water.  The coefficients are calculated by
!  first sampling a range of temperature and salinity at each depth.  
!  The density is then computed at each of the sampled points using the 
!  full UNESCO equation of state.  A least squares method is used to 
!  fit the coefficients of the polynomial to the sampled points.  More 
!  specifically, the densities calculated from the polynomial 
!  formula are in the form of sigma anomalies.  The method is
!  taken from that described by Bryan and Cox (1972).  
!  By default, the program uses the equation of state set by the
!  Joint Panel on Oceanographic Tables and Standards (UNESCO, 1981)
!  an described by Gill (1982).
!
!  This was originall adopted from the GFDL MOM model and extensively 
!  modified to improve readability (February (1999).
!
!  Additionally, improvements to the accuracy of the EOS have been
!  added (March 1999).  There are two improvements: (a) A more 
!  accurate depth-to-pressure conversion based on the Levitus 1994 
!  climatology, and (b) Use of a more recent formula for the 
!  computation of potential temperature (Bryden, 1973) in place of an 
!  older algorithm (Fofonoff, 1962). See Dukowicz (2000).
!
!  References:
!
!    Bryan, K. and M. Cox, An approximate equation of state
!          for numerical models of ocean circulation, J. Phys.
!          Oceanogr., 2, 510-514, 1972.
!
!    Bryden, H.L., New polynomials for thermal expansion, adiabatic
!          temperature gradient and potential temperature of sea water,
!          Deap-Sea Res., 20, 401-408, 1973.
!
!    Dukowicz, J. K., 2000: Reduction of Pressure and Pressure
!          Gradient Errors in Ocean Simulations, J. Phys. Oceanogr.,
!          submitted.
!
!    Fofonoff, N., The Sea: Vol 1, (ed. M. Hill). Interscience,
!          New York, 1962, pp 3-30.
!
!    Gill, A., Atmosphere-Ocean Dynamics: International Geophysical
!          Series No. 30. Academic Press, London, 1982, pp 599-600.
!
!    Hanson, R., and C. Lawson, Extensions and applications of the
!          Householder algorithm for solving linear least squares
!          problems. Math. Comput., 23, 787-812, 1969.
!
!    UNESCO, 10th report of the joint panel on oceanographic tables
!          and standards. UNESCO Tech. Papers in Marine Sci. No. 36,
!          Paris, 1981.
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

   integer (int_kind), parameter ::  &
      nsample_salt = 5,              &! number of pts to sample in S 
      nsample_temp = 2*nsample_salt, &! number of pts in sample in T
      nsample_all  = nsample_salt*nsample_temp  ! total sample points

   integer (int_kind) ::  &
      i,j,k,n,            &! dummy loop counters
      nsample,            &! sample number
      itmax,              &! max iters for least squares
      nlines,             &! number of data lines in km output
      nwords_last_line     ! num data words in last line of km output

   real (r8) ::           &
      dtemp, dsalt,       &! T,S increments for curve fit
      temp_sample,        &! sample temperature in temp range
      salt_sample,        &! sample salinity    in salt range
      density,            &! density in MKS units
      tanom, sanom,       &! T,S anomalies (from level average)
      enorm                ! norm of least squares residual

   real (r8), dimension(:), allocatable :: &
      avg_theta       ! average of sample pot. temps for level k

   real (r8), dimension(nsample_all) :: &
      tsamp,         &! temperature     at each sample point 
      ssamp,         &! salinity        at each sample point
      thsamp,        &! potential temp  at each sample point
      sigma,         &! density         at each sample point
      sigman          ! density anomaly at each sample point

   real (r8), dimension(nsample_all,9) :: &
      lsqarray        ! least squares array to find coeffs

   real (r8), dimension(9) :: &
      lsqcoeffs       ! polynomial coeffs returned by lsq routine

   !***
   !*** bounds for polynomial fit using a reference model of 
   !*** 33 levels from surface to 8000m at z=(k-1)*250 meters
   !*** The user should review the appropriateness of the 
   !*** reference values set below, and modify them if the 
   !*** intended modelling application could be expected to yield 
   !*** temperature and salinity values outside of the ranges set 
   !*** by default.
   !***

   real (r8), dimension(33) ::                &
      trefmin = (/ -2.0_r8, -2.0_r8, -2.0_r8, &
                 & -2.0_r8, -1.0_r8, -1.0_r8, &
                 & -1.0_r8, -1.0_r8, -1.0_r8, &
                 & -1.0_r8, -1.0_r8, -1.0_r8, &
                 & -1.0_r8, -1.0_r8, -1.0_r8, &
                 & -1.0_r8, -1.0_r8, -1.0_r8, &
                 & -1.0_r8,  0.0_r8,  0.0_r8, &
                 &  0.0_r8,  0.0_r8,  0.0_r8, &
                 &  0.0_r8,  0.0_r8,  0.0_r8, &
                 &  0.0_r8,  0.0_r8,  0.0_r8, &
                 &  0.0_r8,  0.0_r8,  0.0_r8 /)

   real (r8), dimension(33) ::                &
      trefmax = (/ 29.0_r8, 19.0_r8, 14.0_r8, &
                 & 11.0_r8,  9.0_r8,  7.0_r8, &
                 &  7.0_r8,  7.0_r8,  7.0_r8, &
                 &  7.0_r8,  7.0_r8,  7.0_r8, &
                 &  7.0_r8,  7.0_r8,  7.0_r8, &
                 &  7.0_r8,  7.0_r8,  7.0_r8, &
                 &  7.0_r8,  7.0_r8,  7.0_r8, &
                 &  7.0_r8,  7.0_r8,  7.0_r8, &
                 &  7.0_r8,  7.0_r8,  7.0_r8, &
                 &  7.0_r8,  7.0_r8,  7.0_r8, &
                 &  7.0_r8,  7.0_r8,  7.0_r8 /)

   real (r8), dimension(33) ::                &
      srefmin = (/ 28.5_r8, 33.7_r8, 34.0_r8, &
                 & 34.1_r8, 34.2_r8, 34.4_r8, &
                 & 34.5_r8, 34.5_r8, 34.6_r8, &
                 & 34.6_r8, 34.6_r8, 34.6_r8, &
                 & 34.6_r8, 34.6_r8, 34.6_r8, &
                 & 34.6_r8, 34.6_r8, 34.6_r8, &
                 & 34.6_r8, 34.6_r8, 34.6_r8, &
                 & 34.6_r8, 34.6_r8, 34.7_r8, &
                 & 34.7_r8, 34.7_r8, 34.7_r8, &
                 & 34.7_r8, 34.7_r8, 34.7_r8, &
                 & 34.7_r8, 34.7_r8, 34.7_r8 /)

   real (r8), dimension(33) ::                &
      srefmax = (/ 37.0_r8, 36.6_r8, 35.8_r8, &
                 & 35.7_r8, 35.3_r8, 35.1_r8, &
                 & 35.1_r8, 35.0_r8, 35.0_r8, &
                 & 35.0_r8, 35.0_r8, 35.0_r8, &
                 & 35.0_r8, 35.0_r8, 35.0_r8, &
                 & 35.0_r8, 35.0_r8, 35.0_r8, &
                 & 35.0_r8, 35.0_r8, 35.0_r8, &
                 & 35.0_r8, 35.0_r8, 35.0_r8, &
                 & 35.0_r8, 35.0_r8, 35.0_r8, &
                 & 35.0_r8, 35.0_r8, 35.0_r8, &
                 & 35.0_r8, 35.0_r8, 35.0_r8 /)

!-----------------------------------------------------------------------
!
!  set the temperature and salinity ranges to be used for each
!  model level when performing the polynomial fitting
!
!-----------------------------------------------------------------------

   do k=1,km
      i = int(zt(k)*mpercm/250.0_r8) + 1
      tmin(k) = trefmin(i)
      tmax(k) = trefmax(i)
      smin(k) = srefmin(i)
      smax(k) = srefmax(i)
   end do

   allocate (avg_theta(km))

!-----------------------------------------------------------------------
!
!  loop over all model levels
!
!-----------------------------------------------------------------------

   do k=1,km

!-----------------------------------------------------------------------
!
!     sample the temperature range with nsample_temp points
!     and the salinity range with nsample_salt points and
!     create an array of possible combinations of T,S samples
!
!-----------------------------------------------------------------------

      dtemp = (tmax(k)-tmin(k)) / (nsample_temp-c1)
      dsalt = (smax(k)-smin(k)) / (nsample_salt-c1)

      nsample = 0
      do i=1,nsample_temp
         temp_sample = tmin(k) + (i-1)*dtemp
         do j=1,nsample_salt
            nsample = nsample + 1
            salt_sample = smin(k) + (j-1)*dsalt
            tsamp(nsample) = temp_sample
            ssamp(nsample) = salt_sample
         end do
      end do

!-----------------------------------------------------------------------
!
!     loop over the number of samples
!
!-----------------------------------------------------------------------

      !***
      !*** initialize averaging sums
      !***

      to  (k) = c0
      so  (k) = c0
      sigo(k) = c0
      avg_theta(k) = c0

      do n=1,nsample_all

!-----------------------------------------------------------------------
!
!        calculate density (sigma) for each t,s combintion at
!        this depth using full unesco equation of state
!        unesco returns density (kg per m**3)
!
!-----------------------------------------------------------------------

         call unesco(tsamp(n),ssamp(n),pressz(k),density)

         sigma(n) = density - 1.0e3_r8

!-----------------------------------------------------------------------
!
!        calculate potential temp. from from insitu temperature,
!        salinity, and pressure
!
!-----------------------------------------------------------------------

         call potem(tsamp(n),ssamp(n),pressz(k),thsamp(n))

!-----------------------------------------------------------------------
!
!        accumulate level averages and end loop over samples
!
!-----------------------------------------------------------------------

         to  (k) = to  (k) + tsamp(n)
         so  (k) = so  (k) + ssamp(n)
         sigo(k) = sigo(k) + sigma(n)
         avg_theta(k) = avg_theta(k) + thsamp(n)

      end do ! loop over samples

!-----------------------------------------------------------------------
!
!     complete layer averages
!
!-----------------------------------------------------------------------

      to  (k) = to  (k)/real(nsample_all)
      so  (k) = so  (k)/real(nsample_all)
      sigo(k) = sigo(k)/real(nsample_all)
      avg_theta(k) = avg_theta(k)/real(nsample_all)

!-----------------------------------------------------------------------
!
!     recompute average (reference) density based on level average 
!     values of T, S, and pressure.
!     use average potential temperature in place of average temp
!
!-----------------------------------------------------------------------

      call unesco (to(k), so(k), pressz(k), density)

      sigo(k) = density - 1.0e3_r8
      to  (k) = avg_theta(k)

!-----------------------------------------------------------------------
!
!     fill array for least squares routine with anomalies
!     and their products (the terms in the desired cubic
!     polynomial.
!
!-----------------------------------------------------------------------

      do n=1,nsample_all

         tsamp(n) = thsamp(n)   !*** replace temp with potential temp

         tanom = tsamp(n) - to(k)
         sanom = ssamp(n) - so(k)
         sigman(n) = sigma(n) - sigo(k)

         lsqarray(n,1) = tanom
         lsqarray(n,2) = sanom
         lsqarray(n,3) = tanom * tanom
         lsqarray(n,4) = tanom * sanom
         lsqarray(n,5) = sanom * sanom
         lsqarray(n,6) = tanom * tanom * tanom
         lsqarray(n,7) = sanom * sanom * tanom
         lsqarray(n,8) = tanom * tanom * sanom
         lsqarray(n,9) = sanom * sanom * sanom

      end do

!-----------------------------------------------------------------------
!
!     LSQL2 is  a Jet Propulsion Laboratory subroutine that
!     computes the least squares fit in an iterative manner for
!     overdetermined systems. it is called here to iteratively
!     determine polynomial coefficients for cubic polynomials
!     that will best fit the density at the sampled points
!
!-----------------------------------------------------------------------

      itmax = 4
      call lsqsl2 (lsqarray, sigman, lsqcoeffs, itmax, enorm, 1.0e-7_r8)

!-----------------------------------------------------------------------
!
!     store coefficients
!
!-----------------------------------------------------------------------

      state_coeffs(:,k) = lsqcoeffs

!-----------------------------------------------------------------------
!
!     end of loop over levels
!
!-----------------------------------------------------------------------

   end do

!-----------------------------------------------------------------------
!
!  rescale some of the coefficients and reference values for correct 
!  units
!
!-----------------------------------------------------------------------

   do k=1,km
      sigo(k) = p001 * sigo(k)
      so  (k) = ppt_to_salt * so  (k) - 0.035_r8

      state_coeffs(1,k) = 1.e-3_r8 * state_coeffs(1,k)
      state_coeffs(3,k) = 1.e-3_r8 * state_coeffs(3,k)
      state_coeffs(5,k) = 1.e+3_r8 * state_coeffs(5,k)
      state_coeffs(6,k) = 1.e-3_r8 * state_coeffs(6,k)
      state_coeffs(7,k) = 1.e+3_r8 * state_coeffs(7,k)
      state_coeffs(9,k) = 1.e+6_r8 * state_coeffs(9,k)
   end do
   do k=1,km
      sigo(k) = sigo(k) * c1000
   end do

   deallocate (avg_theta)

!-----------------------------------------------------------------------
!EOC

 end subroutine init_state_coeffs

!***********************************************************************
!BOP
! !IROUTINE: potem
! !INTERFACE:

 subroutine potem (temp, salt, pbars, theta)

! !DESCRIPTION:
!  This subroutine calculates potential temperature as a function
!  of in-situ temperature, salinity, and pressure.
!
!  Reference:
!    Bryden, H.L., New polynomials for thermal expansion, adiabatic
!          temperature gradient and potential temperature of sea water,
!          Deap-Sea Res., 20, 401-408, 1973.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   real (r8), intent(in) :: &
      temp,                 &! in-situ temperature [degrees C]
      salt,                 &! salinity [per mil]
      pbars                  ! pressure [bars]

! !OUTPUT PARAMETERS:

   real (r8), intent(out) :: &
      theta                   ! potential temperature [degrees C]

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   real (r8) :: prss2, prss3, potmp

!-----------------------------------------------------------------------
!
!  compute powers of several variables
!
!-----------------------------------------------------------------------

   prss2 = pbars*pbars
   prss3 = prss2*pbars

!-----------------------------------------------------------------------
!
!  compute potential temperature from polynomial
!
!-----------------------------------------------------------------------

   potmp = pbars*(3.6504e-4_r8 + temp*(8.3198e-5_r8 +   &
           temp*(-5.4065e-7_r8 + temp*4.0274e-9_r8))) + &
           pbars*(salt - 35.0_r8)*(1.7439e-5_r8 -       &
           temp*2.9778e-7_r8) + prss2*(8.9309e-7_r8 +   &
           temp*(-3.1628e-8_r8 + temp*2.1987e-10_r8)) - &
           4.1057e-9_r8*prss2*(salt - 35.0_r8) +        &
           prss3*(-1.6056e-10_r8 + temp*5.0484e-12_r8)

   theta = temp - potmp

!-----------------------------------------------------------------------
!EOC

 end subroutine potem

!***********************************************************************
!BOP
! !IROUTINE: unesco
! !INTERFACE:

 subroutine unesco (temp, salt, pbars, rho)

! !DESCRIPTION:
!  This subroutine calculates the density of seawater using the
!  standard equation of state recommended by unesco (1981).
!
!  References:
!
!    Gill, A., Atmosphere-Ocean Dynamics: International Geophysical
!         Series No. 30. Academic Press, London, 1982, pp 599-600.
!
!    UNESCO, 10th report of the joint panel on oceanographic tables
!          and standards. UNESCO Tech. Papers in Marine Sci. No. 36,
!          Paris, 1981.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   real (r8), intent(in) :: &
      temp,                 &! in-situ temperature [degrees C]
      salt,                 &! salinity [practical salinity units]
      pbars                  ! pressure [bars]

! !OUTPUT PARAMETERS:

   real (r8), intent(out) :: &
      rho                    ! density in kilograms per cubic meter

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   real (r8) :: rw, rsto, xkw, xksto, xkstp, &
                tem2, tem3, tem4, tem5, slt2, st15, pbar2 

!-----------------------------------------------------------------------
!
!  compute powers of several variables
!
!-----------------------------------------------------------------------

   tem2 = temp**2
   tem3 = temp**3
   tem4 = temp**4
   tem5 = temp**5
   slt2 = salt**2
   st15 = salt**(1.5_r8) 
   pbar2 = pbars**2

!-----------------------------------------------------------------------
!
!  compute density
!
!-----------------------------------------------------------------------

   rw = unt0 + unt1*temp + unt2*tem2 + unt3*tem3 + & 
                           unt4*tem4 + unt5*tem5

   rsto = rw +                                       &
          (uns1t0 + uns1t1*temp + uns1t2*tem2 +      &
                    uns1t3*tem3 + uns1t4*tem4)*salt  &
        + (unsqt0 + unsqt1*temp + unsqt2*tem2)*st15 + uns2t0*slt2

   xkw =    1.965221e+4_r8 +                             &
            1.484206e+2_r8*temp - 2.327105e+0_r8*tem2 +  &
            1.360477e-2_r8*tem3 - 5.155288e-5_r8*tem4

   xksto =  xkw +                                             &
            (5.46746e+1_r8      - 6.03459e-1_r8*temp +        &
             1.09987e-2_r8*tem2 - 6.1670e-5_r8*tem3)*salt     &
          + (7.944e-2_r8  +                                   &
             1.6483e-2_r8 *temp - 5.3009e-4_r8*tem2)*st15

   xkstp =  xksto +                                               &
            (3.239908e+0_r8     + 1.43713e-3_r8*temp +            &
             1.16092e-4_r8*tem2 - 5.77905e-7_r8*tem3)*pbars       &
          + (2.2838e-3_r8       - 1.0981e-5_r8 *temp -            &
             1.6078e-6_r8 *tem2)*pbars*salt                       &
          + 1.91075e-4_r8       *pbars*st15                       &
          + (8.50935e-5_r8      - 6.12293e-6_r8*temp +            &
                                        5.2787e-8_r8 *tem2)*pbar2 &
          + (-9.9348e-7_r8      + 2.0816e-8_r8 *temp +            &
             9.1697e-10_r8*tem2)*pbar2*salt

   rho =   rsto / (c1 - pbars/xkstp)

!-----------------------------------------------------------------------
!EOC

 end subroutine unesco

!***********************************************************************
!BOP
! !IROUTINE: pressure
! !INTERFACE:

 function pressure(depth)

! !DESCRIPTION:
!  This function computes pressure in bars from depth in meters
!  using a mean density derived from depth-dependent global 
!  average temperatures and salinities from Levitus 1994, and 
!  integrating using hydrostatic balance.
!
!  References:
!
!     Levitus, S., R. Burgett, and T.P. Boyer, World Ocean Atlas 
!          1994, Volume 3: Salinity, NOAA Atlas NESDIS 3, US Dept. of 
!          Commerce, 1994.
!
!     Levitus, S. and T.P. Boyer, World Ocean Atlas 1994, 
!          Volume 4: Temperature, NOAA Atlas NESDIS 4, US Dept. of 
!          Commerce, 1994.
!
!     Dukowicz, J. K., 2000: Reduction of Pressure and Pressure
!          Gradient Errors in Ocean Simulations, J. Phys. Oceanogr.,
!          submitted.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   real (r8), intent(in) :: depth    ! depth in meters

! !OUTPUT PARAMETERS:

   real (r8) :: pressure   ! pressure [bars]

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  convert depth in meters to pressure in bars
!
!-----------------------------------------------------------------------

   pressure = 0.059808_r8*(exp(-0.025_r8*depth) - c1)     &
            + 0.100766_r8*depth + 2.28405e-7_r8*depth**2

!-----------------------------------------------------------------------
!EOC

 end function pressure

!***********************************************************************
!BOP
! !IROUTINE: lsqsl2
! !INTERFACE:

 subroutine lsqsl2 (a,b,x,itmax,enorm,eps1)

! !DESCRIPTION:
!  This routine is a modification of a very old ugly bit of code
!  from way back in March,1968 and originally written by a
!  gentleman (presumably) named R. Hanson of unknown descent.
!  The routine finds a linear least squares minimization of Ax-b.
!
! !REVISION HISTORY:
!  same as module

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(:,:), intent(inout) :: &
     a                   ! input matrix A with the value of each of 
                         ! the fitting functions at each sample point

! !INPUT PARAMETERS:

   real (r8), dimension(size(a,dim=1)), intent(in) :: &
     b                   ! data points to fit

   integer (int_kind), intent(in) ::  &
     itmax               ! max number of iterations for least-square

   real (r8), intent(in) ::  &
     eps1                ! some sort of tolerance

! !OUTPUT PARAMETERS:

   real (r8), dimension(size(a,dim=2)), intent(out) :: &
     x                   ! coefficient of each fitting function
                         ! that provides optimal fit of sampled pts

   real (r8), intent(out) ::  &
     enorm               ! norm of least squares residuals

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: irank,it

   real (r8) :: sj,dp,up,bp,aj

   real (r8), dimension(size(a,dim=1),size(a,dim=2)) :: &
     aa

   real (r8), dimension(size(a,dim=1) + 8*size(a,dim=2)) :: &
     s

   real (r8), dimension(size(a,dim=1) + 4*size(a,dim=2)) :: &
     r

   integer (int_kind) :: i,j,k,l,m,n,isw,ip,lm,irp1,irm1,n1,ns,k1,k2, &
                         j1, j2, j3, j4, j5, j6, j7, j8, j9

   real (r8) :: epsloc,am,a1,sp,top,top1,top2,enm1,a2


!-----------------------------------------------------------------------
!
!  initialize various useful numbers
!
!-----------------------------------------------------------------------

   k=size(a,dim=1)
   n=size(a,dim=2)

   it = 0
   irank = 0
   epsloc = 1.e-16_r8

   isw = 1
   l   = min(k,n)
   m   = max(k,n)
   j1  = m
   j2  = j1 + n
   j3  = j2 + n
   j4  = j3 + l
   j5  = j4 + l
   j6  = j5 + l
   j7  = j6 + l
   j8  = j7 + n
   j9  = j8 + n

   lm  = l

!-----------------------------------------------------------------------
!
!  equilibrate columns of a (1)-(2).
!
!-----------------------------------------------------------------------

   aa = a

   do j=1,n
     am = c0
     do i=1,k
       am = max(am,abs(a(i,j)))
     end do

     !*** s(m+n+1)-s(m+2n) contains scaling for output variables.

     s(j2+j) = c1/am
     do i=1,k
       a(i,j) = a(i,j)*s(j2+j)
     end do
   end do

!-----------------------------------------------------------------------
!
!  compute column lengths
!
!-----------------------------------------------------------------------

   do j=1,n
     s(j7+j) = s(j2+j)
   end do

!-----------------------------------------------------------------------
!
!   s(m+1)-s(m+ n) contains variable permutations.
!  set permutation to identity.
!
!-----------------------------------------------------------------------

   do j=1,n
     s(j1+j) = j
   end do

!-----------------------------------------------------------------------
!
!  begin elimination on the matrix a with orthogonal matrices .
!  ip=pivot row
!
!-----------------------------------------------------------------------

   do ip=1,lm

     dp = c0
     k2 = ip
     do j=ip,n
       sj = c0
       do i=ip,k
         sj = sj + a(i,j)**2
       end do
       if (sj >= dp) then
         dp = sj
         k2 = j
       endif
     end do

!-----------------------------------------------------------------------
!
!    maximize (sigma)**2 by column interchange.
!     exchange columns if necessary.
!
!-----------------------------------------------------------------------

     if (k2 /= ip) then
       do i=1,k
         a1      = a(i,ip)
         a(i,ip) = a(i,k2)
         a(i,k2) = a1
       end do

       a1       = s(j1+k2)
       s(j1+k2) = s(j1+ip)
       s(j1+ip) = a1

       a1       = s(j7+k2)
       s(j7+k2) = s(j7+ip)
       s(j7+ip) = a1
     endif

     if (ip /= 1) then
       a1 = c0
       do i=1,ip-1
         a1 = a1 + a(i,ip)**2
       end do
       if (a1 > c0) go to 190
     end if
     if (dp > c0) go to 200

!-----------------------------------------------------------------------
!
!    test for rank deficiency.
!
!-----------------------------------------------------------------------

 190 if (sqrt(dp/a1) > eps1) go to 200
     irank = ip-1
     go to 260

!-----------------------------------------------------------------------
!
!    (eps1) rank is deficient.
!
!-----------------------------------------------------------------------

 200 sp = sqrt(dp)

!-----------------------------------------------------------------------
!
!    begin front elimination on column ip.
!    sp=sqroot(sigma**2).
!
!-----------------------------------------------------------------------

     bp = c1/(dp+sp*abs(a(ip,ip)))

!-----------------------------------------------------------------------
!
!    store beta in s(3n+1)-s(3n+l).
!
!-----------------------------------------------------------------------

     if (ip == k) bp = c0
     r(k+2*n+ip) = bp
     up = sign(sp + abs(a(ip,ip)), a(ip,ip))
     if (ip < k) then
       if (ip < n) then
         do j=ip+1,n
           sj = c0
           do i=ip+1,k
             sj=sj+a(i,j)*a(i,ip)
           end do
           sj = sj + up*a(ip,j)
           sj = bp*sj  !*** sj=yj now
 
           do i=ip+1,k
             a(i,j) = a(i,j) - a(i,ip)*sj
           end do
           a(ip,j) = a(ip,j) - sj*up
         end do
       endif
       a(ip,ip)    = -sign(sp,a(ip,ip))
       r(k+3*n+ip) = up
     endif

   end do
   irank = lm
 260 irp1 = irank+1
   irm1 = irank-1

   if (irank == 0 .or. irank == n) go to 360

!-----------------------------------------------------------------------
!
!  begin back processing for rank deficiency case
!   if irank is less than n.
!
!-----------------------------------------------------------------------

   do j=1,n
     l = min(j,irank)

     !*** unscale columns for rank deficient matrices
     do i=1,l
       a(i,j) = a(i,j)/s(j7+j)
     end do

     s(j7+j) = c1
     s(j2+j) = c1
   end do

   ip = irank

 300 sj = c0
   do j=irp1,n
     sj = sj + a(ip,j)**2
   end do
   sj = sj + a(ip,ip)**2
   aj = sqrt(sj)
   up = sign(aj+abs(a(ip,ip)), a(ip,ip))

!-----------------------------------------------------------------------
!
!  ip th element of u vector calculated.
!  bp = 2/length of u squared.
!
!-----------------------------------------------------------------------

   bp = c1/(sj+abs(a(ip,ip))*aj)

   if (ip-1 > 0) then
     do i=1,ip-1
       dp = a(i,ip)*up
       do j=irp1,n
         dp = dp + a(i,j)*a(ip,j)
       end do
       dp = dp/(sj+abs(a(ip,ip))*aj)

       !*** calc. (aj,u), where aj=jth row of a

       a(i,ip) = a(i,ip) - up*dp

       !*** modify array a.

       do j=irp1,n
         a(i,j) = a(i,j) - a(ip,j)*dp
       end do
     end do
   endif

   a(ip,ip) = -sign(aj,a(ip,ip))

!-----------------------------------------------------------------------
!
!  calc. modified pivot.
!  save beta and ip th element of u vector in r array.
!
!-----------------------------------------------------------------------

   r(k+ip)   = bp
   r(k+n+ip) = up

!-----------------------------------------------------------------------
!
!  test for end of back processing.
!
!-----------------------------------------------------------------------

   if (ip-1 > 0) then
     ip=ip-1
     go to 300
   endif

 360 continue

   do j=1,k
     r(j) = b(j)
   end do

!-----------------------------------------------------------------------
!
!  set initial x vector to zero.
!
!-----------------------------------------------------------------------

   do j=1,n
     x(j) = c0
   end do
   if (irank == 0) go to 690

!-----------------------------------------------------------------------
!
!  apply q to rt. hand side.
!
!-----------------------------------------------------------------------

 390 do ip=1,irank
     sj = r(k+3*n+ip)*r(ip)
     if (ip+1 <= k) then
       do i=ip+1,k
         sj = sj + a(i,ip)*r(i)
       end do
     endif
     bp = r(k+2*n+ip)
     if (ip+1 <= k) then
       do i=ip+1,k
         r(i) = r(i) - bp*a(i,ip)*sj
       end do
     endif
     r(ip) = r(ip) - bp*r(k+3*n+ip)*sj
   end do

   do j=1,irank
     s(j) = r(j)
   end do

   enorm = c0
   if (irp1.gt.k) go to 510
   do j=irp1,k
     enorm = enorm + r(j)**2
   end do
   enorm = sqrt(enorm)
   go to 510

 460 do j=1,n
     sj = c0
     ip = s(j1+j)
     do i=1,k
       sj = sj + r(i)*aa(i,ip)
     end do

     !*** apply to rt. hand side.  apply scaling.

     r(k+n+j) = sj*s(j2+ip)
   end do

   s(1) = r(k+n+1)/a(1,1)
   if (n /= 1) then
     do j=2,n
       sj = c0
       do i=1,j-1
         sj = sj + a(i,j)*s(i)
       end do
       s(j) = (r(k+j+n)-sj)/a(j,j)
     end do
   endif

!-----------------------------------------------------------------------
!
!  entry to continue iterating.  solves tz = c = 1st irank
!  components of qb .
!
!-----------------------------------------------------------------------

 510 s(irank) = s(irank)/a(irank,irank)
   if (irm1 /= 0) then
     do j=1,irm1
       n1 = irank-j
       sj = 0.
       do i=n1+1,irank
         sj = sj + a(n1,i)*s(i)
       end do
       s(n1) = (s(n1)-sj)/a(n1,n1)
     end do
   endif

!-----------------------------------------------------------------------
!
!  z calculated.  compute x = sz.
!
!-----------------------------------------------------------------------

   if (irank /= n) then
     do j=irp1,n
       s(j) = c0
     end do
     do i=1,irank
       sj = r(k+n+i)*s(i)
       do j=irp1,n
         sj = sj + a(i,j)*s(j)
       end do
       do j=irp1,n
         s(j) = s(j) - a(i,j)*r(k+i)*sj
       end do
       s(i) = s(i) - r(k+i)*r(k+n+i)*sj
     end do
   endif

!-----------------------------------------------------------------------
!
!  increment for x of minimal length calculated.
!
!-----------------------------------------------------------------------

   do i=1,n
     x(i) = x(i) + s(i)
   end do

!-----------------------------------------------------------------------
!
!  calc. sup norm of increment and residuals
!
!-----------------------------------------------------------------------

   top1 = c0
   do j=1,n
     top1 = max(top1,abs(s(j))*s(j7+j))
   end do

   do i=1,k
     sj = c0
     do j=1,n
       ip = s(j1+j)
       sj = sj + aa(i,ip)*x(j)*s(j2+ip)
     end do
     r(i) = b(i) - sj
   end do

!-----------------------------------------------------------------------
!
!  calc. sup norm of x.
!
!-----------------------------------------------------------------------

   top = c0
   do j=1,n
     top = max(top,abs(x(j))*s(j7+j))
   end do

!-----------------------------------------------------------------------
!
!  compare relative change in x with tolerance eps .
!
!-----------------------------------------------------------------------

   if (top1-top*epsloc > 0) then
     if (it-itmax < 0) then
       it = it+1
       if (it /= 1) then
         if (top1 > p25*top2) go to 690
       endif
       top2 = top1
       if (isw == 1) then
         go to 390
       else if (isw ==2) then
         go to 460
       endif
     endif
     it=0
   endif
 690 sj=c0
   do j=1,k
     sj = sj + r(j)**2
   end do
   enorm = sqrt(sj)

   if (irank == n .and. isw == 1) then
     enm1 = enorm

     !*** save x array

     do j=1,n
       r(k+j) = x(j)
     end do
     isw = 2
     it = 0
     go to 460
   endif

!-----------------------------------------------------------------------
!
!  choose best solution
!
!-----------------------------------------------------------------------

   if (irank >= n .and. enorm > enm1) then
     do j=1,n
       x(j) = r(k+j)
     end do
     enorm = enm1
   endif

!-----------------------------------------------------------------------
!
!  norm of ax - b located in the cell enorm .
!  rearrange variables.
!
!-----------------------------------------------------------------------

   do j=1,n
     s(j) = s(j1+j)
   end do
   do j=1,n
     do i=j,n
       ip = s(i)
       if (j == ip) go to 780
     end do
 780 s(i) = s(j)
     s(j) = j
     sj   = x(j)
     x(j) = x(i)
     x(i) = sj
   end do

!-----------------------------------------------------------------------
!
!  scale variables.
!
!-----------------------------------------------------------------------

   do j=1,n
     x(j) = x(j)*s(j2+j)
   end do

!-----------------------------------------------------------------------
!EOC

 end subroutine lsqsl2

!***********************************************************************

 end module state_mod

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
