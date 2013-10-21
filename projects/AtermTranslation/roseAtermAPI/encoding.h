/**
  * encoding.h: Low level encoding of ATerm datatype.
  */

#ifndef ENCODING_H
#define ENCODING_H
#include "atypes.h"

#ifdef __cplusplus
extern "C"
{
#endif/* __cplusplus */

/*
 |--------------------------------|
 |info|type |arity|quoted|mark|age|
 |--------------------------------|
  31   9 8 7 6 5 4   3     2   1 0
*/

#define	MASK_QUOTED	(1<<3)
#define	MASK_ANNO	MASK_QUOTED
#define MASK_MARK	(1<<2)
#define MASK_ARITY	((1<<4) | (1<<5) | (1<<6))
#define MASK_TYPE	((1<<7) | (1<<8) | (1<<9))
#define MASK_AGE        ((1<<0) | (1<<1))

#define SHIFT_ARITY   4
#define SHIFT_TYPE    7

#define SHIFT_AGE     0
#define SHIFT_REMOVE_MARK_AGE 3
#define MASK_AGE_MARK   (MASK_AGE|MASK_MARK)

#if AT_64BIT
#define SHIFT_LENGTH  34
#define HEADER_BITS 64
typedef unsigned long header_type;
#else
#define SHIFT_LENGTH  10
#define HEADER_BITS 32
typedef unsigned int header_type;
#endif /* AT_64BIT */

#define LENGTH_BITS (HEADER_BITS - SHIFT_LENGTH)
#define MAX_LENGTH (1 << LENGTH_BITS)

#define GET_AGE(h)     ((unsigned int)(((h) & MASK_AGE) >> SHIFT_AGE))
#define SET_AGE(h, a)  ((h) = (((h) & ~MASK_AGE) | (((a) << SHIFT_AGE) & MASK_AGE)))

#define YOUNG_AGE 0
#define OLD_AGE   3

#define IS_YOUNG(h)      (!(IS_OLD(h)))
#define IS_OLD(h)        (GET_AGE(h) == OLD_AGE)

/* TODO: Optimize */
#define INCREMENT_AGE(h)    (SET_AGE(h, ((GET_AGE(h) < OLD_AGE) ? (GET_AGE(h) + 1) : (GET_AGE(h)))))

#define HIDE_AGE_MARK(h)    ((h) & ~MASK_AGE_MARK)
/* #define EQUAL_HEADER(h1,h2) (HIDE_AGE_MARK(h1)==HIDE_AGE_MARK(h2)) */
#define EQUAL_HEADER(h1,h2) (HIDE_AGE_MARK(h1^h2) == 0)

#define SHIFT_SYMBOL  SHIFT_LENGTH
#define SHIFT_SYM_ARITY SHIFT_LENGTH

#define TERM_SIZE_APPL(arity) ((sizeof(struct __ATerm)/sizeof(header_type))+arity)
#define TERM_SIZE_INT         (sizeof(struct __ATermInt)/sizeof(header_type))
#define TERM_SIZE_REAL        (sizeof(struct __ATermReal)/sizeof(header_type))
#define TERM_SIZE_BLOB        (sizeof(struct __ATermBlob)/sizeof(header_type))
#define TERM_SIZE_LIST        (sizeof(struct __ATermList)/sizeof(header_type))
#define TERM_SIZE_PLACEHOLDER (sizeof(struct __ATermPlaceholder)/sizeof(header_type))
#define TERM_SIZE_SYMBOL      (sizeof(struct _SymEntry)/sizeof(header_type))

#define IS_MARKED(h)    ((h) & MASK_MARK)
#define GET_TYPE(h)     (((h) & MASK_TYPE) >> SHIFT_TYPE)
#define HAS_ANNO(h)     ((h) & MASK_ANNO)
#define GET_ARITY(h)	((unsigned int)(((h) & MASK_ARITY) >> SHIFT_ARITY))
#define GET_SYMBOL(h)	((Symbol) ((h) >> SHIFT_SYMBOL))
#define GET_LENGTH(h)	((h) >> SHIFT_LENGTH)
#define IS_QUOTED(h)	(((h) & MASK_QUOTED) ? ATtrue : ATfalse)

#define SET_MARK(h)     ((h) |= MASK_MARK)
#define SET_ANNO(h)     ((h) |= MASK_ANNO)
/* #define SET_ARITY(h, ar) ((h) = (((h) & ~MASK_ARITY) | \
									((ar) << SHIFT_ARITY)))
*/
#define SET_SYMBOL(h, sym)	((h) = (((h) & ~MASK_SYMBOL) | \
									((sym) << SHIFT_SYMBOL)))
#define SET_LENGTH(h, len)  ((h) = (((h) & ~MASK_LENGTH) | \
									((len) << SHIFT_LENGTH)))
#define SET_QUOTED(h)		((h) |= MASK_QUOTED)

#define CLR_MARK(h)			((h) &= ~MASK_MARK)
#define CLR_ANNO(h)			((h) &= ~MASK_ANNO)
#define CLR_QUOTED(h)		((h) &= ~MASK_QUOTED)

#define APPL_HEADER(anno,ari,sym) ((anno) | ((ari) << SHIFT_ARITY) | \
				   (AT_APPL << SHIFT_TYPE) | \
				   ((header_type)(sym) << SHIFT_SYMBOL))
#define INT_HEADER(anno)          ((anno) | AT_INT << SHIFT_TYPE)
#define REAL_HEADER(anno)         ((anno) | AT_REAL << SHIFT_TYPE)
#define EMPTY_HEADER(anno)        ((anno) | AT_LIST << SHIFT_TYPE)

#define LIST_HEADER(anno,len)     ((anno) | (AT_LIST << SHIFT_TYPE) | \
				   ((MachineWord)(len) << SHIFT_LENGTH) | (2 << SHIFT_ARITY))

#define PLACEHOLDER_HEADER(anno)  ((anno) | (AT_PLACEHOLDER << SHIFT_TYPE) | \
           1 << SHIFT_ARITY)

/*
#define BLOB_HEADER(anno,len)     ((anno) | (AT_BLOB << SHIFT_TYPE) | \
				   ((MachineWord)(len) << SHIFT_LENGTH))
				   */
#define BLOB_HEADER(anno)	  ((anno) | (AT_BLOB << SHIFT_TYPE) | \
				   (2 << SHIFT_ARITY))

#define SYMBOL_HEADER(arity,quoted) \
	(((quoted) ? MASK_QUOTED : 0) | \
	 ((MachineWord)(arity) << SHIFT_SYM_ARITY) | \
	 (AT_SYMBOL << SHIFT_TYPE))

#define FREE_HEADER               (AT_FREE << SHIFT_TYPE)

#define ARG_OFFSET                TERM_SIZE_APPL(0)



#define MAX_HEADER_BITS 64

#define AT_TABLE_SIZE(table_class)  (1<<(table_class))
#define AT_TABLE_MASK(table_class)  (AT_TABLE_SIZE(table_class)-1)

#define MAX_ARITY             MAX_LENGTH

#define MIN_TERM_SIZE         TERM_SIZE_APPL(0)
#define INITIAL_MAX_TERM_SIZE 256


struct __ATerm
{
  header_type   header;
  union _ATerm *next;
};

typedef union _ATerm
{
  header_type     header;
  struct __ATerm  aterm;
  union _ATerm*   subaterm[1];
  MachineWord     word[1];
} *ATerm;

typedef void (*ATermProtFunc)();

#ifdef __cplusplus
}
#endif/* __cplusplus */ 

#endif
