#include <timing.h>
#include <iostream>

class ResultRegistry
{
public:
 const char* msg;
 ResultRegistry* next;
 ResultRegistry(const char* m, ResultRegistry* n) 
   : msg(m), next(n) {} 
 virtual void print() =0;
};

class TimingRegistry : public ResultRegistry
{
public:
 double* result;

 TimingRegistry(double* r, const char* m, ResultRegistry* n) 
   : ResultRegistry(m,n), result(r) {}
 virtual void print() { 
   std::cerr << msg << (*result) << "\n";
 }
};

class FreqRegistry : public ResultRegistry
{
public:
 unsigned* result;

 FreqRegistry(unsigned* r, const char* m, ResultRegistry* n) 
   : ResultRegistry(m,n), result(r) {}

 virtual void print() { 
   std::cerr << msg << (*result) << "\n";
 }
};

static ResultRegistry *registries = 0;


void register_timing(double* variable, const char* msg)
{
  TimingRegistry *r = new TimingRegistry(variable,msg,registries);
  registries = r;
}

void register_freq(unsigned* variable, const char* msg)
{
  FreqRegistry *r = new FreqRegistry(variable,msg,registries);
  registries = r;
}

void report_timing()
{
  for (ResultRegistry *p = registries; p != 0; p = p->next)
  {
     p->print();
  }
}

double GetWallTime(void)
   {
      struct timeval tp;
      static long start=0, startu;
      if (!start)
      {
         gettimeofday(&tp, NULL);
         start = tp.tv_sec;
         startu = tp.tv_usec;
         return(0.0);
      }
      gettimeofday(&tp, NULL);
      return( ((double) (tp.tv_sec - start)) + (tp.tv_usec-startu)/1000000.0 );
   }

