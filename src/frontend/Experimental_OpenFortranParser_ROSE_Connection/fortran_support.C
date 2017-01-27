// CER (12/15/2016):  This is temporary and should be removed before committing to ROSE
#define ROSE_USE_ROSE_ATERM_SUPPORT 1

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
#include "traverse_SgUntypedNodes.hpp"

// DQ (9/10/2014): I think this is declared in the other Fortran support (both of which exist).
// SgSourceFile* OpenFortranParser_globalFilePointer = NULL;

using namespace std;
using namespace rose;

#define DEBUG_ROSE_EXPERIMENTAL 1

#define USE_EXECUTABLE_FROM_PATH 1

SgUntypedFile*
experimental_openFortranParser_main(int argc, char **argv)
   {
  // Make system call to call the parser and build an ATERM file (put into the build tree).

     int i, err;
     string parse_table;

#if DEBUG_ROSE_EXPERIMENTAL
     printf ("In experimental_openFortranParser_main(): Put the call to the new SDF Open Fortran Parser here... argc = %d \n",argc);
#endif

     if (argc < 4)
        {
          printf("usage: fortran_parser --parseTable parse_table_path filename(s)\n");
          return NULL;
        }

  // DQ (1/22/2016): We want to assume that the stratego sglri executable is in the user's path, which is better than using a hard coded path.
  // Nowever it appears that sglri must be run with it's full path.  So we need to know that path to the stratego binary in order to avoid
  // hard coding it into ROSE (as we have done here). The experimental fortran support now requires both aterm and stratego library locations
  // to be specified at configure time for ROSE (this is also now enforced).

     string stratego_bin_path = STRATEGO_BIN_PATH;
     ROSE_ASSERT(stratego_bin_path.empty() == false);

     string commandString = stratego_bin_path + "/sglri ";

  // Parse each filename (args not associated with "--parseTable", "--" or "-I")
     for (i = 1; i < argc; i++)
        {
          if (strncmp(argv[i], "--parseTable", 12) == 0)
             {
               commandString += "-p ";
               commandString += argv[i+1];
               commandString += " ";

               parse_table = string(argv[i+1]);
               i += 1;
             }
            else
             {
            // This skips over commands line arguments that begin with "--" (this does not appears to be meaningful).
               if (strncmp(argv[i], "--", 2) == 0) 
                  {
                // skip args that are not files
                   i += 1;
                   continue;
                  }
                 else
                  {
                 // This only skips over the options that begin with "-I" but not "-I <path>" (where the "-I" and the path are seperated by a space).
                    if (strncmp(argv[i], "-I", 2) == 0)
                       {
                      // Skip the include dir stuff; it's handled by the lexer.
                      // TODO - not currently true, so skip arg for now? 
                         i += 1;
                         continue;
                       }
                      else
                       {
                      // All other options are ignored.
                      // commandString += argv[i];
                       }
                  }
             }
        }

     string filenameWithPath = argv[argc-1];

  // Finished processing command line arguments, make sure there is a parse table
     if (parse_table.empty() == true)
        {
          fprintf(stderr, "fortran_parser: no parse table provided, use option --parseTable\n");
          return NULL;
        }

#if DEBUG_ROSE_EXPERIMENTAL
     printf ("In experimental_openFortranParser_main(): filenameWithPath = %s \n",filenameWithPath.c_str());
#endif

     string filenameWithoutPath = StringUtility::stripPathFromFileName(filenameWithPath);

#if DEBUG_ROSE_EXPERIMENTAL
     printf ("In experimental_openFortranParser_main(): filenameWithPath    = %s \n",filenameWithPath.c_str());
     printf ("In experimental_openFortranParser_main(): filenameWithoutPath = %s \n",filenameWithoutPath.c_str());
     printf ("In experimental_openFortranParser_main(): commandString = %s \n",commandString.c_str());
#endif

     commandString += "-i ";
     commandString += filenameWithPath;

  // commandString += "/home/dquinlan/ROSE/ROSE_CompileTree/git-LINUX-64bit-4.4.7-dq-edg49-fortran-rc-aterm/src/3rdPartyLibraries/experimental-fortran-parser/stratego_transformations/";
  // commandString += "/home/dquinlan/ROSE/ROSE_CompileTree/git-LINUX-64bit-4.8.3-rose_development-rc-experimental_fortran_frontend/src/3rdPartyLibraries/experimental-fortran-parser/stratego_transformations/";
  // commandString += ROSE_AUTOMAKE_TOP_BUILDDIR + "/src/3rdPartyLibraries/experimental-fortran-parser/stratego_transformations/";
     string path_to_fortran_stratego_transformations_directory = findRoseSupportPathFromBuild("src/3rdPartyLibraries/experimental-fortran-parser/stratego_transformations", "bin");

  // Add pipe to begin transforming OFP's ATerm parse tree
     commandString += " | ";

#if USE_EXECUTABLE_FROM_PATH
     commandString += "ofp2fast";
#else
     commandString += path_to_fortran_stratego_transformations_directory;
     commandString += "/ofp2fast";
#endif

     string path_to_fortran_aterm_traversal_directory = findRoseSupportPathFromBuild("src/3rdPartyLibraries/experimental-fortran-parser/aterm_traversal", "bin");

   //CER bool process_using_ofp_roundtrip_support = OpenFortranParser_globalFilePointer->get_experimental_fortran_frontend_OFP_test();
     bool process_using_ofp_roundtrip_support = false;

     if (process_using_ofp_roundtrip_support == false)
        {
       // Convert from OFP's internal representation (FAST) to a ROSE SgUntyped aterm representation
          commandString += " | ";
#if USE_EXECUTABLE_FROM_PATH
          commandString += "fast2sage";
#else
          commandString += path_to_fortran_aterm_traversal_directory;
          commandString += "/fast2sage";
#endif

       // Output the transformed aterm file
          commandString += " -o ";
          commandString += filenameWithoutPath;
          commandString += ".aterm";
        }
     else
        {
       // Prepare the FAST representation for pretty printing
          commandString += " | ";
          commandString += path_to_fortran_stratego_transformations_directory;
          commandString += "/fast2pp";

       // Generate a Fortran text file from the FAST aterm representation (using a stratego tool).
          commandString += " | ";
          commandString += stratego_bin_path + "/ast2text";

       // Add the pretty-printing table command line argument
          commandString += " -p ";
       // string path_to_fortran_pretty_print_directory = findRoseSupportPathFromBuild("src/3rdPartyLibraries/experimental-fortran-parser/pretty_print", "bin");
          string path_to_fortran_pretty_print_directory = ROSE_AUTOMAKE_TOP_SRCDIR + "/src/3rdPartyLibraries/experimental-fortran-parser/pretty_print";
          commandString += path_to_fortran_pretty_print_directory;
          commandString += "/Fortran.pp";

       // Output a text file with prefix.
          commandString += " -o ";
          commandString += "pretty_print" + filenameWithoutPath;
        }

#if DEBUG_ROSE_EXPERIMENTAL
     printf ("In experimental_openFortranParser_main(): commandString = %s \n",commandString.c_str());
#endif

     err = system(commandString.c_str());

     if (err)
        {
          fprintf(stderr, "fortran_parser: error parsing file %s\n", argv[i]);
          return NULL;
        }

  // At this point we have a valid aterm file in the working (current) directory.
  // We have to read that aterm file and generate an uninterpreted AST, then iterate
  // on the uninterpreted AST to resolve types, disambiguate function calls and 
  // array references, etc.; until we have a correctly formed AST.  These operations
  // will be separate passes over the AST which should build a simpler frontend to
  // use as a basis for fortran research and also permit a better design for the
  // frontend to maintain and develop cooperatively with community support.

     if (process_using_ofp_roundtrip_support == false)
        {
       // Initialize the ATerm library
          ATinitialize(argc, argv);

          filenameWithoutPath += ".aterm";

#if DEBUG_ROSE_EXPERIMENTAL
          printf ("In experimental_openFortranParser_main(): Opening aterm file filenameWithoutPath = %s \n",filenameWithoutPath.c_str());
#endif

       // Read the ATerm file that was created by the parser
          FILE * file = fopen(filenameWithoutPath.c_str(), "r");

          if (file == NULL)
             {
               fprintf(stderr, "\nFAILED: in experimental_openFortranParser_main(), unable to open file %s\n\n", filenameWithoutPath.c_str());
               return NULL;
             }

          ATerm SgUntypedFile_term = ATreadFromTextFile(file);
          fclose(file);

#if DEBUG_ROSE_EXPERIMENTAL
          printf ("In experimental_openFortranParser_main(): Calling traverse_SgUntypedFile() \n");
#endif

          SgUntypedFile* untypedFile = NULL;

          if (traverse_SgUntypedFile(SgUntypedFile_term, &untypedFile) != ATtrue || untypedFile == NULL)
             {
               fprintf(stderr, "\nFAILED: in experimental_openFortranParser_main(), unable to parse file %s\n\n", filenameWithoutPath.c_str());
               return NULL;
             }
          else
             {
#if DEBUG_ROSE_EXPERIMENTAL
               printf ("In experimental_openFortranParser_main(): false branch taken: traverse_SgUntypedFile() != ATtrue || untypedFile == NULL: untypedFile = %p \n",untypedFile);
#endif
               return untypedFile;
             }
        }

#if DEBUG_ROSE_EXPERIMENTAL
     printf ("In experimental_openFortranParser_main(): exiting normally without using OFP round trip support\n");
#endif

     return NULL;
  }
