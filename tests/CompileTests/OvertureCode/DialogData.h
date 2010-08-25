#ifndef DIALOG_DATA_H
#define DIALOG_DATA_H
enum button_type{ GI_PUSHBUTTON, GI_TOGGLEBUTTON };

enum MessageTypeEnum 
{
  errorDialog=0,
  warningDialog,
  informationDialog,
  messageDialog
};

struct PushButton
{
// buttonCommand holds the name of the command and 
// buttonLabel holds the label that will appear on the button.
  PushButton(){pb=NULL;sensitive=true;} // *wdh*
  void setSensitive(bool trueOrFalse);  // *wdh*

  aString buttonCommand;
  aString buttonLabel;
  bool sensitive;      // *wdh*
  void *pb; // widget
};

struct ToggleButton
{
  ToggleButton(){tb=NULL;sensitive=true;} // *wdh*
  int setState(bool trueOrFalse ); // *wdh*
  void setSensitive(bool trueOrFalse);  // *wdh*

// buttonCommand holds the name of the command and 
// buttonLabel holds the label that will appear on the button.
  aString buttonCommand;
  aString buttonLabel;
  int state;
  bool sensitive;      // *wdh*
  void *tb; // widget
};

struct TextLabel
{
// textCommand holds the name of the command and 
// textLabel holds the label that will appear in front of the editable text.
// string holds the editable string.
// textWidget holds a pointer to the editable text widget, which makes it possible to change the text
// without typing in the dialog window. This might be useful for correcting typos, for example.
  TextLabel(){textWidget=NULL;sensitive=true; labelWidget=NULL;} // *wdh*
  void setSensitive(bool trueOrFalse);  // *wdh*

  aString textCommand;
  aString textLabel;
  aString string;
  bool sensitive;      // *wdh*
  void *textWidget; // Widget
  void *labelWidget; // Widget
};

struct OptionMenu
{
  OptionMenu(){menupane=NULL;sensitive=true; menuframe=NULL;} // *wdh*
  int setCurrentChoice(int command);          // *wdh*
  void setSensitive(bool trueOrFalse);  // *wdh*

  aString optionLabel;
  int n_options;
  PushButton *optionList;
  aString currentChoice;
  bool sensitive;      // *wdh*
  void *menupane; // widget
  void *menuframe; // widget
};

struct PullDownMenu
{
  PullDownMenu(){menupane=NULL;sensitive=true;} // *wdh*
  // set the state of a toggle button
  int setToggleState(int n, bool trueOrFalse ); // *wdh*
  void setSensitive(bool trueOrFalse);  // *wdh*

  aString menuTitle;
  int n_button;
  button_type type;
  PushButton *pbList;
  ToggleButton *tbList;
  bool sensitive;      // *wdh*
  void *menupane; // widget
};

const int MAX_OP_MENU=10, MAX_PD_MENU=6; // can't have more than 10 option menus or 6 pull-down menus

class DialogData
{
public:
  enum WidgetTypeEnum  // *wdh* 
  {
    optionMenuWidget,
    pushButtonWidget,
    pullDownWidget,
    toggleButtonWidget,
    textBoxWidget
  };

DialogData(); // default constructor. Not normally called by the user
~DialogData(); // destructor

void
openDialog(int managed = 1);

int
dialogCommands(aString **commands);

int
showSibling(); // display a sibling on the screen

int 
hideSibling(); // remove a sibling from the screen

void
setSensitive(int trueFalse);

void
setSensitive(bool trueOrFalse, WidgetTypeEnum widgetType, int number ); // *wdh* 

int
addOptionMenu(const aString &opMainLabel, aString opCommands[], aString opLabels[], int initCommand);

bool
changeOptionMenu(int nOption, aString opCommands[], aString opLabels[], int initCommand);

int
addPulldownMenu(const aString &pdMainLabel, aString commands[], aString labels[], button_type bt, 
		int *initState = NULL);

int
setToggleButtons(aString tbCommands[], aString tbLabels[], int initState[], 
                 int numberOfColumns = 2);

int
setPushButtons(aString pbCommands[], aString pbLabels[], int numberOfRows = 2);

int
setTextBoxes(aString textCommands[], aString textLabels[], aString initString[]);
 
int
setTextLabel(int n, const aString &buff);

int
setTextLabel(const aString & textLabel, const aString &buff); // set the text box with label "textLabel"

int
setToggleState(int n, int trueFalse);

int
setExitCommand(const aString &exitC, const aString &exitL);

void
setWindowTitle(const aString &title);

void
setOptionMenuColumns(int columns);

void
setLastPullDownIsHelp(int trueFalse);

PullDownMenu&
getPulldownMenu(int n);

OptionMenu&
getOptionMenu(int n); // *wdh*

OptionMenu&
getOptionMenu(const aString & opMainLabel); // find the option menu with this label
  
void 
closeDialog();

void *
getWidget();

aString &
getExitCommand();

void
setBuiltInDialog();
   
int
getBuiltInDialog();
   

protected:
void *dialogWindow; // Widget

aString windowTitle;
aString exitCommand;
aString exitLabel;

int builtInDialog; // = 1 if this dialog is parsed inside processSpecialMenuItems, otherwise = 0.
// If builtInDialog == 1, the associated callback functions will return a negative number instead
// of a positive number, which is used for user defined commands.

int pButtonRows; // pack the pButtons into this many rows
int n_pButtons; 
PushButton *pButtons; // array of buttonDefs

int toggleButtonColumns;
int n_toggle;
ToggleButton *tButtons; // array of buttonDefs

int n_text;
TextLabel *textBoxes;

int optionMenuColumns;
int n_optionMenu;
OptionMenu opMenuData[MAX_OP_MENU];

int n_pullDownMenu;
PullDownMenu pdMenuData[MAX_PD_MENU];

DialogData(DialogData &source); // copy constructor disabled since it never gets defined
void operator =(DialogData &source); // assignment operator disabled since it never gets defined

int pdLastIsHelp;
};


class PickInfo
{
public:

PickInfo(); // default constructor

int pickType;
int pickWindow;
float pickBox[4]; // xmin, xmax, ymin, ymax
};

#endif
