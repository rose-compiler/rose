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

// Support for classic visitor pattern
string localPoolBasedTraversalVisitorPatternSupport ( string name )
   {
     string s;
     s += string("     ");
     s += name;
     s += string("::traverseMemoryPoolVisitorPattern(visitor);\n");
     return s;
   }

// Support for ROSE tree traversal type traversal (simpler then classic visitor pattern)
string localPoolNodesBasedTraversalSupport ( string name )
   {
     string s;
     s += string("     ");
     s += name;
     s += string("::traverseMemoryPoolNodes(visit);\n");
     return s;
   }

// Support for ROSE tree traversal type traversal 
// (but visits only one Sage III IR node (of each IR node type) 
// in the memory pool, if one exists)
string traverseRepresentativeNodeSupport ( string name )
   {
     string s;
     s += string("     ");
     s += name;
     s += string("::visitRepresentativeNode(visit);\n");
     return s;
   }


// Support for computation of memory useage.
string memoryUsageSupport ( string name )
   {
     string s;
     s += string("     count += ");
     s += name;
     s += string("::memoryUsage();\n");
     return s;
   }

// Support for computation of memory useage.
string numberOfNodesSupport ( string name )
   {
     string s;
     s += string("     count += ");
     s += name;
     s += string("::numberOfNodes();\n");
     return s;
   }

#if 0
// This is best done more generally using a traversal over the
// collection of IR nodes (so that we can call static members).
// Support for computation of memory useage.
string memoryUsageStatisticsSupport ( string name )
   {
     string s;
     s += string("     cout << \"");
     s += name;
     s += string(" count = \" ");
     s += name;
     s += string("::numberOfNodes()");
     s += string(" << \"");
     s += name;
     s += string(" memory = \" ");
     s += name;
     s += string("::memoryUsage()");
     s += string(" << \"");
     s += name;
     s += string(" % of total \" ");
     s += name;
     s += string("(double)count / (double)totalNumberOfNodes;\n");
     return s;
   }
#endif

string
Grammar::buildMemoryPoolBasedTraversalSupport()
   {
  // This function builds two different styles of traversals (one based on a 
  // classic visitor pattern and one baed on the ROSE traversal mechanism 
  // (though without attributes)).

     string s = string("\n\nvoid traverseMemoryPoolVisitorPattern ( ROSE_VisitorPattern & visitor )\n   {\n");

     for (unsigned int i=0; i < terminalList.size(); i++)
        {
          string name = terminalList[i]->name;
          s += localPoolBasedTraversalVisitorPatternSupport(name);
        }

     s += "   }\n\n";

     s += string("\n\nvoid traverseMemoryPoolNodes ( ROSE_VisitTraversal & visit )\n   {\n");

     for (unsigned int i=0; i < terminalList.size(); i++)
        {
          string name = terminalList[i]->name;
          s += localPoolNodesBasedTraversalSupport(name);
        }

     s += "   }\n\n";

  // DQ (2/9/2006): This allows a traversal over the types of Sage III IR nodes
  // Using this traversal only static member functions of the IR nodes may be called
  // (or any global function).  We don't traverse all the instances of the IR nodes.
     s += string("\n\nvoid traverseRepresentativeNodes ( ROSE_VisitTraversal & visit )\n   {\n");

     for (unsigned int i=0; i < terminalList.size(); i++)
        {
          string name = terminalList[i]->name;
          s += traverseRepresentativeNodeSupport(name);
        }

     s += "   }\n\n";

     s += string("\n\nsize_t memoryUsage ()\n   {\n");
     s += "     size_t count = 0; \n\n";

     for (unsigned int i=0; i < terminalList.size(); i++)
        {
          string name = terminalList[i]->name;
          s += memoryUsageSupport(name);
        }

     s += "\n\n";
     s += "     return count;\n";
     s += "   }\n";

     s += string("\n\nsize_t numberOfNodes ()\n   {\n");
     s += "     size_t count = 0; \n\n";

     for (unsigned int i=0; i < terminalList.size(); i++)
        {
          string name = terminalList[i]->name;
          s += numberOfNodesSupport(name);
        }

     s += "\n\n";
     s += "     return count;\n";
     s += "   }\n";

     return s;
   }

