#ifdef CH_LANG_CC
/*
*      _______              __
*     / ___/ /  ___  __ _  / /  ___
*    / /__/ _ \/ _ \/  V \/ _ \/ _ \
*    \___/_//_/\___/_/_/_/_.__/\___/
*    Please refer to Copyright.txt, in Chombo's root directory.
*/
#endif

#ifndef CH_NTIMER

#include "CH_Timer.H"

#include <iostream>
//#include "memtrack.H"
//#include "memusage.H"
#include <fstream>
#include <list>
#include <vector>
#include <cstdio>
#include <cstring>
#ifndef CH_DISABLE_SIGNALS
#include <unistd.h>
#include <csignal>
#endif

using namespace std;



//===================================================================================
//===================================================================================
//========  New timer code ==========================================================
//===================================================================================
//===================================================================================

struct elem
{
  elem();
  const TraceTimer* val;
  unsigned long long int      time;
  elem(const TraceTimer* p, unsigned long long int t):val(p), time(t){;}
  bool operator < (const elem& rhs) const
  {
    if(val->isPruned()) return false;
    return time > rhs.time;
  }
  static void buildList(std::list<elem>& tlist, const TraceTimer& timer);
};

std::list<elem> tracerlist(false);
std::vector<TraceTimer*> TraceTimer::s_roots;
std::vector<TraceTimer*> TraceTimer::s_currentTimer;
long long int TraceTimer::s_peak = 0;
TraceTimer*  TraceTimer::s_peakTimer = NULL;
bool TraceTimer::s_traceMemory = false;
static int s_depth = TraceTimer::initializer();

double zeroTime = 0;
unsigned long long int zeroTicks = 0;
double secondspertick = 0;

elem::elem()
{
  val=NULL;
  time=0;
}

void elem::buildList(std::list<elem>& tlist, const TraceTimer& timer)
{
  tlist.push_back(elem(&timer, timer.time()));
  const std::vector<TraceTimer*>& children = timer.children();
  for (unsigned int i=0; i<children.size(); i++)
    {
      buildList(tlist, *(children[i]));
    }
}

FILE* sampleFile;
bool sampleFileOpen = false;
int sampleFrequency = 0;


void sampleMem(int a_sig)
{
  TraceTimer::sampleMemUsage();
}

const char* currentTimer()
{
  return TraceTimer::currentTimer();
}

void TraceTimer::sampleMemUsage()
{
//   if(sampleFileOpen)
//     {
//       double m = get_memory_usage_from_OS();
//       const TraceTimer* current =  s_currentTimer[0];

//       fprintf(sampleFile, "%6.1f   ", m);
//       const TraceTimer* timer = s_roots[0];
//       while(timer != current)
//         {
//           fprintf(sampleFile, "%s->", timer->m_name);
//           timer = timer->activeChild();
//           if(timer == NULL) return;
//         }
//       fprintf(sampleFile, "%s\n",timer->m_name);

//     }
//   else
//     {
// #ifndef CH_MPI

//       sampleFile = fopen("memory.samples", "w");
//       sampleFileOpen = true;
//       fprintf(sampleFile, "@ title  \"sample frequency: %d usec\"\n", sampleFrequency);
// #else
//       int flag_i, flag_f;
//       MPI_Initialized(&flag_i);
//       MPI_Finalized(&flag_f);
//       if(flag_i)
//         {
//           char b[1024];
// 	  int outInterv = 1;
// 	  char* charInterv = getenv("CH_OUTPUT_INTERVAL");
// 	  if(charInterv != NULL)
// 	    {
// 	      outInterv =  atoi(charInterv);
// 	      // If zero specified, change it to numProc() which should give time.table.0 only
// 	      if (outInterv == 0) outInterv=numProc();
// 	    }
// 	  int thisProc = procID();
// 	  if((thisProc % outInterv) != 0)
// 	    {
// 	      sprintf(b,"/dev/null");
// 	    }
// 	  else
// 	    {
// 	      sprintf(b, "memory.samples.%d",procID());
// 	    }
	  
//           sampleFile = fopen(b, "w");
//           fprintf(sampleFile, "@ title \"sample frequency: %d usec\"\n", sampleFrequency);
//           fprintf(sampleFile, "@ s%d legend \"%d\"\n",procID(), procID());
//           fprintf(sampleFile, "@ target G0.S%d\n", procID());
//           sampleFileOpen = true;
//         }
// #endif
//     }
}


void writeOnExit()
{
  TraceTimer::report(true);
}

int TraceTimer::initializer()
{
  static bool initialized = false;
  if(initialized) return -11;

#ifndef CH_NTIMER
  const char* rootName = "root";
  TraceTimer* rootTimer = new TraceTimer(rootName, NULL, 0);
  rootTimer->m_thread_id = 0;
  char mutex = 0;
  s_roots.resize(1);
  s_roots[0] = rootTimer;
  s_currentTimer.resize(1);
  s_currentTimer[0]=rootTimer;

  char* timerEnv = getenv("CH_TIMER");
  if(timerEnv == NULL)
    {
      rootTimer->m_pruned = true;
    }
  else
    {
      s_traceMemory = false;
      if(strcmp(timerEnv, "MEMORY")==0)
        {
#ifdef  CH_USE_MEMORY_TRACKING
          s_traceMemory = true;
#endif

        }
      if(strncmp(timerEnv, "SAMPLE=",7)==0)
        {
          sampleFrequency = atoi(timerEnv+7);
#ifdef CH_DISABLE_SIGNALS
          std::cout<<"You requested profile "<<timerEnv
                   <<"  but CH_DISABLE_SIGNALS is turned on, no sampling possible\n";
#else
          signal(SIGALRM, sampleMem);
          ualarm( sampleFrequency, sampleFrequency );
#endif


        }
    }
  rootTimer->start(&mutex);
  zeroTime = TimerGetTimeStampWC();
  zeroTicks = ch_ticks();
  //  OK, I think I have the atexit vs. static objects bug under AIX worked out. we'll see.
  //#ifndef CH_AIX
  // petermc, 21 April 2006:
  // put "#ifndef CH_AIX" around this because on seaborg,
  // the presence of this line causes a segfault at the termination
  // of the program.
  //   OK, last time around the maypole.  It just seems that atexit and MPI_Finalize are
  //   not going to be cooperative.  bvs
#ifndef CH_MPI
  if(timerEnv != NULL) atexit(writeOnExit);
#endif

  //#endif
#endif // CH_NTIMER
  initialized = true;
  return 0;
}


void normalizeMemory(long long int a_m, int& a_memory, char* units)
{
  int kilobytes = a_m/1024;
  if(kilobytes > 5 || kilobytes < -5)
    {
      int megabytes = a_m/(1024*1024);
      if(megabytes > 5 || megabytes < -5)
        {
          strcpy(units, "M");
          a_memory = megabytes;
        }
      else
        {
          strcpy(units, "k");
          a_memory = kilobytes;
        }
    }
  else
    {
      strcpy(units, " ");
      a_memory = a_m;
    }
}

void TraceTimer::currentize() const
{
  if(m_pruned) return;

  if(m_last_WCtime_stamp != 0){
    for(unsigned int i=0; i<m_children.size(); i++)
      {
        m_children[i]->currentize();
      }
    unsigned long long int current = ch_ticks();
    (unsigned long long int&)m_accumulated_WCtime += current - m_last_WCtime_stamp;
    (unsigned long long int&)m_last_WCtime_stamp = current;

#ifdef CH_USE_MEMORY_TRACKING
    if(s_traceMemory)
      {
        long long int current, peak, d;
        overallMemoryUsage(current, peak);
        if(peak > s_peak){
          s_peak = peak;
          s_peakTimer = (TraceTimer*)this;
        }
        d = current - m_last_Memory_Stamp;
        (long long int&)m_memory += d;
        (long long int&)m_last_Memory_Stamp = 0;
      }
#endif

  }
}




int TraceTimer::computeRank() const
{
  tracerlist.clear();
  elem::buildList(tracerlist, *this);
  tracerlist.sort();
  int r=0;
  std::list<elem>::iterator it;
  for(it=tracerlist.begin(); it!=tracerlist.end(); ++it)
    {
      const elem& e = *it;
      //printf("%s %e %d\n",e.val->m_name, e.time, r);
      e.val->m_rank = r;
      ++r;
    }
  return r;
}

const TraceTimer* TraceTimer::activeChild() const
{
  TraceTimer* child;
  for(unsigned int i=0; i<m_children.size(); i++)
    {
      child = m_children[i];
      if(child->m_last_WCtime_stamp != 0) return child;
    }
  return NULL;

}

const std::vector<TraceTimer*>& TraceTimer::children() const
{
  return m_children;
}

void TraceTimer::report(bool a_closeAfter)
{

#ifndef CH_NTIMER


  char* timerEnv = getenv("CH_TIMER");
  if(timerEnv == NULL)
    {
      // pout()<<"CH_TIMER environment variable not set. Timers inactive. Not writing time.table \n";
      return;
    }
  if(strcmp(timerEnv, "MEMORY")==0)
    {
#ifndef  CH_USE_MEMORY_TRACKING
      std::cerr<<"CH_TIMER=MEMORY, but code compiled with MEMORY TRACKING turned off, no memory report\n";
      abort();
#endif
    }
  const TraceTimer& root = *(s_roots[0]); // in MThread code, loop over roots
  root.currentize();
  int numCounters = root.computeRank();

  double elapsedTime = TimerGetTimeStampWC() - zeroTime;
  unsigned long long int elapsedTicks = ch_ticks() - zeroTicks;
  secondspertick = elapsedTime/(double)elapsedTicks;

  int mpirank = 0;
#ifdef CH_MPI
  int proc = getpid();
  int finalized;
  MPI_Finalized(&finalized);
  if(finalized)
    mpirank = GetRank(proc);
  else
    mpirank = procID();
#endif

  if(mpirank >= 0)
    {
      char buf[1024];
#ifdef CH_MPI
      int outInterv = 1;
      char* charInterv = getenv("CH_OUTPUT_INTERVAL");
      if(charInterv != NULL)
        {
          outInterv =  atoi(charInterv);
          // If zero specified, change it to numProc() which should give time.table.0 only
          if (outInterv == 0) outInterv=numProc();
        }

      int thisProc = procID();
      if((thisProc % outInterv) != 0)
        {
          sprintf(buf,"/dev/null");
        }
      else
        {
          sprintf(buf,"time.table.%d",mpirank);
        }
#else
      sprintf(buf,"time.table");
#endif
      static FILE* out = fopen(buf, "w");
      static int reportCount = 0;
      fprintf(out, "-----------\nTimer report %d (%d timers)\n--------------\n",
              reportCount, numCounters);
      reportCount++;
      reportFullTree(out, root, root.m_accumulated_WCtime, 0); //uses recursion
      std::list<elem>::iterator it;
      for(it=tracerlist.begin(); it!=tracerlist.end(); ++it)
        reportOneTree(out, *((*it).val));
      subReport(out, "FORT_", root.m_accumulated_WCtime );
      subReport(out, "MPI_", root.m_accumulated_WCtime );
      fflush(out);
      if(a_closeAfter) fclose(out);
    }

  if(s_traceMemory && mpirank >= 0)
    {
      char buf[1024];
#ifdef CH_MPI
      sprintf(buf,"memory.table.%d",mpirank);
#else
      sprintf(buf,"memory.table");
#endif
      static FILE* out = fopen(buf, "w");
      static int reportCount = 0;
      fprintf(out, "-----------\nMemory report %d (%d timers)\n--------------\n",
              reportCount, numCounters);
      int peak;
      char units[2];
      normalizeMemory(s_peak, peak, units);
      const TraceTimer& t =*s_peakTimer;
      fprintf(out, "Peak Memory: %s [%d] %d%s\n", t.m_name, t.m_rank, peak, units);
      fprintf(out,"    peak     delta  \n");
      reportCount++;
      std::list<elem>::iterator it;
      for(it=tracerlist.begin(); it!=tracerlist.end(); ++it)
        reportMemoryOneTree(out, *((*it).val));
      fflush(out);
      if(a_closeAfter) fclose(out);
    }
#endif
}

void TraceTimer::reset()
{
  char* timerEnv = getenv("CH_TIMER");
  if(timerEnv == NULL)
    {
      // pout()<<"CH_TIMER environment variable not set. Timers inactive. Not writing time.table \n";
      return;
    }
  TraceTimer& root = *(s_roots[0]);
  root.currentize();
  reset(root);
}

void TraceTimer::reset(TraceTimer& node)
{
  node.m_count = 0;
  node.m_accumulated_WCtime = 0;
  node.m_memory = 0;
  for(unsigned int i=0; i<node.m_children.size(); i++)
    {
      reset(*(node.m_children[i]));
    }
}


void sorterHelper(const std::vector<TraceTimer*>& children, std::vector<int>& order)
{
  int n = children.size();
  order.resize(n);
  for(int i=0; i<n; ++i) order[i]=i;
  bool swaps = true;
  while(swaps)
    {
      swaps = false;
      for(int i=0; i<n-1; ++i){
        if(children[order[i]]->time()  < children[order[i+1]]->time())
          {
            int tmp = order[i];
            order[i] = order[i+1];
            order[i+1] = tmp;
            swaps = true;
            break;
          }
      }
    }
}

void TraceTimer::subReport(FILE* out, const char* header, unsigned long long int totalTime)
{
  size_t length = strlen(header);
  fprintf(out, "=======================================================\n");
  unsigned long long int subTime = 0;
  std::list<elem>::iterator it;
  for(it=tracerlist.begin(); it!= tracerlist.end(); ++it)
    {
      const char* name = (*it).val->m_name;
      if(strncmp(header, name, length) == 0){
        if((*it).val->isPruned()){
          //fprintf(out, "             pruned  %s  \n", name);

        } else {
          unsigned long long int t = (*it).val->time();
          int rank = (*it).val->rank();
          subTime += t;
          fprintf(out, "  %8.5f %8lld  %s [%d] \n", t*secondspertick, (*it).val->m_count, name, rank);
        }
      }
    }
  if(subTime > 0)
    fprintf(out, "  %8.5f   %4.1f%%    Total\n", subTime*secondspertick, (double)subTime/totalTime*100.0);
}

void TraceTimer::reportFullTree(FILE* out, const TraceTimer& timer,
                                unsigned long long int totalTime, int depth)
{
  if(timer.m_pruned) return;
  unsigned long long int time = timer.m_accumulated_WCtime;

  if(depth < 20){
    for(int i=0; i<depth; ++i) fprintf(out,"   ");
    double percent = ((double)time)/totalTime * 100.0;
    fprintf(out, "[%d] %s %.5f %4.1f%% %lld \n", timer.m_rank, timer.m_name, time*secondspertick, percent, timer.m_count);
  }
  std::vector<int> ordering;
  sorterHelper(timer.m_children, ordering);
  for(unsigned int i=0; i<timer.m_children.size(); ++i){
    reportFullTree(out, *(timer.m_children[ordering[i]]), totalTime, depth+1);
  }

}
void TraceTimer::reportOneTree(FILE* out, const TraceTimer& timer)
{
  if(timer.m_pruned) return;
  unsigned long long int time = timer.m_accumulated_WCtime;
  unsigned long long int subTime = 0;

  fprintf(out,"---------------------------------------------------------\n");

  fprintf(out,"[%d]%s %.5f %lld\n", timer.m_rank, timer.m_name, time*secondspertick, timer.m_count);
  const std::vector<TraceTimer*>& children = timer.m_children;
  std::vector<int> ordering;
  sorterHelper(children, ordering);
  for(unsigned int i=0; i<children.size(); ++i)
    {
      const TraceTimer& child = *(children[ordering[i]]);
      if(!child.m_pruned)
        {
          unsigned long long int childtime = child.m_accumulated_WCtime;
          if(childtime > 0){
            subTime += childtime;
            double percent = ((double)childtime) / time * 100.0;
            fprintf(out,"    %4.1f%% %7.4f %8lld %s [%d]\n",
                    percent, childtime*secondspertick, child.m_count, child.m_name, child.m_rank);
          }
        } else {
         fprintf(out,"           pruned           \n");
         i=children.size();
      }
    }
  if(time > 0 && children.size() > 0){
    double totalPercent = ((double)subTime)/ time * 100.0;
    fprintf(out, "    %4.1f%%                  Total \n", totalPercent);
  }

}


void TraceTimer::reportMemoryOneTree(FILE* out, const TraceTimer& timer)
{
  if(timer.m_pruned) return;
  long long int m = timer.m_memory;
  long long int p = timer.m_peak;

  if(p==0 && m==0) return;

  char units[2], punits[2];

  int  memory, peak;

  normalizeMemory(m, memory, units);
  normalizeMemory(p, peak, punits);

  fprintf(out,"---------------------------------------------------------\n");

  fprintf(out,"[%d]%s %5d%s  %5d%s %lld\n", timer.m_rank, timer.m_name, peak, punits,
          memory, units, timer.m_count);

  const std::vector<TraceTimer*>& children = timer.m_children;
  std::vector<int> ordering;
  sorterHelper(children, ordering);
  for(unsigned int i=0; i<children.size(); ++i)
    {
      const TraceTimer& child = *(children[ordering[i]]);
      if(!child.m_pruned)
        {
          long long int pm = child.m_peak;
          long long int sm = child.m_memory;

          if(sm != 0 || pm != 0){
            normalizeMemory(sm, memory, units);
            normalizeMemory(pm, peak, punits);
            fprintf(out,"   %5d%s   %5d%s %8lld %s [%d]\n",
                    peak, punits, memory, units, child.m_count, child.m_name, child.m_rank);
          }
        } else {
         fprintf(out,"           pruned           \n");
         i=children.size();
      }
    }


}

// some compilers complain if there isn't at least 1 non-inlined
// function for every class.  These two are mostly to make those compilers happy

char AutoStart::ok = 0;

bool AutoStart::active() { return true;}

bool AutoStartLeaf::active() { return true;}

TraceTimer::~TraceTimer()
{
  for(unsigned int i=0; i<m_children.size(); ++i)
    {
      delete m_children[i];
    }
}

TraceTimer* TraceTimer::getTimer(const char* name)
{
  int thread_id = 0; // this line will change in MThread-aware code.
  TraceTimer* parent = TraceTimer::s_currentTimer[thread_id];
  if(parent->m_pruned) return parent;
  std::vector<TraceTimer*>& children = parent->m_children;
  unsigned int i=0;
  for(; i<children.size(); ++i){
    TraceTimer* timer =  children[i];
    if(timer->m_name == name) return timer;
  }
  TraceTimer* newTimer = new TraceTimer(name, parent, thread_id);
  children.push_back(newTimer);
  return newTimer;
}

TraceTimer::TraceTimer(const char* a_name, TraceTimer* parent, int thread_id)
  :m_pruned(false), m_parent(parent), m_name(a_name), m_count(0),
   m_accumulated_WCtime(0),m_last_WCtime_stamp(0), m_thread_id(thread_id),
   m_memory(0), m_peak(0)
{

}

void TraceTimer::prune()
{
  unsigned int i=0;
  for(; i<m_children.size(); ++i){
    TraceTimer* timer =  m_children[i];
    timer->prune();
  }
  m_pruned = true;
}

void TraceTimer::start(char* mutex)
{
  if(m_pruned) return;
# ifndef NDEBUG
  if(*mutex == 1) {
    char buf[1024];
    sprintf(buf, "double TraceTimer::start called: %s ",m_name);
    std::cerr<<buf;
    abort();
  }
# endif
  ++m_count;
  *mutex = 1;
  s_currentTimer[m_thread_id] = this;
  m_last_WCtime_stamp = ch_ticks();
#ifdef  CH_USE_MEMORY_TRACKING
  if(s_traceMemory)
    {
      long long int peak;
      overallMemoryUsage(m_last_Memory_Stamp, peak);
      if(peak > m_peak) m_peak = 0;
    }
#endif
}
unsigned long long int overflowLong = (unsigned long long int)1<<50;
unsigned long long int TraceTimer::stop(char* mutex)
{
  if(m_pruned) return 0;
#ifndef NDEBUG
  if(s_currentTimer[0] != this)
    {
    char buf[1024];
    sprintf(buf, "TraceTimer::stop called while not parent: %s ",m_name);
    std::cerr<<buf;
    abort();
    }
#endif
  unsigned long long int diff = ch_ticks();
  diff -= m_last_WCtime_stamp;
  if(diff > overflowLong) diff = 0;
  m_accumulated_WCtime += diff;
#ifdef  CH_USE_MEMORY_TRACKING
  if(s_traceMemory)
    {
      long long int current, peak, d;
      overallMemoryUsage(current, peak);
      if(peak > s_peak){
        s_peak = peak;
        s_peakTimer = this;
        TraceTimer* c = this;
        while(c!=NULL){
          c->m_peak = peak;
          c=c->m_parent;
        }
      }



      d = current - m_last_Memory_Stamp;
      m_memory += d;
      m_last_Memory_Stamp = 0;
    }
#endif
  m_last_WCtime_stamp=0;
  s_currentTimer[m_thread_id] = m_parent;
  *mutex=0;
  return diff;
}


void TraceTimer::macroTest2()
{
  CH_TIME("macroTest2");
}
void TraceTimer::macroTest()
{
  CH_TIMERS("testy");
  CH_TIMER("billy", t1);
  CH_TIMER("sally", t2);
  CH_TIMER("renaldo", billy);
  CH_START(t1);
  CH_STOP(t1);
  CH_START(t2);
  CH_STOP(t2);
  CH_START(billy);
  CH_STOP(billy);
  CH_TIMER_REPORT();
  CH_TIMER_RESET();
  CH_TIMER_PRUNE(0.01);
}

void TraceTimer::PruneTimersParentChildPercent(double threshold, TraceTimer* parent)
{
  if(parent->isPruned()) return;
  unsigned long long int time = parent->time();
  const std::vector<TraceTimer*>& children = parent->children();

  for(unsigned int i=0; i<children.size(); ++i)
    {
      TraceTimer* child = children[i];
      if(!child->isPruned())
        {
          unsigned long long int childtime = child->time();
          if(((double)childtime)/time < threshold) child->prune();
          else PruneTimersParentChildPercent(threshold, child);
        }

    }
}


void TraceTimer::PruneTimersParentChildPercent(double percent)
{
#ifndef CH_NTIMER
  char* timerEnv = getenv("CH_TIMER");
  if(timerEnv == NULL)
    {
      // pout()<<"CH_TIMER environment variable not set. Timers inactive. Not writing time.table \n";
      return;
    }
  TraceTimer* root = s_roots[0]; // in MThread code, loop over roots
  root->currentize();
  PruneTimersParentChildPercent(percent, root);
#endif
}

#else // on CH_NTIMER
const char* currentTimer()
{
  const char* rtn ="Timers not active";
  return rtn;
}

#endif //on CH_NTIMER

#ifndef CH_NTIMER

#endif
