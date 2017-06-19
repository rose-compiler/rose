// This code tests the classification of files.
// Files are classified at either unknown, user, 
// or system (system includes system libaries).

// The support for this work is in ROSE/src/util/stringSupport/FileNameClassifier.C


#include "rose.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>

#include <libgen.h>             /* basename(), dirame()               */

using namespace std;
using namespace Rose::StringUtility;


void 
visitorTraversal::visit(SgNode* n)
   {
     SgStatement* statement = isSgStatement(n);
     if (statement != NULL)
        {
          string filename = statement->get_file_info()->get_filename();

       // Skip the case of compiler generated Sg_File_Info objects.
          if (previousFilename != filename && filename != "compilerGenerated")
             {
               printf ("\n\nfilename = %s statement = %s \n",filename.c_str(),n->class_name().c_str());

               FileNameClassification classification;
#if 1
            // string sourceDir = "/home/dquinlan/ROSE/roseCompileTree-g++4.2.2/developersScratchSpace/Dan/fileLocation_tests";

            // This causes the path edit distance to be: 4
               string sourceDir = "/home/dquinlan/ROSE/git-dq-main-rc";

            // This causes the path edit distance to be: 0
            // string sourceDir = "/home/dquinlan/ROSE";

               classification = classifyFileName(filename,sourceDir);
#else
               string home = "/home/dquinlan/";
               string sourceDir = home + "ROSE/svn-rose/";
               classification = classifyFileName("/usr/include/stdio.h",sourceDir);
#endif

               FileNameLocation fileTypeClassification = classification.getLocation();
               FileNameLibrary  libraryClassification  = classification.getLibrary();
               int pathEditDistance = classification.getDistanceFromSourceDirectory();
               
               printf ("fileTypeClassification = %d \n",fileTypeClassification);
               display(fileTypeClassification,"Display fileTypeClassification");
               printf ("libraryClassification  = %d \n",libraryClassification);
               display(libraryClassification,"Display libraryClassification");
               printf ("pathEditDistance       = %d \n",pathEditDistance);

               ROSE_ASSERT(isLink(filename) == false);
             }

          previousFilename = filename;
        }
   }

int
main(int argc, char * argv[])
   {
     SgProject *project = frontend (argc, argv);

     visitorTraversal myvisitor;
     myvisitor.traverse(project,preorder);

     return backend(project);
   }


