#include <rose.h>
#include <vector>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include "spawnFrontend.h"

using namespace std;

SgProject* spawnFrontend(int argc, char *argv[])
   {
     int descriptors[2];
     int r = pipe(descriptors);
     if (r == -1)
        {
          perror("spawnFrontend: pipe: "); 
          return 0;
        }

     pid_t childPid = fork();
     switch (childPid)
        {
          case -1:
             {
            // Error case
               perror("spawnFrontend: fork: "); 
               return 0;
             }

          case 0:
             {
            // Child case
               r = dup2(descriptors[1], 1);
               if (r == -1)
                  {
                    perror("spawnFrontend: dup2: "); 
                    return 0;
                  }
               vector<char*> argv2(argc + 1);
               std::copy(argv, argv + argc, argv2.begin());
               argv2[argc] = 0;
               r = execve("./frontendAndDump", &argv2[0], environ);
               if (r == -1)
                  {
                    perror("spawnFrontend: execve: "); 
                    return 0;
                  }
             }

          default:
             {
            // Parent case
               ostringstream filenameStream;
               filenameStream << "/dev/fd/" << descriptors[0];

               string filename = filenameStream.str();
               int status;
               pid_t waitedPid = waitpid(childPid, &status, WNOHANG);

               if (waitedPid == -1)
                  {
                    perror("spawnFrontend: first waitpid: "); 
                    return 0;
                  }

               if (waitedPid != 0 && status != 0) 
                    return 0;

               SgProject* proj = AST_FILE_IO::readASTFromFile(filename);
               if (waitedPid == 0)
                  {
                 /* Process was still running as of last check */
                    waitedPid = waitpid(childPid, &status, 0);
                  }
               
               if (waitedPid == -1)
                  {
                    perror("spawnFrontend: second waitpid: ");
                    return 0;
                  }
               ROSE_ASSERT (status == 0);

               return proj;
            // Bug: this code hangs if the shelled frontendAndDump process assert fails before writing out the AST
             }
        }
   }
