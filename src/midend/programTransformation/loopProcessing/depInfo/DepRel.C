

#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <assert.h>

#include <DepRel.h>

#define ALIGN_ALL 32767
#define MAX_ALIGN 3
#define MIN_ALIGN -3

void DepRel::Reset( DepDirType t, int min, int max)
{
  align = min;
  int size = max - min +1;
  switch (t) {
  case DEPDIR_NONE:
  case DEPDIR_ALL:
    align = 0; size = 1; break;
  case DEPDIR_EQ:
    if (min < MIN_ALIGN && max > MAX_ALIGN) {
      t = DEPDIR_ALL;
      align = 0; size = 1;
    }
    else if (min < MIN_ALIGN) {
      align = max;
      size = 1;
      t = DEPDIR_LE;
    }
    else if (max > MAX_ALIGN) {
      align = min; size = 1;
      t = DEPDIR_GE;
    }
    break;
  case DEPDIR_LE:
    if (max > MAX_ALIGN) {
      t = DEPDIR_ALL;
      align = 0; size = 1;
    }
    else {
      align = max;
      size = 1;
    }
    break;
  case DEPDIR_GE:
    if (min < MIN_ALIGN) {
      t = DEPDIR_ALL;
      align = 0; size = 1;
    }
    else {
      align = min;
      size = 1;
    }
    break;
  case DEPDIR_NE:
    if (min < MIN_ALIGN || max > MAX_ALIGN) {
      t = DEPDIR_ALL;
      align = 0; size = 1;
    }
    break;
  default:
    assert(false);
  }
  entryType = t | (size << 3);
}

DepDirType GetRelType( int entryType ) 
{
  return static_cast <DepDirType>(entryType & DEPDIR_ALL);
}

int AlignRangeSize( int entryType)
{
  return entryType >> 3;
}

DepDirType  DepRel :: GetDirType() const
{
  DepDirType t = GetRelType(entryType);
  switch (t) {
  case DEPDIR_EQ: return DEPDIR_EQ;
  case DEPDIR_LE: return DEPDIR_LE;
  case DEPDIR_GE: return DEPDIR_GE;
  case DEPDIR_NONE: return DEPDIR_NONE;
  case DEPDIR_ALL: 
  case DEPDIR_NE:  return DEPDIR_ALL;
  default:
      assert(false); 
  }
  return DEPDIR_NONE;
}
        
int DepRel :: GetMinAlign() const
{
  DepDirType t = GetRelType( entryType ); 
  switch (t) {
  case DEPDIR_ALL: 
  case DEPDIR_NE:
  case DEPDIR_LE: 
    return NEG_INFTY;
  case DEPDIR_NONE: 
    return POS_INFTY;
  case DEPDIR_EQ:
  case DEPDIR_GE:
    return align;
  default:
    assert(false);
  }

  /* Avoid MSVC warning */
     return -1;
}

int DepRel :: GetMaxAlign() const
{
  int size = AlignRangeSize( entryType );
  DepDirType t = GetRelType( entryType);
  switch (t) {
  case DEPDIR_ALL: 
  case DEPDIR_NE:
  case DEPDIR_GE:
    return POS_INFTY;
  case DEPDIR_NONE: 
    return NEG_INFTY;
  case DEPDIR_EQ:
  case DEPDIR_LE:
    return align + size - 1;
  default:
    assert(false);
  }
  /* Avoid MSVC warning */
     return -1;
}

bool DepRel :: IsTop() const
{
  return GetRelType( entryType ) == DEPDIR_NONE;
}

bool DepRel :: IsBottom() const
{
  return GetRelType( entryType ) == DEPDIR_ALL;
}

std:: string DepRel :: toString() const 
{
  std:: string res;
  switch ( GetRelType( entryType)) {
  case DEPDIR_NONE:
    res = res + "(); "; 
    break;
  case DEPDIR_ALL:
    res = res + "* ";
    break;
  case DEPDIR_EQ :
    res = res + "== ";
    break;
  case DEPDIR_LE :
    res = res + "<= ";
    break;
  case DEPDIR_GE :
    res = res + ">= ";
    break;
  case DEPDIR_NE :
    res = res + " != ";
    break;
  }
  char buf[20];
  if (align != ALIGN_ALL) {
    sprintf(buf, "%d", align);
    res = res + std:: string(buf);
  }
  int s = AlignRangeSize( entryType );
  if ( s > 1)  {
    sprintf(buf, ",%d", s);
    res = res + std:: string(buf);
  }
  return res;
}

bool LessThan( const DepRel &e1, const DepRel &e2)
{
  if ( e1.IsTop() || e2.IsBottom())
    return true;
  if (e1.IsBottom() || e2.IsTop())
    return false;

  DepDirType t1 = e1.GetDirType(), t2 = e2.GetDirType();
  int a1 = e1.GetMinAlign(), a2 = e2.GetMinAlign();
  int b1 = e1.GetMaxAlign(), b2 = e2.GetMaxAlign();

  DepDirType t = static_cast <DepDirType>(t1 | t2);
  if (t != t2)
    return false;
  if (t == DEPDIR_EQ && a1 >= a2 && b1 <= b2)
    return true;
  else if (t == DEPDIR_LE && b1 <= b2 )
    return true;
  else if (t == DEPDIR_GE && a1 >= a2)
    return true;
  else if (t == DEPDIR_ALL)
     return true;
  else
    return false;
}

bool DepRel::UnionUpdate (DepRel& e1, const DepRel &e2)
{
  DepDirType t1 = e1.GetDirType(), t2 = e2.GetDirType();
  DepDirType t = static_cast <DepDirType>(t1 | t2);
  int a1 = e1.GetMinAlign(), a2 = e2.GetMinAlign();
  int b1 = e1.GetMaxAlign(), b2 = e2.GetMaxAlign();
  int min = (a1<a2)?a1: a2, max = (b1<b2)?b2: b1;
  e1.Reset( t, min, max);
  return t != t1 || a1 != min || b1 != max;
}

DepRel operator | (const DepRel &e1, const DepRel &e2)
{
  DepRel result(e1);
  result |= e2;
  return result;
}

bool DepRel::IntersectUpdate (DepRel& e1, const DepRel &e2)
{
  DepDirType t1 = e1.GetDirType(), t2 = e2.GetDirType();
  DepDirType t = static_cast <DepDirType>(t1 & t2);
  int a1 = e1.GetMinAlign(), a2 = e2.GetMinAlign();
  int b1 = e1.GetMaxAlign(), b2 = e2.GetMaxAlign();
  int min = (a1<a2)?a2 : a1, max = (b1<b2)?b1: b2;

  switch (t) {
  case DEPDIR_NONE: 
  case DEPDIR_LE:  
  case DEPDIR_GE:  
  case DEPDIR_ALL:
    e1.Reset(t, min, max);
    break;
  case DEPDIR_NE: 
    if (min < max)
      e1.Reset(t, min, max);
    break;
  case DEPDIR_EQ:
    if (max < min)
      e1.Reset(DEPDIR_NONE, 0, 0);
    else
      e1.Reset(t, min, max);
    break;
  default:
    assert(false);
  }
  return t1 != e1.GetDirType() || a1 != e1.GetMinAlign() || b1 != e1.GetMaxAlign();
}

DepRel operator & (const DepRel &e1, const DepRel &e2)
{
  DepRel result(e1);
  result &= e2;
  return result;
}

DepRel operator * (const DepRel &e1, const DepRel &e2)
{
  if (e1.IsTop())
    return e1;
  else if (e2.IsTop())
    return e2;

  DepDirType t1 = e1.GetDirType(), t2 = e2.GetDirType();
  DepDirType t = static_cast <DepDirType>(t1 | t2);
  int a1 = e1.GetMinAlign(), a2 = e2.GetMinAlign();
  int b1 = e1.GetMaxAlign(), b2 = e2.GetMaxAlign();
 
  return DepRel(t, AddConst(a1,a2), AddConst(b1, b2));
} 

bool DepRel:: Closure()
{
  DepDirType t = GetDirType();
  int lo = GetMinAlign(), hi = GetMaxAlign();
  if (t == DEPDIR_EQ) {
    if (lo == hi && lo == 0)
      return false;
    else if (lo >= 0) 
      Reset(DEPDIR_GE, lo, lo);
    else if (hi <= 0)
      Reset(DEPDIR_LE, hi, hi);
    else 
      Reset(DEPDIR_ALL, 0, 0);
    return true;
  }
  else if (t == DEPDIR_NE || (t == DEPDIR_LE && hi > 0) || 
           (t == DEPDIR_GE && lo < 0) ) {
    Reset(DEPDIR_ALL, 0, 0);
    return true;
  }
  else
    return false;
}

DepRel Closure( const DepRel &e)
{
   DepRel result(e);
   result.Closure();
   return result;
}
DepRel Reverse( const DepRel &e)
{
  DepDirType t = e.GetDirType();
  int lo = e.GetMinAlign(), hi = e.GetMaxAlign();
  switch (t) {
  case DEPDIR_EQ:
    return DepRel(DEPDIR_EQ, -hi, -lo);
  case DEPDIR_NE:
    return DepRel(DEPDIR_NE, -hi, -lo);
  case DEPDIR_LE:
    return DepRel(DEPDIR_GE, -hi, -lo);
  case DEPDIR_GE:
    return DepRel(DEPDIR_LE, -hi, -lo);
  case DEPDIR_NONE:
  case DEPDIR_ALL:
    return e;
  default:
          { assert(false); /* Avoid MSVC warning */ return e; }
  }
}

DepRel operator - (const DepRel &e)
{
  DepDirType t = e.GetDirType();
  int lo = e.GetMinAlign(), hi = e.GetMaxAlign();

  switch (t) {
  case DEPDIR_NONE:
    return DepRel(DEPDIR_ALL);
  case DEPDIR_ALL:
    return DepRel(DEPDIR_NONE);
  case DEPDIR_EQ:
    return DepRel(DEPDIR_NE, lo, hi);
  case DEPDIR_LE:
    return DepRel(DEPDIR_GE, hi+1);
  case DEPDIR_GE:
    return DepRel(DEPDIR_LE, lo-1);
  case DEPDIR_NE:
    return DepRel(DEPDIR_EQ, lo, hi);
  default:
          { assert(false); /* Avoid MSVC warning */ return e; }
  }
}

DepRel IncreaseAlign( const DepRel &r, int a) 
{
  DepRel result(r);
  result.IncreaseAlign(a);
  return result;
}

