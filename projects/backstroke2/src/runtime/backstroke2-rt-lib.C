#include "backstroke2-rt-lib.h"

#include <pthread.h>
#include <iostream>
#include <cassert>

using namespace std;

Backstroke::SimTime::SimTime():_simTime(-1.0) {
}

Backstroke::SimTime::SimTime(tw_stime simTime):_simTime(simTime) {
}

bool Backstroke::SimTime::operator<(const SimTime& other) {
  return _simTime<other._simTime;
}

// this function is called when an event is reversed ("de-register") and when an event is committed.
void Backstroke::RunTimeSystem::EventRecord::deallocateHeapQueue() {
  while(!registeredHeapQueue.empty()) {
    //cout<<"DEBUG: deleting object."<<endl;
    ptr toDeallocPtr=registeredHeapQueue.front();
    registeredHeapQueue.pop();
    // we only need to call delete(x) because ~X() has already been called in the forward method
    operator delete(toDeallocPtr);
  }
}

Backstroke::RunTimeSystem::RunTimeSystem():currentEventRecord(0) {
}

size_t Backstroke::RunTimeSystem::numberOfUncommittedEvents() {
  return eventRecordDeque.size();
}

size_t Backstroke::RunTimeSystem::size() {
  return eventRecordDeque.size();
}

// allocates EventRecord
void Backstroke::RunTimeSystem::initializeForwardEvent() {
  currentEventRecord=new EventRecord();
}

void Backstroke::RunTimeSystem::finalizeForwardEvent() {
 eventRecordDeque.push_back(currentEventRecord);
 currentEventRecord=0;
}

// deallocates EventRecord
void Backstroke::RunTimeSystem::reverseEvent() {
  EventRecord* restorationEventRecord=eventRecordDeque.back();
  eventRecordDeque.pop_back();
  while(!restorationEventRecord->stack_bitype.empty()) {
    BuiltInType bitype=restorationEventRecord->stack_bitype.top();
    restorationEventRecord->stack_bitype.pop();
    restore(bitype);
  }

  // de-register and delete objects in commit heap queue
  restorationEventRecord->deallocateHeapQueue();

  delete restorationEventRecord;
  restorationEventRecord=0;
}

void Backstroke::RunTimeSystem::registerForCommit(ptr p) {
  currentEventRecord->registeredHeapQueue.push(p);
}

void Backstroke::RunTimeSystem::setEventSimTime(Backstroke::SimTime simTime) {
  currentEventRecord->simTime=simTime;
}

// deallocates EventRecord
void Backstroke::RunTimeSystem::commitEventsLessThanSimTime(Backstroke::SimTime simTime) {
  while(eventRecordDeque.size()>0) {
    EventRecord* commitEventRecord=eventRecordDeque.front();
    assert(commitEventRecord!=0);
    if(commitEventRecord->simTime<simTime) {
      eventRecordDeque.pop_front();
      deallocate(commitEventRecord);
    } else {
      break;
    }
  }
  //cout<<"INFO: record queue empty - all events committed."<<endl;
}

// pops event-record from event-record-queue and deallocates event-record
void Backstroke::RunTimeSystem::commitEvent() {
  EventRecord* commitEventRecord=eventRecordDeque.front();
  eventRecordDeque.pop_front();
  deallocate(commitEventRecord);
}

// deallocates EventRecord
void Backstroke::RunTimeSystem::deallocate(EventRecord* commitEventRecord) {
  //cerr<<"INFO: committing and deallocating event "<<commitEventRecord<<endl;
  commitEventRecord->deallocateHeapQueue();
  // alternatively deallocateHeapQueue could be called by destructor
  delete commitEventRecord; 
}

void Backstroke::RunTimeSystem::restore(BuiltInType bitype) {
  //cout<<"DEBUG: restoring type "<<bitype<<endl;
  switch(bitype) {
    CASE_ENUM_RESTORE(BOOL,bool);
    CASE_ENUM_RESTORE(CHAR,char);
    CASE_ENUM_RESTORE(SHORT_INT,shortint);
    CASE_ENUM_RESTORE(INT,int);
    CASE_ENUM_RESTORE(LONG_INT,longint);
    CASE_ENUM_RESTORE(LONG_LONG_INT,longlongint);
    CASE_ENUM_RESTORE(USHORT_INT,ushortint);
    CASE_ENUM_RESTORE(UINT,uint);
    CASE_ENUM_RESTORE(ULONG_INT,ulongint);
    CASE_ENUM_RESTORE(ULONG_LONG_INT,ulonglongint);
    CASE_ENUM_RESTORE(FLOAT,float);
    CASE_ENUM_RESTORE(DOUBLE,double);
    CASE_ENUM_RESTORE(LONG_DOUBLE,longdouble);
  case BITYPE_PTR: restore_ptr();break;
  default:
    throw "Error: restoring unknown built-in type.";
  }
}

ASSIGN_RESTORE_IMPLEMENTATION(bool,bool,BITYPE_BOOL)
ASSIGN_RESTORE_IMPLEMENTATION(char,char,BITYPE_CHAR)
ASSIGN_RESTORE_IMPLEMENTATION(short int,shortint,BITYPE_SHORT_INT)
ASSIGN_RESTORE_IMPLEMENTATION(int,int,BITYPE_INT)
ASSIGN_RESTORE_IMPLEMENTATION(long int,longint,BITYPE_LONG_INT)
ASSIGN_RESTORE_IMPLEMENTATION(long long int,longlongint,BITYPE_LONG_LONG_INT)
ASSIGN_RESTORE_IMPLEMENTATION(unsigned short int,ushortint,BITYPE_USHORT_INT)
ASSIGN_RESTORE_IMPLEMENTATION(unsigned int,uint,BITYPE_UINT)
ASSIGN_RESTORE_IMPLEMENTATION(unsigned long int,ulongint,BITYPE_ULONG_INT)
ASSIGN_RESTORE_IMPLEMENTATION(unsigned long long int,ulonglongint,BITYPE_ULONG_LONG_INT)
ASSIGN_RESTORE_IMPLEMENTATION(float,float,BITYPE_FLOAT)
ASSIGN_RESTORE_IMPLEMENTATION(double,double,BITYPE_DOUBLE)
ASSIGN_RESTORE_IMPLEMENTATION(long double,longdouble,BITYPE_LONG_DOUBLE)

Backstroke::RunTimeSystem::ptr Backstroke::RunTimeSystem::assignptr(Backstroke::RunTimeSystem::ptr* address, Backstroke::RunTimeSystem::ptr value) {
  if(!is_stack_ptr(address)) {
    currentEventRecord->stack_bitype.push(BITYPE_PTR);
    stack_ptr.push(make_pair(address,*address));
  }
  return *address=value;
}

Backstroke::RunTimeSystem::ptr* Backstroke::RunTimeSystem::avpushptr(Backstroke::RunTimeSystem::ptr* address) {
  if(!is_stack_ptr(address)) {
    currentEventRecord->stack_bitype.push(BITYPE_PTR);
    stack_ptr.push(make_pair(address,*address));
  }
  return address;
}

void Backstroke::RunTimeSystem::restore_ptr() {
  pair<ptr*,ptr> p=stack_ptr.top();
  stack_ptr.pop();
  *(p.first)=p.second;
}

size_t Backstroke::RunTimeSystem::typeSize(BuiltInType biType) {
  switch(biType) {
    CASE_ENUM_SIZEOF(BOOL,bool);
    CASE_ENUM_SIZEOF(CHAR,char);
    CASE_ENUM_SIZEOF(SHORT_INT,short int);
    CASE_ENUM_SIZEOF(INT,int);
    CASE_ENUM_SIZEOF(LONG_INT,long int);
    CASE_ENUM_SIZEOF(LONG_LONG_INT,long long int);
    CASE_ENUM_SIZEOF(USHORT_INT,unsigned short int);
    CASE_ENUM_SIZEOF(UINT,unsigned int);
    CASE_ENUM_SIZEOF(ULONG_INT,unsigned long int);
    CASE_ENUM_SIZEOF(ULONG_LONG_INT,unsigned long long int);
    CASE_ENUM_SIZEOF(FLOAT,float);
    CASE_ENUM_SIZEOF(DOUBLE,double);
    CASE_ENUM_SIZEOF(LONG_DOUBLE,long double);
    CASE_ENUM_SIZEOF(PTR,ptr);
  default:
    cerr<<"Error: size of unsupported built-in type."<<endl;
    throw "Backstoke:Runtime";
  }
}

void Backstroke::RunTimeSystem::init_stack_info() {
  pthread_t self = pthread_self();
  pthread_attr_t attr;
  void *stack;
  size_t stacksize;
  pthread_getattr_np(self, &attr);
  pthread_attr_getstack(&attr, &stack, &stacksize);
  uintptr_t stackmax=(uintptr_t)((long int)stack+(long int)stacksize);
  prog_stack_bottom=(uintptr_t)stack;
  prog_stack_max=stackmax;
  prog_stack_local=prog_stack_max;
}

void Backstroke::RunTimeSystem::print_stack_info() {
  cout<<"STACKTOP     : "<<prog_stack_max<<endl;
  cout<<"STACKLOCALTOP: "<<prog_stack_local<<endl;
  cout<<"STACKEND     : "<<prog_stack_bottom<<endl;
}

#if 0
bool Backstroke::RunTimeSystem::is_stack_ptr(void *ptr) {
  bool is_stack_ptr= ((uintptr_t) ptr >= (uintptr_t) prog_stack_bottom
          && (uintptr_t) ptr <= (uintptr_t) prog_stack_local);
  //cout<<"DEBUG: is_stack_ptr("<<ptr<<"):"<<is_stack_ptr<<endl;
  return is_stack_ptr;
}
#endif
  
size_t Backstroke::RunTimeSystem::currentEventLength() {
  return currentEventRecord->stack_bitype.size();
}

void* Backstroke::RunTimeSystem::allocateArray(size_t arraySize, size_t arrayElementTypeSize) {
  // allocate one additional size_t for size
  size_t* rawMemory=static_cast<size_t*>(::operator new (static_cast<size_t>(arraySize*arrayElementTypeSize)+1));
  // store size
  *rawMemory=arraySize;
  // return array-pointer (excluding size field)
  void* arrayPointer=reinterpret_cast<void*>(rawMemory+1);
  //cout<<"INFO: array pointer: "<<arrayPointer<<endl;
  return arrayPointer;
 }

void Backstroke::RunTimeSystem::registerArrayDeallocation(void* rawMemoryPtr) {
  // TODO: register array ptr (for deallocateArray-call in case of commit)
}

void Backstroke::RunTimeSystem::deallocateArray(void* rawMemoryPtr) {
  ::operator delete[](rawMemoryPtr);
}

