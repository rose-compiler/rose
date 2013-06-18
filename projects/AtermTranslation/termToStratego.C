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

     ROSE_ASSERT (term);

#if 0
  // DQ (3/23/2013): This was already commented out...
     AST_FILE_IO::startUp(sageProject);
     std::string astBlob = AST_FILE_IO::writeASTToString();
     term = ATsetAnnotation(term, ATmake("ast"), ATmake("<blob>", astBlob.length(), astBlob.data()));
#endif

#if 0
  // DQ (3/23/2013): commented out while debugging.
     ATwriteToBinaryFile(term, stdout);
#endif

     return 0;
   }
