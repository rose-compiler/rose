/*

                 Copyright (c) 2010.
      Lawrence Livermore National Security, LLC.
Produced at the Lawrence Livermore National Laboratory.
                  LLNL-CODE-461231
                All rights reserved.

This file is part of LULESH, Version 1.0.
Please also read this link -- http://www.opensource.org/licenses/index.php

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the disclaimer below.

   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the disclaimer (as noted below)
     in the documentation and/or other materials provided with the
     distribution.

   * Neither the name of the LLNS/LLNL nor the names of its contributors
     may be used to endorse or promote products derived from this software
     without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL LAWRENCE LIVERMORE NATIONAL SECURITY, LLC,
THE U.S. DEPARTMENT OF ENERGY OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


Additional BSD Notice

1. This notice is required to be provided under our contract with the U.S.
   Department of Energy (DOE). This work was produced at Lawrence Livermore
   National Laboratory under Contract No. DE-AC52-07NA27344 with the DOE.

2. Neither the United States Government nor Lawrence Livermore National
   Security, LLC nor any of their employees, makes any warranty, express
   or implied, or assumes any liability or responsibility for the accuracy,
   completeness, or usefulness of any information, apparatus, product, or
   process disclosed, or represents that its use would not infringe
   privately-owned rights.

3. Also, reference herein to any specific commercial products, process, or
   services by trade name, trademark, manufacturer or otherwise does not
   necessarily constitute or imply its endorsement, recommendation, or
   favoring by the United States Government or Lawrence Livermore National
   Security, LLC. The views and opinions of authors expressed herein do not
   necessarily state or reflect those of the United States Government or
   Lawrence Livermore National Security, LLC, and shall not be used for
   advertising or product endorsement purposes.

*/

#include <vector>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define LULESH_SHOW_PROGRESS 1

enum { VolumeError = -1, QStopError = -2 } ;

/****************************************************/
/* Allow flexibility for arithmetic representations */
/****************************************************/

/* Could also support fixed point and interval arithmetic types */
typedef float        real4 ;
typedef double       real8 ;
typedef long double  real10 ;  /* 10 bytes on x86 */

typedef int    Index_t ; /* array subscript and loop index */
typedef real8  Real_t ;  /* floating point representation */
typedef int    Int_t ;   /* integer representation */

inline real4  SQRT(real4  arg) { return sqrtf(arg) ; }
inline real8  SQRT(real8  arg) { return sqrt(arg) ; }
inline real10 SQRT(real10 arg) { return sqrtl(arg) ; }

inline real4  CBRT(real4  arg) { return cbrtf(arg) ; }
inline real8  CBRT(real8  arg) { return cbrt(arg) ; }
inline real10 CBRT(real10 arg) { return cbrtl(arg) ; }

inline real4  FABS(real4  arg) { return fabsf(arg) ; }
inline real8  FABS(real8  arg) { return fabs(arg) ; }
inline real10 FABS(real10 arg) { return fabsl(arg) ; }


/************************************************************/
/* Allow for flexible data layout experiments by separating */
/* array interface from underlying implementation.          */
/************************************************************/

struct Mesh {

/* This first implementation allows for runnable code */
/* and is not meant to be optimal. Final implementation */
/* should separate declaration and allocation phases */
/* so that allocation can be scheduled in a cache conscious */
/* manner. */

public:

   /**************/
   /* Allocation */
   /**************/

   void AllocateNodalPersistent(size_t size)
   {
      m_x.reserve(size) ;
      m_y.reserve(size) ;
      m_z.reserve(size) ;

      m_xd.reserve(size) ;
      m_yd.reserve(size) ;
      m_zd.reserve(size) ;

      m_xdd.reserve(size) ;
      m_ydd.reserve(size) ;
      m_zdd.reserve(size) ;

      m_fx.reserve(size) ;
      m_fy.reserve(size) ;
      m_fz.reserve(size) ;

      m_nodalMass.reserve(size) ;
   }

   void AllocateElemPersistent(size_t size)
   {
      m_matElemlist.reserve(size) ;
      m_nodelist.reserve(8*size) ;

      m_lxim.reserve(size) ;
      m_lxip.reserve(size) ;
      m_letam.reserve(size) ;
      m_letap.reserve(size) ;
      m_lzetam.reserve(size) ;
      m_lzetap.reserve(size) ;

      m_elemBC.reserve(size) ;

      m_e.reserve(size) ;

      m_p.reserve(size) ;
      m_q.reserve(size) ;
      m_ql.reserve(size) ;
      m_qq.reserve(size) ;

      m_v.reserve(size) ;
      m_volo.reserve(size) ;
      m_delv.reserve(size) ;
      m_vdov.reserve(size) ;

      m_arealg.reserve(size) ;
   
      m_ss.reserve(size) ;

      m_elemMass.reserve(size) ;
   }

   /* Temporaries should not be initialized in bulk but */
   /* this is a runnable placeholder for now */
   void AllocateElemTemporary(size_t size)
   {
      m_dxx.reserve(size) ;
      m_dyy.reserve(size) ;
      m_dzz.reserve(size) ;

      m_delv_xi.reserve(size) ;
      m_delv_eta.reserve(size) ;
      m_delv_zeta.reserve(size) ;

      m_delx_xi.reserve(size) ;
      m_delx_eta.reserve(size) ;
      m_delx_zeta.reserve(size) ;

      m_vnew.reserve(size) ;
   }

   void AllocateNodesets(size_t size)
   {
      m_symmX.reserve(size) ;
      m_symmY.reserve(size) ;
      m_symmZ.reserve(size) ;
   }
   
   /**********/
   /* Access */
   /**********/

   /* Node-centered */

   Real_t& x(Index_t idx)    { return m_x[idx] ; }
   Real_t& y(Index_t idx)    { return m_y[idx] ; }
   Real_t& z(Index_t idx)    { return m_z[idx] ; }

   Real_t& xd(Index_t idx)   { return m_xd[idx] ; }
   Real_t& yd(Index_t idx)   { return m_yd[idx] ; }
   Real_t& zd(Index_t idx)   { return m_zd[idx] ; }

   Real_t& xdd(Index_t idx)  { return m_xdd[idx] ; }
   Real_t& ydd(Index_t idx)  { return m_ydd[idx] ; }
   Real_t& zdd(Index_t idx)  { return m_zdd[idx] ; }

   Real_t& fx(Index_t idx)   { return m_fx[idx] ; }
   Real_t& fy(Index_t idx)   { return m_fy[idx] ; }
   Real_t& fz(Index_t idx)   { return m_fz[idx] ; }

   Real_t& nodalMass(Index_t idx) { return m_nodalMass[idx] ; }

   Index_t&  symmX(Index_t idx) { return m_symmX[idx] ; }
   Index_t&  symmY(Index_t idx) { return m_symmY[idx] ; }
   Index_t&  symmZ(Index_t idx) { return m_symmZ[idx] ; }

   /* Element-centered */

   Index_t&  matElemlist(Index_t idx) { return m_matElemlist[idx] ; }
   Index_t*  nodelist(Index_t idx)    { return &m_nodelist[Index_t(8)*idx] ; }

   Index_t&  lxim(Index_t idx) { return m_lxim[idx] ; }
   Index_t&  lxip(Index_t idx) { return m_lxip[idx] ; }
   Index_t&  letam(Index_t idx) { return m_letam[idx] ; }
   Index_t&  letap(Index_t idx) { return m_letap[idx] ; }
   Index_t&  lzetam(Index_t idx) { return m_lzetam[idx] ; }
   Index_t&  lzetap(Index_t idx) { return m_lzetap[idx] ; }

   Int_t&  elemBC(Index_t idx) { return m_elemBC[idx] ; }

   Real_t& dxx(Index_t idx)  { return m_dxx[idx] ; }
   Real_t& dyy(Index_t idx)  { return m_dyy[idx] ; }
   Real_t& dzz(Index_t idx)  { return m_dzz[idx] ; }

   Real_t& delv_xi(Index_t idx)    { return m_delv_xi[idx] ; }
   Real_t& delv_eta(Index_t idx)   { return m_delv_eta[idx] ; }
   Real_t& delv_zeta(Index_t idx)  { return m_delv_zeta[idx] ; }

   Real_t& delx_xi(Index_t idx)    { return m_delx_xi[idx] ; }
   Real_t& delx_eta(Index_t idx)   { return m_delx_eta[idx] ; }
   Real_t& delx_zeta(Index_t idx)  { return m_delx_zeta[idx] ; }

   Real_t& e(Index_t idx)          { return m_e[idx] ; }

   Real_t& p(Index_t idx)          { return m_p[idx] ; }
   Real_t& q(Index_t idx)          { return m_q[idx] ; }
   Real_t& ql(Index_t idx)         { return m_ql[idx] ; }
   Real_t& qq(Index_t idx)         { return m_qq[idx] ; }

   Real_t& v(Index_t idx)          { return m_v[idx] ; }
   Real_t& volo(Index_t idx)       { return m_volo[idx] ; }
   Real_t& vnew(Index_t idx)       { return m_vnew[idx] ; }
   Real_t& delv(Index_t idx)       { return m_delv[idx] ; }
   Real_t& vdov(Index_t idx)       { return m_vdov[idx] ; }

   Real_t& arealg(Index_t idx)     { return m_arealg[idx] ; }
   
   Real_t& ss(Index_t idx)         { return m_ss[idx] ; }

   Real_t& elemMass(Index_t idx)  { return m_elemMass[idx] ; }

   /* Params */

   Real_t& dtfixed()              { return m_dtfixed ; }
   Real_t& time()                 { return m_time ; }
   Real_t& deltatime()            { return m_deltatime ; }
   Real_t& deltatimemultlb()      { return m_deltatimemultlb ; }
   Real_t& deltatimemultub()      { return m_deltatimemultub ; }
   Real_t& stoptime()             { return m_stoptime ; }

   Real_t& u_cut()                { return m_u_cut ; }
   Real_t& hgcoef()               { return m_hgcoef ; }
   Real_t& qstop()                { return m_qstop ; }
   Real_t& monoq_max_slope()      { return m_monoq_max_slope ; }
   Real_t& monoq_limiter_mult()   { return m_monoq_limiter_mult ; }
   Real_t& e_cut()                { return m_e_cut ; }
   Real_t& p_cut()                { return m_p_cut ; }
   Real_t& ss4o3()                { return m_ss4o3 ; }
   Real_t& q_cut()                { return m_q_cut ; }
   Real_t& v_cut()                { return m_v_cut ; }
   Real_t& qlc_monoq()            { return m_qlc_monoq ; }
   Real_t& qqc_monoq()            { return m_qqc_monoq ; }
   Real_t& qqc()                  { return m_qqc ; }
   Real_t& eosvmax()              { return m_eosvmax ; }
   Real_t& eosvmin()              { return m_eosvmin ; }
   Real_t& pmin()                 { return m_pmin ; }
   Real_t& emin()                 { return m_emin ; }
   Real_t& dvovmax()              { return m_dvovmax ; }
   Real_t& refdens()              { return m_refdens ; }

   Real_t& dtcourant()            { return m_dtcourant ; }
   Real_t& dthydro()              { return m_dthydro ; }
   Real_t& dtmax()                { return m_dtmax ; }

   Int_t&  cycle()                { return m_cycle ; }

   Index_t&  sizeX()              { return m_sizeX ; }
   Index_t&  sizeY()              { return m_sizeY ; }
   Index_t&  sizeZ()              { return m_sizeZ ; }
   Index_t&  numElem()            { return m_numElem ; }
   Index_t&  numNode()            { return m_numNode ; }

private:

   /******************/
   /* Implementation */
   /******************/

   /* Node-centered */

   std::vector<Real_t> m_x ;  /* coordinates */
   std::vector<Real_t> m_y ;
   std::vector<Real_t> m_z ;

   std::vector<Real_t> m_xd ; /* velocities */
   std::vector<Real_t> m_yd ;
   std::vector<Real_t> m_zd ;

   std::vector<Real_t> m_xdd ; /* accelerations */
   std::vector<Real_t> m_ydd ;
   std::vector<Real_t> m_zdd ;

   std::vector<Real_t> m_fx ;  /* forces */
   std::vector<Real_t> m_fy ;
   std::vector<Real_t> m_fz ;

   std::vector<Real_t> m_nodalMass ;  /* mass */

   std::vector<Index_t> m_symmX ;  /* symmetry plane nodesets */
   std::vector<Index_t> m_symmY ;
   std::vector<Index_t> m_symmZ ;

   /* Element-centered */

   std::vector<Index_t>  m_matElemlist ;  /* material indexset */
   std::vector<Index_t>  m_nodelist ;     /* elemToNode connectivity */

   std::vector<Index_t>  m_lxim ;  /* element connectivity across each face */
   std::vector<Index_t>  m_lxip ;
   std::vector<Index_t>  m_letam ;
   std::vector<Index_t>  m_letap ;
   std::vector<Index_t>  m_lzetam ;
   std::vector<Index_t>  m_lzetap ;

   std::vector<Int_t>    m_elemBC ;  /* symmetry/free-surface flags for each elem face */

   std::vector<Real_t> m_dxx ;  /* principal strains -- temporary */
   std::vector<Real_t> m_dyy ;
   std::vector<Real_t> m_dzz ;

   std::vector<Real_t> m_delv_xi ;    /* velocity gradient -- temporary */
   std::vector<Real_t> m_delv_eta ;
   std::vector<Real_t> m_delv_zeta ;

   std::vector<Real_t> m_delx_xi ;    /* coordinate gradient -- temporary */
   std::vector<Real_t> m_delx_eta ;
   std::vector<Real_t> m_delx_zeta ;
   
   std::vector<Real_t> m_e ;   /* energy */

   std::vector<Real_t> m_p ;   /* pressure */
   std::vector<Real_t> m_q ;   /* q */
   std::vector<Real_t> m_ql ;  /* linear term for q */
   std::vector<Real_t> m_qq ;  /* quadratic term for q */

   std::vector<Real_t> m_v ;     /* relative volume */
   std::vector<Real_t> m_volo ;  /* reference volume */
   std::vector<Real_t> m_vnew ;  /* new relative volume -- temporary */
   std::vector<Real_t> m_delv ;  /* m_vnew - m_v */
   std::vector<Real_t> m_vdov ;  /* volume derivative over volume */

   std::vector<Real_t> m_arealg ;  /* characteristic length of an element */
   
   std::vector<Real_t> m_ss ;      /* "sound speed" */

   std::vector<Real_t> m_elemMass ;  /* mass */

   /* Parameters */

   Real_t  m_dtfixed ;           /* fixed time increment */
   Real_t  m_time ;              /* current time */
   Real_t  m_deltatime ;         /* variable time increment */
   Real_t  m_deltatimemultlb ;
   Real_t  m_deltatimemultub ;
   Real_t  m_stoptime ;          /* end time for simulation */

   Real_t  m_u_cut ;             /* velocity tolerance */
   Real_t  m_hgcoef ;            /* hourglass control */
   Real_t  m_qstop ;             /* excessive q indicator */
   Real_t  m_monoq_max_slope ;
   Real_t  m_monoq_limiter_mult ;
   Real_t  m_e_cut ;             /* energy tolerance */
   Real_t  m_p_cut ;             /* pressure tolerance */
   Real_t  m_ss4o3 ;
   Real_t  m_q_cut ;             /* q tolerance */
   Real_t  m_v_cut ;             /* relative volume tolerance */
   Real_t  m_qlc_monoq ;         /* linear term coef for q */
   Real_t  m_qqc_monoq ;         /* quadratic term coef for q */
   Real_t  m_qqc ;
   Real_t  m_eosvmax ;
   Real_t  m_eosvmin ;
   Real_t  m_pmin ;              /* pressure floor */
   Real_t  m_emin ;              /* energy floor */
   Real_t  m_dvovmax ;           /* maximum allowable volume change */
   Real_t  m_refdens ;           /* reference density */

   Real_t  m_dtcourant ;         /* courant constraint */
   Real_t  m_dthydro ;           /* volume change constraint */
   Real_t  m_dtmax ;             /* maximum allowable time increment */

   Int_t   m_cycle ;             /* iteration count for simulation */

   Index_t   m_sizeX ;           /* X,Y,Z extent of this block */
   Index_t   m_sizeY ;
   Index_t   m_sizeZ ;

   Index_t   m_numElem ;         /* Elements/Nodes in this domain */
   Index_t   m_numNode ;
} mesh ;


template <typename T>
T *Allocate(size_t size)
{
   return static_cast<T *>(malloc(sizeof(T)*size)) ;
}

template <typename T>
void Release(T **ptr)
{
   if (*ptr != NULL) {
      free(*ptr) ;
      *ptr = NULL ;
   }
}


/* Stuff needed for boundary conditions */
/* 2 BCs on each of 6 hexahedral faces (12 bits) */
#define XI_M        0x003
#define XI_M_SYMM   0x001
#define XI_M_FREE   0x002

#define XI_P        0x00c
#define XI_P_SYMM   0x004
#define XI_P_FREE   0x008

#define ETA_M       0x030
#define ETA_M_SYMM  0x010
#define ETA_M_FREE  0x020

#define ETA_P       0x0c0
#define ETA_P_SYMM  0x040
#define ETA_P_FREE  0x080

#define ZETA_M      0x300
#define ZETA_M_SYMM 0x100
#define ZETA_M_FREE 0x200

#define ZETA_P      0xc00
#define ZETA_P_SYMM 0x400
#define ZETA_P_FREE 0x800


static inline
void TimeIncrement()
{
   Real_t targetdt = mesh.stoptime() - mesh.time() ;

   if ((mesh.dtfixed() <= Real_t(0.0)) && (mesh.cycle() != Int_t(0))) {
      Real_t ratio ;
      Real_t olddt = mesh.deltatime() ;

      /* This will require a reduction in parallel */
      Real_t newdt = Real_t(1.0e+20) ;
      if (mesh.dtcourant() < newdt) {
         newdt = mesh.dtcourant() / Real_t(2.0) ;
      }
      if (mesh.dthydro() < newdt) {
         newdt = mesh.dthydro() * Real_t(2.0) / Real_t(3.0) ;
      }

      ratio = newdt / olddt ;
      if (ratio >= Real_t(1.0)) {
         if (ratio < mesh.deltatimemultlb()) {
            newdt = olddt ;
         }
         else if (ratio > mesh.deltatimemultub()) {
            newdt = olddt*mesh.deltatimemultub() ;
         }
      }

      if (newdt > mesh.dtmax()) {
         newdt = mesh.dtmax() ;
      }
      mesh.deltatime() = newdt ;
   }

   /* TRY TO PREVENT VERY SMALL SCALING ON THE NEXT CYCLE */
   if ((targetdt > mesh.deltatime()) &&
       (targetdt < (Real_t(4.0) * mesh.deltatime() / Real_t(3.0))) ) {
      targetdt = Real_t(2.0) * mesh.deltatime() / Real_t(3.0) ;
   }

   if (targetdt < mesh.deltatime()) {
      mesh.deltatime() = targetdt ;
   }

   mesh.time() += mesh.deltatime() ;

   ++mesh.cycle() ;
}

static inline
void InitStressTermsForElems(Index_t numElem, 
                             Real_t *sigxx, Real_t *sigyy, Real_t *sigzz)
{
   //
   // pull in the stresses appropriate to the hydro integration
   //
   for (Index_t i = 0 ; i < numElem ; ++i){
      sigxx[i] =  sigyy[i] = sigzz[i] =  - mesh.p(i) - mesh.q(i) ;
   }
}

static inline
void CalcElemShapeFunctionDerivatives( const Real_t* const x,
                                       const Real_t* const y,
                                       const Real_t* const z,
                                       Real_t b[][8],
                                       Real_t* const volume )
{
  const Real_t x0 = x[0] ;   const Real_t x1 = x[1] ;
  const Real_t x2 = x[2] ;   const Real_t x3 = x[3] ;
  const Real_t x4 = x[4] ;   const Real_t x5 = x[5] ;
  const Real_t x6 = x[6] ;   const Real_t x7 = x[7] ;

  const Real_t y0 = y[0] ;   const Real_t y1 = y[1] ;
  const Real_t y2 = y[2] ;   const Real_t y3 = y[3] ;
  const Real_t y4 = y[4] ;   const Real_t y5 = y[5] ;
  const Real_t y6 = y[6] ;   const Real_t y7 = y[7] ;

  const Real_t z0 = z[0] ;   const Real_t z1 = z[1] ;
  const Real_t z2 = z[2] ;   const Real_t z3 = z[3] ;
  const Real_t z4 = z[4] ;   const Real_t z5 = z[5] ;
  const Real_t z6 = z[6] ;   const Real_t z7 = z[7] ;

  Real_t fjxxi, fjxet, fjxze;
  Real_t fjyxi, fjyet, fjyze;
  Real_t fjzxi, fjzet, fjzze;
  Real_t cjxxi, cjxet, cjxze;
  Real_t cjyxi, cjyet, cjyze;
  Real_t cjzxi, cjzet, cjzze;

  fjxxi = Real_t(.125) * ( (x6-x0) + (x5-x3) - (x7-x1) - (x4-x2) );
  fjxet = Real_t(.125) * ( (x6-x0) - (x5-x3) + (x7-x1) - (x4-x2) );
  fjxze = Real_t(.125) * ( (x6-x0) + (x5-x3) + (x7-x1) + (x4-x2) );

  fjyxi = Real_t(.125) * ( (y6-y0) + (y5-y3) - (y7-y1) - (y4-y2) );
  fjyet = Real_t(.125) * ( (y6-y0) - (y5-y3) + (y7-y1) - (y4-y2) );
  fjyze = Real_t(.125) * ( (y6-y0) + (y5-y3) + (y7-y1) + (y4-y2) );

  fjzxi = Real_t(.125) * ( (z6-z0) + (z5-z3) - (z7-z1) - (z4-z2) );
  fjzet = Real_t(.125) * ( (z6-z0) - (z5-z3) + (z7-z1) - (z4-z2) );
  fjzze = Real_t(.125) * ( (z6-z0) + (z5-z3) + (z7-z1) + (z4-z2) );

  /* compute cofactors */
  cjxxi =    (fjyet * fjzze) - (fjzet * fjyze);
  cjxet =  - (fjyxi * fjzze) + (fjzxi * fjyze);
  cjxze =    (fjyxi * fjzet) - (fjzxi * fjyet);

  cjyxi =  - (fjxet * fjzze) + (fjzet * fjxze);
  cjyet =    (fjxxi * fjzze) - (fjzxi * fjxze);
  cjyze =  - (fjxxi * fjzet) + (fjzxi * fjxet);

  cjzxi =    (fjxet * fjyze) - (fjyet * fjxze);
  cjzet =  - (fjxxi * fjyze) + (fjyxi * fjxze);
  cjzze =    (fjxxi * fjyet) - (fjyxi * fjxet);

  /* calculate partials :
     this need only be done for l = 0,1,2,3   since , by symmetry ,
     (6,7,4,5) = - (0,1,2,3) .
  */
  b[0][0] =   -  cjxxi  -  cjxet  -  cjxze;
  b[0][1] =      cjxxi  -  cjxet  -  cjxze;
  b[0][2] =      cjxxi  +  cjxet  -  cjxze;
  b[0][3] =   -  cjxxi  +  cjxet  -  cjxze;
  b[0][4] = -b[0][2];
  b[0][5] = -b[0][3];
  b[0][6] = -b[0][0];
  b[0][7] = -b[0][1];

  b[1][0] =   -  cjyxi  -  cjyet  -  cjyze;
  b[1][1] =      cjyxi  -  cjyet  -  cjyze;
  b[1][2] =      cjyxi  +  cjyet  -  cjyze;
  b[1][3] =   -  cjyxi  +  cjyet  -  cjyze;
  b[1][4] = -b[1][2];
  b[1][5] = -b[1][3];
  b[1][6] = -b[1][0];
  b[1][7] = -b[1][1];

  b[2][0] =   -  cjzxi  -  cjzet  -  cjzze;
  b[2][1] =      cjzxi  -  cjzet  -  cjzze;
  b[2][2] =      cjzxi  +  cjzet  -  cjzze;
  b[2][3] =   -  cjzxi  +  cjzet  -  cjzze;
  b[2][4] = -b[2][2];
  b[2][5] = -b[2][3];
  b[2][6] = -b[2][0];
  b[2][7] = -b[2][1];

  /* calculate jacobian determinant (volume) */
  *volume = Real_t(8.) * ( fjxet * cjxet + fjyet * cjyet + fjzet * cjzet);
}

static inline
void SumElemFaceNormal(Real_t *normalX0, Real_t *normalY0, Real_t *normalZ0,
                       Real_t *normalX1, Real_t *normalY1, Real_t *normalZ1,
                       Real_t *normalX2, Real_t *normalY2, Real_t *normalZ2,
                       Real_t *normalX3, Real_t *normalY3, Real_t *normalZ3,
                       const Real_t x0, const Real_t y0, const Real_t z0,
                       const Real_t x1, const Real_t y1, const Real_t z1,
                       const Real_t x2, const Real_t y2, const Real_t z2,
                       const Real_t x3, const Real_t y3, const Real_t z3)
{
   Real_t bisectX0 = Real_t(0.5) * (x3 + x2 - x1 - x0);
   Real_t bisectY0 = Real_t(0.5) * (y3 + y2 - y1 - y0);
   Real_t bisectZ0 = Real_t(0.5) * (z3 + z2 - z1 - z0);
   Real_t bisectX1 = Real_t(0.5) * (x2 + x1 - x3 - x0);
   Real_t bisectY1 = Real_t(0.5) * (y2 + y1 - y3 - y0);
   Real_t bisectZ1 = Real_t(0.5) * (z2 + z1 - z3 - z0);
   Real_t areaX = Real_t(0.25) * (bisectY0 * bisectZ1 - bisectZ0 * bisectY1);
   Real_t areaY = Real_t(0.25) * (bisectZ0 * bisectX1 - bisectX0 * bisectZ1);
   Real_t areaZ = Real_t(0.25) * (bisectX0 * bisectY1 - bisectY0 * bisectX1);

   *normalX0 += areaX;
   *normalX1 += areaX;
   *normalX2 += areaX;
   *normalX3 += areaX;

   *normalY0 += areaY;
   *normalY1 += areaY;
   *normalY2 += areaY;
   *normalY3 += areaY;

   *normalZ0 += areaZ;
   *normalZ1 += areaZ;
   *normalZ2 += areaZ;
   *normalZ3 += areaZ;
}

static inline
void CalcElemNodeNormals(Real_t pfx[8],
                         Real_t pfy[8],
                         Real_t pfz[8],
                         const Real_t x[8],
                         const Real_t y[8],
                         const Real_t z[8])
{
   for (Index_t i = 0 ; i < 8 ; ++i) {
      pfx[i] = Real_t(0.0);
      pfy[i] = Real_t(0.0);
      pfz[i] = Real_t(0.0);
   }
   /* evaluate face one: nodes 0, 1, 2, 3 */
   SumElemFaceNormal(&pfx[0], &pfy[0], &pfz[0],
                  &pfx[1], &pfy[1], &pfz[1],
                  &pfx[2], &pfy[2], &pfz[2],
                  &pfx[3], &pfy[3], &pfz[3],
                  x[0], y[0], z[0], x[1], y[1], z[1],
                  x[2], y[2], z[2], x[3], y[3], z[3]);
   /* evaluate face two: nodes 0, 4, 5, 1 */
   SumElemFaceNormal(&pfx[0], &pfy[0], &pfz[0],
                  &pfx[4], &pfy[4], &pfz[4],
                  &pfx[5], &pfy[5], &pfz[5],
                  &pfx[1], &pfy[1], &pfz[1],
                  x[0], y[0], z[0], x[4], y[4], z[4],
                  x[5], y[5], z[5], x[1], y[1], z[1]);
   /* evaluate face three: nodes 1, 5, 6, 2 */
   SumElemFaceNormal(&pfx[1], &pfy[1], &pfz[1],
                  &pfx[5], &pfy[5], &pfz[5],
                  &pfx[6], &pfy[6], &pfz[6],
                  &pfx[2], &pfy[2], &pfz[2],
                  x[1], y[1], z[1], x[5], y[5], z[5],
                  x[6], y[6], z[6], x[2], y[2], z[2]);
   /* evaluate face four: nodes 2, 6, 7, 3 */
   SumElemFaceNormal(&pfx[2], &pfy[2], &pfz[2],
                  &pfx[6], &pfy[6], &pfz[6],
                  &pfx[7], &pfy[7], &pfz[7],
                  &pfx[3], &pfy[3], &pfz[3],
                  x[2], y[2], z[2], x[6], y[6], z[6],
                  x[7], y[7], z[7], x[3], y[3], z[3]);
   /* evaluate face five: nodes 3, 7, 4, 0 */
   SumElemFaceNormal(&pfx[3], &pfy[3], &pfz[3],
                  &pfx[7], &pfy[7], &pfz[7],
                  &pfx[4], &pfy[4], &pfz[4],
                  &pfx[0], &pfy[0], &pfz[0],
                  x[3], y[3], z[3], x[7], y[7], z[7],
                  x[4], y[4], z[4], x[0], y[0], z[0]);
   /* evaluate face six: nodes 4, 7, 6, 5 */
   SumElemFaceNormal(&pfx[4], &pfy[4], &pfz[4],
                  &pfx[7], &pfy[7], &pfz[7],
                  &pfx[6], &pfy[6], &pfz[6],
                  &pfx[5], &pfy[5], &pfz[5],
                  x[4], y[4], z[4], x[7], y[7], z[7],
                  x[6], y[6], z[6], x[5], y[5], z[5]);
}

static inline
void SumElemStressesToNodeForces( const Real_t B[][8],
                                  const Real_t stress_xx,
                                  const Real_t stress_yy,
                                  const Real_t stress_zz,
                                  Real_t* const fx,
                                  Real_t* const fy,
                                  Real_t* const fz )
{
  Real_t pfx0 = B[0][0] ;   Real_t pfx1 = B[0][1] ;
  Real_t pfx2 = B[0][2] ;   Real_t pfx3 = B[0][3] ;
  Real_t pfx4 = B[0][4] ;   Real_t pfx5 = B[0][5] ;
  Real_t pfx6 = B[0][6] ;   Real_t pfx7 = B[0][7] ;

  Real_t pfy0 = B[1][0] ;   Real_t pfy1 = B[1][1] ;
  Real_t pfy2 = B[1][2] ;   Real_t pfy3 = B[1][3] ;
  Real_t pfy4 = B[1][4] ;   Real_t pfy5 = B[1][5] ;
  Real_t pfy6 = B[1][6] ;   Real_t pfy7 = B[1][7] ;

  Real_t pfz0 = B[2][0] ;   Real_t pfz1 = B[2][1] ;
  Real_t pfz2 = B[2][2] ;   Real_t pfz3 = B[2][3] ;
  Real_t pfz4 = B[2][4] ;   Real_t pfz5 = B[2][5] ;
  Real_t pfz6 = B[2][6] ;   Real_t pfz7 = B[2][7] ;

  fx[0] = -( stress_xx * pfx0 );
  fx[1] = -( stress_xx * pfx1 );
  fx[2] = -( stress_xx * pfx2 );
  fx[3] = -( stress_xx * pfx3 );
  fx[4] = -( stress_xx * pfx4 );
  fx[5] = -( stress_xx * pfx5 );
  fx[6] = -( stress_xx * pfx6 );
  fx[7] = -( stress_xx * pfx7 );

  fy[0] = -( stress_yy * pfy0  );
  fy[1] = -( stress_yy * pfy1  );
  fy[2] = -( stress_yy * pfy2  );
  fy[3] = -( stress_yy * pfy3  );
  fy[4] = -( stress_yy * pfy4  );
  fy[5] = -( stress_yy * pfy5  );
  fy[6] = -( stress_yy * pfy6  );
  fy[7] = -( stress_yy * pfy7  );

  fz[0] = -( stress_zz * pfz0 );
  fz[1] = -( stress_zz * pfz1 );
  fz[2] = -( stress_zz * pfz2 );
  fz[3] = -( stress_zz * pfz3 );
  fz[4] = -( stress_zz * pfz4 );
  fz[5] = -( stress_zz * pfz5 );
  fz[6] = -( stress_zz * pfz6 );
  fz[7] = -( stress_zz * pfz7 );
}

static inline
void IntegrateStressForElems( Index_t numElem,
                              Real_t *sigxx, Real_t *sigyy, Real_t *sigzz,
                              Real_t *determ)
{
  Real_t B[3][8] ;// shape function derivatives
  Real_t x_local[8] ;
  Real_t y_local[8] ;
  Real_t z_local[8] ;
  Real_t fx_local[8] ;
  Real_t fy_local[8] ;
  Real_t fz_local[8] ;

  // loop over all elements
  for( Index_t k=0 ; k<numElem ; ++k )
  {
    const Index_t* const elemNodes = mesh.nodelist(k);

    // get nodal coordinates from global arrays and copy into local arrays.
    for( Index_t lnode=0 ; lnode<8 ; ++lnode )
    {
      Index_t gnode = elemNodes[lnode];
      x_local[lnode] = mesh.x(gnode);
      y_local[lnode] = mesh.y(gnode);
      z_local[lnode] = mesh.z(gnode);
    }

    /* Volume calculation involves extra work for numerical consistency. */
    CalcElemShapeFunctionDerivatives(x_local, y_local, z_local,
                                         B, &determ[k]);

    CalcElemNodeNormals( B[0] , B[1], B[2],
                          x_local, y_local, z_local );

    SumElemStressesToNodeForces( B, sigxx[k], sigyy[k], sigzz[k],
                                         fx_local, fy_local, fz_local ) ;

    // copy nodal force contributions to global force arrray.
    for( Index_t lnode=0 ; lnode<8 ; ++lnode )
    {
      Index_t gnode = elemNodes[lnode];
      mesh.fx(gnode) += fx_local[lnode];
      mesh.fy(gnode) += fy_local[lnode];
      mesh.fz(gnode) += fz_local[lnode];
    }
  }
}

static inline
void CollectDomainNodesToElemNodes(const Index_t* elemToNode,
                                   Real_t elemX[8],
                                   Real_t elemY[8],
                                   Real_t elemZ[8])
{
   Index_t nd0i = elemToNode[0] ;
   Index_t nd1i = elemToNode[1] ;
   Index_t nd2i = elemToNode[2] ;
   Index_t nd3i = elemToNode[3] ;
   Index_t nd4i = elemToNode[4] ;
   Index_t nd5i = elemToNode[5] ;
   Index_t nd6i = elemToNode[6] ;
   Index_t nd7i = elemToNode[7] ;

   elemX[0] = mesh.x(nd0i);
   elemX[1] = mesh.x(nd1i);
   elemX[2] = mesh.x(nd2i);
   elemX[3] = mesh.x(nd3i);
   elemX[4] = mesh.x(nd4i);
   elemX[5] = mesh.x(nd5i);
   elemX[6] = mesh.x(nd6i);
   elemX[7] = mesh.x(nd7i);

   elemY[0] = mesh.y(nd0i);
   elemY[1] = mesh.y(nd1i);
   elemY[2] = mesh.y(nd2i);
   elemY[3] = mesh.y(nd3i);
   elemY[4] = mesh.y(nd4i);
   elemY[5] = mesh.y(nd5i);
   elemY[6] = mesh.y(nd6i);
   elemY[7] = mesh.y(nd7i);

   elemZ[0] = mesh.z(nd0i);
   elemZ[1] = mesh.z(nd1i);
   elemZ[2] = mesh.z(nd2i);
   elemZ[3] = mesh.z(nd3i);
   elemZ[4] = mesh.z(nd4i);
   elemZ[5] = mesh.z(nd5i);
   elemZ[6] = mesh.z(nd6i);
   elemZ[7] = mesh.z(nd7i);

}

static inline
void VoluDer(const Real_t x0, const Real_t x1, const Real_t x2,
             const Real_t x3, const Real_t x4, const Real_t x5,
             const Real_t y0, const Real_t y1, const Real_t y2,
             const Real_t y3, const Real_t y4, const Real_t y5,
             const Real_t z0, const Real_t z1, const Real_t z2,
             const Real_t z3, const Real_t z4, const Real_t z5,
             Real_t* dvdx, Real_t* dvdy, Real_t* dvdz)
{
   const Real_t twelfth = Real_t(1.0) / Real_t(12.0) ;

   *dvdx =
      (y1 + y2) * (z0 + z1) - (y0 + y1) * (z1 + z2) +
      (y0 + y4) * (z3 + z4) - (y3 + y4) * (z0 + z4) -
      (y2 + y5) * (z3 + z5) + (y3 + y5) * (z2 + z5);
   *dvdy =
      - (x1 + x2) * (z0 + z1) + (x0 + x1) * (z1 + z2) -
      (x0 + x4) * (z3 + z4) + (x3 + x4) * (z0 + z4) +
      (x2 + x5) * (z3 + z5) - (x3 + x5) * (z2 + z5);

   *dvdz =
      - (y1 + y2) * (x0 + x1) + (y0 + y1) * (x1 + x2) -
      (y0 + y4) * (x3 + x4) + (y3 + y4) * (x0 + x4) +
      (y2 + y5) * (x3 + x5) - (y3 + y5) * (x2 + x5);

   *dvdx *= twelfth;
   *dvdy *= twelfth;
   *dvdz *= twelfth;
}

static inline
void CalcElemVolumeDerivative(Real_t dvdx[8],
                              Real_t dvdy[8],
                              Real_t dvdz[8],
                              const Real_t x[8],
                              const Real_t y[8],
                              const Real_t z[8])
{
   VoluDer(x[1], x[2], x[3], x[4], x[5], x[7],
           y[1], y[2], y[3], y[4], y[5], y[7],
           z[1], z[2], z[3], z[4], z[5], z[7],
           &dvdx[0], &dvdy[0], &dvdz[0]);
   VoluDer(x[0], x[1], x[2], x[7], x[4], x[6],
           y[0], y[1], y[2], y[7], y[4], y[6],
           z[0], z[1], z[2], z[7], z[4], z[6],
           &dvdx[3], &dvdy[3], &dvdz[3]);
   VoluDer(x[3], x[0], x[1], x[6], x[7], x[5],
           y[3], y[0], y[1], y[6], y[7], y[5],
           z[3], z[0], z[1], z[6], z[7], z[5],
           &dvdx[2], &dvdy[2], &dvdz[2]);
   VoluDer(x[2], x[3], x[0], x[5], x[6], x[4],
           y[2], y[3], y[0], y[5], y[6], y[4],
           z[2], z[3], z[0], z[5], z[6], z[4],
           &dvdx[1], &dvdy[1], &dvdz[1]);
   VoluDer(x[7], x[6], x[5], x[0], x[3], x[1],
           y[7], y[6], y[5], y[0], y[3], y[1],
           z[7], z[6], z[5], z[0], z[3], z[1],
           &dvdx[4], &dvdy[4], &dvdz[4]);
   VoluDer(x[4], x[7], x[6], x[1], x[0], x[2],
           y[4], y[7], y[6], y[1], y[0], y[2],
           z[4], z[7], z[6], z[1], z[0], z[2],
           &dvdx[5], &dvdy[5], &dvdz[5]);
   VoluDer(x[5], x[4], x[7], x[2], x[1], x[3],
           y[5], y[4], y[7], y[2], y[1], y[3],
           z[5], z[4], z[7], z[2], z[1], z[3],
           &dvdx[6], &dvdy[6], &dvdz[6]);
   VoluDer(x[6], x[5], x[4], x[3], x[2], x[0],
           y[6], y[5], y[4], y[3], y[2], y[0],
           z[6], z[5], z[4], z[3], z[2], z[0],
           &dvdx[7], &dvdy[7], &dvdz[7]);
}

static inline
void CalcElemFBHourglassForce(Real_t *xd, Real_t *yd, Real_t *zd,  Real_t *hourgam0,
                              Real_t *hourgam1, Real_t *hourgam2, Real_t *hourgam3,
                              Real_t *hourgam4, Real_t *hourgam5, Real_t *hourgam6,
                              Real_t *hourgam7, Real_t coefficient,
                              Real_t *hgfx, Real_t *hgfy, Real_t *hgfz )
{
   Index_t i00=0;
   Index_t i01=1;
   Index_t i02=2;
   Index_t i03=3;

   Real_t h00 =
      hourgam0[i00] * xd[0] + hourgam1[i00] * xd[1] +
      hourgam2[i00] * xd[2] + hourgam3[i00] * xd[3] +
      hourgam4[i00] * xd[4] + hourgam5[i00] * xd[5] +
      hourgam6[i00] * xd[6] + hourgam7[i00] * xd[7];

   Real_t h01 =
      hourgam0[i01] * xd[0] + hourgam1[i01] * xd[1] +
      hourgam2[i01] * xd[2] + hourgam3[i01] * xd[3] +
      hourgam4[i01] * xd[4] + hourgam5[i01] * xd[5] +
      hourgam6[i01] * xd[6] + hourgam7[i01] * xd[7];

   Real_t h02 =
      hourgam0[i02] * xd[0] + hourgam1[i02] * xd[1]+
      hourgam2[i02] * xd[2] + hourgam3[i02] * xd[3]+
      hourgam4[i02] * xd[4] + hourgam5[i02] * xd[5]+
      hourgam6[i02] * xd[6] + hourgam7[i02] * xd[7];

   Real_t h03 =
      hourgam0[i03] * xd[0] + hourgam1[i03] * xd[1] +
      hourgam2[i03] * xd[2] + hourgam3[i03] * xd[3] +
      hourgam4[i03] * xd[4] + hourgam5[i03] * xd[5] +
      hourgam6[i03] * xd[6] + hourgam7[i03] * xd[7];

   hgfx[0] = coefficient *
      (hourgam0[i00] * h00 + hourgam0[i01] * h01 +
       hourgam0[i02] * h02 + hourgam0[i03] * h03);

   hgfx[1] = coefficient *
      (hourgam1[i00] * h00 + hourgam1[i01] * h01 +
       hourgam1[i02] * h02 + hourgam1[i03] * h03);

   hgfx[2] = coefficient *
      (hourgam2[i00] * h00 + hourgam2[i01] * h01 +
       hourgam2[i02] * h02 + hourgam2[i03] * h03);

   hgfx[3] = coefficient *
      (hourgam3[i00] * h00 + hourgam3[i01] * h01 +
       hourgam3[i02] * h02 + hourgam3[i03] * h03);

   hgfx[4] = coefficient *
      (hourgam4[i00] * h00 + hourgam4[i01] * h01 +
       hourgam4[i02] * h02 + hourgam4[i03] * h03);

   hgfx[5] = coefficient *
      (hourgam5[i00] * h00 + hourgam5[i01] * h01 +
       hourgam5[i02] * h02 + hourgam5[i03] * h03);

   hgfx[6] = coefficient *
      (hourgam6[i00] * h00 + hourgam6[i01] * h01 +
       hourgam6[i02] * h02 + hourgam6[i03] * h03);

   hgfx[7] = coefficient *
      (hourgam7[i00] * h00 + hourgam7[i01] * h01 +
       hourgam7[i02] * h02 + hourgam7[i03] * h03);

   h00 =
      hourgam0[i00] * yd[0] + hourgam1[i00] * yd[1] +
      hourgam2[i00] * yd[2] + hourgam3[i00] * yd[3] +
      hourgam4[i00] * yd[4] + hourgam5[i00] * yd[5] +
      hourgam6[i00] * yd[6] + hourgam7[i00] * yd[7];

   h01 =
      hourgam0[i01] * yd[0] + hourgam1[i01] * yd[1] +
      hourgam2[i01] * yd[2] + hourgam3[i01] * yd[3] +
      hourgam4[i01] * yd[4] + hourgam5[i01] * yd[5] +
      hourgam6[i01] * yd[6] + hourgam7[i01] * yd[7];

   h02 =
      hourgam0[i02] * yd[0] + hourgam1[i02] * yd[1]+
      hourgam2[i02] * yd[2] + hourgam3[i02] * yd[3]+
      hourgam4[i02] * yd[4] + hourgam5[i02] * yd[5]+
      hourgam6[i02] * yd[6] + hourgam7[i02] * yd[7];

   h03 =
      hourgam0[i03] * yd[0] + hourgam1[i03] * yd[1] +
      hourgam2[i03] * yd[2] + hourgam3[i03] * yd[3] +
      hourgam4[i03] * yd[4] + hourgam5[i03] * yd[5] +
      hourgam6[i03] * yd[6] + hourgam7[i03] * yd[7];


   hgfy[0] = coefficient *
      (hourgam0[i00] * h00 + hourgam0[i01] * h01 +
       hourgam0[i02] * h02 + hourgam0[i03] * h03);

   hgfy[1] = coefficient *
      (hourgam1[i00] * h00 + hourgam1[i01] * h01 +
       hourgam1[i02] * h02 + hourgam1[i03] * h03);

   hgfy[2] = coefficient *
      (hourgam2[i00] * h00 + hourgam2[i01] * h01 +
       hourgam2[i02] * h02 + hourgam2[i03] * h03);

   hgfy[3] = coefficient *
      (hourgam3[i00] * h00 + hourgam3[i01] * h01 +
       hourgam3[i02] * h02 + hourgam3[i03] * h03);

   hgfy[4] = coefficient *
      (hourgam4[i00] * h00 + hourgam4[i01] * h01 +
       hourgam4[i02] * h02 + hourgam4[i03] * h03);

   hgfy[5] = coefficient *
      (hourgam5[i00] * h00 + hourgam5[i01] * h01 +
       hourgam5[i02] * h02 + hourgam5[i03] * h03);

   hgfy[6] = coefficient *
      (hourgam6[i00] * h00 + hourgam6[i01] * h01 +
       hourgam6[i02] * h02 + hourgam6[i03] * h03);

   hgfy[7] = coefficient *
      (hourgam7[i00] * h00 + hourgam7[i01] * h01 +
       hourgam7[i02] * h02 + hourgam7[i03] * h03);

   h00 =
      hourgam0[i00] * zd[0] + hourgam1[i00] * zd[1] +
      hourgam2[i00] * zd[2] + hourgam3[i00] * zd[3] +
      hourgam4[i00] * zd[4] + hourgam5[i00] * zd[5] +
      hourgam6[i00] * zd[6] + hourgam7[i00] * zd[7];

   h01 =
      hourgam0[i01] * zd[0] + hourgam1[i01] * zd[1] +
      hourgam2[i01] * zd[2] + hourgam3[i01] * zd[3] +
      hourgam4[i01] * zd[4] + hourgam5[i01] * zd[5] +
      hourgam6[i01] * zd[6] + hourgam7[i01] * zd[7];

   h02 =
      hourgam0[i02] * zd[0] + hourgam1[i02] * zd[1]+
      hourgam2[i02] * zd[2] + hourgam3[i02] * zd[3]+
      hourgam4[i02] * zd[4] + hourgam5[i02] * zd[5]+
      hourgam6[i02] * zd[6] + hourgam7[i02] * zd[7];

   h03 =
      hourgam0[i03] * zd[0] + hourgam1[i03] * zd[1] +
      hourgam2[i03] * zd[2] + hourgam3[i03] * zd[3] +
      hourgam4[i03] * zd[4] + hourgam5[i03] * zd[5] +
      hourgam6[i03] * zd[6] + hourgam7[i03] * zd[7];


   hgfz[0] = coefficient *
      (hourgam0[i00] * h00 + hourgam0[i01] * h01 +
       hourgam0[i02] * h02 + hourgam0[i03] * h03);

   hgfz[1] = coefficient *
      (hourgam1[i00] * h00 + hourgam1[i01] * h01 +
       hourgam1[i02] * h02 + hourgam1[i03] * h03);

   hgfz[2] = coefficient *
      (hourgam2[i00] * h00 + hourgam2[i01] * h01 +
       hourgam2[i02] * h02 + hourgam2[i03] * h03);

   hgfz[3] = coefficient *
      (hourgam3[i00] * h00 + hourgam3[i01] * h01 +
       hourgam3[i02] * h02 + hourgam3[i03] * h03);

   hgfz[4] = coefficient *
      (hourgam4[i00] * h00 + hourgam4[i01] * h01 +
       hourgam4[i02] * h02 + hourgam4[i03] * h03);

   hgfz[5] = coefficient *
      (hourgam5[i00] * h00 + hourgam5[i01] * h01 +
       hourgam5[i02] * h02 + hourgam5[i03] * h03);

   hgfz[6] = coefficient *
      (hourgam6[i00] * h00 + hourgam6[i01] * h01 +
       hourgam6[i02] * h02 + hourgam6[i03] * h03);

   hgfz[7] = coefficient *
      (hourgam7[i00] * h00 + hourgam7[i01] * h01 +
       hourgam7[i02] * h02 + hourgam7[i03] * h03);
}

static inline
void CalcFBHourglassForceForElems(Real_t *determ,
            Real_t *x8n,      Real_t *y8n,      Real_t *z8n,
            Real_t *dvdx,     Real_t *dvdy,     Real_t *dvdz,
            Real_t hourg)
{
   /*************************************************
    *
    *     FUNCTION: Calculates the Flanagan-Belytschko anti-hourglass
    *               force.
    *
    *************************************************/

   Index_t numElem = mesh.numElem() ;

   Real_t hgfx[8], hgfy[8], hgfz[8] ;

   Real_t coefficient;

   Real_t  gamma[4][8];
   Real_t hourgam0[4], hourgam1[4], hourgam2[4], hourgam3[4] ;
   Real_t hourgam4[4], hourgam5[4], hourgam6[4], hourgam7[4];
   Real_t xd1[8], yd1[8], zd1[8] ;

   gamma[0][0] = Real_t( 1.);
   gamma[0][1] = Real_t( 1.);
   gamma[0][2] = Real_t(-1.);
   gamma[0][3] = Real_t(-1.);
   gamma[0][4] = Real_t(-1.);
   gamma[0][5] = Real_t(-1.);
   gamma[0][6] = Real_t( 1.);
   gamma[0][7] = Real_t( 1.);
   gamma[1][0] = Real_t( 1.);
   gamma[1][1] = Real_t(-1.);
   gamma[1][2] = Real_t(-1.);
   gamma[1][3] = Real_t( 1.);
   gamma[1][4] = Real_t(-1.);
   gamma[1][5] = Real_t( 1.);
   gamma[1][6] = Real_t( 1.);
   gamma[1][7] = Real_t(-1.);
   gamma[2][0] = Real_t( 1.);
   gamma[2][1] = Real_t(-1.);
   gamma[2][2] = Real_t( 1.);
   gamma[2][3] = Real_t(-1.);
   gamma[2][4] = Real_t( 1.);
   gamma[2][5] = Real_t(-1.);
   gamma[2][6] = Real_t( 1.);
   gamma[2][7] = Real_t(-1.);
   gamma[3][0] = Real_t(-1.);
   gamma[3][1] = Real_t( 1.);
   gamma[3][2] = Real_t(-1.);
   gamma[3][3] = Real_t( 1.);
   gamma[3][4] = Real_t( 1.);
   gamma[3][5] = Real_t(-1.);
   gamma[3][6] = Real_t( 1.);
   gamma[3][7] = Real_t(-1.);

/*************************************************/
/*    compute the hourglass modes */


   for(Index_t i2=0;i2<numElem;++i2){
      const Index_t *elemToNode = mesh.nodelist(i2);
      Index_t i3=8*i2;
      Real_t volinv=Real_t(1.0)/determ[i2];
      Real_t ss1, mass1, volume13 ;
      for(Index_t i1=0;i1<4;++i1){

         Real_t hourmodx =
            x8n[i3] * gamma[i1][0] + x8n[i3+1] * gamma[i1][1] +
            x8n[i3+2] * gamma[i1][2] + x8n[i3+3] * gamma[i1][3] +
            x8n[i3+4] * gamma[i1][4] + x8n[i3+5] * gamma[i1][5] +
            x8n[i3+6] * gamma[i1][6] + x8n[i3+7] * gamma[i1][7];

         Real_t hourmody =
            y8n[i3] * gamma[i1][0] + y8n[i3+1] * gamma[i1][1] +
            y8n[i3+2] * gamma[i1][2] + y8n[i3+3] * gamma[i1][3] +
            y8n[i3+4] * gamma[i1][4] + y8n[i3+5] * gamma[i1][5] +
            y8n[i3+6] * gamma[i1][6] + y8n[i3+7] * gamma[i1][7];

         Real_t hourmodz =
            z8n[i3] * gamma[i1][0] + z8n[i3+1] * gamma[i1][1] +
            z8n[i3+2] * gamma[i1][2] + z8n[i3+3] * gamma[i1][3] +
            z8n[i3+4] * gamma[i1][4] + z8n[i3+5] * gamma[i1][5] +
            z8n[i3+6] * gamma[i1][6] + z8n[i3+7] * gamma[i1][7];

         hourgam0[i1] = gamma[i1][0] -  volinv*(dvdx[i3  ] * hourmodx +
                                                  dvdy[i3  ] * hourmody +
                                                  dvdz[i3  ] * hourmodz );

         hourgam1[i1] = gamma[i1][1] -  volinv*(dvdx[i3+1] * hourmodx +
                                                  dvdy[i3+1] * hourmody +
                                                  dvdz[i3+1] * hourmodz );

         hourgam2[i1] = gamma[i1][2] -  volinv*(dvdx[i3+2] * hourmodx +
                                                  dvdy[i3+2] * hourmody +
                                                  dvdz[i3+2] * hourmodz );

         hourgam3[i1] = gamma[i1][3] -  volinv*(dvdx[i3+3] * hourmodx +
                                                  dvdy[i3+3] * hourmody +
                                                  dvdz[i3+3] * hourmodz );

         hourgam4[i1] = gamma[i1][4] -  volinv*(dvdx[i3+4] * hourmodx +
                                                  dvdy[i3+4] * hourmody +
                                                  dvdz[i3+4] * hourmodz );

         hourgam5[i1] = gamma[i1][5] -  volinv*(dvdx[i3+5] * hourmodx +
                                                  dvdy[i3+5] * hourmody +
                                                  dvdz[i3+5] * hourmodz );

         hourgam6[i1] = gamma[i1][6] -  volinv*(dvdx[i3+6] * hourmodx +
                                                  dvdy[i3+6] * hourmody +
                                                  dvdz[i3+6] * hourmodz );

         hourgam7[i1] = gamma[i1][7] -  volinv*(dvdx[i3+7] * hourmodx +
                                                  dvdy[i3+7] * hourmody +
                                                  dvdz[i3+7] * hourmodz );

      }

      /* compute forces */
      /* store forces into h arrays (force arrays) */

      ss1=mesh.ss(i2);
      mass1=mesh.elemMass(i2);
      volume13=CBRT(determ[i2]);

      Index_t n0si2 = elemToNode[0];
      Index_t n1si2 = elemToNode[1];
      Index_t n2si2 = elemToNode[2];
      Index_t n3si2 = elemToNode[3];
      Index_t n4si2 = elemToNode[4];
      Index_t n5si2 = elemToNode[5];
      Index_t n6si2 = elemToNode[6];
      Index_t n7si2 = elemToNode[7];

      xd1[0] = mesh.xd(n0si2);
      xd1[1] = mesh.xd(n1si2);
      xd1[2] = mesh.xd(n2si2);
      xd1[3] = mesh.xd(n3si2);
      xd1[4] = mesh.xd(n4si2);
      xd1[5] = mesh.xd(n5si2);
      xd1[6] = mesh.xd(n6si2);
      xd1[7] = mesh.xd(n7si2);

      yd1[0] = mesh.yd(n0si2);
      yd1[1] = mesh.yd(n1si2);
      yd1[2] = mesh.yd(n2si2);
      yd1[3] = mesh.yd(n3si2);
      yd1[4] = mesh.yd(n4si2);
      yd1[5] = mesh.yd(n5si2);
      yd1[6] = mesh.yd(n6si2);
      yd1[7] = mesh.yd(n7si2);

      zd1[0] = mesh.zd(n0si2);
      zd1[1] = mesh.zd(n1si2);
      zd1[2] = mesh.zd(n2si2);
      zd1[3] = mesh.zd(n3si2);
      zd1[4] = mesh.zd(n4si2);
      zd1[5] = mesh.zd(n5si2);
      zd1[6] = mesh.zd(n6si2);
      zd1[7] = mesh.zd(n7si2);

      coefficient = - hourg * Real_t(0.01) * ss1 * mass1 / volume13;

      CalcElemFBHourglassForce(xd1,yd1,zd1,
                      hourgam0,hourgam1,hourgam2,hourgam3,
                      hourgam4,hourgam5,hourgam6,hourgam7,
                      coefficient, hgfx, hgfy, hgfz);

      mesh.fx(n0si2) += hgfx[0];
      mesh.fy(n0si2) += hgfy[0];
      mesh.fz(n0si2) += hgfz[0];

      mesh.fx(n1si2) += hgfx[1];
      mesh.fy(n1si2) += hgfy[1];
      mesh.fz(n1si2) += hgfz[1];

      mesh.fx(n2si2) += hgfx[2];
      mesh.fy(n2si2) += hgfy[2];
      mesh.fz(n2si2) += hgfz[2];

      mesh.fx(n3si2) += hgfx[3];
      mesh.fy(n3si2) += hgfy[3];
      mesh.fz(n3si2) += hgfz[3];

      mesh.fx(n4si2) += hgfx[4];
      mesh.fy(n4si2) += hgfy[4];
      mesh.fz(n4si2) += hgfz[4];

      mesh.fx(n5si2) += hgfx[5];
      mesh.fy(n5si2) += hgfy[5];
      mesh.fz(n5si2) += hgfz[5];

      mesh.fx(n6si2) += hgfx[6];
      mesh.fy(n6si2) += hgfy[6];
      mesh.fz(n6si2) += hgfz[6];

      mesh.fx(n7si2) += hgfx[7];
      mesh.fy(n7si2) += hgfy[7];
      mesh.fz(n7si2) += hgfz[7];
   }
}

static inline
void CalcHourglassControlForElems(Real_t determ[], Real_t hgcoef)
{
   Index_t i, ii, jj ;
   Real_t  x1[8],  y1[8],  z1[8] ;
   Real_t pfx[8], pfy[8], pfz[8] ;
   Index_t numElem = mesh.numElem() ;
   Index_t numElem8 = numElem * 8 ;
   Real_t *dvdx = Allocate<Real_t>(numElem8) ;
   Real_t *dvdy = Allocate<Real_t>(numElem8) ;
   Real_t *dvdz = Allocate<Real_t>(numElem8) ;
   Real_t *x8n  = Allocate<Real_t>(numElem8) ;
   Real_t *y8n  = Allocate<Real_t>(numElem8) ;
   Real_t *z8n  = Allocate<Real_t>(numElem8) ;

   /* start loop over elements */
   for (i=0 ; i<numElem ; ++i){

      Index_t* elemToNode = mesh.nodelist(i);
      CollectDomainNodesToElemNodes(elemToNode, x1, y1, z1);

      CalcElemVolumeDerivative(pfx, pfy, pfz, x1, y1, z1);

      /* load into temporary storage for FB Hour Glass control */
      for(ii=0;ii<8;++ii){
         jj=8*i+ii;

         dvdx[jj] = pfx[ii];
         dvdy[jj] = pfy[ii];
         dvdz[jj] = pfz[ii];
         x8n[jj]  = x1[ii];
         y8n[jj]  = y1[ii];
         z8n[jj]  = z1[ii];
      }

      determ[i] = mesh.volo(i) * mesh.v(i);

      /* Do a check for negative volumes */
      if ( mesh.v(i) <= Real_t(0.0) ) {
         exit(VolumeError) ;
      }
   }

   if ( hgcoef > Real_t(0.) ) {
      CalcFBHourglassForceForElems(determ,x8n,y8n,z8n,dvdx,dvdy,dvdz,hgcoef) ;
   }

   Release(&z8n) ;
   Release(&y8n) ;
   Release(&x8n) ;
   Release(&dvdz) ;
   Release(&dvdy) ;
   Release(&dvdx) ;

   return ;
}

static inline
void CalcVolumeForceForElems()
{
   Index_t numElem = mesh.numElem() ;
   if (numElem != 0) {
      Real_t  hgcoef = mesh.hgcoef() ;
      Real_t *sigxx  = Allocate<Real_t>(numElem) ;
      Real_t *sigyy  = Allocate<Real_t>(numElem) ;
      Real_t *sigzz  = Allocate<Real_t>(numElem) ;
      Real_t *determ = Allocate<Real_t>(numElem) ;

      /* Sum contributions to total stress tensor */
      InitStressTermsForElems(numElem, sigxx, sigyy, sigzz);

      // call elemlib stress integration loop to produce nodal forces from
      // material stresses.
      IntegrateStressForElems( numElem, sigxx, sigyy, sigzz, determ) ;

      // check for negative element volume
      for ( Index_t k=0 ; k<numElem ; ++k ) {
         if (determ[k] <= Real_t(0.0)) {
            exit(VolumeError) ;
         }
      }

      CalcHourglassControlForElems(determ, hgcoef) ;

      Release(&determ) ;
      Release(&sigzz) ;
      Release(&sigyy) ;
      Release(&sigxx) ;
   }
}

static inline void CalcForceForNodes()
{
  Index_t numNode = mesh.numNode() ;
  for (Index_t i=0; i<numNode; ++i) {
     mesh.fx(i) = Real_t(0.0) ;
     mesh.fy(i) = Real_t(0.0) ;
     mesh.fz(i) = Real_t(0.0) ;
  }

  /* Calcforce calls partial, force, hourq */
  CalcVolumeForceForElems() ;

  /* Calculate Nodal Forces at domain boundaries */
  /* problem->commSBN->Transfer(CommSBN::forces); */

}

static inline
void CalcAccelerationForNodes()
{
   Index_t numNode = mesh.numNode() ;
   for (Index_t i = 0; i < numNode; ++i) {
      mesh.xdd(i) = mesh.fx(i) / mesh.nodalMass(i);
      mesh.ydd(i) = mesh.fy(i) / mesh.nodalMass(i);
      mesh.zdd(i) = mesh.fz(i) / mesh.nodalMass(i);
   }
}

static inline
void ApplyAccelerationBoundaryConditionsForNodes()
{
  Index_t numNodeBC = (mesh.sizeX()+1)*(mesh.sizeX()+1) ;
  for(Index_t i=0 ; i<numNodeBC ; ++i)
     mesh.xdd(mesh.symmX(i)) = Real_t(0.0) ;

  for(Index_t i=0 ; i<numNodeBC ; ++i)
     mesh.ydd(mesh.symmY(i)) = Real_t(0.0) ;

  for(Index_t i=0 ; i<numNodeBC ; ++i)
     mesh.zdd(mesh.symmZ(i)) = Real_t(0.0) ;
}

static inline
void CalcVelocityForNodes(const Real_t dt, const Real_t u_cut)
{
   Index_t numNode = mesh.numNode() ;

   for ( Index_t i = 0 ; i < numNode ; ++i )
   {
     Real_t xdtmp, ydtmp, zdtmp ;

     xdtmp = mesh.xd(i) + mesh.xdd(i) * dt ;
     if( FABS(xdtmp) < u_cut ) xdtmp = Real_t(0.0);
     mesh.xd(i) = xdtmp ;

     ydtmp = mesh.yd(i) + mesh.ydd(i) * dt ;
     if( FABS(ydtmp) < u_cut ) ydtmp = Real_t(0.0);
     mesh.yd(i) = ydtmp ;

     zdtmp = mesh.zd(i) + mesh.zdd(i) * dt ;
     if( FABS(zdtmp) < u_cut ) zdtmp = Real_t(0.0);
     mesh.zd(i) = zdtmp ;
   }
}

static inline
void CalcPositionForNodes(const Real_t dt)
{
   Index_t numNode = mesh.numNode() ;

   for ( Index_t i = 0 ; i < numNode ; ++i )
   {
     mesh.x(i) += mesh.xd(i) * dt ;
     mesh.y(i) += mesh.yd(i) * dt ;
     mesh.z(i) += mesh.zd(i) * dt ;
   }
}

static inline
void LagrangeNodal()
{
  const Real_t delt = mesh.deltatime() ;
  Real_t u_cut = mesh.u_cut() ;

  /* time of boundary condition evaluation is beginning of step for force and
   * acceleration boundary conditions. */
  CalcForceForNodes();

  CalcAccelerationForNodes();

  ApplyAccelerationBoundaryConditionsForNodes();

  CalcVelocityForNodes( delt, u_cut ) ;

  CalcPositionForNodes( delt );

  return;
}

static inline
Real_t CalcElemVolume( const Real_t x0, const Real_t x1,
               const Real_t x2, const Real_t x3,
               const Real_t x4, const Real_t x5,
               const Real_t x6, const Real_t x7,
               const Real_t y0, const Real_t y1,
               const Real_t y2, const Real_t y3,
               const Real_t y4, const Real_t y5,
               const Real_t y6, const Real_t y7,
               const Real_t z0, const Real_t z1,
               const Real_t z2, const Real_t z3,
               const Real_t z4, const Real_t z5,
               const Real_t z6, const Real_t z7 )
{
  Real_t twelveth = Real_t(1.0)/Real_t(12.0);

  Real_t dx61 = x6 - x1;
  Real_t dy61 = y6 - y1;
  Real_t dz61 = z6 - z1;

  Real_t dx70 = x7 - x0;
  Real_t dy70 = y7 - y0;
  Real_t dz70 = z7 - z0;

  Real_t dx63 = x6 - x3;
  Real_t dy63 = y6 - y3;
  Real_t dz63 = z6 - z3;

  Real_t dx20 = x2 - x0;
  Real_t dy20 = y2 - y0;
  Real_t dz20 = z2 - z0;

  Real_t dx50 = x5 - x0;
  Real_t dy50 = y5 - y0;
  Real_t dz50 = z5 - z0;

  Real_t dx64 = x6 - x4;
  Real_t dy64 = y6 - y4;
  Real_t dz64 = z6 - z4;

  Real_t dx31 = x3 - x1;
  Real_t dy31 = y3 - y1;
  Real_t dz31 = z3 - z1;

  Real_t dx72 = x7 - x2;
  Real_t dy72 = y7 - y2;
  Real_t dz72 = z7 - z2;

  Real_t dx43 = x4 - x3;
  Real_t dy43 = y4 - y3;
  Real_t dz43 = z4 - z3;

  Real_t dx57 = x5 - x7;
  Real_t dy57 = y5 - y7;
  Real_t dz57 = z5 - z7;

  Real_t dx14 = x1 - x4;
  Real_t dy14 = y1 - y4;
  Real_t dz14 = z1 - z4;

  Real_t dx25 = x2 - x5;
  Real_t dy25 = y2 - y5;
  Real_t dz25 = z2 - z5;

#define TRIPLE_PRODUCT(x1, y1, z1, x2, y2, z2, x3, y3, z3) \
   ((x1)*((y2)*(z3) - (z2)*(y3)) + (x2)*((z1)*(y3) - (y1)*(z3)) + (x3)*((y1)*(z2) - (z1)*(y2)))

  Real_t volume =
    TRIPLE_PRODUCT(dx31 + dx72, dx63, dx20,
       dy31 + dy72, dy63, dy20,
       dz31 + dz72, dz63, dz20) +
    TRIPLE_PRODUCT(dx43 + dx57, dx64, dx70,
       dy43 + dy57, dy64, dy70,
       dz43 + dz57, dz64, dz70) +
    TRIPLE_PRODUCT(dx14 + dx25, dx61, dx50,
       dy14 + dy25, dy61, dy50,
       dz14 + dz25, dz61, dz50);

#undef TRIPLE_PRODUCT

  volume *= twelveth;

  return volume ;
}

static inline
Real_t CalcElemVolume( const Real_t x[8], const Real_t y[8], const Real_t z[8] )
{
return CalcElemVolume( x[0], x[1], x[2], x[3], x[4], x[5], x[6], x[7],
                       y[0], y[1], y[2], y[3], y[4], y[5], y[6], y[7],
                       z[0], z[1], z[2], z[3], z[4], z[5], z[6], z[7]);
}

static inline
Real_t AreaFace( const Real_t x0, const Real_t x1,
                 const Real_t x2, const Real_t x3,
                 const Real_t y0, const Real_t y1,
                 const Real_t y2, const Real_t y3,
                 const Real_t z0, const Real_t z1,
                 const Real_t z2, const Real_t z3)
{
   Real_t fx = (x2 - x0) - (x3 - x1);
   Real_t fy = (y2 - y0) - (y3 - y1);
   Real_t fz = (z2 - z0) - (z3 - z1);
   Real_t gx = (x2 - x0) + (x3 - x1);
   Real_t gy = (y2 - y0) + (y3 - y1);
   Real_t gz = (z2 - z0) + (z3 - z1);
   Real_t area =
      (fx * fx + fy * fy + fz * fz) *
      (gx * gx + gy * gy + gz * gz) -
      (fx * gx + fy * gy + fz * gz) *
      (fx * gx + fy * gy + fz * gz);
   return area ;
}

static inline
Real_t CalcElemCharacteristicLength( const Real_t x[8],
                                     const Real_t y[8],
                                     const Real_t z[8],
                                     const Real_t volume)
{
   Real_t a, charLength = Real_t(0.0);

   a = AreaFace(x[0],x[1],x[2],x[3],
                y[0],y[1],y[2],y[3],
                z[0],z[1],z[2],z[3]) ;
   charLength = std::max(a,charLength) ;

   a = AreaFace(x[4],x[5],x[6],x[7],
                y[4],y[5],y[6],y[7],
                z[4],z[5],z[6],z[7]) ;
   charLength = std::max(a,charLength) ;

   a = AreaFace(x[0],x[1],x[5],x[4],
                y[0],y[1],y[5],y[4],
                z[0],z[1],z[5],z[4]) ;
   charLength = std::max(a,charLength) ;

   a = AreaFace(x[1],x[2],x[6],x[5],
                y[1],y[2],y[6],y[5],
                z[1],z[2],z[6],z[5]) ;
   charLength = std::max(a,charLength) ;

   a = AreaFace(x[2],x[3],x[7],x[6],
                y[2],y[3],y[7],y[6],
                z[2],z[3],z[7],z[6]) ;
   charLength = std::max(a,charLength) ;

   a = AreaFace(x[3],x[0],x[4],x[7],
                y[3],y[0],y[4],y[7],
                z[3],z[0],z[4],z[7]) ;
   charLength = std::max(a,charLength) ;

   charLength = Real_t(4.0) * volume / SQRT(charLength);

   return charLength;
}

static inline
void CalcElemVelocityGrandient( const Real_t* const xvel,
                                const Real_t* const yvel,
                                const Real_t* const zvel,
                                const Real_t b[][8],
                                const Real_t detJ,
                                Real_t* const d )
{
  const Real_t inv_detJ = Real_t(1.0) / detJ ;
  Real_t dyddx, dxddy, dzddx, dxddz, dzddy, dyddz;
  const Real_t* const pfx = b[0];
  const Real_t* const pfy = b[1];
  const Real_t* const pfz = b[2];

  d[0] = inv_detJ * ( pfx[0] * (xvel[0]-xvel[6])
                     + pfx[1] * (xvel[1]-xvel[7])
                     + pfx[2] * (xvel[2]-xvel[4])
                     + pfx[3] * (xvel[3]-xvel[5]) );

  d[1] = inv_detJ * ( pfy[0] * (yvel[0]-yvel[6])
                     + pfy[1] * (yvel[1]-yvel[7])
                     + pfy[2] * (yvel[2]-yvel[4])
                     + pfy[3] * (yvel[3]-yvel[5]) );

  d[2] = inv_detJ * ( pfz[0] * (zvel[0]-zvel[6])
                     + pfz[1] * (zvel[1]-zvel[7])
                     + pfz[2] * (zvel[2]-zvel[4])
                     + pfz[3] * (zvel[3]-zvel[5]) );

  dyddx  = inv_detJ * ( pfx[0] * (yvel[0]-yvel[6])
                      + pfx[1] * (yvel[1]-yvel[7])
                      + pfx[2] * (yvel[2]-yvel[4])
                      + pfx[3] * (yvel[3]-yvel[5]) );

  dxddy  = inv_detJ * ( pfy[0] * (xvel[0]-xvel[6])
                      + pfy[1] * (xvel[1]-xvel[7])
                      + pfy[2] * (xvel[2]-xvel[4])
                      + pfy[3] * (xvel[3]-xvel[5]) );

  dzddx  = inv_detJ * ( pfx[0] * (zvel[0]-zvel[6])
                      + pfx[1] * (zvel[1]-zvel[7])
                      + pfx[2] * (zvel[2]-zvel[4])
                      + pfx[3] * (zvel[3]-zvel[5]) );

  dxddz  = inv_detJ * ( pfz[0] * (xvel[0]-xvel[6])
                      + pfz[1] * (xvel[1]-xvel[7])
                      + pfz[2] * (xvel[2]-xvel[4])
                      + pfz[3] * (xvel[3]-xvel[5]) );

  dzddy  = inv_detJ * ( pfy[0] * (zvel[0]-zvel[6])
                      + pfy[1] * (zvel[1]-zvel[7])
                      + pfy[2] * (zvel[2]-zvel[4])
                      + pfy[3] * (zvel[3]-zvel[5]) );

  dyddz  = inv_detJ * ( pfz[0] * (yvel[0]-yvel[6])
                      + pfz[1] * (yvel[1]-yvel[7])
                      + pfz[2] * (yvel[2]-yvel[4])
                      + pfz[3] * (yvel[3]-yvel[5]) );
  d[5]  = Real_t( .5) * ( dxddy + dyddx );
  d[4]  = Real_t( .5) * ( dxddz + dzddx );
  d[3]  = Real_t( .5) * ( dzddy + dyddz );
}

static inline
void CalcKinematicsForElems( Index_t numElem, Real_t dt )
{
  Real_t B[3][8] ; /** shape function derivatives */
  Real_t D[6] ;
  Real_t x_local[8] ;
  Real_t y_local[8] ;
  Real_t z_local[8] ;
  Real_t xd_local[8] ;
  Real_t yd_local[8] ;
  Real_t zd_local[8] ;
  Real_t detJ = Real_t(0.0) ;

  // loop over all elements
  for( Index_t k=0 ; k<numElem ; ++k )
  {
    Real_t volume ;
    Real_t relativeVolume ;
    const Index_t* const elemToNode = mesh.nodelist(k) ;

    // get nodal coordinates from global arrays and copy into local arrays.
    for( Index_t lnode=0 ; lnode<8 ; ++lnode )
    {
      Index_t gnode = elemToNode[lnode];
      x_local[lnode] = mesh.x(gnode);
      y_local[lnode] = mesh.y(gnode);
      z_local[lnode] = mesh.z(gnode);
    }

    // volume calculations
    volume = CalcElemVolume(x_local, y_local, z_local );
    relativeVolume = volume / mesh.volo(k) ;
    mesh.vnew(k) = relativeVolume ;
    mesh.delv(k) = relativeVolume - mesh.v(k) ;

    // set characteristic length
    mesh.arealg(k) = CalcElemCharacteristicLength(x_local,
                                                  y_local,
                                                  z_local,
                                                  volume);

    // get nodal velocities from global array and copy into local arrays.
    for( Index_t lnode=0 ; lnode<8 ; ++lnode )
    {
      Index_t gnode = elemToNode[lnode];
      xd_local[lnode] = mesh.xd(gnode);
      yd_local[lnode] = mesh.yd(gnode);
      zd_local[lnode] = mesh.zd(gnode);
    }

    Real_t dt2 = Real_t(0.5) * dt;
    for ( Index_t j=0 ; j<8 ; ++j )
    {
       x_local[j] -= dt2 * xd_local[j];
       y_local[j] -= dt2 * yd_local[j];
       z_local[j] -= dt2 * zd_local[j];
    }

    CalcElemShapeFunctionDerivatives( x_local,
                                          y_local,
                                          z_local,
                                          B, &detJ );

    CalcElemVelocityGrandient( xd_local,
                               yd_local,
                               zd_local,
                               B, detJ, D );

    // put velocity gradient quantities into their global arrays.
    mesh.dxx(k) = D[0];
    mesh.dyy(k) = D[1];
    mesh.dzz(k) = D[2];
  }
}

static inline
void CalcLagrangeElements(Real_t deltatime)
{
   Index_t numElem = mesh.numElem() ;
   if (numElem > 0) {
      // set element connectivity array as a single dimension array. It is
      // assumed that the array will be of length numelems*numnodesperelem.

      CalcKinematicsForElems(numElem, deltatime) ;

      // element loop to do some stuff not included in the elemlib function.
      for ( Index_t k=0 ; k<numElem ; ++k )
      {
        // calc strain rate and apply as constraint (only done in FB element)
        Real_t vdov = mesh.dxx(k) + mesh.dyy(k) + mesh.dzz(k) ;
        Real_t vdovthird = vdov/Real_t(3.0) ;
        
        // make the rate of deformation tensor deviatoric
        mesh.vdov(k) = vdov ;
        mesh.dxx(k) -= vdovthird ;
        mesh.dyy(k) -= vdovthird ;
        mesh.dzz(k) -= vdovthird ;

        // See if any volumes are negative, and take appropriate action.
        if (mesh.vnew(k) <= Real_t(0.0))
        {
           exit(VolumeError) ;
        }
      }
   }
}

static inline
void CalcMonotonicQGradientsForElems()
{
#define SUM4(a,b,c,d) (a + b + c + d)
   Index_t numElem = mesh.numElem() ;
   const Real_t ptiny = Real_t(1.e-36) ;

   for (Index_t i = 0 ; i < numElem ; ++i ) {
      Real_t ax,ay,az ;
      Real_t dxv,dyv,dzv ;

      const Index_t *elemToNode = mesh.nodelist(i);
      Index_t n0 = elemToNode[0] ;
      Index_t n1 = elemToNode[1] ;
      Index_t n2 = elemToNode[2] ;
      Index_t n3 = elemToNode[3] ;
      Index_t n4 = elemToNode[4] ;
      Index_t n5 = elemToNode[5] ;
      Index_t n6 = elemToNode[6] ;
      Index_t n7 = elemToNode[7] ;

      Real_t x0 = mesh.x(n0) ;
      Real_t x1 = mesh.x(n1) ;
      Real_t x2 = mesh.x(n2) ;
      Real_t x3 = mesh.x(n3) ;
      Real_t x4 = mesh.x(n4) ;
      Real_t x5 = mesh.x(n5) ;
      Real_t x6 = mesh.x(n6) ;
      Real_t x7 = mesh.x(n7) ;

      Real_t y0 = mesh.y(n0) ;
      Real_t y1 = mesh.y(n1) ;
      Real_t y2 = mesh.y(n2) ;
      Real_t y3 = mesh.y(n3) ;
      Real_t y4 = mesh.y(n4) ;
      Real_t y5 = mesh.y(n5) ;
      Real_t y6 = mesh.y(n6) ;
      Real_t y7 = mesh.y(n7) ;

      Real_t z0 = mesh.z(n0) ;
      Real_t z1 = mesh.z(n1) ;
      Real_t z2 = mesh.z(n2) ;
      Real_t z3 = mesh.z(n3) ;
      Real_t z4 = mesh.z(n4) ;
      Real_t z5 = mesh.z(n5) ;
      Real_t z6 = mesh.z(n6) ;
      Real_t z7 = mesh.z(n7) ;

      Real_t xv0 = mesh.xd(n0) ;
      Real_t xv1 = mesh.xd(n1) ;
      Real_t xv2 = mesh.xd(n2) ;
      Real_t xv3 = mesh.xd(n3) ;
      Real_t xv4 = mesh.xd(n4) ;
      Real_t xv5 = mesh.xd(n5) ;
      Real_t xv6 = mesh.xd(n6) ;
      Real_t xv7 = mesh.xd(n7) ;

      Real_t yv0 = mesh.yd(n0) ;
      Real_t yv1 = mesh.yd(n1) ;
      Real_t yv2 = mesh.yd(n2) ;
      Real_t yv3 = mesh.yd(n3) ;
      Real_t yv4 = mesh.yd(n4) ;
      Real_t yv5 = mesh.yd(n5) ;
      Real_t yv6 = mesh.yd(n6) ;
      Real_t yv7 = mesh.yd(n7) ;

      Real_t zv0 = mesh.zd(n0) ;
      Real_t zv1 = mesh.zd(n1) ;
      Real_t zv2 = mesh.zd(n2) ;
      Real_t zv3 = mesh.zd(n3) ;
      Real_t zv4 = mesh.zd(n4) ;
      Real_t zv5 = mesh.zd(n5) ;
      Real_t zv6 = mesh.zd(n6) ;
      Real_t zv7 = mesh.zd(n7) ;

      Real_t vol = mesh.volo(i)*mesh.vnew(i) ;
      Real_t norm = Real_t(1.0) / ( vol + ptiny ) ;

      Real_t dxj = Real_t(-0.25)*(SUM4(x0,x1,x5,x4) - SUM4(x3,x2,x6,x7)) ;
      Real_t dyj = Real_t(-0.25)*(SUM4(y0,y1,y5,y4) - SUM4(y3,y2,y6,y7)) ;
      Real_t dzj = Real_t(-0.25)*(SUM4(z0,z1,z5,z4) - SUM4(z3,z2,z6,z7)) ;

      Real_t dxi = Real_t( 0.25)*(SUM4(x1,x2,x6,x5) - SUM4(x0,x3,x7,x4)) ;
      Real_t dyi = Real_t( 0.25)*(SUM4(y1,y2,y6,y5) - SUM4(y0,y3,y7,y4)) ;
      Real_t dzi = Real_t( 0.25)*(SUM4(z1,z2,z6,z5) - SUM4(z0,z3,z7,z4)) ;

      Real_t dxk = Real_t( 0.25)*(SUM4(x4,x5,x6,x7) - SUM4(x0,x1,x2,x3)) ;
      Real_t dyk = Real_t( 0.25)*(SUM4(y4,y5,y6,y7) - SUM4(y0,y1,y2,y3)) ;
      Real_t dzk = Real_t( 0.25)*(SUM4(z4,z5,z6,z7) - SUM4(z0,z1,z2,z3)) ;

      /* find delvk and delxk ( i cross j ) */

      ax = dyi*dzj - dzi*dyj ;
      ay = dzi*dxj - dxi*dzj ;
      az = dxi*dyj - dyi*dxj ;

      mesh.delx_zeta(i) = vol / SQRT(ax*ax + ay*ay + az*az + ptiny) ;

      ax *= norm ;
      ay *= norm ;
      az *= norm ;

      dxv = Real_t(0.25)*(SUM4(xv4,xv5,xv6,xv7) - SUM4(xv0,xv1,xv2,xv3)) ;
      dyv = Real_t(0.25)*(SUM4(yv4,yv5,yv6,yv7) - SUM4(yv0,yv1,yv2,yv3)) ;
      dzv = Real_t(0.25)*(SUM4(zv4,zv5,zv6,zv7) - SUM4(zv0,zv1,zv2,zv3)) ;

      mesh.delv_zeta(i) = ax*dxv + ay*dyv + az*dzv ;

      /* find delxi and delvi ( j cross k ) */

      ax = dyj*dzk - dzj*dyk ;
      ay = dzj*dxk - dxj*dzk ;
      az = dxj*dyk - dyj*dxk ;

      mesh.delx_xi(i) = vol / SQRT(ax*ax + ay*ay + az*az + ptiny) ;

      ax *= norm ;
      ay *= norm ;
      az *= norm ;

      dxv = Real_t(0.25)*(SUM4(xv1,xv2,xv6,xv5) - SUM4(xv0,xv3,xv7,xv4)) ;
      dyv = Real_t(0.25)*(SUM4(yv1,yv2,yv6,yv5) - SUM4(yv0,yv3,yv7,yv4)) ;
      dzv = Real_t(0.25)*(SUM4(zv1,zv2,zv6,zv5) - SUM4(zv0,zv3,zv7,zv4)) ;

      mesh.delv_xi(i) = ax*dxv + ay*dyv + az*dzv ;

      /* find delxj and delvj ( k cross i ) */

      ax = dyk*dzi - dzk*dyi ;
      ay = dzk*dxi - dxk*dzi ;
      az = dxk*dyi - dyk*dxi ;

      mesh.delx_eta(i) = vol / SQRT(ax*ax + ay*ay + az*az + ptiny) ;

      ax *= norm ;
      ay *= norm ;
      az *= norm ;

      dxv = Real_t(-0.25)*(SUM4(xv0,xv1,xv5,xv4) - SUM4(xv3,xv2,xv6,xv7)) ;
      dyv = Real_t(-0.25)*(SUM4(yv0,yv1,yv5,yv4) - SUM4(yv3,yv2,yv6,yv7)) ;
      dzv = Real_t(-0.25)*(SUM4(zv0,zv1,zv5,zv4) - SUM4(zv3,zv2,zv6,zv7)) ;

      mesh.delv_eta(i) = ax*dxv + ay*dyv + az*dzv ;
   }
#undef SUM4
}

static inline
void CalcMonotonicQRegionForElems(// parameters
                          Real_t qlc_monoq,
                          Real_t qqc_monoq,
                          Real_t monoq_limiter_mult,
                          Real_t monoq_max_slope,
                          Real_t ptiny,

                          // the elementset length
                          Index_t elength )
{
   for ( Index_t ielem = 0 ; ielem < elength; ++ielem ) {
      Real_t qlin, qquad ;
      Real_t phixi, phieta, phizeta ;
      Index_t i = mesh.matElemlist(ielem);
      Int_t bcMask = mesh.elemBC(i) ;
      Real_t delvm, delvp ;

      /*  phixi     */
      Real_t norm = Real_t(1.) / ( mesh.delv_xi(i) + ptiny ) ;

      switch (bcMask & XI_M) {
         case 0:         delvm = mesh.delv_xi(mesh.lxim(i)) ; break ;
         case XI_M_SYMM: delvm = mesh.delv_xi(i) ;            break ;
         case XI_M_FREE: delvm = Real_t(0.0) ;                break ;
         default:        /* ERROR */ ;                        break ;
      }
      switch (bcMask & XI_P) {
         case 0:         delvp = mesh.delv_xi(mesh.lxip(i)) ; break ;
         case XI_P_SYMM: delvp = mesh.delv_xi(i) ;            break ;
         case XI_P_FREE: delvp = Real_t(0.0) ;                break ;
         default:        /* ERROR */ ;                        break ;
      }

      delvm = delvm * norm ;
      delvp = delvp * norm ;

      phixi = Real_t(.5) * ( delvm + delvp ) ;

      delvm *= monoq_limiter_mult ;
      delvp *= monoq_limiter_mult ;

      if ( delvm < phixi ) phixi = delvm ;
      if ( delvp < phixi ) phixi = delvp ;
      if ( phixi < Real_t(0.)) phixi = Real_t(0.) ;
      if ( phixi > monoq_max_slope) phixi = monoq_max_slope;


      /*  phieta     */
      norm = Real_t(1.) / ( mesh.delv_eta(i) + ptiny ) ;

      switch (bcMask & ETA_M) {
         case 0:          delvm = mesh.delv_eta(mesh.letam(i)) ; break ;
         case ETA_M_SYMM: delvm = mesh.delv_eta(i) ;             break ;
         case ETA_M_FREE: delvm = Real_t(0.0) ;                  break ;
         default:         /* ERROR */ ;                          break ;
      }
      switch (bcMask & ETA_P) {
         case 0:          delvp = mesh.delv_eta(mesh.letap(i)) ; break ;
         case ETA_P_SYMM: delvp = mesh.delv_eta(i) ;             break ;
         case ETA_P_FREE: delvp = Real_t(0.0) ;                  break ;
         default:         /* ERROR */ ;                          break ;
      }

      delvm = delvm * norm ;
      delvp = delvp * norm ;

      phieta = Real_t(.5) * ( delvm + delvp ) ;

      delvm *= monoq_limiter_mult ;
      delvp *= monoq_limiter_mult ;

      if ( delvm  < phieta ) phieta = delvm ;
      if ( delvp  < phieta ) phieta = delvp ;
      if ( phieta < Real_t(0.)) phieta = Real_t(0.) ;
      if ( phieta > monoq_max_slope)  phieta = monoq_max_slope;

      /*  phizeta     */
      norm = Real_t(1.) / ( mesh.delv_zeta(i) + ptiny ) ;

      switch (bcMask & ZETA_M) {
         case 0:           delvm = mesh.delv_zeta(mesh.lzetam(i)) ; break ;
         case ZETA_M_SYMM: delvm = mesh.delv_zeta(i) ;              break ;
         case ZETA_M_FREE: delvm = Real_t(0.0) ;                    break ;
         default:          /* ERROR */ ;                            break ;
      }
      switch (bcMask & ZETA_P) {
         case 0:           delvp = mesh.delv_zeta(mesh.lzetap(i)) ; break ;
         case ZETA_P_SYMM: delvp = mesh.delv_zeta(i) ;              break ;
         case ZETA_P_FREE: delvp = Real_t(0.0) ;                    break ;
         default:          /* ERROR */ ;                            break ;
      }

      delvm = delvm * norm ;
      delvp = delvp * norm ;

      phizeta = Real_t(.5) * ( delvm + delvp ) ;

      delvm *= monoq_limiter_mult ;
      delvp *= monoq_limiter_mult ;

      if ( delvm   < phizeta ) phizeta = delvm ;
      if ( delvp   < phizeta ) phizeta = delvp ;
      if ( phizeta < Real_t(0.)) phizeta = Real_t(0.);
      if ( phizeta > monoq_max_slope  ) phizeta = monoq_max_slope;

      /* Remove length scale */

      if ( mesh.vdov(i) > Real_t(0.) )  {
         qlin  = Real_t(0.) ;
         qquad = Real_t(0.) ;
      }
      else {
         Real_t delvxxi   = mesh.delv_xi(i)   * mesh.delx_xi(i)   ;
         Real_t delvxeta  = mesh.delv_eta(i)  * mesh.delx_eta(i)  ;
         Real_t delvxzeta = mesh.delv_zeta(i) * mesh.delx_zeta(i) ;

         if ( delvxxi   > Real_t(0.) ) delvxxi   = Real_t(0.) ;
         if ( delvxeta  > Real_t(0.) ) delvxeta  = Real_t(0.) ;
         if ( delvxzeta > Real_t(0.) ) delvxzeta = Real_t(0.) ;

         Real_t rho = mesh.elemMass(i) / (mesh.volo(i) * mesh.vnew(i)) ;

         qlin = -qlc_monoq * rho *
            (  delvxxi   * (Real_t(1.) - phixi) +
               delvxeta  * (Real_t(1.) - phieta) +
               delvxzeta * (Real_t(1.) - phizeta)  ) ;

         qquad = qqc_monoq * rho *
            (  delvxxi*delvxxi     * (Real_t(1.) - phixi*phixi) +
               delvxeta*delvxeta   * (Real_t(1.) - phieta*phieta) +
               delvxzeta*delvxzeta * (Real_t(1.) - phizeta*phizeta)  ) ;
      }

      mesh.qq(i) = qquad ;
      mesh.ql(i) = qlin  ;
   }
}

static inline
void CalcMonotonicQForElems()
{  
   //
   // initialize parameters
   // 
   const Real_t ptiny        = Real_t(1.e-36) ;
   Real_t monoq_max_slope    = mesh.monoq_max_slope() ;
   Real_t monoq_limiter_mult = mesh.monoq_limiter_mult() ;

   //
   // calculate the monotonic q for pure regions
   //
   Index_t elength = mesh.numElem() ;
   if (elength > 0) {
      Real_t qlc_monoq = mesh.qlc_monoq();
      Real_t qqc_monoq = mesh.qqc_monoq();
      CalcMonotonicQRegionForElems(// parameters
                           qlc_monoq,
                           qqc_monoq,
                           monoq_limiter_mult,
                           monoq_max_slope,
                           ptiny,

                           // the elemset length
                           elength );
   }
}

static inline
void CalcQForElems()
{
   Real_t qstop = mesh.qstop() ;
   Index_t numElem = mesh.numElem() ;

   //
   // MONOTONIC Q option
   //

   /* Calculate velocity gradients */
   CalcMonotonicQGradientsForElems() ;

   /* Transfer veloctiy gradients in the first order elements */
   /* problem->commElements->Transfer(CommElements::monoQ) ; */
   CalcMonotonicQForElems() ;

   /* Don't allow excessive artificial viscosity */
   if (numElem != 0) {
      Index_t idx = -1; 
      for (Index_t i=0; i<numElem; ++i) {
         if ( mesh.q(i) > qstop ) {
            idx = i ;
            break ;
         }
      }

      if(idx >= 0) {
         exit(QStopError) ;
      }
   }
}

static inline
void CalcPressureForElems(Real_t* p_new, Real_t* bvc,
                          Real_t* pbvc, Real_t* e_old,
                          Real_t* compression, Real_t *vnewc,
                          Real_t pmin,
                          Real_t p_cut, Real_t eosvmax,
                          Index_t length)
{
   Real_t c1s = Real_t(2.0)/Real_t(3.0) ;
   for (Index_t i = 0; i < length ; ++i) {
      bvc[i] = c1s * (compression[i] + Real_t(1.));
      pbvc[i] = c1s;
   }

   for (Index_t i = 0 ; i < length ; ++i){
      p_new[i] = bvc[i] * e_old[i] ;

      if    (FABS(p_new[i]) <  p_cut   )
         p_new[i] = Real_t(0.0) ;

      if    ( vnewc[i] >= eosvmax ) /* impossible condition here? */
         p_new[i] = Real_t(0.0) ;

      if    (p_new[i]       <  pmin)
         p_new[i]   = pmin ;
   }
}

static inline
void CalcEnergyForElems(Real_t* p_new, Real_t* e_new, Real_t* q_new,
                        Real_t* bvc, Real_t* pbvc,
                        Real_t* p_old, Real_t* e_old, Real_t* q_old,
                        Real_t* compression, Real_t* compHalfStep,
                        Real_t* vnewc, Real_t* work, Real_t* delvc, Real_t pmin,
                        Real_t p_cut, Real_t  e_cut, Real_t q_cut, Real_t emin,
                        Real_t* qq, Real_t* ql,
                        Real_t rho0,
                        Real_t eosvmax,
                        Index_t length)
{
   const Real_t sixth = Real_t(1.0) / Real_t(6.0) ;
   Real_t *pHalfStep = Allocate<Real_t>(length) ;

   for (Index_t i = 0 ; i < length ; ++i) {
      e_new[i] = e_old[i] - Real_t(0.5) * delvc[i] * (p_old[i] + q_old[i])
         + Real_t(0.5) * work[i];

      if (e_new[i]  < emin ) {
         e_new[i] = emin ;
      }
   }

   CalcPressureForElems(pHalfStep, bvc, pbvc, e_new, compHalfStep, vnewc,
                   pmin, p_cut, eosvmax, length);

   for (Index_t i = 0 ; i < length ; ++i) {
      Real_t vhalf = Real_t(1.) / (Real_t(1.) + compHalfStep[i]) ;

      if ( delvc[i] > Real_t(0.) ) {
         q_new[i] /* = qq[i] = ql[i] */ = Real_t(0.) ;
      }
      else {
         Real_t ssc = ( pbvc[i] * e_new[i]
                 + vhalf * vhalf * bvc[i] * pHalfStep[i] ) / rho0 ;

         if ( ssc <= Real_t(.1111111e-36) ) {
            ssc = Real_t(.3333333e-18) ;
         } else {
            ssc = SQRT(ssc) ;
         }

         q_new[i] = (ssc*ql[i] + qq[i]) ;
      }

      e_new[i] = e_new[i] + Real_t(0.5) * delvc[i]
         * (  Real_t(3.0)*(p_old[i]     + q_old[i])
              - Real_t(4.0)*(pHalfStep[i] + q_new[i])) ;
   }

   for (Index_t i = 0 ; i < length ; ++i) {

      e_new[i] += Real_t(0.5) * work[i];

      if (FABS(e_new[i]) < e_cut) {
         e_new[i] = Real_t(0.)  ;
      }
      if (     e_new[i]  < emin ) {
         e_new[i] = emin ;
      }
   }

   CalcPressureForElems(p_new, bvc, pbvc, e_new, compression, vnewc,
                   pmin, p_cut, eosvmax, length);

   for (Index_t i = 0 ; i < length ; ++i){
      Real_t q_tilde ;

      if (delvc[i] > Real_t(0.)) {
         q_tilde = Real_t(0.) ;
      }
      else {
         Real_t ssc = ( pbvc[i] * e_new[i]
                 + vnewc[i] * vnewc[i] * bvc[i] * p_new[i] ) / rho0 ;

         if ( ssc <= Real_t(.1111111e-36) ) {
            ssc = Real_t(.3333333e-18) ;
         } else {
            ssc = SQRT(ssc) ;
         }

         q_tilde = (ssc*ql[i] + qq[i]) ;
      }

      e_new[i] = e_new[i] - (  Real_t(7.0)*(p_old[i]     + q_old[i])
                               - Real_t(8.0)*(pHalfStep[i] + q_new[i])
                               + (p_new[i] + q_tilde)) * delvc[i]*sixth ;

      if (FABS(e_new[i]) < e_cut) {
         e_new[i] = Real_t(0.)  ;
      }
      if (     e_new[i]  < emin ) {
         e_new[i] = emin ;
      }
   }

   CalcPressureForElems(p_new, bvc, pbvc, e_new, compression, vnewc,
                   pmin, p_cut, eosvmax, length);

   for (Index_t i = 0 ; i < length ; ++i){

      if ( delvc[i] <= Real_t(0.) ) {
         Real_t ssc = ( pbvc[i] * e_new[i]
                 + vnewc[i] * vnewc[i] * bvc[i] * p_new[i] ) / rho0 ;

         if ( ssc <= Real_t(.1111111e-36) ) {
            ssc = Real_t(.3333333e-18) ;
         } else {
            ssc = SQRT(ssc) ;
         }

         q_new[i] = (ssc*ql[i] + qq[i]) ;

         if (FABS(q_new[i]) < q_cut) q_new[i] = Real_t(0.) ;
      }
   }

   Release(&pHalfStep) ;

   return ;
}

static inline
void CalcSoundSpeedForElems(Real_t *vnewc, Real_t rho0, Real_t *enewc,
                            Real_t *pnewc, Real_t *pbvc,
                            Real_t *bvc, Real_t ss4o3, Index_t nz)
{
   for (Index_t i = 0; i < nz ; ++i) {
      Index_t iz = mesh.matElemlist(i);
      Real_t ssTmp = (pbvc[i] * enewc[i] + vnewc[i] * vnewc[i] *
                 bvc[i] * pnewc[i]) / rho0;
      if (ssTmp <= Real_t(.1111111e-36)) {
         ssTmp = Real_t(.3333333e-18);
      }
      else {
         ssTmp = SQRT(ssTmp) ;
      }
      mesh.ss(iz) = ssTmp;
   }
}

static inline
void EvalEOSForElems(Real_t *vnewc, Index_t length)
{
   Real_t  e_cut = mesh.e_cut();
   Real_t  p_cut = mesh.p_cut();
   Real_t  ss4o3 = mesh.ss4o3();
   Real_t  q_cut = mesh.q_cut();

   Real_t eosvmax = mesh.eosvmax() ;
   Real_t eosvmin = mesh.eosvmin() ;
   Real_t pmin    = mesh.pmin() ;
   Real_t emin    = mesh.emin() ;
   Real_t rho0    = mesh.refdens() ;

   Real_t *e_old = Allocate<Real_t>(length) ;
   Real_t *delvc = Allocate<Real_t>(length) ;
   Real_t *p_old = Allocate<Real_t>(length) ;
   Real_t *q_old = Allocate<Real_t>(length) ;
   Real_t *compression = Allocate<Real_t>(length) ;
   Real_t *compHalfStep = Allocate<Real_t>(length) ;
   Real_t *qq = Allocate<Real_t>(length) ;
   Real_t *ql = Allocate<Real_t>(length) ;
   Real_t *work = Allocate<Real_t>(length) ;
   Real_t *p_new = Allocate<Real_t>(length) ;
   Real_t *e_new = Allocate<Real_t>(length) ;
   Real_t *q_new = Allocate<Real_t>(length) ;
   Real_t *bvc = Allocate<Real_t>(length) ;
   Real_t *pbvc = Allocate<Real_t>(length) ;

   /* compress data, minimal set */
   for (Index_t i=0; i<length; ++i) {
      Index_t zidx = mesh.matElemlist(i) ;
      e_old[i] = mesh.e(zidx) ;
   }

   for (Index_t i=0; i<length; ++i) {
      Index_t zidx = mesh.matElemlist(i) ;
      delvc[i] = mesh.delv(zidx) ;
   }

   for (Index_t i=0; i<length; ++i) {
      Index_t zidx = mesh.matElemlist(i) ;
      p_old[i] = mesh.p(zidx) ;
   }

   for (Index_t i=0; i<length; ++i) {
      Index_t zidx = mesh.matElemlist(i) ;
      q_old[i] = mesh.q(zidx) ;
   }

   for (Index_t i = 0; i < length ; ++i) {
      Real_t vchalf ;
      compression[i] = Real_t(1.) / vnewc[i] - Real_t(1.);
      vchalf = vnewc[i] - delvc[i] * Real_t(.5);
      compHalfStep[i] = Real_t(1.) / vchalf - Real_t(1.);
   }

   /* Check for v > eosvmax or v < eosvmin */
   if ( eosvmin != Real_t(0.) ) {
      for(Index_t i=0 ; i<length ; ++i) {
         if (vnewc[i] <= eosvmin) { /* impossible due to calling func? */
            compHalfStep[i] = compression[i] ;
         }
      }
   }
   if ( eosvmax != Real_t(0.) ) {
      for(Index_t i=0 ; i<length ; ++i) {
         if (vnewc[i] >= eosvmax) { /* impossible due to calling func? */
            p_old[i]        = Real_t(0.) ;
            compression[i]  = Real_t(0.) ;
            compHalfStep[i] = Real_t(0.) ;
         }
      }
   }

   for (Index_t i = 0 ; i < length ; ++i) {
      Index_t zidx = mesh.matElemlist(i) ;
      qq[i] = mesh.qq(zidx) ;
      ql[i] = mesh.ql(zidx) ;
      work[i] = Real_t(0.) ; 
   }

   CalcEnergyForElems(p_new, e_new, q_new, bvc, pbvc,
                 p_old, e_old,  q_old, compression, compHalfStep,
                 vnewc, work,  delvc, pmin,
                 p_cut, e_cut, q_cut, emin,
                 qq, ql, rho0, eosvmax, length);


   for (Index_t i=0; i<length; ++i) {
      Index_t zidx = mesh.matElemlist(i) ;
      mesh.p(zidx) = p_new[i] ;
   }

   for (Index_t i=0; i<length; ++i) {
      Index_t zidx = mesh.matElemlist(i) ;
      mesh.e(zidx) = e_new[i] ;
   }

   for (Index_t i=0; i<length; ++i) {
      Index_t zidx = mesh.matElemlist(i) ;
      mesh.q(zidx) = q_new[i] ;
   }

   CalcSoundSpeedForElems(vnewc, rho0, e_new, p_new,
             pbvc, bvc, ss4o3, length) ;

   Release(&pbvc) ;
   Release(&bvc) ;
   Release(&q_new) ;
   Release(&e_new) ;
   Release(&p_new) ;
   Release(&work) ;
   Release(&ql) ;
   Release(&qq) ;
   Release(&compHalfStep) ;
   Release(&compression) ;
   Release(&q_old) ;
   Release(&p_old) ;
   Release(&delvc) ;
   Release(&e_old) ;
}

static inline
void ApplyMaterialPropertiesForElems()
{
  Index_t length = mesh.numElem() ;

  if (length != 0) {
    /* Expose all of the variables needed for material evaluation */
    Real_t eosvmin = mesh.eosvmin() ;
    Real_t eosvmax = mesh.eosvmax() ;
    Real_t *vnewc = Allocate<Real_t>(length) ;

    for (Index_t i=0 ; i<length ; ++i) {
       Index_t zn = mesh.matElemlist(i) ;
       vnewc[i] = mesh.vnew(zn) ;
    }

    if (eosvmin != Real_t(0.)) {
       for(Index_t i=0 ; i<length ; ++i) {
          if (vnewc[i] < eosvmin)
             vnewc[i] = eosvmin ;
       }
    }

    if (eosvmax != Real_t(0.)) {
       for(Index_t i=0 ; i<length ; ++i) {
          if (vnewc[i] > eosvmax)
             vnewc[i] = eosvmax ;
       }
    }

    for (Index_t i=0; i<length; ++i) {
       Index_t zn = mesh.matElemlist(i) ;
       Real_t vc = mesh.v(zn) ;
       if (eosvmin != Real_t(0.)) {
          if (vc < eosvmin)
             vc = eosvmin ;
       }
       if (eosvmax != Real_t(0.)) {
          if (vc > eosvmax)
             vc = eosvmax ;
       }
       if (vc <= 0.) {
          exit(VolumeError) ;
       }
    }

    EvalEOSForElems(vnewc, length);

    Release(&vnewc) ;

  }
}

static inline
void UpdateVolumesForElems()
{
   Index_t numElem = mesh.numElem();
   if (numElem != 0) {
      Real_t v_cut = mesh.v_cut();

      for(Index_t i=0 ; i<numElem ; ++i) {
         Real_t tmpV ;
         tmpV = mesh.vnew(i) ;

         if ( FABS(tmpV - Real_t(1.0)) < v_cut )
            tmpV = Real_t(1.0) ;
         mesh.v(i) = tmpV ;
      }
   }

   return ;
}

static inline
void LagrangeElements()
{
  const Real_t deltatime = mesh.deltatime() ;

  CalcLagrangeElements(deltatime) ;

  /* Calculate Q.  (Monotonic q option requires communication) */
  CalcQForElems() ;

  ApplyMaterialPropertiesForElems() ;

  UpdateVolumesForElems() ;
}

static inline
void CalcCourantConstraintForElems()
{
   Real_t dtcourant = Real_t(1.0e+20) ;
   Index_t   courant_elem = -1 ;
   Real_t      qqc = mesh.qqc() ;
   Index_t length = mesh.numElem() ;

   Real_t  qqc2 = Real_t(64.0) * qqc * qqc ;

   for (Index_t i = 0 ; i < length ; ++i) {
      Index_t indx = mesh.matElemlist(i) ;

      Real_t dtf = mesh.ss(indx) * mesh.ss(indx) ;

      if ( mesh.vdov(indx) < Real_t(0.) ) {

         dtf = dtf
            + qqc2 * mesh.arealg(indx) * mesh.arealg(indx)
            * mesh.vdov(indx) * mesh.vdov(indx) ;
      }

      dtf = SQRT(dtf) ;

      dtf = mesh.arealg(indx) / dtf ;

   /* determine minimum timestep with its corresponding elem */
      if (mesh.vdov(indx) != Real_t(0.)) {
         if ( dtf < dtcourant ) {
            dtcourant = dtf ;
            courant_elem = indx ;
         }
      }
   }

   /* Don't try to register a time constraint if none of the elements
    * were active */
   if (courant_elem != -1) {
      mesh.dtcourant() = dtcourant ;
   }

   return ;
}

static inline
void CalcHydroConstraintForElems()
{
   Real_t dthydro = Real_t(1.0e+20) ;
   Index_t hydro_elem = -1 ;
   Real_t dvovmax = mesh.dvovmax() ;
   Index_t length = mesh.numElem() ;

   for (Index_t i = 0 ; i < length ; ++i) {
      Index_t indx = mesh.matElemlist(i) ;

      if (mesh.vdov(indx) != Real_t(0.)) {
         Real_t dtdvov = dvovmax / (FABS(mesh.vdov(indx))+Real_t(1.e-20)) ;
         if ( dthydro > dtdvov ) {
            dthydro = dtdvov ;
            hydro_elem = indx ;
         }
      }
   }

   if (hydro_elem != -1) {
      mesh.dthydro() = dthydro ;
   }

   return ;
}

static inline
void CalcTimeConstraintsForElems() {
   /* evaluate time constraint */
   CalcCourantConstraintForElems() ;

   /* check hydro constraint */
   CalcHydroConstraintForElems() ;
}

static inline
void LagrangeLeapFrog()
{
   /* calculate nodal forces, accelerations, velocities, positions, with
    * applied boundary conditions and slide surface considerations */
   LagrangeNodal();

   /* calculate element quantities (i.e. velocity gradient & q), and update
    * material states */
   LagrangeElements();

   CalcTimeConstraintsForElems();

   // LagrangeRelease() ;  Creation/destruction of temps may be important to capture 
}

int main(int argc, char *argv[])
{
   Index_t edgeElems = 45 ;
   Index_t edgeNodes = edgeElems+1 ;
   // Real_t ds = Real_t(1.125)/Real_t(edgeElems) ; /* may accumulate roundoff */
   Real_t tx, ty, tz ;
   Index_t nidx, zidx ;
   Index_t meshElems, meshNodes ;

   /* get run options to measure various metrics */

   /* ... */

   /****************************/
   /*   Initialize Sedov Mesh  */
   /****************************/

   /* construct a uniform box for this processor */

   mesh.sizeX()   = edgeElems ;
   mesh.sizeY()   = edgeElems ;
   mesh.sizeZ()   = edgeElems ;
   mesh.numElem() = edgeElems*edgeElems*edgeElems ;
   mesh.numNode() = edgeNodes*edgeNodes*edgeNodes ;

   meshElems = mesh.numElem() ;
   meshNodes = mesh.numNode() ;

   /* allocate field memory */

   mesh.AllocateElemPersistent(mesh.numElem()) ;
   mesh.AllocateElemTemporary (mesh.numElem()) ;

   mesh.AllocateNodalPersistent(mesh.numNode()) ;
   mesh.AllocateNodesets(edgeNodes*edgeNodes) ;

   /* Basic Field Initialization */

   for (Index_t i=0; i<meshElems; ++i) {
      mesh.e(i) = Real_t(0.0) ;
      mesh.p(i) = Real_t(0.0) ;
      mesh.q(i) = Real_t(0.0) ;
      mesh.v(i) = Real_t(1.0) ;
   }

   for (Index_t i=0; i<meshNodes; ++i) {
      mesh.xd(i) = Real_t(0.0) ;
      mesh.yd(i) = Real_t(0.0) ;
      mesh.zd(i) = Real_t(0.0) ;
   }

   for (Index_t i=0; i<meshNodes; ++i) {
      mesh.xdd(i) = Real_t(0.0) ;
      mesh.ydd(i) = Real_t(0.0) ;
      mesh.zdd(i) = Real_t(0.0) ;
   }

   for (Index_t i=0; i<meshNodes; ++i) {
      mesh.nodalMass(i) = Real_t(0.0) ;
   }

   /* initialize nodal coordinates */

   nidx = 0 ;
   tz  = Real_t(0.) ;
   for (Index_t plane=0; plane<edgeNodes; ++plane) {
      ty = Real_t(0.) ;
      for (Index_t row=0; row<edgeNodes; ++row) {
         tx = Real_t(0.) ;
         for (Index_t col=0; col<edgeNodes; ++col) {
            mesh.x(nidx) = tx ;
            mesh.y(nidx) = ty ;
            mesh.z(nidx) = tz ;
            ++nidx ;
            // tx += ds ; /* may accumulate roundoff... */
            tx = Real_t(1.125)*Real_t(col+1)/Real_t(edgeElems) ;
         }
         // ty += ds ;  /* may accumulate roundoff... */
         ty = Real_t(1.125)*Real_t(row+1)/Real_t(edgeElems) ;
      }
      // tz += ds ;  /* may accumulate roundoff... */
      tz = Real_t(1.125)*Real_t(plane+1)/Real_t(edgeElems) ;
   }


   /* embed hexehedral elements in nodal point lattice */

   nidx = 0 ;
   zidx = 0 ;
   for (Index_t plane=0; plane<edgeElems; ++plane) {
      for (Index_t row=0; row<edgeElems; ++row) {
         for (Index_t col=0; col<edgeElems; ++col) {
            Index_t *localNode = mesh.nodelist(zidx) ;
            localNode[0] = nidx                                       ;
            localNode[1] = nidx                                   + 1 ;
            localNode[2] = nidx                       + edgeNodes + 1 ;
            localNode[3] = nidx                       + edgeNodes     ;
            localNode[4] = nidx + edgeNodes*edgeNodes                 ;
            localNode[5] = nidx + edgeNodes*edgeNodes             + 1 ;
            localNode[6] = nidx + edgeNodes*edgeNodes + edgeNodes + 1 ;
            localNode[7] = nidx + edgeNodes*edgeNodes + edgeNodes     ;
            ++zidx ;
            ++nidx ;
         }
         ++nidx ;
      }
      nidx += edgeNodes ;
   }

   /* Create a material IndexSet (entire mesh same material for now) */
   for (Index_t i=0; i<meshElems; ++i) {
      mesh.matElemlist(i) = i ;
   }
   
   /* initialize material parameters */
   mesh.dtfixed() = Real_t(-1.0e-7) ;
   mesh.deltatime() = Real_t(1.0e-7) ;
   mesh.deltatimemultlb() = Real_t(1.1) ;
   mesh.deltatimemultub() = Real_t(1.2) ;
   mesh.stoptime()  = Real_t(1.0e-2) ;
   mesh.dtcourant() = Real_t(1.0e+20) ;
   mesh.dthydro()   = Real_t(1.0e+20) ;
   mesh.dtmax()     = Real_t(1.0e-2) ;
   mesh.time()    = Real_t(0.) ;
   mesh.cycle()   = 0 ;

   mesh.e_cut() = Real_t(1.0e-7) ;
   mesh.p_cut() = Real_t(1.0e-7) ;
   mesh.q_cut() = Real_t(1.0e-7) ;
   mesh.u_cut() = Real_t(1.0e-7) ;
   mesh.v_cut() = Real_t(1.0e-10) ;

   mesh.hgcoef()      = Real_t(3.0) ;
   mesh.ss4o3()       = Real_t(4.0)/Real_t(3.0) ;

   mesh.qstop()              =  Real_t(1.0e+12) ;
   mesh.monoq_max_slope()    =  Real_t(1.0) ;
   mesh.monoq_limiter_mult() =  Real_t(2.0) ;
   mesh.qlc_monoq()          = Real_t(0.5) ;
   mesh.qqc_monoq()          = Real_t(2.0)/Real_t(3.0) ;
   mesh.qqc()                = Real_t(2.0) ;

   mesh.pmin() =  Real_t(0.) ;
   mesh.emin() = Real_t(-1.0e+15) ;

   mesh.dvovmax() =  Real_t(0.1) ;

   mesh.eosvmax() =  Real_t(1.0e+9) ;
   mesh.eosvmin() =  Real_t(1.0e-9) ;

   mesh.refdens() =  Real_t(1.0) ;

   /* initialize field data */
   for (Index_t i=0; i<meshElems; ++i) {
      Real_t x_local[8], y_local[8], z_local[8] ;
      Index_t *elemToNode = mesh.nodelist(i) ;
      for( Index_t lnode=0 ; lnode<8 ; ++lnode )
      {
        Index_t gnode = elemToNode[lnode];
        x_local[lnode] = mesh.x(gnode);
        y_local[lnode] = mesh.y(gnode);
        z_local[lnode] = mesh.z(gnode);
      }

      // volume calculations
      Real_t volume = CalcElemVolume(x_local, y_local, z_local );
      mesh.volo(i) = volume ;
      mesh.elemMass(i) = volume ;
      for (Index_t j=0; j<8; ++j) {
         Index_t idx = elemToNode[j] ;
         mesh.nodalMass(idx) += volume / Real_t(8.0) ;
      }
   }

   /* deposit energy */
   mesh.e(0) = Real_t(3.948746e+7) ;

   /* set up symmetry nodesets */
   nidx = 0 ;
   for (Index_t i=0; i<edgeNodes; ++i) {
      Index_t planeInc = i*edgeNodes*edgeNodes ;
      Index_t rowInc   = i*edgeNodes ;
      for (Index_t j=0; j<edgeNodes; ++j) {
         mesh.symmX(nidx) = planeInc + j*edgeNodes ;
         mesh.symmY(nidx) = planeInc + j ;
         mesh.symmZ(nidx) = rowInc   + j ;
         ++nidx ;
      }
   }

   /* set up elemement connectivity information */
   mesh.lxim(0) = 0 ;
   for (Index_t i=1; i<meshElems; ++i) {
      mesh.lxim(i)   = i-1 ;
      mesh.lxip(i-1) = i ;
   }
   mesh.lxip(meshElems-1) = meshElems-1 ;

   for (Index_t i=0; i<edgeElems; ++i) {
      mesh.letam(i) = i ; 
      mesh.letap(meshElems-edgeElems+i) = meshElems-edgeElems+i ;
   }
   for (Index_t i=edgeElems; i<meshElems; ++i) {
      mesh.letam(i) = i-edgeElems ;
      mesh.letap(i-edgeElems) = i ;
   }

   for (Index_t i=0; i<edgeElems*edgeElems; ++i) {
      mesh.lzetam(i) = i ;
      mesh.lzetap(meshElems-edgeElems*edgeElems+i) = meshElems-edgeElems*edgeElems+i ;
   }
   for (Index_t i=edgeElems*edgeElems; i<meshElems; ++i) {
      mesh.lzetam(i) = i - edgeElems*edgeElems ;
      mesh.lzetap(i-edgeElems*edgeElems) = i ;
   }

   /* set up boundary condition information */
   for (Index_t i=0; i<meshElems; ++i) {
      mesh.elemBC(i) = 0 ;  /* clear BCs by default */
   }

   /* faces on "external" boundaries will be */
   /* symmetry plane or free surface BCs */
   for (Index_t i=0; i<edgeElems; ++i) {
      Index_t planeInc = i*edgeElems*edgeElems ;
      Index_t rowInc   = i*edgeElems ;
      for (Index_t j=0; j<edgeElems; ++j) {
         mesh.elemBC(planeInc+j*edgeElems) |= XI_M_SYMM ;
         mesh.elemBC(planeInc+j*edgeElems+edgeElems-1) |= XI_P_FREE ;
         mesh.elemBC(planeInc+j) |= ETA_M_SYMM ;
         mesh.elemBC(planeInc+j+edgeElems*edgeElems-edgeElems) |= ETA_P_FREE ;
         mesh.elemBC(rowInc+j) |= ZETA_M_SYMM ;
         mesh.elemBC(rowInc+j+meshElems-edgeElems*edgeElems) |= ZETA_P_FREE ;
      }
   }

   /* timestep to solution */
   int its=0;
   while(mesh.time() < mesh.stoptime() ) {
      TimeIncrement() ;
      LagrangeLeapFrog() ;
      its++;
      /* problem->commNodes->Transfer(CommNodes::syncposvel) ; */
#if LULESH_SHOW_PROGRESS
      printf("time = %e, dt=%e\n",
             double(mesh.time()), double(mesh.deltatime()) ) ;
#endif
   }
   printf("iterations: %d\n",its);
   //   FILE *fp = fopen("x.asc","wb");
   //for (Index_t i=0; i<mesh.numElem(); i++)
   //    fprintf(fp,"%.6f\n",mesh.x(i));
   //fclose(fp);
               
   return 0 ;
}

