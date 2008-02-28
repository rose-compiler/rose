// ################################################################
// #                           Header Files                       #
// ################################################################

#include "ROSETTA_macros.h"
#include "grammar.h"
#include "terminal.h"
#include "grammarString.h"
#include <sstream>

using namespace std;


// ################################################################
// #                   Grammar Member Functions                   #
// ################################################################

string localOutputVisitorSupport ( string name )
   {
     string s;
     s += string("          virtual void visit(");
     s += name;
     s += " *variable_";
     s += name;

  // DQ (11/26/2005): Not clear if these should be pure virtual
  // s += ") = 0;\n";

  // DQ (12/23/2005): Need to build the implementation and let 
  // derived classed overload as appropriate
     s += ") {}\n";

     return s;
   }

string
Grammar::buildVisitorBaseClass()
   {
     string s = string("class ROSE_VisitorPattern\n   {\n     public:\n virtual ~ROSE_VisitorPattern() {};\n");

     for (unsigned int i=0; i < terminalList.size(); i++)
        {
          string name = terminalList[i]->name;
          s += localOutputVisitorSupport(name);
        }

     s += "   };\n";

     return s;
   }
