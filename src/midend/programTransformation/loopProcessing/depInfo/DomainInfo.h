
#ifndef DOMAIN_DEP_INTERFACE
#define DOMAIN_DEP_INTERFACE

#include <DepInfo.h>
#include <LatticeElemList.h>

class DomainCond :  protected DepInfo 
{
 public:
  DomainCond( int loops);
  DomainCond() : DepInfo() {}
  DomainCond (const DomainCond &that) : DepInfo(that) {}
  DomainCond( DepInfo &dep, DepDirection dir);
  DomainCond& operator = ( const DomainCond &that)
      { DepInfo :: operator =(that); return *this; }

  int NumOfLoops() const { return DepInfo::rows(); }
  bool IsTop() const;
  bool IsBottom() const;
  std::string toString() const;

  const DepRel& Entry(int index1, int index2) const
    { return DepInfo::Entry(index1, index2); }
  DepRel& Entry(int index1, int index2)
    { return DepInfo::Entry(index1, index2); }
  bool ClosureCond();
  void SetLoopRel(int index1, int index2, const DepRel &r) ;
  void Initialize( const DepRel &r);

  bool operator < (const DomainCond &that) const
    { return DepInfo :: operator < (that); }
  bool operator <= (const DomainCond &that) const
    { return DepInfo :: operator <= (that); }
  bool operator |= (const DomainCond &that);
  bool operator &= (const DomainCond &that);

  bool operator ==( const DomainCond &that) const
     { return DepInfo::operator==(that); }
  bool operator != (const DomainCond &that) const
     { return ! operator ==(that); }

  void InsertLoop( int level);
  void RemoveLoop( int level);
  void RestrictDepInfo( DepInfo &dep, DepDirection dir) const;
  void MergeLoop( int index1, int index2);
  void SwapLoop( int index1, int index2);
  void AlignLoop( int index1, int align);
};
typedef IteratorImpl<DomainCond> DomainCondConstIteratorImpl;
typedef IteratorImpl<DomainCond&> DomainCondUpdateIteratorImpl;
typedef IteratorWrap<DomainCond,DomainCondConstIteratorImpl> DomainCondConstIterator;
typedef IteratorWrap<DomainCond&, DomainCondUpdateIteratorImpl> DomainCondUpdateIterator;

DomainCond operator | ( const DomainCond &d1, const DomainCond &d2) ;
DomainCond operator & ( const DomainCond &d1, const DomainCond &d2) ;

class DomainInfoImpl : public LatticeElemList <DomainCond>
{
  void operator =(const DomainInfoImpl& that) {}
 public:
  DomainInfoImpl() {}
  DomainInfoImpl( const DomainInfoImpl &that)
    : LatticeElemList <DomainCond>(that) {}
  virtual ~DomainInfoImpl() {}
  DomainInfoImpl* Clone() const { return new DomainInfoImpl(*this); }
 friend class CountRefHandle<DomainInfoImpl>;
};

class DomainInfo : protected CountRefHandle <DomainInfoImpl>
{
 public:
   DomainInfo();
   DomainInfo(DomainCond c) ;

   int NumOfConds() const;
   int NumOfLoops() const;
   DepRel GetLoopRel( int i1, int i2) const;

   bool AddCond( const DomainCond &c) ;
   bool operator |= (const DomainInfo &that);
   void UpdateDomainCond( bool (*Update)(DomainCond &info) );

   bool IsTop() const;
   bool IsBottom() const;
   std::string toString() const;

   DomainCondConstIterator GetConstIterator() const;
   DomainCondUpdateIterator GetUpdateIterator();
};

DomainInfo  operator & ( const DomainInfo &d1, const DomainInfo &d2) ;
DomainInfo  operator | ( const DomainInfo &d1, const DomainInfo &d2) ;
DomainInfo  operator - ( const DomainInfo &d1, const DomainInfo &d2) ;
void RestrictDepDomain (DepInfo &dep, const DomainInfo &domain,
                        DepDirection dir);

#endif
