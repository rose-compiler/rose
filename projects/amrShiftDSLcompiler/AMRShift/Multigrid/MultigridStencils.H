#ifndef _MULTIGRIDSTENCILS_H_
#define _MULTIGRIDSTENCILS_H_

#include <memory>

#include "RectMDArray.H"
#include "LevelData.H"
#include "BLIterator.H"
#include "CH_Timer.H"
#include "Stencil.H"

extern Stencil<double> g_Laplacian;
extern Stencil<double> g_PointRelax;
extern Stencil<double> g_AvgDown[1 << DIM];
extern Stencil<double> g_FineInterp;

extern void buildMultigridStencils();

#endif
