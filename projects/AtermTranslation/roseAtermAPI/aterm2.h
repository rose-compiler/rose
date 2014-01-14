/**
  * aterm2.h: Definition of the level 2 interface
  * of the ATerm library.
  */

#ifndef ATERM2_H
#define ATERM2_H

/**
  * The level 2 interface is a strict superset 
  * of the level 1 interface.
  */

#include "aterm1.h"
#include "afun.h"
#include "abool.h"

#ifdef __cplusplus
extern "C"
{
#endif/* __cplusplus */



/**
  * We define some new datatypes.
  */

struct __ATermInt
{
  header_type header;
  ATerm       next;
  int         value;
};

typedef union _ATermInt
{
  header_type        header;
  struct __ATermInt  aterm;
} *ATermInt;

struct __ATermReal
{
  header_type header;
  ATerm       next;
  double      value;
};

typedef union _ATermReal
{
  header_type         header;
  struct __ATermReal  aterm;
} *ATermReal;

struct __ATermAppl
{
  header_type header;
  ATerm       next;
  ATerm       arg[1];
};

typedef union _ATermAppl
{
  header_type         header;
  struct __ATermAppl  aterm;
} *ATermAppl;

struct __ATermList
{
  header_type       header;
  ATerm             next;
  ATerm             head;
  union _ATermList *tail;
};

typedef union _ATermList
{
  header_type         header;
  struct __ATermList  aterm;
} *ATermList;

struct __ATermPlaceholder
{
  header_type header;
  ATerm       next;
  ATerm       ph_type;
};

typedef union _ATermPlaceholder
{
  header_type                header;
  struct __ATermPlaceholder  aterm;
} *ATermPlaceholder;

struct __ATermBlob
{
  header_type  header;
  ATerm        next;
  unsigned int size;
  void        *data;
};

typedef union _ATermBlob
{
  header_type         header;
  struct __ATermBlob  aterm;
} *ATermBlob;

struct _ATermTable;

typedef struct _ATermTable *ATermIndexedSet;
typedef struct _ATermTable *ATermTable;


/** The following functions implement the operations of
  * the 'standard' ATerm interface, and should appear
  * in some form in every implementation of the ATerm
  * datatype.
  */

/* The ATermInt type */
ATermInt ATmakeInt(int value);
/*int      ATgetInt(ATermInt term);*/
#define ATgetInt(t) (((ATermInt)t)->aterm.value)

/* The ATermReal type */
ATermReal ATmakeReal(double value);
/*double    ATgetReal(ATermReal term);*/
#define ATgetReal(t) (((ATermReal)t)->aterm.value)

/* The ATermAppl type */
ATermAppl ATmakeAppl(AFun sym, ...);
ATermAppl ATmakeAppl0(AFun sym);
ATermAppl ATmakeAppl1(AFun sym, ATerm arg0);
ATermAppl ATmakeAppl2(AFun sym, ATerm arg0, ATerm arg1);
ATermAppl ATmakeAppl3(AFun sym, ATerm arg0, ATerm arg1, ATerm arg2);
ATermAppl ATmakeAppl4(AFun sym, ATerm arg0, ATerm arg1, ATerm arg2,
		       ATerm arg3);
ATermAppl ATmakeAppl5(AFun sym, ATerm arg0, ATerm arg1, ATerm arg2,
		       ATerm arg4, ATerm arg5);
ATermAppl ATmakeAppl6(AFun sym, ATerm arg0, ATerm arg1, ATerm arg2,
		       ATerm arg4, ATerm arg5, ATerm arg6);

/*AFun    ATgetAFun(ATermAppl appl);*/
#define ATgetAFun(appl) GET_SYMBOL((appl)->header)
#define ATgetSymbol ATgetAFun

/* ATerm     ATgetArgument(ATermAppl appl, unsigned int arg); */
#define ATgetArgument(appl,idx) (((ATermAppl)appl)->aterm.arg[idx])
ATermAppl ATsetArgument(ATermAppl appl, ATerm arg, unsigned int n);

/* Portability */
ATermList ATgetArguments(ATermAppl appl);
ATermAppl ATmakeApplList(AFun sym, ATermList args);
ATermAppl ATmakeApplArray(AFun sym, ATerm args[]);

/* The ATermList type */
extern ATermList ATempty;

ATermList ATmakeList(unsigned int n, ...);

/* ATermList ATmakeList0(); */
#define ATmakeList0() (ATempty)

ATermList ATmakeList1(ATerm el0);

/* ATermList ATmakeList2(ATerm el0, ATerm el1); */
#define ATmakeList2(el0, el1)           ATinsert(ATmakeList1(el1), el0)
#define ATmakeList3(el0, el1, el2)      ATinsert(ATmakeList2(el1,el2), el0)
#define ATmakeList4(el0, el1, el2, el3) ATinsert(ATmakeList3(el1,el2,el3), el0)
#define ATmakeList5(el0, el1, el2, el3, el4) \
                ATinsert(ATmakeList4(el1,el2,el3,el4), el0)
#define ATmakeList6(el0, el1, el2, el3, el4, el5) \
                ATinsert(ATmakeList5(el1,el2,el3,el4,el5), el0)

/*unsigned int ATgetLength(ATermList list);*/
unsigned int ATgetLength(ATermList list);

/* ATerm ATgetFirst(ATermList list);*/
#define   ATgetFirst(l) (((ATermList)l)->aterm.head)

/* ATermList ATgetNext(ATermList list);*/
#define   ATgetNext(l)  (((ATermList)l)->aterm.tail)

/*ATbool ATisEmpty(ATermList list);*/
#define ATisEmpty(list) ((ATbool)(((ATermList)list)->aterm.head == NULL \
				 && ((ATermList)list)->aterm.tail == NULL))

ATermList ATgetTail(ATermList list, int start);
ATermList ATreplaceTail(ATermList list, ATermList newtail, int start);
ATermList ATgetPrefix(ATermList list);
ATerm     ATgetLast(ATermList list);
ATermList ATgetSlice(ATermList list, unsigned int start, unsigned int end);
ATermList ATinsert(ATermList list, ATerm el);
ATermList ATinsertAt(ATermList list, ATerm el, unsigned int index);
ATermList ATappend(ATermList list, ATerm el);
ATermList ATconcat(ATermList list1, ATermList list2);
int       ATindexOf(ATermList list, ATerm el, int start);
int       ATlastIndexOf(ATermList list, ATerm el, int start);
ATerm     ATelementAt(ATermList list, unsigned int index);
ATermList ATremoveElement(ATermList list, ATerm el);
ATermList ATremoveElementAt(ATermList list, unsigned int idx);
ATermList ATremoveAll(ATermList list, ATerm el);
ATermList ATreplace(ATermList list, ATerm el, unsigned int idx);
ATermList ATreverse(ATermList list);
ATermList ATsort(ATermList list, int (*compare)(const ATerm t1, const ATerm t2));
int       ATcompare(ATerm t1, ATerm t2);
ATerm     ATdictCreate();
ATerm     ATdictGet(ATerm dict, ATerm key);
ATerm     ATdictPut(ATerm dict, ATerm key, ATerm value);
ATerm     ATdictRemove(ATerm dict, ATerm key);

ATermTable ATtableCreate(long initial_size, int max_load_pct);
void       ATtableDestroy(ATermTable table);
void       ATtableReset(ATermTable table);
void       ATtablePut(ATermTable table, ATerm key, ATerm value);
ATerm	   ATtableGet(ATermTable table, ATerm key);
void       ATtableRemove(ATermTable table, ATerm key);
ATermList  ATtableKeys(ATermTable table);
ATermList  ATtableValues(ATermTable table);

ATermIndexedSet   
           ATindexedSetCreate(long initial_size, int max_load_pct);
void       ATindexedSetDestroy(ATermIndexedSet set);
void       ATindexedSetReset(ATermIndexedSet set);
long       ATindexedSetPut(ATermIndexedSet set, ATerm elem, ATbool *isnew);
long       ATindexedSetGetIndex(ATermIndexedSet set, ATerm elem);
void       ATindexedSetRemove(ATermIndexedSet set, ATerm elem);
ATermList  ATindexedSetElements(ATermIndexedSet set);
ATerm      ATindexedSetGetElem(ATermIndexedSet set, long index);

/* Higher order functions */
ATermList ATfilter(ATermList list, ATbool (*predicate)(ATerm));

/* The ATermPlaceholder type */
ATermPlaceholder ATmakePlaceholder(ATerm type);
/*ATerm            ATgetPlaceholder(ATermPlaceholder ph);*/
#define ATgetPlaceholder(ph) (((ATermPlaceholder)ph)->aterm.ph_type)

/* The ATermBlob type */
ATermBlob ATmakeBlob(unsigned int size, void *data);
/*void   *ATgetBlobData(ATermBlob blob);*/
#define ATgetBlobData(blob) (((ATermBlob)blob)->aterm.data)

/*int     ATgetBlobSize(ATermBlob blob);*/
#define ATgetBlobSize(blob) (((ATermBlob)blob)->aterm.size)

void    ATregisterBlobDestructor(ATbool (*destructor)(ATermBlob));
void    ATunregisterBlobDestructor(ATbool (*destructor)(ATermBlob));

AFun  ATmakeAFun(const char *name, int arity, ATbool quoted);
#define ATmakeSymbol ATmakeAFun

/*char   *ATgetName(AFun sym);*/
#define ATgetName(sym) (at_lookup_table[(sym)]->name)
/*int     ATgetArity(AFun sym);*/
#define ATgetArity(sym) ((unsigned int)GET_LENGTH(at_lookup_table_alias[(sym)]->header))
/*ATbool  ATisQuoted(AFun sym);*/
#define ATisQuoted(sym) IS_QUOTED(at_lookup_table_alias[(sym)]->header)

void    ATprotectAFun(AFun sym);
#define ATprotectSymbol ATprotectAFun
void    ATunprotectAFun(AFun sym);
#define ATunprotectSymbol ATunprotectAFun
void ATprotectMemory(void *start, int size);
void ATunprotectMemory(void *start);

/* convenience macro's for previously private functions */
#define ATgetAnnotations(t) AT_getAnnotations( (t) )
#define ATsetAnnotations(t,a) AT_setAnnotations( (t), (a) )
#define ATremoveAnnotations(t) AT_removeAnnotations( (t) )

ATerm AT_getAnnotations(ATerm t);
ATerm AT_setAnnotations(ATerm t, ATerm annos);
ATerm AT_removeAnnotations(ATerm t);

/* Deep strip */
ATerm ATremoveAllAnnotations(ATerm t);

/* Calculate checksum using the
   "RSA Data Security, Inc. MD5 Message-Digest Algorithm" (see RFC1321)
*/
unsigned char *ATchecksum(ATerm t);
ATbool ATdiff(ATerm t1, ATerm t2, ATerm *templ, ATerm *diffs);

/* Compare two ATerms. This is a complete stable ordering on ATerms.
 * They are compared 'lexicographically', function names before the
 * arguments, terms before annotations. Function names are compared
 * using strcmp, integers and reals are compared
 * using integer and double comparison, blobs are compared using memcmp. 
 * If the types of the terms are different the integer value of ATgetType
 * is used.
 */
int ATcompare(ATerm t1, ATerm t2);

void ATsetChecking(ATbool on);
ATbool ATgetChecking(void);

/*int ATgetGCCount();*/
extern int at_gc_count;
#define ATgetGCCount()    (at_gc_count)

unsigned long  ATcalcUniqueSubterms(ATerm t);
unsigned long  ATcalcUniqueSymbols(ATerm t);

unsigned long  ATcalcTextSize(ATerm t);

void AT_writeToStringBuffer(ATerm t, char *buffer);
#define ATwriteToStringBuffer(t,b) AT_writeToStringBuffer((t),(b))

#define ATgetAFunId(afun) ((afun))

#ifdef __cplusplus
}
#endif/* __cplusplus */ 

#endif
