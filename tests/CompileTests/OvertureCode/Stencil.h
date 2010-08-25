// Stencil.h

#ifndef _STENCIL_H_
#define _STENCIL_H_

/*
//============================================================================
This class represents a stencil.
It can act on gridfunctions and can be used to update a coefficient matrix


WTB: krister@tdb.uu.se

//============================================================================

*/

#include <iostream.h>
#include "MappedGridFunction.h"
#include "MappedGridOperators.h"
#include "A++.h"

//\begin{>StencilPublic.tex}{\subsubsection{Typedefs}}
//\no function header:
// These Typedefs are used for more readable code:
// {\footnotesize
// \begin{verbatim}
//  typedef IntegerArray Point;
// \end{verbatim}
// }
//
//  A Point is used for offsets. 
//  The dimension of the Point is internally 1x3, 
//  Methods accept Points 3x1,2x1,1x1,1x2, too. 
//
//\end{StencilPublic.tex}

typedef IntegerArray Point;


//\begin{>>StencilPublic.tex}{}
//\no function header:
//
// {\footnotesize
// \begin{verbatim}
//  typedef RealMappedGridFunction CoefficientMatrix;
// \end{verbatim}
// }
//
//
//  A CoefficientMatrix is a RealMappedGridFunction which uses the Overture
//  sparse format. It may be created by a Stencil and may be used by Oges for
//  solving a system.
//
//\end{StencilPublic.tex}


typedef RealMappedGridFunction CoefficientMatrix;

//\begin{>>StencilPublic.tex}{}
//\no function header:
//
// {\footnotesize
// \begin{verbatim}
//  enum predefinedStencils {
//    identity,
//    displacex,          // displacex(u_i) -> u_{i+1}
//    inversedisplacex,    // inversedisplacex(u_i) -> u_{i-1}
//    dplusx,
//    dminusx,
//    dzerox,
//    dnullx,
//    dplusxdminusx
//  };
// \end{verbatim}
// }
// {\ff \ST ::predefinedStencils} is an enumerate used for creating 
// predefined stencils by a special constructor.
// Note that these stencils are defined on a grid with spacing 1.
//
//\end{StencilPublic.tex}

class Stencil
{
  friend
    ostream &operator<<(ostream &os, const Stencil &s);
public:
  // The existing predefined stencils are
  enum predefinedStencils {
    identity,
    displacex,          // displacex(u_i) -> u_{i+1}
    inversedisplacex,    // inversedisplacex(u_i) -> u_{i-1}
    dplusx,
    dminusx,
    dzerox,
    dnullx,
    dplusxdminusx
  };
       

  // constructors, operators, and misc methods
  //==============================================================
  Stencil();
  Stencil(RealArray weights_, IntegerArray offsets_); // weights nx1; offsets nx3
  Stencil(const Stencil&);
  Stencil(const predefinedStencils &choice);
  ~Stencil();
  Stencil &operator= (const Stencil&);
  Stencil operator() (const Stencil&) const;
  Stencil operator* (Real d) const;
  Stencil operator+ (const Stencil &) const;
  Stencil operator- (const Stencil &) const;
  Stencil operator- () const;
  Stencil operator/ (Real d) const;
  void operator+= (const Stencil&);
  void operator-= (const Stencil&);
  void operator*= (Real d);
  void operator/= (Real d);

  void addWeight(Real d,const Point &offset);

  // display shows things depending on 'what':
  enum displayWhat { text=1, structure, both };

  void display(const aString &str="", displayWhat what = text, 
               ostream &os = cout) const;
  void displayText(const aString &str="", ostream &os = cout) const;
  void displayStructure(const aString &str="", ostream &os = cout, 
			int legend = FALSE) const;


  // default rotate: 90 degrees anticlock looking from z-axis
  Stencil rotate(int axis = axis3, int steps=1) const; 

  // Methods dealing with the Stencil acting on a targetGF 
  //==============================================================  

  // Values taken from sourceGF
  // Returns reference to targetGF, allows some nicer syntax 
  RealMappedGridFunction &
  applyStencil(const RealMappedGridFunction &sourceGF, 
               RealMappedGridFunction &targetGF, 
	       const Index &I1, 
	       const Index &I2, 
	       const Index &I3, 
	       const Index &N) const;

  // Following method used by Operators who assume RealArray, not GridFunction
  RealArray &
  applyStencil(const RealArray  &sourceGF, 
               RealArray  &targetGF, 
	       const Index &I1, 
	       const Index &I2, 
	       const Index &I3, 
	       const Index &N) const;
  

  // Methods dealing with creation of a discretization Matrix
  //==============================================================  

  // asserts coeff is an initialized coefficient matrix
  // It adds weights to coeff (that is, it does not nullify)
  // For systems, mixed terms can be obtained via iE and iC
  void
  makeCoefficients(CoefficientMatrix &coeff, 
                   const Index &I1, 
		   const Index &I2, 
		   const Index &I3, 
		   const int iE = 0, // Equation
		   const int iC  = 0 // Component
		   ) const;

  // For systems, if the stencil shall be applied on a vector, 
  // this method returns correct operator
  void
  makeCoefficients(CoefficientMatrix &coeff, 
                   const Index &I1, 
		   const Index &I2, 
		   const Index &I3, 
		   const Index &C
		   ) const;

  // conformOffsets decides how the offsets are numbered.
  // Its purpose is to generate conform coefficient matrixes
  // so that the A++ opertor+ works
  // offsets_ must hold existing offsets
  void conformOffsets(IntegerArray newOffsets);


  // Methods giving some info
  //==============================================================  
  IntegerArray getWidth() const; 

  int offsetExist(const Point &p) const;
  // if return is TRUE, i holds the index for the point 
  int offsetExist(const Point &p, int &i) const;

  // Method that estimates the order of the stencil for a known derivative
  Real checkOrder(MappedGridOperators::derivativeTypes) const;

  // Remove zeros can be useful
  void removeZeros();

  // Internal test routine - not intended to be official
  void test(); 

private:
//\begin{>StencilPrivate.tex}{}
//\no function header:
//
// This is the attributes
// { \footnotesize
// \begin{verbatim}
  int nrOfWeights;   // number of weights
  IntegerArray width;    // width is 3x2 array 
  RealArray weights; // the weights (nx1) ... 
  IntegerArray offsets;   // ... have these offsets (nx3)
  static int debug;  // debug flag
// \end{verbatim}
// }
//\end{StencilPrivate.tex}

//\begin{>>StencilPrivate.tex}{}
//\no function header:
// Here are some private methods: 
// {\footnotesize
// \begin{verbatim}
  void resizePoint(Point &p) const; // Used to transform p to 1x3 
  void updateWidth();               // Used to update width after some constructions
  void removeWeight(int i);         // Used to remove a (zero) weight
// \end{verbatim}
// }
};
//\end{StencilPrivate.tex}

Stencil operator*(Real,const Stencil &);

#endif
