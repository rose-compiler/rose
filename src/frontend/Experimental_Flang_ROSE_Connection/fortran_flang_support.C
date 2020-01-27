#include "sage3basic.h"
#include "rose_config.h"

#include "fortran_flang_support.h"
#include "flang-external-builder-main.h"

using namespace Rose;

#include <iostream>
using std::string;
using std::cout;
using std::endl;

#define DEBUG_EXPERIMENTAL_FORTRAN 0
#define DOT_FILE_GENERATION 0

#if DOT_FILE_GENERATION
#   include "wholeAST_API.h"
#endif

SgGlobal* initialize_global_scope(SgSourceFile* file)
{
 // First we have to get the global scope initialized (and pushed onto the stack).

 // Set the default for source position generation to be consistent with other languages (e.g. C/C++).
    SageBuilder::setSourcePositionClassificationMode(SageBuilder::e_sourcePositionFrontendConstruction);
 // TODO      SageBuilder::setSourcePositionClassificationMode(SageBuilder::e_sourcePositionCompilerGenerated);

    SgGlobal* globalScope = file->get_globalScope();
    ROSE_ASSERT(globalScope != NULL);
    ROSE_ASSERT(globalScope->get_parent() != NULL);

 // Fortran is case insensitive
    globalScope->setCaseInsensitive(true);

 // DQ (8/21/2008): endOfConstruct is not set to be consistent with startOfConstruct.
    ROSE_ASSERT(globalScope->get_endOfConstruct()   != NULL);
    ROSE_ASSERT(globalScope->get_startOfConstruct() != NULL);

 // DQ (10/10/2010): Set the start position of global scope to "1".
    globalScope->get_startOfConstruct()->set_line(1);

 // DQ (10/10/2010): Set this position to the same value so that if we increment
 // by "1" the start and end will not be the same value.
    globalScope->get_endOfConstruct()->set_line(1);

    ROSE_ASSERT(SageBuilder::emptyScopeStack() == true);
    SageBuilder::pushScopeStack(globalScope);

#ifdef WHERE_IS_DEBUG_COMMENT_LEVEL
    if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
       {
          SageBuilder::topScopeStack()->get_startOfConstruct()->display("In initialize_global_scope(): start");
          SageBuilder::topScopeStack()->get_endOfConstruct  ()->display("In initialize_global_scope(): end");
       }
#endif

    return globalScope;
}

int
experimental_fortran_main(int argc, char* argv[], SgSourceFile* sg_source_file)
   {
      // Perhaps do the following:
      //   1. Run f18 from the command line
      //   2. This function is a call back

      // Or:
      //   1. Call a function in the f18 frontend passing (argc, argv)
      //   2. The f18 function then calls this function as a call back

      cout << "\n";
      cout << "-->  experimental_fortran_flang_main: will call flang parser \n";

      int status = 0;

      SgGlobal* global_scope = initialize_global_scope(sg_source_file);

      status = flang_external_builder_main(argc, argv);

      cout << "\n";
      cout << "FINISHED parsing...\n\n";

      return status;
  }
