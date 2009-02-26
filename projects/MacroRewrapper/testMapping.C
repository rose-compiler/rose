// Example ROSE Translator: used within ROSE/tutorial

#include "rose.h"
#include <functional>
using namespace std;

bool VERBOSE_MESSAGES_OF_WAVE = true;
bool do_db_optimization = false;
#include "macroRewrapper.h"


static const char* OPTION_NAMESTYLEFILE = "*imd:db";

//! Default command-line prefix for ROSE options
static const char* OPTION_PREFIX_ROSE = "-rose:";


//! Default command-option separator tag
static const char* OPTION_VALUE_SEPARATOR = "$^";


int main( int argc, char * argv[] )
   {
  // Build the AST used by ROSE
     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT(project != NULL);

  // Build a list of functions within the AST
     AnalyzeMacroCalls* macroCalls = new AnalyzeMacroCalls(project);
//     return backend(project);

   }

