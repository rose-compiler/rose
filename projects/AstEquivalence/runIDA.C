#include <rose.h>
#include <stdlib.h>
using namespace std;

int
main(int argc, char** argv)
   {
     ios::sync_with_stdio();     // Syncs C++ and C I/O subsystems!

     std::vector<char*> newArgs;
     for(int i =0; i < argc ; ++i)
       newArgs.push_back(strdup(argv[i]));

     newArgs.push_back(strdup("-rose:read_executable_file_format_only"));
     newArgs.push_back(0);
     
     SgProject* project = frontend(newArgs.size(),&newArgs[0]);
     ROSE_ASSERT (project != NULL);

     //Files will only exist for executables
     for(int i =0; i < project->numberOfFiles(); ++i)
     {
       SgFile* file = (*project)[i];
       std::string filename = file->getFileName(); 



     }

     return 1;
   }
