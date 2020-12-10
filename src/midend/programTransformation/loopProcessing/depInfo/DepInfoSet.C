#include <stdlib.h>

#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

#include <DepRel.h>
#include <DepInfoSet.h>

typedef LatticeElemList<DepInfo>::iterator DepInfoSetIterator;

DepInfoConstIterator DepInfoSet :: GetConstIterator() const
{ if (ConstPtr() == 0)
       return DepInfoConstIterator();
  else
     return  new IteratorImplTemplate<DepInfo,DepInfoSetIterator>( DepInfoSetIterator(ConstRef()));
}

DepInfoUpdateIterator DepInfoSet :: GetUpdateIterator()
{  if (ConstPtr() == 0)
       return DepInfoUpdateIterator();
  else
     return  new IteratorImplTemplate<DepInfo&,DepInfoSetIterator>( DepInfoSetIterator(UpdateRef()));
}

DepInfoSet:: DepInfoSet() : CountRefHandle <DepInfoSetImpl>( new DepInfoSetImpl() )
{}

void DepInfoSet :: UpdateDepInfo( bool (*Update)(DepInfo &info) )
      { UpdateRef().UpdateElem(Update); }

int DepInfoSet :: NumOfDeps() const { return ConstRef().NumberOfEntries(); }

std::string DepInfoSet :: toString() const
 {
   std::string r;
   for (DepInfoSetIterator iter(ConstRef()); !iter.ReachEnd(); iter.Advance()) {
     r = r + iter.Current().toString();
   }
   return r;
 }

bool DepInfoSet :: operator |= (const DepInfoSet &that)
{
  bool mod = false;
  for (DepInfoSetIterator iter(that.ConstRef()); !iter.ReachEnd();
       iter.Advance()) {
     bool tmp = AddDep(iter.Current());
     mod = mod || tmp;
  }
  return mod;
}

bool DepInfoSet :: operator &= ( const DepInfoSet &dm2)
{
  bool mod = false;
  DepInfoSet result(DepInfoSetGenerator::GetTopInfoSet());
  for (DepInfoSetIterator iter1(ConstRef()); !iter1.ReachEnd(); iter1++) {
      DepInfo c1 = iter1.Current();
      DepInfoSetIterator iter2(dm2.ConstRef());
      for ( ; !iter2.ReachEnd(); ++iter2) {
         DepInfo d = c1 &  iter2.Current();
         result.AddDep( d );
         if (d == c1)
            break;
      }
      mod = (mod || iter2.ReachEnd());
   }
   *this = result;
   return mod;
}
DepInfoSet DepInfoSetGenerator::GetTopInfoSet()
{
  return DepInfoSet();
}

DepInfoSet DepInfoSetGenerator::GetBottomInfoSet(int nr, int nc, int commLevel)
{
  DepInfoSet result;
  result.AddDep(DepInfoGenerator::GetBottomDepInfo(nr,nc, commLevel));
  return result;
}

DepInfoSet DepInfoSetGenerator::GetIDInfoSet(int nr)
{
  DepInfoSet result;
  result.AddDep(DepInfoGenerator::GetIDDepInfo(nr, false));
  return result;
}


DepRel DepInfoSet :: GetDepRel( int index1, int index2 ) const
{
  DepRel r(DEPDIR_NONE);
  for (DepInfoSetIterator iter(ConstRef()); !iter.ReachEnd(); iter++) {
     r |= iter.Current().Entry(index1, index2) ;
  }
  return r;
}

bool DepInfoSet :: IsTop() const
{
  if (NumOfDeps() == 0)
    return true;
  else if (NumOfDeps() == 1) {
    if (ConstRef().First()->GetEntry().IsTop())
      return true;
  }
  return false;
}

bool DepInfoSet :: IsBottom(int commLevel) const
{
  if (NumOfDeps() == 1) {
    if (ConstRef().First()->GetEntry().IsBottom(commLevel))
      return true;
  }
  return false;
}

class MergeDepInfo : public LatticeElemMerge<DepInfo>
{
bool IsTop(const DepInfo& d) { return d.IsTop(); }
bool MergeElem( const DepInfo &d1, const DepInfo& d2, DepInfo &result)
 {
  int nr = d1.rows(), nc = d1.cols();
  assert( nr == d2.rows() && nc == d2.cols());

  int iref = 0;
  for (int i = 0; i < nr; i++) {
    for (int j = 0; j < nc; j++) {
       DepRel r1 = d1.Entry(i,j), r2 = d2.Entry(i,j);
       if (r1 == r2) continue;
       if (r2 <= r1) {
         if (iref != 1 && iref != 0)
             return false;
         else
              iref = 1;
       }
       else if (r1 <= r2) {
         if (iref != 2 && iref != 0)
             return false;
         else
             iref = 2;
       }
       else if (iref != 0)
           return false;
       else {
           iref = 3;
       }
     }
  }
  if (iref <= 1)
     result = d1;
  else if (iref == 2)
      result = d2;
  else if (iref == 3)
     result = d1 | d2;
  return true;
 }
};

bool DepInfoSet :: AddDep( const DepInfo &c)
  {
    MergeDepInfo merge;
    return UpdateRef().AddElem(c, &merge);
  }

DepInfoSet operator & ( const DepInfoSet &dm1, const DepInfoSet &dm2)
{
   DepInfoSet result(dm1);
   result &= dm2;
   return result;
}

DepInfoSet  operator | (const DepInfoSet& dm1, const DepInfoSet& dm2)
{
   DepInfoSet result(dm1);
   result |= dm2;
   return result;
}

DepInfoSet Closure( DepInfoSet t )
{
  DepInfoSet result(DepInfoSetGenerator::GetTopInfoSet());
  for (DepInfoConstIterator iter = t.GetConstIterator(); !iter.ReachEnd(); iter++) {
       result.AddDep( Closure(iter.Current()));
  }

  bool change = true;
  while (change) {
    change = false;
    DepInfoSet tmp = result;
    for (DepInfoConstIterator iter1 = result.GetConstIterator(); !iter1.ReachEnd(); iter1++) {
      for (DepInfoConstIterator iter2= result.GetConstIterator(); !iter2.ReachEnd(); iter2++) {
         DepInfo d = iter1.Current() * iter2.Current();
         if (tmp.AddDep( d))
             change = true;
      }
    }
    result = tmp;
  }
  return result;
}

DepInfoSet operator * (const DepInfoSet &t1, const DepInfoSet &t2)
{
  DepInfoSet result(DepInfoSetGenerator::GetTopInfoSet());
  for (DepInfoConstIterator iter1 = t1.GetConstIterator(); !iter1.ReachEnd(); iter1++) {
    for (DepInfoConstIterator iter2=t2.GetConstIterator(); !iter2.ReachEnd(); iter2++) {
       DepInfo d = iter1.Current() * iter2.Current();
       result.AddDep(d);
     }
   }
   return result;
}


