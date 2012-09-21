c
#define idummy 8
#define itercomp 1
c     Have team leader unscramble grid bricks, not master thread.
c     See if this will run with all OpenMP flush commands commented out.
c     We do not know the answer to the above question, but we do know
c     that the code will not run properly on Interlagos when compiled
c     with the Cray Fortran compiler unless the flushes that we have
c     at spin-waits are uncommented.  These, however, can remain
c     commented out on Intel hardware using Intel compilers.
c     Jagan suggests a theory of this.  The idea is that if, as the
c     OpenMP documentation suggests, the flush command is only a note
c     to the compiler not to move assignment statements for the listed
c     variables across this boundary in the code, then we should be
c     able to achieve the same desired effect by moving the flush
c     outside of the spin-wait itself and placing it directly before
c     this point.  Then the cache-coherency hardware should accomplish
c     the desired updating of the variable in question.  So we are
c     testing this theory with this code.  Simply uncommenting the flush
c     inside the spin-wait gives a 5% performance hit to the code
c     overall.
c
c     Suggestions:
c     We will use the C compiler and SSE intrinsic functions.
c     1)  We need an IBM-like "extract" intrinsic for floating point
c         quadwords.
c     2)  Why can't we stop at the Fortran-I level, with outer loops
c         on  i  and an index array  ii(i) ??
c     3)  Why do we need multiple threads per core?
c         Why can't the compiler generate a single efficient thread?
c     4)  I should be able, at the advanced programming level, to pre-
c         fetch data into a locally dimensioned (private), cache-
c         resident array without having the cache lines of the globally
c         shared array appear for no reason in my cache.
c     5)  I should be able, at the advanced programming level, to store
c         something into memory without reading it first.  I will agree
c         always to stor entire cache lines, but only if someone else
c         will agree to support cache line alignment assertions.
c     6)  I would like to evict cache lines from my private cache, where
c         space is critical, but without evicting them from any shared
c         cache that they might reside in.  This I would use only for
c         read-only variables, or in any case I would take responsibil-
c         ity for making the code work properly.
c     7)  I would like to be able to declare cache resident variables.
c         I can already do this by dimensioning them on the stack.
c         However, these variables get shoved out by junk that the
c         hardware puts in my private cache whether I want it or not.
c         So I need some way to demand that my cache resident variables
c         take precedence, and should not be evicted if there are other
c         things, regardless of how recently I appear to have used them,
c         that are sitting in my cache and could be evicted instead.
c
c+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
c+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
c
c     7/25/11:
c     This version specializes to the restricted case of the demo code.
c     A number of performance features that apply only to the demo have
c     been implemented.
c     1)  The time step is ASSUMED constant.
c     2)  The messaging has been dramatically simplified.
c     3)  Thread control flow has been dramatically simplified by
c         incorporating into PPMMF the copying of boundary information
c         out of message arrays, alternatively the application of the
c         boundary conditions (in the Y-pass), and by the copying of
c         new briquettes directly into message arrays or augmented brick
c         ghost briquettes.
c     THIS NEW VERSION ONLY WORKS FOR    NSUGARCUBES = 1    !!!!!!!!!
c
c     6/1/11:
c     On the off-chance that thread barriers in OpenMP are slowing down
c     this parallel implementation, a system of dynamic load balancing
c     has been implemented.  The OpenMP threads are divided, using their
c     thread numbers, into groups of 4, with the first in each group
c     acting as a master.  Each such group updates pencils of grid cells
c     consisting of 2x2 sets of grid pencils that are one briquette
c     wide.  This will produce a great deal of sharing of cached data
c     within each group of 4 threads, which we assume will be scheduled
c     on a single Knights Ferry core.
c     Thread barriers at the ends of grid brick updates have been
c     removed.  Updating of the next brick in the sequence begins
c     potentially before the update of the previous brick is complete.
c     There is no reason for a thread barrier at the end of a pass
c     either, I think.  A full thread barrier, however, must occur
c     at the point where we readjust the time step value.
c     Just for now, I have not build the thread groups of 4.
c     I have also put a thread barrier at the end of each 1-D pass.
c     This is not necessary, but it sure is convenient.
c
c     The right way to handle the data copying, while avoiding intense
c     confusion within the PPMMF routine, is to hand PPMMF on each call
c     the starting addresses of strips of briquettes it must overwrite
c     with updated data, which new briquettes to write there, and the
c     value of the increment from one briquette to the next in the
c     strips to be written.  Then the thread synchronization is much
c     simpler, because when the grid brick update is complete, so is
c     all of the message construction.  In the case where we do MPI,
c     there is more data copying that must occur and that cannot be done
c     along with the grid brick update.  But this change would eliminate
c     half of the data copying, reduce main memory bandwidth require-
c     ments, and reduce thread synchronization costs.  I did it the
c     other way, because I made so many bugs that were so difficult to
c     find when I used this approach before (2008-2009).
c
c     10/27/10:
c     A big issue popped up related to the loop below.
c     It occurred when I set both  nsugar  and  nbdy  to  4.
c     The i-loop bounds specified below request that the values in the
c     loop be computed for all but the first 4 and all bu the last 4
c     grid cells in the augmented X-strip.
c     For a value of  nsugar = 4,  we have two choices.
c     1)  We can save one more grid plane of  uy  and  uz  values,
c         so that we can perform the loop for  i  = -7.
c     2)  We can shift the iterations of the loop that we perform on
c         each grid briquette over by one grid plane, so that the
c         iteration for  i = -7  would actually have been computed on
c         the previous briquette as  i = -3.
c     For option (1), if we save data in quanta of entire briquettes,
c     and if we save the same amount of data for all temporary arrays,
c     we would have to add a whole briquette of data to every single
c     temporary in the problem.  This would have a huge negative impact.
c     For option (2), this would require that we somehow modify our
c     loop tranlation rule, which would be OK in principle.  Shifting
c     over by one grid plane the iterations we compute below would also
c     have the effect that we would actually compute one grid plane of
c     results, for the final briquette, that the program did not
c     request.  In this case, this extra work would be harmless, but
c     in general, it would be impossible to know this.  We would
c     perhaps require a programmer to tell us that it would be OK.
c     If we did not know, we would have to write special code for the
c     final briquette, which would result in further bloat.
c     There are not really any good options to deal with this.
c     We can, however, apply the following logic to the situation.
c     There is nothing special about the augmented regions.  They can be
c     just more of the problem domain that has been communicated to us
c     from other MPI ranks.  Alternatively, they could have been
c     generated from boundary conditions.  So this gives rise to a RULE.
c     RULE:
c     Any calculation that is requested to be performed in the main
c     region of the problem must be legal to perform in any ghost cell
c     region as well.  The translator, in order to pipeline the code,
c     may decide to extend the requested region in which particular
c     quantities are evaluated.  If it does this, it might give a
c     warning to the programmer, BUT THIS MUST BE PERMISSIBLE TO DO.
c
cc      do i = 5-nbdy,nx+nbdy-4
c      ibegin = 1-nx-4
c      ibegin = max(ibegin,5-nx*(icube+nghostcubes))
c      if (ibegin .le. -4)   then
c      do i = ibegin,-4
ccdir$ ivdep
c!DEC$ VECTOR ALWAYS
cc!DEC$ VECTOR ALIGNED
cc      do 1210   i = -2,n+5
c      do j = 1,nssq
c      vortz = uxzt(j,ii(i)) - uxzb(j,ii(i))
c      vortz = uy(j,ii(i+1)) - uy(j,ii(i-1)) - vortz
c      vorty = uz(j,ii(i+1)) - uz(j,ii(i-1))
c      vorty = uxzn(j,ii(i)) - uxzf(j,ii(i)) - vorty
c      vortx = uyzn(j,ii(i)) - uyzf(j,ii(i))
c      vortx = uzzt(j,ii(i)) - uzzb(j,ii(i)) - vortx
c      vortsq = vortx * vortx
c      vortsq = vorty * vorty  +  vortsq
c      vortsq = vortz * vortz  +  vortsq
c      thyng = sqrt (vortsq)
c      vort(j,ii(i)) = thyng * halfdxinv
c      divu(j,ii(i)) = uzzn(j,ii(i)) - uzzf(j,ii(i))
c      divu(j,ii(i)) = uyzt(j,ii(i)) - uyzb(j,ii(i)) + divu(j,ii(i))
c      divu(j,ii(i)) = ux(j,ii(i-1)) - ux(j,ii(i+1)) - divu(j,ii(i))
c      divu(j,ii(i)) = divu(j,ii(i)) * dxinv
c      enddo
c      enddo
cc1210  continue
c
c+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
c+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
c
c     The overall domain must be a unit cube.
c     The origin will be placed at the center.
c
c
c
c+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
c     THE FOLLOWING PARAMETERS DETERMINE THE RESTART/PARALLEL MODALITY
c+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define isrestart 0
#define nndumpsperrestartdump 100
c
c     An explanation of the user-modifiable parameters that follow
c     immediately below is given just below them.  The idea is for the
c     user to select any consistent set of units for the problem and
c     then to enter the various quantities in those units.  That act
c     will determine the units of the computation.  The stop time for
c     the problem can be expressed in sound crossing times or as a
c     simple time in the user's choice of units.  To make this work,
c     the user must specify negligible values of density, pressure,
c     velocity magnitude, and energy in order for the code to properly
c     protect divisions, logs, square roots, etc., that might otherwise
c     lead to floating point exceptions.
c
#if defined(USING_MPI)
#define ismpi USING_MPI
#else
#define ismpi 0
#endif
#define isomp 1
#define is64bit 0
#define isconstdt 1
#define cconstdt 0.00125001
#define nnparallelloops 20
c
c+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
c     THE FOLLOWING PARAMETERS DETERMINE THE RAYLEIGH-TAYLOR PROBLEM
c+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define ssoundcrossings 2.
#define sstoptime 2.
#define ccourinitmx .3
#define ssafety .5
#define nndumpstot 20
#define nndumpstodo 20
c+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
c     THE FOLLOWING PARAMETERS DETERMINE THE GRID AND PARALLEL LAYOUT
c+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define nnzteams (1+ismpi*0)
#define nnyteams 1
#define nnBricksPerDimPerRank 2
c
c     The number of threads (cores) must be a multiple of 4.
c     The number of briquettes in each dimension must be even.
c
#if defined(isMIC) || defined(isMICHOST)
#ifdef NDEBUG
#define is6io 0
#else
#define is6io 1
#endif
#ifdef isMIC
#define STOP call cstop ()
#define stop call cstop ()
#endif
#else
c#error "Should not get here, macros are supplied by the build system"
#define nncpucores (128)
#define nnxfac (64)
#define nnyfac (64)
c                                       nnBricksPerDimPerRank must be 2.
#define nntzbricks (1*nnBricksPerDimPerRank)
#define nntybricks (1*nnBricksPerDimPerRank)
#define is6io 0
#endif
c+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define isintelfortran 1
#define ismmprefetch 0
c                                   mm_prefetch
c+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
c
c+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
c     THE FOLLOWING DERIVED PARAMETERS MUST NOT BE MODIFIED
c+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define isviolent 0
#define mmnybdry 0
#define mmxybdry 0
c
#define uujiggle 0.
#define isfvdiffusion 0
#define uufvdiffusion 0.025
#define ffvdiscrep1 0.06
#define ffvdiscrep2 0.12
c
#define isstrongshocks 0
#define isfvnavstokes 0
#define isnavstokes 0+isfvnavstokes
#define ddynamicviscosity 0.
#define nnhalfwaves 2
#define istp2singlemode 1
#define isbeatfreq 0
#define ismultimode 0
#define ppower 0.
#define mmodemin 5
#define mmodemax 6
#define wwavelength 1.
#define ddisplacement0 0.005
#define ttransitionthickness .025
#define ggamma (5./3.)
#define ssoundspd0light 1.82574185835
#define ddensity0light .05
#define aatwood (19./21.)
c
#define istp2 1
#define rrshellin 1.
#define rrshellout 1.2
#define rrbdryin 1.7
#define rrbdryout 2.0
#define rrhoinside .05
#define rrhoshell 1.
#define rrhooutside .05
#define rrhobdry 10.
#define ppinside .1
#define ppshell .1
#define ppoutside 13.
#define ppbdry 1.
#define uurbdryatreq1 1.
#define uurotatreq1 0.
#define ggravaccel 0.
c
c     Violent problems are ones in which shocks are so strong that we
c     risk pressures and densities hitting floor values if we do not
c     apply our monotonicity constraints everywhere.  Generally, we
c     find that it is a good idea to consider problems violent that
c     involve shocks of Mach 4 or greater.
c     In nonviolent problems, we do not bother to check for negative
c     values of the pressure or the density in the interpolation
c     process, although we may check for them in the Riemann solver.
c
#define nntxbricks nntzbricks
#define nnbdy 4
#define nnsugar max(4,nnbdy)
#define nnsugarcubes 1
c
c     Every node will be assigned a 2x2x2 octobrick of 8 bricks,
c                to be updated by nncpucores cooperatively.
c                This reflects the fact that nnBricksPerDimPerRank
c                must be 2 in this code (it is hard-wired).
c     Every core will update strips of nnsugarcubes**2 briquettes.
c                Bricks must be big enough to keep all nncpucores
c                productively busy with a local barrier at the end of
c                each brick update.
c
#define nnnnx nnxfac*(nnsugar*nnsugarcubes)
#define nnnny nnyfac*(nnsugar*nnsugarcubes)
#define nnnnz nnnnx
#define issmartdiffusion isstrongshocks
#define nnprevtimes (128*16-1)
c                     nnprevtimes+1 must be a multiple of 16.
c
c+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
c     THE FOLLOWING PARAMETERS SPECIFY TRIVIAL VALUES OF QUANTITIES
c+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define ssmlrho .00001
#define ssmallp .000001
#define ssmalle .000001
#define ssmallu .000001
c
c+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
c     THE FOLLOWING PARAMETERS PRODUCE MODS FOR THE STAR PROBLEM
c+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
c
c     Good values for the Helium shell flash convection problem at the
c     later time are to run the problem for  1000  "sound crossings"
c     and to save  800  dumps.
c
#define issphere 0*isNoAccl
#define hheatmultiplier 1.
c
#define rrydberg 8.31
#define aairmolecwt 1.524
#define rradin0 6.0
#define rradinner 8.0
#define rradbase 9.5
#define rradtop 30.0
#define rradouter 35.0
#define rradout0 35.5
#define rradmax 36.0
#define ddlayerbot 1.5
#define ddlayertop 1.0
#define ttotallum 0.01614
#define ggrav00base 0.49545
#define rrho00base 11.74
#define pprs00base 16.96
#define ggammabelow 1.2
#define ggammaabove 1.01
#define ggravconst .00006673
#define ggammaair (5./3.)
#define rrhocldtoair0 (1./2.26)
#define ssignalspeed 1.7
c
c&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
c
c     isrestart  =  1  if begin from a restart dump,  0  otherwise.
c     nndumpsperrestartdump  =  Number of output dump intervals between
c                               restart dumps.
c     ismpi  =  1  if using MPI,  0  otherwise.
c     isomp  =  1  if using OpenMP,  0  otherwise.
c               OpenMP cannot be used on Roadrunner (RR).
c               If not on RR, must set  isomp=1  if  nncpucores > 1.
c     ssoundcrossings  =  The number of sound crossing times (with sound
c                         speed  ssoundspd0light) of the primary initial
c                         disturbance wavelength.  This defines the
c                         length of simulated time for the problem,
c                         unless  sstoptime  is set to a positive value.
c     sstoptime  =  0.  if the problem is to be run for the prescribed
c                   number of sound crossing times.  Otherwise, set this
c                   constant to the value of the simulated time at which
c                   the problem is to be stopped (finished).
c     ccourinitmx  =  The initial value of the Courant number limit
c                     (which must be strictly less than 1.) to be used
c                     to set the time step value.  This time step value
c                     will not be adjusted during the run, so a reason-
c                     able margin of safety is recommended.  For the
c                     Rayleigh-Taylor test problem 1, fluid velocities
c                     never much exceed 10% of ssoundspd0light, and thus
c                     a value of  0.65  suffices to keep the computation
c                     numerically stable during the entire length of the
c                     run.
c     nndumpstot  =  The total number of output dumps desired (not
c                    counting the initial, zeroth dump).
c     nndumpstodo  =  The number of output dumps to do by the end of
c                     this segment of the run (if the run is to be
c                     restarted).
c     isperiodic = 1, if the boundary conditions are periodic in the two
c                  horizontal directions, X and Z.  In this case, the
c                  maximum and minimum intial interface displacement
c                  will be placed at the center of a grid cell in X & Z.
c                  This flag is  0  if the X and Z boundaries are
c                  reflecting, which they must be if nnhalfwaves is odd.
c     uujiggle  =  The magnitude of the horizontal velocity with
c                  which we move the entire flow back and forth in
c                  the two horizontal dimensions, X and Z, in each
c                  time step pair.  This can only be done if we
c                  have set the flag for periodic boundary
c                  conditions in these two horizontal dimensions.
c                  Experimentation leads to a suggested value of 0.025.
c     nnzteams  =  The number of teams in the X and Z (horizontal)
c                  dimensions.  A team is a set of MPI ranks that
c                  collaboratively update a rectangular solid subset of
c                  the problem domain.  Each team has a team leader MPI
c                  rank that handles all the I/O tasks for the team.
c     nnyteams  =  The number of teams in the Y (vertical) dimension.
c                  Each team will write a separate set of output files.
c                  Thus each data dump consists of  nnyteams*nnzteams**2
c                  sets of disk files.  64 team members per team leader
c                  tends to work well in consolidating the I/O without
c                  adding undue overhead to the run.
c     nntzbricks  =  The number of grid bricks in the Z and X dimensions
c                    for each team.
c     nntybricks  =  The number of grid bricks in the Y dimension for
c                    each team.  Each team has nntybricks*nntzbricks**2
c                    workers and one team leader.
c     nncpucores  =  The number of CPU cores that will cooperatively
c                    update each grid brick.  On Roadrunner this is 8.
c                    On other hardware, this is the number of worker
c                    threads that will update each grid brick.  If this
c                    is more than 1, these threads will use a shared
c                    memory and we MUST have  isomp=1.
c     nnnnx  =  This derived parameter is the number of grid cells in
c               the X dimension in each grid brick.
c     nnnny  =  This derived parameter is the number of grid cells in
c               the Y dimension in each grid brick.
c     nnnnz  =  This derived parameter is the number of grid cells in
c               the Z dimension in each grid brick.
c     nnxfac  =  used to derive  nnnnx  according to the formula.
c     nnyfac  =  used to derive  nnnny  according to the formula.
c                The formula may not be changed.  nnxfac and nnyfac
c                are any desired positive integers.
c     ssmlrho  =  A trivial value of density (see comments at outset).
c     ssmallp  =  A trivial value of pressure (see comments at outset).
c     ssmalle  =  A trivial value of energy (see comments at outset).
c     ssmallu  =  A trivial value of velocity magnitude
c                 (see comments at outset).
c
c&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
c
c
c
c
c
c
c+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
c     THE FOLLOWING PARAMETERS SHOULD NOT BE MODIFIED BY THE USER
c+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
c
#ifdef DACS_NEW_API
#define isDACS31 DACS_NEW_API
#else
#define isDACS31 0
#endif
c
#define isfvhiresAMD 0
c#define isfvhiresAMD 1
#define ishiresout 1*(1-isfvhiresAMD)
#define iscompressedFV 0
c
#ifndef isMIC
#define isNoAccl 1
#endif
c
c     The following set of parameters determine the output.
c
#define isendpasses 0
#define isendpencils 0
#define iiloopsforpencils 1
#define isrankio 0
#define istrace 0
#define isnullwrites 0
#define isrhoio 0
#define ispio 0
#define isuxio 0
#define isuyio 0
#define isuzio 0
#define isfvio 0
#define isX1io 0
#define isY1io 0
#define isZ1io 0
#define isZ2io 0
#define isY2io 0
#define isX2io 0
#define isPPMio 0
#define isPPMvio 0
#define ismsgio 0
c
#define isveltest 0
#define isxperiment 0
#define isyperiment 0
#define iszperiment 0
c
#define sizeof_float (4*(1+is64bit))
#define sizeof_char 1
#define sizeof_int2 2
#define sizeof_int 4
#define sizeof_int8 8
c
c
c     5/9/10:
c     I now append to each face message two records, one of previous
c     time step values and one of previous Courant numbers.  In each
c     such record, there is an entry #0 that gives the number of the
c     most recent time step in the record followed by  nprevtimes
c     entries.  From a given time step number, we compute the remainder
c     relative to  nprevtimes  to find the number of its entry in the
c     history record.  These piggy-backed message payloads allow us to
c     perform lazy global reductions, arriving at valid global results
c     only a few time steps out of date.  We use this to control the
c     time step in fast-flow problems.  We save the global time step
c     and Courant number history back  nprevtimes  steps in our restart
c     dumps.  This allows us to automate going back to the last dump
c     when a pressure hits the floor, for example.  Even if we did not
c     write the dump to disk, we can save it in the master's otherwise
c     useless memory over this interval.
c
c     5/6/10:
c     In this code, each MPI worker rank (intended to occupy an entire
c     multi-CPU node of the interconnection network) updates 8 grid
c     bricks that together (2x2x2) form a single, larger grid brick.
c     This decomposition of a single node's single grid brick into 8
c     sub-bricks dramatically simplifies the MPI messaging.
c     There is a barrier for all the cores in an MPI rank (a single
c     "workerbee") at the end of each sub-brick update.  After this
c     barrier, the master thread both sends and receives one MPI message
c     (an X-face) and also does a local on-node copy of such a message.
c     Then the master thread waits upon an X-message destined for a
c     different subbrick.  After it is received, this is used to augment
c     a Y-message from that subbrick, and that is dispatched.  Then the
c     master thread waits upon an Y-message destined for still another
c     subbrick.  After it is received, it is used to augment a Z-message
c     from that subbrick, and this is dispatched.  We then go to the
c     next subbrick, wait on its Z-message, and, after a thread barrier,
c     update it.  If you think this is simple, well then you will love
c     this code.  It only took about 3 weeks to debug this messaging.
c     On Roadrunner, a single MPI worker rank is assigned to each Cell
c     processor.
c
c     The single-brick part of this code started working about 3/30/10.
c     I have never turned on the PPB diffusion, because it does not now
c     seem to be necessary.  But it is there in case we ever need it.
c
c     2/26/10:   RHO1NU  ON LINE  24890!!!  (case of issphere == 1)
c     The case of the sphere is not at all set up in this code as yet.
c
c     4/7/10:    I have put 45 elements into the profiles, not 35.
c
c     5/1/10:
c      parameter (lendd0y=nsugar*16*(ncubesy+2*nghostcubes)*2)
c     was
c      parameter (lendd0y=nsugar*16*(ncubesy+2*(nghostcubes+1))*2)
c     why??
c
c     If routines to be transformed automatically have their temporary
c     arrays allocated on the stack, then these routines can be
c     optimized in place before being in-lined, and this could have
c     desirable consequences.
c
c     Summary diagnostic output is now (12/28/08) produced by this code.
c     At each dump interval, a fairly complete set of horizontally
c     averaged quantities is computed and printed out in a "Y-Profile"
c     dump file.  A complete time history of several summary quantities
c     up to the time of this dump is written out at the end of this
c     Y-Profile file for the first (master) team.
c     All this binary data is saved in the "aaa" restart dump file.
c
#define ccoefns .00625
#define isover2GB 0
c
#define isnotused 1
#define issaved 0
c
#define is48in1 1
      subroutine ppmmf(xxl,yyb,zzf,dd,ddnu,incx,incy,incz,
     &                 ddghostl,ddghostr,ddnusend,incxsend,
     &                 vvoxels,incvx,
     &               deex,dt,courmx,
     &               smlrho,smallu,ujiggle,time,
     &               nbqx,nbqy,nbqz,jbq,kbq,ipass,itymes,ifvis,ArgsImg,
     &               flopcounts,ifdebug,myrank,mythread,
     &               ibrick,jbrick,kbrick,mbrick,iold,
     &               MyBrickX,MyBrickY,MyBrickZ,
     &               NXBricks,NYBricks,NZBricks,
     &               dddummy1)
c
c
c
c
c                    **************************************************
c                    **************************************************
c                    **************************************************
c                    **************************************************
c                    *****                                        *****
c                    *****       THIS CODE WAS WRITTEN BY         *****
c                    *****                                        *****
c                    *****            PAUL R. WOODWARD            *****
c                    *****                                        *****
c                    *****               May, 1992                *****
c                    *****          REVISED  August, 1992         *****
c                    *****        REVISED  September, 1992        *****
c                    *****           REVISED  July, 1993          *****
c                    *****           REVISED  June, 1997          *****
c                    *****          REVISED  August, 1997         *****
c                    *****        REVISED  September, 2001        *****
c                    *****         REVISED  December, 2005        *****
c                    *****          REVISED  August, 2006         *****
c                    *****         REVISED  November, 2006        *****
c                    *****         REVISED  November, 2007        *****
c                    *****          REVISED  April, 2008          *****
c                    *****         REVISED  December, 2008        *****
c                    *****         REVISED  January, 2009         *****
c                    *****          REVISED  March, 2009          *****
c                    *****           REVISED  July, 2009          *****
c                    *****         REVISED  December, 2009        *****
c                    *****           REVISED  July, 2011          *****
c                    *****                                        *****
c                    *****                                        *****
c                    *****          ALL RIGHTS RESERVED           *****
c                    *****     THAT LEGALLY CAN BE RESERVED       *****
c                    *****    UNDER THE LOS ALAMOS CONTRACT TO    *****
c                    *****     PAUL R. WOODWARD  DATED 2004,7.    *****
c                    *****                                        *****
c                    **************************************************
c                    **************************************************
c                    **************************************************
c                    **************************************************
c
c
c     TO MAKE FROM THIS A WINDOWS DLL I MUST INCLUDE THINGS LIKE:
c
c!MS$ATTRIBUTES DLLEXPORT, STDCALL :: ppmmffairlyslow
c!MS$ATTRIBUTES ALIAS : "PPMMFFAIRLYSLOW" :: ppmmffairlyslow
c!MS$ATTRIBUTES REFERENCE :: xl, rho, p, ux, uy, uz, fair
c!MS$ATTRIBUTES REFERENCE :: rhonu,pnu,uxnu,uynu,uznu,fairnu
c!MS$ATTRIBUTES REFERENCE :: ccdt,cccourmx,ccgamma
c!MS$ATTRIBUTES REFERENCE :: ccsmlrho,ccsmalle,ccsmallp,ccsmallu
c!MS$ATTRIBUTES REFERENCE :: iin,iinbdy,nflopcounts
c
c
c
c
c     nbqx  =   the number of "real" sugar cubes, or briquettes, in the
c               pencil of sugar cubes to be processed in this routine.
c               The grid brick that is being updated is assumed to be a
c               rectangular solid with nbqx x nbqy x nbqz sugar cubes.
c     nsugar =  the number of grid cells on each side of a sugar cube,
c               alternatively referred to as a briquette.
c
c     jbq  = the first transverse index of the pencil of briquettes
c               that are to be processed by this routine.  This index
c               points to the first of two briquettes in the pencil.
c
c     kbq  = the second transverse index of the pencil of briquettes
c               that are to be processed by this routine.  This index
c               points to the first of two briquettes in the pencil.
c
c     iffirstxpass = a flag that is  1  if this is the first x-pass
c                       and that is  0  otherwise.
c
c     iffirstypass = a flag that is  1  if this is the first y-pass
c                       and that is  0  otherwise.
c
c     iflastzpass  = a flag that is  1  if this is the second z-pass
c                       and that is  0  otherwise.
c
c     iflastypass  = a flag that is  1  if this is the second y-pass
c                       and that is  0  otherwise.
c     ifvis   = a flag that is 0 if no visualization data is desired
c               on this call and is 1 if we are to generate it.
c               If this flag is on in the first x-pass, we construct a
c               visualization record, ivvoxels, containing all possible
c               fields that we might want to look at.
c
c     deex    = the width of a grid cell, also the height and depth.
c
c     nbdy    = the number of boundary ghost cells required by this
c               algorithm.  This must be less than or equal to  nsugar.
c
c     DD      = the array in main memory that contains the briquettes
c               of data for the fundamental fluid state variables.
c     incx    = The number of briquettes to skip in order to arrive at
c               the next briquette in the X-direction in  DD.
c     incy    = The number of briquettes to skip in order to arrive at
c               the next briquette in the Y-direction in  DD.
c     incz    = The number of briquettes to skip in order to arrive at
c               the next briquette in the Z-direction in  DD.
c
c
c     dt     = time step.
c     smlrho = a trivial value of density.
c     smallu = a trivial value of velocity.
c     courmx = the maximum Courant number encountered so far (C*dt/dm,
c              or the largest fraction of a zone traversed by a
c              sound wave during the timestep).
c     ujiggle  =   The magnitude of the horizontal velocity with
c                  which we move the entire flow back and forth in
c                  the two horizontal dimensions, X and Z, in each
c                  time step pair.  This can only be done if we
c                  have set the flag for periodic boundary
c                  conditions in these two horizontal dimensions.
c                  Experimentation leads to a suggested value of 0.025.
c
c
c
      parameter (nsugar=nnsugar)
      parameter (nsugarsq=nsugar*nsugar)
      parameter (nsugarcubed=nsugarsq*nsugar)
      parameter (nsugarcubes=nnsugarcubes)
      parameter (nvsugar=nsugar*nsugarcubes)
      parameter (nvsugarcubed=nvsugar*nvsugar*nvsugar)
      parameter (nbdy=nnbdy)
      parameter (nbdy1=nbdy+nsugar-1)
      parameter (nghostcubes=(nbdy1/nsugar))
      parameter (nghostcells=nghostcubes*nsugar)
      parameter (n=nsugar)
      parameter (nx=nsugar)
      parameter (ny=nsugar)
      parameter (nz=nsugar)
      parameter (nxx=nx*nsugarcubes)
      parameter (nyy=ny*nsugarcubes)
      parameter (nzz=nz*nsugarcubes)
      parameter (nssq=nyy*nzz)
c
      parameter (ncs=nsugarcubes*nsugarcubes)
      parameter (nfluids=0)
      parameter (nvarsfld=nfluids*10)
      parameter (nvarsflddiag=nvarsfld)
      parameter (nvars=1+nvarsfld)
      parameter (ninsugarcube=nsugarcubed*nvars)
      parameter (nsugarcubessq=nsugarcubes*nsugarcubes)
c
c
c     The order of the variables in the  DD  array is:
c     rho, p, ux, uy, uz
c
c
      dimension     xxl(nsugar*2,nbqx)
      dimension     yyb(nsugar*2,nbqy)
      dimension     zzf(nsugar*2,nbqz)
      dimension      dd(ninsugarcube,nbqx*nbqy*nbqz)
      dimension    ddnu(ninsugarcube,nbqx*nbqy*nbqz)
      dimension    ddghostl(ninsugarcube)
      dimension    ddghostr(ninsugarcube)
      dimension    ddnusend(ninsugarcube,1+(nbqx-1)*incxsend)
      dimension      dddummy1(ninsugarcube,nbqx*nbqy*nbqz,idummy)
c
c     The arrays  dd  and  ddnu  are dimensioned differently in the
c     calling routine.  They are both augmented bricks with only the
c     extra ghost elements filled in that we need in this 1-D pass.
c     The addresses that we are passed for these arrays are those of
c     the first element of the first briquette in the strip that we
c     will update in this pass.  We are given increments that tell us
c     how to move around from one briquette to another in these arrays.
c     The X-increment moves us along the strip of briquettes in the
c     direction of this pass.
c     7/25/11:
c     The ghost briquette at the left-hand end of our strip to be
c     updated is  ddghostl,  and  ddghostr  is the one at the right-hand
c     end of the strip.  If the value of the briquette increment,
c     incxsend, is zero, then  ddnusend  is the single briquette that
c     we must copy into a send-message array.  This will be either the
c     first or the last briquette in our new strip, depending upon
c     which brick we are and which pass we are in.  If  incxsend  is
c     positive, then we need to copy every new briquette into  ddnusend
c     using this briquette increment to step our way along in this array
c
c     The arrays  xxl, yyb,  and  zzf  are treated similarly.
c     We are given the address of the first element of the first
c     briquette that we will update in this pass.  Locating the data
c     for other briquettes is then easy, but of course we rely on
c     the Fortran compiler's trust in us when we index addresses
c     outside of the array bounds stated in this routine.  We are
c     grown ups.  We can handle this.
c
c     We assume that boundary conditions have already been applied in
c     augmenting the grid brick with ghost briquettes.
c
      character*1   vvoxels(nxx*nyy*nzz,
     &                      (nbqx/nsugarcubes)*(nbqy/nsugarcubes)
     &                                        *(nbqz/nsugarcubes))
c
c
c              1     2   3   4   5   6   7   8
c     vars:  vort, divu, S, rho, p, ux, uy, uz
c
      dimension     ArgsImg(18)
c
      real*8   flopcounts(8)
c
c-----------------------------------------------------------------------
c
      dimension         d(ninsugarcube,nsugarcubes,nsugarcubes,0:1)
c      dimension                mydcube(nsugarcubes,nsugarcubes,0:1)
c      dimension       dnu(ninsugarcube,nsugarcubes,nsugarcubes)
c      dimension      ddbg(ninsugarcube,0:nsugarcubes+1,
c     &                                 0:nsugarcubes+1,2)
c      dimension    dnudbg(ninsugarcube,nsugarcubes,nsugarcubes,0:1)
c
      dimension dc(ny,nz,nx,nvars,nsugarcubes,nsugarcubes,0:1)
      equivalence   (dc,d)
cc
c      dimension   dnuc(ny*nz*nx,nvars,nsugarcubes,nsugarcubes)
c      equivalence   (dnuc,dnu)
cc
c      dimension    xlcube(nsugar*2,0:1)
c      dimension           myxlcube(0:1)
c      dimension    ybcube(nsugar*2,nsugarcubes)
c      dimension    zfcube(nsugar*2,nsugarcubes)
cc
c      character*1   voxels(nxx*nyy*nzz,1,0:1)
c      character*1   voxelcube(nxx*nyy*nzz,0:1)
c      equivalence  (voxelcube,voxels)
cc
c      character*1    voxlrho(nxx,nyy,nzz)
c      equivalence   (voxlrho,voxels(1,1,0))
cc
cc              1     2   3   4   5   6   7   8
cc     vars:  vort, divu, S, rho, p, ux, uy, uz
cc
cc
cc
cc     ALL THE FOLLOWING INPUT ARRAYS ARE PRODUCED IN  DIVERSIFY,
cc     WHICH UNPACKS THE STORAGE ARRAY  D  CONTAINING THE FUNDAMENTAL
cc     FLUID STATE VARIABLES:  RHO, UX, UY, UZ
cc
cc
c      dimension        xl(nx+nghostcells)
c      dimension        xr(nx+nghostcells)
c      dimension        ym(nssq),   zm(nssq),   cylm(nssq)
c      dimension        sintwopicyl(nssq),   costwopir(nssq)
c      dimension        sintwopixl(nx+nghostcells)
c      dimension        cospixl(nx+nghostcells)
cc
cc
cc     Output arrays from Diversify that are used everywhere:
cc
c      dimension       rho(nssq,nx+nghostcells)
cc
c      dimension      rhoc(ny,nz,ncs,nx+nghostcells)
c      equivalence   (rhoc,rho)
cc
cc
cc     Output arrays from FVsCsEs:
cc
c      dimension           dm(nssq,nx+nghostcells)
c      dimension       courno(nssq,nx+nghostcells)
cc
cc
cc     Arrays set for input to ppmintrf0vec:
cc
c      dimension       unsmth(nssq,nx+nghostcells)
cc
cc
cc     Output arrays from ppmintrf0vec:
cc
c      dimension         rhol(nssq,nx+nghostcells)
c      dimension         rhor(nssq,nx+nghostcells)
c      dimension         drho(nssq,nx+nghostcells)
c      dimension         rho6(nssq,nx+nghostcells)
cc      dimension       unsmth(nssq,nx+nghostcells)
cc
cc
cc     Scratch arrays for ppmintrf0vec:
cc
c      dimension          dal(nssq,nx+nghostcells)
c      dimension       absdal(nssq,nx+nghostcells)
c      dimension       dasppm(nssq,nx+nghostcells)
c      dimension       damnot(nssq,nx+nghostcells)
c      dimension       alsmth(nssq,nx+nghostcells)
c      dimension       alunsm(nssq,nx+nghostcells)
cc
cc
cc     Output arrays from Riemann0:
cc
c      dimension        uxavl(nssq,nx+nghostcells)
c      dimension       duxavl(nssq,nx+nghostcells)
c      dimension       sigmal(nssq,nx+nghostcells)
cc
cc
cc     Output arrays from Fluxes:
cc
c      dimension        dvoll(nssq,nx+nghostcells)
c      dimension       dmassl(nssq,nx+nghostcells)
cc
cc
cc     Output arrays from CellUpdate:
cc
c      dimension        rhonu(nssq,nx+nghostcells)
cc
c#if istrace
c      dimension             xldbg(1-nghostcells:nx*nbqx+nghostcells)
c      dimension             xrdbg(1-nghostcells:nx*nbqx+nghostcells)
c      dimension       rhodbg(nssq,1-nghostcells:nx*nbqx+nghostcells)
c      dimension        dmdbg(nssq,1-nghostcells:nx*nbqx+nghostcells)
c      dimension    cournodbg(nssq,1-nghostcells:nx*nbqx+nghostcells)
c      dimension    unsmthdbg(nssq,1-nghostcells:nx*nbqx+nghostcells)
c      dimension      rholdbg(nssq,1-nghostcells:nx*nbqx+nghostcells)
c      dimension      rhordbg(nssq,1-nghostcells:nx*nbqx+nghostcells)
c      dimension      drhodbg(nssq,1-nghostcells:nx*nbqx+nghostcells)
c      dimension      rho6dbg(nssq,1-nghostcells:nx*nbqx+nghostcells)
c      dimension     uxavldbg(nssq,1-nghostcells:nx*nbqx+nghostcells)
c      dimension    duxavldbg(nssq,1-nghostcells:nx*nbqx+nghostcells)
c      dimension    sigmaldbg(nssq,1-nghostcells:nx*nbqx+nghostcells)
c      dimension     dvolldbg(nssq,1-nghostcells:nx*nbqx+nghostcells)
c      dimension    dmassldbg(nssq,1-nghostcells:nx*nbqx+nghostcells)
c      dimension     rhonudbg(nssq,1-nghostcells:nx*nbqx+nghostcells)
c#endif
cc
cc
cc     Visualization arrays:
cc
c      dimension        adiab(nssq,nx+nghostcells)
cc
cc
cc     Scratch arrays:
cc
c      dimension     thing01(nssq,nx)
c      dimension     thing02(nssq,nx)
cc
cc
cc     Scratch arraylets:
cc
c      dimension     thngy01(nssq), thngy02(nssq), thngy03(nssq)
c      dimension     thngy04(nssq), thngy05(nssq), thngy06(nssq)
cc
c      dimension      thngy1(nssq),  thngy2(nssq),        s(nssq)
c      dimension       almon(nssq),   armon(nssq), unsmooth(nssq)
cc
c      dimension      thingl(nssq),  dthing(nssq),   thing6(nssq)
cc
cc
c      real*8   flops,adds,amults,recips,cvmgms,sqrts,rsqrts,exps,oop
c      dimension        ii(1-nx-nghostcells:0)
c      dimension       iis(nx)
cc
cc
cc
cc
cc
cc
cc     It is imperative that all the working data for this routine be on
cc     the stack if OpenMP is to deliver the proper performance.
cc     Introducing a "save" statement, which puts everything on the heap,
cc     is a disaster if we are using OpenMP.  That requires of course
cc     that we put in a "threadprivate" statement, which gives correct
cc     results but not correct performance.  This appears to be inexplic-
cc     able.  It could be related to OpenMP's not keeping its threads
cc     locked onto processor cores, as it should do.  Regardless of where
cc     this working storage is placed in memory, on a cache-based CPU it
cc     should all be cache resident.  Hence there is some bad interaction
cc     of OpenMP and the hardware.  The slow down for putting this data
cc     on the heap is a factor of 2.  Incredible.
cc
cc
cc
c      myifdebug = ifdebug
cc
c      flops = flopcounts(1)
c      adds = flopcounts(2)
c      amults = flopcounts(3)
c      recips = flopcounts(4)
c      cvmgms = flopcounts(5)
c      sqrts = flopcounts(6)
c      rsqrts = flopcounts(7)
c      exps = flopcounts(8)
cc
cc
c      third = 1. / 3.
c      sixth = .5 * third
c      forthd = 4. * third
c      twelth = .25 * third
c      fftnth = .2 * third
c      smlfrc = .005
c      small = .000002
c      almost1 = .999998
c      smaller = .0000001
cc
c      crterr = .1
c      ferrfc = 10.
cc
c      stpner = 20.
c      crtrat = .05
c      smlrat = .1
cc
cc
c      GammaAir = 1.4
c      smlvol = smaller * deex
c      smallmass = smlvol * smlrho
cc
cc
cc      ssmalu = .01 * SmallU
c      smlusq = smallu * smallu
c      ssmalu = .00000001 * SmallU
c      ssmlu2 = ssmalu * ssmalu
cc
c      halfdx = .5 * deex
c      qtrdx = .25 * deex
c      dxinv = 1. / deex
c      halfdxinv = .5 * dxinv
c      dtbydx = dt * dxinv
c      dxbydt = 1. / dtbydx
c      halfdt = .5 * dt
cc
c      ifxpass = 0
c      if ((ipass .eq. 1) .or. (ipass .eq. 6))   ifxpass = 1
c      ifypass = 0
c      if ((ipass .eq. 2) .or. (ipass .eq. 5))   ifypass = 1
c      ifzpass = 0
c      if ((ipass .eq. 3) .or. (ipass .eq. 4))   ifzpass = 1
c      zpass = ifzpass
c      lbrick = ibrick*ifxpass + jbrick*ifypass + kbrick*ifzpass
c      twopie = 8. * atan (1.)
c      twopi = twopie
c      pi = .5 * twopi
cc
c      icoff = - ny*nz
c      do kcube = 1,nsugarcubes
c         do k = 1,nsugar*2
c         zfcube(k,kcube) = zzf(k,kcube)
c         enddo
c      do jcube = 1,nsugarcubes
c         do j = 1,nsugar*2
c         ybcube(j,jcube) = yyb(j,jcube)
c         enddo
c      icoff = icoff + ny*nz
c      jk = 0
c      do kk = 1,nz
c      zz = .5 * (zfcube(kk,kcube) + zfcube(kk+1,kcube))
c!DIR$ SIMD
cc!DEC$ VECTOR ALIGNED
c      do jj = 1,ny
c      jk = jk + 1
c      ym(icoff+jk) = .5 * (ybcube(jj,jcube) + ybcube(jj+1,jcube))
c      zm(icoff+jk) = zz
c      enddo
c      enddo
c      enddo
c      enddo
cc
c      ninloop = nssq
c      oop = ninloop
c      amults = amults + oop
c      adds = adds + oop
cc
c      
cc
c!DEC$ VECTOR ALWAYS
cc!DEC$ VECTOR ALIGNED
c      do j = 1,nssq
c      cylm(j) = ym(j)
c      if (zpass .gt. 0.)   cylm(j) = zm(j)
c      sintwopicyl(j) = sin(twopi*cylm(j))
c      r = sqrt (ym(j)*ym(j) + zm(j)*zm(j))
c      costwopir(j) = cos(twopi*r)
c      enddo
cc
c      cvmgms = cvmgms + oop
c      sqrts = sqrts + oop
c      exps = exps + oop * 2.
c      amults = amults + oop * 4.
c      adds = adds + oop
cc
cc
c      ifstpn = 1
c      icmpar = 1
cc
c      ifmono = 1
c      ifcont = 1
c      courmxignore = 0.
cc      
c      noffinterp = 3
cc
cc
c      do i = 1-nx-nghostcells,0
c      ii(i) = i + nx + nghostcells
c      enddo
c      ic0 = 0
c      ic1 = 1
cc
c      i16m00 = 8
c      i16m01 = 7
c      i16m02 = 6
c      i16m03 = 5
c      i16m04 = 4
c      i16m05 = 3
c      i16m06 = 2
c      i16m07 = 1
cc
cc
cc
cc
c#if isPPMio
c      write (6,*) 'I will now begin the big loop.'
c#endif
cc
cc
cc
cc
cc
cc
cc     We will assume that the fractional volume variables given as
cc     arguments to this routine satisfy the simple constraint that the
cc     cell averages,  fv,  lie between  0  and  1.
cc
cc
cc
cc
cc
cc-----------------------------------------------------------------------
cc
cc
cc
c#if isPPMio
c      write (6,*)   'incx,incy,incz =',incx,incy,incz
cc      if (ipass .eq. 3)
cc     &   write (6,*)   'jcube,kcube,mycube =',jcube,kcube,mycube
c#endif
cc
cc
cc
cc
c      do 9990 icube = -nghostcubes,nbqx+nghostcubes
cc
cc      do i = 1-nx,0
cc      iis(nx+i) = ii(i-nghostcells)
cc      enddo
cc      do i = 1-nx-nghostcells,-nx
cc      ii(i) = ii(i+nx)
cc      enddo
cc      do i = 1-nx,0
cc      ii(i) = iis(nx+i)
cc      enddo
c      ic0 = 1 - ic0
c      ic1 = 1 - ic1
cc
c      i16m11 = i16m07
c      i16m10 = i16m06
c      i16m09 = i16m05
c      i16m08 = i16m04
c      i16m07 = i16m03
c      i16m06 = i16m02
c      i16m05 = i16m01
c      i16m04 = i16m00
c      i16m03 = i16m11
c      i16m02 = i16m10
c      i16m01 = i16m09
c      i16m00 = i16m08
cc
c#if isPPMio
c      write (6,*) 'icube, jbq, kbq =',icube,jbq,kbq
c      write (6,*) '       nbqx, nbqy, nbqz =',nbqx,nbqy,nbqz
c      write (6,*) '       ipass, itymes =',ipass,itymes
c      write (6,*) '       myrank, mythread, mbrick =',
c     &                    myrank, mythread, mbrick
c#endif
cc
cc     We begin by fetching 1+2*nghostcubes planes of grid briquettes.
cc
c      icget = icube + 1
c      if (icget .le. nbqx+nghostcubes)   then
cc
c
c         mycube = 1 + (icget-1)*incx  -  (incy + incz)
c         do kcube = 1,nsugarcubes
c         mycube = mycube + incz
c         micube = mycube
c         do jcube = 1,nsugarcubes
c         micube = micube + incy
c#if ismmprefetch
c         if (icget .eq. 0)   then
c            if ((ifypass .gt. 0) .and.(MyBrickY .eq. 1))   then
c               mydcube(jcube,kcube,ic1) = 1
c               do j = 1,nsugarcubed,16
c                  call mm_prefetch(dd(j,1),2)
c               enddo
c            else
c               mydcube(jcube,kcube,ic1) = -20*incx
c               do j = 1,nsugarcubed,16
c                  call mm_prefetch(ddghostl(j),2)
c               enddo
c            endif
c         elseif (icget .le. nbqx)   then
c            mydcube(jcube,kcube,ic1) = micube
c            do j = 1,nsugarcubed,16
c               call mm_prefetch(dd(j,micube),2)
c            enddo
c         else
c            if ((ifypass .gt. 0) .and.(MyBrickY .eq. NYBricks))   then
c               mydcube(jcube,kcube,ic1) = micube - incx
c               do j = 1,nsugarcubed,16
c                  call mm_prefetch(dd(j,micube-incx),2)
c               enddo
c            else
c               mydcube(jcube,kcube,ic1) = -10*incx
c               do j = 1,nsugarcubed,16
c                  call mm_prefetch(ddghostr(j),2)
c               enddo
c            endif
c         endif
c#else
cc
cc      Remember that    nsugarcubes   must equal   1.
cc
c         if (icget .eq. 0)   then
c            if ((ifypass .gt. 0) .and.(MyBrickY .eq. 1))   then
c               jdestb = 1 - nsugarsq
c               jsrcb = nsugarcubed
c               do iippllnn = 1,nsugar
c               jdestb = jdestb + nsugarsq
c               jsrcb = jsrcb - nsugarsq
c               do j = 1,nsugarsq
c                  d(jdestb+j,jcube,kcube,ic1) = dd(jsrcb+j,1)
c               enddo
c               enddo
c            else
c               do j = 1,nsugarcubed
c                  d(j,jcube,kcube,ic1) = ddghostl(j)
c               enddo
c            endif
c         elseif (icget .le. nbqx)   then
c            do j = 1,nsugarcubed
c               d(j,jcube,kcube,ic1) = dd(j,micube)
c            enddo
c         else
c            if ((ifypass .gt. 0) .and.(MyBrickY .eq. NYBricks))   then
c               jdestb = 1 - nsugarsq
c               jsrcb = nsugarcubed
c               do iippllnn = 1,nsugar
c               jdestb = jdestb + nsugarsq
c               jsrcb = jsrcb - nsugarsq
c               do j = 1,nsugarsq
c                  d(jdestb+j,jcube,kcube,ic1) = dd(jsrcb+j,micube-incx)
c               enddo
c               enddo
c            else
c               do j = 1,nsugarcubed
c                  d(j,jcube,kcube,ic1) = ddghostr(j)
c               enddo
c            endif
c         endif
c#endif
c         enddo
c         enddo
cc
c#if ismmprefetch
c         myxlcube(ic1) = icget
c         call mm_prefetch(xxl(1,icget),2)
c#else
c         do i = 1,nsugar*2
c         xlcube(i,ic1) = xxl(i,icget)
c         enddo
c#endif
cc
c         if (icube .lt. 1-nghostcubes)   go to 9990
c      endif
cc
cccccccccccccccccccccccccc  Fake memory write ccccccccccccccccccccccccc
c      do kcube = 1,nsugarcubes
c      do jcube = 1,nsugarcubes
c      mynucube = 1  +  (nbqx- (icube - nghostcubes)) * incx
c     &              +  (jcube - 1) * incy  +  (kcube - 1) * incz
c!DEC$ VECTOR ALWAYS
cc!DEC$ VECTOR ALIGNED
c      do k = 1,nsugarcubed
c         dddummy1(k,mynucube,1) = d(k,jcube,kcube,ic1)
c         dddummy1(k,mynucube,2) = d(k,jcube,kcube,ic1)
c         dddummy1(k,mynucube,3) = d(k,jcube,kcube,ic1)
c         dddummy1(k,mynucube,4) = d(k,jcube,kcube,ic1)
c         dddummy1(k,mynucube,5) = d(k,jcube,kcube,ic1)
c         dddummy1(k,mynucube,6) = d(k,jcube,kcube,ic1)
c         dddummy1(k,mynucube,7) = d(k,jcube,kcube,ic1)
c         dddummy1(k,mynucube,8) = d(k,jcube,kcube,ic1)
c      enddo
c      enddo
c      enddo
ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
cc
cc     Now we continue by unpacking the plane of grid briquettes that we
cc     prefetched on the previous traversal of this loop on icube.
cc     That briquette corresponds to the index  icube.
cc
c#if isPPMio
c      write (6,*) 'I am about to unpack cube ic0 =',ic0
c#endif
cc
cc      do i = 1-nx,0
c#if ismmprefetch
c      xl(i16m00) = xxl(4,myxlcube(ic0))
c      xr(i16m00) = xxl(5,myxlcube(ic0))
cc
c      xl(i16m01) = xxl(3,myxlcube(ic0))
c      xr(i16m01) = xxl(4,myxlcube(ic0))
cc
c      xl(i16m02) = xxl(2,myxlcube(ic0))
c      xr(i16m02) = xxl(3,myxlcube(ic0))
cc
c      xl(i16m03) = xxl(1,myxlcube(ic0))
c      xr(i16m03) = xxl(2,myxlcube(ic0))
c#else
c      xl(i16m00) = xlcube(4,ic0)
c      xr(i16m00) = xlcube(5,ic0)
cc
c      xl(i16m01) = xlcube(3,ic0)
c      xr(i16m01) = xlcube(4,ic0)
cc
c      xl(i16m02) = xlcube(2,ic0)
c      xr(i16m02) = xlcube(3,ic0)
cc
c      xl(i16m03) = xlcube(1,ic0)
c      xr(i16m03) = xlcube(2,ic0)
c#endif
cc      enddo
cc
cc
cc
cc     I know that I run past the array bound on the first indices below,
cc     but the compiler allows it, and it is much more efficient code.
cc     Equivalences on subroutine arguments are not allowed by the
cc     compiler, so the compiler writers made me do it.
cc
cc
c#if ismmprefetch
c      if (icube .eq. 0)   then
c         if ((ifypass .gt. 0) .and.(MyBrickY .eq. 1))   then
c            nc = 0
c            do kcube = 1,nsugarcubes
c            do jcube = 1,nsugarcubes
c            nc = nc + 1
ccdir$ ivdep
c!DEC$ VECTOR ALWAYS
cc!DEC$ VECTOR ALIGNED
c            do j = 1,ny*nz
c        rhoc(j,1,nc,i16m03) = dd(j+3*ny*nz,mydcube(jcube,kcube,ic0))
c        rhoc(j,1,nc,i16m02) = dd(j+2*ny*nz,mydcube(jcube,kcube,ic0))
c        rhoc(j,1,nc,i16m01) = dd(j+ny*nz,mydcube(jcube,kcube,ic0))
c        rhoc(j,1,nc,i16m00) = dd(j,mydcube(jcube,kcube,ic0))
c            enddo
c            enddo
c            enddo
c         else
c            nc = 0
c            do kcube = 1,nsugarcubes
c            do jcube = 1,nsugarcubes
c            nc = nc + 1
ccdir$ ivdep
c!DEC$ VECTOR ALWAYS
cc!DEC$ VECTOR ALIGNED
c            do j = 1,ny*nz
c              rhoc(j,1,nc,i16m00) = ddghostl(j+3*ny*nz)
c              rhoc(j,1,nc,i16m01) = ddghostl(j+2*ny*nz)
c              rhoc(j,1,nc,i16m02) = ddghostl(j+ny*nz)
c              rhoc(j,1,nc,i16m03) = ddghostl(j)
c            enddo
c            enddo
c            enddo
c         endif
c      elseif (icube .le. nbqx)   then
c      nc = 0
c      do kcube = 1,nsugarcubes
c      do jcube = 1,nsugarcubes
c      nc = nc + 1
ccdir$ ivdep
c!DEC$ VECTOR ALWAYS
cc!DEC$ VECTOR ALIGNED
c      do j = 1,ny*nz
c        rhoc(j,1,nc,i16m00) = dd(j+3*ny*nz,mydcube(jcube,kcube,ic0))
c        rhoc(j,1,nc,i16m01) = dd(j+2*ny*nz,mydcube(jcube,kcube,ic0))
c        rhoc(j,1,nc,i16m02) = dd(j+ny*nz,mydcube(jcube,kcube,ic0))
c        rhoc(j,1,nc,i16m03) = dd(j,mydcube(jcube,kcube,ic0))
c      enddo
c      enddo
c      enddo
cc
cc     7/25/11:
cc     At this point, we want to evict the cache lines holding the values
cc     of DD from the cache.  The OpenMP flush command will do this, but
cc     it will screw up all the other executing threads, because its
cc     granularity is not fine enough.  The intrinsic, mm_clflush, call-
cc     able only from C, will evict just one cache line from our cache.
cc     This is precisely what we want.  Otherwise our cache will fill up
cc     with useless junk, preventing us from making efficient use of it.
cc
c      else
c         if ((ifypass .gt. 0) .and.(MyBrickY .eq. NYBricks))   then
c            nc = 0
c            do kcube = 1,nsugarcubes
c            do jcube = 1,nsugarcubes
c            nc = nc + 1
ccdir$ ivdep
c!DEC$ VECTOR ALWAYS
cc!DEC$ VECTOR ALIGNED
c            do j = 1,ny*nz
c        rhoc(j,1,nc,i16m03) = dd(j+3*ny*nz,mydcube(jcube,kcube,ic0))
c        rhoc(j,1,nc,i16m02) = dd(j+2*ny*nz,mydcube(jcube,kcube,ic0))
c        rhoc(j,1,nc,i16m01) = dd(j+ny*nz,mydcube(jcube,kcube,ic0))
c        rhoc(j,1,nc,i16m00) = dd(j,mydcube(jcube,kcube,ic0))
c            enddo
c            enddo
c            enddo
c         else
c            nc = 0
c            do kcube = 1,nsugarcubes
c            do jcube = 1,nsugarcubes
c            nc = nc + 1
ccdir$ ivdep
c!DEC$ VECTOR ALWAYS
cc!DEC$ VECTOR ALIGNED
c            do j = 1,ny*nz
c              rhoc(j,1,nc,i16m00) = ddghostr(j+3*ny*nz)
c              rhoc(j,1,nc,i16m01) = ddghostr(j+2*ny*nz)
c              rhoc(j,1,nc,i16m02) = ddghostr(j+ny*nz)
c              rhoc(j,1,nc,i16m03) = ddghostr(j)
c            enddo
c            enddo
c            enddo
c         endif
c      endif
c#else
c      nc = 0
c      do kcube = 1,nsugarcubes
c      do jcube = 1,nsugarcubes
c      nc = nc + 1
cc      do i = 1-nx,0
ccdir$ ivdep
c!DEC$ VECTOR ALWAYS
cc!DEC$ VECTOR ALIGNED
c      do j = 1,ny*nz
c        rhoc(j,1,nc,i16m00) = dc(j,1,4,1,jcube,kcube,ic0)
c        rhoc(j,1,nc,i16m01) = dc(j,1,3,1,jcube,kcube,ic0)
c        rhoc(j,1,nc,i16m02) = dc(j,1,2,1,jcube,kcube,ic0)
c        rhoc(j,1,nc,i16m03) = dc(j,1,1,1,jcube,kcube,ic0)
c      enddo
cc      enddo
c      enddo
c      enddo
c#endif
cc
cc     We cannot divide by zero in any of the loops below.
cc
c      if (ifypass .eq. 0)   then
cc
c      sintwopixl(i16m00) = sin (twopi * xl(i16m00))
c      sintwopixl(i16m01) = sin (twopi * xl(i16m01))
c      sintwopixl(i16m02) = sin (twopi * xl(i16m02))
c      sintwopixl(i16m03) = sin (twopi * xl(i16m03))
cc
c!DEC$ VECTOR ALWAYS
cc!DEC$ VECTOR ALIGNED
c      do j = 1,nssq
c      uxavl(j,i16m00) = sintwopixl(i16m00) * sintwopicyl(j)
c      uxavl(j,i16m01) = sintwopixl(i16m01) * sintwopicyl(j)
c      uxavl(j,i16m02) = sintwopixl(i16m02) * sintwopicyl(j)
c      uxavl(j,i16m03) = sintwopixl(i16m03) * sintwopicyl(j)
c      enddo
cc
c      else
cc
c      cospixl(i16m00) = cos (pi * xl(i16m00))
c      cospixl(i16m01) = cos (pi * xl(i16m01))
c      cospixl(i16m02) = cos (pi * xl(i16m02))
c      cospixl(i16m03) = cos (pi * xl(i16m03))
cc
c!DEC$ VECTOR ALWAYS
cc!DEC$ VECTOR ALIGNED
c      do j = 1,nssq
c      uxavl(j,i16m00) = cospixl(i16m00) * costwopir(j)
c      uxavl(j,i16m01) = cospixl(i16m01) * costwopir(j)
c      uxavl(j,i16m02) = cospixl(i16m02) * costwopir(j)
c      uxavl(j,i16m03) = cospixl(i16m03) * costwopir(j)
c      enddo
cc
c      endif
cc
c      ninloop = nssq
c      oop = ninloop
c      amults = amults + oop * 5.
c      exps = exps + 4.
cc
cc
cc     Now we have all the principal arrays unpacked.
cc
cc
c#if istrace
c      iplnm0 = icube * 4
c      iplnm1 = iplnm0 - 1
c      iplnm2 = iplnm0 - 2
c      iplnm3 = iplnm0 - 3
c      iplnm4 = iplnm0 - 4
c      iplnm5 = iplnm0 - 5
c      iplnm6 = iplnm0 - 6
c      iplnm7 = iplnm0 - 7
c!DEC$ VECTOR ALWAYS
c      do j = 1,nssq
c      xldbg(iplnm0) = xl(i16m00)
c      xldbg(iplnm1) = xl(i16m01)
c      xldbg(iplnm2) = xl(i16m02)
c      xldbg(iplnm3) = xl(i16m03)
cc
c      xrdbg(iplnm0) = xr(i16m00)
c      xrdbg(iplnm1) = xr(i16m01)
c      xrdbg(iplnm2) = xr(i16m02)
c      xrdbg(iplnm3) = xr(i16m03)
cc
c      rhodbg(j,iplnm0) = rho(j,i16m00)
c      rhodbg(j,iplnm1) = rho(j,i16m01)
c      rhodbg(j,iplnm2) = rho(j,i16m02)
c      rhodbg(j,iplnm3) = rho(j,i16m03)
cc
c      uxavldbg(j,iplnm0) = uxavl(j,i16m00)
c      uxavldbg(j,iplnm1) = uxavl(j,i16m01)
c      uxavldbg(j,iplnm2) = uxavl(j,i16m02)
c      uxavldbg(j,iplnm3) = uxavl(j,i16m03)
c      enddo
c#endif
cc
cc     In the Y-pass, we have a reflecting boundary condition, but it is       
cc     modified by a pressure adjustment that reflects hydrostatic             
cc     equilibrium.  At the bottom of the brick, we must increase the          
cc     pressure, by an amount equal to    - rho * g * (y-increment)            
cc     At the top of the brick, we decrease the pressure by an amount =        
cc     rho * g * (y-increment). 
cc     We do not do this here, because we now assume that all boundary
cc     conditions are set in the calling routine, when the grid brick is
cc     augmented with ghost cells.                                               
cc
cc     Remember that we are always in the X-pass.
cc
cc
cc
cc     We can fully update a plane of grid briquettes when we have
cc     unpacked  1 + 2*nghostcubes  planes of briquettes.
cc     Here we wait for this condition to be fulfilled, but in the
cc     automatically pipelined translation we will begin computing as
cc     soon as we have unpacked one plane of briquettes.
cc
cc
cc     We begin by calling the routines that use the transverse informa-
cc     tion.  This will allow us to overwrite those workspace arrays
cc     once these calls are completed (in a GPU-oriented translation).
cc     This assumes that we go down the entire strip of grid briquettes
cc     for each kernel subroutine that is called below.
cc
cc
cc
cc
cc
cc
ccc      do i = 1-nbdy,n+nbdy
cc      do i = 1-nx,0
c      do iii = 1, itercomp
c!DEC$ VECTOR ALWAYS
cc!DEC$ VECTOR ALIGNED
c      do jk = 1,nssq
c      dm(jk,i16m00) = rho(jk,i16m00) * deex
c      courno(jk,i16m00) = abs(uxavl(jk,i16m00)) * dtbydx
cc
c      dm(jk,i16m01) = rho(jk,i16m01) * deex
c      courno(jk,i16m01) = abs(uxavl(jk,i16m01)) * dtbydx
cc
c      dm(jk,i16m02) = rho(jk,i16m02) * deex
c      courno(jk,i16m02) = abs(uxavl(jk,i16m02)) * dtbydx
cc
c      dm(jk,i16m03) = rho(jk,i16m03) * deex
c      courno(jk,i16m03) = abs(uxavl(jk,i16m03)) * dtbydx
c      enddo
cc      enddo
cc
c      ninloop = nssq * 4
c      oop = ninloop
c      cvmgms = cvmgms + oop
c      amults = amults + oop * 2.
cc
c      enddo
cc
c#if istrace
c!DEC$ VECTOR ALWAYS
c      do j = 1,nssq
c      dmdbg(j,iplnm0) = dm(j,i16m00)
c      dmdbg(j,iplnm1) = dm(j,i16m01)
c      dmdbg(j,iplnm2) = dm(j,i16m02)
c      dmdbg(j,iplnm3) = dm(j,i16m03)
cc
c      cournodbg(j,iplnm0) = courno(j,i16m00)
c      cournodbg(j,iplnm1) = courno(j,i16m01)
c      cournodbg(j,iplnm2) = courno(j,i16m02)
c      cournodbg(j,iplnm3) = courno(j,i16m03)
c      enddo
c#endif
cc
cc
cc
cc      ifstpn = 1
cc      icmpar = 1
cc
cc
cc
cc
cc      sixth = 1. / 6.
cc
cc     First compute the properties of an interpolation parabola,
cc     assuming that the function is smooth.
cc
cc
cc
cc
c      if (icube .ge. 2-nghostcubes)   then
cc
cc
cc     The code for the second and all subsequent briquettes begins here.
cc     ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
cc
cc
cc
cc
ccc      do   i = -nbdy+3,n+nbdy-2
cc      ibegin = 1-nx-2
cc      ibegin = max(ibegin,3-nx*(icube+nghostcubes))
cc      if (ibegin .le. -2)   then
cc      do i = ibegin,-2
c!DEC$ VECTOR ALWAYS
cc!DEC$ VECTOR ALIGNED
c      do jk = 1,nssq
c#if isviolent
c      unsmth(jk,i16m02) = 1.
c      unsmth(jk,i16m03) = 1.
c      unsmth(jk,i16m04) = 1.
c      unsmth(jk,i16m05) = 1.
c#else
c      unsmth(jk,i16m02) = 0.
c      unsmth(jk,i16m03) = 0.
c      unsmth(jk,i16m04) = 0.
c      unsmth(jk,i16m05) = 0.
c#endif
c      enddo
cc      enddo
cc      endif
cc
c#if istrace
c!DEC$ VECTOR ALWAYS
c      do j = 1,nssq
c      unsmthdbg(j,iplnm2) = unsmth(j,i16m02)
c      unsmthdbg(j,iplnm3) = unsmth(j,i16m03)
c      unsmthdbg(j,iplnm4) = unsmth(j,i16m04)
c      unsmthdbg(j,iplnm5) = unsmth(j,i16m05)
c      enddo
c#endif
cc
cc
cc
ccc      do 100   i = 2-nbdy,n+nbdy
cc      ibegin = 1-nx
cc      ibegin = max(ibegin,2-nx*(icube+nghostcubes))
cc      do 100   i = ibegin,0
c      do iii = 1, itercomp
c!DEC$ VECTOR ALWAYS
cc!DEC$ VECTOR ALIGNED
c      do jk = 1,nssq
c      dal(jk,i16m00) = rho(jk,i16m00) - rho(jk,i16m01)
c      absdal(jk,i16m00) = abs (dal(jk,i16m00))
cc
c      dal(jk,i16m01) = rho(jk,i16m01) - rho(jk,i16m02)
c      absdal(jk,i16m01) = abs (dal(jk,i16m01))
cc
c      dal(jk,i16m02) = rho(jk,i16m02) - rho(jk,i16m03)
c      absdal(jk,i16m02) = abs (dal(jk,i16m02))
cc
c      dal(jk,i16m03) = rho(jk,i16m03) - rho(jk,i16m04)
c      absdal(jk,i16m03) = abs (dal(jk,i16m03))
c      enddo
cc100   continue
cc
c      ninloop = nssq * 4
c      oop = ninloop
c      recips = recips + 1.
c      cvmgms = cvmgms + oop
c      amults = amults + 1.
c      adds = adds + oop + 2.
cc
c      enddo
cc      do 1000   i = -mbdy-1,n+mbdy+2
ccc     da(i,jk) = .5 * (dal(i,jk) + dal(i,jk+1))
ccc     a6(i,jk) = .5 * (dal(i,jk) - dal(i,jk+1))
cc      dasppm(i,jk) = .5 * (dal(i,jk) + dal(i,jk+1))
cc1000  continue
cc
cc     dasppm  is an estimate of the variable difference in zone (i) 
cc             based only upon an assumption that the function is 
cc             smooth (a generalization of Fromm's slope for a
cc             non-uniform grid).  It is the variable difference
cc             in the zone for the unique parabola which has the
cc             prescribed average values in the 3 zones centered on (i).
cc
cc     Now compute the indicator function,  unsmth,  which is positive
cc     in zones where monotonicity constraints may be required and
cc     vanishes elsewhere.  It is constructed from  ferror,  an
cc     estimate of the interpolation errors associated with the
cc     parabolic fits to the data near a given zone.
cc
cc      crterr = .1
cc      ferrfc = 10.
cc
cc      do 2000   i = -mbdy,n+mbdy+2
cc      absdal(jk,i) = abs (dal(jk,i))
cc      alsmth(jk,i) =  a(jk,i)  -  .5 * dal(jk,i)  -
cc     1             sixth * (dasppm(jk,i) - dasppm(jk,i-1))
cc2000  continue
cc
cc     azrdif = (azrext - azr),   see notes at beginning of routine.
cc     azldif = (azlext - azl),   see notes at beginning of routine.
cc
cc     The factor of (1/2) in  ferror  below is of purely historical
cc     origin.  It could be removed, with appropriate readjustment of
cc     various dimensionless constants.
cc
cc
cc      write (6,*) '     (a(1,i),i=1,nx) =',(a(1,i),i=1,nx)
cc      write (6,*) '   (dal(1,i),i=1,nx) =',(dal(1,i),i=1,nx)
cc      write (6,*) '(absdal(1,i),i=1,nx) =',(absdal(1,i),i=1,nx)
cc      write (6,*) '(smalda(1,i),i=1,nx) =',(smalda(1,i),i=1,nx)
cc
cc
cc
ccc      do 5000   i = 3-nbdy,n+nbdy-2
cc      ibegin = 1-nx-2
cc      ibegin = max(ibegin,3-nx*(icube+nghostcubes))
cc      if (ibegin .le. -2)   then
cc      do 5000   i = ibegin,-2
c      do iii = 1, itercomp
c!DEC$ VECTOR ALWAYS
cc!DEC$ VECTOR ALIGNED
c      do jk = 1,nssq
c#if isviolent
c      unsmth(jk,i16m02) = 1.
c#else
c      adiff = rho(jk,i16m01) - rho(jk,i16m03)
c      azrdif = 3. * dal(jk,i16m02)  -  dal(jk,i16m03)  -  adiff
c      azldif = dal(jk,i16m00)  -  3. * dal(jk,i16m01)  +  adiff
cc     ferror = .5 * ( abs (azldif)  +  abs (azrdif) )  /
cc    &       (absdal(jk,i16m02) + absdal(jk,i16m01) + smalda(jk,i16m02))
c      thngy1(jk) = azldif
c      if (thngy1(jk) .lt. 0.)   thngy1(jk) = - thngy1(jk)
c      thngy2(jk) = azrdif
c      if (thngy2(jk) .lt. 0.)   thngy2(jk) = - thngy2(jk)
c      ferror = .5 * (thngy1(jk) + thngy2(jk))  /
c     &      (absdal(jk,i16m02) + absdal(jk,i16m01) + small)
cc     unsmooth(jk) = min (1.,  max (0., ferrfc * (ferror - crterr)))
c      unsmooth(jk) = ferrfc * (ferror - crterr)
c      if (unsmooth(jk) .lt. 0.)   unsmooth(jk) = 0.
c      if (unsmooth(jk) .gt. 1.)   unsmooth(jk) = 1.
cc     unsmth(jk,i16m02) = max (unsmooth(jk), unsmth(jk,i16m02))
c      if (unsmooth(jk) .gt. unsmth(jk,i16m02))
c     &                      unsmth(jk,i16m02) = unsmooth(jk)
c#endif
cc
cc     THIS ROUTINE HAS BEEN MODIFIED (7/27/01) SO THAT THE ARRAY  UNSMTH
cc     IS NOW GIVEN INITIAL VALUES IN THE CALLING ROUTINE.  THE IDEA IS
cc     THAT THE CALLING ROUTINE MAY ALREADY KNOW WHERE THE SHOCKS ARE,
cc     AND MAY WISH TO DEMAND THAT THE VALUE OF  UNSMTH  COMPUTED HERE
cc     AND RETURNED TO THE CALLING ROUTINE (AND USED HERE TO GENERATE THE
cc     INTERPOLATION PARABOLA) SHOULD BE AT LEAST AS LARGE AS SOME NUMBER
cc     WHICH HAS BEEN COMPUTED EARLIER (SUCH AS THE ARRAY  SHOCKD).
cc
cc     Now, in zones marked by  unsmth  we will apply monotonicity
cc     constraints.
cc
cc     For the case where the function is smooth:
cc
cc     al(jk,i16m02) = alsmth(jk,i16m02)
cc     ar(jk,i16m02) = alsmth(jk,i16m01)
cc     da(jk,i16m02) = ar(jk,i16m02) - al(jk,i16m02)
cc     a6(jk,i16m02) = 6. * (a(jk,i16m02)
cc    &                  -  .5 * (al(jk,i16m02) + ar(jk,i16m02)))
cc     damnot(jk,i16m02) = dasppm(jk,i16m02)
cc
cc     When  unsmth(jk,i16m02)  is positive,
cc     extrapolation of our parabolic fits to the function do not
cc     give accurate results.  Therefore the function is not
cc     sufficiently smooth here to justify these fits.  To account
cc     for this lack of smoothness, and to keep our interpolations
cc     reasonable, if perhaps inaccurate, we must apply monotonicity
cc     constraints.  We begin by monotonizing the variable
cc     differences  da.  We will turn on these constraints
cc     continuously, using the factors  unsmth  and  smooth.
cc     The constraints are turned on continuously, beginning when
cc     unsmth(jk,i16m02)  is first positive and ending (fully on) when
cc     unsmth(jk,i16m02)  reaches unity.
cc
c      dasppm(jk,i16m02) = .5 * (dal(jk,i16m02) + dal(jk,i16m01))
c      s(jk) = 1.
c      if (dasppm(jk,i16m02) .lt. 0.)   s(jk) = -1.
cc     damax = 2.  *  min (s(jk)*dal(jk,i16m02), s(jk)*dal(jk,i16m01))
cc     damon = min (s(jk)*dasppm(jk,i16m02), damax)
cc     damon = s(jk)  *  max (damon, 0.)
c      thngy1(jk) = s(jk) * dal(jk,i16m02)
c      thngy2(jk) = s(jk) * dal(jk,i16m01)
c      if (thngy2(jk) .lt. thngy1(jk))   thngy1(jk) = thngy2(jk)
c      thngy1(jk) = 2. * thngy1(jk)
c      thngy2(jk) = s(jk) * dasppm(jk,i16m02)
c      if (thngy2(jk) .lt. thngy1(jk))   thngy1(jk) = thngy2(jk)
c      if (thngy1(jk) .lt. 0.)   thngy1(jk) = 0.
c      damnot(jk,i16m02) = s(jk) * thngy1(jk)
cc
cc
c#if isviolent
c      unsmth(jk,i16m03) = 1.
c#else
c      adiff = rho(jk,i16m02) - rho(jk,i16m04)
c      azrdif = 3. * dal(jk,i16m03)  -  dal(jk,i16m04)  -  adiff
c      azldif = dal(jk,i16m01)  -  3. * dal(jk,i16m02)  +  adiff
c      thngy1(jk) = azldif
c      if (thngy1(jk) .lt. 0.)   thngy1(jk) = - thngy1(jk)
c      thngy2(jk) = azrdif
c      if (thngy2(jk) .lt. 0.)   thngy2(jk) = - thngy2(jk)
c      ferror = .5 * (thngy1(jk) + thngy2(jk))  /
c     &      (absdal(jk,i16m03) + absdal(jk,i16m02) + small)
c      unsmooth(jk) = ferrfc * (ferror - crterr)
c      if (unsmooth(jk) .lt. 0.)   unsmooth(jk) = 0.
c      if (unsmooth(jk) .gt. 1.)   unsmooth(jk) = 1.
c      if (unsmooth(jk) .gt. unsmth(jk,i16m03))
c     &                      unsmth(jk,i16m03) = unsmooth(jk)
c#endif
c      dasppm(jk,i16m03) = .5 * (dal(jk,i16m03) + dal(jk,i16m02))
c      s(jk) = 1.
c      if (dasppm(jk,i16m03) .lt. 0.)   s(jk) = -1.
c      thngy1(jk) = s(jk) * dal(jk,i16m03)
c      thngy2(jk) = s(jk) * dal(jk,i16m02)
c      if (thngy2(jk) .lt. thngy1(jk))   thngy1(jk) = thngy2(jk)
c      thngy1(jk) = 2. * thngy1(jk)
c      thngy2(jk) = s(jk) * dasppm(jk,i16m03)
c      if (thngy2(jk) .lt. thngy1(jk))   thngy1(jk) = thngy2(jk)
c      if (thngy1(jk) .lt. 0.)   thngy1(jk) = 0.
c      damnot(jk,i16m03) = s(jk) * thngy1(jk)
cc
cc
c#if isviolent
c      unsmth(jk,i16m04) = 1.
c#else
c      adiff = rho(jk,i16m03) - rho(jk,i16m05)
c      azrdif = 3. * dal(jk,i16m04)  -  dal(jk,i16m05)  -  adiff
c      azldif = dal(jk,i16m02)  -  3. * dal(jk,i16m03)  +  adiff
c      thngy1(jk) = azldif
c      if (thngy1(jk) .lt. 0.)   thngy1(jk) = - thngy1(jk)
c      thngy2(jk) = azrdif
c      if (thngy2(jk) .lt. 0.)   thngy2(jk) = - thngy2(jk)
c      ferror = .5 * (thngy1(jk) + thngy2(jk))  /
c     &      (absdal(jk,i16m04) + absdal(jk,i16m03) + small)
c      unsmooth(jk) = ferrfc * (ferror - crterr)
c      if (unsmooth(jk) .lt. 0.)   unsmooth(jk) = 0.
c      if (unsmooth(jk) .gt. 1.)   unsmooth(jk) = 1.
c      if (unsmooth(jk) .gt. unsmth(jk,i16m04))
c     &                      unsmth(jk,i16m04) = unsmooth(jk)
c#endif
c      dasppm(jk,i16m04) = .5 * (dal(jk,i16m04) + dal(jk,i16m03))
c      s(jk) = 1.
c      if (dasppm(jk,i16m04) .lt. 0.)   s(jk) = -1.
c      thngy1(jk) = s(jk) * dal(jk,i16m04)
c      thngy2(jk) = s(jk) * dal(jk,i16m03)
c      if (thngy2(jk) .lt. thngy1(jk))   thngy1(jk) = thngy2(jk)
c      thngy1(jk) = 2. * thngy1(jk)
c      thngy2(jk) = s(jk) * dasppm(jk,i16m04)
c      if (thngy2(jk) .lt. thngy1(jk))   thngy1(jk) = thngy2(jk)
c      if (thngy1(jk) .lt. 0.)   thngy1(jk) = 0.
c      damnot(jk,i16m04) = s(jk) * thngy1(jk)
cc
cc
c#if isviolent
c      unsmth(jk,i16m05) = 1.
c#else
c      adiff = rho(jk,i16m04) - rho(jk,i16m06)
c      azrdif = 3. * dal(jk,i16m05)  -  dal(jk,i16m06)  -  adiff
c      azldif = dal(jk,i16m03)  -  3. * dal(jk,i16m04)  +  adiff
c      thngy1(jk) = azldif
c      if (thngy1(jk) .lt. 0.)   thngy1(jk) = - thngy1(jk)
c      thngy2(jk) = azrdif
c      if (thngy2(jk) .lt. 0.)   thngy2(jk) = - thngy2(jk)
c      ferror = .5 * (thngy1(jk) + thngy2(jk))  /
c     &      (absdal(jk,i16m05) + absdal(jk,i16m04) + small)
c      unsmooth(jk) = ferrfc * (ferror - crterr)
c      if (unsmooth(jk) .lt. 0.)   unsmooth(jk) = 0.
c      if (unsmooth(jk) .gt. 1.)   unsmooth(jk) = 1.
c      if (unsmooth(jk) .gt. unsmth(jk,i16m05))
c     &                      unsmth(jk,i16m05) = unsmooth(jk)
c#endif
c      dasppm(jk,i16m05) = .5 * (dal(jk,i16m05) + dal(jk,i16m04))
c      s(jk) = 1.
c      if (dasppm(jk,i16m05) .lt. 0.)   s(jk) = -1.
c      thngy1(jk) = s(jk) * dal(jk,i16m05)
c      thngy2(jk) = s(jk) * dal(jk,i16m04)
c      if (thngy2(jk) .lt. thngy1(jk))   thngy1(jk) = thngy2(jk)
c      thngy1(jk) = 2. * thngy1(jk)
c      thngy2(jk) = s(jk) * dasppm(jk,i16m05)
c      if (thngy2(jk) .lt. thngy1(jk))   thngy1(jk) = thngy2(jk)
c      if (thngy1(jk) .lt. 0.)   thngy1(jk) = 0.
c      damnot(jk,i16m05) = s(jk) * thngy1(jk)
c      enddo
cc5000  continue
cc
c      ninloop = nssq * 4
c      oop = ninloop
c#if isviolent
c      cvmgms = cvmgms + oop * 4.
c      amults = amults + oop * 6.
c      adds = adds + oop * 3.
c#else
c      recips = recips + oop
c      cvmgms = cvmgms + oop * 9.
c      amults = amults + oop * 12.
c      adds = adds + oop * 14.
c#endif
cc
c      enddo
cc
c#if istrace
c!DEC$ VECTOR ALWAYS
c      do j = 1,nssq
c      unsmthdbg(j,iplnm2) = unsmth(j,i16m02)
c      unsmthdbg(j,iplnm3) = unsmth(j,i16m03)
c      unsmthdbg(j,iplnm4) = unsmth(j,i16m04)
c      unsmthdbg(j,iplnm5) = unsmth(j,i16m05)
c      enddo
c#endif
cc
cc      write (6,*) '(  unsmth(1,i),i=1,nx) =',(unsmth(1,i),i=1,nx)
cc      write (6,*) '(  dasppm(1,i),i=1,nx) =',(dasppm(1,i),i=1,nx)
cc      write (6,*) '(  damnot(1,i),i=1,nx) =',(damnot(1,i),i=1,nx)
cc
cc     At this point, all smooth zones have their interpolation
cc     parabolae set.  In zones where the function is not smooth,
cc     we have reset the variable differences  dasppm  to give
cc     monotonized differences  damnot  (also set in smooth zones).
cc     Now, in those unsmooth zones (only) we need to construct
cc     the left and right interface values.  We do this by
cc     computing the left interface values for all unsmooth zones
cc     and for all smooth zones which have an unsmooth zone on
cc     their left.
cc     When vectorizing this code, it is necessary to have an idea of how
cc     many cells are likely to be designated as unsmooth.  I had hoped
cc     that very few cells would turn out to be so designated, but I was
cc     disappointed to see that in fact roughly half the cells quickly
cc     become designated as unsmooth in typical problems in which shocks
cc     and contact discontinuities arise.  Therefore, it pays to process
cc     all cells, both smooth and unsmooth, together in vector mode on
cc     CPUs who compiler teams think vector processing will dominate in
cc     application codes (and who therefore neglect to write the more
cc     difficult quality scalar code compilers).
cc     This is why the smooth cell case is commented out in the loop
cc     above.
cc
cc
cc
cc
cc
c      if (icube .ge. 3-nghostcubes)   then
cc
cccccccccccccccccccccccccc  Fake memory write ccccccccccccccccccccccccc
c      do kcube = 1,nsugarcubes
c      do jcube = 1,nsugarcubes
c      mynucube = 1  +  (icube - nghostcubes - 1) * incx
c     &              +  (jcube - 1) * incy  +  (kcube - 1) * incz
c!DEC$ VECTOR ALWAYS
cc!DEC$ VECTOR ALIGNED
c      do k = 1,nsugarcubed
c         dddummy1(k,mynucube,1) = d(k,jcube,kcube,ic0)
c         dddummy1(k,mynucube,2) = d(k,jcube,kcube,ic0)
c         dddummy1(k,mynucube,3) = d(k,jcube,kcube,ic0)
c         dddummy1(k,mynucube,4) = d(k,jcube,kcube,ic0)
c         dddummy1(k,mynucube,5) = d(k,jcube,kcube,ic0)
c         dddummy1(k,mynucube,6) = d(k,jcube,kcube,ic0)
c         dddummy1(k,mynucube,7) = d(k,jcube,kcube,ic0)
c         dddummy1(k,mynucube,8) = d(k,jcube,kcube,ic0)
c      enddo
c      enddo
c      enddo
ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
cc
cc     The code for the third and all subsequent briquettes begins here.
cc     +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
cc
cc
cc
cc
ccc      do 6000   i = 4-nbdy,n+nbdy-2
cc      ibegin = 1-nx-2
cc      ibegin = max(ibegin,4-nx*(icube+nghostcubes))
cc      if (ibegin .le. -2)   then
cc      do 6000   i = ibegin,-2
c      do iii = 1, itercomp
c!DEC$ VECTOR ALWAYS
cc!DEC$ VECTOR ALIGNED
c      do jk = 1,nssq
c#if isviolent == 0
c      alsmth(jk,i16m02) =  rho(jk,i16m02)  -  .5 * dal(jk,i16m02)  -
c     &                sixth * (dasppm(jk,i16m02) - dasppm(jk,i16m03))
c      alsmth(jk,i16m03) =  rho(jk,i16m03)  -  .5 * dal(jk,i16m03)  -
c     &                sixth * (dasppm(jk,i16m03) - dasppm(jk,i16m04))
c      alsmth(jk,i16m04) =  rho(jk,i16m04)  -  .5 * dal(jk,i16m04)  -
c     &                sixth * (dasppm(jk,i16m04) - dasppm(jk,i16m05))
c      alsmth(jk,i16m05) =  rho(jk,i16m05)  -  .5 * dal(jk,i16m05)  -
c     &                sixth * (dasppm(jk,i16m05) - dasppm(jk,i16m06))
c#endif
c      alunsm(jk,i16m02) =  rho(jk,i16m02)  -  .5 * dal(jk,i16m02)  -
c     &                sixth * (damnot(jk,i16m02) - damnot(jk,i16m03))
c      alunsm(jk,i16m03) =  rho(jk,i16m03)  -  .5 * dal(jk,i16m03)  -
c     &                sixth * (damnot(jk,i16m03) - damnot(jk,i16m04))
c      alunsm(jk,i16m04) =  rho(jk,i16m04)  -  .5 * dal(jk,i16m04)  -
c     &                sixth * (damnot(jk,i16m04) - damnot(jk,i16m05))
c      alunsm(jk,i16m05) =  rho(jk,i16m05)  -  .5 * dal(jk,i16m05)  -
c     &                sixth * (damnot(jk,i16m05) - damnot(jk,i16m06))
c      enddo
cc6000  continue
cc
c      ninloop = nssq * 4
c      oop = ninloop
c#if isviolent
c      amults = amults + oop * 2.
c      adds = adds + oop * 3.
c#else
c      amults = amults + oop * 3.
c      adds = adds + oop * 5.
c#endif
cc
c      enddo
cc
cc      write (6,*) '(alsmth(1,i),i=1,nx) =',(alsmth(1,i),i=1,nx)
cc      write (6,*) '(alunsm(1,i),i=1,nx) =',(alunsm(1,i),i=1,nx)
cc
cc     In unsmooth zones reset  al  and  ar,  then apply
cc     monotonicity constraints to the implied interpolation
cc     parabolae.
cc
cc
cc
ccc      do 7000   i = 4-nbdy,n+nbdy-3
cc      ibegin = 1-nx-3
cc      ibegin = max(ibegin,4-nx*(icube+nghostcubes))
cc      if (ibegin .le. -3)   then
cc      do 7000   i = ibegin,-3
c      do iii = 1, itercomp
c!DEC$ VECTOR ALWAYS
cc!DEC$ VECTOR ALIGNED
c      do jk = 1,nssq
c      rhol(jk,i16m03) = alunsm(jk,i16m03)
c      rhor(jk,i16m03) = alunsm(jk,i16m02)
c      almon(jk) = 3. * rho(jk,i16m03)  -  2. * rhor(jk,i16m03)
c      armon(jk) = 3. * rho(jk,i16m03)  -  2. * rhol(jk,i16m03)
c      if (((rho(jk,i16m03) - rhol(jk,i16m03))
c     &   * (rho(jk,i16m03) - rhor(jk,i16m03))) .ge. 0.)  then
c         rhol(jk,i16m03) = rho(jk,i16m03)
c         rhor(jk,i16m03) = rho(jk,i16m03)
c         almon(jk) = rho(jk,i16m03)
c         armon(jk) = rho(jk,i16m03)
c      endif
c      if (((rhor(jk,i16m03) - rhol(jk,i16m03))
c     &   * (almon(jk) - rhol(jk,i16m03))) .gt. 0.)
c     &      rhol(jk,i16m03) = almon(jk)
c      if (((rhor(jk,i16m03) - rhol(jk,i16m03))
c     &   * (armon(jk) - rhor(jk,i16m03))) .lt. 0.)
c     &      rhor(jk,i16m03) = armon(jk)
c#if isviolent == 0
c      rhol(jk,i16m03) = alsmth(jk,i16m03)
c     &     -  unsmth(jk,i16m03) * (alsmth(jk,i16m03) - rhol(jk,i16m03))
c      rhor(jk,i16m03) = alsmth(jk,i16m02)
c     &     -  unsmth(jk,i16m03) * (alsmth(jk,i16m02) - rhor(jk,i16m03))
c#endif
c      drho(jk,i16m03) = rhor(jk,i16m03) - rhol(jk,i16m03)
c      rho6(jk,i16m03) = 6. * (rho(jk,i16m03)
c     &             -  .5 * (rhol(jk,i16m03) + rhor(jk,i16m03)))
cc
cc
c      rhol(jk,i16m04) = alunsm(jk,i16m04)
c      rhor(jk,i16m04) = alunsm(jk,i16m03)
c      almon(jk) = 3. * rho(jk,i16m04)  -  2. * rhor(jk,i16m04)
c      armon(jk) = 3. * rho(jk,i16m04)  -  2. * rhol(jk,i16m04)
c      if (((rho(jk,i16m04) - rhol(jk,i16m04))
c     &   * (rho(jk,i16m04) - rhor(jk,i16m04))) .ge. 0.)  then
c         rhol(jk,i16m04) = rho(jk,i16m04)
c         rhor(jk,i16m04) = rho(jk,i16m04)
c         almon(jk) = rho(jk,i16m04)
c         armon(jk) = rho(jk,i16m04)
c      endif
c      if (((rhor(jk,i16m04) - rhol(jk,i16m04))
c     &   * (almon(jk) - rhol(jk,i16m04))) .gt. 0.)
c     &      rhol(jk,i16m04) = almon(jk)
c      if (((rhor(jk,i16m04) - rhol(jk,i16m04))
c     &   * (armon(jk) - rhor(jk,i16m04))) .lt. 0.)
c     &      rhor(jk,i16m04) = armon(jk)
c#if isviolent == 0
c      rhol(jk,i16m04) = alsmth(jk,i16m04)
c     &     -  unsmth(jk,i16m04) * (alsmth(jk,i16m04) - rhol(jk,i16m04))
c      rhor(jk,i16m04) = alsmth(jk,i16m03)
c     &     -  unsmth(jk,i16m04) * (alsmth(jk,i16m03) - rhor(jk,i16m04))
c#endif
c      drho(jk,i16m04) = rhor(jk,i16m04) - rhol(jk,i16m04)
c      rho6(jk,i16m04) = 6. * (rho(jk,i16m04)
c     &             -  .5 * (rhol(jk,i16m04) + rhor(jk,i16m04)))
cc
cc
c      rhol(jk,i16m05) = alunsm(jk,i16m05)
c      rhor(jk,i16m05) = alunsm(jk,i16m04)
c      almon(jk) = 3. * rho(jk,i16m05)  -  2. * rhor(jk,i16m05)
c      armon(jk) = 3. * rho(jk,i16m05)  -  2. * rhol(jk,i16m05)
c      if (((rho(jk,i16m05) - rhol(jk,i16m05))
c     &   * (rho(jk,i16m05) - rhor(jk,i16m05))) .ge. 0.)  then
c         rhol(jk,i16m05) = rho(jk,i16m05)
c         rhor(jk,i16m05) = rho(jk,i16m05)
c         almon(jk) = rho(jk,i16m05)
c         armon(jk) = rho(jk,i16m05)
c      endif
c      if (((rhor(jk,i16m05) - rhol(jk,i16m05))
c     &   * (almon(jk) - rhol(jk,i16m05))) .gt. 0.)
c     &      rhol(jk,i16m05) = almon(jk)
c      if (((rhor(jk,i16m05) - rhol(jk,i16m05))
c     &   * (armon(jk) - rhor(jk,i16m05))) .lt. 0.)
c     &      rhor(jk,i16m05) = armon(jk)
c#if isviolent == 0
c      rhol(jk,i16m05) = alsmth(jk,i16m05)
c     &     -  unsmth(jk,i16m05) * (alsmth(jk,i16m05) - rhol(jk,i16m05))
c      rhor(jk,i16m05) = alsmth(jk,i16m04)
c     &     -  unsmth(jk,i16m05) * (alsmth(jk,i16m04) - rhor(jk,i16m05))
c#endif
c      drho(jk,i16m05) = rhor(jk,i16m05) - rhol(jk,i16m05)
c      rho6(jk,i16m05) = 6. * (rho(jk,i16m05)
c     &             -  .5 * (rhol(jk,i16m05) + rhor(jk,i16m05)))
cc
cc
c      rhol(jk,i16m06) = alunsm(jk,i16m06)
c      rhor(jk,i16m06) = alunsm(jk,i16m05)
c      almon(jk) = 3. * rho(jk,i16m06)  -  2. * rhor(jk,i16m06)
c      armon(jk) = 3. * rho(jk,i16m06)  -  2. * rhol(jk,i16m06)
c      if (((rho(jk,i16m06) - rhol(jk,i16m06))
c     &   * (rho(jk,i16m06) - rhor(jk,i16m06))) .ge. 0.)  then
c         rhol(jk,i16m06) = rho(jk,i16m06)
c         rhor(jk,i16m06) = rho(jk,i16m06)
c         almon(jk) = rho(jk,i16m06)
c         armon(jk) = rho(jk,i16m06)
c      endif
c      if (((rhor(jk,i16m06) - rhol(jk,i16m06))
c     &   * (almon(jk) - rhol(jk,i16m06))) .gt. 0.)
c     &      rhol(jk,i16m06) = almon(jk)
c      if (((rhor(jk,i16m06) - rhol(jk,i16m06))
c     &   * (armon(jk) - rhor(jk,i16m06))) .lt. 0.)
c     &      rhor(jk,i16m06) = armon(jk)
c#if isviolent == 0
c      rhol(jk,i16m06) = alsmth(jk,i16m06)
c     &     -  unsmth(jk,i16m06) * (alsmth(jk,i16m06) - rhol(jk,i16m06))
c      rhor(jk,i16m06) = alsmth(jk,i16m05)
c     &     -  unsmth(jk,i16m06) * (alsmth(jk,i16m05) - rhor(jk,i16m06))
c#endif
c      drho(jk,i16m06) = rhor(jk,i16m06) - rhol(jk,i16m06)
c      rho6(jk,i16m06) = 6. * (rho(jk,i16m06)
c     &             -  .5 * (rhol(jk,i16m06) + rhor(jk,i16m06)))
c      enddo
cc7000  continue
cc
c      ninloop = nssq * 4
c      oop = ninloop
c      cvmgms = cvmgms + oop * 6.
c#if isviolent
c      amults = amults + oop * 8.
c      adds = adds + oop * 11.
c#else
c      amults = amults + oop * 10.
c      adds = adds + oop * 15.
c#endif
cc
c      enddo
cc      endif
cc
cc      endif
cc
cc      endif
cc
cc
c#if istrace
c!DEC$ VECTOR ALWAYS
c      do j = 1,nssq
c      rholdbg(j,iplnm3) = rhol(j,i16m03)
c      rholdbg(j,iplnm4) = rhol(j,i16m04)
c      rholdbg(j,iplnm5) = rhol(j,i16m05)
c      rholdbg(j,iplnm6) = rhol(j,i16m06)
cc
c      rhordbg(j,iplnm3) = rhor(j,i16m03)
c      rhordbg(j,iplnm4) = rhor(j,i16m04)
c      rhordbg(j,iplnm5) = rhor(j,i16m05)
c      rhordbg(j,iplnm6) = rhor(j,i16m06)
cc
c      drhodbg(j,iplnm3) = drho(j,i16m03)
c      drhodbg(j,iplnm4) = drho(j,i16m04)
c      drhodbg(j,iplnm5) = drho(j,i16m05)
c      drhodbg(j,iplnm6) = drho(j,i16m06)
cc
c      rho6dbg(j,iplnm3) = rho6(j,i16m03)
c      rho6dbg(j,iplnm4) = rho6(j,i16m04)
c      rho6dbg(j,iplnm5) = rho6(j,i16m05)
c      rho6dbg(j,iplnm6) = rho6(j,i16m06)
c      enddo
c#endif
cc
cc
cc
cc     uxavl is the time-centered interface velocity, usually taken from
cc     the solution of a Riemann problem.  It provides us with a linearly
cc     interpolated view of the time-centered velocity across each grid
cc     cell.  This allows us to solve for the fraction, sigmal, of the
cc     upstream cell that crosses the interface L during the time step.
cc     The average velocity along the path that just reaches the inter-
cc     face at the end of the time step is   ul - .5*sigmal*dull   or
cc     ul  +  .5*sigmal*dulr,     where  sigmal  is a positive definite
cc     quantity and  dull  and  dulr  are the differences
cc     uxavl - uxavlzl     and     uxavr - uxavl.     Thus we get sigmal
cc     from     sigmal  =  (dt/dx) * (ul - .5*sigmal*dull)
cc              sigmal * (1 + .5*dt*dull/dx)  =  ul*dt/dx
cc     or       sigmal * (1 + .5*dt*dulr/dx)  =  -ul*dt/dx
cc     Thus     sigmal  =  abs(ul) / ((dx/dt) + .5*dul)
cc
cc
cc
ccc      do 3500   i = -nbdy+4,n+nbdy-3
cc      ibegin = 1-nx-3
cc      ibegin = max(ibegin,4-nx*(icube+nghostcubes))
cc      if (ibegin .le. -3)   then
cc      do 3500   i = ibegin,-3
c      do iii = 1, itercomp
c!DEC$ VECTOR ALWAYS
cc!DEC$ VECTOR ALIGNED
c      do jk = 1,nssq
c      duxavl(jk,i16m03) = uxavl(jk,i16m02) - uxavl(jk,i16m03)
c      duxavl(jk,i16m04) = uxavl(jk,i16m03) - uxavl(jk,i16m04)
c      duxavl(jk,i16m05) = uxavl(jk,i16m04) - uxavl(jk,i16m05)
c      duxavl(jk,i16m06) = uxavl(jk,i16m05) - uxavl(jk,i16m06)
c      enddo
cc3500  continue
cc
c      ninloop = nssq * 4
c      oop = ninloop
c      adds = adds + oop
cc
c      enddo
cc
cc
ccc      do 3600   i = -nbdy+5,n+nbdy-3
cc      ibegin = 1-nx-3
cc      ibegin = max(ibegin,5-nx*(icube+nghostcubes))
cc      if (ibegin .le. -3)   then
cc      do 3600   i = ibegin,-3
c      do iii = 1, itercomp
c!DEC$ VECTOR ALWAYS
cc!DEC$ VECTOR ALIGNED
c      do jk = 1,nssq
c      thngy01(jk) = duxavl(jk,i16m04)
c      if (uxavl(jk,i16m03) .lt. 0.)   thngy01(jk) = duxavl(jk,i16m03)
c      sigmal(jk,i16m03) = abs(uxavl(jk,i16m03))
c     &                 / (dxbydt  +  .5 * thngy01(jk))
cc
c      thngy01(jk) = duxavl(jk,i16m05)
c      if (uxavl(jk,i16m04) .lt. 0.)   thngy01(jk) = duxavl(jk,i16m04)
c      sigmal(jk,i16m04) = abs(uxavl(jk,i16m04))
c     &                 / (dxbydt  +  .5 * thngy01(jk))
cc
c      thngy01(jk) = duxavl(jk,i16m06)
c      if (uxavl(jk,i16m05) .lt. 0.)   thngy01(jk) = duxavl(jk,i16m05)
c      sigmal(jk,i16m05) = abs(uxavl(jk,i16m05))
c     &                 / (dxbydt  +  .5 * thngy01(jk))
cc
c      thngy01(jk) = duxavl(jk,i16m07)
c      if (uxavl(jk,i16m06) .lt. 0.)   thngy01(jk) = duxavl(jk,i16m06)
c      sigmal(jk,i16m06) = abs(uxavl(jk,i16m06))
c     &                 / (dxbydt  +  .5 * thngy01(jk))
c      enddo
cc3600  continue
cc
c      ninloop = nssq * 4
c      oop = ninloop
c      recips = recips + oop
c      cvmgms = cvmgms + oop * 2.
c      amults = amults + oop * 2.
c      adds = adds + oop
cc
c      enddo
cc
c#if istrace
c!DEC$ VECTOR ALWAYS
c      do j = 1,nssq
c      duxavldbg(j,iplnm3) = duxavl(j,i16m03)
c      duxavldbg(j,iplnm4) = duxavl(j,i16m04)
c      duxavldbg(j,iplnm5) = duxavl(j,i16m05)
c      duxavldbg(j,iplnm6) = duxavl(j,i16m06)
cc
c      sigmaldbg(j,iplnm3) = sigmal(j,i16m03)
c      sigmaldbg(j,iplnm4) = sigmal(j,i16m04)
c      sigmaldbg(j,iplnm5) = sigmal(j,i16m05)
c      sigmaldbg(j,iplnm6) = sigmal(j,i16m06)
c      enddo
c#endif
cc
cc
cc
c#if isPPMvio
cc      write (6,*) 'about to do loop 3700 in PPMMF.'
cc      if (mbrick .eq. 4)
c      if (ifdebug .gt. 0)
c     &   write (6,*) 'about to do loop 3700 in PPMMF.'
c#endif
cc
ccc      do 3700   i = 5-nbdy,n+nbdy-3
cc      ibegin = 1-nx-3
cc      ibegin = max(ibegin,5-nx*(icube+nghostcubes))
cc      do 3700   i = ibegin,-3
c      do iii = 1, itercomp
c!DEC$ VECTOR ALWAYS
cc!DEC$ VECTOR ALIGNED
c      do jk = 1,nssq
c      xfr0 = .5 * sigmal(jk,i16m03)
c      xfr01 = 1.  -  forthd * xfr0
c      thingl(jk) = rhor(jk,i16m04)
c      if (uxavl(jk,i16m03) .lt. 0.)   thingl(jk) = rhol(jk,i16m03)
c      dthing(jk) = - drho(jk,i16m04)
c      if (uxavl(jk,i16m03) .lt. 0.)   dthing(jk) = drho(jk,i16m03)
c      thing6(jk) = rho6(jk,i16m04)
c      if (uxavl(jk,i16m03) .lt. 0.)   thing6(jk) = rho6(jk,i16m03)
c      rhomlr0 = thingl(jk)  + xfr0 * (dthing(jk)  + xfr01 * thing6(jk))
c      dvoll(jk,i16m03) = sigmal(jk,i16m03) * deex
c      if (uxavl(jk,i16m03) .lt. 0.)
c     &                     dvoll(jk,i16m03) = - dvoll(jk,i16m03)
c      dmassl(jk,i16m03) = rhomlr0 * dvoll(jk,i16m03)
cc
c      xfr0 = .5 * sigmal(jk,i16m04)
c      xfr01 = 1.  -  forthd * xfr0
c      thingl(jk) = rhor(jk,i16m05)
c      if (uxavl(jk,i16m04) .lt. 0.)   thingl(jk) = rhol(jk,i16m04)
c      dthing(jk) = - drho(jk,i16m05)
c      if (uxavl(jk,i16m04) .lt. 0.)   dthing(jk) = drho(jk,i16m04)
c      thing6(jk) = rho6(jk,i16m05)
c      if (uxavl(jk,i16m04) .lt. 0.)   thing6(jk) = rho6(jk,i16m04)
c      rhomlr0 = thingl(jk)  + xfr0 * (dthing(jk)  + xfr01 * thing6(jk))
c      dvoll(jk,i16m04) = sigmal(jk,i16m04) * deex
c      if (uxavl(jk,i16m04) .lt. 0.)
c     &                     dvoll(jk,i16m04) = - dvoll(jk,i16m04)
c      dmassl(jk,i16m04) = rhomlr0 * dvoll(jk,i16m04)
cc
c      xfr0 = .5 * sigmal(jk,i16m05)
c      xfr01 = 1.  -  forthd * xfr0
c      thingl(jk) = rhor(jk,i16m06)
c      if (uxavl(jk,i16m05) .lt. 0.)   thingl(jk) = rhol(jk,i16m05)
c      dthing(jk) = - drho(jk,i16m06)
c      if (uxavl(jk,i16m05) .lt. 0.)   dthing(jk) = drho(jk,i16m05)
c      thing6(jk) = rho6(jk,i16m06)
c      if (uxavl(jk,i16m05) .lt. 0.)   thing6(jk) = rho6(jk,i16m05)
c      rhomlr0 = thingl(jk)  + xfr0 * (dthing(jk)  + xfr01 * thing6(jk))
c      dvoll(jk,i16m05) = sigmal(jk,i16m05) * deex
c      if (uxavl(jk,i16m05) .lt. 0.)
c     &                     dvoll(jk,i16m05) = - dvoll(jk,i16m05)
c      dmassl(jk,i16m05) = rhomlr0 * dvoll(jk,i16m05)
cc
c      xfr0 = .5 * sigmal(jk,i16m06)
c      xfr01 = 1.  -  forthd * xfr0
c      thingl(jk) = rhor(jk,i16m07)
c      if (uxavl(jk,i16m06) .lt. 0.)   thingl(jk) = rhol(jk,i16m06)
c      dthing(jk) = - drho(jk,i16m07)
c      if (uxavl(jk,i16m06) .lt. 0.)   dthing(jk) = drho(jk,i16m06)
c      thing6(jk) = rho6(jk,i16m07)
c      if (uxavl(jk,i16m06) .lt. 0.)   thing6(jk) = rho6(jk,i16m06)
c      rhomlr0 = thingl(jk)  + xfr0 * (dthing(jk)  + xfr01 * thing6(jk))
c      dvoll(jk,i16m06) = sigmal(jk,i16m06) * deex
c      if (uxavl(jk,i16m06) .lt. 0.)
c     &                     dvoll(jk,i16m06) = - dvoll(jk,i16m06)
c      dmassl(jk,i16m06) = rhomlr0 * dvoll(jk,i16m06)
c      enddo
cc3700  continue
cc
c      ninloop = nssq * 4
c      cvmgms = cvmgms + oop * 3.
c      amults = amults + oop * 6.
c      adds = adds + oop * 3.
cc
c      enddo
cc
c#if istrace
c!DEC$ VECTOR ALWAYS
c      do j = 1,nssq
c      dvolldbg(j,iplnm3) = dvoll(j,i16m03)
c      dvolldbg(j,iplnm4) = dvoll(j,i16m04)
c      dvolldbg(j,iplnm5) = dvoll(j,i16m05)
c      dvolldbg(j,iplnm6) = dvoll(j,i16m06)
cc
c      dmassldbg(j,iplnm3) = dmassl(j,i16m03)
c      dmassldbg(j,iplnm4) = dmassl(j,i16m04)
c      dmassldbg(j,iplnm5) = dmassl(j,i16m05)
c      dmassldbg(j,iplnm6) = dmassl(j,i16m06)
c      enddo
c#endif
cc
cc
cc
cc     ********************************************************
cc     Apply conservation law to obtain new zone-averaged
cc     values of density.
cc     Because our density is actually a concentration of a
cc     contaminant, we must divide its new mass by that of the
cc     fluid as a whole below.
cc     ********************************************************
cc
cc
ccc      do 8000   i = -nbdy+5,n+nbdy-4
cc      ibegin = 1-nx-4
cc      ibegin = max(ibegin,5-nx*(icube+nghostcubes))
cc      if (ibegin .le. -4)   then
cc      do 8000   i = ibegin,-4
c      do iii = 1, itercomp
c!DEC$ VECTOR ALWAYS
cc!DEC$ VECTOR ALIGNED
c      do jk = 1,nssq
c      dmnu = dm(jk,i16m04) + dmassl(jk,i16m04) - dmassl(jk,i16m03)
c      rhonu(jk,i16m04) = dmnu / (deex + dvoll(jk,i16m04)
c     &                               - dvoll(jk,i16m03)) 
cc
c      dmnu = dm(jk,i16m05) + dmassl(jk,i16m05) - dmassl(jk,i16m04)
c      rhonu(jk,i16m05) = dmnu / (deex + dvoll(jk,i16m05)
c     &                               - dvoll(jk,i16m04)) 
cc
c      dmnu = dm(jk,i16m06) + dmassl(jk,i16m06) - dmassl(jk,i16m05)
c      rhonu(jk,i16m06) = dmnu / (deex + dvoll(jk,i16m06)
c     &                               - dvoll(jk,i16m05)) 
cc
c      dmnu = dm(jk,i16m07) + dmassl(jk,i16m07) - dmassl(jk,i16m06)
c      rhonu(jk,i16m07) = dmnu / (deex + dvoll(jk,i16m07)
c     &                               - dvoll(jk,i16m06)) 
c      enddo
cc8000  continue
cc
c      ninloop = nssq * 4
c      oop = ninloop
c      amults = amults + oop
c      adds = adds + oop * 2.
cc
c      enddo
cc
c#if istrace
c!DEC$ VECTOR ALWAYS
c      do j = 1,nssq
c      rhonudbg(j,iplnm4) = rhonu(j,i16m04)
c      rhonudbg(j,iplnm5) = rhonu(j,i16m05)
c      rhonudbg(j,iplnm6) = rhonu(j,i16m06)
c      rhonudbg(j,iplnm7) = rhonu(j,i16m07)
c      enddo
c#endif
cc
cc
cc     Reset maximum Courant number, if appropriate.
cc     By the way, the Intel 5.0 Fortran vectorizer actually vectorizes
cc     this 9200-loop!
cc
c#if isPPMio
c      write (6,*) 'about to do loop 9200 in PPMMF.'
c      if (ifdebug .gt. 0)
c     &   write (6,*) 'about to do loop 9200 in PPMMF.'
c#endif
cc
ccc      do 9200   i = 1,nx
cc      ibegin = 1-nx-4
cc      ibegin = max(ibegin,5-nx*(icube+nghostcubes))
cc      do 9200   i = ibegin,-4
c      do iii = 1, itercomp
ccdir$ ivdep
c!DEC$ VECTOR ALWAYS
cc!DEC$ VECTOR ALIGNED
c      do jk = 1,nssq
c      thngy01(jk) = max (courno(jk,i16m04), courno(jk,i16m05))
c      thngy01(jk) = max (thngy01(jk), courno(jk,i16m06))
c      thngy01(jk) = max (thngy01(jk), courno(jk,i16m07))
c      enddo
cc9200  continue
cc
c      do jk = 1,nssq
c      courmx = max (courmx, thngy01(jk))
c      enddo
cc
c      ninloop = nssq * 4
c      oop = ninloop
c      cvmgms = cvmgms + oop
c      adds = adds + oop
cc
c      enddo
cc      endif
ccc                This corresponds to the 8000-loop's "if"
cc      endif
ccc                This corresponds to the 3600-loop's "if"
cc      endif
ccc                This corresponds to the 3500-loop's "if"
cc      endif
ccc                This corresponds to the 3400-loop's "if"
cc
cc
cc
cc
cc     ==================================================================
cc
cc
cc
cc
cc
cc     PREPARE TO WRITE BACK RESULTS TO MAIN MEMORY
cc     BY PLACING THEM INTO SUGAR-CUBE DATA STRUCTURES FOR BLOCKED
cc     WRITE-BACK.
cc
cc
c      if (ifvis .gt. 0)   then
cc
cc
cc
cc
cc     NOW, IF IT IS REQUESTED BY THE CALLING ROUTINE, GENERATE EITHER
cc     BRICK OF BYTES DATA FOR IMAGE RENDERING OR COMPRESSED DUMP DATA
cc     FOR ARCHIVING THE RESULTS OF THE RUN.
cc
cc     ******************************************************************
cc     WE WILL ASSUME THAT NX, NY, AND NZ ARE ALL INTEGER MULTIPLES OF 4.
cc     ******************************************************************
cc
cc
cc
c#if isPPMio
c      write (6,*) 'I am about to get the voxels.'
c#endif
cc
cc      character*1   voxels(nxx*nyy*nzz,(8+9*nfluids),0:1)
cc      character*1   voxelcube(nxx*nyy*nzz*(8+9*nfluids),0:1)
cc      equivalence  (voxelcube,voxels)
cc
cc              1     2   3   4   5   6   7   8
cc     vars:  vort, divu, S, rho, p, ux, uy, uz
cc
cc
c         call CellVis (rho,voxlrho,
c     &                 thing01,thing02,
c     &                 smallu,GammaAir,ArgsImg,
c     &             adds,amults,recips,cvmgms,sqrts,rsqrts,exps,
c     &             ifdebug,time,myrank,mythread,mbrick,
c     &             iold,icube,jbq,kbq,ipass,
c     &             MyBrickX,MyBrickY,MyBrickZ,
c     &             NXBricks,NYBricks,NZBricks,
c     &             i16m07,i16m06,i16m05,i16m04)
cc
c#if isPPMio
c      write (6,*) 'I got the voxels.'
c#endif
cc
cc
cc     Output arrays from CellVis:
cc
cc      character*1   voxlvort(nxx,nyy,nzz),   voxldivu(nxx,nyy,nzz)
cc      character*1      voxls(nxx,nyy,nzz),    voxlrho(nxx,nyy,nzz)
cc      character*1      voxlp(nxx,nyy,nzz),     voxlux(nxx,nyy,nzz)
cc      character*1     voxluy(nxx,nyy,nzz),     voxluz(nxx,nyy,nzz)
cc      equivalence - (voxlvort,voxels(1,1)),  (voxldivu,voxels(1,2))
cc      equivalence     (voxls,voxels(1,3)),   (voxlrho,voxels(1,4))
cc      equivalence     (voxlp,voxels(1,5)),    (voxlux,voxels(1,6))
cc      equivalence    (voxluy,voxels(1,7)),    (voxluz,voxels(1,8))
cc
cc
cc      character*1   vvoxels(nxx*nyy*nzz*(8+9*nfluids),
cc     &                      (nbqx/nsugarcubes)*(nbqy/nsugarcubes)
cc     &                                        *(nbqz/nsugarcubes))
cc
cc      character*1   voxels(nxx*nyy*nzz,(8+9*nfluids),0:1)
cc      character*1   voxelcube(nxx*nyy*nzz*(8+9*nfluids),0:1)
cc      equivalence  (voxelcube,voxels)
cc
cc              1     2   3   4   5   6   7   8
cc     vars:  vort, divu, S, rho, p, ux, uy, uz
cc
cc
cc
cc
c      ivcube = (icube - nghostcubes + nsugarcubes - 1) / nsugarcubes
c      icremain = icube - nghostcubes  -  ivcube * nsugarcubes
c      if (icremain .eq. 0)   then
c      mynuvcube = 1  +  (ivcube - 1) * incvx
c#if isPPMio
c      write (6,*) 'I will write my voxels to mynuvcube =',mynuvcube
c      write (6,*) 'icube, ivcube, ipass =',icube,ivcube,ipass
c#endif
cc
cc!DEC$ VECTOR ALWAYS
cc!DEC$ VECTOR ALIGNED
c         do k = 1,nxx*nyy*nzz
c            vvoxels(k,mynuvcube) = voxelcube(k,0)
c         enddo
cc
c#if isPPMio
c      write (6,*) 'I wrote back my voxels.'
cc
cc      write (6,*)
cc      write (6,*)   'mynucube =',mynucube
cc      do i = 1,nsugar
cc      write (6,*) 'RHO in Z-Y plane for  i =',i,'     from DDNU'
cc      do j = nsugar,1,-1
cc      ioff = 3*nx*ny*nz + i + (j-1)*nsugar
cc      write (6,501) (iachar(vvoxels(ioff+nsugarsq*(k-1),icube)),k=1,8)
cc      enddo
cc      enddo
cc501   format (8i5)
cc      write (6,*)
c#endif
c      endif
cc
cc
cc
c      endif
cc                    This endif is for the test on ifvis.
cc
cc
cc     NOW WRITE BACK RESULTS OF THE COMPUTATION TO MAIN MEMORY.
cc
cc
cc     The order of the variables in  d  is as follows:
cc
cc       1   2   3   4   5
cc      rho, p, ux, uy, uz
cc
cc
cc
cc
cc
c      if ((ipass .eq. 3) .or. (ipass .eq. 6))   then
cc
cc
c      jb = - ny * nz
c      do kcube = 1,nsugarcubes
c      do jcube = 1,nsugarcubes
c      jb = jb  +  ny * nz
cc      jki = 0
ccc      do i = 1,nx
cc      do i = ibegin,-nghostcells
ccDIR$ IVDEP
cc!DIR$ SIMD 
cc!DEC$ VECTOR ALIGNED
c         do j = 1,ny*nz
cc         jki = jki + 1
c         jj = jb + j
c         jki = 3 * ny*nz + j
c         dnuc(jki,1,jcube,kcube) = rhonu(jj,i16m04)
cc
c         jki = jki - ny*nz
c         dnuc(jki,1,jcube,kcube) = rhonu(jj,i16m05)
cc
c         jki = jki - ny*nz
c         dnuc(jki,1,jcube,kcube) = rhonu(jj,i16m06)
cc
c         jki = jki - ny*nz
c         dnuc(jki,1,jcube,kcube) = rhonu(jj,i16m07)
c         enddo
cc      enddo
c      enddo
c      enddo
cc
cc     I will write back  dnu  into  ddnu  in the same way regardless of
cc     which 1-D pass I am on.
cc
c      elseif ((ipass .eq. 1) .or. (ipass .eq. 2))   then
cc
cc
cc     The order of the variables in  d  is as follows:
cc
cc       1   2   3   4   5
cc      rho, p, ux, uy, uz
cc
cc
c      jkb = - ny * nz
c      do kcube = 1,nsugarcubes
c      do jcube = 1,nsugarcubes
c      jkb = jkb  +  ny * nz
ccc      do i = 1,nx
cc      do i = ibegin,-nghostcells
c         jk = 0
c         do k = 1,nz
c         kijb = k  -  nx*nz
c!DEC$ NOVECTOR
c         do j = 1,ny
c         jk = jk + 1
c         jjk = jkb + jk
c         kijb = kijb + nx*nz
c         kij = kijb  +  3 * nz
c         dnuc(kij,1,jcube,kcube) = rhonu(jjk,i16m04)
cc
c         kij = kij - nz
c         dnuc(kij,1,jcube,kcube) = rhonu(jjk,i16m05)
cc
c         kij = kij - nz
c         dnuc(kij,1,jcube,kcube) = rhonu(jjk,i16m06)
cc
c         kij = kij - nz
c         dnuc(kij,1,jcube,kcube) = rhonu(jjk,i16m07)
c         enddo
c         enddo
cc      enddo
c      enddo
c      enddo
cc
c#if isPPMvio
cc      write (6,*)
cc      write (6,*) 'RHONU from within Unify:'
cc      do i = 1,8
cc      do jko = 0,7
cc      jkoff = jko * 8
cc      write (6,101)  (rhonu(jkoff+jk,ii(i)),jk=1,8)
cc      enddo
cc      write (6,*)
cc      enddo
cc      write (6,*)
cc      write (6,*) 'RHONU from DNU within Unify:'
cc      do i = 1,8
cc      ioff = (i-1) * 64
cc      do jko = 0,7
cc      jkoff = jko * 8  +  ioff
cc      write (6,101)  (dnu(jkoff+jk,1),jk=1,8)
cc      enddo
cc      write (6,*)
cc      enddo
cc101   format (1p8e11.3)
c#endif
cc
c      else
cc
cc     when  iflastzpass = 1,
cc        present x  =  orig z  --->  new y
cc        present y  =  orig x  --->  new z
cc        present z  =  orig y  --->  new x
cc     when  iflastypass = 1,
cc        present x  =  orig y  --->  new y
cc        present y  =  orig z  --->  new z
cc        present z  =  orig x  --->  new x
cc     so that in either case,
cc
cc     present  j = iy + (iz-1)*nsugar
cc     new  j = i65 + (iy-1)*nsugar
cc
cc
cc     The order of the variables in  d  is as follows:
cc
cc       1   2   3   4   5
cc      rho, p, ux, uy, uz
cc
cc
cc
c      jb = - ny * nz
c      do kcube = 1,nsugarcubes
c      do jcube = 1,nsugarcubes
c      jb = jb  +  ny * nz
ccc      do i = 1,nx
cc      do i = ibegin,-nghostcells
c         ijkb = 1 - nx
c!DEC$ NOVECTOR
c         do j = 1,ny*nz
c         jj = jb + j
c         ijkb = ijkb + nx
c         ijk = ijkb + 3
c         dnuc(ijk,1,jcube,kcube) = rhonu(jj,i16m04)
cc
c         ijk = ijk - 1
c         dnuc(ijk,1,jcube,kcube) = rhonu(jj,i16m05)
cc
c         ijk = ijk - 1
c         dnuc(ijk,1,jcube,kcube) = rhonu(jj,i16m06)
cc
c         ijk = ijk - 1
c         dnuc(ijk,1,jcube,kcube) = rhonu(jj,i16m07)
c         enddo
cc      enddo
c      enddo
c      enddo
cc
cc
cc     present Z  --->  new X,   present Y  --->  new Z
cc                               present X  --->  new Y
cc
cc
c      endif
cc
cc
cc
cc
cc     Write back the new sugar cube.
cc
cc      dimension       dnu(ninsugarcube,nsugarcubes,nsugarcubes,0:1)
cc
cc     ddnu  as provided to this routine points to the briquette for
cc           jbq, kbq, and for  ibq = 1.   We can navigate inside  ddnu
cc           via the briquette increments, incx, incy, and incz, that
cc           are provided as arguments to this call.  We may have to
cc           navigate backwards in  X,  and that will be OK.
cc
cc
c#if isPPMio
c      write (6,*) 'about to write back DNU for icube =',icube
c#endif
cc
cccccccccccccccccccccccccc  Fake memory write ccccccccccccccccccccccccc
c      do kcube = 1,nsugarcubes
c      do jcube = 1,nsugarcubes
c      mynucube = 1  +  (icube - nghostcubes - 1) * incx
c     &              +  (jcube - 1) * incy  +  (kcube - 1) * incz
c!DEC$ VECTOR ALWAYS
cc!DEC$ VECTOR ALIGNED
c      do k = 1,nsugarcubed
c         dddummy1(k,mynucube,1) = d(k,jcube,kcube,1)
c         dddummy1(k,mynucube,2) = d(k,jcube,kcube,1)
c         dddummy1(k,mynucube,3) = d(k,jcube,kcube,1)
c         dddummy1(k,mynucube,4) = d(k,jcube,kcube,1)
c         dddummy1(k,mynucube,5) = d(k,jcube,kcube,1)
c         dddummy1(k,mynucube,6) = d(k,jcube,kcube,1)
c         dddummy1(k,mynucube,7) = d(k,jcube,kcube,1)
c         dddummy1(k,mynucube,8) = d(k,jcube,kcube,1)
c      enddo
c      enddo
c      enddo
ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
cc
c      do kcube = 1,nsugarcubes
c      do jcube = 1,nsugarcubes
c      mynucube = 1  +  (icube - nghostcubes - 1) * incx
c     &              +  (jcube - 1) * incy  +  (kcube - 1) * incz
c!DEC$ VECTOR ALWAYS
cc!DEC$ VECTOR ALIGNED
c      do k = 1,nsugarcubed
c         ddnu(k,mynucube) = dnu(k,jcube,kcube)
c      enddo
c      enddo
c      enddo
cc
cc     7/25/11:
cc     At this point, we want to evict the cache lines holding the values
cc     of DDNU from the cache.  The OpenMP flush will do this, but
cc     it will screw up all the other executing threads, because its
cc     granularity is not fine enough.  The intrinsic, mm_clflush, call-
cc     able only from C, will evict just one cache line from our cache.
cc     This is precisely what we want.  Otherwise our cache will fill up
cc     with useless junk, preventing us from making efficient use of it.
cc
cc
c      if (incxsend .eq. 0)   then
cc      lbrick = ibrick*ifxpass + jbrick*ifypass + kbrick*ifzpass
c      if (((icube-nghostcubes .eq. 1) .and. (lbrick .eq. 1)) .or.
c     &    ((icube-nghostcubes .eq. nbqx) .and. (lbrick .eq. 2)))   then
c         do kcube = 1,nsugarcubes
c         do jcube = 1,nsugarcubes
c         mynucube = 1  +  (jcube - 1) * incy  +  (kcube - 1) * incz
c!DEC$ VECTOR ALWAYS
cc!DEC$ VECTOR ALIGNED
c         do k = 1,nsugarcubed
c            ddnusend(k,mynucube) = dnu(k,jcube,kcube)
c         enddo
c         enddo
c         enddo
c      endif
c      elseif (incxsend .gt. 0)   then
c         do kcube = 1,nsugarcubes
c         do jcube = 1,nsugarcubes
c         mynucube = 1  +  (icube - nghostcubes - 1) * incxsend
c     &                 +  (jcube - 1) * incy  +  (kcube - 1) * incz
c!DEC$ VECTOR ALWAYS
cc!DEC$ VECTOR ALIGNED
c         do k = 1,nsugarcubed
c            ddnusend(k,mynucube) = dnu(k,jcube,kcube)
c         enddo
c         enddo
c         enddo
c      endif
c#if isPPMio
c      write (6,*) 'wrote back DNU for icube =',icube
cc
cc      if (ifdebug .gt. 0)   write (6,*) 'I wrote back DNU.'
cc
cc      write (6,*)
cc      write (6,*)   'mynucube =',mynucube
cc      do i = 1,nsugar
cc      write (6,*) 'RHO in Z-Y plane for  i =',i,'     from DDNU'
cc      do j = nsugar,1,-1
cc      write (6,201)  (ddnu(k+nsugar*(i-1+(j-1)*nsugar),mynucube),k=1,8)
cc      enddo
cc      enddo
cc201   format (1p8e11.3)
cc      write (6,*)
c#endif
cc
cc
cc
cc
cc     ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
cc     The code for the third and all subsequent briquettes ends here.
cc
c      else
cc
cc     The code for just the second briquette begins here.
cc     ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
cc
cc
cc
ccc      do 6000   i = 4-nbdy,n+nbdy-2
cc      do 6000   i = ibegin,-2
c      do iii = 1, itercomp
c!DEC$ VECTOR ALWAYS
cc!DEC$ VECTOR ALIGNED
c      do jk = 1,nssq
c#if isviolent == 0
c      alsmth(jk,i16m02) =  rho(jk,i16m02)  -  .5 * dal(jk,i16m02)  -
c     &                sixth * (dasppm(jk,i16m02) - dasppm(jk,i16m03))
c      alsmth(jk,i16m03) =  rho(jk,i16m03)  -  .5 * dal(jk,i16m03)  -
c     &                sixth * (dasppm(jk,i16m03) - dasppm(jk,i16m04))
c      alsmth(jk,i16m04) =  rho(jk,i16m04)  -  .5 * dal(jk,i16m04)  -
c     &                sixth * (dasppm(jk,i16m04) - dasppm(jk,i16m05))
c#endif
c      alunsm(jk,i16m02) =  rho(jk,i16m02)  -  .5 * dal(jk,i16m02)  -
c     &                sixth * (damnot(jk,i16m02) - damnot(jk,i16m03))
c      alunsm(jk,i16m03) =  rho(jk,i16m03)  -  .5 * dal(jk,i16m03)  -
c     &                sixth * (damnot(jk,i16m03) - damnot(jk,i16m04))
c      alunsm(jk,i16m04) =  rho(jk,i16m04)  -  .5 * dal(jk,i16m04)  -
c     &                sixth * (damnot(jk,i16m04) - damnot(jk,i16m05))
c      enddo
cc6000  continue
cc
c      ninloop = nssq * 3
c      oop = ninloop
c#if isviolent
c      amults = amults + oop * 2.
c      adds = adds + oop * 3.
c#else
c      amults = amults + oop * 3.
c      adds = adds + oop * 5.
c#endif
cc
c      enddo
cc
cc
cc
ccc      do 7000   i = 4-nbdy,n+nbdy-3
cc      ibegin = 1-nx-3
cc      ibegin = max(ibegin,4-nx*(icube+nghostcubes))
cc      if (ibegin .le. -3)   then
cc      do 7000   i = ibegin,-3
c      do iii = 1, itercomp
c!DEC$ VECTOR ALWAYS
cc!DEC$ VECTOR ALIGNED
c      do jk = 1,nssq
c      rhol(jk,i16m03) = alunsm(jk,i16m03)
c      rhor(jk,i16m03) = alunsm(jk,i16m02)
c      almon(jk) = 3. * rho(jk,i16m03)  -  2. * rhor(jk,i16m03)
c      armon(jk) = 3. * rho(jk,i16m03)  -  2. * rhol(jk,i16m03)
c      if (((rho(jk,i16m03) - rhol(jk,i16m03))
c     &   * (rho(jk,i16m03) - rhor(jk,i16m03))) .ge. 0.)  then
c         rhol(jk,i16m03) = rho(jk,i16m03)
c         rhor(jk,i16m03) = rho(jk,i16m03)
c         almon(jk) = rho(jk,i16m03)
c         armon(jk) = rho(jk,i16m03)
c      endif
c      if (((rhor(jk,i16m03) - rhol(jk,i16m03))
c     &   * (almon(jk) - rhol(jk,i16m03))) .gt. 0.)
c     &      rhol(jk,i16m03) = almon(jk)
c      if (((rhor(jk,i16m03) - rhol(jk,i16m03))
c     &   * (armon(jk) - rhor(jk,i16m03))) .lt. 0.)
c     &      rhor(jk,i16m03) = armon(jk)
c#if isviolent == 0
c      rhol(jk,i16m03) = alsmth(jk,i16m03)
c     &     -  unsmth(jk,i16m03) * (alsmth(jk,i16m03) - rhol(jk,i16m03))
c      rhor(jk,i16m03) = alsmth(jk,i16m02)
c     &     -  unsmth(jk,i16m03) * (alsmth(jk,i16m02) - rhor(jk,i16m03))
c#endif
c      drho(jk,i16m03) = rhor(jk,i16m03) - rhol(jk,i16m03)
c      rho6(jk,i16m03) = 6. * (rho(jk,i16m03)
c     &             -  .5 * (rhol(jk,i16m03) + rhor(jk,i16m03)))
cc
cc
c      rhol(jk,i16m04) = alunsm(jk,i16m04)
c      rhor(jk,i16m04) = alunsm(jk,i16m03)
c      almon(jk) = 3. * rho(jk,i16m04)  -  2. * rhor(jk,i16m04)
c      armon(jk) = 3. * rho(jk,i16m04)  -  2. * rhol(jk,i16m04)
c      if (((rho(jk,i16m04) - rhol(jk,i16m04))
c     &   * (rho(jk,i16m04) - rhor(jk,i16m04))) .ge. 0.)  then
c         rhol(jk,i16m04) = rho(jk,i16m04)
c         rhor(jk,i16m04) = rho(jk,i16m04)
c         almon(jk) = rho(jk,i16m04)
c         armon(jk) = rho(jk,i16m04)
c      endif
c      if (((rhor(jk,i16m04) - rhol(jk,i16m04))
c     &   * (almon(jk) - rhol(jk,i16m04))) .gt. 0.)
c     &      rhol(jk,i16m04) = almon(jk)
c      if (((rhor(jk,i16m04) - rhol(jk,i16m04))
c     &   * (armon(jk) - rhor(jk,i16m04))) .lt. 0.)
c     &      rhor(jk,i16m04) = armon(jk)
c#if isviolent == 0
c      rhol(jk,i16m04) = alsmth(jk,i16m04)
c     &     -  unsmth(jk,i16m04) * (alsmth(jk,i16m04) - rhol(jk,i16m04))
c      rhor(jk,i16m04) = alsmth(jk,i16m03)
c     &     -  unsmth(jk,i16m04) * (alsmth(jk,i16m03) - rhor(jk,i16m04))
c#endif
c      drho(jk,i16m04) = rhor(jk,i16m04) - rhol(jk,i16m04)
c      rho6(jk,i16m04) = 6. * (rho(jk,i16m04)
c     &             -  .5 * (rhol(jk,i16m04) + rhor(jk,i16m04)))
c      enddo
cc7000  continue
cc
c      ninloop = nssq * 2
c      oop = ninloop
c      cvmgms = cvmgms + oop * 6.
c#if isviolent
c      amults = amults + oop * 8.
c      adds = adds + oop * 11.
c#else
c      amults = amults + oop * 10.
c      adds = adds + oop * 15.
c#endif
cc
c      enddo
cc
cc
ccc      do 3500   i = -nbdy+4,n+nbdy-3
cc      ibegin = 1-nx-3
cc      ibegin = max(ibegin,4-nx*(icube+nghostcubes))
cc      if (ibegin .le. -3)   then
cc      do 3500   i = ibegin,-3
c      do iii = 1, itercomp
c!DEC$ VECTOR ALWAYS
cc!DEC$ VECTOR ALIGNED
c      do jk = 1,nssq
c      duxavl(jk,i16m03) = uxavl(jk,i16m02) - uxavl(jk,i16m03)
c      duxavl(jk,i16m04) = uxavl(jk,i16m03) - uxavl(jk,i16m04)
c      duxavl(jk,i16m05) = uxavl(jk,i16m04) - uxavl(jk,i16m05)
c      duxavl(jk,i16m06) = uxavl(jk,i16m05) - uxavl(jk,i16m06)
c      enddo
cc3500  continue
cc
c      ninloop = nssq * 4
c      oop = ninloop
c      adds = adds + oop
cc
c      enddo
cc
cc
ccc      do 3600   i = -nbdy+5,n+nbdy-3
cc      ibegin = 1-nx-3
cc      ibegin = max(ibegin,5-nx*(icube+nghostcubes))
cc      if (ibegin .le. -3)   then
cc      do 3600   i = ibegin,-3
c      do iii = 1, itercomp
c!DEC$ VECTOR ALWAYS
cc!DEC$ VECTOR ALIGNED
c      do jk = 1,nssq
c      thngy01(jk) = duxavl(jk,i16m04)
c      if (uxavl(jk,i16m03) .lt. 0.)   thngy01(jk) = duxavl(jk,i16m03)
c      sigmal(jk,i16m03) = abs(uxavl(jk,i16m03))
c     &                 / (dxbydt  +  .5 * thngy01(jk))
cc
c      thngy01(jk) = duxavl(jk,i16m05)
c      if (uxavl(jk,i16m04) .lt. 0.)   thngy01(jk) = duxavl(jk,i16m04)
c      sigmal(jk,i16m04) = abs(uxavl(jk,i16m04))
c     &                 / (dxbydt  +  .5 * thngy01(jk))
cc
c      thngy01(jk) = duxavl(jk,i16m06)
c      if (uxavl(jk,i16m05) .lt. 0.)   thngy01(jk) = duxavl(jk,i16m05)
c      sigmal(jk,i16m05) = abs(uxavl(jk,i16m05))
c     &                 / (dxbydt  +  .5 * thngy01(jk))
c      enddo
cc3600  continue
cc
c      ninloop = nssq * 3
c      oop = ninloop
c      recips = recips + oop
c      cvmgms = cvmgms + oop * 2.
c      amults = amults + oop * 2.
c      adds = adds + oop
cc
c      enddo
cc
cc
ccc      do 3700   i = 5-nbdy,n+nbdy-3
cc      ibegin = 1-nx-3
cc      ibegin = max(ibegin,5-nx*(icube+nghostcubes))
cc      do 3700   i = ibegin,-3
c      do iii = 1, itercomp
c!DEC$ VECTOR ALWAYS
cc!DEC$ VECTOR ALIGNED
c      do jk = 1,nssq
c      xfr0 = .5 * sigmal(jk,i16m03)
c      xfr01 = 1.  -  forthd * xfr0
c      thingl(jk) = rhor(jk,i16m04)
c      if (uxavl(jk,i16m03) .lt. 0.)   thingl(jk) = rhol(jk,i16m03)
c      dthing(jk) = - drho(jk,i16m04)
c      if (uxavl(jk,i16m03) .lt. 0.)   dthing(jk) = drho(jk,i16m03)
c      thing6(jk) = rho6(jk,i16m04)
c      if (uxavl(jk,i16m03) .lt. 0.)   thing6(jk) = rho6(jk,i16m03)
c      rhomlr0 = thingl(jk)  + xfr0 * (dthing(jk)  + xfr01 * thing6(jk))
c      dvoll(jk,i16m03) = sigmal(jk,i16m03) * deex
c      if (uxavl(jk,i16m03) .lt. 0.)
c     &                     dvoll(jk,i16m03) = - dvoll(jk,i16m03)
c      dmassl(jk,i16m03) = rhomlr0 * dvoll(jk,i16m03)
c      enddo
cc3700  continue
cc
c      ninloop = nssq
c      cvmgms = cvmgms + oop * 3.
c      amults = amults + oop * 6.
c      adds = adds + oop * 3.
cc
c      enddo
cc
cc
cc     ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
cc     The code for just the second briquette ends here.
cc
c      endif
cc
cc
c      else
cc
cc     The code for just the first briquette begins here.
cc     ||||||||||||||||||||||||||||||||||||||||||||||||||
cc
cc
cc
ccc      do   i = -nbdy+3,n+nbdy-2
cc      do i = ibegin,-2
c!DEC$ VECTOR ALWAYS
cc!DEC$ VECTOR ALIGNED
c      do jk = 1,nssq
c#if isviolent
c      unsmth(jk,i16m02) = 1.
c      unsmth(jk,i16m03) = 1.
c#else
c      unsmth(jk,i16m02) = 0.
c      unsmth(jk,i16m03) = 0.
c#endif
c      enddo
cc      enddo
cc
cc
cc
ccc      do 100   i = 2-nbdy,n+nbdy
cc      do 100   i = ibegin,0
c      do iii = 1, itercomp
c!DEC$ VECTOR ALWAYS
cc!DEC$ VECTOR ALIGNED
c      do jk = 1,nssq
c      dal(jk,i16m00) = rho(jk,i16m00) - rho(jk,i16m01)
c      absdal(jk,i16m00) = abs (dal(jk,i16m00))
cc
c      dal(jk,i16m01) = rho(jk,i16m01) - rho(jk,i16m02)
c      absdal(jk,i16m01) = abs (dal(jk,i16m01))
cc
c      dal(jk,i16m02) = rho(jk,i16m02) - rho(jk,i16m03)
c      absdal(jk,i16m02) = abs (dal(jk,i16m02))
c      enddo
cc100   continue
cc
c      ninloop = nssq * 3
c      oop = ninloop
c      recips = recips + 1.
c      cvmgms = cvmgms + oop
c      amults = amults + 1.
c      adds = adds + oop + 2.
cc
c      enddo
cc
cc
cc     ||||||||||||||||||||||||||||||||||||||||||||||||
cc     The code for just the first briquette ends here.
cc
c      endif
cc
cc
cccccccccccccccccccccccccc  Fake memory write ccccccccccccccccccccccccc
c      do kcube = 1,nsugarcubes
c      do jcube = 1,nsugarcubes
c      mynucube = 1  +  (nbqx- (icube - nghostcubes)) * incx
c     &              +  (jcube - 1) * incy  +  (kcube - 1) * incz
c!DEC$ VECTOR ALWAYS
cc!DEC$ VECTOR ALIGNED
c      do k = 1,nsugarcubed
c         dddummy1(k,mynucube,1) = d(k,jcube,kcube,0)
c         dddummy1(k,mynucube,2) = d(k,jcube,kcube,0)
c         dddummy1(k,mynucube,3) = d(k,jcube,kcube,0)
c         dddummy1(k,mynucube,4) = d(k,jcube,kcube,0)
c         dddummy1(k,mynucube,5) = d(k,jcube,kcube,0)
c         dddummy1(k,mynucube,6) = d(k,jcube,kcube,0)
c         dddummy1(k,mynucube,7) = d(k,jcube,kcube,0)
c         dddummy1(k,mynucube,8) = d(k,jcube,kcube,0)
c      enddo
c      enddo
c      enddo
ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
cc
cc
cc
cc
c9990  continue
cc
cc                     This is the end of the loop over  icube.
cc
cc
cc
c#if istrace
cc      dimension             xldbg(1-nghostcells:nx*nbqx+nghostcells)
cc      dimension             xrdbg(1-nghostcells:nx*nbqx+nghostcells)
cc      dimension       rhodbg(nssq,1-nghostcells:nx*nbqx+nghostcells)
cc      dimension        dmdbg(nssq,1-nghostcells:nx*nbqx+nghostcells)
cc      dimension    cournodbg(nssq,1-nghostcells:nx*nbqx+nghostcells)
cc      dimension    unsmthdbg(nssq,1-nghostcells:nx*nbqx+nghostcells)
cc      dimension      rholdbg(nssq,1-nghostcells:nx*nbqx+nghostcells)
cc      dimension      rhordbg(nssq,1-nghostcells:nx*nbqx+nghostcells)
cc      dimension      drhodbg(nssq,1-nghostcells:nx*nbqx+nghostcells)
cc      dimension      rho6dbg(nssq,1-nghostcells:nx*nbqx+nghostcells)
cc      dimension     uxavldbg(nssq,1-nghostcells:nx*nbqx+nghostcells)
cc      dimension    duxavldbg(nssq,1-nghostcells:nx*nbqx+nghostcells)
cc      dimension    sigmaldbg(nssq,1-nghostcells:nx*nbqx+nghostcells)
cc      dimension     dvolldbg(nssq,1-nghostcells:nx*nbqx+nghostcells)
cc      dimension    dmassldbg(nssq,1-nghostcells:nx*nbqx+nghostcells)
cc      dimension     rhonudbg(nssq,1-nghostcells:nx*nbqx+nghostcells)
c#endif
cc
cc
cc
cc
cc
c      flops = 1.4d+01*(sqrts+rsqrts) + 1.4d+01*exps + 0.0d+00*cvmgms
c      flops = adds + amults + 3.0d+00*recips + flops
cc      write (6,*) 'flops =',flops
cc
c      flopcounts(1) = flops
c      flopcounts(2) = adds
c      flopcounts(3) = amults
c      flopcounts(4) = recips
c      flopcounts(5) = cvmgms
c      flopcounts(6) = sqrts
c      flopcounts(7) = rsqrts
c      flopcounts(8) = exps
cc
cc
cc     loop #    adds    mults    cvmgms    recips    flops     exps
cc       0         0       5/4       0        0         5        1/4
cc       1         0        2        1        0         2
cc       2         1        0        1        0         1
cc       3        14       12        9        1        29
cc       4         5        3        0        0         8
cc       5        15       10        6        0        25
cc       7         1        0        0        0         1
cc       8         1        2        2        1         6
cc       9         3        6        3        0         9
cc      10         2        1        0        0         3
cc      11         1        0        1        0         1
cc
cc     Total      43       38       23        2        90         3
cc
      return
      end
c
