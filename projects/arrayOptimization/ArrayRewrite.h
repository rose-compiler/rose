#ifndef ARRAY_REWRITE_H
#define ARRAY_REWRITE_H

#include <ArrayInterface.h>
#include <AstInterface.h>

class CreateTmpArray
{
  map<string, AstNodePtr>& varmap;
  list<AstNodePtr>& newStmts;
  AstNodePtr model;
 public:
  CreateTmpArray( map<string, AstNodePtr>& _varmap, list<AstNodePtr>& _newstmts) 
     :  varmap(_varmap), newStmts(_newstmts), model(0) {} 
  AstNodePtr create_tmp_array( AstInterface& fa, const AstNodePtr& arrayExp, const string name);
  void set_model_array( const AstNodePtr& mod) { model = mod; }
};

class RewriteConstructArrayAccess 
   : public CreateTmpArray, public MapObject<SymbolicVal, SymbolicVal>
{
  // rewrite reads of construct_array ops to remove implicit array temperaries 
  // rewrite reads of unknown array ops by creating explicit temperaries
  ArrayInterface& anal;
  AstInterface& fa;
public:
  RewriteConstructArrayAccess( AstInterface& _fa, ArrayInterface& a,
                               map<string, AstNodePtr>& _varmap,
                               list<AstNodePtr>& _newstmts)
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



