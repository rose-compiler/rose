// Author: Dan Quinlan
// $Id: AstPerformance.h,v 1.5 2008/01/25 19:53:30 dquinlan Exp $

#ifndef AST_PERFORMANCE_H
#define AST_PERFORMANCE_H

// #include <rose.h>

#include <string>
#include <vector>
#include <list>

#ifdef _MSC_VER
#include <time.h>
#endif

#include <assert.h>

/*! \brief This is a mechanism for reporting the performance of processing of the AST, subtrees, 
           and IR nodes.  

    The use of these performance monitors is designed to be simple to use, support
    hierarchies of timers, and to generate simple reports.  Tests of the performance 
    monitors are in the ROSE/tests/roseTests/astPerformanceTests directory.

    Possible uses:
    -#) timing performance,
    -#) memory performance,
    -#) ...

 */

// Future Design:
//   1) Different levels of output
//   2) External file output (for more complete statistics)
//   3) A mechanism to report on hotspots, performance data, etc. ???

// namespace ROSE_Performance {
// This namespace represents the support for performance 
// monitoring of the difference phases of processing 
// within ROSE.

// using namespace std;

typedef clock_t RoseTimeType;

class ROSE_MemoryUsage
   {
 //! Function that I got from Bill Henshaw (originally from PetSc), for computing current memory in use.

  // DQ (12/11/2006): Added simpler function than ramust mechanism, used for computing the memory in use.
  // long int getCurrentMemoryUsage(); // reported in Kilobytes.

  // JJW 1-25-2008: Added struct to allow only a single access to
  // /proc/self/statm, and to replace the STATM_CODE macro
     int memory_pages;
     int resident_pages;
     int shared_pages;
     int code_pages;
     int stack_pages;
     int library_pages;
     int dirty_pages;
     bool valid;
   public:
     ROSE_MemoryUsage(): memory_pages(0), resident_pages(0), shared_pages(0),
                         code_pages(0), stack_pages(0), library_pages(0),
                         dirty_pages(0) {
       valid = getStatmInfo();
     }

     bool informationValid() const {return valid;}

   private:
 //! Get memory usage information from /proc/statm, returning true (and filling
 //! in the parameter structure) if the information is available and false if it
 //! is not.
     bool getStatmInfo();

  // DQ (12/12/2006): Supporting functions (available from reading /proc/<pid>/statm file).
   public:
     int getPageSizeBytes() const;
     int getAvailableMemoryPages() const;
     int getNumberOfResidentPages() const;
     int getNumberOfSharedPages() const;
     int getNumberOfCodePages() const;
     int getNumberOfStackPages() const;
     int getNumberOfLibraryPages() const;
     int getNumberOfDirtyPages() const;

     int getNumberOfCodePlusLibraryPages() const;
     long getMemoryUsageKilobytes() const;

     double getPageSizeMegabytes() const;
     double getAvailableMemoryMegabytes() const;
     double getNumberOfResidentMegabytes() const;
     double getNumberOfSharedMegabytes() const;
     double getNumberOfCodeMegabytes() const;
     double getNumberOfStackMegabytes() const;
     double getNumberOfLibraryMegabytes() const;
     double getNumberOfDirtyMegabytes() const;

     double getNumberOfCodePlusLibraryMegabytes() const;
     double getMemoryUsageMegabytes() const;
   };


class ProcessingPhase
   {
  // This class stores a string and a final number to record the performance.

     protected:
      //! title of performance measurement (e.g. "total time", "memory use", etc.)
          std::string name;
          double performance;
          double resolution;
      //! Memory usage information -- the constructor of this member obtains
      //! the information if it can
          ROSE_MemoryUsage memoryUsage;

#if 0
       // DQ (12/8/2006): Mechanism based on RAMUST (which is problematic)
          unsigned long internalMemoryUsageData;
#else
       // DQ (12/11/2006): Mechanism based on PetSc implementation (used in Overture).
          double internalMemoryUsageData;
#endif
       // Permit a hierarchy of performance monitors
          std::vector<ProcessingPhase*> childList;

     public:
          ProcessingPhase ();
          ProcessingPhase ( const std::string & s, double p, ProcessingPhase *parent );

       // DQ (7/21/2010): We need a destructor so that we can set the internalMemoryUsageData 
       // in the destructor instead of the constructor.  This way the memory usage is for the 
       // operation that is being evaluated instead of the memory usage just before the 
       // operation being evaluated.
          virtual ~ProcessingPhase();

          void outputReport ( int n );
          void outputReportToFile ( std::ofstream & datafile );
          void outputReportHeaderToFile ( std::ofstream & datafile );

          void stopTiming(const RoseTimeType& timer);
          static double getCurrentDelta(const RoseTimeType& timer);

       // DQ (9/1/2006): These are defined in the class because 
       // timer functions should have the lowest possible overhead.
          std::string get_name() const { return name; }
          void   set_name ( std::string & s) { name = s; }
          double get_performance() const { return performance; }
          void   set_performance (const double & p) { performance = p; }
          double get_resolution() const { return resolution; }
          void   set_resolution (const double & r) { resolution = r; }
#if 0
          unsigned long get_memory_usage() const { return internalMemoryUsageData; }
          void   set_memory_usage (const unsigned long & m) { internalMemoryUsageData = m; }
#else
          double get_memory_usage() const { return internalMemoryUsageData; }
          void   set_memory_usage (const double & m) { internalMemoryUsageData = m; }
#endif
   };

// Forward reference required from "void AstPerformance::generateReportToFile(SgProject*);"
class SgProject;

class AstPerformance
   {
     public:
       // DQ (9/1/2006): Note that we don't have the project yet when this is called within "main()" to summarize the total time.
          AstPerformance ( std::string s , bool outputReport = false );
          virtual ~AstPerformance();

       // This is the evolving data (built locally so that parents in the hierarchy can refer to it)
          ProcessingPhase* localData;

       // DQ (9/1/2006): Moved to the base class.
       // Use the Linux timer to provide nanosecond resolution
       // JJW (5/21/2008): Changed back to clock(3) for portability

          std::string label;
          static SgProject* project;

          static void set_project(SgProject* projectParameter);

          bool outputReportInDestructor;

          void generateReportFromObject() const;
          void generateReportToFile( SgProject* project ) const;
          static void generateReport();

       // virtual double performanceResolution();
          static double performanceResolution();

       // Support for file locking (so that the performance file is not 
       // corrupted by parallel writes as used in parallel compiles with 
       // the "-j#" make option).
          static int getLock();
          static void releaseLock (int fd );

       // Timer function support
          static void reportAccumulatedTime ( const std::string & s, const double & accumulatedTime, const double & numberFunctionCalls );
   
          static void startTimer ( RoseTimeType & time );
          static void accumulateTime ( RoseTimeType & startTime, double & accumulatedTime, double & numberFunctionCalls );

     protected:
       // Storage of all performance information about 
       // processing phases saved here for later processing.
          static std::vector<ProcessingPhase*> data;

       // This allows any existing performance monitor to become 
       // the parent of any child performance monitor
          static std::list<AstPerformance*> performanceStack;
   };


class TimingPerformance : public AstPerformance
   {
          RoseTimeType timer;

  // Used for timing compilation within ROSE
     public:
          TimingPerformance ( std::string s , bool outputReport = false );
          virtual ~TimingPerformance();

       // virtual double performanceResolution();
          static double performanceResolution();

	  typedef RoseTimeType time_type; // For compatibility
   };

// comment out use of namespace
// }

#endif

