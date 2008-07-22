#include <sys/stat.h>
#include <sys/wait.h>
#include <vector>
#include <string>
#include <unistd.h>
#include <cassert>
#include <stdlib.h>
#include <string.h>

using namespace std;

int systemFromVector(const vector<string>& argv) {
  assert (!argv.empty());
  pid_t pid = fork();
  if (pid == -1) {perror("fork"); abort();}
  if (pid == 0) { // Child
    vector<const char*> argvC(argv.size() + 1);
    for (size_t i = 0; i < argv.size(); ++i) {
      argvC[i] = strdup(argv[i].c_str());
    }
    argvC.back() = NULL;
    execvp(argv[0].c_str(), (char* const*)&argvC[0]);
    perror(("execvp in systemFromVector: " + argv[0]).c_str());
    exit(1); // Should not get here normally
  } else { // Parent
    int status;
    pid_t err = waitpid(pid, &status, 0);
    if (err == -1) {perror("waitpid"); abort();}
    return status;
  }
}

// EOF is not handled correctly here -- EOF is normally set when the child
// process exits
FILE* popenReadFromVector(const vector<string>& argv) {
  assert (!argv.empty());
  int pipeDescriptors[2];
  int pipeErr = pipe(pipeDescriptors);
  if (pipeErr == -1) {perror("pipe"); abort();}
  pid_t pid = fork();
  if (pid == -1) {perror("fork"); abort();}
  if (pid == 0) { // Child
    vector<const char*> argvC(argv.size() + 1);
    for (size_t i = 0; i < argv.size(); ++i) {
      argvC[i] = strdup(argv[i].c_str());
    }
    argvC.back() = NULL;
    int closeErr = close(pipeDescriptors[0]);
    if (closeErr == -1) {perror("close (in child)"); abort();}
    int dup2Err = dup2(pipeDescriptors[1], 1); // stdout
    if (dup2Err == -1) {perror("dup2"); abort();}
    execvp(argv[0].c_str(), (char* const*)&argvC[0]);
    perror(("execvp in popenReadFromVector: " + argv[0]).c_str());
    exit(1); // Should not get here normally
  } else { // Parent
    int closeErr = close(pipeDescriptors[1]);
    if (closeErr == -1) {perror("close (in parent)"); abort();}
    return fdopen(pipeDescriptors[0], "r");
  }
}

int pcloseFromVector(FILE* f) { // Assumes there is only one child process
  int status;
  /* pid_t err = */ wait(&status);
  fclose(f);
  return status;
}


