// This is the second simple test that applies a Laplacian operator
// with 2*DIM+1 size stencil with one ghost cell along each face
// of the data box. Thus the size of the destination box is 
//BLOCKSIZE^DIM and the size of the source box is (BLOCKSIZE+2)^DIM
//FOR these tests BLOCKSIZE=DOMAINSIZE because we are working with 
//a single box. BLOCKSIZE, DOMAINSIZE and DIM are set in the Makefile

#define DIM 2
#define BLOCKSIZE 5
#define DOMAINSIZE 7

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

// function to initialize the src data
void initialize(RectMDArray<double>& patch)
{
  Box D0 = patch.getBox();
  int k=1;
  for (Point pt = D0.getLowCorner();D0.notDone(pt);D0.increment(pt))
      patch[pt] = k++;
}

int main(int argc, char* argv[])
{

  Point zero=getZeros();
  Point lo=zero;
// Point hi=getOnes()*(DOMAINSIZE-1);
  Point hi=getOnes()*(6);

  Box bxdest(lo,hi); //box low and high corners for destination
  
  // This will grow the box by one ghost
  // along each face and become the box for
  // the source box. 
  Box bxsrc=bxdest.grow(1);
  
  //source and destination data containers
  RectMDArray<double> Asrc(bxsrc);
  RectMDArray<double> Adest(bxdest);

  // all the coefficients I need for this operation
  double ident=1.0;
  double C0=-4.0;

  initialize(Asrc);

  cout <<" The source Box" << endl;
  Asrc.print();
  cout << endl;

#if 0
  vector<Point>shft;
  vector<double>wt;
  shft.push_back(zero);
  wt.push_back(C0);
  for (int dir=0;dir<DIM;dir++)
    {
      Point thishft=getUnitv(dir);
      shft.push_back(thishft);
      wt.push_back(ident);
      shft.push_back(thishft*(-1));
      wt.push_back(ident);
    }
  
  // build the stencil, and the stencil operator
  Stencil<double> laplace(wt,shft);
#else
  // This is a simpler interface to interpret (suggested by Anshu).
     Stencil<double> laplace(pair<Shift,double>(zero,C0));

  // DQ: these are used as relative offsets instead of points, so then might be more clear if 
  // there was a RelativeOffset class.
     Point xdir = getUnitv(0);

  // here I am using "+" operator defined on stencils, which is one mechanism for composing
     laplace=laplace+(pair<Shift,double>(xdir,ident));

  // DQ: this might be simpler if we have an operator*=() member function.
  // xdir=xdir*(-1);
     xdir *= -1;
     laplace=laplace+(pair<Shift,double>(xdir,ident));
     Point ydir=getUnitv(1);
     laplace=laplace+(pair<Shift,double>(ydir,ident));

  // DQ: this might be simpler if we have an operator*=() member function.
  // ydir=ydir*(-1);
     ydir *= -1;
     laplace=laplace+(pair<Shift,double>(ydir,ident));

#if DIM==3
     Point zdir=getUnitv(1);
     laplace=laplace+(pair<Shift,double>(zdir,ident));

  // DQ: this might be simpler if we have an operator*=() member function.
  // zdir=zdir*(-1);
     zdir *= -1;
     laplace=laplace+(pair<Shift,double>(zdir,ident));
#endif
#endif

  StencilOperator<double,double, double> op;

  //apply stencil operator
  op(laplace,Adest,Asrc,bxdest);
  cout <<" The destination Box" << endl;
  Adest.print();
}
