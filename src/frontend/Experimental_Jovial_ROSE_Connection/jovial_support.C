// C++ code calling an Jovial frontend function.

// DQ (11/13/2017): This is a policy violation, sage3basic.h must be the first file included.
// #include "rose_config.h"

#include "sage3basic.h"

#include "rose_config.h"

#include <assert.h>
#include <iostream>
#include <string>

#include "jovial_support.h"
#include "ATermToUntypedJovialTraversal.h"
#include "UntypedJovialTraversal.h"

int jovial_main(int argc, char** argv, SgSourceFile* sg_source_file)
   {
     int status;
     std::string parse_table;

     assert(sg_source_file != NULL);

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
     std::string parse_table_path = "src/3rdPartyLibraries/experimental-jovial-parser/bin/Jovial.tbl";
     parse_table = findRoseSupportPathFromSource(parse_table_path, "bin");
     commandString += " -p " + parse_table;

  // Add source code location information to output
     commandString += " --preserve-locations";

  // Output the transformed aterm file
     commandString += " -o " + filenameWithoutPath + ".aterm";
     std::cout << "PARSER command: " << commandString << "\n";

  // Make system call to run parser and output ATerm parse-tree file
     status = system(commandString.c_str());
     if (status != 0)
        {
           fprintf(stderr, "\nFAILED: in jovial_main(), unable to parse file %s\n\n", filenameWithoutPath.c_str());
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
           fprintf(stderr, "\nFAILED: in jovial_main(), unable to open file %s\n\n", aterm_filename.c_str());
           return 1;
        }

     ATerm module_term = ATreadFromTextFile(file);
     fclose(file);

     std::cout << "SUCCESSFULLY read ATerm parse-tree file " << "\n";

     ATermSupport::ATermToUntypedJovialTraversal* aterm_traversal = NULL;

     aterm_traversal = new ATermSupport::ATermToUntypedJovialTraversal(sg_source_file);

     if (aterm_traversal->traverse_Module(module_term) != ATtrue)
        {
           fprintf(stderr, "\nFAILED: in jovial_main(), unable to traverse ATerm file %s\n\n", aterm_filename.c_str());
           return 1;
        }

     std::cout << "\nSUCCESSFULLY traversed Jovial parse-tree" << "\n\n";

  // Rasmussen (11/9/17): Create a dot file.  This is temporary or should
  // at least be a rose option.
     SgUntypedGlobalScope* global_scope = aterm_traversal->get_scope();
     generateDOT(global_scope, filenameWithoutPath + ".ut");

  // Step 3 - Traverse the SgUntypedFile object and convert to regular sage nodes
  // ------

  // Build the ATerm traversal object

     Jovial::Untyped::UntypedTraversal sg_traversal(sg_source_file);
     Jovial::Untyped::InheritedAttribute scope = NULL;

  // Traverse the untyped tree and convert to sage nodes
     sg_traversal.traverse(aterm_traversal->get_file(),scope);

  // Generate dot file for Sage nodes.
     generateDOT(SageBuilder::getGlobalScopeFromScopeStack(), filenameWithoutPath);

     if (aterm_traversal)  delete aterm_traversal;

     return 0;
   }
