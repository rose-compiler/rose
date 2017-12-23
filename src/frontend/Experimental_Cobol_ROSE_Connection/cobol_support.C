// cobol_main function to initiate parsing
// ---------------------------------------

// DQ (11/13/2017): This is a policy violation, sage3basic.h must be the first file included.
// #include "rose_config.h"
#include "sage3basic.h"
#include "rose_config.h"

#include "cobol_support.h"
#include "gnucobpt.h"
#include "rose_convert_cobol.h"
#include "CobolGraph.h"
#include "ATermToUntypedCobolTraversal.h"
#include "UntypedCobolTraversal.h"

#define PARSE_USING_STRATEGO 1

int parse_using_stratego(int argc, char** argv, SgSourceFile* sg_source_file);


int cobol_main(int argc, char** argv, SgSourceFile* sg_source_file)
   {
     char* cobol_argv[2];
     int cobol_argc;
     int status = 1;

     ROSE_ASSERT (sg_source_file != NULL);

     if (PARSE_USING_STRATEGO)
        {
           status = parse_using_stratego(argc, argv, sg_source_file);
        }
     else
        {
           std::string filenameWithPath = sg_source_file->getFileName();
           std::string filename = Rose::StringUtility::stripPathFromFileName(filenameWithPath);

        // Initialize argc and argv variables for call to Cobol parser
           cobol_argc = 2;
           cobol_argv[0] = strdup("ROSE::cobol_main");
           cobol_argv[1] = strdup(filename.c_str());

        // Call the main entry function for access to GnuCOBOL parse-tree information
        //   - this function subsequently calls cobpt_convert_cb_program() for each file parsed

           status = gnucobol_parsetree_main (cobol_argc, cobol_argv);

           std::cout << "\ncobol_main: IN DEVELOPMENT (called gnucobol_parsetree_main) ...\n\n";

           ROSE_ASSERT (status == 0);

           free(cobol_argv[0]);
           free(cobol_argv[1]);

        // Output digraph file
           CobolSupport::CobolGraph cobol_graph(filename);

           cobol_graph.graph(cobpt_program, cobpt_local_cache, NULL);
        }

     return status;
   }


int parse_using_stratego(int argc, char** argv, SgSourceFile* sg_source_file)
  {
     int status = 1;

     std::string stratego_bin_path = STRATEGO_BIN_PATH;
     assert (stratego_bin_path.empty() == false);

  // Step 1 - Parse the input file
  // ------
     std::string commandString = stratego_bin_path + "/sglri";

  // Filename is obtained from the source-file object
     std::string filenameWithPath = sg_source_file->getFileName();
     std::string filenameWithoutPath = Rose::StringUtility::stripPathFromFileName(filenameWithPath);
     commandString += " -i " + filenameWithPath;

  // Add path to the parse table (located in the source tree)
     std::string parse_table_path = "src/3rdPartyLibraries/experimental-cobol-parser/bin/Cobol.tbl";
     std::string parse_table = findRoseSupportPathFromSource(parse_table_path, "bin");
     commandString += " -p " + parse_table;

  // Add source code location information to output
  // commandString += " --preserve-locations";

  // Output the transformed aterm file
     commandString += " -o " + filenameWithoutPath + ".aterm";
     std::cout << "PARSER command: " << commandString << "\n";

  // Make system call to run parser and output ATerm parse-tree file
     status = system(commandString.c_str());
     if (status != 0)
        {
           fprintf(stderr, "\nFAILED: in cobol_main(), parse_using_stratego(): unable to parse file %s\n\n", filenameWithoutPath.c_str());
           return status;
        }

  // Step 2 - Traverse the ATerm parse tree and convert into Untyped nodes
  // ------

  // Initialize the ATerm library
     ATinitialize(argc, argv);

     std::string aterm_filename = filenameWithoutPath + ".aterm";

     std::cout << "OPENING ATerm parse-tree file " << aterm_filename << "\n";

  // Read the ATerm file that was created by the parser
     FILE * file = fopen(aterm_filename.c_str(), "r");
     if (file == NULL)
        {
           fprintf(stderr, "\nFAILED: in cobol_main(), parse_using_stratego(): unable to open file %s\n\n", aterm_filename.c_str());
           return 1;
        }

     ATerm module_term = ATreadFromTextFile(file);
     fclose(file);

     std::cout << "SUCCESSFULLY read ATerm parse-tree file " << "\n";

     ATermSupport::ATermToUntypedCobolTraversal* aterm_traversal = NULL;

     aterm_traversal = new ATermSupport::ATermToUntypedCobolTraversal(sg_source_file);

     if (aterm_traversal->traverse_CobolSourceProgram(module_term) != ATtrue)
        {
           fprintf(stderr, "\nFAILED: in cobol_main(), parse_using_stratego(): unable to traverse ATerm file %s\n\n", aterm_filename.c_str());
           return 1;
        }

     std::cout << "\nSUCCESSFULLY traversed Cobol parse-tree" << "\n\n";

     return 0;
  }
