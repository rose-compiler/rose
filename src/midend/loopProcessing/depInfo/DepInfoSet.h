

#ifndef DEP_INFO_LIST_H
#define DEP_INFO_LIST_H

#include <DepInfo.h>
#include <CountRefHandle.h>
#include <LatticeElemList.h>

class DepInfoSetImpl : public LatticeElemList <DepInfo>
{
  void operator =(const DepInfoSetImpl& that) {}
 public:
  DepInfoSetImpl() {}
  DepInfoSetImpl( const DepInfoSetImpl &that)
    : LatticeElemList <DepInfo>(that) {}
  virtual ~DepInfoSetImpl() {}
  DepInfoSetImpl* Clone() const { return new DepInfoSetImpl(*this); }
 friend class CountRefHandle<DepInfoSetImpl>;
};

class DepInfoSet : protected CountRefHandle <DepInfoSetImpl>
{
 public:
   DepInfoSet();
   int NumOfDeps() const ;
   DepRel GetDepRel( int index1, int index2 ) const;

// Boolean IsTop() const;
   int IsTop() const;
// Boolean IsBottom(int commLevel = 0) const;
   int IsBottom(int commLevel = 0) const;
   std::string ToString() const;

// Boolean AddDep( const DepInfo &c);
   int AddDep( const DepInfo &c);
// Boolean operator |= (const DepInfoSet &that);
   int operator |= (const DepInfoSet &that);
// Boolean operator &= (const DepInfoSet &that);
   int operator &= (const DepInfoSet &that);
 
// void UpdateDepInfo( Boolean (*Update)(DepInfo &info) );
   void UpdateDepInfo( int (*Update)(DepInfo &info) );
  
   DepInfoConstIterator GetConstIterator() const;
   DepInfoUpdateIterator GetUpdateIterator();

  friend class DepInfoSetGenerator;
};

class DepInfoSetGenerator
{
 public:
  static DepInfoSet GetTopInfoSet();
  static DepInfoSet GetBottomInfoSet(int nr, int nc, int commLevel = 0);
  static DepInfoSet GetIDInfoSet( int nr);
};
 
DepInfoSet  operator & ( const DepInfoSet &d1, const DepInfoSet &d2) ;
DepInfoSet  operator | ( const DepInfoSet &d1, const DepInfoSet &d2) ;
DepInfoSet  operator * ( const DepInfoSet &d1, const DepInfoSet &d2) ;
DepInfoSet Closure( DepInfoSet d);

#endif
