

#include <stdlib.h>

#include <ReuseAnalysis.h>
#include <DepInfoAnal.h>

int GetConstArrayBound( LoopTransformInterface& a, const AstNodePtr& array, 
                        int dim, unsigned defaultSize)
{
   int lb, ub;
   if (  a.GetArrayBound( array, dim, lb, ub) ) {
      return (ub - lb + 1);
   }
   return defaultSize;
}

class AstRefGetAccessStride : public ProcessAstNode
{
  std::string name;
  bool Traverse ( AstInterface &fa, const AstNodePtr& v,
                       AstInterface::TraversalVisitType t)
  { 
   std::string name1;
   if (fa.IsVarRef(v, 0, &name1)) {
       if (name == name1) {
          return false;
       }
    }
    return true;
  }
 public:
  bool SweepArrayDimension( LoopTransformInterface &fa, const AstNodePtr& r, 
                                std::string ivar, int dim)
  {
   AstInterface::AstNodeList subs;
   if (! fa.IsArrayAccess(r, 0, &subs)) 
      assert(false);
   name = ivar;
   AstInterface::AstNodeList::iterator  listp = subs.begin(); 
   for (int i = 0; listp != subs.end() && i < dim; ++listp,++i);
   assert(listp != subs.end());
   AstNodePtr sub = *listp;
   return !ReadAstTraverse(fa, sub, *this, AstInterface::PreOrder);
  }
  unsigned operator() ( LoopTransformInterface &fa, const AstNodePtr& r, 
                        std::string _name, unsigned arrayBound)
  {
   AstNodePtr nameAst;
   AstInterface::AstNodeList subs;
   AstInterface & ai = fa;
   if ( fa.IsArrayAccess(r, &nameAst, &subs)) {
     name = _name;
     AstNodeType elemtype;
     AstNodePtr isexp = ai.IsExpression(r, &elemtype);
     assert(isexp != AST_NULL);
     int typesize;
     ai.GetTypeInfo(elemtype, 0, 0, &typesize); 

     unsigned size = 1, rsize = 0;

     AstInterface::AstNodeList::iterator  list = subs.begin();
     for (int dim = 0; list != subs.end(); ++list,++dim) { 
       AstNodePtr sub = *list;
       if (!ReadAstTraverse(fa, sub, *this, AstInterface::PreOrder)) {
          rsize = size;
       }
       size *= GetConstArrayBound( fa, nameAst, dim, arrayBound);
      }
      return rsize* typesize;
    }
    else
      return 0;
  }
};

bool ReferenceDimension(LoopTransformInterface& la, const AstNodePtr& r,
                       const std::string& ivarname, int dim)
{
  return AstRefGetAccessStride()(la, r, ivarname, dim);
}

int ReferenceStride(LoopTransformInterface& la, const AstNodePtr& r, 
                    const std::string& ivarname, unsigned arrayBound)
{
   return AstRefGetAccessStride()(la, r, ivarname, arrayBound );
}

float SelfSpatialReuse( LoopTransformInterface& la, const AstNodePtr& r, 
                        const std::string& ivarname, unsigned linesize,
                        unsigned defaultArrayBound)
{
        int size = AstRefGetAccessStride()(la, r, ivarname, defaultArrayBound );
        if (size > 0 && size < (int)linesize) 
          return 1 - size * 1.0 /linesize ;
        else
          return 0;
}


DepType TemporaryReuseRefs( DepInfoConstIterator ep, Map2Object<AstNodePtr, DepDirection,int>& loopmap, 
                            CollectObject<AstNodePtr>& refCollect, int* dist) 
{
  int mdist = 0;
  DepType reuseType;
  
  for ( ep.Reset();  !ep.ReachEnd(); ++ep) {
     DepInfo d = ep.Current();
     DepType t = d.GetDepType();
     if (t != DEPTYPE_OUTPUT && t != DEPTYPE_INPUT && t != DEPTYPE_TRUE
            && t != DEPTYPE_ANTI)
             continue;
       reuseType = (DepType)( reuseType | t);
     AstNodePtr src = d.SrcRef(), snk = d.SnkRef();
     assert(src != AST_NULL && snk != AST_NULL);
     int loop1 = loopmap(src, DEP_SRC), loop2 = loopmap(snk, DEP_SINK); 
     assert(loop1 >= 0 && loop2 >= 0);
     DepRel r = d.Entry(loop1, loop2);
     int a = r.GetMaxAlign();
     if (a <= 0 && (dist == 0 || (*dist) < 0 || a >= -(*dist)) ) {
       refCollect(snk);
       if (mdist > 0 && mdist < -a)
         mdist = -a;
     }
     else if (dist == 0 || (*dist) < 0) {
        mdist = -1;
        refCollect(snk); 
     }
  }
  if (dist != 0)
     *dist = mdist;
  return reuseType;
}

class CollectArrayRef : public CollectObject<AstNodePtr>
{
   CollectObject<AstNodePtr> &collect;
   LoopTransformInterface& la;
 public:
   bool operator()(const AstNodePtr& cur) 
    {
      AstNodePtr array;
      if (la.IsArrayAccess(cur, &array) ) {
          collect(cur); 
          return true;
      } 
      return false;
    }
   CollectArrayRef( LoopTransformInterface& _la, 
                    CollectObject<AstNodePtr>& _collect) 
      : collect(_collect), la(_la) {}
};

void ArrayReferences( LoopTransformInterface& la, const AstNodePtr& s, 
                      CollectObject<AstNodePtr>& refCollect)
{
  CollectArrayRef col(la, refCollect);
  AnalyzeStmtRefs(la, s, col, col);
}
