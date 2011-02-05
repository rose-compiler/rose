#ifndef ANALYSIS_INTERFACE_H
#define ANALYSIS_INTERFACE_H
// This header provide abstract interfaces to some common program analyses,
// such as
// * side effects
// * alias

#include "FunctionObject.h"

class FunctionSideEffectInterface
{
 public:
   //! Store modifying reference nodes (collect) within a function (fc)
  // returns false if unknown function encountered
  virtual bool get_modify(AstInterface& fa, const AstNodePtr& fc,
                               CollectObject<AstNodePtr>* collect = 0) = 0 ;
   //! Store read reference nodes (collect) within a function (fc)
  // returns false if unknown function encountered
  virtual bool get_read(AstInterface& fa, const AstNodePtr& fc,
                               CollectObject<AstNodePtr>* collect = 0) = 0;
  virtual ~FunctionSideEffectInterface() {}
};

class NoFunctionSideEffectAnalysis : public FunctionSideEffectInterface
{
 public:
  virtual bool get_modify(AstInterface& fa, const AstNodePtr& fc,
                               CollectObject<AstNodePtr>* collect = 0) 
   { return false; }
  virtual bool get_read(AstInterface& fa, const AstNodePtr& fc,
                               CollectObject<AstNodePtr>* collect = 0) 
   { return false; }
  virtual ~NoFunctionSideEffectAnalysis() {}
};

class SideEffectAnalysisInterface 
{
 public:
  // returns false if stmts may ---modify-- unknown (non-collected) locations
  virtual bool 
   get_side_effect( AstInterface& fa, const AstNodePtr& stmts,
                    CollectObject< std::pair<AstNodePtr, AstNodePtr> >* mod,
                    CollectObject< std::pair<AstNodePtr, AstNodePtr> >* read= 0,
                    CollectObject< std::pair<AstNodePtr, AstNodePtr> >* kill = 0) = 0;
  virtual ~SideEffectAnalysisInterface() {}
};

class FunctionAliasInterface
{
 public:
  // returns false if unknown function encountered
  virtual bool
     may_alias(AstInterface& fa, const AstNodePtr& fc, const AstNodePtr& result,
               CollectObject< std::pair<AstNodePtr, int> >& collectalias) = 0;
  virtual bool
     allow_alias(AstInterface& fa, const AstNodePtr& fc, 
               CollectObject< std::pair<AstNodePtr, int> >& collectalias) = 0;
  virtual ~FunctionAliasInterface() {}
};

class NoFunctionAliasAnalysis : public FunctionAliasInterface
{
 public:
  virtual bool
    may_alias(AstInterface& fa, const AstNodePtr& fc, const AstNodePtr& result,
              CollectObject< std::pair<AstNodePtr, int> >& collectalias) 
   { return false; }
  virtual ~NoFunctionAliasAnalysis() {}
};
//! Abstract interface to alias analysis
class AliasAnalysisInterface
{
 public:
   //! Check if two references r1, r2 may be alias to each other
  virtual bool
     may_alias(AstInterface& fa, const AstNodePtr& r1, const AstNodePtr& r2) = 0;
  virtual ~AliasAnalysisInterface() {}
};

class AssumeNoAlias : public AliasAnalysisInterface
{
 public:
  virtual bool
     may_alias(AstInterface& fa, const AstNodePtr& r1, const AstNodePtr& r2)
   { return false; }
};


#endif
