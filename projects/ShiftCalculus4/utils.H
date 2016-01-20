#ifndef _UTILS_H
#define _UTILS_H

#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <memory>
#include <stdio.h>
#include <fstream>
#include "PowerItoI.H"
#include "RectMDArray.H"
/// sets facta += fact b * b
template <int ncomp> 
void RAdd(RectMDArray<double,ncomp> & a_dest, RectMDArray<double,ncomp> & a_src, const double a_fact)
{
  for(int k =0; k< a_dest.dataSize(); k++)
    {
      a_dest[k] += a_fact*a_src[k];
    }
}

/// sets facta *= factb
template <int ncomp> 
void RMult(RectMDArray<double,ncomp> & a_dest, RectMDArray<double,ncomp> & a_src)
{
  for(int k =0; k< a_dest.dataSize(); k++)
    {
      a_dest[k] *= a_src[k];
    }
}

/// sets facta *= b
template <int ncomp> 
void ScalarMult(RectMDArray<double,ncomp> &a_dest, const double a_fact)
{
  for(int k =0; k< a_dest.dataSize(); k++)
    {
      a_dest[k] *= a_fact;
    }
}
#endif

