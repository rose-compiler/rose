#ifndef BACKSTROKE_RT_LIB
#define BACKSTROKE_RT_LIB

#include <stack>
#include <queue>
#include <deque>
#include <inttypes.h>
#include <map>

using namespace std;

#include "ross.h"

namespace Backstroke {

  class SimTime {
  public:
    SimTime();
    SimTime(tw_stime simTime);
    bool operator<(const SimTime& other);
  private:
    tw_stime _simTime;
  };
  
#include "backstroke2-rt-lib-macros.h"

class RunTimeSystem {
 private:
  enum BuiltInType {
    BITYPE_BOOL,
    BITYPE_CHAR,
    BITYPE_SHORT_INT,
    BITYPE_INT,
    BITYPE_LONG_INT,
    BITYPE_LONG_LONG_INT,
    BITYPE_USHORT_INT,
    BITYPE_UINT,
    BITYPE_ULONG_INT,
    BITYPE_ULONG_LONG_INT,
    BITYPE_FLOAT,
    BITYPE_DOUBLE,
    BITYPE_LONG_DOUBLE,
    BITYPE_PTR,
    BITYPE_NUM
  };
 public:
  typedef void* ptr;
  struct EventRecord {
    void deallocateHeapQueue();
    stack<BuiltInType> stack_bitype;
    queue<ptr> registeredHeapQueue;
    SimTime simTime;
  };
  RunTimeSystem();
  void initializeForwardEvent();
  void setEventSimTime(Backstroke::SimTime simTime);
  void finalizeForwardEvent();
  void reverseEvent();
  void commitEventsLessThanSimTime(Backstroke::SimTime simTime);
  void commitEvent();
  void deallocate(EventRecord* commitEventRecord);
  void registerForCommit(ptr p);

  size_t numberOfUncommittedEvents();
  size_t size();
  size_t currentEventLength();

  // headers with private data members for all built-in types
  STACK_ASSIGN_RESTORE_HEADER(bool,bool);
  STACK_ASSIGN_RESTORE_HEADER(char,char);
  STACK_ASSIGN_RESTORE_HEADER(short int,shortint);
  STACK_ASSIGN_RESTORE_HEADER(int,int);
  STACK_ASSIGN_RESTORE_HEADER(long int,longint);
  STACK_ASSIGN_RESTORE_HEADER(long long int,longlongint);
  STACK_ASSIGN_RESTORE_HEADER(unsigned short int,ushortint);
  STACK_ASSIGN_RESTORE_HEADER(unsigned int,uint);
  STACK_ASSIGN_RESTORE_HEADER(unsigned long int,ulongint);
  STACK_ASSIGN_RESTORE_HEADER(unsigned long long int,ulonglongint);
  STACK_ASSIGN_RESTORE_HEADER(float,float);
  STACK_ASSIGN_RESTORE_HEADER(double,double);
  STACK_ASSIGN_RESTORE_HEADER(long double,longdouble);

  // headers for ptr handling
 private:
  stack<pair<ptr*, ptr> > stack_ptr;
  void restore_ptr();
 public:
  ptr assignptr(ptr* address, ptr value);
  ptr* avpushptr(ptr* address);

 public:
  void init_stack_info(); // must be called to initialize run-time-system
  void print_stack_info();
  inline bool is_stack_ptr(void *ptr) {
    bool is_stack_ptr= ((uintptr_t) ptr >= (uintptr_t) prog_stack_bottom
			&& (uintptr_t) ptr <= (uintptr_t) prog_stack_local);
    //cout<<"DEBUG: is_stack_ptr("<<ptr<<"):"<<is_stack_ptr<<endl;
    return is_stack_ptr;
  }

  void restore(BuiltInType bitype);
  deque<EventRecord*> eventRecordDeque;
  size_t typeSize(BuiltInType biType);
  EventRecord* currentEventRecord;

  // STACK/HEAP detection
  uintptr_t prog_stack_bottom;
  uintptr_t prog_stack_local; // can be set to function stack (default: same as stack_bottom)
  uintptr_t prog_stack_max;

  // alloc functions
  void* allocateArray(size_t arraySize, size_t ArrayElementTypeSize);

  // dealloc functions
  // also requires callArrayElementDestructors(ArrayElementType* arrayPointer) to be called
  void registerArrayDeallocation(void* rawMemoryPtr);
  void deallocateArray(void* rawMemoryPtr);

};

 typedef RunTimeSystem RunTimeStateStorage;

 class RunTimeLpStateStorage {
   typedef std::map<void*, RunTimeStateStorage*> LpToRTSSMapping;
 public:
   RunTimeStateStorage* getLpStateStorage(void* lp) {
     return lp_ss_mapping[lp];
   }
   void setLpStateStorage(void* lp, RunTimeStateStorage* ss) {
     lp_ss_mapping[lp]=ss;
   }
   bool isSetLp(void* lp) {
     LpToRTSSMapping::iterator it=lp_ss_mapping.find(lp);
     return it!=lp_ss_mapping.end();
   }
   RunTimeStateStorage* getLpStateStorageAutoAllocated(void* lp) {
     if(isSetLp(lp)) {
       return lp_ss_mapping[lp];
     } else {
       RunTimeStateStorage* rtss=new RunTimeStateStorage();
       lp_ss_mapping[lp]=rtss;
       return rtss;
     }
   }   
 private:
   LpToRTSSMapping lp_ss_mapping;
 };

// template function for array operator delete[]
template <typename ArrayElementType>
void callArrayElementDestructors(ArrayElementType* arrayPointer) {
  // determine array size (platform specific)
  std::size_t* rawMemory=reinterpret_cast<std::size_t*>(arrayPointer)-1;
  std::size_t arraySize=*rawMemory;
  //cout<<"INFO: determined array size: "<<arraySize<<endl;
  if(arrayPointer != 0) {    
    ArrayElementType *p = arrayPointer + arraySize;
    while (p != arrayPointer)        
      (--p)->~UserType();
    //::operator delete [](rawMemory);
  }
}

} // end of namespace Backstroke



#endif
