#include "rose_msvc.h"

#include "processSupport.h"

#if !ROSE_MICROSOFT_OS
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cassert>
#endif

#include <cstdlib>
#include <cstring>

// DQ (3/22/2009): This should be required, but only MSVS catches it.
#include <assert.h>

using namespace std;

int systemFromVector(const vector<string>& argv) {
  assert (!argv.empty());

#if !ROSE_MICROSOFT_OS
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
#else
    vector<const char*> argvC(argv.size() + 1);
    for (size_t i = 0; i < argv.size(); ++i) {
      argvC[i] = strdup(argv[i].c_str());
    }
    argvC.back() = NULL;
//    execvp(argv[0].c_str(), (char* const*)&argvC[0]);
//  assert(false);

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si,sizeof(si));
	si.cb=sizeof(si);
	ZeroMemory(&pi,sizeof(pi));
	if(!CreateProcess(argv[0].c_str(),(char* )&argvC[0],0,0,0,0,0,0,&si,&pi)) {
		// todo Windows: tps: Does not work right now. This is called when a generated program is compiled
		//Could not start process;
	     printf ("Error: no MSVS implementation available. Final code not compiled for now. \n");
		// assert(false);
		 return 1;
	}
	WaitForSingleObject(pi.hProcess,INFINITE);

  return 1;
#endif
}

// EOF is not handled correctly here -- EOF is normally set when the child
// process exits
FILE* popenReadFromVector(const vector<string>& argv) {
  assert (!argv.empty());
  int pipeDescriptors[2];

#if !ROSE_MICROSOFT_OS
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
#else
	// tps: does not work right now. Have not hit this assert yet.
  printf ("Error: no MSVS implementation available popenReadFromVector() (not implemented) \n");
  assert(false);

  return NULL;
#endif
}

int pcloseFromVector(FILE* f)
   {
  // Assumes there is only one child process

  // DQ (11/28/2009): MSVC warns of status not being used, so initialize it.
     int status = 0;

#if !ROSE_MICROSOFT_OS
  /* pid_t err = */ wait(&status);
#else
     printf ("Error: no MSVS implementation available pcloseFromVector()(not implemented) \n");
     assert(false);
#endif

     fclose(f);
     return status;
   }

rose_exception::rose_exception( const char *what )
    : what_( what )
{}

const char *rose_exception::what() const throw()
   {
     return what_;
   }

// DQ (8/22/2009): Added throw since EDG wants to see that the throw options match when ROSE_ABORT 
// is a macro to "abort()" in "stdlib.h". Als, attributes are not permitted in a function definition
// So this definition just uses "__THROW"
// void ROSE_ABORT()
// #ifdef __APPLE__
//#ifdef __GNUC__
//#  include <features.h>
//#if __GNUC_PREREQ(4,3)
//       If gcc_version >= 4.3
//#else

#ifdef USE_ROSE
void ROSE_ABORT() __THROW
#else
void ROSE_ABORT() throw()
#endif
{
    throw rose_exception( "abort" );
}

//#endif
//#endif

void ROSE_ABORT( const char *message )
{
    throw rose_exception( message );
}
