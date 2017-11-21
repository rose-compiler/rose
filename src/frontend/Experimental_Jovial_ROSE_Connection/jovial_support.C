// C++ code calling an Jovial frontend function.

// DQ (11/13/2017): This is a policy violation, sage3basic.h must be the first file included.
// #include "rose_config.h"

#include "sage3basic.h"

#include "rose_config.h"

#include <assert.h>
#include <iostream>
#include <string>

#include "jovial_support.h"
#include "ATtoUntypedJovialTraversal.h"

int jovial_main(int argc, char** argv, SgSourceFile* sg_source_file)
   {
     int i;
     int status = 1;
     std::string parse_table;

     printf ("\n WARNING: Call to Jovial frontend not yet fully implemented! \n\n");

     assert(sg_source_file != NULL);

  // Rasmussen (9/28/2017): A start at implementing (see fortran_support.C).
  // Need to:
  //   1. Process command line args for filename ...
  //   2. Form command to call SDF parser for input file (creating filename.jov.aterm)
  //   3. Read filename.jov.aterm
  //   4. Traverse filename.jov.aterm creating Sage untyped nodes
  //   5. Traverse the Sage untyped nodes to complete the Sage IR

  // TODO!!!
  // std::string stratego_bin_path = STRATEGO_BIN_PATH;
     std::string stratego_bin_path = "/nfs/casc/overture/ROSE/opt/rhel7/x86_64/stratego/strategoxt-0.17.1/bin";
     assert (stratego_bin_path.empty() == false);

  // TODO!!! - could be from ROSE build tree
  // std::string jovial_bin_path = JOVIAL_BIN_PATH;
     std::string jovial_bin_path = "/nfs/casc/overture/ROSE/opt/rhel7/x86_64/stratego/jovial-sdf-0.5/bin";
     assert (jovial_bin_path.empty() == false);

     std::string commandString = stratego_bin_path + "/sglri";
     std::cout << "COMMAND: " << commandString << "\n";

  // DQ (9/29/2017): Added ifdef to ignore this when ROSE is not configured to use STRATEGO.
  // #ifdef USE_ROSE_STRATEGO_SUPPORT

  // Step 1
  // ------

  // Filename can be obtained from the source-file object
     std::string filenameWithPath = sg_source_file->getFileName();
     std::string filenameWithoutPath = Rose::StringUtility::stripPathFromFileName(filenameWithPath);

  // Parse each filename (args not associated with "--parseTable", "--" or "-I")
     for (i = 1; i < argc; i++)
        {
          std::cout << "ARG " << i << " is " << argv[i] << "\n";
          if (strncmp(argv[i], "--parseTable", 12) == 0)
             {
            // TODO
            // commandString += " -p ";
            // commandString += argv[i+1];
            // commandString += " ";

               parse_table = std::string(argv[i+1]);
               std::cout << "FOUND --parseTable argument: " + parse_table;
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
                       }
                  }
             }
        }

  // Finished processing command line arguments, make sure there is a parse table
     if (parse_table.empty() == true)
        {
          fprintf(stderr, "fortran_parser: no parse table provided, use option --parseTable\n");
       // TODO
       // return status;
        }

  // Step 2
  // ------

  // Add path to the parse table
     commandString += " -p " + jovial_bin_path + "/Jovial.tbl";
     std::cout << "COMMAND: " << commandString << "\n";

  // Add source code location information to output
     commandString += " --preserve-locations";
     std::cout << "COMMAND: " << commandString << "\n";

     commandString += " -i " + filenameWithPath;
     std::cout << "COMMAND: " << commandString << "\n";

  // Output the transformed aterm file
     commandString += " -o " + filenameWithoutPath + ".aterm";
     std::cout << "COMMAND: " << commandString << "\n";

  // Make system call to run parser and output ATerm parse-tree file
     status = system(commandString.c_str());
     std::cout << "COMMAND status = " << status << "\n";

  // Step 3
  // ------

  // Initialize the ATerm library
     ATinitialize(argc, argv);

     std::string aterm_filename = filenameWithoutPath + ".aterm";

     std::cout << "OPENING ATerm parse-tree file " << aterm_filename << "\n";

  // Read the ATerm file that was created by the parser
     FILE * file = fopen(aterm_filename.c_str(), "r");
     if (file == NULL)
        {
           fprintf(stderr, "\nFAILED: in jovial_main(), unable to open file %s\n\n", aterm_filename.c_str());
           return status;
        }

     ATerm module_term = ATreadFromTextFile(file);
     fclose(file);

     std::cout << "SUCCESSFULLY read ATerm parse-tree file " << "\n";

  // Step 4
  // ------

     ATermSupport::ATtoUntypedJovialTraversal* aterm_traversal = NULL;

     aterm_traversal = new ATermSupport::ATtoUntypedJovialTraversal(sg_source_file);

     if (aterm_traversal->traverse_Module(module_term) != ATtrue)
        {
           return status;
        }

     std::cout << "\nSUCCESSFULLY traversed Jovial parse-tree" << "\n\n";

  // Rasmussen (11/9/17): Create a dot file.  This is temporary or should
  // at least be an rose option.
     SgUntypedGlobalScope* global_scope = aterm_traversal->get_scope();
     generateDOT(global_scope, filenameWithoutPath);

//----------------------------------------------------------------------
//  Traverse the SgUntypedFile object and convert to regular sage nodes
//----------------------------------------------------------------------

  // Step 5
  // ------

#if 0
  // Build the traversal object
     Jovial::Untyped::UntypedTraversal sg_traversal(globalFilePointer);
     Jovial::Untyped::InheritedAttribute scope = NULL;

  // Traverse the untyped tree and convert to sage nodes
     sg_traversal.traverse(ofp_traversal->get_file(),scope);

     if (ofp_traversal)  delete ofp_traversal;
#endif

     assert (status == 0);

     return status;
   }



