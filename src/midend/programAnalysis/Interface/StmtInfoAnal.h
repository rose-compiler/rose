
#ifndef DEP_INFO_ANAL
#define DEP_INFO_ANAL

#include <support/ProcessAstTree.h>
class AstInterface;
class StmtInfoCollect
{ 
 public:
  typedef enum {NONE = 0, LOCAL_MOD_VAR = 1, LOCAL_READ_VAR = 2, FUNC_CALL = 4}
           CollectType;
  virtual void Append( const AstNodePtr& var, CollectType t) = 0; 
  int GetCollectType() const { return t; }
 protected:
  StmtInfoCollect(int _t) : t(_t) {}
 private:
  int t;
};
 
class CollectStmtInfoTraverse : public ProcessAstTreeBase
{
 private:
  StmtInfoCollect *col;
  Boolean ProcessTree( AstInterface &_fa, const AstNodePtr& s,
                       AstInterface::TraversalVisitType t);
 public:
  CollectStmtInfoTraverse() : col(0) {}

  void Collect( AstInterface& fa, const AstNodePtr& h, 
                          StmtInfoCollect* c) 
       { col = c; operator()(fa, h); }
};

#endif
