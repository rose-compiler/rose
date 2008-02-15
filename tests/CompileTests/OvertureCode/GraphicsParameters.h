#ifndef GRAPHICS_PARAMETERS_H
#define GRAPHICS_PARAMETERS_H

#include "Mapping.h"

class GenericGraphicsInterface;  // forward declaration
class PlotStuff;  // forward declaration

// Here is the form of a user defined colour table
typedef void (*ColourTableFunctionPointer)(const real & value, real & red, real & green, real & blue);
// /Description: Convert a value from [0,1] into (red,green,blue) values, each in the range [0,1]
// /value (input) :  0 <= value <= 1 
// /red, green, blue (output) : values in the range [0,1]

enum GraphicsOptions
{
  GI_AXES_ORIGIN,  
  GI_BOUNDARY_COLOUR_OPTION,
  GI_BACK_GROUND_GRID_FOR_STREAM_LINES,
  GI_BLOCK_BOUNDARY_COLOUR_OPTION,
  GI_BOTTOM_LABEL,
  GI_BOTTOM_LABEL_SUP_1,
  GI_BOTTOM_LABEL_SUP_2,
  GI_BOTTOM_LABEL_SUP_3,
  GI_COLOUR_INTERPOLATION_POINTS,
  GI_COLOUR_LINE_CONTOURS,
  GI_CONTOUR_ON_GRID_FACE,
  GI_CONTOUR_SURFACE_VERTICAL_SCALE_FACTOR,
  GI_COLOUR_TABLE,
  GI_COMPONENT_FOR_CONTOURS,
  GI_COMPONENT_FOR_SURFACE_CONTOURS,
  GI_COMPONENTS_TO_PLOT,
  GI_CONTOUR_LEVELS,
  GI_COORDINATE_PLANES,
  GI_GRID_COORDINATE_PLANES,
  GI_GRID_BOUNDARY_CONDITION_OPTIONS,
  GI_GRID_OPTIONS,
  GI_GRID_LINES,
  GI_GRID_LINE_COLOUR_OPTION,
  GI_GRIDS_TO_PLOT,
  GI_HARD_COPY_TYPE,
  GI_ISO_SURFACE_VALUES,
  GI_KEEP_ASPECT_RATIO,
  GI_LABEL_GRIDS_AND_BOUNDARIES,
  GI_LINE_OFFSET,
  GI_MAPPING_COLOUR,
  GI_MINIMUM_CONTOUR_SPACING,
  GI_MIN_AND_MAX_CONTOUR_LEVELS,
  GI_MIN_AND_MAX_STREAM_LINES,
  GI_MULTIGRID_LEVEL_TO_PLOT,
  GI_NUMBER_OF_CONTOUR_LEVELS,
  GI_NUMBER_OF_GHOST_LINES_TO_PLOT,
  GI_OUTPUT_FORMAT,
  GI_PLOT_BOUNDS,
  GI_PLOT_BACK_GROUND_GRID,
  GI_PLOT_BACKUP_INTERPOLATION_POINTS,
  GI_PLOT_BLOCK_BOUNDARIES,
  GI_PLOT_COLOUR_BAR,
  GI_PLOT_CONTOUR_LINES,
  GI_PLOT_LABELS,
  GI_PLOT_LINES_ON_GRID_BOUNDARIES,
  GI_PLOT_LINES_ON_MAPPING_BOUNDARIES,
  GI_PLOT_MAPPING_EDGES,
  GI_PLOT_GRID_BOUNDARIES_ON_CONTOUR_PLOTS,
  GI_PLOT_GRID_LINES,
  GI_PLOT_GRID_POINTS_ON_CURVES,
  GI_PLOT_INTERPOLATION_POINTS,
  GI_PLOT_INTERPOLATION_CELLS,
  GI_PLOT_NON_PHYSICAL_BOUNDARIES,
  GI_PLOT_SHADED_MAPPING_BOUNDARIES,
  GI_PLOT_SHADED_SURFACE,
  GI_PLOT_SHADED_SURFACE_GRIDS,
  GI_PLOT_THE_AXES,
  GI_PLOT_THE_OBJECT,
  GI_PLOT_THE_OBJECT_AND_EXIT,
  GI_PLOT_WIRE_FRAME,
  GI_POINT_COLOUR,
  GI_POINT_OFFSET,
  GI_POINT_SIZE,
  GI_POINT_SYMBOL,
  GI_RASTER_RESOLUTION,
  GI_REFINEMENT_LEVEL_TO_PLOT,
  GI_SURFACE_OFFSET,
  GI_STREAM_LINE_TOLERANCE,
  GI_U_COMPONENT_FOR_STREAM_LINES,
  GI_V_COMPONENT_FOR_STREAM_LINES,
  GI_W_COMPONENT_FOR_STREAM_LINES,
  GI_TOP_LABEL,
  GI_TOP_LABEL_SUB_1,
  GI_TOP_LABEL_SUB_2,
  GI_TOP_LABEL_SUB_3,
  GI_USE_PLOT_BOUNDS,
  GI_USE_PLOT_BOUNDS_OR_LARGER,
  GI_PLOT_2D_CONTOURS_ON_COORDINATE_PLANES,
  GI_NORMAL_AXIS_FOR_2D_CONTOURS_ON_COORDINATE_PLANES,
  GI_Y_LEVEL_FOR_1D_GRIDS,
  GI_Z_LEVEL_FOR_2D_GRIDS,
  GI_PLOT_UNS_NODES,
  GI_PLOT_UNS_FACES,
  GI_PLOT_UNS_EDGES,
  GI_PLOT_UNS_BOUNDARY_EDGES
};

class GraphicsParameters
{
public:

enum ColourTables
{
  rainbow,
  gray,
  red,
  green,
  blue,
  userDefined,
  numberOfColourTables
} colourTable;

enum HardCopyType
{
  postScript,       // post script file with drawing commands
  encapsulatedPostScript,
  postScriptRaster, // post script containing a bit map image of the screen
  ppm               // portable pixmap format (P6 binary format)
};

enum OutputFormat   // formats for outputing postscript files
{
  colour8Bit,       // compressed colour file with 225 colours
  colour24Bit,      // 24 bits of colour (2^24 colours)
  blackAndWhite,    // black and white
  grayScale         // 8 bit gray scale (2^8 shades of gray)
};

enum Sizes // **** remember to change the version of this list in GenericGraphicsInterface
{
  lineWidth,       // basic width in pixels for a line
  axisNumberSize,  // size of number labels on the axes
  axisLabelSize,   // size of axis label ("x-axis")
  axisMinorTickSize,
  axisMajorTickSize,
  topLabelSize,     
  topSubLabelSize,    
  bottomLabelSize,
  bottomSubLabelSize,
  minorContourWidth,  // this times lineWidth = actual width in pixels for minor contour lines
  majorContourWidth,  // this times lineWidth = actual width in pixels for major contour lines
  streamLineWidth,    // this times lineWidth = actual width in pixels for stream lines
  labelLineWidth,     // this times lineWidth = actual width in pixels for lines that make up a character
  curveLineWidth,     // this times lineWidth = actual width in pixels for curves drawn
  extraSize2,
  extraSize3,
  extraSize4,
  numberOfSizes    // counts number of entries in this list
};
  
enum GridOptions   // bit-map values for grid plotting options: gridOption(grid)
{
  plotGrid=1,
  plotInterpolation=2,
  plotShadedSurfaces=4,
  plotInteriorGridLines=8,
  plotBoundaryGridLines=16,
  plotBlockBoundaries=32,
  plotBackupInterpolation=64
};

enum ColourOptions  // options for colouring boundaries, grids lines and block boundaries
{ 
  defaultColour,   // default 
  colourByGrid,
  colourByRefinementLevel,
  colourByBoundaryCondition,
  colourByShare,
  colourByValue,
  colourBlack
};

GraphicsParameters(bool default0=FALSE); 

~GraphicsParameters(); 

bool 
isDefault();

// use these functions to determine current values for parameters
aString &       
get(const GraphicsOptions & option, aString & label) const;
int &          
get(const GraphicsOptions & option, int & value) const;  
real &         
get(const GraphicsOptions & option, real & value) const;  
IntegerArray & 
get(const GraphicsOptions & option, IntegerArray & values) const;
RealArray &    
get(const GraphicsOptions & option, RealArray & values) const;
real &         
get(const Sizes & option, real & value) const;  

// use these set functions to set a value for a GraphicsOptions parameter
int 
set(const GraphicsOptions & option, const aString & label);
int 
set(const GraphicsOptions & option, int value);  
int 
set(const GraphicsOptions & option, real value);  
int 
set(const GraphicsOptions & option, const IntegerArray & values);
int 
set(const GraphicsOptions & option, const RealArray & values);
int 
set(const Sizes & option, real value);  // set a size

// use these functions for setting a PlotStuffOptions parameter
//  int 
//  set(const PlotStuffOptions & option, const aString & label);
//  int 
//  set(const PlotStuffOptions & option, int value);  
//  int 
//  set(const PlotStuffOptions & option, real value);  
//  int 
//  set(const PlotStuffOptions & option, const IntegerArray & values);
//  int 
//  set(const PlotStuffOptions & option, const RealArray & values);

int 
setColourTable(ColourTableFunctionPointer ctf); // provide a function to use for a colour table

//access routines
bool 
getObjectWasPlotted() const;  // true if an object was plotted

friend
class GenericGraphicsInterface;  
friend 
class GL_GraphicsInterface;
friend 
class PlotStuff;

friend int 
viewMappings( MappingInformation & mapInfo );
friend int
plotAListOfMappings(MappingInformation & mapInfo,    // get rid of this eventually
		    const int & mapNumberToPlot,
		    int & numberOfMapsPlotted,   
		    IntegerArray & listOfMapsToPlot, 
		    aString *localColourNames,
		    const int & numberOfColourNames,
		    const bool & plotTheAxes,
		    GraphicsParameters & params );
friend int 
readMappings( MappingInformation & mapInfo ); // get rid of this eventually
  
// some accessfunctions
bool & 
getLabelGridsAndBoundaries(){ return labelGridsAndBoundaries; }
int  & 
getBoundaryColourOption(){ return boundaryColourOption; }
bool & 
getPlotShadedSurface(){ return plotShadedSurface; }
bool & 
getPlotLinesOnMappingBoundaries(){ return plotLinesOnMappingBoundaries; }
bool & 
getPlotNonPhysicalBoundaries(){ return plotNonPhysicalBoundaries; }
bool & 
getPlotGridPointsOnCurves(){ return plotGridPointsOnCurves; }
int  & 
getNumberOfGhostLinesToPlot(){ return numberOfGhostLinesToPlot; }
aString & 
getMappingColour(){ return mappingColour; }

protected:   // should be protected but GL_GraphicsInterface needs these

enum ToggledItems  // label bit flags used by gridsToPlot
{
  toggleGrids=1,
  toggleContours=2,
  toggleStreamLines=4,
  toggleSum=1+2+4
};

// parameters used generally:
aString topLabel,topLabel1,topLabel2,topLabel3;
aString bottomLabel,bottomLabel1,bottomLabel2,bottomLabel3;
bool plotObject;                  // immediately plot the object
bool plotObjectAndExit;           // immediately plot the object then exit
bool usePlotBounds;               // use plot bounds found in the plotBound array
bool usePlotBoundsOrLarger;      // plot bounds should include bounds from plotBound array
bool plotTheAxes;
RealArray plotBound;              // plotting bounds: plotBound(2,3)
bool objectWasPlotted;            // true on exit if object was plotted (and not erased)
int numberOfGhostLinesToPlot;
RealArray size;                   // holds sizes for items in the Sizes enum
bool plotBoundsChanged;  // true if the plot bounds were changed
real relativeChangeForPlotBounds;  // use with usePlotBoundsOrLarger
int multigridLevelToPlot;
int refinementLevelToPlot;
bool keepAspectRatio;            // default true. Keep the aspect ratio.

HardCopyType hardCopyType;
OutputFormat outputFormat;
int rasterResolution;       // resolution for off screen renderer
  
bool plotTitleLabels;     // plot the top title etc.
  
// parameters used to plot Mappings
aString mappingColour;
bool plotGridPointsOnCurves;
bool plotLinesOnMappingBoundaries;
bool plotNonPhysicalBoundaries;
bool plotShadedMappingBoundaries;
bool plotMappingEdges;
real lineOffset, pointOffset, surfaceOffset;      // shift (lines/points/polygons) this many "units" before plotting

// parameters for plotting grids
IntegerArray gridsToPlot;                     // bit flag: 1=plot grid, 2=plot contours, 4=plot streamlines.
IntegerArray gridOptions;                     // bit flag holding various options
IntegerArray gridBoundaryConditionOptions;    // another bit flag
bool colourInterpolationPoints;
bool plotInterpolationPoints;
bool plotInterpolationCells;  
bool plotBackupInterpolationPoints;
bool plotBranchCuts;
bool plotLinesOnGridBoundaries;
int plotGridLines;               // bit 1 for 2d, bit 2 for 3d
bool plotGridBlockBoundaries;
bool plotShadedSurfaceGrids;
bool plotUnsNodes, plotUnsFaces, plotUnsEdges, plotUnsBoundaryEdges;
bool labelBoundaries;

int boundaryColourOption;      // 0=default, 1=by grid, 2=by refinement level, 3=BC, 4=by share
int gridLineColourOption;      // 0=default, 1=by grid, 2=by refinement level
int blockBoundaryColourOption; // 0=default, 1=by grid, 2=by refinement level, 3=BC, 4=by share
  
int boundaryColourValue;
int gridLineColourValue;
int blockBoundaryColourValue;
  
//  bool colourBoundariesByBoundaryConditions;
//  bool colourGridByRefinementLevel;
//  bool colourBoundariesByShare;
//  bool colourBlockBoundaries;
real yLevelFor1DGrids;  // for 1d grid plots.
real zLevelFor2DGrids;
bool labelGridsAndBoundaries;
  
// These are for the contour plotter:
bool plotContourLines;
bool plotShadedSurface;
int numberOfContourLevels;
bool plotWireFrame;
bool colourLineContours;
bool plotColourBar;
int componentForContours;
int componentForSurfaceContours;  // component for 3D contours on grid boundaries
IntegerArray componentsToPlot;  // for 1d contour plots, multiple components
IntegerArray minAndMaxContourLevelsSpecified;
RealArray minAndMaxContourLevels;            // for user specified min and max contour levels
real minimumContourSpacing;
RealArray contourLevels;
bool plotDashedLinesForNegativeContours;
RealArray axesOrigin; 
bool plotGridBoundariesOnContourPlots;
real contourSurfaceVerticalScaleFactor;
  
bool linePlots;                // plot solution on lines that intersect the range
int numberOfLines;
int numberOfPointsPerLine;
RealArray linePlotEndPoints;

// 3d contour plotter
// int numberOfNormalsForContourPlanes;
// IntegerArray numberOfPlanesPerNormal;
// RealArray normalForContourPlane;
// RealArray pointOnContourPlane;
int numberOfContourPlanes;
RealArray contourPlane;          //  contourPlane(0:5,number)   0:2 = normal, 3:5 = point on plane

IntegerArray plotContourOnGridFace;
bool plot2DContoursOnCoordinatePlanes;
int normalAxisFor2DContoursOnCoordinatePlanes;
int numberOfIsoSurfaces;
RealArray isoSurfaceValue;
int numberOfCoordinatePlanes;
IntegerArray coordinatePlane;
int numberOfGridCoordinatePlanes;
IntegerArray gridCoordinatePlane;

// These are for streamlines
IntegerArray backGroundGridDimension;   // number of points on back ground grid backGroundGridDimension(3)
int uComponentForStreamLines;
int vComponentForStreamLines;
int wComponentForStreamLines;
real minStreamLine, maxStreamLine;   // determines how colours appear
real streamLineStoppingTolerance;    // stop drawing when velocity decreases by this much
bool minAndMaxStreamLinesSpecified;
  
// for 3d stream lines
int numberOfStreamLineStartingPoints;
realArray streamLineStartingPoint;
IntegerArray numberOfRakePoints;
realArray rakeEndPoint;
IntegerArray numberOfRectanglePoints;
realArray rectanglePoint;

// for plotting points:
real pointSize;
int pointSymbol;
aString pointColour;

bool defaultObject;   

ColourTableFunctionPointer colourTableFunction;

};

#endif
