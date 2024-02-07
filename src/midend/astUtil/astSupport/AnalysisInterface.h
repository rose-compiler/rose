#ifndef ANALYSIS_INTERFACE_H
#define ANALYSIS_INTERFACE_H

#include "FunctionObject.h"
#include "SymbolicVal.h"

class FunctionSideEffectInterface
{
 public:
  //! traverses a function call to collect data being modified. 
  // returns false if unknown function encountered
  virtual bool get_modify(AstInterface& fa, const AstNodePtr& fc,
                               CollectObject<AstNodePtr>* collect = 0) = 0 ;

  //! traverses a function call to collect data being read. Returns the callee if requested.
  virtual bool get_read(AstInterface& fa, const AstNodePtr& fc,
                               CollectObject<AstNodePtr>* collect = 0) = 0;
  virtual bool get_call( AstInterface&, const AstNodePtr&, CollectObject<AstNodePtr>* = nullptr)  {
     return false;
  }
  virtual ~FunctionSideEffectInterface() {}
};

class NoFunctionSideEffectAnalysis : public FunctionSideEffectInterface
{
 public:
  virtual bool get_modify(AstInterface&, const AstNodePtr&, CollectObject<AstNodePtr>* = nullptr) {
      return false;
  }
  virtual bool get_read(AstInterface&, const AstNodePtr&, CollectObject<AstNodePtr>* = nullptr, AstNodePtr* = nullptr) {
      return false;
  }
  virtual bool get_call( AstInterface&, const AstNodePtr&, CollectObject<AstNodePtr>* = nullptr) {
      return false;
  }
  virtual ~NoFunctionSideEffectAnalysis() {}
};

template <class AstNodePtr>
class SideEffectAnalysisInterface 
{
 public:
  typedef std::function<bool(AstNodePtr,AstNodePtr)> CollectObject;
 
  // returns false if stmts may ---modify-- unknown (non-collected) locations
  virtual bool 
   get_side_effect( AstInterface& fa, const AstNodePtr& stmts,
                    CollectObject* mod,
                    CollectObject* read= 0,
                    CollectObject* kill = 0,
                    CollectObject* call = 0) = 0;
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
    may_alias(AstInterface&, const AstNodePtr&, const AstNodePtr&, CollectObject< std::pair<AstNodePtr, int>>&) {
      return false;
  }
  virtual ~NoFunctionAliasAnalysis() {}
};

class AliasAnalysisInterface
{
 public:
  virtual void analyze(AstInterface&, const AstNodePtr&) {}
  virtual bool may_alias(AstInterface& fa, const AstNodePtr& r1, const AstNodePtr& r2) = 0;
  virtual ~AliasAnalysisInterface() {}
};

class AssumeNoAlias : public AliasAnalysisInterface
{
 public:
  virtual bool may_alias(AstInterface&, const AstNodePtr&, const AstNodePtr&) {
      return false;
  }
};

// This is the interface to access loop structure stored in AST.
class LoopInterface
{
public:
  ///  Virtual destructor.
  virtual ~LoopInterface() {}

  /**
   *  @param  __fa  AST Interface to access @a __fc.
   *  @param  __fc  AST node to parse.
   *  @param  __init  Returns node holding initial statement, if possible.
   *  @param  __cond  Returns node holding conditional statement, if possible.
   *  @param  __incr  Returns node holding incremental statement, if possible.
   *  @param  __body  Returns node holding loop body, if possible.
   *  @return  Whether the AST __fc is a loop 
   */
  virtual bool
  IsLoop(AstInterface& __fa, const AstNodePtr& __fc,
         AstNodePtr* __init = 0, AstNodePtr* __cond = 0, AstNodePtr* __incr = 0,
         //SymbolicVal* __init = 0, SymbolicVal* __cond = 0, SymbolicVal* __incr = 0,
         AstNodePtr* __body = 0) const = 0;
  
  /**
   *  @param  __fa  AST Interface to access @a __fc.
   *  @param  __fc  AST node to parse.
   *  @param  __ivar  Returns node holding iteration variable, if possible.
   *  @param  __start  Returns node holding initial value, if possible.
   *  @param  __stop  Returns node holding bound value, if possible.
   *  @param  __step  Returns node holding increment value, if possible.
   *  @param  __body  Returns node holding iteration body, if possible.
   *  @return  Whether the AST __fc is a Fortran-style loop 
   */
  virtual bool
  IsFortranLoop(AstInterface& __fa, const AstNodePtr& __fc,
        AstNodePtr* __ivar = 0, AstNodePtr* __start = 0, AstNodePtr* __stop = 0, AstNodePtr* __step = 0,
        AstNodePtr* __body = 0) const = 0;
};

// interface FunctionInterface
/**
 *  @brief  Generic interface to parse/access function AST node.
 *  @since  11/20/2009, jichi
 *
 *  TODO 11/20/2009: 
 *  This is the interface to access function structure stored in AST.
 *  Currently supported function structure is:
 *  - return_type
 *  - function_name
 *  -?
 */

#endif
