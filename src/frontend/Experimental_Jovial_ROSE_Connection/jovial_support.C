// C++ code calling an Jovial frontend function.

#include <rose_paths.h>

#include "rose.h"

#include <assert.h>

#include "jovial_support.h"

// using namespace std;

#ifdef BUILD_EXECUTABLE
int main(int argc, char** argv)
#else
int jovial_main(int argc, char** argv)
#endif
   {
     int status = 0;

     printf ("Call to Jovial frontend not implemented! \n");

  // Rasmussen (9/28/2017): A start at implementing (see fortran_support.C).
  // Need to:
  //   1. Process command line args for filename ...
  //   2. Form command to call SDF parser for input file (creating filename.jov.aterm)
  //   3. Read filename.jov.aterm
  //   4. Traverse filename.jov.aterm creating Sage untyped nodes
  //   5. Traverse the Sage untyped nodes to complete the Sage IR

     string stratego_bin_path = STRATEGO_BIN_PATH;
     ROSE_ASSERT(stratego_bin_path.empty() == false);

     string commandString = stratego_bin_path + "/sglri ";

  // Step 1
  // Parse each filename (args not associated with "--parseTable", "--" or "-I")


  // Step 2
     err = system(commandString.c_str());

  // Step 3
     ATerm program_term = ATreadFromTextFile(file);
     fclose(file);

  // Step 4
     if (ofp_traversal->traverse_Program(program_term) != ATtrue)
        {
           status = 1;
           return status;
        }

//----------------------------------------------------------------------
//  Traverse the SgUntypedFile object and convert to regular sage nodes
//----------------------------------------------------------------------

  // Step 5

  // Build the traversal object
     Jovial::Untyped::UntypedTraversal sg_traversal(globalFilePointer);
     Jovial::Untyped::InheritedAttribute scope = NULL;

  // Traverse the untyped tree and convert to sage nodes
     sg_traversal.traverse(ofp_traversal->get_file(),scope);

     if (ofp_traversal)  delete ofp_traversal;

     assert (status == 0);

     return status;
   }



