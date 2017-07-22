#ifndef ARRAY_REWRITE_H
#define ARRAY_REWRITE_H

#include <ArrayInterface.h>
#include <AstInterface.h>

class CreateTmpArray
{
  std::map<std::string, AstNodePtr>& varmap;
  std::list<AstNodePtr>& newStmts;
  AstNodePtr model;
  AstNodePtr scope;
 public:
  CreateTmpArray( std::map<std::string, AstNodePtr>& _varmap, std::list<AstNodePtr>& _newstmts,const AstNodePtr& declscope) 
     :  varmap(_varmap), newStmts(_newstmts), scope(declscope), model() {} 
  AstNodePtr create_tmp_array( AstInterface& fa, const AstNodePtr& arrayExp, const std::string name);
  void set_model_array( const AstNodePtr& mod) { model = mod; }
  const AstNodePtr& get_decl_scope() { return scope; }
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
                               std::list<AstNodePtr>& _newstmts, const AstNodePtr& declloc)
      : CreateTmpArray(_varmap, _newstmts,declloc), anal(a), fa(_fa) {}
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

class RewriteToArrayAst : public TransformAstTree
{
  ArrayInterface& anal;

 public:
  RewriteToArrayAst( ArrayInterface &_op) : anal(_op) {}
  bool operator()(AstInterface& fa, const AstNodePtr& head, AstNodePtr& result);
};

class RewriteFromArrayAst : public TransformAstTree
{
  ArrayInterface& anal;
 public:
  RewriteFromArrayAst( ArrayInterface &_op) : anal(_op) {}
  bool operator()(AstInterface& fa, const AstNodePtr& head, AstNodePtr& result);
};

#endif



