#ifndef _MULTIGRID_H_
#define _MULTIGRID_H_

#include <memory>

#include "RectMDArray.H"
#include "LevelData.H"
#include "BLIterator.H"
#include "CH_Timer.H"
#include "Stencil.H"
#include "MultigridStencils.H"

/// Multigrid: solve a FV discretization of Poisson's equation on a union of rectangles.
#pragma DSL keyword
class Multigrid
{
public:
  /// Default constructor.
  Multigrid();
  /// Not assuming strong construction, so we have constructor and define.
  Multigrid(
            const BoxLayout& a_bl,
            double a_dx,
            int a_level
            );
  void define(
         const BoxLayout& a_bl,
         double a_dx,
         int a_level
         );
  ///  Destructor.
  ~Multigrid();
  /// Compute residual.
  void residual(
                LevelData<double >& a_res,
                LevelData<double >& a_phi,
                const LevelData<double >& a_rhs
                );
  /// Compute max norm of the residual.
  double resnorm(
                LevelData<double >& a_phi,
                const LevelData<double >& a_rhs
                );
  /// Point relaxation. Currently point Jacobi.
  void pointRelax(
                  LevelData<double >& a_phi,
                  const LevelData<double >& a_rhs,
                  int a_numIter
                  );
  /// average down residual to next coarser level. Assumes exact averaging on control volumes.
  void avgDown(
               LevelData<double >& a_resc,
               const LevelData<double >& a_res
               );
  /// Piecewise constant interpolation of coarse correction to increment fine solution.
  void fineInterp(
                  LevelData<double >& a_phiFine,
                  const LevelData<double >& a_deltaCoarse
                  );
  /// Multigrid v cycle.
  void vCycle(
              LevelData<double >& a_phi,
              const LevelData<double >& a_rhs
              );
private:
  LevelData<double > m_res;
  LevelData<double > m_rescReAligned;
  LevelData<double > m_deltaReAligned;
  LevelData<double > m_resc;
  LevelData<double > m_delta;
  BoxLayout m_bl;
  Multigrid* m_coarsePtr;
  double m_dx;
  int m_level;
  int m_preRelax = 2*DIM;
  int m_postRelax = 2*DIM;
  int m_bottomRelax = 20;
  long long m_flops = 0;
  bool m_isAligned;
};
#endif
