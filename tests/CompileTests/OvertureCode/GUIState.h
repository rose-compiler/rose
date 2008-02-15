#ifndef GUIState_H
#define GUIState_H

#include "OvertureTypes.h"
#include "aString.H"
#include "DialogData.h"

#define MAX_SIBLINGS 10

class SelectionInfo
{
public:
  SelectionInfo(){nSelect=0; active=0;}  // *wdh*
// selection(i,0): globalID of object i
// selection(i,1): min z-buffer value of object i
// selection(i,2): max z-buffer value of object i
  IntegerArray selection; 
  int nSelect; // number of objects that were selected
// active == 1 if the object contains a window coordinate, otherwise the rest of the fields are undefined
  int active; 
  real r[4];  // window coordinates of the pickBox. r[0]: rMin, r[1]: rMax, r[2]: sMin, r[3]: sMax
  real zbMin; // z-buffer value of the closest object.
// 3D coordinate corresponding to the r window coordinate and the closest selected object. 
// Only defined if nSelect > 0.
  real x[3];  
  int globalID; // global ID of the closest object. Only defined if nSelect > 0.
  int winNumber; // window number where the picking occured
};

//  class PickInfo3D // maybe change name to PositionInfo ?
//  {
//   public:
//    PickInfo3D(){active=0; inside=0;}  // *wdh*
//    int active; // == 1 if the object contains a point, otherwise the rest of the fields are undefined
//    real r[3]; // average of pickBox + zBuffer coordinate
//    int inside; // == 1 if r could be turned into a 3D coordinate
//    real x[3];  // 3D coordinate. Undefined if inside != 1.
//    int globalID;     // global ID of the picked object. Undefined if inside != 1.
//  };

class GUIState : public DialogData
{
public:
// Constructors. Can only be called from the friend classes GL_GraphicsInterface or PlotStuff.
GUIState(); // default constructor.
~GUIState(); // destructor

void
setUserMenu(const aString menu[], const aString & menuTitle);
void
setUserButtons(const aString buttons[][2]);
void
buildPopup(const aString menu[]);
void
setAllCommands();
DialogData &
getDialogSibling(int number=-1); // maybe better to return a pointer since we might run out of internal space

static int 
addPrefix(const aString label[], const aString & prefix, aString cmd[], const int maxCommands);

GL_GraphicsInterface *gl; // pointer to the GL_GraphicsInterface object that was used to create this object

// keep a local copy of all the data!
int nPopup;  // number of popup commands. 0 if not beeing used
aString *popupMenu;

int nWindowButtons; // number of window buttons. 0 if not beeing used
aString *windowButtonCommands;
aString *windowButtonLabels;

int nPulldown;   // number of pulldown commands. 0 if not beeing used
aString pulldownTitle;
aString *pulldownCommand;

GUIState *prev;

int nAllCommands;
aString *allCommands;

int nDialogSiblings;
DialogData dialogSiblings[MAX_SIBLINGS];

protected:
GUIState(GUIState &source); // copy constructor disabled since it never gets defined
void operator =(GUIState &source); // assignment operator disabled since it never gets defined

};

#endif
