#ifndef GenericGridMotion_h
#define GenericGridMotion_h

# include <Overture.h>
# include "GenericGridMotionParameters.h"

//...forward declaration of class
class DynamicMovingGrids;

class GenericGridMotion
//
//\begin{>GenericGridMotion.tex}{\subsection{\GenericGridMotion abstract base class}}
//\no function header:
//   This is the base class for derived classes that are used to specify
//   the motion of component grids through Mappings. This class specifies 
//   the interface to the DynamicMovingGrids class, which is what the
//   user will call in an application code to manage the moving grids.
//   Examples of currently implemented derived GridMotion classes are
//    \begin{itemize}
//      \item{} MatrixTransformGridMotion
//    \end{itemize}
//
// /Author: D L Brown 980319
//
//\end{GenericGridMotion.tex}
{
  // DynamicMovingGrids class needs to call swapMappings, but nobody else does
  friend class DynamicMovingGrids;
  
 public:
  GenericGridMotion ();
  GenericGridMotion (CompositeGrid& cg_, 
                     const int& numberOfLevels_);
  GenericGridMotion (const GenericGridMotion& ggm);
  GenericGridMotion& operator=(const GenericGridMotion& ggm);

  virtual ~GenericGridMotion ();

  void initialize (CompositeGrid& cg_, 
                   const int& numberOfLevels_);

  virtual void moveMappings (const real & time,
                             const real & timestep, 
			     const int & level)=0;


  virtual void moveMappings (const real & time,
                             const real & timestep,
			     const realCompositeGridFunction & u,
			     const int & level)=0;
  

  inline Mapping* mapping (const int & grid, const int & level);
  inline int  numberOfGrids();
  inline int  numberOfLevels();
  inline int  numberOfDimensions();
  inline bool isNonlinear();
  inline void getHasMoved (LogicalArray &hasMoved);
  inline bool analyticVelocityAvailable();
  
  virtual void getAnalyticVelocity(realMappedGridFunction& velocity,
                                   const int& grid,
				   const int& level,
				   const real& time,
				   CompositeGrid** cgMoving,
				   const Index& I1=nullIndex,
				   const Index& I2=nullIndex,
				   const Index& I3=nullIndex)=0;
  
 protected:

  
  CompositeGrid cg;
  
  //...this is a two-dimensional array of pointers to derived Mappings;
  //   sized [numberOfGrids,numberOfLevels]; it is setup by the function
  //   setupMovingMappingArray in the base class
  Mapping*** movingMapping;
  
  //...this sets up the mapping array
  void setupMovingMappingArray (int& numberOfGrids, int& numberOfLevels);
  //...deep copy of the mapping array
  void copyMovingMappingArray (Mapping*** movingMapping_);
  //...deletes the elements of the mapping array
  void destructMovingMappingArray ();
  //...part of the moving grid algorithm may involve swapping pointers to the
  //   CompositeGrids; the mapping pointers will have to be swapped correspondingly;
  //   This function does that. 
  void swapMappings ();

  //...these protected values are set by the constructor from its input parameters
  int numberOfLevels_;
  int numberOfGrids_;
  int numberOfDimensions_;
  
  //...these variables must be set by the derived class
  LogicalArray hasMoved_;
  bool analyticVelocityAvailable_;
  bool isNonlinear_;

  //...this function is used to initialize the moving Mappings and their parameters
  //   again it is pure virtual since the base class doesn't actually know
  //   what mappings are involved

  virtual void initializeMappings ()=0;

  //...this flag remembers if the mapping list has been initialized
  bool movingMappingHasBeenSetup;
  


  
  
};

//\begin{>>GenericGridMotion.tex}{\subsubsection{numberOfGrids}}
inline int GenericGridMotion::
numberOfGrids()
//
// /Purpose: returns the local idea of how many component grids there are
//\end{GenericGridMotion.tex}
{
  return numberOfGrids_;
};

//\begin{>>GenericGridMotion.tex}{\subsubsection{numberOfLevels}}
inline int GenericGridMotion::
numberOfLevels()
// /Purpose:
//   Returns the number of timestepping levels that the GridMotion
//   class thinks there is.
//\end{GenericGridMotion.tex}
{
  return numberOfLevels_;
};

//\begin{>>GenericGridMotion.tex}{\subsubsection{numberOfDimensions}}
inline int GenericGridMotion::
numberOfDimensions()
// 
// /Purpose: returns the dimensionality of space for this grid
//\end{GenericGridMotion.tex}
{
  return numberOfDimensions_;
};

//\begin{>>GenericGridMotion.tex}{\subsubsection{isNonlinear}}
inline bool GenericGridMotion::
isNonlinear()
//
// /Purpose: this function is used by a GridMotion class to tell
//           other classes whether the grid movement depends on the solution
//           of the PDE or the geometry of the grids.  For example, if the motion depended on the fluid 
//           flow or the surface curvature of a grid (true or not?), the GridMotion class should declare itself
//           nonlinear.
//\end{GenericGridMotion.tex}
{
  return isNonlinear_;
};

//\begin{>>GenericGridMotion.tex}{\subsubsection{getHasMoved}}
inline void GenericGridMotion::
getHasMoved (LogicalArray &hasMoved)
//
// /Purpose: return the hasMoved array; 
//\end{GenericGridMotion.tex}
{
  hasMoved=hasMoved_;
};

//\begin{>>GenericGridMotion.tex}{\subsubsection{analyticVelocityAvailable}}
inline bool GenericGridMotion::
analyticVelocityAvailable()
//
// /Purpose: determine if an analytic velocity is defined for this GridMotion class
//     through the getAnalyticVelocity() function.  The derived class constructor
//     must set the value of analyticVelocityAvailable\_.
//
// /Return Values: ~
//   \begin{itemize}
//     \item{LogicalFalse:} an analytic velocity function is not available
//     \item{LogicalTrue:}  an analytic velocity function is available
//   \end{itemize}
//\end{GenericGridMotion.tex}
{return analyticVelocityAvailable_;};  

//\begin{GenericGridMotion.tex}{\subsubsection{mapping}}
inline Mapping* GenericGridMotion::
mapping (const int & grid, const int & level)
// /Purpose: this is an access function to the Mapping*'s in the movingMapping array
// /grid: which grid
// /level: which level
//\end{GenericGridMotion.tex}
{
  return movingMapping[grid][level];
};
#endif
