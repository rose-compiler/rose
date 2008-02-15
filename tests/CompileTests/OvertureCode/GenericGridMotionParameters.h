#ifndef GenericGridMotionParameters_h
#define GenericGridMotionParameters_h

#include <Overture.h>

class GenericGridMotionParameters
{
//
//\begin{>GenericGridMotionParameters.tex}{\subsection{\GenericGridMotionParameters class}}
//\no function header:
//   This is the abstract base class for a container class to store parameters for
//   GridMotion classes derived from the for the \GenericGridMotion
//   class. The base class contains only the following accessable parameter(s):
//
// /movingGrid(grid): a LogicalArray that indicates which grids move.
//   Values are 
//   \begin{itemize}
//     \item 0: grid doesn't move
//     \item 1: grid can move
//    \end{itemize}
//
//    The intent is that additional parameters that may be required by a GridMotion class
//    derived from the \GenericGridMotion class would be stored in a container class derived
//    from this one.  By keeping all the parameters in a single object, the interface for
//    classes derived from the \GenericGridMotion base class can be kept uniform, even if
//    the actual parameters are different.
//\end{GenericGridMotionParameters.tex}

 public:
  GenericGridMotionParameters ();
  GenericGridMotionParameters (const GenericGridMotionParameters& params_);
  GenericGridMotionParameters& operator= (const GenericGridMotionParameters& params_);
//...ecgs only
//  explicit GenericGridMotionParameters (const CompositeGrid& compositeGrid_);
// ***obsolete: this is not a reasonable form of the constructor
  GenericGridMotionParameters (const CompositeGrid& compositeGrid_);
  GenericGridMotionParameters (const int& numberOfDimensions, const int& numberOfGrids);
  
  virtual ~GenericGridMotionParameters ();
  
  LogicalArray movingGrid;
 protected:
  int numberOfGrids, numberOfDimensions;
};


#endif
