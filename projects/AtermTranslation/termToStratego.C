#include "rose.h"
#include "aterm1.h"
#include "atermTranslation.h"

int main (int argc, char* argv[]) 
   {
     printf ("Building ATerm bottom; \n");
     ATerm bottom;

     printf ("Building the ROSE AST \n");
     SgProject* project = frontend(argc,argv);

     printf ("Calling ATinit \n");
     ATinit(argc, argv, &bottom);

     printf ("Calling convertNodeToAterm \n");
     ATerm term = convertNodeToAterm(project /*.get_file(0).get_root()*/);
     printf ("DONE: Calling convertNodeToAterm \n");

     ROSE_ASSERT (term != NULL);

#if 0
  // DQ (3/23/2013): This was already commented out...
     AST_FILE_IO::startUp(sageProject);
     std::string astBlob = AST_FILE_IO::writeASTToString();
     term = ATsetAnnotation(term, ATmake("ast"), ATmake("<blob>", astBlob.length(), astBlob.data()));
#endif

#if 0
  // DQ (3/23/2013): commented out while debugging.
  // This can be usefull for piping output to other Stratego and Aterm tools/phases.
     ATwriteToBinaryFile(term, stdout);
#endif

  // Open a file for writing...
     FILE* f = fopen("atermFile.aterm","w");

  // Write the aterm to the file.
     ATbool status = ATwriteToTextFile(term,f);
     ROSE_ASSERT(status == ATtrue);

  // Close the file.
     fclose(f);

#if 0
  // Testing the ATerm file I/O.
  // Open a file for writing...
     FILE* file = fopen("atermFile.aterm","r");

     printf ("Read aterm file \n");

  // Write the aterm to the file.
     ATerm term2 = ATreadFromTextFile(file);
     ROSE_ASSERT(term2 != NULL);

     printf ("Closing file after reading aterm \n");

  // Close the file.
     fclose(file);
#endif

     return 0;
   }
