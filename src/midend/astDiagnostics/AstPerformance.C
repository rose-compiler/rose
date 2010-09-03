// One include file to include them all!
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "HiddenList.h"
#include <fstream>

#if 1
// file locking support
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#ifndef _MSC_VER
#include <sys/resource.h>
#else
#include <windows.h> 		// getpagesize()
#endif
#endif


#include <boost/thread.hpp>	// sleep()

// DQ (12/8/2006): Linux memory usage mechanism (no longer used, implemented internally (below)).
// #include<memoryUsage.h>

using namespace std;

// DQ (8/29/2007): Part of initial support for more portable timers (suggested by Matt Sottile at LANL)
// JJW (5/21/2008): Changed back to clock() for portability

bool ROSE_MemoryUsage::getStatmInfo() {
  FILE *file = fopen("/proc/self/statm", "r");;
  if (!file) {
    // printf("Unable to access system file /proc/self/statm to get memory usage data\n");
    return false;
  }
  int numEntries = fscanf(file, "%d %d %d %d %d %d %d",
			  &memory_pages, &resident_pages, &shared_pages,
			  &code_pages, &stack_pages, &library_pages,
			  &dirty_pages);
  if (numEntries != 7) return false;
  fclose(file);
  return true;
}


int
ROSE_MemoryUsage::getAvailableMemoryPages() const
   {
     return memory_pages;
   }

int
ROSE_MemoryUsage::getNumberOfResidentPages() const
   {
     return resident_pages;
   }

int
ROSE_MemoryUsage::getNumberOfSharedPages() const
   {
     return shared_pages;
   }

int
ROSE_MemoryUsage::getNumberOfCodePages() const
   {
     return code_pages;
   }

int
ROSE_MemoryUsage::getNumberOfStackPages() const
   {
     return stack_pages;
   }

int
ROSE_MemoryUsage::getNumberOfLibraryPages() const
   {
     return library_pages;
   }

int
ROSE_MemoryUsage::getNumberOfDirtyPages() const
   {
     return dirty_pages;
   }

int
ROSE_MemoryUsage::getNumberOfCodePlusLibraryPages() const
   {
     return library_pages + code_pages;
   }

long
ROSE_MemoryUsage::getMemoryUsageKilobytes() const
   {
     return ((long)getNumberOfResidentPages() * (long)getPageSizeBytes()) / (long)1024;
   }

int
ROSE_MemoryUsage::getPageSizeBytes() const
   {
#ifdef _MSC_VER

     // CH (4/6/2010): Windows's version of `getpagesize()'
     SYSTEM_INFO system_info;
     GetSystemInfo(&system_info);
     return static_cast<int>(system_info.dwPageSize);

#else
     return getpagesize();
#endif
   }

#if 0
long int
ROSE_MemoryUsage::getCurrentMemoryUsage()
   {
     return getNumberOfResidentPages() * getPageSizeBytes();
   }
#endif

double
ROSE_MemoryUsage::getPageSizeMegabytes() const
   {
     return getPageSizeBytes() / (1024.0 * 1024.0);
   }

double
ROSE_MemoryUsage::getAvailableMemoryMegabytes() const
   {
     return getAvailableMemoryPages() * getPageSizeMegabytes();
   }

double
ROSE_MemoryUsage::getNumberOfResidentMegabytes() const
   {
     return getNumberOfResidentPages() * getPageSizeMegabytes();
   }

double
ROSE_MemoryUsage::getNumberOfSharedMegabytes() const
   {
     return getNumberOfSharedPages() * getPageSizeMegabytes();
   }

double
ROSE_MemoryUsage::getNumberOfCodeMegabytes() const
   {
     return getNumberOfCodePages() * getPageSizeMegabytes();
   }

double
ROSE_MemoryUsage::getNumberOfStackMegabytes() const
   {
     return getNumberOfStackPages() * getPageSizeMegabytes();
   }

double
ROSE_MemoryUsage::getNumberOfLibraryMegabytes() const
   {
     return getNumberOfLibraryPages() * getPageSizeMegabytes();
   }

double
ROSE_MemoryUsage::getNumberOfDirtyMegabytes() const
   {
     return getNumberOfDirtyPages() * getPageSizeMegabytes();
   }

double
ROSE_MemoryUsage::getMemoryUsageMegabytes() const
   {
     return getNumberOfResidentPages() * getPageSizeMegabytes();
   }

double
ROSE_MemoryUsage::getNumberOfCodePlusLibraryMegabytes() const
   {
     return getNumberOfCodePlusLibraryPages() * getPageSizeMegabytes();
   }



#if 0
// Used for checking if high resolution timer is available
#ifdef __timespec_defined
   #warning "__timespec_defined IS defined"
#else
   #warning "__timespec_defined is NOT defined"
#endif
#endif

// static data defined in the AstPerformance class
std::vector<ProcessingPhase*> AstPerformance::data;

// Support fo hierarchy of performance monitors
std::list<AstPerformance*> AstPerformance::performanceStack;

// static SgProject IR node require for report generation to a file
SgProject* AstPerformance::project = NULL;

AstPerformance::AstPerformance( std::string s , bool outputReport )
   : label(s), outputReportInDestructor(outputReport)
   {
     ProcessingPhase* parentData = NULL;
  // check the stack for an existing performance monitor (it will be come the parent)
     if (performanceStack.size() > 0)
        {
          std::list<AstPerformance*>::iterator i = performanceStack.begin();
          parentData = (*i)->localData;
          assert(parentData != NULL);
          localData = new ProcessingPhase(label,0.0,parentData);
        }
       else
        {
          assert(parentData == NULL);
          localData = new ProcessingPhase(label,0.0,parentData);

       // If this performance monitor does not have a parent then add it to the static list
          data.push_back(localData);
        }

     assert(localData != NULL);

  // I would have hoped that the correct virtual function would have been called, but this didn't work!
  // double resolution = performanceResolution();
  // printf ("resolution = %f \n",resolution);
  // localData->set_resolution(resolution);

  // Put this performance monitor onto the stack
     performanceStack.push_front(this);
   }

AstPerformance::~AstPerformance()
   {
  // printf ("Inside of AstPerformance destructor ... project = %p outputReportInDestructor = %s \n",project,outputReportInDestructor ? "true" : "false");

  // DQ (7/21/2010): Call this here before we get too far into the derived class constructor.
  // localData->set_memory_usage((double) (localData->memoryUsage.getMemoryUsageMegabytes()));

  // Remove this performance monitor from the stack
     performanceStack.pop_front();

  // DQ (9/6/2006): This will reset the time; to a nearly zero value!
  // DQ (9/1/2006): Need to stop the timer and record the elapsed time.
  // localData->stopTiming(timer);

     if (outputReportInDestructor == true)
        {
       // DQ (9/1/2006): Only output the performance report if verbose is set (greater than zero)
          if (project->get_verbose() > 0)
             {
            // Output any saved performance data (see ROSE/src/astDiagnostics/AstPerformance.h)
            // printf ("Calling generateReport() ... \n");
               generateReport();
             }
        }

     if (project != NULL)
        {
       // printf ("Calling generateReportToFile() ... \n");
          generateReportToFile(project);
       // printf ("DONE: Calling generateReportToFile() ... \n");
        }
       else
        {
       // printf ("Skipped performance report generation to the performance file \n");
        }

  // printf ("Leaving AstPerformance destructor ... \n");
   }

ProcessingPhase::ProcessingPhase ()
   : name("default name"), performance(-1.0), resolution(-1.0), memoryUsage(), internalMemoryUsageData(0)
   {
   }

// Forward declaration
// extern int RAMUST::getMem(int);

ProcessingPhase::ProcessingPhase ( const std::string & s, double p, ProcessingPhase *parent )
   : name(s), performance(p), resolution(-1.0), memoryUsage(), internalMemoryUsageData(0)
   {
#if 0
  // DQ (12/8/2006): Use Linux memory usage mechanism
     RAMUST* ram = new RAMUST();

  // Memory useage needs to be scaled else units of 10K are the smallest resolution.
     unsigned long id = getpid();
     unsigned long startMemoryUsage = ram->getMem(id);
     printf ("Memory usage for %ld (start): %ld 10K.\n",id,startMemoryUsage);
     delete ram;

     internalMemoryUsageData = startMemoryUsage;
#else
  // DQ (7/21/2010): Set in the destructor instead of the constructor.
  // internalMemoryUsageData = ROSE_MemoryUsage::getMemoryUsageKilobytes();
  // internalMemoryUsageData = memoryUsage.getMemoryUsageMegabytes();
#endif


     if (parent != NULL)
          parent->childList.push_back(this);
   }

ProcessingPhase::~ProcessingPhase()
   {
  // DQ (7/21/2010): This is too late of a stage to set this value!
  // internalMemoryUsageData = memoryUsage.getMemoryUsageMegabytes();
   }

double
ProcessingPhase::getCurrentDelta(const RoseTimeType& timer)
   {
#if 1
     return double(clock() - timer) / CLOCKS_PER_SEC;
#else
     double returnValue = double(clock() - timer) / CLOCKS_PER_SEC;
  // internalMemoryUsageData = memoryUsage.getMemoryUsageMegabytes();
     internalMemoryUsageData = get_memory_usage() - internalMemoryUsageData;
     return returnValue;
#endif
   }

#if 0
// DQ (7/21/2010): I don't think this is called anymore!
void
ProcessingPhase::stopTiming(const RoseTimeType& timer) {
     double performance = getCurrentDelta(timer);

#if 0
  // DQ (12/8/2006): Use Linux memory usage mechanism
     RAMUST* ram = new RAMUST();

  // Memory useage needs to be scaled else units of 10K are the smallest resolution.
     unsigned long id = getpid();
     unsigned long endMemoryUsage = ram->getMem(id);
     printf ("Memory usage for %ld (end): %ld 10K.\n",id,endMemoryUsage-internalMemoryUsageData);
     delete ram;

  // Save the difference in the memory usage
     internalMemoryUsageData = endMemoryUsage - internalMemoryUsageData;
#else
  // internalMemoryUsageData = ROSE_MemoryUsage::getMemoryUsageKilobytes() - internalMemoryUsageData;
     internalMemoryUsageData = get_memory_usage() - internalMemoryUsageData;
#endif

#if 0
     printf ("ProcessingPhase::stopTiming: name = %s endTimeSec = %ld endTimeNanoSec = %ld performance = %f \n",
          name.c_str(),endTimeSec,endTimeNanoSec,performance);
#endif
     set_performance(performance);

  // reset the resolution (since it seems that the base class virtual function was called)
     double resolution = TimingPerformance::performanceResolution();
  // printf ("resolution = %f \n",resolution);
     set_resolution(resolution);
   }
#endif

int
AstPerformance::getLock()
   {
     int fd;

  // printf ("Build the lock file \n");
  // generate a lock 
     if ( SgProject::get_verbose() >= 1 )
          printf ("Acquiring a lock: rose_performance_report_lockfile.lock \n");

  // DQ (8/24/2008): Setup counters to detect when file locks are in place (this was a problem this morning)
     unsigned long counter             = 0;
     const unsigned long userTolerance = 10;

     while ( (fd = open("rose_performance_report_lockfile.lock", O_WRONLY | O_CREAT | O_EXCL)) == -1 )
        {
       // Skip the message output if this is the first try!
          if ( counter > 0 )
               printf ("Waiting for lock! counter = %lu userTolerance = %lu \n",counter,userTolerance);

#ifdef _MSC_VER
          Sleep(1000);
#else
          sleep(1);
#endif
          counter++;

       // DQ (8/24/2008): If after waiting a short while and the lock is still there, then report the issue.
          if ( counter > userTolerance )
             {
               printf ("Waiting for file lock (run \"make clean\" to remove lock files, if problem persists)... \n");

            // Reset the counter to prevent it from over flowing on nightly tests, though that might take a long time :-).
               counter = 1;
             }
        }

     if (fd == -1)
        {
          perror("error in opening lock file: rose_performance_report_lockfile.lock");
       // exit(1);
        }

     return fd;
   }


void
AstPerformance::releaseLock (int fd )
   {
     close(fd);

     if ( SgProject::get_verbose() >= 1 )
          printf ("Removing the lock file \n");

     remove("rose_performance_report_lockfile.lock");
   }


void
ProcessingPhase::outputReport ( int n )
   {
  // indent child data
     for (int i=0; i < n; i++)
          printf (" ");

  // printf ("%s %f \n",name.c_str(),performance);
#if 0
     printf ("%s %f %ld \n",name.c_str(),performance,internalMemoryUsageData);
#else
     printf ("%s time = %4.3f (sec) memory usage %5.3f (megabytes) \n",name.c_str(),performance,internalMemoryUsageData);
#endif
  // printf ("name = %s performance = %f \n",name.c_str(),performance);

  // printf ("Children: childList = %zu \n",childList.size());
     std::vector<ProcessingPhase*>::iterator i = childList.begin();
     while (i != childList.end())
        {
          (*i)->outputReport(n+5);
          i++;
        }
   }

void AstPerformance::generateReport() {
  AstPerformance("", false).generateReportFromObject();
}

void
AstPerformance::generateReportFromObject() const
   {
  // output any performance data saved by different phases during the compilation

  // DQ (6/9/2010): Change the return type to size_t to support larger number of IR nodes
  // using values that overflow signed values of int. Note this is only an error for ROSE 
  // compiling ROSE.
  // Declaration of global functions (generated by ROSETTA)
     extern size_t numberOfNodes();

#if 0
/* Structure which says how much of each resource has been used.  */
struct rusage
  {
    /* Total amount of user time used.  */
    struct timeval ru_utime;
    /* Total amount of system time used.  */
    struct timeval ru_stime;
    /* Maximum resident set size (in kilobytes).  */
    long int ru_maxrss;
    /* Amount of sharing of text segment memory
       with other processes (kilobyte-seconds).  */
    long int ru_ixrss;
    /* Amount of data segment memory used (kilobyte-seconds).  */
    long int ru_idrss;
    /* Amount of stack memory used (kilobyte-seconds).  */
    long int ru_isrss;
    /* Number of soft page faults (i.e. those serviced by reclaiming
       a page from the list of pages awaiting reallocation.  */
    long int ru_minflt;
    /* Number of hard page faults (i.e. those that required I/O).  */
    long int ru_majflt;
    /* Number of times a process was swapped out of physical memory.  */
    long int ru_nswap;
    /* Number of input operations via the file system.  Note: This
       and `ru_oublock' do not include operations with the cache.  */
    long int ru_inblock;
    /* Number of output operations via the file system.  */
    long int ru_oublock;
    /* Number of IPC messages sent.  */
    long int ru_msgsnd;
    /* Number of IPC messages received.  */
    long int ru_msgrcv;
    /* Number of signals delivered.  */
    long int ru_nsignals;
    /* Number of voluntary context switches, i.e. because the process
       gave up the process before it had to (usually to wait for some
       resource to be available).  */
    long int ru_nvcsw;
    /* Number of involuntary context switches, i.e. a higher priority process
       became runnable or the current process used up its time slice.  */
    long int ru_nivcsw;
  };
#endif

     printf ("\n\n");
     std::vector<ProcessingPhase*>::iterator i = data.begin();
     if (i != data.end())
        {
          size_t numberOf_IR_Nodes     = numberOfNodes();
          size_t numberOfKiloBytesUsed = memoryUsage() / (1 << 10);

       // DQ (12/8/2006): Using new Linux memory usage.
       // DQ (9/6/2006): Computed using getrusage();
       // long int memoryComputedFromSystem = 0;
       // unsigned long memoryComputedFromSystem = (*i)->get_memory_usage();
          double memoryComputedFromSystem = (*i)->get_memory_usage();
#if 0
       // int getrusage(int who, struct rusage *usage);
       // Choice of who: RUSAGE_SELF, RUSAGE_CHILDREN, RUSAGE_BOTH
       // Build data structure to hold rusage data
          struct rusage memoryUsageData;
          int status = getrusage(RUSAGE_SELF, &memoryUsageData);
          ROSE_ASSERT(status == 0);

          memoryComputedFromSystem = memoryUsageData.ru_maxrss;

          printf ("memoryUsageData.ru_maxrss   = %ld \n",memoryUsageData.ru_maxrss);
          printf ("memoryUsageData.ru_ixrss    = %ld \n",memoryUsageData.ru_ixrss);
          printf ("memoryUsageData.ru_idrss    = %ld \n",memoryUsageData.ru_idrss);
          printf ("memoryUsageData.ru_minflt   = %ld \n",memoryUsageData.ru_minflt);
          printf ("memoryUsageData.ru_majflt   = %ld \n",memoryUsageData.ru_majflt);
          printf ("memoryUsageData.ru_nswap    = %ld \n",memoryUsageData.ru_nswap);
          printf ("memoryUsageData.ru_inblock  = %ld \n",memoryUsageData.ru_inblock);
          printf ("memoryUsageData.ru_oublock  = %ld \n",memoryUsageData.ru_oublock);
          printf ("memoryUsageData.ru_msgsnd   = %ld \n",memoryUsageData.ru_msgsnd);
          printf ("memoryUsageData.ru_msgrcv   = %ld \n",memoryUsageData.ru_msgrcv);
          printf ("memoryUsageData.ru_nsignals = %ld \n",memoryUsageData.ru_nsignals);
          printf ("memoryUsageData.ru_nvcsw    = %ld \n",memoryUsageData.ru_nvcsw);
          printf ("memoryUsageData.ru_nivcsw   = %ld \n",memoryUsageData.ru_nivcsw);
#endif

	  ROSE_MemoryUsage currentUsage;
	  if (currentUsage.informationValid()) {
	    printf ("General System Data: \n");
	    printf ("     timer resolution (sec)   = %f \n",(*i)->get_resolution());
	    printf ("     page size (bytes)        = %7d (megabytes) = %8.3f \n",currentUsage.getPageSizeBytes(),currentUsage.getPageSizeMegabytes());
	    printf ("     available memory (pages) = %7d (megabytes) = %8.3f \n",currentUsage.getAvailableMemoryPages(),currentUsage.getAvailableMemoryMegabytes());
	    printf ("     resident memory (pages)  = %7d (megabytes) = %8.3f \n",currentUsage.getNumberOfResidentPages(),currentUsage.getNumberOfResidentMegabytes());
	    printf ("     shared pages             = %7d (megabytes) = %8.3f \n",currentUsage.getNumberOfSharedPages(),currentUsage.getNumberOfSharedMegabytes());
	    printf ("     code size (pages)        = %7d (megabytes) = %8.3f \n",currentUsage.getNumberOfCodePages(),currentUsage.getNumberOfCodeMegabytes());
	    printf ("     stack size (pages)       = %7d (megabytes) = %8.3f \n",currentUsage.getNumberOfStackPages(),currentUsage.getNumberOfStackMegabytes());
	    printf ("     library size (pages)     = %7d (megabytes) = %8.3f \n",currentUsage.getNumberOfLibraryPages(),currentUsage.getNumberOfLibraryMegabytes());
	    printf ("     dirty pages              = %7d (megabytes) = %8.3f \n",currentUsage.getNumberOfDirtyPages(),currentUsage.getNumberOfDirtyMegabytes());
	    printf ("     executable code pages    = %7d (megabytes) = %8.3f \n",currentUsage.getNumberOfCodePlusLibraryPages(),currentUsage.getNumberOfCodePlusLibraryMegabytes());

       // printf ("Performance Report (resolution = %f, number of IR nodes = %d, memory used (calculated for AST) = %d Kilobytes, memory used (actual) = %ld Kilobytes ): \n",
       //      (*i)->get_resolution(),numberOf_IR_Nodes,numberOfKiloBytesUsed,memoryComputedFromSystem);
            printf ("Performance Report (timer resolution = %f, number of IR nodes = %zu, memory used (calculated from AST memory pool) = %zu Kilobytes, memory used (actual) = %8.3f Megabytes ): \n",
                 (*i)->get_resolution(),numberOf_IR_Nodes,numberOfKiloBytesUsed,memoryComputedFromSystem);
	  } else {
	    printf("Memory usage information from system is not available.\n");
	  }
        }
     
     while (i != data.end())
        {
          (*i)->outputReport(5);
          i++;
        }

    printf ("\n\n");

    printf ("Hidden list timing information: \n");

 // DQ (8/3/2007): report the elapsed time for the intersection function (called many times)
    reportAccumulatedTime("Intersection Time",Hidden_List_Computation::accumulatedIntersectionTime,Hidden_List_Computation::accumulatedIntersectionCalls);
    reportAccumulatedTime("Intersection Find Time",Hidden_List_Computation::accumulatedIntersectionFindTime,Hidden_List_Computation::accumulatedIntersectionFindCalls);
    reportAccumulatedTime("Intersection After Find Time",Hidden_List_Computation::accumulatedIntersectionAfterFindTime,Hidden_List_Computation::accumulatedIntersectionAfterFindCalls);
    reportAccumulatedTime("Intersection FillScopeStack case 1 Time",Hidden_List_Computation::accumulatedIntersectionFillScopeStack_1_Time,Hidden_List_Computation::accumulatedIntersectionFillScopeStack_1_Calls);
    reportAccumulatedTime("Intersection FillScopeStack case 2 Time",Hidden_List_Computation::accumulatedIntersectionFillScopeStack_2_Time,Hidden_List_Computation::accumulatedIntersectionFillScopeStack_2_Calls);

    reportAccumulatedTime("Scope 0 Intersection Time",Hidden_List_Computation::accumulatedIntersectionScopeTime[0],Hidden_List_Computation::accumulatedIntersectionScopeCalls[0]);
    reportAccumulatedTime("Scope 1 Intersection Time",Hidden_List_Computation::accumulatedIntersectionScopeTime[1],Hidden_List_Computation::accumulatedIntersectionScopeCalls[1]);
    reportAccumulatedTime("Scope 2 Intersection Time",Hidden_List_Computation::accumulatedIntersectionScopeTime[2],Hidden_List_Computation::accumulatedIntersectionScopeCalls[2]);
    reportAccumulatedTime("Scope 3 Intersection Time",Hidden_List_Computation::accumulatedIntersectionScopeTime[3],Hidden_List_Computation::accumulatedIntersectionScopeCalls[3]);
    reportAccumulatedTime("Scope 4 Intersection Time",Hidden_List_Computation::accumulatedIntersectionScopeTime[4],Hidden_List_Computation::accumulatedIntersectionScopeCalls[4]);
    reportAccumulatedTime("Scope 5 Intersection Time",Hidden_List_Computation::accumulatedIntersectionScopeTime[5],Hidden_List_Computation::accumulatedIntersectionScopeCalls[5]);
    reportAccumulatedTime("Collect Global scope and namespace data: EvaluateInheritedAttribute Time",Hidden_List_Computation::accumulatedEvaluateInheritedAttribute_1_Time,Hidden_List_Computation::accumulatedEvaluateInheritedAttribute_1_Calls);
    reportAccumulatedTime("Collect Global scope and namespace data: EvaluateInheritedAttribute (common case) Time",Hidden_List_Computation::accumulatedEvaluateInheritedAttributeCommonCaseTime,Hidden_List_Computation::accumulatedEvaluateInheritedAttributeCommonCaseCalls);

    reportAccumulatedTime("accumulatedEqstr_SgDeclarationStatement Time",Hidden_List_Computation::accumulatedEqstr_SgDeclarationStatementTime,Hidden_List_Computation::accumulatedEqstr_SgDeclarationStatementCalls);

    reportAccumulatedTime("Hidden List computation: EvaluateInheritedAttribute Time",Hidden_List_Computation::accumulatedEvaluateInheritedAttribute_2_Time,Hidden_List_Computation::accumulatedEvaluateInheritedAttribute_2_Calls);
    reportAccumulatedTime("Hidden List computation: EvaluateSynthesizedAttribute Time",Hidden_List_Computation::accumulatedEvaluateSynthesizedAttribute_2_Time,Hidden_List_Computation::accumulatedEvaluateSynthesizedAttribute_2_Calls);

    reportAccumulatedTime("Hidden List computation: EvaluateInheritedAttribute (case  1) Time",Hidden_List_Computation::accumulatedEvaluateInheritedAttributeCaseTime[ 1],Hidden_List_Computation::accumulatedEvaluateInheritedAttributeCaseCalls[ 1]);
    reportAccumulatedTime("Hidden List computation: EvaluateInheritedAttribute (case  2) Time",Hidden_List_Computation::accumulatedEvaluateInheritedAttributeCaseTime[ 2],Hidden_List_Computation::accumulatedEvaluateInheritedAttributeCaseCalls[ 2]);
    reportAccumulatedTime("Hidden List computation: EvaluateInheritedAttribute (case  3) Time",Hidden_List_Computation::accumulatedEvaluateInheritedAttributeCaseTime[ 3],Hidden_List_Computation::accumulatedEvaluateInheritedAttributeCaseCalls[ 3]);
    reportAccumulatedTime("Hidden List computation: EvaluateInheritedAttribute (case  4) Time",Hidden_List_Computation::accumulatedEvaluateInheritedAttributeCaseTime[ 4],Hidden_List_Computation::accumulatedEvaluateInheritedAttributeCaseCalls[ 4]);
    reportAccumulatedTime("Hidden List computation: EvaluateInheritedAttribute (case  5) Time",Hidden_List_Computation::accumulatedEvaluateInheritedAttributeCaseTime[ 5],Hidden_List_Computation::accumulatedEvaluateInheritedAttributeCaseCalls[ 5]);
    reportAccumulatedTime("Hidden List computation: EvaluateInheritedAttribute (case  6) Time",Hidden_List_Computation::accumulatedEvaluateInheritedAttributeCaseTime[ 6],Hidden_List_Computation::accumulatedEvaluateInheritedAttributeCaseCalls[ 6]);
    reportAccumulatedTime("Hidden List computation: EvaluateInheritedAttribute (case  7) Time",Hidden_List_Computation::accumulatedEvaluateInheritedAttributeCaseTime[ 7],Hidden_List_Computation::accumulatedEvaluateInheritedAttributeCaseCalls[ 7]);
    reportAccumulatedTime("Hidden List computation: EvaluateInheritedAttribute (case  8) Time",Hidden_List_Computation::accumulatedEvaluateInheritedAttributeCaseTime[ 8],Hidden_List_Computation::accumulatedEvaluateInheritedAttributeCaseCalls[ 8]);
    reportAccumulatedTime("Hidden List computation: EvaluateInheritedAttribute (case  9) Time",Hidden_List_Computation::accumulatedEvaluateInheritedAttributeCaseTime[ 9],Hidden_List_Computation::accumulatedEvaluateInheritedAttributeCaseCalls[ 9]);
    reportAccumulatedTime("Hidden List computation: EvaluateInheritedAttribute (case 10) Time",Hidden_List_Computation::accumulatedEvaluateInheritedAttributeCaseTime[10],Hidden_List_Computation::accumulatedEvaluateInheritedAttributeCaseCalls[10]);
    reportAccumulatedTime("Hidden List computation: EvaluateInheritedAttribute (case 11) Time",Hidden_List_Computation::accumulatedEvaluateInheritedAttributeCaseTime[11],Hidden_List_Computation::accumulatedEvaluateInheritedAttributeCaseCalls[11]);
    reportAccumulatedTime("Hidden List computation: EvaluateInheritedAttribute (case 12) Time",Hidden_List_Computation::accumulatedEvaluateInheritedAttributeCaseTime[12],Hidden_List_Computation::accumulatedEvaluateInheritedAttributeCaseCalls[12]);
    reportAccumulatedTime("Hidden List computation: EvaluateInheritedAttribute (case 13) Time",Hidden_List_Computation::accumulatedEvaluateInheritedAttributeCaseTime[13],Hidden_List_Computation::accumulatedEvaluateInheritedAttributeCaseCalls[13]);

    reportAccumulatedTime("Hidden List computation: CreateNewStackFrame Time",Hidden_List_Computation::accumulatedCreateNewStackFrameTime,Hidden_List_Computation::accumulatedCreateNewStackFrameCalls);
    reportAccumulatedTime("Hidden List computation: UpdateScopeWithClass Time",Hidden_List_Computation::accumulatedUpdateScopeWithClassTime,Hidden_List_Computation::accumulatedUpdateScopeWithClassCalls);

    printf ("\n\n");
   }

void
ProcessingPhase::outputReportToFile ( std::ofstream & datafile )
   {
  // For CSV formatted files we have to escape all ',' characters.
  // So we have to identify any fields that contain ',' and handle them as special cases.
     string csv_field = name;

  // DQ (8/30/2006): Just quote those csv fields that contain a ',' character.
     string commaSeparator      = ",";
     if (csv_field.find(commaSeparator) != string::npos)
        {
          csv_field = "\"" + csv_field + "\"";
        }

     datafile << ", " << csv_field << ", " << performance;

  // printf ("name = %s performance = %f \n",csv_field.c_str(),performance);

  // printf ("Children: childList = %zu \n",childList.size());
     std::vector<ProcessingPhase*>::iterator i = childList.begin();
     while (i != childList.end())
        {
          (*i)->outputReportToFile(datafile);
          i++;
        }
   }

void
AstPerformance::set_project(SgProject* projectParameter)
   {
     project = projectParameter;
     ROSE_ASSERT(project != NULL);
   }

void
AstPerformance::generateReportToFile( SgProject* project ) const
   {
  // output CVS file with performance information data saved by different phases during the compilation

  // DQ (6/9/2010): Change the return type to size_t to support larger number of IR nodes
  // using values that overflow signed values of int. Note this is only an error for ROSE 
  // compiling ROSE.
  // Declaration of global functions (generated by ROSETTA)
     extern size_t numberOfNodes();
     extern size_t memoryUsage();

     set_project(project);

#if 0
  // DQ (9/6/2006): This will reset the time; to a nearly zero value!
     std::vector<ProcessingPhase*>::iterator j = data.begin();
     while (j != data.end())
        {
       // (*j)->stopTiming(timer);
          j++;
        }
#endif

     ROSE_ASSERT(project != NULL);
     string output_filename = project->get_compilationPerformanceFile();

  // Ignore generation of a report if no source file was specified!
     if (project->numberOfFiles() == 0 || output_filename.empty() == true)
        {
          if ( SgProject::get_verbose() >= 2 )
             {
            // printf ("No source file specified for compilation, performance report output skipped  project->numberOfFiles() = %d \n",project->numberOfFiles());
               if (project->numberOfFiles() == 0)
                  {
                    printf ("Note: No source file specified for output of performance data (CVS file). \n");
                  }
                 else
                  {
                    printf ("Note: No Compilation Performance File specified for output of performance data (use -rose:compilationPerformanceFile <filename>) \n");
                  }
             }
          return;
        }

     string source_file     = project->get_file(0).get_sourceFileNameWithPath();

     ofstream datafile ( output_filename.c_str() , ios::out | ios::app );

     if ( datafile.good() == false )
        {
          printf ("File failed to open \n");
          ROSE_ASSERT(false);
        }

  // datafile << "This is a test!" << std::endl;

  // printf ("Get the lock ... \n");

  // generate a lock 
     int fd = getLock();
     ROSE_ASSERT(fd > 0);
  // printf ("Got the lock ... \n");

  // Put the data for each ProcessingPhase out to a CSV formatted file
  // output the data
     datafile << "filename," << source_file << ", number of AST nodes, " << numberOfNodes() << ", memory, " << memoryUsage() << " ";

  // printf ("Output the data to the file ... \n");
     std::vector<ProcessingPhase*>::iterator i = data.begin();
     while (i != data.end())
        {
          (*i)->outputReportToFile(datafile);
          i++;
        }

     datafile << endl;

  // printf ("Done: Output the data to the file ... (calling flush) \n");

     datafile.flush();

  // printf ("Done with file flush() ... \n");

  // release the lock
  // printf ("Releasing the file lock \n");
     releaseLock(fd);

  // printf ("File output complete \n");
     datafile.close();
   }

double
AstPerformance::performanceResolution()
   {
  // printf ("Inside of AstPerformance::performanceResolution() \n");
      return -1.0;  // default value
   }

TimingPerformance::TimingPerformance ( std::string s , bool outputReport )
// Save the label explaining what the performance number means
   : AstPerformance(s,outputReport)
   {
      timer = clock(); // Liao, 2/18/2009, fixing bug 2009. This has to be turned on 
                      //since timer is used as the start time for calculating performance 
   }

TimingPerformance::~TimingPerformance()
   {
  // DQ (9/1/2006): Refactor the code to stop the timing so that we can call it in the 
  // destructor and the report generation (both trigger the stopping of all timers).
     assert(localData != NULL);
     double p = ProcessingPhase::getCurrentDelta(timer);
     if (p<0.0) // Liao, 2/18/2009, avoid future bug 
     {
       cerr<<"Error: AstPerformance.C TimingPerformance::~TimingPerformance() set negative performance value!"<<endl;
       ROSE_ASSERT(false);
     }
     localData->set_performance(ProcessingPhase::getCurrentDelta(timer));
     localData->set_resolution(performanceResolution());
#if 0
  // reset the resolution (since it seems that the base class virtual function was called)
     double resolution = performanceResolution();
  // printf ("resolution = %f \n",resolution);
     localData->set_resolution(resolution);
#endif

  // DQ (7/21/2010): Set this here to record the useage of memory in the interval being evaluated.
  // internalMemoryUsageData = memoryUsage.getMemoryUsageMegabytes();
  // localData->internalMemoryUsageData = get_memory_usage() - localData->internalMemoryUsageData;
  // localData->set_memory_usage( (double)(get_memory_usage()));
  // localData->internalMemoryUsageData = memoryUsage.getMemoryUsageMegabytes();
     ROSE_MemoryUsage memoryUsage;
     localData->set_memory_usage( memoryUsage.getMemoryUsageMegabytes() );
   }

double
TimingPerformance::performanceResolution()
   {
  // printf ("Inside of TimingPerformance::performanceResolution() \n");
  // This may not be the correct resolution of the clock
     double resolution = 1.0 / (double) CLOCKS_PER_SEC;
     return resolution;
   }

void
AstPerformance::reportAccumulatedTime ( const string & s, const double & accumulatedTime, const double & numberFunctionCalls )
   {
     printf ("     Accumulated time for %s = %f number of calls = %ld \n",s.c_str(),accumulatedTime,(long)numberFunctionCalls);
   }

void
AstPerformance::startTimer ( RoseTimeType & time )
   {
     time = clock();
   }

void
AstPerformance::accumulateTime ( RoseTimeType & startTime, double & accumulatedTime, double & numberFunctionCalls )
   {
     accumulatedTime += ProcessingPhase::getCurrentDelta(startTime);
     numberFunctionCalls += 1.0;
   }

