// DQ (9/27/2013): This is required to be defined for the 64bit ATerm support.
#if (__x86_64__ == 1)
// 64-bit machines are required to set this before including the ATerm header files.
   #define SIZEOF_LONG 8
   #define SIZEOF_VOID_P 8
#else
// 32-bit machines need not have the values defined (but it is required for this program).
   #define SIZEOF_LONG 4
   #define SIZEOF_VOID_P 4
#endif

#include "rose.h"

// #include "rose_aterm_api.h"
#include "aterm1.h"
#include "aterm2.h"

#include <libgen.h>
#include <boost/lexical_cast.hpp>
#include <string>
#include <iomanip>

#include <fstream>
#include <vector>
#include <set>
#include <assert.h>

#include "atermTranslation.h"

using namespace std;

int
main(int argc, char* argv[])
   {
  // This program tests the conversion of the ROSE AST to n ATerm.

  // Generate a ROSE AST as input.
  // printf ("Building the ROSE AST \n");
     SgProject* project = frontend(argc,argv);
  // printf ("Done: Building the ROSE AST \n");

     SgFile* roseFile = project->operator[](0);
     ROSE_ASSERT(roseFile != NULL);
     SgSourceFile* sourceFile = isSgSourceFile(roseFile);
     ROSE_ASSERT(sourceFile != NULL);

  // printf ("Calling ATinit \n");
     ATerm bottom;
     ATinit(argc, argv, &bottom);
#if 0
     printf ("Calling AtermSupport::convertAstNodeToRoseAterm \n");
#endif
  // Actually this implementation in convertNodeToAterm already adds the pointer value to the aterm, so we can just return this Aterm directly.
     ATerm term = AtermSupport::convertNodeToAterm(sourceFile);
#if 0
     printf ("DONE: Calling AtermSupport::convertAstNodeToRoseAterm term = %p \n",term);
#endif
     ROSE_ASSERT (term != NULL);

     string roseAST_filename = project->get_file(0).getFileName();

  // Write out the ATerm as a text file format.
     string aterm_filename_TAF = roseAST_filename + "_TAF.aterm";
     char* s_TAF = strdup(aterm_filename_TAF.c_str());
     string file_basename_TAF = basename(s_TAF);
#if 0
     printf ("aterm_filename_TAF = %s \n",file_basename_TAF.c_str());
#endif
     FILE* aterm_file_TAF = fopen(file_basename_TAF.c_str(),"w");
#if 0
     printf ("aterm_file_TAF = %p \n",aterm_file_TAF);
#endif
     ATbool success_TAF = ATwriteToTextFile(term,aterm_file_TAF);
     ROSE_ASSERT(success_TAF != 0);
     fclose(aterm_file_TAF);

  // Build the binary file format.
     string aterm_filename_BAF = roseAST_filename + "_BAF.aterm";
     char* s_BAF = strdup(aterm_filename_BAF.c_str());
     string file_basename_BAF = basename(s_BAF);
#if 0
     printf ("file_basename_BAF = %s \n",file_basename_BAF.c_str());
#endif
     FILE* aterm_file_BAF = fopen(file_basename_BAF.c_str(),"w");
#if 0
     printf ("aterm_file_BAF = %p \n",aterm_file_BAF);
#endif
     ATbool success_BAF = ATwriteToBinaryFile(term,aterm_file_BAF);
     ROSE_ASSERT(success_BAF != 0);
     fclose(aterm_file_BAF);

#if 0
  // This ATerm file format appears to not be supported (was supported through at least 1.8, but not in version 2.5 required for STRATEGO).
  // Build the Streamable file format.
     string aterm_filename_SAF = roseAST_filename + "_SAF.aterm";
     FILE* aterm_file_SAF = fopen(aterm_filename_SAF.c_str(),"w");
     ATbool success_SAF = ATwriteToSAFFile(term,aterm_file_SAF);
     ROSE_ASSERT(success_SAF != 0);
     fclose(aterm_file_SAF);
#endif
#if 0
     printf ("Program Terminated Normally \n");
#endif
     return 0;
   }
