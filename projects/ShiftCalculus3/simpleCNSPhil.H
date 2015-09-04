// This is the begining of a real application CNS
// At present it only implements the stencils for 
// directionally defined first and second derivative
// discretizations for computing the solution of the
// compressible Navier-stokes equation
// There is a meaningless input, and the operators 
// are applied to it. In a real application there will 
// be boundary conditions, sensible initial conditions,
// ghost cell fill and meaningfull coefficients. Additionally
// there will be concept of multiple components in the data
// holders, where results are obtained for one component by
// applying stencil operators or pointwise functions on some
// sunset of the data holder's components.

#ifndef _CNS_H_
#define _CNS_H_

#if 0
#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <memory>
#include <stdio.h>
#include <fstream>
#include "Shift.H"
#include "Stencil.H" 
#include "PowerItoI.H"
#include "RectMDArray.H"
#include "LevelData.H"
#else
#include "simpleCNS.h"
#endif


// only correct for 3D
#define QRHO  0
#define QVELX 1
#define QVELY 2
#define QVELZ 3
#define QPRES 4
#define QTEMP 5
#define QNUM  6

#define CRHO  0
#define CMOMX 1
#define CMOMY 2
#define CMOMZ 3
#define CENG  4
#define CNUM  5


class CNS
{
public:
  void parseCommandLine(int argc, char* argv[]);
  void initialize(RectMDArray<double, CNUM>& a_patch) const;
  void initialize(LevelData<double, CNUM> & a_cons) const;
  double consToPrimF(Tensor<double,QNUM>& a_Q, CTensor<double, CNUM>& a_U) const;
  double consToPrim(LevelData<double,QNUM>& a_Q, const LevelData<double, CNUM>& a_U)const ;
  void rhs(LevelData<double,CNUM>    & a_fp, 
           const LevelData<double,QNUM> & a_prim) const;
  void makeStencils();
  void hypFluxesF(int a_dir,Tensor<double,CNUM>& a_flux, CTensor<double, QNUM>& a_q) const;
  void viscousRhsF(Tensor<double,DIM>& a_divTau, 
                  CTensor<double,DIM,DIM,DIM>& a_d2u) const;
  void viscousHeatingF(double& a_edot,CTensor<double,DIM,DIM>& a_gradu) const;


  int    m_nstep   = 10;
  int    m_numblockpower = 0;
  //number of ghost
  const int    m_nghost = 4;
  Stencil<double> m_duds[DIM];
  Stencil<double> m_d2uds2[DIM];
  double m_problo  = -0.1;
  double m_probhi  =  0.1;

  int    m_numblocks = pow(2, m_numblockpower);
  int    m_ncell     = m_numblocks*BLOCKSIZE;
  double m_dx        = (m_probhi-m_problo)/m_ncell;
  
 

  double m_cfl     = 0.5;

  //diffusion coefficients
  double m_eta  = 1.0e-4;
  double m_alam = 1.5e2;
  //ideal gas constant
  double m_gamma = 1.4;
  //pi
  double m_pi = 4.*atan(1.0);

  
  //coefficient thermal expansion at constant volume
  double m_CV  = 8.3333333333e6;
};


#endif
