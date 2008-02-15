
#include <general.h>

#include <computation/LoopTreeDepComp.h>
#include <CompSliceLocality.h>
#include <CompSliceRegistry.h>
#include <ReuseAnalysis.h>

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// DQ (3/8/2006): Since this is not used in a header file it is OK here!
#define Boolean int

//not precise since inter-statement spatial reuses not considered
float CompSliceLocalityAnal::
SpatialReuses(const CompSlice *slice)
{
  float reuse = 0;
  CompSlice::ConstLoopIterator sliceIter = slice->GetConstLoopIterator();
  for (LoopTreeNode *n; (n = sliceIter.Current()); sliceIter++) {
     int level = n->LoopLevel();
     CompSlice::ConstStmtIterator iter1 = sliceIter.GetConstStmtIterator();
     for (LoopTreeNode *s; (s = iter1.Current()); iter1++) {
          reuse += anal.SelfSpatialReuses( s, level, linesize);
     }
   }
   return reuse;
}

int CompSliceLocalityAnal::
TemporaryReuses( const CompSlice *slice1, const CompSlice *slice2, AstNodeSet &refSet)
{
  CompSlice::ConstStmtIterator stmtIter2 = slice2->GetConstStmtIterator();
  for (LoopTreeNode *n2; (n2 = stmtIter2.Current()); stmtIter2++) {
     LoopTreeNode *l2 = stmtIter2.CurrentInfo().loop;
     int level2 = l2->LoopLevel();
     CompSlice::ConstStmtIterator stmtIter1 = slice1->GetConstStmtIterator();
     for (LoopTreeNode *n1; (n1 = stmtIter1.Current()); stmtIter1++) {
        LoopTreeNode *l1 = stmtIter1.CurrentInfo().loop;
        int level1 = l1->LoopLevel();
        anal.TemporaryReuseRefs(n1,level1,n2,level2,refSet, reuseDist);
     }
  }
  return refSet.size();
}

class CollectArrayNames : public CollectObject<AstNodePtr>
{
  LoopTransformInterface& la;
  set <string>&  result;
 public:
  CollectArrayNames(LoopTransformInterface& _la, set<string>& r) 
      : la(_la),result(r) {}
   Boolean operator()(const AstNodePtr& r) 
   {
      AstNodePtr arr;
      string arrname;
      if (la.IsArrayAccess(r, &arr)  && AstInterface::IsVarRef(arr,0,&arrname)) {
          result.insert(arrname);
          return true;
      }
      return false;
   }
};

class CollectRegisteredArrayRefs : public CollectObject<AstNodePtr>
{
  LoopTransformInterface& la;
  set <string>&  reg;
  CompSliceLocalityAnal::AstNodeSet& refset;
 public:
  CollectRegisteredArrayRefs(LoopTransformInterface& _la, set<string> & r, 
                            CompSliceLocalityAnal::AstNodeSet& s) 
      : la(_la), reg(r), refset(s){}
   Boolean operator()(const AstNodePtr& r) {
      AstNodePtr arr;
      string arrname;
      if (la.IsArrayAccess(r, &arr)  && AstInterface::IsVarRef(arr,0,&arrname)
          && reg.find(arrname) != reg.end())  {
         refset.insert(r);
         return true;
      }
      return false;
   }
};

int CompSliceLocalityAnal::
SpatialReuses( const CompSlice *slice1, const CompSlice *slice2,
                 AstNodeSet &refSet)
{
  set<string> arrnames;
  CompSlice::ConstStmtIterator stmtIter1 = slice1->GetConstStmtIterator();
  LoopTransformInterface& fa = anal.GetLoopTransformInterface();
  CollectArrayNames op(fa, arrnames);
  for (LoopTreeNode *n1; (n1 = stmtIter1.Current()); stmtIter1++) {
      AstNodePtr s1 = stmtIter1.Current()->GetOrigStmt();
      ArrayReferences( fa, s1, op);
  }
  CollectRegisteredArrayRefs op2(fa, arrnames,refSet);
  CompSlice::ConstStmtIterator stmtIter2 = slice2->GetConstStmtIterator();
  for (LoopTreeNode *n2; (n2 = stmtIter2.Current()); stmtIter2++) {
      AstNodePtr s2 = stmtIter2.Current()->GetOrigStmt();
      ArrayReferences( fa, s2, op2);
  }
  return refSet.size();
}

class SliceInfoToString
{
 public:
  static string ToString( const CompSliceLocalityRegistry::SliceSelfInfo& info) 
  { return info.ToString(); }
  static string ToString( const CompSliceLocalityRegistry::SliceRelInfo& info) 
  { return info.ToString(); }
};

class CompSliceLocalityRegistry::Impl
   : public CompSliceRegistry<SliceSelfInfo,SelfInitInfo,SliceInfoToString,SliceRelInfo,
                              RelInitInfo, SliceInfoToString,CreateInitInfo> 
{
  public:
    Impl( CompSliceLocalityAnal& a) 
       : CompSliceRegistry<SliceSelfInfo,SelfInitInfo,SliceInfoToString,SliceRelInfo,
                         RelInitInfo,SliceInfoToString, CreateInitInfo>(CreateInitInfo(a)) {}
};

CompSliceLocalityRegistry::
CompSliceLocalityRegistry( LoopTreeLocalityAnal &a, unsigned ls, unsigned reuseDist)
    : CompSliceLocalityAnal(a, ls, reuseDist)
{
  impl = new Impl( *this );
}

CompSliceLocalityRegistry::~CompSliceLocalityRegistry()
{
  delete impl;
}

int CompSliceLocalityRegistry::
TemporaryReuses(const CompSlice *slice1, const CompSlice *slice2, AstNodeSet* refset)
{
  return (slice1==slice2)? impl->CreateNode(slice1)->GetInfo().TemporaryReuses(refset)
                  :impl->CreateEdge(slice1,slice2)->GetInfo().TemporaryReuses(refset);
}

int CompSliceLocalityRegistry::
SpatialReuses(const CompSlice *slice1, const CompSlice *slice2)
{
  assert(slice1 != slice2);
  return impl->CreateEdge(slice1,slice2)->GetInfo().SpatialReuses();
}

float CompSliceLocalityRegistry::SpatialReuses( const CompSlice *slice)
{
  return impl->CreateNode(slice)->GetInfo().SpatialReuses(); 
}
  
int CompSliceLocalityRegistry::TemporaryReuses( const CompSlice *slice)
{
  return impl->CreateNode(slice)->GetInfo().TemporaryReuses();
}

#define TEMPLATE_ONLY
#include <IDGraphCreate.C>
template class IDGraphCreateTemplate
        <GraphNodeTemplate<CompSliceLocalityRegistry::SliceSelfInfo,
                           CompSliceLocalityRegistry::SelfInitInfo, SliceInfoToString>,
         GraphEdgeTemplate<CompSliceLocalityRegistry::SliceRelInfo, 
                           CompSliceLocalityRegistry::RelInitInfo, SliceInfoToString> >;
