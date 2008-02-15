/*  Dynamic Vector Manager */

#include <stdio.h>
#include <stdlib.h>
#include "set.h"

static void Set_Panic(char *s)
{
  fprintf(stderr, "%s:Out of Memory", s);
  exit(1);
}

/* Create a Dynamic Bit vector "Set" */

void Set_Init(PSet set)
{
  set->size = 1;
  set->data = (unsigned short *) malloc(sizeof(unsigned short int));
  if (set->data == NULL) Set_Panic("Set.init");
  set->data[0] = 0;
}

/* Destroy a Dynamic Bit Vector */

void Set_Done(PSet set)
{
  set->size = 0;
  free(set->data);
}

/* Clean a Dynamic Bit Vector */

void Set_Clean(PSet set)
{
  int i;
  for (i = 0; i < set->size; i++) set->data[i] = 0;
}


static void Set_Resize(PSet set, int n)
{
  void *x;
  int i;

  if (n <= set->size) return;
  if ((x = realloc(set->data, n*sizeof(unsigned short int))) == NULL)
    Set_Panic("Set.resize");
  else set->data = (unsigned short *) x;
  for (i = set->size; i < n; i++) set->data[i] = 0;
  set->size = n;
}

void Set_AddItem(PSet set, int n)
{
  int p, b;

  p = n / SET_NBITS;
  b = n % SET_NBITS;
  if (p+1 > set->size) Set_Resize(set, p+1);
  set->data[p] |= (unsigned short int) 1 << b;
}

void Set_DelItem(PSet set, int n)
{
  int p, b;

  p = n / SET_NBITS;
  b = n % SET_NBITS;
  if (p >= set->size) return;
  set->data[p] &= ~((unsigned short int) 1 << b);
}

int Set_IsItem(PSet set, int n)
{
  int p, b;

  p = n / SET_NBITS;
  b = n % SET_NBITS;
  if (p >= set->size) return 0;
  return (set->data[p] & ((unsigned short int) 1 << b));
}

int Set_Empty(PSet set)
{
  int i;
  for (i = 0; i < set->size; i++)
    if (set->data[i] != 0) return 0;
  return 1;
}

int Set_MaxIndex(PSet set)
{
  int i, c;
  i = set->size - 1;
  while (i >= 0) {
    if (set->data[i] != 0) break;
    i--;
  }
  if (i < 0) return -1;
  c = (i+1) * SET_NBITS;
  while (c >= 0 && !Set_IsItem(set, c)) c--;
  return c;
}

int Set_MinIndex(PSet set)
{
  int i, c;
  c = set->size - 1;
  i = 0;
  while (i <= c) {
    if (set->data[i] != 0) break;
    i++;
  }
  if (i>c) return -1;
  c = i * SET_NBITS;
  i = c + SET_NBITS;
  while (c <= i && !Set_IsItem(set, c)) c++;
  return c;
}

void Set_GetRange(PSet set, int *s, int *f)
{
  *s = Set_MinIndex(set);
  *f = Set_MaxIndex(set);
}

int Set_Elements(PSet set)
{
  int s, f, c = 0;
  Set_GetRange(set, &s, &f);
  for( ; s <= f; s++)
    if (Set_IsItem(set, s)) c++;
  return c;
}

void Set_ForEach(PSet set, Set_Func fn)
{
  int i, c;
  Set_GetRange(set, &i, &c);
  for ( ; i <= c; i++)
    if (Set_IsItem(set, i))
      (*fn) (i);
}

void Set_Union(PSet set1, PSet set2)
{
  int i, x;

  x = set2->size;
  Set_Resize(set1, x);
  for (i = 0; i < x; i++)
    set1->data[i] |= set2->data[i];
}

void Set_Diference(PSet set1, PSet set2)
{
  int i, x;
  x = set1->size;
  if (set2->size < x) x = set2->size;
  for (i = 0; i < x; i++)
    set1->data[i] &= ~set2->data[i];
}

void Set_Intersect(PSet set1, PSet set2)
{
  int i, x;
  x = set1->size;
  if (set2->size < x) x = set2->size;
  for (i = 0; i < x; i++)
    set1->data[i] &= set2->data[i];
  for (i = x; i < set1->size; i++)
    set1->data[i] = 0;
}

void Set_AddRange(PSet set, int start, int end)
{
  int i;
  for (i = start; i <= end; i++)
    Set_AddItem(set, i);
}

void Set_DelRange(PSet set, int start, int end)
{
  int i;
  for (i = start; i <= end; i++)
    Set_DelItem(set, i);
}

void Set_Copy(PSet set1, PSet set2)
{
  Set_Clean(set1);
  Set_Union(set1, set2);
}

int Set_Equal(PSet set1, PSet set2)
{
  int i, s1, s2;
  s1 = set1->size - 1;
  while (s1 > 0 && set1->data[s1] == 0) s1--;
  s2 = set2->size - 1;
  while (s2 > 0 && set2->data[s2] == 0) s2--;
  if (s1 != s2) return 0;

  for (i = 0; i <= s1; i++)
    if (set1->data[i] != set2->data[i]) return 0;
  return 1;
}

int Set_Diferent(PSet set1, PSet set2)
{
  int i, s1, s2;
  s1 = set1->size - 1;
  while (s1 > 0 && set1->data[s1] == 0) s1--;
  s2 = set2->size - 1;
  while (s2 > 0 && set2->data[s2] == 0) s2--;

  if (s1 > s2) s1 = s2;  /* Little Set */
  for (i = 0; i <= s1; i++)
    if (set1->data[i] & set2->data[i]) return 0;
  return 1;
}

int Set_Includes(PSet set1, PSet set2)
{
  int i, s1, s2;
  s1 = set1->size - 1;
  while (s1 > 0 && set1->data[s1]==0) s1--;
  s2 = set2->size - 1;
  while (s2 > 0 && set2->data[s2]==0) s2--;
  if (s2 > s1) return 0;
  for (i = 0; i <= s2; i++)
    if ((set1->data[i] & set2->data[i]) != set2->data[i]) return 0;
  return 1;
}

void Set_PrintInt(PSet set)
{
  int i, c;
  Set_GetRange(set, &i, &c);
  for ( ; i <= c; i++)
    if (Set_IsItem(set, i))
      printf("%d ",i);
}

void Set_PrintChar(PSet set)
{
  int i, c;
  Set_GetRange(set, &i, &c);
  for ( ; i <= c; i++)
    if (Set_IsItem(set, i))
      printf("%c",i);
}


