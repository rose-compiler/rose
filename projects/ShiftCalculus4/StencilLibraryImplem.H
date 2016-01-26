#define INTERPNUMCOEFS 5
#define INTERPREFRATIO 4
#include "SPACE.H"
#include "Stencil.H"


template <class TVal> 
void averageCC(const RectMDArray<TVal>& a_avg,
               const RectMDArray<TVal>& a_phi,
               const Box& a_destBox,
               int a_refRatio)
{
  assert(a_avg.getBox().contains(a_destBox));
  assert(a_phi.getBox().
         contains(a_destBox.refineCC(a_refRatio)));
  // Setup for computing stencils.
  array<Shift,DIM> S = getShiftVec();
  StencilOperator<double,TVal,TVal> op;
  // Compute averaging as a tensor product of 1D stencils.
  Point r;
  Stencil<double> stencil[DIM];
  for (int dir = 0; dir < DIM ; dir++)
    {
      Point ptc = getZeros();
      r = getOnes() + getUnitv(dir)*(a_refRatio-1);
      double reffrac = 1./a_refRatio;
      stencil[dir] = reffrac*(S^ptc);
      for (int iref = 1; iref < a_refRatio;iref++)
        {
          Point ptiref = getUnitv(dir)*iref;
          stencil[dir] = stencil[dir] + reffrac*(S^ptiref);
        }
      stencil[dir].setSrcRefratio(r);
    }
  Box dirbox;
  r = getOnes() + getUnitv(0)*(a_refRatio-1);
  Box srcbox = a_destBox.refineCC(getOnes()*a_refRatio);
  dirbox = srcbox.coarsen(r);
  RectMDArray<double> tmpDest[DIM];
  tmpDest[0].define(dirbox);
  op(stencil[0],tmpDest[0],a_phi,dirbox);
  for (int dir = 1; dir < DIM; dir++)
    {
      r = getOnes() + getUnitv(dir)*(a_refRatio-1);
      dirbox=dirbox.coarsen(r);
      tmpDest[dir].define(dirbox);
      op(stencil[dir],tmpDest[dir],tmpDest[dir-1],dirbox);
    }
  copyTo<TVal>(a_avg,tmpDest[DIM-1],dirbox);
};
template <class TVal> 
void averageTrap(const RectMDArray<TVal>& a_avg,
                 const RectMDArray<TVal>& a_src,
                 const Box& a_destBox)
{
  
  // set up shift and stencil variables.

  int refRatio = 2;
  array<Shift,DIM> S = getShiftVec();
  Stencil<double> stencil[DIM];
  StencilOperator<double,TVal,TVal> op;
  
  assert(a_destBox.refine(refRatio).coarsen(refRatio) == a_destBox);
 
  // Stencil is a tensor product of 1D trapezoidal rule stencils. 

  Point r = getOnes();
  for (int dir = 0; dir < DIM ; dir++)
    {
      Point ptc = getZeros();
      Point ptl = getUnitv(dir);
      Point pth = ptl*(-1);
      r = r - getUnitv(dir);
      stencil[dir] = .25*(S^ptl) + .5*(S^ptc) + .25*(S^pth);
      stencil[dir].setSrcRefratio(r);
    }

  RectMDArray<TVal> tmpDest[DIM];
  Box srcbox = a_destBox.refine(refRatio);
  r = getOnes() + getUnitv(0)*(refRatio-1);
  Box dirbox = srcbox.coarsen(r);
  tmpDest[0].define(dirbox);
  Point r2 = getUnitv(0)*(-1);
  dirbox.grow(r2);
  op(stencil[0],tmpDest[0],a_src,dirbox);

  for (int dir = 1; dir < DIM; dir++)
    {
      r = r + getUnitv(dir)*(refRatio-1);
      dirbox= srcbox.coarsen(r);
      tmpDest[dir].define(dirbox);
      r2=r2+getUnitv(dir)*(-1);
      dirbox = dirbox.grow(r2);
      op(stencil[dir],tmpDest[dir],tmpDest[dir-1],dirbox);
    }
  copyTo<TVal>(a_avg,tmpDest[DIM-1],dirbox);
};
template <class TVal> 
void consInterp4_5(const RectMDArray<TVal>& a_phiFine,
                   const RectMDArray<TVal>& a_phiCoarse,
                   const Box& a_destBox)
{  
  array<Shift,DIM> S = getShiftVec();
  double coefs[INTERPREFRATIO][INTERPNUMCOEFS] =
    {{-77./2048,616./2048,1878./2048,-432./2048,63./2048},
     {-19./2048,88./2048,2218./2048,-272./2048,33./2048},
     {33./2048,-272./2048,2218./2048,88./2048,-19./2048},
     {63./2048,-432./2048,1878./2048,616./2048,-77./2048}};
  Stencil<double> interpStencil;
  StencilOperator<double,TVal,TVal> op;
  RectMDArray<TVal> tmpDest[DIM+1];
  Point r = getOnes();
  Box destBoxC = a_destBox.coarsen(r*INTERPREFRATIO); 
  tmpDest[0].define(destBoxC.grow(2));
  a_phiCoarse.copyTo(tmpDest[0]);

  // tensor product interpolation.

  for (int dir = 0; dir < DIM ; dir++)
    {
      // set up destination boxes and RectMDArrays for this diraction's sweep.

      Point ed = getUnitv(dir);
      Box bxdir = a_destBox.coarsen(getOnes() + r*(INTERPREFRATIO-1));
      r = r - ed;
      bxdir = bxdir.grow(r*2);
      Box bxDest = a_destBox.coarsen(getOnes() + r*(INTERPREFRATIO-1)).grow(r*2);
      
      tmpDest[dir+1].define(bxDest);
      tmpDest[dir+1].setVal(0.);
      for (int iref=0;iref < INTERPREFRATIO; iref++)
        {
          // generate stencil.
          Point pt = ed*(-2);
          interpStencil = 
            coefs[iref][0]*(S^(pt)) +
            coefs[iref][1]*(S^(pt + ed)) +
            coefs[iref][2]*(S^(pt + ed*2)) +
            coefs[iref][3]*(S^(pt + ed*3)) +
            coefs[iref][4]*(S^(pt + ed*4));

          // refining in the dir direction.
          interpStencil.setDestRefratio(getUnitv(dir)*(INTERPREFRATIO-1) + getOnes());

          // m_destShift determines the fine point in the stencil being interpolated.
          interpStencil.setDestShift(ed*iref);

          // apply operator for direction, fine point in stencil.

          op(interpStencil,tmpDest[dir+1],tmpDest[dir],bxdir);

        }
    }
  copyTo<double>(tmpDest[DIM],a_phiFine,a_destBox);
};
template <class TVal> 
void consInterp4_4(const RectMDArray<TVal>& a_phiFine,
                   const RectMDArray<TVal>& a_phiCoarse,
                   const Box& a_destBox)
{  
  array<Shift,DIM> S = getShiftVec();
  double coefs[INTERPREFRATIO][INTERPNUMCOEFS] =
    {{-35./1024,294./1024,960./1024,-230./1024,35./1024},
     {-13./1024,58./1024,1088./1024,-122./1024,13./1024},
     {13./1024,-122./1024,1088./1024,58./1024,-13./1024},
     {35./1024,-230./1024,960./1024,294./1024,-35./1024}};
  Stencil<double> interpStencil;
  StencilOperator<double,TVal,TVal> op;
  RectMDArray<TVal> tmpDest[DIM+1];
  Point r = getOnes();
  Box destBoxC = a_destBox.coarsen(r*INTERPREFRATIO); 
  tmpDest[0].define(destBoxC.grow(2));
  a_phiCoarse.copyTo(tmpDest[0]);

  // tensor product interpolation.

  for (int dir = 0; dir < DIM ; dir++)
    {
      // set up destination boxes and RectMDArrays for this diraction's sweep.

      Point ed = getUnitv(dir);
      Box bxdir = a_destBox.coarsen(getOnes() + r*(INTERPREFRATIO-1));
      r = r - ed;
      bxdir = bxdir.grow(r*2);
      Box bxDest = a_destBox.coarsen(getOnes() + r*(INTERPREFRATIO-1)).grow(r*2);
      
      tmpDest[dir+1].define(bxDest);
      tmpDest[dir+1].setVal(0.);
      for (int iref=0;iref < INTERPREFRATIO; iref++)
        {
          // generate stencil.
          Point pt = ed*(-2);
          interpStencil = 
            coefs[iref][0]*(S^(pt)) +
            coefs[iref][1]*(S^(pt + ed)) +
            coefs[iref][2]*(S^(pt + ed*2)) +
            coefs[iref][3]*(S^(pt + ed*3)) +
            coefs[iref][4]*(S^(pt + ed*4));

          interpStencil.setDestRefratio(getUnitv(dir)*(INTERPREFRATIO-1) + getOnes());
          interpStencil.setDestShift(ed*iref);

          // apply operator.

          op(interpStencil,tmpDest[dir+1],tmpDest[dir],bxdir);

        }
    }
  copyTo<double>(tmpDest[DIM],a_phiFine,a_destBox);
};
template <class TVal> 
void averageCC(const RectMDArray<TVal>& a_avg,
               const RectMDArray<TVal>& a_phi,
               const Box& a_destBox,
               const Point& a_refRatio,
               const Point& a_destOffset)
{
  assert(a_avg.getBox().contains(a_destBox).shift(a_destOffset));
  assert(a_phi.getBox().
         contains(a_destBox.refineCC(a_refRatio).shift(a_destOffset)));
  // Setup for computing stencils.
  array<Shift,DIM> S = getShiftVec();
  StencilOperator<double,TVal,TVal> op;
  // Compute averaging as a tensor product of 1D stencils.
  Point r;
  Stencil<double> stencil[DIM];
  for (int dir = 0; dir < DIM ; dir++)
    {
      Point ptc = getZeros();
      r = getOnes() + getUnitv(dir)*(a_refRatio[dir]-1);
      double reffrac = 1./a_refRatio[dir];
      stencil[dir] = reffrac*(S^ptc);
      for (int iref = 1; iref < a_refRatio[dir];iref++)
        {
          Point ptiref = getUnitv(dir)*iref;
          stencil[dir] = stencil[dir] + reffrac*(S^ptiref);
        }
      stencil[dir].setSrcRefratio(r);
    }
  Box dirbox;
  r = getOnes() + getUnitv(0)*(a_refRatio[0]-1);
  Box srcbox = a_destBox.refineCC(getOnes()*a_refRatio);
  dirbox = srcbox.coarsen(r);
  RectMDArray<double> tmpDest[DIM];
  tmpDest[0].define(dirbox);
  op(stencil[0],tmpDest[0],a_phi,dirbox);
  for (int dir = 1; dir < DIM; dir++)
    {
      r = getOnes() + getUnitv(dir)*(a_refRatio[dir]-1);
      dirbox=dirbox.coarsen(r);
      tmpDest[dir].define(dirbox);
      op(stencil[dir],tmpDest[dir],tmpDest[dir-1],dirbox);
    }
  copyTo<TVal>(a_avg,tmpDest[DIM-1],dirbox);
};
