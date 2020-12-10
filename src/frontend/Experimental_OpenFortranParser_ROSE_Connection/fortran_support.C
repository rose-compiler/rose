// C++ code calling an Fortran frontend function.

// sage3basic.h must be the first file included for the ROSE build system to work properly
//
#include "sage3basic.h"
#include "rose_config.h"

#include "ATermToSageFortranTraversal.h"
#include "fortran_support.h"

using namespace Rose;
using std::string;
using std::cout;
using std::endl;

#define DEBUG_EXPERIMENTAL_FORTRAN 0
#define DOT_FILE_GENERATION 0

#if DOT_FILE_GENERATION
#   include "wholeAST_API.h"
#endif

// TODO: THIS IS TEMPORARY (obtain it from somewhere else)
static SgGlobal* initialize_global_scope(SgSourceFile* file)
{
 // First we have to get the global scope initialized (and pushed onto the stack).

 // Set the default for source position generation to be consistent with other languages (e.g. C/C++).
    SageBuilder::setSourcePositionClassificationMode(SageBuilder::e_sourcePositionFrontendConstruction);
 // TODO      SageBuilder::setSourcePositionClassificationMode(SageBuilder::e_sourcePositionCompilerGenerated);

    SgGlobal* globalScope = file->get_globalScope();
    ROSE_ASSERT(globalScope != NULL);
    ROSE_ASSERT(globalScope->get_parent() != NULL);

 // Jovial is case insensitive
    globalScope->setCaseInsensitive(true);

    ROSE_ASSERT(globalScope->get_endOfConstruct()   != NULL);
    ROSE_ASSERT(globalScope->get_startOfConstruct() != NULL);

 // Not sure why this isn't set at construction
    globalScope->get_startOfConstruct()->set_line(1);
    globalScope->get_endOfConstruct()->set_line(1);

    SageBuilder::pushScopeStack(globalScope);

    return globalScope;
}

int
experimental_fortran_main(int argc, char* argv[], SgSourceFile* sg_source_file)
   {
  // Make system call to call the parser, then traverse resulting ATerm file to create AST.

     int status;
     string parse_table;

     assert(sg_source_file != NULL);

     std::string stratego_bin_path = STRATEGO_BIN_PATH;
     assert (stratego_bin_path.empty() == false);

  // Step 1 - Parse the input file
  // ------

  // The filename is obtained from the source-file object
     std::string filenameWithPath = sg_source_file->getFileName();
     std::string filenameWithoutPath = Rose::StringUtility::stripPathFromFileName(filenameWithPath);

     std::string commandString = stratego_bin_path + "/sglri";
     commandString += " -i " + filenameWithPath;

  // Add path to the parse table (located in the source tree)
     std::string parse_table_path = "src/3rdPartyLibraries/experimental-fortran-parser/share/rose";
     parse_table = findRoseSupportPathFromSource(parse_table_path, "share/rose") + "/Fortran.tbl";
     commandString += " -p " + parse_table;

  // Add source code location information to output
     commandString += " --preserve-locations";

  // Output the transformed aterm file
     commandString += " -o " + filenameWithoutPath + ".aterm";

  // Make system call to run parser and output ATerm parse-tree file
     status = system(commandString.c_str());
     if (status != 0)
        {
           fprintf(stderr, "\nFAILED: in fortran_main(), unable to parse file %s\n\n", filenameWithoutPath.c_str());
           return status;
        }

  // Step 2 - Traverse the ATerm parse tree and convert into Sage nodes
  // ------

  // Initialize the ATerm library
     ATinitialize(argc, argv);

     std::string aterm_filename = filenameWithoutPath + ".aterm";

  // Read the ATerm file that was created by the parser
     FILE* file = fopen(aterm_filename.c_str(), "r");
     if (file == NULL)
        {
           fprintf(stderr, "\nFAILED: in fortran_main(), unable to open file %s\n\n", aterm_filename.c_str());
           return 1;
        }

     ATerm module_term = ATreadFromTextFile(file);
     fclose(file);

  // Initialize the global scope and put it on the SageInterface scope stack
  // for usage by the sage tree builder during the ATerm traversal.
     initialize_global_scope(sg_source_file);

     ATermSupport::ATermToSageFortranTraversal* aterm_traversal;
     aterm_traversal = new ATermSupport::ATermToSageFortranTraversal(sg_source_file);

     if (aterm_traversal->traverse_Program(module_term) != ATtrue)
        {
           fprintf(stderr, "\nFAILED: in fortran_main(), unable to traverse ATerm file %s\n\n", aterm_filename.c_str());
           return 1;
        }

     if (aterm_traversal) delete aterm_traversal;

     return 0;
  }
