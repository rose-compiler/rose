#ifndef ANALYSIS_INTERFACE_H
#define ANALYSIS_INTERFACE_H

#include <FunctionObject.h>

class FunctionSideEffectInterface
{
 public:
  // returns false if unknown function encountered
  virtual bool get_modify(AstInterface& fa, const AstNodePtr& fc,
                               CollectObject<AstNodePtr>* collect = 0) = 0 ;

  virtual bool get_read(AstInterface& fa, const AstNodePtr& fc,
                               CollectObject<AstNodePtr>* collect = 0) = 0;
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
};

class SideEffectAnalysisInterface 
{
 public:
  // returns false if stmts may ---modify-- unknown (non-collected) locations
  virtual bool 
   get_side_effect( AstInterface& fa, const AstNodePtr& stmts,
                    CollectObject< STD pair<AstNodePtr, AstNodePtr> >* mod,
                    CollectObject< STD pair<AstNodePtr, AstNodePtr> >* read= 0,
                    CollectObject< STD pair<AstNodePtr, AstNodePtr> >* kill = 0) = 0;
};

class FunctionAliasInterface
{
 public:
  // returns false if unknown function encountered
  virtual bool
     may_alias(AstInterface& fa, const AstNodePtr& fc, const AstNodePtr& result,
               CollectObject< STD pair<AstNodePtr, int> >& collectalias) = 0;
  virtual bool
     allow_alias(AstInterface& fa, const AstNodePtr& fc, 
               CollectObject< STD pair<AstNodePtr, int> >& collectalias) = 0;
};

class NoFunctionAliasAnalysis : public FunctionAliasInterface
{
 public:
  virtual bool
    may_alias(AstInterface& fa, const AstNodePtr& fc, const AstNodePtr& result,
	      CollectObject< STD pair<AstNodePtr, int> >& collectalias) 
   { return false; }
};

class AliasAnalysisInterface
{
 public:
  virtual bool
     may_alias(AstInterface& fa, const AstNodePtr& r1, const AstNodePtr& r2) = 0;
};

class AssumeNoAlias : public AliasAnalysisInterface
{
 public:
  virtual bool
     may_alias(AstInterface& fa, const AstNodePtr& r1, const AstNodePtr& r2)
   { return false; }
};


#endif
