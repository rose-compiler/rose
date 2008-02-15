#include <general.h>

#include <iostream>
#include <sstream>

#include <DomainInfo.h>

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// DQ (3/8/2006): Since this is not used in a heade file it is OK here!
#define Boolean int

inline DomainInfoImpl* Clone(const DomainInfoImpl& that)
   { return that.Clone(); }

typedef  LatticeElemList<DomainCond>::Iterator DomainInfoIterator;

DomainCondConstIterator DomainInfo::GetConstIterator() const
 { return new IteratorImplTemplate<DomainCond, DomainInfoIterator>(DomainInfoIterator(ConstRef()) ); }

DomainCondUpdateIterator DomainInfo:: GetUpdateIterator()
 { return new IteratorImplTemplate<DomainCond&, DomainInfoIterator>(DomainInfoIterator(UpdateRef())); }

DomainInfo::DomainInfo() 
   : CountRefHandle <DomainInfoImpl>( new DomainInfoImpl() )
       {}

DomainInfo:: DomainInfo(DomainCond c)
     : CountRefHandle <DomainInfoImpl>( DomainInfoImpl() )
     { AddCond(c); }

int DomainInfo::NumOfConds() const { return ConstRef().NumberOfEntries(); }

Boolean DomainInfo::AddCond( const DomainCond &c)
     { return UpdateRef().AddElem(c); }

Boolean DomainInfo::operator |= (const DomainInfo &that)
       {  return UpdateRef() |= that.ConstRef(); }

void DomainInfo:: UpdateDomainCond( Boolean (*Update)(DomainCond &info) )
      { UpdateRef().UpdateElem(Update); }

string DomainInfo::ToString() const 
{ 
  string res;
  const LatticeElemList<DomainCond>& list = ConstRef();
  for ( LatticeElemList<DomainCond>::Iterator iter(list); 
        !iter.ReachEnd(); iter++) {
       res = res + iter.Current().ToString() + "\n";   
  }
  return res;
}

DomainCond::DomainCond( int dim ) 
  : DepInfo(DepInfoGenerator::GetBottomDepInfo(dim,dim, dim))
{  
   for (int i = 0; i < dim; i++) {
      Entry(i,i) = DepRel(DEPDIR_EQ, 0); 
    }
}

void DomainCond :: Initialize( const DepRel &r)
{
    int dim = NumOfLoops();
   for (int i = 0; i < dim; i++) {
     for (int j = i+1; j < dim; j++) {
        Entry(i,j) = r;
     }
   }
}

void DomainCond :: SetLoopRel(int index1, int index2, const DepRel &r)
{ 
   assert( index1 != index2);
   Entry(index1, index2) = r;
   Entry(index2, index1) = Reverse(r);
   ClosureCond();
} 

string DomainCond :: ToString() const 
{  
   stringstream res;

   int num = NumOfLoops();
   res << "# of loops = " << num << ": ";
   for (int i = 0; i < num; i++) { 
      for (int j = i+1; j < num; j++) { 
        DepRel r = Entry(i,j);
        if (!r.IsBottom()) {
           res << i << "," << j << ":" << r.ToString() << ";"; 
        }
     }
   }
   return res.str();
}

Boolean DomainCond :: IsTop() const
{
  if (ConstPtr() == 0)
    return true;
  for (int i = 0; i < NumOfLoops(); i++) {
    for (int j = i+1; j < NumOfLoops(); j++) {
       if (Entry(i,j).IsTop())
          return true;
    }
  }
  return false;
}

Boolean DomainCond :: IsBottom() const
{
  for (int i = 0; i < NumOfLoops(); i++) {
    for (int j = i+1; j < NumOfLoops(); j++) {
      if ( ! Entry(i,j).IsBottom())
         return false;
    }
  }
  return true;
}

Boolean DomainCond :: ClosureCond()
{
   for (int i = 0; i < NumOfLoops(); ++i) {
     for (int j = i+1; j < NumOfLoops(); ++j) {
       Entry(i,j) &= Reverse(Entry(j,i));
       Entry(j,i) &= Reverse(Entry(i,j));
    }
  }
   DomainCond d1 = (*this);
Boolean mod = false;
   while ( (*this) *= d1) 
     mod = true;
   return mod;
}


Boolean DomainCond :: operator |= ( const DomainCond &d2)
{
  return DepInfo:: operator |=(d2);
}

Boolean DomainCond :: operator &=( const DomainCond &d2)
{
Boolean r = DepInfo:: operator &= (d2);
  ClosureCond();
  return r;
}

void DomainCond :: InsertLoop( int level)
{
  DepInfo::InsertLoop(level, DEP_SRC_SINK);
  Entry(level,level) = DEPDIR_EQ;
}

void DomainCond :: RemoveLoop( int level)
{
  DepInfo::RemoveLoop(level, DEP_SRC_SINK);
}

void DomainCond::MergeLoop( int index1, int index2)
{
  DepInfo::MergeLoop(index1,index2, DEP_SRC_SINK);
  SetLoopRel( index1, index2, DEPDIR_EQ);
}

void DomainCond::SwapLoop( int index1, int index2)
{
  DepInfo::SwapLoop(index1, index2, DEP_SRC_SINK);
}

void DomainCond::AlignLoop( int index, int align)
{
  DepInfo::AlignLoop(index, align, DEP_SRC_SINK);
}


DomainCond operator |  ( const DomainCond &info1, DomainCond &info2)
{
  DomainCond result(info1);
  result |= info2; 
  return result;
}

DomainCond operator & ( const DomainCond &info1, const DomainCond &info2)
{
  DomainCond result(info1);
  result &= info2;
  return result;
}

void DomainCond ::  RestrictDepInfo (DepInfo &dep, DepDirection dir) const
{
  if (IsTop() || IsBottom())
     return;

  DepInfo info( *this);
  if (dir & DEP_SRC) {
     dep &= info * dep;
  }
  if (dir & DEP_SINK) {
     dep &= dep * info;
  }
}


DomainCond::DomainCond( DepInfo &dep, DepDirection dir)
{
  DepInfo dep1 = Reverse(dep);
  DepInfo::operator = ((dir == DEP_SRC)? dep * dep1 : dep1 * dep);
}

DomainInfo  operator - ( const DomainCond &d1, const DomainCond &d2)
{
  assert( d1.NumOfLoops() == d2.NumOfLoops());
  DomainInfo result;
  for (int i = 0; i < d1.NumOfLoops(); i++) {
     for (int j = 0; j < i; j++) {
       DepRel r =  d1.Entry(j,i) & (- d2.Entry(j,i));
       if (!r.IsTop()) {
          DomainCond d(d1);
          d.SetLoopRel( j, i, r );
          if (!d.IsTop())
             result.AddCond( d );
       }
     }
  }
  return result;
}

DomainInfo operator & ( const DomainInfo &dm1, const DomainInfo &dm2)
{
   DomainInfo result;
   for (DomainCondConstIterator iter1=dm1.GetConstIterator(); !iter1.ReachEnd(); iter1++) {
      DomainCond c1 = iter1.Current();
      for (DomainCondConstIterator iter2=dm2.GetConstIterator(); !iter2.ReachEnd(); iter2++) {
         DomainCond d = c1 &  iter2.Current();
         result.AddCond( d );
      }
   }
   return result;
}

DomainInfo  operator | (const DomainInfo& dm1, const DomainInfo& dm2)
{
   DomainInfo result(dm1);
   result |= dm2;
   return result;
}

DomainInfo operator - ( const DomainInfo &info1, const DomainInfo& info2)
{
  DomainInfo result(info1);
  for (DomainCondConstIterator iter2=info2.GetConstIterator(); !iter2.ReachEnd(); iter2++) {
    DomainInfo tmp;
    for (DomainCondConstIterator iter1=result.GetConstIterator(); !iter1.ReachEnd(); iter1++) {
      tmp  |=  iter1.Current() - iter2.Current();
    }
    result = tmp;
  }
  return result;
}

Boolean DomainInfo :: IsTop() const
{
  if (NumOfConds() == 0)
    return true;
  else if (NumOfConds() == 1) {
    if (ConstRef().First()->GetEntry().IsTop())
      return true;
  }
  return false;
}

Boolean DomainInfo :: IsBottom() const
{
  if (NumOfConds() == 1) {
    if (ConstRef().First()->GetEntry().IsBottom())
      return true;
  }
  return false;
}

DepRel DomainInfo :: GetLoopRel( int loop1, int loop2) const
{
  DepRel result(DEPDIR_NONE);
  for (DomainInfoIterator iter(ConstRef()); !iter.ReachEnd(); iter++) {
     result |= iter.Current().Entry(loop1, loop2);
  }
  return result;
}

int DomainInfo :: NumOfLoops() const
{
  return DomainInfoIterator(ConstRef()).Current().NumOfLoops(); 
}

void RestrictDepDomain (DepInfo &dep, const DomainInfo &domain,
                        DepDirection dir)
{
  DomainCondConstIterator iter=domain.GetConstIterator();
  iter.Current().RestrictDepInfo(dep, dir);
  for (iter++; !iter.ReachEnd(); iter++) {
    DepInfo d1(dep);
    iter.Current().RestrictDepInfo(d1, dir);
    dep |= d1;
 }
}


