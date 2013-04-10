#ifndef ARRAY_REWRITE_H
#define ARRAY_REWRITE_H

#include <ArrayInterface.h>
#include <AstInterface.h>

class CreateTmpArray
{
  std::map<std::string, AstNodePtr>& varmap;
  std::list<AstNodePtr>& newStmts;
  AstNodePtr model;
 public:
  CreateTmpArray( std::map<std::string, AstNodePtr>& _varmap, std::list<AstNodePtr>& _newstmts) 
     :  varmap(_varmap), newStmts(_newstmts), model() {} 
  AstNodePtr create_tmp_array( AstInterface& fa, const AstNodePtr& arrayExp, const std::string name);
  void set_model_array( const AstNodePtr& mod) { model = mod; }
};

class RewriteConstructArrayAccess 
   : public CreateTmpArray, public MapObject<SymbolicVal, SymbolicVal>
{
  // rewrite reads of construct_array ops to remove implicit array temperaries 
  // rewrite reads of unknown array ops by creating explicit temperaries
  ArrayInterface& anal;
  CPPAstInterface& fa;
public:
  RewriteConstructArrayAccess( CPPAstInterface& _fa, ArrayInterface& a,
                               std::map<std::string, AstNodePtr>& _varmap,
                               std::list<AstNodePtr>& _newstmts)
      : CreateTmpArray(_varmap, _newstmts), anal(a), fa(_fa) {}
  bool rewritable( const SymbolicVal& head);
  SymbolicVal operator()( const SymbolicVal& orig);
};

class RewriteArrayModOp : public TransformAstTree
{
  ArrayInterface& anal;

 public:
  RewriteArrayModOp( ArrayInterface &_op) : anal(_op) {}
  bool operator()(AstInterface& fa, const AstNodePtr& head, AstNodePtr& result);
};

class ROSE_DLL_API RewriteToArrayAst : public TransformAstTree
{
  ArrayInterface& anal;

 public:
  RewriteToArrayAst( ArrayInterface &_op) : anal(_op) {}
  bool operator()(AstInterface& fa, const AstNodePtr& head, AstNodePtr& result);
};

class ROSE_DLL_API RewriteFromArrayAst : public TransformAstTree
{
  ArrayInterface& anal;
 public:
  RewriteFromArrayAst( ArrayInterface &_op) : anal(_op) {}
  bool operator()(AstInterface& fa, const AstNodePtr& head, AstNodePtr& result);
};

#endif



