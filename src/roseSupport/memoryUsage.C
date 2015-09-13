// tps (01/14/2010) : Switching from rose.h to sage3.
//#include "sage3basic.h"

// This function came from PETSc -- it may not work on all computers as written

// Do this for now -- works on Linux machines:
// #define PETSC_HAVE_GETRUSAGE 1
// #define PETSC_HAVE_STDLIB_H 1
// #define PETSC_HAVE_SYS_RESOURCE_H 1
#define PETSC_USE_PROC_FOR_SIZE 1
#define PETSC_HAVE_GETPAGESIZE 1
#define PETSC_MAX_PATH_LEN     4096

typedef double PetscLogDouble;

#if defined(PETSC_HAVE_PWD_H)
#include <pwd.h>
#endif
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#if defined(PETSC_HAVE_UNISTD_H)
#include <unistd.h>
#endif
#if defined(PETSC_HAVE_STDLIB_H)
#include <stdlib.h>
#endif
#if defined(PETSC_HAVE_SYS_UTSNAME_H)
#include <sys/utsname.h>
#endif
#include <fcntl.h>
#include <time.h>  
#if defined(PETSC_HAVE_SYS_SYSTEMINFO_H)
#include <sys/systeminfo.h>
#endif

#if defined(PETSC_HAVE_TASK_INFO)
#include <mach/mach.h>
#endif

#if defined(PETSC_HAVE_SYS_RESOURCE_H)
#include <sys/resource.h>
#endif
#if defined(PETSC_HAVE_SYS_PROCFS_H)
/* #include <sys/int_types.h> Required if using gcc on solaris 2.6 */
#include <sys/procfs.h>
#endif
#if defined(PETSC_HAVE_FCNTL_H)
#include <fcntl.h>
#endif

/*@
   PetscMemoryGetCurrentUsage - Returns the current resident set size (memory used)
   for the program.

   Not Collective

   Output Parameter:
.   mem - memory usage in bytes

   Options Database Key:
.  -memory_info - Print memory usage at end of run
.  -malloc_log - Activate logging of memory usage

   Level: intermediate

   Notes:
   The memory usage reported here includes all Fortran arrays 
   (that may be used in application-defined sections of code).
   This routine thus provides a more complete picture of memory
   usage than PetscMallocGetCurrentUsage() for codes that employ Fortran with
   hardwired arrays.

.seealso: PetscMallocGetMaximumUsage(), PetscMemoryGetMaximumUsage(), PetscMallocGetCurrentUsage()

   Concepts: resident set size
   Concepts: memory usage

@*/

#if 0
// This code is removed and new code based on this is in ROSE/src/midend/astDiagnostics/astPerformance.[hC]

long int
rose::getCurrentMemoryUsage()
   {
  // PetscLogDouble mem;
     long int mem;

#if defined(PETSC_USE_PROCFS_FOR_SIZE)

#error "DEAD CODE!"

  FILE                   *file;
  int                    fd;
  char                   proc[PETSC_MAX_PATH_LEN];
  prpsinfo_t             prusage;
#elif defined(PETSC_USE_SBREAK_FOR_SIZE)

#error "DEAD CODE!"

  long                   *ii = sbreak(0); 
  int                    fd = ii - (long*)0; 
#elif defined(PETSC_USE_PROC_FOR_SIZE)
  FILE                   *file;
  char                   proc[PETSC_MAX_PATH_LEN];
  int                    mm,rss;
#elif defined(PETSC_HAVE_TASK_INFO)

#error "DEAD CODE!"

  /*  task_basic_info_data_t ti;
      unsigned int           count; */
  /* 
     The next line defined variables that are not used; but if they 
     are not included the code crashes. Something must be wrong
     with either the task_info() command or compiler corrupting the 
     stack.
  */
  /* kern_return_t          kerr; */
#elif defined(PETSC_HAVE_GETRUSAGE)

#error "DEAD CODE!"

  static struct rusage   temp;
#endif

#if defined(PETSC_USE_PROCFS_FOR_SIZE)

#error "DEAD CODE!"

  sprintf(proc,"/proc/%d",(int)getpid());
  if ((fd = open(proc,O_RDONLY)) == -1) {
    SETERRQ1(PETSC_ERR_FILE_OPEN,"Unable to access system file %s to get memory usage data",file);
  }
  if (ioctl(fd,PIOCPSINFO,&prusage) == -1) {
    SETERRQ1(PETSC_ERR_FILE_READ,"Unable to access system file %s to get memory usage data",file); 
  }
  mem = (PetscLogDouble)prusage.pr_byrssize;
  close(fd);

#elif defined(PETSC_USE_SBREAK_FOR_SIZE)

#error "DEAD CODE!"

  mem = (PetscLogDouble)(8*fd - 4294967296); /* 2^32 - upper bits */

#elif defined(PETSC_USE_PROC_FOR_SIZE) && defined(PETSC_HAVE_GETPAGESIZE)
     sprintf(proc,"/proc/%d/statm",(int)getpid());
     if (!(file = fopen(proc,"r")))
        {
          printf("Unable to access system file %s to get memory usage data",proc);
        }
     fscanf(file,"%d %d",&mm,&rss);
  // mem = ((PetscLogDouble)rss) * ((PetscLogDouble)getpagesize());
     mem = rss * getpagesize();
     fclose(file);

#elif defined(PETSC_HAVE_TASK_INFO)

#error "DEAD CODE!"

  mem = 0;
  /* if ((kerr = task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)&ti,&count)) != KERN_SUCCESS) SETERRQ1(PETSC_ERR_LIB,"Mach system call failed: kern_return_t ",kerr);
   mem = (PetscLogDouble) ti.resident_size; */
  
#elif defined(PETSC_HAVE_GETRUSAGE)

#error "DEAD CODE!"

  getrusage(RUSAGE_SELF,&temp);
  cout << "getrusage: temp.ru_maxrss= " << temp.ru_maxrss << endl;
#if defined(PETSC_USE_KBYTES_FOR_SIZE)

#error "DEAD CODE!"

  mem = 1024.0 * ((PetscLogDouble)temp.ru_maxrss);
#else
  mem = ((PetscLogDouble)getpagesize())*((PetscLogDouble)temp.ru_maxrss);
#endif

#else

#error "DEAD CODE!"

  mem = 0.0;
#endif

  // DQ (12/11/2006): Modified to return Kilobytes instead of Megabytes
  // return double(mem/(1024.*1024.));
  // return double(mem/(1024.0));
     return double(mem/(1024.0));
}

#endif

