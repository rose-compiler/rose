// ################################################################
// #                           Header Files                       #
// ################################################################

#include "ROSETTA_macros.h"
#include "grammar.h"
#include "AstNodeClass.h"
#include "grammarString.h"
#include <sstream>

using namespace std;


// ################################################################
// #                   Grammar Member Functions                   #
// ################################################################

string localOutputSupport ( string name )
   {
     const int indentationOffset = 41;

     string s;
     s += string("     printf (\"     Size of ");
     s += name;

     int numberOfSpaces = indentationOffset - name.size();
     for (int n = 0; n < numberOfSpaces; n++)
          s += string(" ");

     s += string(" = %lu \\n\",(unsigned long) sizeof(");
     s += name;
     s += string("));\n");

     return s;
   }


// DQ (5/24/2005): This builds a function to report the sizes of each type of IR node.
// This is part of an attempt to evaluate how we can make the IR more compact as
// part of support for the reading a whole project into memory.
string
Grammar::buildMemoryStorageEvaluationSupport()
   {
     string s = string("void outputSizeOfIntermediateRepresentation()\n   {\n");

  // string name = "AstAttributeMechanism";
     s += localOutputSupport("AstAttributeMechanism");

     for (unsigned int i=0; i < terminalList.size(); i++)
        {
          string name = terminalList[i]->name;
          s += localOutputSupport(name);
        }

     s += "   }\n";

     return s;
   }

