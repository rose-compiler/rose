#include "rose.h"

#include "rose_aterm_api.h"

ATerm rose_AT_getAnnotations(ATerm t)
   {
  // Initial implementation.

     ROSE_ASSERT(!"ROSE ATerm API: rose_AT_getAnnotations(): not implemented!");

     return NULL;
   }

// **********************************************************************************
// These are the implementation that are not taking advantage of function forwarding.
// These are required to support the SDF support, 57 functions to support...
// **********************************************************************************

AFun ATmakeAFun(const char *name, int arity, ATbool quoted)
   {
     return NULL;
   }

void ATprotectAFun(AFun sym)
   {
   }

unsigned long  ATcalcTextSize(ATerm t)
   {
     return 0;
   }

void AT_writeToStringBuffer(ATerm t, char *buffer)
   {
   }

ATermList ATinsert(ATermList list, ATerm el)
   {
     return NULL;
   }

ATermBlob ATmakeBlob(unsigned int size, void *data)
   {
     return NULL;
   }

ATermAppl ATmakeAppl1(AFun sym, ATerm arg0)
   {
     return NULL;
   }

ATerm AT_getAnnotations(ATerm t)
   {
     return NULL;
   }

ATermPlaceholder ATmakePlaceholder(ATerm type)
   {
     return NULL;
   }

ATermAppl ATmakeApplList(AFun sym, ATermList args)
   {
     return NULL;
   }

ATermList ATreverse(ATermList list)
   {
     return NULL;
   }

ATerm AT_setAnnotations(ATerm t, ATerm annos)
   {
     return NULL;
   }

ATermList ATgetSlice(ATermList list, unsigned int start, unsigned int end)
   {
     return NULL;
   }

ATermAppl ATsetArgument(ATermAppl appl, ATerm arg, unsigned int n)
   {
     return NULL;
   }

ATermAppl ATmakeAppl0(AFun sym)
   {
     return NULL;
   }

ATerm ATelementAt(ATermList list, unsigned int index)
   {
     return NULL;
   }

ATermInt ATmakeInt(int value)
   {
     return NULL;
   }

ATermAppl ATmakeAppl3(AFun sym, ATerm arg0, ATerm arg1, ATerm arg2)
   {
     return NULL;
   }

unsigned int ATgetLength(ATermList list)
   {
     return 0;
   }

ATermList ATconcat(ATermList list1, ATermList list2)
   {
     return NULL;
   }

ATermList ATreplace(ATermList list, ATerm el, unsigned int idx)
   {
     return NULL;
   }

ATermAppl ATmakeAppl6(AFun sym, ATerm arg0, ATerm arg1, ATerm arg2, ATerm arg4, ATerm arg5, ATerm arg6)
   {
     return NULL;
   }

ATermAppl ATmakeAppl2(AFun sym, ATerm arg0, ATerm arg1)
   {
     return NULL;
   }

ATermList ATreplaceTail(ATermList list, ATermList newtail, int start)
   {
     return NULL;
   }

ATermList ATappend(ATermList list, ATerm el)
   {
     return NULL;
   }

ATermAppl ATmakeAppl(AFun sym, ...)
   {
     return NULL;
   }

ATermList ATgetTail(ATermList list, int start)
   {
     return NULL;
   }

ATermList ATmakeList1(ATerm el0)
   {
     return NULL;
   }

int ATcompare(ATerm t1, ATerm t2)
   {
     return 0;
   }

ATermTable ATtableCreate(long initial_size, int max_load_pct)
   {
     return NULL;
   }

void ATtableDestroy(ATermTable table)
   {
   }

ATerm AT_removeAnnotations(ATerm t)
   {
     return NULL;
   }

void ATtableReset(ATermTable table)
   {
   }

ATermList ATsort(ATermList list, int (*compare)(const ATerm t1, const ATerm t2))
   {
     return NULL;
   }

void ATtablePut(ATermTable table, ATerm key, ATerm value)
   {
   }

ATerm	ATtableGet(ATermTable table, ATerm key)
   {
     return NULL;
   }

ATerm ATremoveAllAnnotations(ATerm t)
   {
     return NULL;
   }

ATermReal ATmakeReal(double value)
   {
     return NULL;
   }

ATermList ATgetArguments(ATermAppl appl)
   {
     return NULL;
   }

int ATindexOf(ATermList list, ATerm el, int start)
   {
     return 0;
   }

ATermList  ATtableKeys(ATermTable table)
   {
     return NULL;
   }

void ATindexedSetDestroy(ATermIndexedSet set)
   {
   }

ATermList ATremoveElement(ATermList list, ATerm el)
   {
     return NULL;
   }

void ATtableRemove(ATermTable table, ATerm key)
   {
   }


ATermIndexedSet ATindexedSetCreate(long initial_size, int max_load_pct)
   {
     return NULL;
   }

// void       ATindexedSetDestroy(ATermIndexedSet set);
// void       ATindexedSetReset(ATermIndexedSet set);

long ATindexedSetPut(ATermIndexedSet set, ATerm elem, ATbool *isnew)
   {
     return 0;
   }

long ATindexedSetGetIndex(ATermIndexedSet set, ATerm elem)
   {
     return 0;
   }

void ATindexedSetRemove(ATermIndexedSet set, ATerm elem)
   {
   }

// ATermList  ATindexedSetElements(ATermIndexedSet set);
// ATerm      ATindexedSetGetElem(ATermIndexedSet set, long index);

ATermList ATgetPrefix(ATermList list)
   {
     return NULL;
   }

ATerm ATgetLast(ATermList list)
   {
     return NULL;
   }

ATermList ATmakeList(unsigned int n, ...)
   {
     return NULL;
   }

ATermAppl ATmakeAppl4(AFun sym, ATerm arg0, ATerm arg1, ATerm arg2, ATerm arg3)
   {
     return NULL;
   }

ATermAppl ATmakeApplArray(AFun sym, ATerm args[])
   {
     return NULL;
   }

ATerm ATindexedSetGetElem(ATermIndexedSet set, long index)
   {
     return NULL;
   }

unsigned long  ATcalcUniqueSubterms(ATerm t)
   {
     return 0;
   }

ATermList  ATtableValues(ATermTable table)
   {
     return NULL;
   }

void ATsetChecking(ATbool on)
   {
   }



// From gc.h (garbage collection support).  This is an example of a 
// function in the ATerm library being used which is not a part of 
// its API (as represented by the ATerm header files.
// However, it is in gc.h (so maybe I am being too strict here).
// Out ROSE implementation should maybe have a gc.h file so that
// we can support this (though I am hoping to skip the GC work).
extern "C"
void AT_collect()
   {
   }

// This is declared in aterm2.h but defined in memory.c and is referenced directly (or via macros).
ATermList ATempty;

// This is declared in aterm2.h (not clear where it is defined in the ATerm library source files).
int at_gc_count;

// This is a function not a part or the API (declared in _aterm.h).
extern "C"
unsigned long AT_calcAllocatedSize()
   {
     return 0;
   }



// ************* Stratego Specific API Requirements ************
// Adding 5 more functions from the Aterm API.
// *************************************************************

ATermAppl ATmakeAppl5(AFun sym, ATerm arg0, ATerm arg1, ATerm arg2, ATerm arg4, ATerm arg5)
   {
     return NULL;
   }

ATermList  ATindexedSetElements(ATermIndexedSet set)
   {
     return NULL;
   }

unsigned char *ATchecksum(ATerm t)
   {
     return 0;
   }

void ATindexedSetReset(ATermIndexedSet set)
   {
   }


// From afun.h (should be put into afun.C file later.
ATbool AT_findSymbol(char *name, int arity, ATbool quoted)
   {
   }













