
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "unparser.h"
#include "unparse_debug.h"


// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;


//-----------------------------------------------------------------------------------
//  void Unparser::printDebugInfo
//  
//  prints out debugging information specified in debug_info. A newline is also
//  inserted if true. 
//-----------------------------------------------------------------------------------
void
Unparse_Debug::printDebugInfo(const std::string& debug_info, bool newline)
   {
     if (unp->opt.get_debug_opt()) 
        {
//        opt.display("Unparser::printDebugInfo(char*,bool)");

          cout << debug_info;
          if (newline)
               cout << endl;

//        printf ("Exiting in Unparser::printDebugInfo(char*,bool) \n");
//        ROSE_ABORT();
        }
   }

void
Unparse_Debug::printDebugInfo(int debug_info, bool newline)
   {
     if (unp->opt.get_debug_opt())
        {
//        opt.display("Unparser::printDebugInfo(int,bool)");

          cout << debug_info;
          if (newline)
               cout << endl;

//        printf ("Exiting in Unparser::printDebugInfo(int,bool) \n");
//        ROSE_ABORT();
        }
   }
