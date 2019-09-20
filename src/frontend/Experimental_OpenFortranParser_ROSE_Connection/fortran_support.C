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
#include "ATermToUntypedFortranTraversal.h"
#include "UntypedFortranTraversal.h"
#include "UntypedFortranConverter.h"

using namespace Rose;
using std::string;
using std::cout;
using std::endl;

#define DEBUG_EXPERIMENTAL_FORTRAN 0
#define DOT_FILE_GENERATION 0

#if DOT_FILE_GENERATION
#   include "wholeAST_API.h"
#endif


int
experimental_fortran_main(int argc, char* argv[], SgSourceFile* sg_source_file)
   {
  // Make system call to call the parser, then traverse resulting ATerm file to create AST.

     int i, status;
     string parse_table;
     ATermSupport::ATermToUntypedFortranTraversal* aterm_traversal = NULL;

     ROSE_ASSERT(sg_source_file != NULL);
     ROSE_ASSERT(sg_source_file->get_experimental_fortran_frontend() == true);

  // Rasmussen (11/13/2017): Moved parse table to ROSE 3rdPartyLibraries (no longer set by caller).
     if (argc < 2)
        {
          printf("usage: fortran_parser filename(s)\n");
          return 1;
        }

  // Rasmussen (11/13/2017): The experimental fortran support now requires both aterm and stratego
  // library locations to be specified at configure time for ROSE (this is also now enforced).

     string stratego_bin_path = STRATEGO_BIN_PATH;
     ROSE_ASSERT(stratego_bin_path.empty() == false);

  // Step 1 - Parse the input file
  // ------
     string commandString = stratego_bin_path + "/sglri ";

  // Rasmussen (11/13/2017): Moved parse table to ROSE 3rdPartyLibraries (no longer set by caller).
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
     string parse_table_path = "src/3rdPartyLibraries/experimental-fortran-parser/share/rose";
     parse_table = findRoseSupportPathFromSource(parse_table_path, "share/rose");

     if (sg_source_file->get_experimental_cuda_fortran_frontend() == false)
        {
           parse_table += "/Fortran.tbl";
        }
     else
        {
           parse_table += "/CUDA_Fortran.tbl";
        }
     commandString += "-p " + parse_table + " ";

  // Rasmussen (11/14/2017): TODO: What about multiple files?
  // string filenameWithPath = argv[argc-1];
     string filenameWithPath = sg_source_file->getFileName();
     string filenameWithoutPath = StringUtility::stripPathFromFileName(filenameWithPath);

     commandString += "-i " + filenameWithPath;

  // Add source code location information to output
     commandString += " --preserve-locations";

  // Output the transformed aterm file
     commandString += " -o " + filenameWithoutPath + ".aterm";

#if DEBUG_EXPERIMENTAL_FORTRAN
     cout << "experimental_fortran_main(): filenameWithoutPath = " << filenameWithoutPath << endl;
     cout << "... filenameWithPath = " << filenameWithPath << endl;
     cout << "... commandString    = " << commandString << endl;
     cout << "... is experimental_cuda_fortran_frontend: " << sg_source_file->get_experimental_cuda_fortran_frontend() << endl;
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

#if DEBUG_EXPERIMENTAL_FORTRAN
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

  // Step 2 - Traverse the ATerm parse tree and convert into Untyped nodes
  // ------

  // Create object to traverse the ATerm file
     aterm_traversal = new ATermSupport::ATermToUntypedFortranTraversal(OpenFortranParser_globalFilePointer);

     if (aterm_traversal->traverse_Program(program_term) != ATtrue)
        {
           fprintf(stderr, "\nFAILED: in experimental_openFortranParser_main(), unable to traverse file %s\n\n", aterm_filename.c_str());
           return 1;
        }

  // Rasmussen (01/22/18): Create a dot file.  This is temporary or should
  // at least be a rose option.
#if DOT_FILE_GENERATION
     SgUntypedGlobalScope* global_scope = aterm_traversal->get_scope();
     generateDOT(global_scope, filenameWithoutPath + ".ut");
#endif

  // Step 3 - Traverse the SgUntypedFile object and convert to regular sage nodes
  // ------

  // Create the untyped traversal object

     Untyped::UntypedFortranConverter sg_converter;
     Untyped::UntypedFortranTraversal sg_traversal(OpenFortranParser_globalFilePointer, &sg_converter);
     Untyped::InheritedAttribute scope = NULL;

  // Traverse the untyped tree and convert to sage nodes
     sg_traversal.traverse(aterm_traversal->get_file(), scope);

  // Generate dot file for Sage nodes.
#if DOT_FILE_GENERATION
     generateDOT(SageBuilder::getGlobalScopeFromScopeStack(), filenameWithoutPath);
  // generateWholeGraphOfAST(filenameWithoutPath+"_WholeAST");
#endif

     if (aterm_traversal)  delete aterm_traversal;

     return 0;
  }
