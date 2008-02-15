/*  Dynamic Vector Manager */

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "collect.h"

static void Collection_Panic(char *s)
{
  (void ) fprintf(stderr, "%s:Out of Memory", s);
  exit(1);
}

/* Create a Dynamic vector "Col",
   Each element is "elem_s" long,
   Initial size = "size",
   Each time it grows, it grows in "extend" elements
*/

void Collection_Init(PCollection Col, int elem_s, int size, int extend)
{
  size_t space;

  Col->el_size = elem_s;
  Col->n_ext   = extend;
  Col->size    = size;
  Col->el_free = 0;

  space         = elem_s*size;
  Col->data     = (char *) malloc(space);
  if (Col->data==NULL) Collection_Panic("Collection.init");
  memset(Col->data, 0, space);
}

/* Destroy a Dynamic Vector */

void Collection_Done(PCollection Col)
{
  Col->size=0;
  Col->el_free=0;
  free(Col->data);
}

/* Clean a Dynamic Vector */

void Collection_Clean(PCollection Col)
{
  Col->el_free=0;
}

static void Collection_Resize(PCollection Col)
{
  void *x;
  Col->size+=Col->n_ext;
  if ((x=realloc(Col->data, Col->el_size*Col->size))==NULL)
    Collection_Panic("Collection.resize");
  else Col->data = (char *) x;
}

int Collection_New(PCollection Col)
{
  if (Col->el_free == Col->size) Collection_Resize(Col);
  Col->el_free++;
  return (Col->el_free-1);
}

int Collection_Count(PCollection Col)
{
  return (Col->el_free);
}

void *Collection_At(PCollection Col, int pos)
{
  char *n;
  if (pos > Col->el_free) return NULL;
  n = Col->data;
  n += Col->el_size*pos;
  return n;
}

void Collection_Get(PCollection Col, int pos, void *data)
{
  char *n;
  if (pos > Col->el_free) Collection_Panic("Collection.get");
  n = Col->data;
  n += Col->el_size*pos;
  (void) memcpy(data, n, Col->el_size);
}

void Collection_Put(PCollection Col, int pos, void *data)
{
  char *n;
  if (pos > Col->el_free) Collection_Panic("Collection.put");
  n = Col->data;
  n += Col->el_size*pos;
  (void) memcpy(n, data, Col->el_size);
}

void Collection_ForEach(PCollection Col, Collection_Func fn)
{
  char *el_mem;
  int  count, el_pos;

  el_mem = Col->data;
  count  = Col->el_free;
  el_pos = 0;

  while(count--) {
    (*fn) (el_mem);
    el_mem += Col->el_size;
    el_pos++;
  }
}

void Collection_ForEachPos(PCollection Col, Collection_FuncPos fn)
{
  char *el_mem;
  int  count, el_pos;

  el_mem = Col->data;
  count  = Col->el_free;
  el_pos = 0;

  while(count--) {
    (*fn) (el_mem, el_pos);
    el_mem += Col->el_size;
    el_pos++;
  }
}

int Collection_FirstThat(PCollection Col, Collection_Comp fn, void *data)
{
  char *el_mem;
  int  count, el_pos;

  el_mem = Col->data;
  count  = Col->el_free;
  el_pos = 0;

  while (count--) {
    if ((*fn) (el_mem, data) != 0) return el_pos;
    el_mem += Col->el_size;
    el_pos++;
  }
  return -1;
}



