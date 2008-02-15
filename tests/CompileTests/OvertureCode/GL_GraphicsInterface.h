#ifndef GL_GRAPHICS_INTERFACE_H 
#define GL_GRAPHICS_INTERFACE_H 

//============================================================================================
//     -----------------------------------
//     ------GL_GraphicsInterface---------
//     -----------------------------------
//
//  This class is a graphics interface based on OpenGL. It uses OpenGL for graphics
//  and MOTIF to manage windows and menues. Command files are
//  supported through the base class, GenericGraphicsInterface
//
//
//   This class can plot 
//     o Mappings of any sort
//
//  Optional plotting parameters are passed using an object of the GraphicsParameters class.
//  An object of this class can be used to set many parameters such as the labels on the
//  plots, whether to plot axes, whether to plot line contours etc.
//
// 
//===========================================================================================

#include "GenericGraphicsInterface.h"
#include "GraphicsParameters.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "mogl.h"
#include "GUIState.h"

// extern intArray Overture::nullIntegerDistributedArray();

// define these for the sgi
#if defined(__sgi) && defined(GL_EXT_polygon_offset)
#define glPolygonOffset glPolygonOffsetEXT
#ifndef GL_POLYGON_OFFSET_FILL
#define GL_POLYGON_OFFSET_FILL GL_POLYGON_OFFSET_EXT
#endif
#define OFFSET_FACTOR .00001
#else
#define OFFSET_FACTOR 1.
#endif

// define single and double precision versions of the OpenGL functions
#ifndef OV_USE_DOUBLE
// here are single precision definitions:
#define glColor3     glColor3f
#define glMultMatrix glMultMatrixf
#define glNormal3v   glNormal3fv
#define glRotate     glRotatef
#define glScale      glScalef
#define glTranslate  glTranslatef
#define glVertex2    glVertex2f
#define glVertex3    glVertex3f
#define glVertex2v   glVertex2fv
#define glVertex3v   glVertex3fv
#else
#define glColor3     glColor3d
#define glMultMatrix glMultMatrixd
#define glNormal3v   glNormal3dv
#define glRotate     glRotated
#define glScale      glScaled
#define glTranslate  glTranslated
#define glVertex2    glVertex2d
#define glVertex3    glVertex3d
#define glVertex2v   glVertex2dv
#define glVertex3v   glVertex3dv
#endif

// More feed back tokens for pass through tokens:
enum GLenum2
{
  GL_DISABLE_TOKEN              = 0x0701,
  GL_ENABLE_TOKEN               = 0x0702,
  GL_LINE_WIDTH_TOKEN		= 0x0703,
  GL_POINT_SIZE_TOKEN		= 0x0704,
  GL_LINE_STIPPLE_TOKEN         = 0x0705 
};

class UnstructuredMapping;




class GL_GraphicsInterface : public GenericGraphicsInterface
{
public:
  
// Default constructor (will open a window)
GL_GraphicsInterface(const aString & windowTitle = nullString);

// Constructor that takes argc and argv from main program and strips away Motif parameters
GL_GraphicsInterface(int & argc, char *argv[], const aString & windowTitle = nullString );

//  This Constructor will only create a window if the the argument initialize=TRUE
//  To create a window later, call createWindow()
GL_GraphicsInterface( const bool initialize, const aString & windowTitle = nullString );

~GL_GraphicsInterface();
  
// Open another graphics window (and also a command window if it isn't already opened)
virtual int 
createWindow(const aString & windowTitle = nullString,
	     int argc=0, 
	     char *argv[] = NULL );
  
// display the screen, not normally called by a user. 
void 
display(const int & win_number); 

// set the model view matrix for the current bounds
void 
setModelViewMatrix();

// initialize the screen, not normally called by a user.
void 
init(const int & win_number);    
  
// display help on a topic in the help pull-down  menu
bool
displayHelp( const aString & topic );

// destroy the window 
virtual int
destroyWindow(int win_number);

// Put up a menu and wait for a response, return the menu item chosen as answer,
// The return value is the item number (0,1,2,... ) (return -1 if there is an error)
// An error can occur if we are reading a command file
virtual int
getMenuItem(const aString *menu, aString & answer, const aString & prompt=nullString);
  
// Erase all graphics display lists and delete the ones that aren't hidable
virtual void 
erase(); 

// erases all display lists in window win_number.
virtual void 
erase(const int & win_number, bool forceDelete = false);

// erase a list of display lists
virtual void
erase(const IntegerArray & displayList);

// input a filename through a file selection dialog
virtual void
inputFileName(aString &answer, const aString & prompt=nullString, const aString &extension=nullString);

// Input a string after displaying an optional prompt
virtual void
inputString(aString &answer, const aString & prompt=nullString);

// output a string in the message window
virtual void
outputString(const aString & message);

// Redraw all graphics display list
virtual void
redraw( bool immediate=FALSE );

// Reset the view to default
void
resetView(const int & win_number); // AP: make it protected?

// Select objects using the cursor
//  virtual int
//  select(IntegerArray & selection,  const aString & prompt=nullString );
  
// This routine sets the projection and modelview to a normalized system on [-1,1]x[-1,1]
void
setNormalizedCoordinates();

// This routine un-does the previous function
void
unsetNormalizedCoordinates();

// convert normalized coordinates [-1,+1] to world (globalBound) coordinates
virtual int
normalizedToWorldCoordinates(const RealArray & r, RealArray & x ) const;

// convert pick [0,1] + zBuffer [0,2**m] coordinates to world coordinates. 
virtual int
pickToWorldCoordinates(const realArray & r, realArray & x, const int & win_number ) const;

// functions for setting the bounding box and the rotation center
virtual void
setGlobalBound(const RealArray &xBound);

virtual RealArray
getGlobalBound() const;

void
setRotationCenter(real rotationPoint[3], int win_number=-1);

virtual void
resetGlobalBound(const int &win_number); 

// plot points
void
plotPoints(const realArray & points, GraphicsParameters & parameters =Overture::defaultGraphicsParameters() );

void
plotPoints(const realArray & points, 
	   const realArray & value,
	   GraphicsParameters & parameters =Overture::defaultGraphicsParameters() );

//  choose a colour
virtual aString
chooseAColour();

  //  set the colour for subsequent objects that are plotted
virtual int
setColour( const aString & colourName );

 // set colour to default for a given type of item
virtual int
setColour( ItemColourEnum item ); 

virtual aString
getColourName( int i ) const;

// This utility routines plot Axes
void
plotAxes(const RealArray & xBound, 
	 const int numberOfDimensions,
	 GraphicsParameters & parameters=Overture::defaultGraphicsParameters(),
	 int win_number = -1);

// erase the axes
void
eraseAxes(int win_number);  

// Draw coloured squares with a number inside them to label colours on the plot
virtual void
drawColouredSquares(const IntegerArray numberList,
		    GraphicsParameters & parameters=Overture::defaultGraphicsParameters(),
		    const int & numberOfColourNames = -1 , // use default colours by default
		    aString *colourNames = NULL);
  

// This utility routines plots a label, this label does NOT rotate or scale with the plot
void
label(const aString & string,     
      real xPosition, 
      real yPosition,
      real size=.1,
      int centering=0, 
      real angle=0.,
      GraphicsParameters & parameters=Overture::defaultGraphicsParameters());
  
// This label uses raster fonts  **** this is not finished *****
void
labelR(const aString & string,     
       const real xPosition, 
       const real yPosition,
       const real size=.1,
       const int centering=0, 
       const real angle=0.,
       GraphicsParameters & parameters=Overture::defaultGraphicsParameters());
  
// Plot a label in 2D world coordinates
// This label DOES rotate and scale with the plot
void
xLabel(const aString & string,     
       const real xPosition, 
       const real yPosition,
       const real size=.1,
       const int centering=0,    // -1=left justify, 0=center, 1=right justify
       const real angle=0.,
       GraphicsParameters & parameters=Overture::defaultGraphicsParameters(),
       int win_number = -1);
  
// Plot a label in 3D world coordinates
// This label DOES rotate and scale with the plot
void
xLabel(const aString & string,     
       const RealArray & x,    // supply 3 position coordinates
       const real size=.1,    
       const int centering=0, 
       const real angle=0.,   
       GraphicsParameters & parameters =Overture::defaultGraphicsParameters(),
       int win_number = -1);

void
xLabel(const aString & string,     
       const real x[3], 
       const real size =.1,
       const int centering = 0,
       const real angle = 0.,
       GraphicsParameters & parameters  =Overture::defaultGraphicsParameters(),
       int win_number = -1 );

// Plot a label with position and size in World coordinates, 
// This label DOES rotate and scale with the plot. This version of xLabel 
// plots the string in the plane formed by the vectors {\ff rightVector}
// and {\ff upVector}.
void
xLabel(const aString & string,     
       const RealArray & x,     // supply 3 position coordinates
       const real size,         // size in world coordinates
       const int centering,
       const RealArray & rightVector,   // string lies parallel to this vector
       const RealArray & upVector,      // in the plane of these two vectors
       GraphicsParameters & parameters=Overture::defaultGraphicsParameters(),
       int win_number = -1);

void
xLabel(const aString & string,     
       const real x[3],  
       const real size,      
       const int centering,
       const real rightVector[3],  
       const real upVector[3],
       GraphicsParameters & parameters  =Overture::defaultGraphicsParameters(),
       int win_number = -1);


// Draw a colour bar. It is defined in contour.C
void
drawColourBar(const int & numberOfContourLevels,
	      RealArray & contourLevels,
	      real uMin=0., 
	      real uMax=1.,
	      GraphicsParameters & parameters=Overture::defaultGraphicsParameters(),
	      real xLeft=.775,  // .8
	      real xRight=.825, // .85
	      real yBottom=-.75,
	      real yTop=.75);

// erase the colour bar.
void
eraseColourBar();

//  Save the graphics window in hard-copy form
virtual int
hardCopy(const aString & fileName=nullString, 
	 GraphicsParameters & parameters=Overture::defaultGraphicsParameters(),
	 int win_number=-1);
  

// render directly to post-script without a raster
int
renderPS(const char * fileName, 
	 GraphicsParameters & parameters =Overture::defaultGraphicsParameters() );
  
int
psToRaster(const aString & fileName,
	   const aString & ppmFileName );
  
enum
{
  numberOfColourNames=25
};

aString *colourNames;
aString *fileMenuItems;
aString *helpMenuItems;
aString *graphicsFileMenuItems;
aString *graphicsHelpMenuItems;
aString *menuBarItems;

// Set some view parameters
enum ViewParameters
{
  xAxisAngle,      // angle to rotate about x-axis (absolute value, not incremental)
  yAxisAngle,
  zAxisAngle,
  xTranslation,
  yTranslation,
  zTranslation,
  magnification
};

enum displayListProperty{ // bitwise flags for handling display lists.
  lightDL = 1, // if this bit is set, the display list should be plotted with lighting, otherwise not.
  plotDL  = 2, // if this bit is set, the display list should be plotted, otherwise not
  hideableDL = 4  // if this bit is set, the display list can be hidden (i.e., not plotted) instead of
// getting erased.
};

void 
setView(const ViewParameters & viewParameter, const real & value);

  // this next routine is normally only called internally. Make it protected?
void
changeView(const int & win_number,
	   const float & dx,   
	   const float & dy , 
	   const float & dz,
	   const float & dThetaX=0.,
	   const float & dThetaY=0.,
	   const float & dThetaZ=0.,
	   const float & magnify=1. );
  

void
setRotationTransformation(int win_number);


virtual int
generateNewDisplayList(bool lit = false, bool plotIt = true, bool hideable = false);
  
virtual void
setLighting(int list, bool lit);

void
setPlotDL(int list, bool plot);

// access functions for display lists
virtual const int & 
getFirstDL(const int & win){ return topLabelList[win]; }

virtual const int & 
getFirstFixedDL(const int & win){ return topLabelList[win]; }

virtual const int & 
getTopLabelDL(const int & win){ return topLabelList[win]; }

virtual const int & 
getTopLabel1DL(const int & win){ return topLabel1List[win]; }

virtual const int & 
getTopLabel2DL(const int & win){ return topLabel2List[win]; }

virtual const int & 
getTopLabel3DL(const int & win){ return topLabel3List[win]; }

virtual const int & 
getBottomLabelDL(const int & win){ return bottomLabelList[win]; }

virtual const int & 
getBottomLabel1DL(const int & win){ return bottomLabel1List[win]; }

virtual const int & 
getBottomLabel2DL(const int & win){ return bottomLabel2List[win]; }

virtual const int & 
getBottomLabel3DL(const int & win){ return bottomLabel3List[win]; }

virtual const int & 
getColouredSquaresDL(const int & win){ return colouredSquaresList[win]; }

virtual const int & 
getColourBarDL(const int & win){ return colourBarList[win]; }

virtual const int & 
getFirstUserLabelDL(const int & win){ return firstUserLabelList[win]; }

virtual const int & 
getLastUserLabelDL(const int & win){ return lastUserLabelList[win]; }

virtual const int & 
getLastFixedDL(const int & win){ return lastUserLabelList[win]; }

virtual const int & 
getFirstRotableDL(const int & win){ return axesList[win]; }

virtual const int & 
getAxesDL(const int & win){ return axesList[win]; }

virtual const int & 
getFirstUserRotableDL(const int & win){ return firstUserRotableList[win]; }

virtual const int & 
getLastUserRotableDL(const int & win){ return lastUserRotableList[win]; }

virtual const int & 
getLastRotableDL(const int & win){ return lastUserRotableList[win]; }

virtual const int & 
getLastDL(const int & win){ return lastUserRotableList[win]; }

virtual const int & 
getMaxNOfDL(const int & win){ return maximumNumberOfDisplayLists[win]; }

int
setAxesLabels( const aString & xAxisLabel=blankString,
	       const aString & yAxisLabel=blankString,
	       const aString & zAxisLabel=blankString );
  
int
setAxesOrigin( const real & x0=defaultOrigin , const real & y0=defaultOrigin , const real&  z0=defaultOrigin );

virtual int 
getCurrentWindow();

GUIState * 
getCurrentGUI(){ return currentGUI;}

void
setCurrentWindow(const int & w);

// access function for the variable plotTheAxes
virtual int & 
sgPlotTheAxes(){return plotTheAxes[currentWindow];} // lhs & rhs

// access function for the variable plotBackGroundGrid
virtual int & 
sgPlotBackGroundGrid(){return plotBackGroundGrid[currentWindow];};

//  virtual void
//  setUserMenu(const aString menu[], const aString & menuTitle);

//  virtual void
//  setUserButtons(const aString buttons[][2]);

virtual void
pollEvents();

// Set scale factor for line widths (this can be used to increase the line widths for
// high-res off screen rendering.
void 
setLineWidthScaleFactor(const real & lineWidthScaleFactor = 1, int win_number = -1 );

virtual int
getAnswer(aString & answer, const aString & prompt);

virtual int
getAnswer(aString & answer, const aString & prompt,
	  SelectionInfo &selection);

//  virtual int
//  getAnswer(aString & answer, const aString & prompt,
//  	  PickInfo3D &pick);

//  virtual int
//  getAnswer(aString & answer, const aString & prompt,
//  	  SelectionInfo &selection, PickInfo3D &pick);

int
getAnswerNoBlock(aString & answer, const aString & prompt);


virtual int
pickPoints( realArray & x, 
	    bool plotPoints = TRUE,
	    int win_number = -1 );

void
pushGUI( GUIState &newState );

void
popGUI();


virtual int
beginRecordDisplayLists( IntegerArray & displayLists);

virtual int
endRecordDisplayLists( IntegerArray & displayLists);

virtual void
createMessageDialog(aString msg, MessageTypeEnum type);

int
pause();

protected:

void
lightsOn(int win_number);

void
lightsOff(int win_number);

int currentWindow;

// plotting routines should only use display lists up to numberOfAvailableDisplayLists.
// a few more are reserved for labels and other stuff
int topLabelList[MAX_WINDOWS],                     // display list number for the top label
  topLabel1List[MAX_WINDOWS],             // display list number for the top label sub 1
  topLabel2List[MAX_WINDOWS],
  topLabel3List[MAX_WINDOWS],
  bottomLabelList[MAX_WINDOWS],
  bottomLabel1List[MAX_WINDOWS],
  bottomLabel2List[MAX_WINDOWS],
  bottomLabel3List[MAX_WINDOWS],
  colouredSquaresList[MAX_WINDOWS],
  colourBarList[MAX_WINDOWS],
  firstUserLabelList[MAX_WINDOWS],                        
  lastUserLabelList[MAX_WINDOWS], /* = firstUserLabelList+100 */ 
  axesList[MAX_WINDOWS],             // axes are rotated
  firstUserRotableList[MAX_WINDOWS],                        
  lastUserRotableList[MAX_WINDOWS],                        
  maximumNumberOfDisplayLists[MAX_WINDOWS]; // this is the actual number that we have

// is the lighting on in the list?
// should the list be plotted?
// should the list be erased or just not plotted?
IntegerArray plotInfo[MAX_WINDOWS]; 

bool keepAspectRatio[MAX_WINDOWS];   // usually we keep the aspect ratio for plots.
  
bool viewHasChanged[MAX_WINDOWS];

// global bounding box and rotation center
RealArray globalBound[MAX_WINDOWS];
real rotationCenter[MAX_WINDOWS][3];

real magnificationFactor[MAX_WINDOWS], magnificationIncrement[MAX_WINDOWS];
real aspectRatio[MAX_WINDOWS], fractionOfScreen[MAX_WINDOWS];
  
aString wTitle[MAX_WINDOWS];

aString defaultWindowTitle;
GraphicsParameters::HardCopyType hardCopyType[MAX_WINDOWS];
GraphicsParameters::OutputFormat outputFormat[MAX_WINDOWS];

aString hardCopyFile[MAX_WINDOWS];
int rasterResolution[MAX_WINDOWS];       // resolution for off screen renderer

aString movieBaseName[MAX_WINDOWS];
bool saveMovie[MAX_WINDOWS];
int numberOfMovieFrames[MAX_WINDOWS], movieFirstFrame[MAX_WINDOWS];
real movieDxRot[MAX_WINDOWS], movieDyRot[MAX_WINDOWS], movieDzRot[MAX_WINDOWS];
real movieDxTrans[MAX_WINDOWS], movieDyTrans[MAX_WINDOWS], movieDzTrans[MAX_WINDOWS];
real movieRelZoom[MAX_WINDOWS];

// hardcopy & movie dialog windows
DialogData hardCopyDialog[MAX_WINDOWS];
DialogData movieDialog[MAX_WINDOWS];

int userLabel[MAX_WINDOWS];              // current number of userLabels;
int labelsPlotted[MAX_WINDOWS];          // true if the top/bottom labels have been plotted.

real xEyeCoordinate, yEyeCoordinate, zEyeCoordinate;  // position of the eye
real dtx[MAX_WINDOWS], dty[MAX_WINDOWS], dtz[MAX_WINDOWS];
real xShift[MAX_WINDOWS], yShift[MAX_WINDOWS], zShift[MAX_WINDOWS];

real deltaX[MAX_WINDOWS], deltaY[MAX_WINDOWS], deltaZ[MAX_WINDOWS], deltaAngle[MAX_WINDOWS];
real defaultNear[MAX_WINDOWS], defaultFar[MAX_WINDOWS];

real leftSide[MAX_WINDOWS], rightSide[MAX_WINDOWS];
real bottom[MAX_WINDOWS], top[MAX_WINDOWS], near[MAX_WINDOWS], far[MAX_WINDOWS]; 

RealArray rotationMatrix[MAX_WINDOWS], matrix[MAX_WINDOWS];
Index I4;

real windowScaleFactor[MAX_WINDOWS][3];      // current scale factors for the plotting window.
bool userDefinedRotationPoint[MAX_WINDOWS];  // TRUE if the user has defined the rotation center

int axesOriginOption[MAX_WINDOWS];           // indicates where to place the axes origin
RealArray axesOrigin[MAX_WINDOWS];

aString xAxisLabel[MAX_WINDOWS];
aString yAxisLabel[MAX_WINDOWS];
aString zAxisLabel[MAX_WINDOWS];

real shiftCorrection[MAX_WINDOWS][3]; // default correction for changing the rotation center

real backGround[MAX_WINDOWS][4];  // back ground colour
real foreGround[MAX_WINDOWS][4];  // text colour (not presently used)

// we can change the back-ground and text colour
// replace this by
aString backGroundName[MAX_WINDOWS], foreGroundName[MAX_WINDOWS];

bool saveTransformationInfo;
GLdouble modelMatrix[MAX_WINDOWS][16], projectionMatrix[MAX_WINDOWS][16];
GLint viewPort[MAX_WINDOWS][4];

// here are default values for lights
int lightIsOn[MAX_WINDOWS][numberOfLights];  // which lights are on
GLfloat ambient[MAX_WINDOWS][numberOfLights][4];
GLfloat diffuse[MAX_WINDOWS][numberOfLights][4];
GLfloat specular[MAX_WINDOWS][numberOfLights][4];
GLfloat position[MAX_WINDOWS][numberOfLights][4];
GLfloat globalAmbient[MAX_WINDOWS][4];

// default material properties, these values are used for surfaces in 3D when we give them
// different colours
GLfloat materialAmbient[MAX_WINDOWS][4]; // AP: this variable is no longer used
GLfloat materialDiffuse[MAX_WINDOWS][4]; // AP: this variable is no longer used
GLfloat materialSpecular[MAX_WINDOWS][4];
GLfloat materialShininess[MAX_WINDOWS];
GLfloat materialScaleFactor[MAX_WINDOWS]; // AP: this variable is no longer used

// view characteristics structure, used to communicate with mogl
ViewCharacteristics viewChar[MAX_WINDOWS];

int lighting[MAX_WINDOWS];
int plotTheAxes[MAX_WINDOWS]; // 0=no axes, 1,2,3= 1D,2D,3D axes.
int plotBackGroundGrid[MAX_WINDOWS]; // 0=no grid, 1,2,3= 1D,2D,3D grid.

  // Scale line widths/ point sizes by this amount (for high-res off-screen render)
real lineWidthScaleFactor[MAX_WINDOWS];

// clipping planes
enum
{
  maximumNumberOfClippingPlanes=6,
  maximumNumberOfTextStrings=25
};
  
// number of clipping planes that have been turned on
int numberOfClippingPlanes[MAX_WINDOWS];               

// TRUE if a clipping plane is turned on
// each clipping plane is defined by four constants c0*x+c1*y+c2*z+c3
int clippingPlaneIsOn[MAX_WINDOWS][maximumNumberOfClippingPlanes];   
double clippingPlaneEquation[MAX_WINDOWS][maximumNumberOfClippingPlanes][4];
// store the clipping plane names in a array (for convenience)
GLenum clip[MAX_WINDOWS][maximumNumberOfClippingPlanes];
ClippingPlaneInfo clippingPlaneInfo[MAX_WINDOWS];

int textIsOn[maximumNumberOfTextStrings];

// GLU objects
// GLUnurbsObj, make an array of 3 for the different modes : 
//              GLU_FILL, GLU_OUTLINE_POLYGON and GLU_OUTLINE_PATCH
GLUnurbsObj *OV_GLUnurbsArray[3];

void setProjection(const int & win_number); 
  
// for displaying user labels (defined interactively)
void
annotate(const aString & answer);  
  
void
constructor(); // int & argc, char *argv[], const aString & windowTitle = "Your Slogan Here");

int
setKeepAspectRatio( bool trueOrFalse=true );


virtual int
processSpecialMenuItems(aString & answer);

int
getAnswerSelectPick(aString & answer, const aString & prompt,
		    SelectionInfo *selection_ = NULL,
		    int blocking = 1);

int
interactiveAnswer(aString & answer,
		  const aString & prompt,
		  SelectionInfo * selection_,
		  int blocking = 1);

// plot and erase title labels
void
plotMappingBoundaries(const Mapping & mapping,
		      const RealArray & vertex,
		      const int colourOption=1, 
		      const real zRaise=0.,
		      GraphicsParameters & parameters=Overture::defaultGraphicsParameters());
  
virtual void
plotLabels(GraphicsParameters & parameters, 
	   const real & labelSize=-1.,  // <0 means use default in parameters
	   const real & topLabelHeight=.925,
	   const real & bottomLabelHeight=-.925, 
	   int win_number = -1);

virtual void
eraseLabels(GraphicsParameters & parameters, int win_number = -1); 

//  int
//  select(const float xPick[], IntegerArray & selection, const int & selectOption=0 );
  
void
pickNew(PickInfo &xPick, SelectionInfo & select);

int
selectNew(PickInfo & xPick, SelectionInfo *select);

void
setColourFromTable( const real value, GraphicsParameters & parameters =Overture::defaultGraphicsParameters() );

void
plotShadedFace(const realArray & x, 
	       const Index & I1, 
	       const Index & I2, 
	       const Index & I3, 
	       const int axis,
	       const int side,
	       const int domainDimension,
	       const int rangeDimension,
	       const RealArray & rgb,
	       const intArray & mask = Overture::nullIntegerDistributedArray() );
  
void
plotLinesOnSurface(const realArray & x, 
		   const Index & I1, 
		   const Index & I2, 
		   const Index & I3, 
		   const int axis,
		   const bool offsetLines,
		   const real eps,
		   GraphicsParameters & parameters,
		   const intArray & mask = Overture::nullIntegerDistributedArray() );

void
plotMappingEdges(const realArray & x, 
		 const IntegerArray & gridIndexRange,
		 GraphicsParameters & parameters,
		 const intArray & mask,
		 int grid =0 );
  

int
offScreenRender(const char * fileName, 
		GraphicsParameters & parameters=Overture::defaultGraphicsParameters() );

int
saveRasterInAFile(const aString & fileName, 
		  void *buffer, 
		  const GLint & width, 
		  const GLint & height,
		  const int & rgbType  =0,
		  GraphicsParameters & parameters=Overture::defaultGraphicsParameters() );

void
rleCompress( const int num, GLubyte *xBuffer, FILE *outFile, const int numPerLine = 30 );

void
setMaterialProperties(float ambr, float ambg, float ambb,
		      float difr, float difg, float difb,
		      float specr, float specg, float specb, float shine);

virtual void
appendCommandHistory(const aString &answer);

void
setupHardCopy(DialogData &hcd, int win);

void
setupMovie(DialogData &mov, int win);

void
hardcopyCommands(aString &longAnswer, int win_number);

void
movieCommands(aString &longAnswer, int win_number);

void
openGUI();

void
disableGUI();

GLUnurbsObj * 
getGLUnurbsObj(const GLfloat &type);


};
  
// define some macros to add extra info to the feed-back array
// Each macro will replace a single OpenGL call by 3 calls, adding extra info to the feedback array.

#define glEnable(stuff)    glEnable(stuff),   glPassThrough(GL_ENABLE_TOKEN),     glPassThrough(stuff)
#define glDisable(stuff)   glDisable(stuff),  glPassThrough(GL_DISABLE_TOKEN),    glPassThrough(stuff)
#define glLineWidth(size)  glLineWidth(size), glPassThrough(GL_LINE_WIDTH_TOKEN), glPassThrough(size)
#define glLineStipple(num,pattern) glLineStipple(num,pattern), \
              glPassThrough(GL_LINE_STIPPLE_TOKEN), glPassThrough(num), glPassThrough(pattern)
#define glPointSize(size)  glPointSize(size), glPassThrough(GL_POINT_SIZE_TOKEN), glPassThrough(size)





#endif


