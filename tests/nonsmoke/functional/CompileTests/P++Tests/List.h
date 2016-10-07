#ifndef _List_h
#define _List_h

#include "bsparti.h"

typedef void Gtype; /* generic type */
typedef int (*pfunc)(); /* pointer to function (for EQ functions) */

typedef struct _thing_cell { /* a link in a list of the things in the proc */
  Gtype* item;
  struct _thing_cell* next;
  struct _thing_cell* prev;
  int referenceCount; /* Code added by Dan Quinlan */
} Cell;

typedef struct _list {
  Cell* dummy;                  /* ptr to the dummy cell */
  Cell* fptr;                   /* scratch ptr used by FOREACH */
  Cell* optr;                   /* scratch ptr used by NEXT_ITEM */
  int referenceCount;           /* Code added by Dan Quinlan */
} *List;


/* Added by Dan Quinlan */
void delete_Cell   ( Cell*   X );

/*
 * These macros can be used as follows
 *    Cell *thing;
 *    List l;
 *       for (thing = FIRST(l); STILL_IN(thing, l); thing = NEXT(thing)) {
 *          ...
 *       }
 *    }
 *
 *   Gtype* item;
 *   FOREACH(item, l) { ...just like a for loop...  }
 *
 *   item = FIRST_ITEM(l)
 *   while (item) { ... just like a while loop...;  item = NEXT_ITEM(l); }
 *
 *   item = LAST_ITEM(l)
 *   while (item) { ... backwards too! ...; item = PREV_ITEM(l); }
 */

#define FIRST(l)          (l->dummy->next)
#define FIRST_ITEM(l)     (l->optr = FIRST(l), l->optr->item)
#define LAST(l)           (l->dummy->prev)
#define LAST_ITEM(l)      (l->optr = LAST(l), l->optr->item)
#define EMPTY(l)          (l->dummy == FIRST(l))
#define STILL_IN(t, l)    (t != l->dummy)
#define NEXT(l)           (l->next)
#define NEXT_ITEM(l)      (l->optr = NEXT(l->optr), l->optr->item)
#define PREV(l)           (l->prev)
#define PREV_ITEM(l)      (l->optr = PREV(l->optr), l->optr->item)

#define FOREACH(i,l) for(l->fptr = FIRST(l), i = l->fptr->item; \
                         STILL_IN(l->fptr, l); \
                         l->fptr = NEXT(l->fptr), i = l->fptr->item)

#define FOREACHt(i,l,type) for(l->fptr = FIRST(l), i = (type) l->fptr->item; \
                         STILL_IN(l->fptr, l); \
                         l->fptr = NEXT(l->fptr), i = (type) l->fptr->item)

List create_List();                     /* create an empty List */
void int_destroy_List P_((List));       /* destroy a List */
void insert_List P_((Gtype*, List));    /* insert an element into a List */
Gtype* remove_List P_((Gtype*, List));  /* remove an item from a List */
void remove_List_List P_((List, List)); /* remove elements of l1 from
                                           list l2 */
Gtype* member_List P_((Gtype*, List));  /* membership test for Lists */
Gtype* eq_member_List P_((Gtype*, List, pfunc)); /* membership with EQ func */
int length_List P_((List));             /* number of elements in a List */
List dup_List P_((List));               /* duplicate a list */
void append_List P_((List, List));      /* l1 <-- l1 + l2 */      
List intersection_List P_((List, List));/* intersection of two lists */
List union_List P_((List, List));       /* union of two lists */
void replace_List P_((Gtype*,Gtype*,List)); /* change an item in a list */

#define head_List(l) remove_List((void*) 0, l)
#define destroy_List(l) (int_destroy_List(l), l = 0)

#endif /* _List_h */
