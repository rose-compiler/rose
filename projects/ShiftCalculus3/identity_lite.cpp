// This is the simplest test that applies the identity stencil operator
// to the src data container and returns the result in destination
// data container (effectively a copy operation)
// The size of the both boxes is BLOCKSIZE^DIM
// FOR these tests BLOCKSIZE=DOMAINSIZE because we are working with 
// a single box. BLOCKSIZE, DOMAINSIZE and DIM are set in the Makefile

#define DIM 2
#define BLOCKSIZE 5
#define DOMAINSIZE 7

// #include <iostream>
// #include <cassert>
// #include <cmath>
#include "vector_lite.h"
// #include <memory>
// #include <stdio.h>
// #include <fstream>
#include "Shift_lite.H"
#include "Stencil_lite.H" 

// #include "PowerItoI.H"
// #include "RectMDArray.H"

#if 0
// function to initialize the src data
void initialize(RectMDArray<double>& patch)
{
  Box D0 = patch.getBox();
  int k=1;
  for (Point pt = D0.getLowCorner();D0.notDone(pt);D0.increment(pt))
      patch[pt] = k++;
}
#endif

int main(int argc, char* argv[])
{

  Point zero=getZeros();
  Point lo=zero;

// DQ: This appears to be a bug (fix in alt branch): generates "Point hi=getOnes()* 7 - 1;" instead of "Point hi=getOnes()* (7 - 1);"
// Point hi=getOnes()*(DOMAINSIZE-1);
  Point hi=getOnes()*(6);

  Box bx(lo,hi); //box low and high corners for the data container

  //source and destination data containers
  RectMDArray<double> Asrc(bx);
  RectMDArray<double> Adest(bx);
  double ident=1.0;

//initialize(Asrc);

//cout <<" The source Box" << endl;
//Asrc.print();
//cout << endl;

  // build the identity stencil
// Stencil<double> id(pair<Shift,double>(getZeros(),ident));
  Stencil<double> id(pair<Shift,double>(zero,ident));
  StencilOperator<double,double, double> op;
  op(id,Adest,Asrc,bx);
//cout <<" The destination Box" << endl;
//Adest.print();

}
