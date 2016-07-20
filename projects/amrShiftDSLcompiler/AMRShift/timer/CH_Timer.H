#ifdef CH_LANG_CC
/*
*      _______              __
*     / ___/ /  ___  __ _  / /  ___
*    / /__/ _ \/ _ \/  V \/ _ \/ _ \
*    \___/_//_/\___/_/_/_/_.__/\___/
*    Please refer to Copyright.txt, in Chombo's root directory.
*/
#endif

#ifndef _CH_TIMER_H_
#define _CH_TIMER_H_

#include <cstdlib>
#include <cstdio>
#include <vector>
#include <iostream>
#include "ClockTicks.H"
#include <sys/time.h>
#ifdef CH_MPI
#include "mpi.h"
#endif


  /** TraceTimer class is a self-tracing code instrumentation system

     TraceTimer class is a self-tracing code instrumentation system
     for Chombo (or any other package really).  The user interface is specified
     by a small set of macros.  The usage model is that you just leave these
     timers in the code, for good.  Initially, your application will have 'main'
     and a few hewavy functions instrumented, and the lower level Chombo library
     instrumentation.  As your tool or application matures, it
     will garner a larger set of instrumentation giving clear views of your
     code performance.  After a routine has been
     cleverly and lovingly optimized, you leave in the timers, to spot when
     some later bug fix or *improvement* undoes your previous labors.

     \note
     You should never need to use or interact with the the classes TraceTimer or
     AutoStart.  Use the macros. They call the right functions and classes for you.

     The first macro is what people will use the most:
\code
     CH_TIME("label");
\endcode

     This is the simplest interface for timers.  you place this macro call in a function
     you wish to be timed.  It handles making the timer, calling 'start' when you
     enter the function, and calling 'stop' when you leave the function.  A good
     idea is to use a 'label' specific enough to be unambiguous without being
     overwhelming.  for instance:

\code
    void AMRLevelPolytropicGas::define(AMRLevel*            a_coarserLevelPtr,
                                       const ProblemDomain& a_problemDomain,
                                       int                  a_level,
                                       int                  a_refRatio)
    {
       CH_TIME("AMRLevelPolytropicGas::define");
     .
     .
    }
\endcode

     In this case, we have a class with many constructors and define functions that
     all funnel into a single general function.  We can just call this 'define' and
     not worry about naming/instrumenting all the different overloaded instances. If
     you slip up and use the same label twice, that is not a real problem, the two
     locations will be timed and tracked properly (even if one is a sibling or parent
     of the other). The only place it will make things a little harder is in the output
     where you might have the same name show up and look confusing.
    <br><br>
     In serial, you will see a file called <em>time.table</em> (in parallel, you will get a
     <em>time.table.n</em> (where n is the rank number) files).  If
     you want fewer files, you can do 
     setenv CH_OUTPUT_INTERVAL nproc 
     and it will only output every nproc processors time.table.n files
     (where n%nproc == 0).     I won't go into this file
     format.  It is kind of gprof-ish, with what I consider improvements.  The real
     benefit here is profiling that understands our Chombo context, a smaller information
     set to observe, and the fact that, so far in my testing, the timers have negligible
     impact on the run time or memory use of the code.
     <br><br>
     By default, Chombo compiles in the instructions for the timers wherever the macros
     appear.  If the compiler macro <b>CH_NTIMER</b> is defined, then all the CH_TIME* macros
     evaluate to empty expressions at compile time.

     \par So, you put some CH_TIME calls in your code and ran it, and nothing happened:
     Chombo looks for the environment variable <b>CH_TIMER</b>. If it is set to anything (even
     if it is set to 'false' or 'no' or whatever) then the timers will be active and
     reporting will happen.  If this environment variable is not set, then all the timers
     check a bool and return after doing nothing.
     \par
     One point of interest with using the environment variable: In parallel jobs using
     mpich, only processor 0 inherits the environment variables from the shell where
     you invoke 'mpirun', the rest read your .cshrc (.bashrc, etc.) file to get their
     environment.  To time all your processes, you need to make sure the <b>CH_TIMER</b>
     environment variable gets to all your processes.



     \par Auto hierarchy:
     The timers automatically figure out their parent/child relationships.  They
     also can be placed in template code.  This has some consequences.  First,
     if you have a low level function instrumented that has no timers near it in
     the code call stack, you will see it show up as a child of a high level timer.
     the root timer "main" will catch all orphaned timers.  So, even though you
     might make no call to, say, 'exchange' in your 'main' function, you might
     very well call a function, that calls a function, that calls 'exchange'. Since
     no code in between was instrumented, this exchange is accounted for at 'main'.
     This might look strange, but it should prove very powerful. An expensive orphan
     is exactly where you should consider some more timers, or reconsidering code
     design.

     \par
      For performance reasons, child timers have only one parent.  As a consequence
      each CH_TIME("label") label can show up at multiple places in your output. Each
      instance has it's own timer.  So, each path through the call graph that arrives
      at a low-level function has a unique lineage, with it's own counter and time.
      Thus, I can instrument LevelData::copyTo once, but copyTo can appear in many
      places in the time.table file.


     The next level up in complexity is the set of *four* macros for when you want
     sub-function resolution in your timers. For instance, in a really huge function
     that you have not figured out how to re-factor, or built with lots of bad cut n paste
     code 're-use'.
\code
     CH_TIMERS("parent");
     CH_TIMER("child1", t1);
     CH_TIMER("child2", t2);
     CH_START(t1);
     CH_STOP(t1);
     CH_START(t2);
     CH_STOP(t2);
     CH_START(t1);
     CH_STOP(t1);
\endcode

     CH_TIMERS has the same semantic as CH_TIME, except that you can declare an
     arbitrary number of children after it in the same function scope.  The
     children here do not autostart and autostop, you have to tell them where to
     start and stop timing.  The children can themselves be parents for timers
     in called functions, of course. The children obey a set of mutual exclusions. The
     following generate run time errors:
     - double start called
     - double stop called
     - start called when another child is also started
     - you leave the function with a child not stopped

     the following will generate compile time errors:
     - more than one CH_TIME macro in a function
     - invoking CH_TIMER("child", t) without having first invoked CH_TIMERS
     - re-using the timer handle ie. CH_TIMER("bobby", t1); CH_TIMER("sally", t1)
     - mixing CH_TIME macro with CH_TIMER
     - mixing CH_TIME macro with CH_TIMERS

     You do not have to put any calls in your main routine to activate the clocks
     or generate a report at completion, this is handled with static iniitalization
     and an atexit function.
     <br><br>
     There is a larger argument of manual instrumentation being counter to good development.
     Profiling the code is supposed to tell you where to expend your optimization effort.
     Manual instrumentation opens the door to people wasting time *assuming* what parts of the
     code are going to take up lots of time and instrumenting them, before seeing any real
     performance data.  Good judgement is needed.  We have a body of knowledge about Chombo
     that will inform us about a good minimal first set of functions to instrument.
  */
  class TraceTimer
  {
  public:
    virtual ~TraceTimer();
    void start(char* mutex);
    unsigned long long int stop(char* mutex);
    static void report(bool a_closeAfter=false);
    static void reset();


    void leafStart();
    void leafStop();

    unsigned long long int time() const
    {
      return m_accumulated_WCtime;
    }

    int rank() const
    {
      return m_rank;
    }
    long long int count() const
    {
      return m_count;
    }

    void prune();
    bool isPruned() const {return m_pruned;}

    static int initializer(); // don't use
    static TraceTimer* getTimer(const char* name); // don't use
    const std::vector<TraceTimer*>& children() const ;//don't use.

    static void PruneTimersParentChildPercent(double percent);
    static void sampleMemUsage() ;

    static const char* currentTimer()
    {
      return s_currentTimer[0]->m_name;
    }

  private:
    TraceTimer(const char* a_name, TraceTimer* parent, int thread_id);
    static std::vector<TraceTimer*> s_roots;
    static std::vector<TraceTimer*> s_currentTimer;
    static bool s_traceMemory;
    static long long int s_peak;
    static TraceTimer*   s_peakTimer;

    bool               m_pruned;
    TraceTimer*        m_parent;
    std::vector<TraceTimer*> m_children;
    const char*        m_name;
    long long int      m_count;
    unsigned long long int      m_accumulated_WCtime;
    unsigned long long int      m_last_WCtime_stamp;
    mutable int        m_rank;
    int                m_thread_id;
    long long int      m_memory;
    long long int      m_last_Memory_Stamp;
    long long int      m_peak;

    void reportTree(FILE* out, const TraceTimer& node, int depth);
    const TraceTimer* activeChild() const;

    void macroTest();
    void macroTest2();

    void currentize() const ;
    int  computeRank() const;
    static void reportFullTree(FILE* out, const TraceTimer& timer,
                               unsigned long long int totalTime, int depth);
    static void reportOneTree(FILE* out, const TraceTimer& timer);
    static void reportMemoryOneTree(FILE* out, const TraceTimer& timer);
    static void subReport(FILE* out, const char* header, unsigned long long int totalTime);
    static void reset(TraceTimer& timer);
    static void PruneTimersParentChildPercent(double threshold, TraceTimer* parent);

  };


  class AutoStartLeaf
  {
  public:
     AutoStartLeaf(TraceTimer* a_timer):m_timer(a_timer) {a_timer->leafStart();}
    ~AutoStartLeaf(){m_timer->leafStop();}
    bool active();
  private:
    TraceTimer* m_timer;
  };

  class AutoStart
  {
  public:
    AutoStart(TraceTimer* a_timer, char* mutex, char* childMutex)
      :m_mutex(mutex), m_childMutex(childMutex),m_timer(a_timer)
    {a_timer->start(mutex);}
    AutoStart(TraceTimer* a_timer, char* mutex)
      :m_mutex(mutex), m_childMutex(&AutoStart::ok),m_timer(a_timer)
    {a_timer->start(mutex);}
    inline ~AutoStart();
    bool active();
  private:
    char* m_mutex;
    char* m_childMutex;
    TraceTimer* m_timer;
    static char ok;

  };

  inline
  AutoStart::~AutoStart()
  {
#  ifndef NDEBUG
    if(*m_childMutex == 1)
      {
	std::cerr<<" stop called in timer unexpectedly";
	abort();
      }
#  endif
    m_timer->stop(m_mutex);
  }

  inline double TimerGetTimeStampWC(){

    // #ifdef CH_MPI
    //     return( MPI_Wtime() );
    // #endif
#ifndef CH_NTIMER
    struct timeval tv;   //  Values from call to gettimeofday
    struct timezone tz;
    gettimeofday(&tv, &tz);
    return((double)tv.tv_sec + 0.000001 * (double)tv.tv_usec);
#else
    return 0;
#endif
  }

inline void TraceTimer::leafStart()
{
  if(m_pruned) return;
  ++m_count;
  m_last_WCtime_stamp = ch_ticks();
}

inline void TraceTimer::leafStop()
{
  if(m_pruned) return;
  m_accumulated_WCtime +=  ch_ticks() - m_last_WCtime_stamp;
  m_last_WCtime_stamp=0;
}

#ifdef CH_NTIMER

#define CH_TIMER(name, tpointer)   (void)0
#define CH_TIME(name)    (void)0
#define CH_TIMELEAF(name)  (void)0
#define CH_TIMERS(name)    (void)0
#define CH_START(tpointer)  (void)0
#define CH_STOP(tpointer)  (void)0
#define CH_STOPV(tpointer, val) (void)0
#define CH_TIMER_REPORT()  (void)0
#define CH_TIMER_RESET()   (void)0
#define CH_TIMER_PRUNE(threshold)  (void)0

#else // CH_NTIMER


#define CH_TIMER(name, tpointer)                                        \
  const char* TimerTag_##tpointer = name ;                             \
  TraceTimer* tpointer = TraceTimer::getTimer(TimerTag_##tpointer)

#define CH_TIME(name)                                                   \
  const char* TimerTagA = name ;                                       \
  char CH_TimermutexA = 0;                                              \
  TraceTimer* ch_tpointer = TraceTimer::getTimer(TimerTagA); \
  AutoStart autostart(ch_tpointer, &CH_TimermutexA)

#define CH_TIMELEAF(name)                                                   \
  const char* TimerTagA = name ;                                       \
  TraceTimer* ch_tpointer = TraceTimer::getTimer(TimerTagA); \
  AutoStartLeaf autostart(ch_tpointer)

#define CH_TIMERS(name)                                                   \
  const char* TimerTagA = name ;                                       \
  char CH_TimermutexA = 0;                                              \
  char CH_Timermutex = 0;                                               \
  TraceTimer* ch_tpointer = TraceTimer::getTimer(TimerTagA); \
  AutoStart autostart(ch_tpointer, &CH_TimermutexA, &CH_Timermutex)


#define CH_START(tpointer) tpointer->start(&CH_Timermutex)

#define CH_STOP(tpointer) tpointer->stop(&CH_Timermutex)
#define CH_STOPV(tpointer, val ) val = tpointer->stop(&CH_Timermutex)

#define CH_TIMER_REPORT() TraceTimer::report()

#define CH_TIMER_RESET() TraceTimer::reset()

#define CH_TIMER_PRUNE(threshold) TraceTimer::PruneTimersParentChildPercent(threshold)

#endif  // CH_NTIMER



#endif // CH_TIMER_H
