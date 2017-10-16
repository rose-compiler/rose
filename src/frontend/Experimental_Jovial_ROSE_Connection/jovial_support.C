// C++ code calling an Jovial frontend function.

#include "rose_config.h"

#include "sage3basic.h"

#include <assert.h>
#include <iostream>
#include <string>

#include "jovial_support.h"
#include "ATtoUntypedJovialTraversal.h"

//TODO: int jovial_main(int argc, char** argv, SgSourceFile* sg_source_file)
int jovial_main(int argc, char** argv)
   {
     int status = 0;

     printf ("\n WARNING: Call to Jovial frontend not yet implemented! \n\n");

  // Rasmussen (9/28/2017): A start at implementing (see fortran_support.C).
  // Need to:
  //   1. Process command line args for filename ...
  //   2. Form command to call SDF parser for input file (creating filename.jov.aterm)
  //   3. Read filename.jov.aterm
  //   4. Traverse filename.jov.aterm creating Sage untyped nodes
  //   5. Traverse the Sage untyped nodes to complete the Sage IR

  // TODO
  // std::string stratego_bin_path = STRATEGO_BIN_PATH;
     std::string stratego_bin_path = "/nfs/casc/overture/ROSE/opt/rhel7/x86_64/stratego/strategoxt-0.17.1/bin";
     assert (stratego_bin_path.empty() == false);

  // TODO - could be from ROSE build tree
  // std::string jovial_bin_path = JOVIAL_BIN_PATH;
     std::string jovial_bin_path = "/nfs/casc/overture/ROSE/opt/rhel7/x86_64/stratego/jovial-sdf-0.5/bin";
     assert (jovial_bin_path.empty() == false);

     std::string commandString = stratego_bin_path + "/sglri";
     std::cout << "COMMAND: " << commandString << "\n";


  // DQ (9/29/2017): Added ifdef to ignore this when ROSE is not configured to use STRATEGO.
  // #ifdef USE_ROSE_STRATEGO_SUPPORT

  // Step 1
  // ------

  // Parse each filename (args not associated with "--parseTable", "--" or "-I")

  // TODO
     std::string filenameWithPath = "tiny.jovial";
  // TODO
  // std::string filenameWithoutPath = StringUtility::stripPathFromFileName(filenameWithPath);
     std::string filenameWithoutPath = filenameWithPath;

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

     filenameWithoutPath += ".aterm";

     std::cout << "OPENING ATerm parse-tree file " << filenameWithoutPath << "\n";

  // Read the ATerm file that was created by the parser
     FILE * file = fopen(filenameWithoutPath.c_str(), "r");
     if (file == NULL)
        {
           fprintf(stderr, "\nFAILED: in jovial_main(), unable to open file %s\n\n", filenameWithoutPath.c_str());
           return 1;
        }

     ATerm module_term = ATreadFromTextFile(file);
     fclose(file);

     std::cout << "SUCCESSFULLY read ATerm parse-tree file " << "\n";

  // Step 4
  // ------

     Jovial::ATtoUntypedJovialTraversal* aterm_traversal = NULL;

  // TODO
  // aterm_traversal = new Jovial::ATtoUntypedJovialTraversal(sg_source_file);
     aterm_traversal = new Jovial::ATtoUntypedJovialTraversal(NULL);

     if (aterm_traversal->traverse_Module(module_term) != ATtrue)
        {
           status = 1;
           return status;
        }

     std::cout << "\nSUCCESSFULLY traversed Jovial parse-tree" << "\n\n";

//----------------------------------------------------------------------
//  Traverse the SgUntypedFile object and convert to regular sage nodes
//----------------------------------------------------------------------

#if 0
  // Step 5
  // ------

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



