#ifndef GENERIC_GRAPHICS_INTERFACE_H
#define GENERIC_GRAPHICS_INTERFACE_H

//============================================================================================
//     -----------------------------------------
//     ------Generic Graphics Interface---------
//     -----------------------------------------
//
//   This is the generic graphics interface. It supports menus, command files and
//   some generic plotting. This Class does not know how to do any plotting. The
//   derived Class GLGraphicsInterface does know how to plot.
// 
//===========================================================================================

#include "Overture.h"
#include "GraphicsParameters.h"

// extern GraphicsParameters Overture::defaultGraphicsParameters();  // use these as a default argument
extern const aString nullString;

// forward declarations
class CompositeSurface;
class UnstructuredMapping;
class AdvancingFront;  

#include "GUIState.h"
//  class SelectionInfo;
//  class PickInfo3D;
//  class GUIState;

class GenericGraphicsInterface
{
public:
  
enum
{
  defaultOrigin=-(INT_MAX/2)
};

enum ItemColourEnum
{
  backGroundColour=0,
  textColour,
  numberOfItemColours
};

//
// Default constructor
//
GenericGraphicsInterface();
//
// Constructor that takes argc and argv from main program and strips away GLUT parameters
//
GenericGraphicsInterface(int & argc, char *argv[]);

virtual 
~GenericGraphicsInterface();
  
// return TRUE if the graphics is turned on (Note: this routine is non-virtual).
bool 
graphicsIsOn();

// Output a string 
virtual void 
outputString(const aString & message);
  
// Input a string after displaying an optional prompt
virtual void 
inputString(aString &answer, const aString & prompt=nullString);
  
//  Start reading a command file (if no file name is given, prompt for one)
FILE* 
readCommandFile(const aString & commandFileName=nullString);
  
// Start saving a command file (if no file name is given, prompt for one)
FILE* 
saveCommandFile(const aString & commandFileName=nullString);
  
//  Start reading commands from an array of Strings, commands terminated by the aString=""
int 
readCommandsFromStrings(const aString *commands);

int 
getValues(const aString & prompt, 
	  IntegerArray & values,
	  const int minimunValue= INT_MIN, 
	  const int maximumValue= INT_MAX,
	  const int sort = 0 );

int 
getValues(const aString & prompt, 
	  RealArray & values,
	  const real minimunValue= -REAL_MAX, 
	  const real maximumValue= REAL_MAX,
	  const int sort = 0 );

const aString & 
getDefaultPrompt();

// set the deafult prompt and clear the stack of deafult prompts
int 
setDefaultPrompt(const aString & prompt);

// push a default prompt onto a stack and make the current prompt
int 
pushDefaultPrompt(const aString & prompt );

// pop a default prompt off the stack and make the next prompt the new default
int 
popDefaultPrompt();

// make a new default prompt by appending to the current, and push onto the stack
int 
appendToTheDefaultPrompt(const aString & appendage );

// remove the last thing appended to the default prompt (just pop's the stack)
int 
unAppendTheDefaultPrompt();
  
// Stop reading the command file (and close the file)
void 
stopReadingCommandFile();

// Stop saving the command file (and close the file)
void 
stopSavingCommandFile();

// Start saving an echo file (if no file name is given, prompt for one)
FILE* 
saveEchoFile(const aString & fileName=nullString);
  
// Stop saving the echo file (and close the file)
void 
stopSavingEchoFile();

// add cascading entries to a long menu.
int 
buildCascadingMenu( aString *&menu,
		    int startCascade, 
		    int endCascade ) const ;
  
int 
indexInCascadingMenu( int & index,
		      const int startCascade,
		      const int endCascade ) const;
  

int 
getMatch(const aString *menu, aString & answer);
  
int 
savePickCommands(bool trueOrFalse=TRUE );

//
// Here are the PURE VIRTUAL FUNCTIONS:
//

virtual void
createMessageDialog(aString msg, MessageTypeEnum type)=0;

// Open the window (but only if one is not already open)
virtual int 
createWindow(const aString & windowTitle = nullString,
	     int argc=0, 
	     char *argv[] = NULL )=0;

// destroy the window 
virtual int
destroyWindow(int win_number)=0;

// get the number of the current graphics window
virtual int 
getCurrentWindow()=0;

// input a filename
virtual void
inputFileName(aString &answer, const aString & prompt=nullString, const aString & extension=nullString)=0;

virtual int
generateNewDisplayList(bool lit = false, bool plotIt = true, bool erasable = true)=0;
  
virtual void
setLighting(int list, bool lit)=0;

//  virtual void
//  setPlotDL(int list, bool lit)=0;

// access functions for display lists
virtual const int & 
getFirstDL(const int & win)=0;

virtual const int & 
getFirstFixedDL(const int & win)=0;

virtual const int & 
getTopLabelDL(const int & win)=0;

virtual const int & 
getTopLabel1DL(const int & win)=0;

virtual const int & 
getTopLabel2DL(const int & win)=0;

virtual const int & 
getTopLabel3DL(const int & win)=0;

virtual const int & 
getBottomLabelDL(const int & win)=0;

virtual const int & 
getBottomLabel1DL(const int & win)=0;

virtual const int & 
getBottomLabel2DL(const int & win)=0;

virtual const int & 
getBottomLabel3DL(const int & win)=0;

virtual const int & 
getColouredSquaresDL(const int & win)=0;

virtual const int & 
getColourBarDL(const int & win)=0;

virtual const int & 
getFirstUserLabelDL(const int & win)=0;

virtual const int & 
getLastUserLabelDL(const int & win)=0;

virtual const int & 
getLastFixedDL(const int & win)=0;

virtual const int & 
getFirstRotableDL(const int & win)=0;

virtual const int & 
getAxesDL(const int & win)=0;

virtual const int & 
getFirstUserRotableDL(const int & win)=0;

virtual const int & 
getLastUserRotableDL(const int & win)=0;

virtual const int & 
getLastRotableDL(const int & win)=0;

virtual const int & 
getLastDL(const int & win)=0;

virtual const int & 
getMaxNOfDL(const int & win)=0;


// Erase all graphics display lists
virtual void 
erase()=0;

// erases all display lists in window win_number.
virtual void 
erase(const int & win_number, bool forceDelete = false)=0;

// erase a list of display lists
virtual void
erase(const IntegerArray & displayList)=0;

// Redraw all graphics display lists
virtual void 
redraw( bool immediate=FALSE)=0;

// Draw coloured squares with a number inside them to label colours on the plot
virtual void
drawColouredSquares(const IntegerArray numberList,
		    GraphicsParameters & parameters=Overture::defaultGraphicsParameters(),
		    const int & numberOfColourNames = -1 , // use default colours by default
		    aString *colourNames = NULL)=0;

// plot and erase title labels
virtual void
plotLabels(GraphicsParameters & parameters, 
	   const real & labelSize=-1.,  // <0 means use default in parameters
	   const real & topLabelHeight=.925,
	   const real & bottomLabelHeight=-.925, 
	   int win_number = -1)=0;

virtual void
eraseLabels(GraphicsParameters & parameters, int win_number = -1)=0; 

// Plot a Mapping, pass optional parameters
virtual void 
plot(Mapping & map, 
     GraphicsParameters & parameters=Overture::defaultGraphicsParameters(),
     int dList=0, bool lit=FALSE)=0;

// Plot a MappedGrid
virtual void 
plot(MappedGrid & mg, 
     GraphicsParameters & parameters=Overture::defaultGraphicsParameters() )=0;

// Plot contours and/or a shaded surface plot of a realMappedGridFunction in 2D or 3D
virtual void 
contour(const realMappedGridFunction & u, 
	GraphicsParameters & parameters=Overture::defaultGraphicsParameters())=0;

// Plot streamlines of a 2D vector field
virtual void 
streamLines(const realMappedGridFunction & uv, 
	    GraphicsParameters & parameters=Overture::defaultGraphicsParameters())=0;

// Plot a GridCollection or Composite grid
virtual void 
plot(GridCollection & cg, 
     GraphicsParameters & parameters=Overture::defaultGraphicsParameters())=0;

// Plot contours and/or a shaded surface plot of a GridCollectionFunction/CompositeGridFunction in 2D
virtual void 
contour(const realGridCollectionFunction & u, 
	GraphicsParameters & parameters=Overture::defaultGraphicsParameters())=0;

// Plot streamlines of a vector field
virtual void
streamLines(const realGridCollectionFunction & uv, 
	    GraphicsParameters & parameters=Overture::defaultGraphicsParameters())=0;

// plot an advancing front
virtual void 
plot(AdvancingFront & front, 
     GraphicsParameters & parameters=Overture::defaultGraphicsParameters())=0;
  

// Plot 1D functions
virtual void
plot( const realArray & t, 
      const realArray & x, 
      const aString & title = nullString, 
      const aString & tName       = nullString,
      const aString *xName        = NULL,
      GraphicsParameters & parameters=Overture::defaultGraphicsParameters()  )=0;

// Plot a time sequence of 1D functions to generate a surface
virtual void
plot(const realArray & x, 
     const realArray & t,
     const realArray & u, 
     GraphicsParameters & parameters =Overture::defaultGraphicsParameters() )=0;

// plot points
virtual void 
plotPoints(const realArray & points, 
	   GraphicsParameters & parameters=Overture::defaultGraphicsParameters() )=0;

// choose a colour
virtual aString 
chooseAColour()=0;

//  set the colour for subsequent objects that are plotted
virtual int 
setColour( const aString & colourName )=0;

// set colour to default for a given type of item
virtual int 
setColour( ItemColourEnum item )=0;  

// return the name of a colour for i=0,1,2,...
virtual aString 
getColourName( int i ) const=0;

// convert normalized coordinates [-1,+1] to world (globalBound) coordinates
virtual int 
normalizedToWorldCoordinates(const RealArray & r, RealArray & x ) const=0;


virtual int
pickPoints( realArray & x, 
	    bool plotPoints = TRUE,
	    int win_number = -1 )=0;

// Functions for setting the bounding box and the rotation center
virtual void 
setGlobalBound(const RealArray &xBound)=0;

virtual RealArray 
getGlobalBound() const=0;

virtual void 
resetGlobalBound(const int & win_number)=0;

// access function for the variable PlotTheAxes
virtual int & 
sgPlotTheAxes()=0;

// access function for the variable plotBackGroundGrid
virtual int & 
sgPlotBackGroundGrid()=0;

/*  // setup function for a user defined pull-down menu */
/*  virtual void  */
/*  setUserMenu(const aString menu[], const aString & menuTitle)=0; */

/*  // setup function for user defined buttons */
/*  virtual void  */
/*  setUserButtons(const aString buttons[][2])=0; */

//  Save the graphics window in hard-copy form
virtual int
hardCopy(const aString & fileName=nullString, 
	 GraphicsParameters & parameters=Overture::defaultGraphicsParameters(),
	 int win_number=-1)=0;

virtual void
pollEvents()=0;

virtual void
pushGUI( GUIState &newState )=0;

virtual void
popGUI()=0;

virtual int
getMenuItem(const aString *menu, aString & answer, const aString & prompt=nullString)=0;

virtual int
getAnswer(aString & answer, const aString & prompt)=0;

virtual int
getAnswer(aString & answer, const aString & prompt,
	  SelectionInfo &selection)=0;

//  virtual int
//  getAnswer(aString & answer, const aString & prompt,
//  	  PickInfo3D &pick)=0;

//  virtual int
//  getAnswer(aString & answer, const aString & prompt,
//  	  SelectionInfo &selection, PickInfo3D &pick)=0;

virtual int
beginRecordDisplayLists( IntegerArray & displayLists)=0;

virtual int
endRecordDisplayLists( IntegerArray & displayLists)=0;

protected:
// These must be the same as in GraphicsParameters, put here for convenience
enum Sizes
{
  lineWidth           =GraphicsParameters::lineWidth,       
  axisNumberSize      =GraphicsParameters::axisNumberSize,  
  axisLabelSize       =GraphicsParameters::axisLabelSize,   
  axisMinorTickSize   =GraphicsParameters::axisMinorTickSize,
  axisMajorTickSize   =GraphicsParameters::axisMajorTickSize,
  topLabelSize        =GraphicsParameters::topLabelSize  ,     
  topSubLabelSize     =GraphicsParameters::topSubLabelSize,    
  bottomLabelSize     =GraphicsParameters::bottomLabelSize,
  bottomSubLabelSize  =GraphicsParameters::bottomSubLabelSize,
  minorContourWidth   =GraphicsParameters::minorContourWidth,
  majorContourWidth   =GraphicsParameters::majorContourWidth,
  streamLineWidth     =GraphicsParameters::streamLineWidth,  
  labelLineWidth      =GraphicsParameters::labelLineWidth,   
  curveLineWidth      =GraphicsParameters::curveLineWidth, 
  extraSize1,         
  extraSize2,
  extraSize3,
  numberOfSizes    // counts number of entries in this list
};

aString readFileName, saveFileName, echoFileName;
FILE *readFile, *saveFile, *echoFile;          // command files
bool readCommands, saveCommands;
bool savePick;             // if false, do not save pick related stuff in the command file.
  
const aString *stringCommands;
bool singleProcessorGraphicsMode;  // true if we are only plotting on one processor
int processorForGraphics;          // use this processor for plotting graphics on

GUIState *currentGUI; // the GUIState needs to be here so it is available from fileAnswer

void 
constructor(int & argc, char *argv[]);

int 
readLineFromCommandFile(aString & answer );

virtual int 
processSpecialMenuItems(aString & answer);

public:
virtual int 
pause()=0;

protected:
aString defaultPrompt, indentBlanks;
aString *defaultPromptStack; // stack of default prompts
int maxNumberOfDefaultPrompts,topOfDefaultPromptStack;
int saveFileCount;  // counts lines written to log file -- for flushing the file
bool graphicsWindowIsOpen;

int numberRecorded;
IntegerArray *recordDisplayLists;


int
fileAnswer(aString & answer,
	   const aString & prompt,
	   SelectionInfo * selection_);

int 
promptAnswerSelectPick(aString & answer, 
		       const aString & prompt, 
		       SelectionInfo * selection_);

int
promptAnswer(aString & answer, 
	     const aString & prompt = nullString);
virtual void
appendCommandHistory(const aString &answer)=0;

};
  
#endif
