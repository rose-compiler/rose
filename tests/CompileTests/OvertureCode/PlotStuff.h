#ifndef PLOTSTUFF_H 
#define PLOTSTUFF_H  "PlotStuff.h"

//============================================================================================
//     -------------------------------------------------------------
//     ------This Class knows how to Plot Stuff from Overture-------
//     -------------------------------------------------------------
//
//  This Class is derived from GL_GraphicsInterface which is derived from GenericGraphicsInterface
// 
//===========================================================================================

#include "Overture.h"
#include "PlotStuffParameters.h"
#include "GL_GraphicsInterface.h"
#include "TrimmedMapping.h"

class AdvancingFront;  // forward declaration
class TrimmedMapping;
class NurbsMapping;

class PlotStuff : public GL_GraphicsInterface
{
public:
  
// Default constructor
PlotStuff(const aString & windowTitle = "Your Slogan Here");

// Constructor that takes argc and argv from main program and strips away Motif parameters
PlotStuff(int & argc, char *argv[], const aString & windowTitle = "Your Slogan Here");

//  This Constructor will only create a window if the the argument initialize=TRUE
//  To create a window later, call createWindow()
PlotStuff( const bool initialize, const aString & windowTitle = "Your Slogan Here" );

virtual ~PlotStuff();
  
// Plot a Mapping, pass optional parameters
virtual void 
plot(Mapping & map, 
     GraphicsParameters & parameters=Overture::defaultGraphicsParameters(),
     int dList=0, bool lit=FALSE);

// plot a structured mapping
void 
plotStructured(Mapping & map, 
	       GraphicsParameters & parameters=Overture::defaultGraphicsParameters(),
	       int dList=0, bool lit=FALSE);

// plot a CompositeSurface
void 
plotCompositeSurface(CompositeSurface & cs, 
		     GraphicsParameters & params=Overture::defaultGraphicsParameters());


// plot a trimmed mapping, uses glunurbs unless Mesa is configured
void
plotTrimmedMapping(TrimmedMapping &map,
		   GraphicsParameters & params=Overture::defaultGraphicsParameters(),
		   int dList =0, bool lit =FALSE);

// plot a nurbs mapping, uses glunurbs unless Mesa is configured
void 
plotNurbsMapping(NurbsMapping &map,
		 GraphicsParameters & params=Overture::defaultGraphicsParameters(),
		 int dList =0, bool lit =FALSE);

// nurbs rendering calls using gl
void 
renderTrimmedNurbsMapping(TrimmedMapping &map, GraphicsParameters & params=Overture::defaultGraphicsParameters(), bool lit=FALSE);

void
renderNurbsSurface(NurbsMapping &map, GLfloat mode);

void
renderNurbsCurve(NurbsMapping &map, GLfloat mode, GLenum type);

void 
renderTrimmedNurbsByMode(TrimmedMapping &map, GLfloat mode);

// plot an unstructured mapping
void
plotUM(UnstructuredMapping & map,
     GraphicsParameters & parameters=Overture::defaultGraphicsParameters(),
	       int dList=0, bool lit=FALSE);

// Plot a MappedGrid
virtual void 
plot(MappedGrid & mg, 
     GraphicsParameters & parameters=Overture::defaultGraphicsParameters() );

// Plot contours and/or a shaded surface plot of a realMappedGridFunction in 2D or 3D
virtual void 
contour(const realMappedGridFunction & u, 
	GraphicsParameters & parameters=Overture::defaultGraphicsParameters());

// Plot streamlines of a 2D vector field
virtual void 
streamLines(const realMappedGridFunction & uv, 
	    GraphicsParameters & parameters=Overture::defaultGraphicsParameters());

// Plot a GridCollection or Composite grid
virtual void 
plot(GridCollection & cg, 
     GraphicsParameters & parameters=Overture::defaultGraphicsParameters());

// Plot contours and/or a shaded surface plot of a GridCollectionFunction/CompositeGridFunction in 2D
virtual void 
contour(const realGridCollectionFunction & u, 
	GraphicsParameters & parameters=Overture::defaultGraphicsParameters());

// Plot streamlines of a vector field
virtual void
streamLines(const realGridCollectionFunction & uv, 
	    GraphicsParameters & parameters=Overture::defaultGraphicsParameters());

// plot an advancing front
virtual void 
plot(AdvancingFront & front, 
     GraphicsParameters & parameters=Overture::defaultGraphicsParameters());
  

// Plot 1D functions
virtual void
plot( const realArray & t, 
      const realArray & x, 
      const aString & title = nullString, 
      const aString & tName       = nullString,
      const aString *xName        = NULL,
      GraphicsParameters & parameters=Overture::defaultGraphicsParameters()  );

// Plot a time sequence of 1D functions to generate a surface
virtual void
plot(const realArray & x, 
     const realArray & t,
     const realArray & u, 
     GraphicsParameters & parameters =Overture::defaultGraphicsParameters() );

//
// non-virtual functions:
//

// output a grid function or grid data to an ascii file.
int
fileOutput( realGridCollectionFunction & u );
  
// Plot the boundaries of a 2D grid
void
plotGridBoundaries(const GridCollection & cg, const int colourOption=0, const real zRaise=0.,
		   GraphicsParameters & parameters=Overture::defaultGraphicsParameters());

  int
  plotUnstructured(const UnstructuredMapping & map, GraphicsParameters & par,
		   int dList=0, bool lit=FALSE);

int
plotAdvancingFront(const AdvancingFront &front,
		   GraphicsParameters & par, const int plotOptions);

protected:
int 
bcNumber( const int bc );

void
grid3d(const GridCollection & cg, GraphicsParameters & parameters,
       IntegerArray & numberList, 
       int & number, int list, int litList);

void
streamLines2d(GridCollection & gc, 
	      const realGridCollectionFunction & uv, 
	      GraphicsParameters & parameters);

void
streamLines3d(GridCollection & gc, 
	      const realGridCollectionFunction & uv, 
	      GraphicsParameters & parameters);

void
drawAStreamLine(const GridCollection & cg, const realGridCollectionFunction & uv,
		real arrowScaleFactor, GraphicsParameters & parameters);

void
contour1d(const realGridCollectionFunction & u, 
	  GraphicsParameters & parameters=Overture::defaultGraphicsParameters());

void
contour2d(const realGridCollectionFunction & u, 
	  GraphicsParameters & parameters=Overture::defaultGraphicsParameters());

void
contour3d(const realGridCollectionFunction & u, 
	  GraphicsParameters & parameters=Overture::defaultGraphicsParameters());

int
drawContourLinesOnAnElement(const RealArray & u,
			    const RealArray & x,
			    const int numberOfVerticies,
			    const real deltaU,
			    const real deltaUInverse,
			    const real uMin, 
			    const real uMax,
			    const real uAverage,
			    const real uScaleFactor,
			    const real uRaise,
			    const RealArray & contourLevels,
			    bool & lineStipple,
			    bool contourLevelsSpecified,
			    GraphicsParameters & psp );
  
void
contourCuts( const realGridCollectionFunction & uGCF, GraphicsParameters & parameters);
  
void
getGridBounds(const GridCollection & gc, GraphicsParameters & params, RealArray & xBound);

void
getPlotBounds(const GridCollection & gc, GraphicsParameters & params, RealArray & xBound);

void
getBounds(const realGridCollectionFunction & u,
	  real & uMin, 
	  real & uMax,
	  GraphicsParameters & parameters,   
	  const Range & R0=nullRange,      // check these entries of component 0
	  const Range & R1=nullRange,      // check these entries of component 1
	  const Range & R2=nullRange,
	  const Range & R3=nullRange,
	  const Range & R4=nullRange);
  
void
getNormal( const MappedGrid & mg, const IntegerArray & iv, const int axis, RealArray & normal);

void
plotShadedFace(const MappedGrid & c,
	       const Index & I1, 
	       const Index & I2, 
	       const Index & I3, 
	       const int axis,
	       const int side,
	       GraphicsParameters & parameters );

// for grid plots
IntegerArray boundaryConditionList;
int numberOfBoundaryConditions;

// for streamlines:
IntegerArray maskForStreamLines;
IntegerArray componentsToInterpolate;
};
  
#endif
