!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module solvers

!BOP
! !MODULE: solvers
!
! !DESCRIPTION:
!  This module contains routines and operators for solving the elliptic
!  system for surface pressure in the barotropic mode.
!
! !REVISION HISTORY:
!  CVS:$Id: solvers.F90,v 1.14 2002/12/02 13:45:10 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $

! !USES:

   use kinds_mod
   use blocks
   use distribution
   use domain
   use constants
   use boundary
   use global_reductions
   use gather_scatter
   use broadcast
   use grid
   use io
   use time_management
   use exit_mod

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:

   public :: init_solvers, &
             elliptic_solver

! !PUBLIC DATA MEMBERS:

   real (r8), dimension (nx_block,ny_block,max_blocks_clinic), &
      public :: & 
      AC,                &! time-independent part of center 9pt weight
      A0_CLINIC           ! time-dependent center weight of 9pt operator
                          !   in baroclinic block distribution

   integer (int_kind), public :: &
      solv_sum_iters      ! accumulated no of iterations (diagnostic)

   real (r8), public ::  &
      rms_residual        ! residual (also a diagnostic)

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  other operator and preconditioner weights for barotropic operator
!
!-----------------------------------------------------------------------

   real (r8), dimension (nx_block,ny_block,max_blocks_tropic) :: & 
      A0,AN,AE,ANE,         &! barotropic (9pt) operator coefficients
      RCALCT_B               ! land mask in barotropic distribution 

   real (r8), dimension (:,:,:), allocatable :: & 
      PCC,PCN,PCS,PCE,PCW,  &! preconditioner coefficients
      PCNE,PCSE,PCNW,PCSW

!-----------------------------------------------------------------------
!
!  scalar convergence-related variables
!
!-----------------------------------------------------------------------

   logical (log_kind) :: &
      lprecond            ! true if computed preconditioner to be used

   real (r8) ::          &
      solv_convrg,       &! convergence error criterion
      sor,               &! for jacobi solver
      resid_norm          ! residual normalization

   integer (int_kind), parameter :: &
      solv_pcg = 1,      &! predefined solver types
      solv_cgr = 2,      &
      solv_jac = 3

   integer (int_kind) :: &
      solv_itype,        &! integer solver method (1=pcg, 2=cgr, 3=jac)
      solv_max_iters,    &! max number of iterations for solver
      solv_ncheck         ! check convergence every ncheck iterations

!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: elliptic_solver
! !INTERFACE:

 subroutine elliptic_solver(PRESS, RHS)

! !DESCRIPTION:
!  Solves the elliptic equation for surface pressure by calling
!  the requested solver routine.  Also redistributes necessary
!  array to the barotropic distribution of blocks for better performance
!  of the solver.
!  The elliptic equation is
!  \begin{equation}
!     AF = B
!  \end{equation}
!  where $F$ is a field (eg surface pressure), $B$ is the right hand side
!  and $A$ is the operator defined as
!  \begin{equation}
!     AF = a \nabla\cdot(H \nabla F)
!  \end{equation}
!  where $a$ is the cell area.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,max_blocks_clinic), &
      intent(in) :: &
      RHS                  ! right-hand-side of linear system
                           !  for blocks in baroclinic distribution

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,max_blocks_clinic), &
     intent(inout) :: &
     PRESS              ! on input,  initial guess
                        ! on output, final solution for sfc pressure

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   real (r8), dimension(nx_block,ny_block,max_blocks_tropic) :: &
      P_TROPIC,         &! surface pressure in barotropic distribution
      RHS_TROPIC         ! right hand side  in barotropic distribution

!-----------------------------------------------------------------------
!
!  switch to the barotropic distribution for iterative solvers
!
!-----------------------------------------------------------------------

   call redistribute_blocks(A0,         distrb_tropic, &
                            A0_CLINIC,  distrb_clinic)
   call redistribute_blocks(P_TROPIC,   distrb_tropic, &
                            PRESS,      distrb_clinic)
   call redistribute_blocks(RHS_TROPIC, distrb_tropic, &
                            RHS,        distrb_clinic)

!-----------------------------------------------------------------------
!
!  call proper routine based on user choice of solver
!
!-----------------------------------------------------------------------

   if (my_task < distrb_tropic%nprocs) then
      select case(solv_itype)
      case (solv_pcg)
         call pcg(P_TROPIC,RHS_TROPIC)      ! precond conjg grad solver
      case (solv_cgr)
         call cgr(P_TROPIC,RHS_TROPIC)      ! conjugate residual solver
      case (solv_jac)
         call jacobi(P_TROPIC,RHS_TROPIC)   ! simple jacobi solver
      end select
   endif

!-----------------------------------------------------------------------
!
!  switch solution back to the baroclinic distribution
!
!-----------------------------------------------------------------------

   call redistribute_blocks(PRESS,    distrb_clinic, &
                            P_TROPIC, distrb_tropic)

!-----------------------------------------------------------------------
!EOC

 end subroutine elliptic_solver

!***********************************************************************
!BOP
! !IROUTINE: init_solvers
! !INTERFACE:

 subroutine init_solvers

! !DESCRIPTION:
!  This routine initializes choice of solver, calculates the 
!  coefficients of the 9-point stencils for the barotropic operator and
!  reads in a preconditioner if requested.
!
! !REVISION HISTORY:
!  same as module

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables:
!
!       {X,Y}{NE,SE,NW,SW} = contribution to {ne,se,nw,sw} coefficients 
!         from {x,y} components of divergence
!       HU = depth at U points
!
!-----------------------------------------------------------------------

   real (r8) ::         &
      xne,xse,xnw,xsw,  &! contribution to coefficients from x,y
      yne,yse,ynw,ysw,  &!   components of divergence
      ase,anw,asw

   character (char_len) :: &
      solv_type,           &! user choice of solver method
      precond_file          ! file containing preconditioner

   namelist /solver_nml/ solv_convrg, solv_max_iters, solv_ncheck, &
                         lprecond, solv_type, precond_file

   integer (int_kind) :: &
      i,j,n,             &! dummy counter
      iblock,            &! block counter
      ncheck,            &! scalar for checking PC/mask compatibility
      nu,                &! I/O unit number and status flag
      nml_error           ! namelist i/o error flag

   integer (int_kind),dimension(:), allocatable :: &
      icheck              ! check for PC/mask compatibility in block

   real (r8), dimension(:,:,:), allocatable :: &
      WORK0,WORKC,WORKN,  &! temp space for computing operator and
      WORKS,WORKE,WORKW,  &! preconditioner coefficients
      WORKNE,WORKNW,WORKSE,WORKSW, &
      RCALC_TMP

   logical (log_kind) :: &
      mlandne, mlandnw, mlandse, mlandsw ! land mask at nbr points

   type (block) ::      &
      this_block         ! block information for current block

!-----------------------------------------------------------------------
!
!  read solver choice and solver constants from namelist input
!  (namelist input file opened in initial.F)
!
!-----------------------------------------------------------------------

   solv_convrg    = eps
   solv_max_iters = 1000
   solv_ncheck    = 10
   lprecond       = .false.
   solv_type      = 'pcg'
   precond_file   = 'empty'

   if (my_task == master_task) then
      open (nml_in, file=nml_filename, status='old',iostat=nml_error)
      if (nml_error /= 0) then
         nml_error = -1
      else
         nml_error =  1
      endif
      do while (nml_error > 0)
         read(nml_in, nml=solver_nml,iostat=nml_error)
      end do
      if (nml_error == 0) close(nml_in)
   endif

   call broadcast_scalar(nml_error, master_task)
   if (nml_error /= 0) then
      call exit_POP(sigAbort,'ERROR reading solver_nml')
   endif

   if (my_task == master_task) then
      write(stdout,delim_fmt)
      write(stdout,blank_fmt)
      write(stdout,'(a35)') ' Solver options (barotropic solver)'
      write(stdout,blank_fmt)
      write(stdout,delim_fmt)

      write(stdout,'(a13)') ' Solver type:'
      select case(solv_type(1:3))
      case('pcg')
         write(stdout,'(a35)') '  Preconditioned Conjugate Gradient'
         solv_itype = solv_pcg
      case('cgr')
         write(stdout,'(a29)') '  Conjugate Gradient Residual'
         solv_itype = solv_cgr
      case('jac')
         write(stdout,'(a8)')  '  Jacobi'
         solv_itype = solv_jac
      case default
         solv_itype = -1000
      end select

      write(stdout,'(a28,1pe12.5)') ' Solver converged for err < ', &
                                      solv_convrg
      write(stdout,'(a29,i6)') ' Solver maximum iterations = ', &
                                 solv_max_iters
      write(stdout,'(a35,i6,a11)') ' Solver convergence checked every ',&
                                     solv_ncheck, ' iterations'

   endif

   call broadcast_scalar(solv_convrg,    master_task)
   call broadcast_scalar(solv_max_iters, master_task)
   call broadcast_scalar(solv_ncheck,    master_task)
   call broadcast_scalar(lprecond,       master_task)
   call broadcast_scalar(solv_itype,     master_task)
   call broadcast_scalar(precond_file,   master_task)

   if (solv_itype == -1000) then
      call exit_POP(sigAbort, &
                    'unknown solver type: must be pcg, cgr or jacobi')
   endif

!-----------------------------------------------------------------------
!
!  set sor for jacobi solver
!
!-----------------------------------------------------------------------

   sor = p25     ! should be < 1/2

!-----------------------------------------------------------------------
!
!  compute nine point operator coefficients: compute on baroclinic
!  decomposition first where grid info defined and redistribute
!  to barotropic distribution
!  leave A0,AC in baroclinic distribution to facilitate easy
!  time-dependent changes in barotropic routine
!
!-----------------------------------------------------------------------

   allocate(WORK0 (nx_block,ny_block,nblocks_clinic), &
            WORKC (nx_block,ny_block,nblocks_clinic), &
            WORKN (nx_block,ny_block,nblocks_clinic), &
            WORKE (nx_block,ny_block,nblocks_clinic), &
            WORKNE(nx_block,ny_block,nblocks_clinic), &
            RCALC_TMP(nx_block,ny_block,nblocks_clinic))

   do iblock = 1,nblocks_clinic
      this_block = get_block(blocks_clinic(iblock),iblock)  

      WORK0    (:,:,iblock) = c0
      WORKC    (:,:,iblock) = c0
      WORKN    (:,:,iblock) = c0
      WORKE    (:,:,iblock) = c0
      WORKNE   (:,:,iblock) = c0
      RCALC_TMP(:,:,iblock) = c0

      do j=2,ny_block
      do i=2,nx_block

         xne = p25*HU(i  ,j  ,iblock)*DXUR(i  ,j  ,iblock)* &
                                      DYU (i  ,j  ,iblock)
         xse = p25*HU(i  ,j-1,iblock)*DXUR(i  ,j-1,iblock)* &
                                      DYU (i  ,j-1,iblock)
         xnw = p25*HU(i-1,j  ,iblock)*DXUR(i-1,j  ,iblock)* &
                                      DYU (i-1,j  ,iblock)
         xsw = p25*HU(i-1,j-1,iblock)*DXUR(i-1,j-1,iblock)* &
                                      DYU (i-1,j-1,iblock)

         yne = p25*HU(i  ,j  ,iblock)*DYUR(i  ,j  ,iblock)* &
                                      DXU (i  ,j  ,iblock)
         yse = p25*HU(i  ,j-1,iblock)*DYUR(i  ,j-1,iblock)* &
                                      DXU (i  ,j-1,iblock)
         ynw = p25*HU(i-1,j  ,iblock)*DYUR(i-1,j  ,iblock)* &
                                      DXU (i-1,j  ,iblock)
         ysw = p25*HU(i-1,j-1,iblock)*DYUR(i-1,j-1,iblock)* &
                                      DXU (i-1,j-1,iblock)

         WORKNE(i,j,iblock) = xne + yne
         ase                = xse + yse
         anw                = xnw + ynw
         asw                = xsw + ysw
 
         WORKE(i,j,iblock)  = xne + xse - yne - yse
         WORKN(i,j,iblock)  = yne + ynw - xne - xnw

         AC(i,j,iblock)  = -(WORKNE(i,j,iblock) + ase + anw + asw)

         WORK0(i,j,iblock)  = TAREA(i,j,iblock)**2
         RCALC_TMP(i,j,iblock) = RCALCT(i,j,iblock)

      end do
      end do
   end do

   A0_CLINIC  = AC

   call redistribute_blocks(AN , distrb_tropic, WORKN,  distrb_clinic)
   call redistribute_blocks(AE , distrb_tropic, WORKE,  distrb_clinic)
   call redistribute_blocks(ANE, distrb_tropic, WORKNE, distrb_clinic)

!-----------------------------------------------------------------------
!
!  calculate normalization constant (darea,darea) for rms_residual
!  in cgr routine.
!
!-----------------------------------------------------------------------

   resid_norm = c1/global_sum(WORK0, distrb_clinic, &
                              field_loc_center, RCALC_TMP)
   solv_convrg = solv_convrg**2/resid_norm

   call redistribute_blocks(RCALCT_B,   distrb_tropic, &
                            RCALC_TMP,  distrb_clinic)

   deallocate(RCALC_TMP)

!-----------------------------------------------------------------------
!
!  setup preconditioner if required
!
!-----------------------------------------------------------------------

   if (lprecond) then

      call exit_POP(sigAbort,'This option not currently supported')
!     if (my_task == master_task) then
!       write(stdout,*) ' Preconditioner read from file: ', &
!                         trim(precond_file)
!     endif
!
!     allocate(WORKS (nx_block,ny_block,nblocks_clinic), &
!              WORKW (nx_block,ny_block,nblocks_clinic), &
!              WORKNW(nx_block,ny_block,nblocks_clinic), &
!              WORKSE(nx_block,ny_block,nblocks_clinic), &
!              WORKSW(nx_block,ny_block,nblocks_clinic))
!
!     allocate(PCC     (nx_block,ny_block,nblocks_tropic), &
!              PCN     (nx_block,ny_block,nblocks_tropic), &
!              PCS     (nx_block,ny_block,nblocks_tropic), &
!              PCE     (nx_block,ny_block,nblocks_tropic), &
!              PCW     (nx_block,ny_block,nblocks_tropic), &
!              PCNE    (nx_block,ny_block,nblocks_tropic), &
!              PCSE    (nx_block,ny_block,nblocks_tropic), &
!              PCNW    (nx_block,ny_block,nblocks_tropic), &
!              PCSW    (nx_block,ny_block,nblocks_tropic))
!
!     allocate(icheck(nblocks_clinic))
!
!-----------------------------------------------------------------------
!
!    read preconditioner and check that it is consistent with
!    KMU field
!
!-----------------------------------------------------------------------
!
!     call open_parallel_file(nu,precond_file,recl_dbl)
!     call read_array(nu,WORKC)
!     call read_array(nu,WORKN)
!     call read_array(nu,WORKS)
!     call read_array(nu,WORKE)
!     call read_array(nu,WORKW)
!     call read_array(nu,WORKNE)
!     call read_array(nu,WORKNW)
!     call read_array(nu,WORKSE)
!     call read_array(nu,WORKSW)
!     call close_parallel_file(nu)
!
!     if (my_task == master_task) then
!       write(stdout,blank_fmt)
!       write(stdout,*) ' file read: ', trim(precond_file)
!     endif
!
!-----------------------------------------------------------------------
!
!    check that PC is consistent with KMU field
!
!-----------------------------------------------------------------------
!
!     do iblock = 1,nblocks_clinic
!
!       this_block = get_block(blocks_clinic(iblock),iblock)  
!
!       icheck(iblock) = 0
!
!       do j=this_block%jb,this_block%je
!       do i=this_block%ib,this_block%ie
!
!         mlandne = .false.
!         mlandnw = .false.
!         mlandse = .false.
!         mlandsw = .false.
!         if (KMU(i  ,j  ,iblock) == 0) mlandne = .true.
!         if (KMU(i-1,j  ,iblock) == 0) mlandnw = .true.
!         if (KMU(i  ,j-1,iblock) == 0) mlandse = .true.
!         if (KMU(i-1,j-1,iblock) == 0) mlandsw = .true.
!
!         if (mlandne .and. WORKNE(i,j,iblock) /= c0)  &
!                           icheck(iblock) = icheck(iblock) + 1
!
!         if (mlandnw .and. WORKNW(i,j,iblock) /= c0)  &
!                           icheck(iblock) = icheck(iblock) + 1
!
!         if (mlandse .and. WORKSE(i,j,iblock) /= c0)  &
!                           icheck(iblock) = icheck(iblock) + 1
!
!         if (mlandsw .and. WORKSW(i,j,iblock) /= c0)  &
!                           icheck(iblock) = icheck(iblock) + 1
!      
!         if (mlandne .and. mlandnw .and. (WORKN(i,j,iblock) /= c0)) &
!                           icheck(iblock) = icheck(iblock) + 1
!         if (mlandne .and. mlandse .and. (WORKE(i,j,iblock) /= c0)) &
!                           icheck(iblock) = icheck(iblock) + 1
!         if (mlandnw .and. mlandsw .and. (WORKW(i,j,iblock) /= c0)) &
!                           icheck(iblock) = icheck(iblock) + 1
!         if (mlandse .and. mlandsw .and. (WORKS(i,j,iblock) /= c0)) &
!                           icheck(iblock) = icheck(iblock) + 1
!         if (mlandne .and. mlandse .and.                            &
!             mlandnw .and. mlandsw .and. (WORKC(i,j,iblock) /= c0)) &
!                           icheck(iblock) = icheck(iblock) + 1
!       end do
!       end do
!     end do
!
!     ncheck = sum(icheck)
!     if (global_sum(ncheck, distrb_clinic) /= 0) then
!       call exit_POP(sigAbort,'PC and KMU are incompatible')
!     endif
!
!     deallocate(icheck)
!
!     call redistribute_blocks(PCC ,distrb_tropic,WORKC ,distrb_clinic)
!     call redistribute_blocks(PCN ,distrb_tropic,WORKN ,distrb_clinic)
!     call redistribute_blocks(PCE ,distrb_tropic,WORKE ,distrb_clinic)
!     call redistribute_blocks(PCS ,distrb_tropic,WORKS ,distrb_clinic)
!     call redistribute_blocks(PCW ,distrb_tropic,WORKW ,distrb_clinic)
!     call redistribute_blocks(PCNE,distrb_tropic,WORKNE,distrb_clinic)
!     call redistribute_blocks(PCNW,distrb_tropic,WORKNW,distrb_clinic)
!     call redistribute_blocks(PCSE,distrb_tropic,WORKSE,distrb_clinic)
!     call redistribute_blocks(PCSW,distrb_tropic,WORKSW,distrb_clinic)
!
!     deallocate(WORKS, WORKW, WORKNW, WORKSE, WORKSW)
!
   else ! no preconditioner

      if (my_task == master_task) then
         write(stdout,'(a18)') ' No preconditioner'
      endif

   endif

   deallocate(WORK0, WORKC, WORKN, WORKE, WORKNE)

!-----------------------------------------------------------------------
!EOC

 end subroutine init_solvers

!***********************************************************************
!BOP
! !IROUTINE: pcg
! !INTERFACE:

 subroutine pcg(X,B)

! !DESCRIPTION:
!  This routine uses a preconditioned conjugate-gradient solver to
!  solve the equation $Ax=b$.  Both the operator $A$ and preconditioner
!  are nine-point stencils. If no preconditioner has been supplied,
!  a diagonal preconditioner is applied.  Convergence is checked
!  every {\em ncheck} steps.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,max_blocks_tropic), &
      intent(in) :: &
      B                         ! right hand side of linear system

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,max_blocks_tropic), &
      intent(inout) :: &
      X                  ! on input,  an initial guess for the solution
                         ! on output, solution of the linear system

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      m,                 &! local iteration counter
      iblock              ! local block     counter

   real (r8) ::          &
      eta0,eta1,rr        ! scalar inner product results

   real (r8), dimension(nx_block,ny_block,max_blocks_tropic) :: &
      R,                 &! residual (b-Ax)
      S,                 &! conjugate direction vector
      Q,WORK0,WORK1       ! various cg intermediate results

   character (char_len) :: & 
      noconvrg           ! error message for no convergence

   type (block) ::      &
      this_block         ! block information for current block

!-----------------------------------------------------------------------
!
!  compute initial residual and initialize S
!
!-----------------------------------------------------------------------

   !$OMP PARALLEL DO PRIVATE(iblock,this_block)

   do iblock=1,nblocks_tropic
      this_block = get_block(blocks_tropic(iblock),iblock)  

      call btrop_operator(S,X,this_block,iblock)
      R(:,:,iblock) = B(:,:,iblock) - S(:,:,iblock)
      S(:,:,iblock) = c0
   end do ! block loop

   !$OMP END PARALLEL DO

!-----------------------------------------------------------------------
!
!  initialize fields and scalars
!
!-----------------------------------------------------------------------

   call update_ghost_cells(R, bndy_tropic, field_loc_center, &
                                           field_type_scalar)
   eta0 =c1 
   solv_sum_iters = solv_max_iters
 
!-----------------------------------------------------------------------
!
!  iterate
!
!-----------------------------------------------------------------------

   iter_loop: do m = 1, solv_max_iters

!-----------------------------------------------------------------------
!
!     calculate (PC)r 
!     diagonal preconditioner if preconditioner not specified
!
!-----------------------------------------------------------------------

      !$OMP PARALLEL DO PRIVATE(iblock,this_block)

      do iblock=1,nblocks_tropic
         this_block = get_block(blocks_tropic(iblock),iblock)  

         if (lprecond) then
            call preconditioner(WORK1,R,this_block,iblock)
         else
            where (A0(:,:,iblock) /= c0)
               WORK1(:,:,iblock) = R(:,:,iblock)/A0(:,:,iblock)
            elsewhere
               WORK1(:,:,iblock) = c0
            endwhere
         endif

         WORK0(:,:,iblock) = R(:,:,iblock)*WORK1(:,:,iblock)
      end do ! block loop

      !$OMP END PARALLEL DO

!-----------------------------------------------------------------------
!
!     update conjugate direction vector s
!
!-----------------------------------------------------------------------

      if (lprecond) &
         call update_ghost_cells(WORK1,bndy_tropic, field_loc_center,&
                                                    field_type_scalar)
      !*** (r,(PC)r)
      eta1 = global_sum(WORK0, distrb_tropic, field_loc_center, RCALCT_B)

      !$OMP PARALLEL DO PRIVATE(iblock,this_block)

      do iblock=1,nblocks_tropic
         this_block = get_block(blocks_tropic(iblock),iblock)  

         S(:,:,iblock) = WORK1(:,:,iblock) + S(:,:,iblock)*(eta1/eta0) 

!-----------------------------------------------------------------------
!
!        compute As
!
!-----------------------------------------------------------------------

         call btrop_operator(Q,S,this_block,iblock)
         WORK0(:,:,iblock) = Q(:,:,iblock)*S(:,:,iblock)

      end do ! block loop

      !$OMP END PARALLEL DO

!-----------------------------------------------------------------------
!
!     compute next solution and residual
!
!-----------------------------------------------------------------------

      call update_ghost_cells(Q, bndy_tropic, field_loc_center, &
                                              field_type_scalar)

      eta0 = eta1
      eta1 = eta0/global_sum(WORK0, distrb_tropic, &
                             field_loc_center, RCALCT_B)

      !$OMP PARALLEL DO PRIVATE(iblock,this_block)

      do iblock=1,nblocks_tropic
         this_block = get_block(blocks_tropic(iblock),iblock)  

         X(:,:,iblock) = X(:,:,iblock) + eta1*S(:,:,iblock)
         R(:,:,iblock) = R(:,:,iblock) - eta1*Q(:,:,iblock)

         if (mod(m,solv_ncheck) == 0) then

            call btrop_operator(R,X,this_block,iblock)
            R(:,:,iblock) = B(:,:,iblock) - R(:,:,iblock)
            WORK0(:,:,iblock) = R(:,:,iblock)*R(:,:,iblock)
         endif
      end do ! block loop

      !$OMP END PARALLEL DO

!-----------------------------------------------------------------------
!
!     test for convergence
!
!-----------------------------------------------------------------------

      if (mod(m,solv_ncheck) == 0) then

         call update_ghost_cells(R, bndy_tropic, field_loc_center,&
                                                 field_type_scalar)

         rr = global_sum(WORK0, distrb_tropic, &
                         field_loc_center, RCALCT_B)   ! (r,r)

         if (rr < solv_convrg) then
            solv_sum_iters = m
            exit iter_loop
         endif

      endif

   enddo iter_loop

   rms_residual = sqrt(rr*resid_norm)

   if (solv_sum_iters == solv_max_iters) then
      if (solv_convrg /= c0) then
         write(noconvrg,'(a45,i11)') & 
           'Barotropic solver not converged at time step ', nsteps_total
         call exit_POP(sigAbort,noconvrg)
      endif
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine pcg

!***********************************************************************
!BOP
! !IROUTINE: cgr
! !INTERFACE:

 subroutine cgr(X,B)

! !DESCRIPTION:
!  This routine uses a conjugate residual solver to
!  solve the equation $Ax=b$.  Both the operator $A$ and preconditioner
!  are nine-point stencils. If no preconditioner has been supplied,
!  a diagonal preconditioner is applied.  Convergence is checked
!  every {\em ncheck} steps.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,max_blocks_tropic), &
      intent(in) :: &
      B                         ! right hand side of linear system

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,max_blocks_tropic), &
      intent(inout) :: &
      X                 ! on input,  an initial guess for the solution
                        ! on output, solution of the linear system

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) ::   & 
      m,                   &! local iteration counter
      iblock                ! local block     counter

   real (r8) ::            &
      eps0,eps1,eta0,eta1, &! scalar inner product results
      rr,rrold              ! residual norms

   real (r8), dimension(nx_block,ny_block,max_blocks_tropic) :: &
      R,                   &! residual (b-Ax)
      S,                   &! conjugate direction vector
      Q,                   &! As (operator acting on s)
      AR,                  &! Ar (operator acting on residual)
      WORK                  ! various cg intermediate results

   character (char_len) ::  & 
      noconvrg              ! error message for no convergence

   type (block) ::          &
      this_block            ! block information for current block

!-----------------------------------------------------------------------
!
!  compute initial residual and its norm
!
!-----------------------------------------------------------------------

   !$OMP PARALLEL DO PRIVATE(iblock,this_block)

   do iblock=1,nblocks_tropic
      this_block = get_block(blocks_tropic(iblock),iblock)  

      call btrop_operator(S,X,this_block,iblock)
      R(:,:,iblock) = B(:,:,iblock) - S(:,:,iblock)
      WORK(:,:,iblock) = R(:,:,iblock)*R(:,:,iblock)
      S(:,:,iblock) = c0
      Q(:,:,iblock) = c0
   end do ! block loop

   !$OMP END PARALLEL DO

   call update_ghost_cells(R, bndy_tropic, field_loc_center, &
                                           field_type_scalar)
   ! initial (r,r)
   rr = global_sum(WORK, distrb_tropic, field_loc_center, RCALCT_B)
   rrold = rr
 
!-----------------------------------------------------------------------
!
!  initialize scalars
!
!-----------------------------------------------------------------------

   eps1 = c1 
   solv_sum_iters = solv_max_iters
 
!-----------------------------------------------------------------------
!
!  iterate
!
!-----------------------------------------------------------------------

   iter_loop: do m = 1, solv_max_iters

!-----------------------------------------------------------------------
!
!     calculate Ar
!
!-----------------------------------------------------------------------

      !$OMP PARALLEL DO PRIVATE(iblock,this_block)

      do iblock=1,nblocks_tropic
         this_block = get_block(blocks_tropic(iblock),iblock)  

         call btrop_operator(AR,R,this_block,iblock)
         WORK(:,:,iblock) = R(:,:,iblock)*AR(:,:,iblock)
      end do ! block loop

      !$OMP END PARALLEL DO

!-----------------------------------------------------------------------
!
!     update conjugate direction vector s, and q = As
!
!-----------------------------------------------------------------------

      call update_ghost_cells(AR, bndy_tropic, field_loc_center, &
                                               field_type_scalar)
      ! (r,Ar)
      eps0 = global_sum(WORK, distrb_tropic, field_loc_center, RCALCT_B)
      eta1 = eps0/eps1

      !$OMP PARALLEL DO PRIVATE(iblock,this_block)

      do iblock=1,nblocks_tropic
         this_block = get_block(blocks_tropic(iblock),iblock)  

         S(:,:,iblock) =  R(:,:,iblock) +  eta1*S(:,:,iblock) 
         Q(:,:,iblock) = AR(:,:,iblock) +  eta1*Q(:,:,iblock) 
         WORK(:,:,iblock) = Q(:,:,iblock)*Q(:,:,iblock)
      end do ! block loop

      !$OMP END PARALLEL DO

!-----------------------------------------------------------------------
!
!     compute next solution and residual, update (r,r).
!     every ncheck steps recalculate (r,r) = (b - Ax,b - Ax) and
!     exit if it is not decreasing (due to roundoff error).
!
!-----------------------------------------------------------------------
      ! (As,As)
      eps1 = global_sum(WORK, distrb_tropic, field_loc_center, RCALCT_B)
      eta0 = eps0/eps1

      !$OMP PARALLEL DO PRIVATE(iblock,this_block)

      do iblock=1,nblocks_tropic
         this_block = get_block(blocks_tropic(iblock),iblock)  

         X(:,:,iblock) = X(:,:,iblock) + eta0*S(:,:,iblock)

         if (mod(m,solv_ncheck) == 0) then
            call btrop_operator(R,X,this_block,iblock)
            R(:,:,iblock) = B(:,:,iblock) - R(:,:,iblock)
            WORK(:,:,iblock) = R(:,:,iblock)*R(:,:,iblock)
         else
            R(:,:,iblock) = R(:,:,iblock) - eta0*Q(:,:,iblock)
         endif
      end do ! block loop

      !$OMP END PARALLEL DO

!-----------------------------------------------------------------------
!
!     test for convergence
!     every ncheck steps the residual norm is recalculated as
!     r = b - Ax to avoid roundoff error in the accumulated
!     residual (r,r) = old (r,r) - eta0*q.  if the recalculated
!     (r,r) is not less than its previously calculated value, 
!     then the solution is not converging due to machine roundoff
!     error, and the routine is exited.
!
!-----------------------------------------------------------------------

      if (mod(m,solv_ncheck) == 0) then
         call update_ghost_cells(R, bndy_tropic, field_loc_center, &
                                                 field_type_scalar)

         rr = global_sum(WORK, distrb_tropic, field_loc_center, RCALCT_B)
         if (rr > rrold) then
            solv_sum_iters = m
            exit iter_loop
         endif
         rrold = rr
      else
         rr = rr - eta0**2*eps1
      endif

      eps1 = eps0  ! update for next pass

      if (rr < solv_convrg) then
         solv_sum_iters = m
         exit iter_loop
      endif

   enddo iter_loop

   rms_residual = sqrt(abs(rr)*resid_norm)

   if (solv_sum_iters == solv_max_iters) then
      if (solv_convrg /= c0) then
        write(noconvrg,'(a45,i11)') & 
           'Barotropic solver not converged at time step ', nsteps_total
        call exit_POP(sigAbort,noconvrg)
      endif
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine cgr

!***********************************************************************
!BOP
! !IROUTINE: jacobi
! !INTERFACE:

 subroutine jacobi(X,B)

! !DESCRIPTION:
!  This routine uses a simple Richardson-Jacobi solver to
!  solve the equation $Ax=b$.  Both the operator $A$ and preconditioner
!  are nine-point stencils. If no preconditioner has been supplied,
!  a diagonal preconditioner is applied.  Convergence is checked
!  every {\em ncheck} steps.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,max_blocks_tropic), &
      intent(in) :: &
      B                         ! right hand side of linear system

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,max_blocks_tropic), &
      intent(inout) :: &
      X                 ! on input,  an initial guess for the solution
                        ! on output, solution of the linear system

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) ::  & 
      m,                  &! local iteration counter
      iblock               ! local block     counter

   real (r8) ::           &
      rr                   ! scalar inner product results

   real (r8), dimension(nx_block,ny_block,max_blocks_tropic) :: &
      R,                  &! residual (b-Ax)
      WORK0,WORK1          ! various intermediate results

   character (char_len) :: & 
      noconvrg             ! error message for no convergence

   type (block) ::         &
      this_block           ! block information for current block

!-----------------------------------------------------------------------
!
!  iterate
!
!-----------------------------------------------------------------------

   solv_sum_iters = solv_max_iters

   iter_loop: do m = 1, solv_max_iters

!-----------------------------------------------------------------------
!
!     calculate residual r = b - Ax
!
!-----------------------------------------------------------------------

      !$OMP PARALLEL DO PRIVATE(iblock,this_block)

      do iblock=1,nblocks_tropic
         this_block = get_block(blocks_tropic(iblock),iblock)  

         call btrop_operator(WORK0,X,this_block,iblock)
         R(:,:,iblock) = B(:,:,iblock) - WORK0(:,:,iblock)
      end do ! block loop

      !$OMP END PARALLEL DO

      call update_ghost_cells(R, bndy_tropic, field_loc_center, &
                                              field_type_scalar)

!-----------------------------------------------------------------------
!
!     calculate (PC)r 
!
!-----------------------------------------------------------------------

      !$OMP PARALLEL DO PRIVATE(iblock,this_block)

      do iblock=1,nblocks_tropic
         this_block = get_block(blocks_tropic(iblock),iblock)  

         if (lprecond) then
            call preconditioner(WORK1,R,this_block,iblock)
         else
            where (A0(:,:,iblock) /= c0)
               WORK1(:,:,iblock) = R(:,:,iblock)/A0(:,:,iblock)
            elsewhere
               WORK1(:,:,iblock) = c0
            endwhere
         endif

!-----------------------------------------------------------------------
!
!        compute next solution
!
!-----------------------------------------------------------------------

         X(:,:,iblock) = X(:,:,iblock) + sor*WORK1(:,:,iblock)
      end do ! block loop

      !$OMP END PARALLEL DO

      if (lprecond) &
         call update_ghost_cells(X, bndy_tropic, field_loc_center, &
                                                 field_type_scalar)

!-----------------------------------------------------------------------
!
!     test for convergence
!
!-----------------------------------------------------------------------

      if (mod(m,solv_ncheck) == 0) then

         WORK0 = R*R
         ! (r,r)
         rr = global_sum(WORK0, distrb_tropic, field_loc_center, RCALCT_B)
          
         if (rr < solv_convrg) then
            solv_sum_iters = m
            exit iter_loop
         endif

      endif

   enddo iter_loop

   rms_residual = sqrt(rr*resid_norm)

   if (solv_sum_iters == solv_max_iters) then
      if (solv_convrg /= c0) then
         write(noconvrg,'(a45,i11)') & 
           'Barotropic solver not converged at time step ', nsteps_total
         call exit_POP(sigAbort,noconvrg)
      endif
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine jacobi

!***********************************************************************
!BOP
! !IROUTINE: preconditioner
! !INTERFACE:

 subroutine preconditioner(PX,X,this_block,bid)

! !DESCRIPTION:
!  This function applies a precomputed preconditioner as a nine-point
!  stencil operator.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,max_blocks_tropic), &
      intent(in) :: & 
      X                     ! array to be operated on 

   type (block), intent(in) :: &
      this_block             ! block info for this block

   integer (int_kind), intent(in) :: &
      bid                    ! local block address for this block

! !OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,max_blocks_tropic), &
      intent(out) :: &
      PX                  ! nine point operator result

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      i,j                ! dummy counters

!-----------------------------------------------------------------------

   PX(:,:,bid) = c0

   do j=this_block%jb,this_block%je
   do i=this_block%ib,this_block%ie
      PX(i,j,bid) = PCNE(i,j,bid)*X(i+1,j+1,bid) + &
                    PCNW(i,j,bid)*X(i-1,j+1,bid) + &
                    PCSE(i,j,bid)*X(i+1,j-1,bid) + &
                    PCSW(i,j,bid)*X(i-1,j-1,bid) + &
                    PCN (i,j,bid)*X(i  ,j+1,bid) + &
                    PCS (i,j,bid)*X(i  ,j-1,bid) + &
                    PCE (i,j,bid)*X(i+1,j  ,bid) + &
                    PCW (i,j,bid)*X(i-1,j  ,bid) + &
                    PCC (i,j,bid)*X(i  ,j  ,bid)
   end do
   end do

!-----------------------------------------------------------------------
!EOC

 end subroutine preconditioner

!***********************************************************************
!BOP
! !IROUTINE: btrop_operator
! !INTERFACE:

 subroutine btrop_operator(AX,X,this_block,bid)

! !DESCRIPTION:
!  This routine applies the nine-point stencil operator for the
!  barotropic solver.  It takes advantage of some 9pt weights being 
!  shifted versions of others.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,max_blocks_tropic), &
      intent(in) :: & 
      X                  ! array to be operated on 

   type (block), intent(in) :: &
      this_block             ! block info for this block

   integer (int_kind), intent(in) :: &
      bid                    ! local block address for this block

! !OUTPUT PARAMETERS:

   real (r8), dimension(nx_block,ny_block,max_blocks_tropic), &
      intent(out) :: &
      AX                     ! nine point operator result (Ax)

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      i,j                ! dummy counters

!-----------------------------------------------------------------------

   AX(:,:,bid) = c0

   do j=this_block%jb,this_block%je
   do i=this_block%ib,this_block%ie
      AX(i,j,bid) = A0 (i  ,j  ,bid)*X(i  ,j  ,bid) + &
                    AN (i  ,j  ,bid)*X(i  ,j+1,bid) + &
                    AN (i  ,j-1,bid)*X(i  ,j-1,bid) + &
                    AE (i  ,j  ,bid)*X(i+1,j  ,bid) + &
                    AE (i-1,j  ,bid)*X(i-1,j  ,bid) + &
                    ANE(i  ,j  ,bid)*X(i+1,j+1,bid) + &
                    ANE(i  ,j-1,bid)*X(i+1,j-1,bid) + &
                    ANE(i-1,j  ,bid)*X(i-1,j+1,bid) + &
                    ANE(i-1,j-1,bid)*X(i-1,j-1,bid)
   end do
   end do

!-----------------------------------------------------------------------
!EOC

 end subroutine btrop_operator

!***********************************************************************

 end module solvers

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
