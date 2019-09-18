#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int executeBinary(char** args, char* outfile);

int executeBinary(char** args, char* outfile)
{
  int pid = fork();

  if (pid < 0) 
  {
    exit(EXIT_FAILURE);
  }

  if (pid)
  {
    // parent process
    int status = 0;
            
    waitpid(pid, &status, 0); // wait for the child to exit
    
    FILE* file = fopen(outfile, "w");
    fprintf(file, "%d\n", status);
    fclose(file);
    
    if (WIFEXITED(status))
    {
      status = WEXITSTATUS(status);
    }
    
    return status;
  }

  // execute the program
  /*const int errc =*/ execvp(args[0], &args[0]);
  exit(EXIT_FAILURE);
}


// execWrapper output.file specimen arguments..
int main(int argc, char* argv[])
{
  assert(argc >= 3);
   
  return executeBinary(&argv[2], argv[1]); 
}
