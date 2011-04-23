
#ifndef DEP_REL_INFO
#define DEP_REL_INFO

#include <const.h>
#include <stdlib.h>
#include <iostream>
#include <string>

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
  int GetAlign() const {return align; };
  std:: string toString() const;
  void Dump() const { std:: cerr << toString() << std:: endl; }
  bool IsTop() const;
  bool IsBottom() const;

  bool IncreaseAlign(int inc)
     { int tmp = align; align = AddConst(align, inc); return tmp != align; }
  bool Closure();
  bool operator == (const DepRel &that) const
    { return entryType == that.entryType && align == that.align; }
  bool operator != (const DepRel &that) const
    { return ! operator ==(that); }

  static std::string toString(int entry_type);
  static bool IntersectUpdate(DepRel& e1, const DepRel &that);
  static bool UnionUpdate(DepRel& e1, const DepRel &that);
};

inline bool operator &= (DepRel& e1, const DepRel &that)
   { return DepRel::IntersectUpdate(e1,that); }
inline bool operator |= (DepRel& e1, const DepRel &that)
  { return DepRel::UnionUpdate(e1, that); }

bool LessThan( const DepRel &e1, const DepRel &e2);
inline bool operator <= ( const DepRel &e1, const DepRel &e2)
  { return LessThan(e1, e2); }
inline bool operator < ( const DepRel &e1, const DepRel &e2)
  { return e1 <= e2 && e1 != e2; }
DepRel operator & ( const DepRel &e1, const DepRel &e2);
DepRel operator | ( const DepRel &e1, const DepRel &e2);
DepRel operator * ( const DepRel &e1, const DepRel &e2);
DepRel Reverse( const DepRel &e);
DepRel Closure( const DepRel &e);
DepRel operator - ( const DepRel &e);
DepRel IncreaseAlign( const DepRel &r, int a) ;


#endif



