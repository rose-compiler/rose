#ifndef _STENCILLIBRARY_H_
#define _STENCILLIBRARY_H_
#include <array>
#include <tuple>
#include "Stencil.H"
#include "RectMDArray.H"

using namespace std;

/// Averages node-centered a_dest onto a_src assuming coarsening by a factor of two
/// on a_destBox.
template <class TVal>  
void averageTrap(const RectMDArray<TVal>& a_dest,
                 const RectMDArray<TVal>& a_src,
                 const Box& a_destBox);
/// Averages cell-centered data a_dest onto a_src on a_destBox. 
template <class TVal> 
void averageCC(const RectMDArray<TVal>& a_avg,
               const RectMDArray<TVal>& a_phi,
               const Box& a_destBox,
               const Point a_refRatio,
               int a_offset);
/// Averages cell-centered data a_dest onto a_src on a_destBox, with 
/// anisotropic refinement ratio a_refratio and an offset in the destination.
/// used for constructing flux registers. 
template <class TVal> 
void averageCC(const RectMDArray<TVal>& a_avg,
               const RectMDArray<TVal>& a_phi,
               const Box& a_destBox,
               const Point a_refRatio,
               int a_destOffset);
/// Fourth-order finite-volume interpolation of data from a_dest onto a_src 
/// on a_destBox. Hard-wired to factor of 4 refinement.
template <class TVal> 
void consInterp4_4(const RectMDArray<TVal>& a_interp,
                   const RectMDArray<TVal>& a_phi,
                   const Box& a_destBox);
/// Fifth-order finite-volume interpolation of data from a_dest onto a_src 
/// on a_destBox.Hard-wired to factor of 4 refinement.
template <class TVal> 
void consInterp5_4(const RectMDArray<TVal>& a_interp,
                   const RectMDArray<TVal>& a_phi,
                   const Box& a_destBox);
#include "StencilLibraryImplem.H"
#endif
