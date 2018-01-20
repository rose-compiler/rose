#include "sage3basic.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
// #include "rose_config.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
#include "rose_config.h"
// #include "rosePublicConfig.h"

#include "fortran_support.h"

#include <aterm2.h>
#include "UntypedTraversal.h"

using namespace std;
using namespace Rose;

#define DEBUG_ROSE_EXPERIMENTAL 0

#include "ATermToUntypedFortranTraversal.h"

int
experimental_openFortranParser_main(int argc, char **argv)
   {
  // Make system call to call the parser, then traverse resulting ATerm file to create AST.

     int i, status;
     string parse_table;
     OFP::ATermToUntypedFortranTraversal* aterm_traversal = NULL;

  // Rasmussen (11/13/2017): Moved parse table to ROSE 3rdPartyLibraries (no longer set by caller).
     if (argc < 2)
        {
          printf("usage: fortran_parser filename(s)\n");
          return 1;
        }

  // DQ (1/22/2016): We want to assume that the stratego sglri executable is in the user's path, which is better than using a hard coded path.
  // Nowever it appears that sglri must be run with it's full path.  So we need to know that path to the stratego binary in order to avoid
  // hard coding it into ROSE (as we have done here). The experimental fortran support now requires both aterm and stratego library locations
  // to be specified at configure time for ROSE (this is also now enforced).

     string stratego_bin_path = STRATEGO_BIN_PATH;
     ROSE_ASSERT(stratego_bin_path.empty() == false);

     string commandString = stratego_bin_path + "/sglri ";

  // Rasmussen (11/13/2017): Moved parse table to ROSE 3rdPartyLibraries (no longer set by caller).
  // Parse each filename (args not associated with "--parseTable", "--" or "-I")
     for (i = 1; i < argc; i++)
        {
        // Skips over commands line arguments that begin with "--" (none are meaningful).
           if (strncmp(argv[i], "--", 2) == 0) 
              {
              // skip args that are not files
                 i += 1;
                 continue;
              }
           else if (strncmp(argv[i], "-I", 2) == 0)
              {
              // Rasmussen (11/14/2017): Skips for now but needs to be tested (also -Ipath_to_file)
                 i += 1;
                 continue;
              }
        }

  // Parse table location is now stored in the source tree
     string parse_table_path = "src/3rdPartyLibraries/experimental-fortran-parser/bin/Fortran.tbl";
     parse_table = findRoseSupportPathFromSource(parse_table_path, "bin");
     commandString += "-p " + parse_table + " ";

  // Rasmussen (11/14/2017): TODO: What about multiple files?
     string filenameWithPath = argv[argc-1];
     string filenameWithoutPath = StringUtility::stripPathFromFileName(filenameWithPath);

     commandString += "-i " + filenameWithPath;

  // Add source code location information to output
     commandString += " --preserve-locations";

  // Output the transformed aterm file
     commandString += " -o " + filenameWithoutPath + ".aterm";

#if DEBUG_ROSE_EXPERIMENTAL
     printf ("In experimental_openFortranParser_main(): filenameWithPath = %s \n",filenameWithPath.c_str());
     printf ("In experimental_openFortranParser_main(): filenameWithoutPath = %s \n",filenameWithoutPath.c_str());
     printf ("In experimental_openFortranParser_main(): commandString = %s \n",commandString.c_str());
#endif

     status = system(commandString.c_str());

     if (status != 0)
        {
          fprintf(stderr, "fortran_parser: error parsing file %s\n", argv[i]);
          return 1;
        }

  // At this point we have a valid ATerm file in the working (current) directory.
  // We have to read that ATerm file and generate an untyped AST, then iterate
  // on the untyped AST to resolve types, disambiguate function calls and 
  // array references, etc.; until we have a correctly formed AST.  These operations
  // will be separate passes over the AST which should build a simpler frontend to
  // use as a basis for fortran research and also permit a better design for the
  // frontend to maintain and develop cooperatively with community support.

  // Initialize the ATerm library
     ATinitialize(argc, argv);

     string aterm_filename = filenameWithoutPath + ".aterm";

#if DEBUG_ROSE_EXPERIMENTAL
     printf ("In experimental_openFortranParser_main(): Opening aterm file = %s \n", aterm_filename.c_str());
#endif

  // Read the ATerm file that was created by the parser
     FILE * file = fopen(aterm_filename.c_str(), "r");
     if (file == NULL)
        {
           fprintf(stderr, "\nFAILED: in experimental_openFortranParser_main(), unable to open file %s\n\n", aterm_filename.c_str());
           return 1;
        }

     ATerm program_term = ATreadFromTextFile(file);
     fclose(file);

#if DEBUG_ROSE_EXPERIMENTAL
     printf ("In experimental_openFortranParser_main(): Calling traverse_SgUntypedFile() \n");
#endif

//----------------------------------------------------------------------
//  Traverse the ATerm file and convert to untyped nodes
//----------------------------------------------------------------------

  // Create object to traverse the ATerm file
     aterm_traversal = new OFP::ATermToUntypedFortranTraversal(OpenFortranParser_globalFilePointer);

     if (aterm_traversal->traverse_Program(program_term) != ATtrue)
        {
           fprintf(stderr, "\nFAILED: in experimental_openFortranParser_main(), unable to traverse file %s\n\n", aterm_filename.c_str());
           return 1;
        }

#if DEBUG_ROSE_EXPERIMENTAL
     printf ("In experimental_openFortranParser_main(): successfully traversed ATerms, beginning traversal \n");
     printf ("--------------------------------------------------------------\n\n");
#endif

//----------------------------------------------------------------------
//  Traverse the SgUntypedFile object and convert to regular sage nodes
//----------------------------------------------------------------------

  // Create the untyped traversal object
     Fortran::Untyped::UntypedTraversal sg_traversal(OpenFortranParser_globalFilePointer);
     Fortran::Untyped::InheritedAttribute scope = NULL;

  // Traverse the untyped tree and convert to sage nodes
     sg_traversal.traverse(aterm_traversal->get_file(), scope);

  // Generate dot file for Sage nodes.
     generateDOT(SageBuilder::getGlobalScopeFromScopeStack(), filenameWithoutPath);

     if (aterm_traversal)  delete aterm_traversal;

     return 0;
  }
