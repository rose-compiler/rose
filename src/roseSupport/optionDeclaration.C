// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"

#include "optionDeclaration.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;
using namespace rose;

// All constructors save the input value and generate a string to save the value as a string.
// We should address the fact that this results is redundent storage (and select only one).
// At this point I am not sure which one I want (a switch statement in getValue would be one option).

string
OptionDeclaration::getValue()
   {
     return valueString;
   }


OptionDeclaration::OptionDeclaration( string s, int value )
   {
     optionName   = s;
     valueInteger = value;
     valueString = StringUtility::numberToString(value);
   }

OptionDeclaration::OptionDeclaration( string s, double value )
   {
     optionName  = s;
     valuedouble = value;
     valueString = StringUtility::numberToString(value);
   }

OptionDeclaration::OptionDeclaration( string s, string value )
   {
     optionName  = s;
     valueString = value;
   }


