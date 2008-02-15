
#ifndef DEP_REL_INFO
#define DEP_REL_INFO

#include <const.h>
#include <stdlib.h>
#include <iostream>

typedef enum {DEPDIR_NONE=0, DEPDIR_EQ=1, DEPDIR_LE=3, DEPDIR_GE=5,
              DEPDIR_NE=6, DEPDIR_ALL=7} DepDirType;

class DepRel 
{
  int entryType;
  int align;

  DepRel* operator & () { return 0; }
 public:
  DepRel(DepDirType t = DEPDIR_ALL, int a = 0)
    { Reset( t, a, a); }
  DepRel( DepDirType t, int min, int max)
    { Reset( t, min, max); }
  DepRel(const DepRel& that) : entryType(that.entryType), align(that.align) {}
  DepRel& operator = (const DepRel& that)
    { entryType = that.entryType; align = that.align; return *this; }

  void Reset(DepDirType t, int minalign, int maxalign);
  DepDirType GetDirType() const;
  int GetMinAlign() const;
  int GetMaxAlign() const;
  std::string ToString() const;
  void Dump() const { std::cerr << ToString() << std::endl; }
//Boolean IsTop() const;
  int IsTop() const;
//Boolean IsBottom() const;
  int IsBottom() const;

//Boolean IncreaseAlign(int inc)
  int IncreaseAlign(int inc)
     { int tmp = align; align = AddConst(align, inc); return tmp != align; }
//Boolean Closure();
  int Closure();
//Boolean operator == (const DepRel &that) const
  int operator == (const DepRel &that) const
    { return entryType == that.entryType && align == that.align; }
//Boolean operator != (const DepRel &that) const
  int operator != (const DepRel &that) const
    { return ! operator ==(that); }

//static Boolean IntersectUpdate(DepRel& e1, const DepRel &that);
  static int IntersectUpdate(DepRel& e1, const DepRel &that);
//static Boolean UnionUpdate(DepRel& e1, const DepRel &that);
  static int UnionUpdate(DepRel& e1, const DepRel &that);
};

// DQ (1/2/2006): Updated these two function with 
// correct code using return statement from Qing.
// inline Boolean operator &= (DepRel& e1, const DepRel &that)
inline int operator &= (DepRel& e1, const DepRel &that)
   { return DepRel::IntersectUpdate(e1,that); }
// inline Boolean operator |= (DepRel& e1, const DepRel &that)
inline int operator |= (DepRel& e1, const DepRel &that)
   { return DepRel::UnionUpdate(e1, that); }


// Boolean operator <= ( const DepRel &e1, const DepRel &e2);
int operator <= ( const DepRel &e1, const DepRel &e2);
// inline Boolean operator < ( const DepRel &e1, const DepRel &e2)
inline int operator < ( const DepRel &e1, const DepRel &e2)
  { return e1 <= e2 && e1 != e2; }
DepRel operator & ( const DepRel &e1, const DepRel &e2);
DepRel operator | ( const DepRel &e1, const DepRel &e2);
DepRel operator * ( const DepRel &e1, const DepRel &e2);
DepRel Reverse( const DepRel &e);
DepRel Closure( const DepRel &e);
DepRel operator - ( const DepRel &e);
DepRel IncreaseAlign( const DepRel &r, int a) ;


#endif



