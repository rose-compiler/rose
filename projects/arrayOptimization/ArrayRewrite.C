
// DQ (1/1/2006): This is OK if not declared in a header file
using namespace std;

#include <general.h>
#include <ArrayRewrite.h>
#include <AstInterface.h>
#include <DepInfoAnal.h>
#include <DomainInfo.h>

// DQ (3/13/2006): If we have to have this at lease it is 
// not in the header files seen by ALL ROSE applications.
#define Boolean int

bool RecognizeArrayOp( AstInterface& fa, ArrayInterface& anal,
                      const AstNodePtr& orig)
{
  if (!fa.IsFunctionCall(orig)) 
      return false;
  if (anal.is_array_mod_op( orig)) {
    cerr << "recognized array mod op: ";
    fa.write( orig, cerr);
    cerr << endl;
    return true;
  }
  else if ( anal.is_array_construct_op( orig)) {
    cerr << "recognized array construct op: ";
    fa.write(orig, cerr);
    cerr << endl;
    return true;
  }
  else  {
    cerr << "not recognize array op: ";
    fa.write(orig, cerr);
    cerr << endl;
  }
  return false;
}

class HasDependence : public CollectObject<DepInfo>
{
  bool result;
 public:
  HasDependence() : result(false) {}
  virtual Boolean operator()( const DepInfo& info) 
   { result = true; return true; }
  bool& get_result() { return result; }
};

class RewriteModArrayAccess : public CreateTmpArray, public TransformAstTree
{
  // rewrite reads of modified array by creating explicit temperaries
  AstNodePtr stmt, lhs, modarray;
  unsigned int size;
  LoopTransformInterface la;
  DepInfoAnal depAnal;
  ArrayInterface& anal;
public:
  RewriteModArrayAccess( AstInterface& ai, ArrayInterface& a, 
                         const AstNodePtr& _stmt, const AstNodePtr& _lhs,
                         map<string, AstNodePtr>& _varmap,
                         list<AstNodePtr>& _newstmts)
    : CreateTmpArray(_varmap, _newstmts),
      stmt(_stmt), lhs(_lhs), 
      la( ai, a, ArrayAnnotation::get_inst(), &a),
      depAnal(la),
      anal(a)
     { 
        AstInterface::AstNodeList subs;
        if (!ArrayAnnotation::get_inst()->is_access_array_elem( lhs, &modarray, &subs))
           assert(false);
        size = subs.size();
      }
  bool operator() (AstInterface& fa, const AstNodePtr& orig, AstNodePtr& result)
 {
  AstNodePtr array;
  AstInterface::AstNodeList subs;
  if (!ArrayAnnotation::get_inst()->is_access_array_elem(orig, &array, &subs) || subs.size() == 0)
     return false;
  HasDependence test;
  if (fa.IsSameVarRef( array, modarray)) {
     assert(size == subs.size());
     DomainCond domain(size);
     DepInfoAnal::StmtRefDep ref = depAnal.GetStmtRefDep(la,stmt,orig, stmt,lhs);
     depAnal.ComputeArrayDep( la, ref,DEPTYPE_NONE,test, test);
  }
  else 
     test.get_result() = la.IsAliasedRef(array, modarray);
  if (test.get_result()) {
      string splitname = fa.GetVarName(array) + "_tmp";
      result = create_tmp_array( fa, array, splitname);
      AstInterface::AstNodeList subscopy = fa.CreateList();
      for (AstInterface::AstNodeListIterator p = fa.GetAstNodeListIterator(subs); 
           !p.ReachEnd(); ++p) {
         fa.ListAppend(subscopy, fa.CopyAstTree(*p));
      }   
      result = ArrayAnnotation::get_inst()->create_access_array_elem( fa, result, subscopy);
      return true;
  }
  return false;
 }
};

bool RewriteConstructArrayAccess:: rewritable( const SymbolicVal& orig)
{
  AstNodePtr arrayExp;
  if (ArrayAnnotation::get_inst()->is_access_array_elem( orig, &arrayExp) ||
      ArrayAnnotation::get_inst()->is_access_array_length( orig, &arrayExp)) {
      if (ArrayAnnotation::get_inst()->is_array_construct_op( arrayExp) 
          || AstInterface::IsVarRef(arrayExp))
          return true;
  }
  return false;
}

AstNodePtr CreateTmpArray::
create_tmp_array( AstInterface& fa, const AstNodePtr& arrayExp, const string name)
{
  string expname = fa.AstToString(arrayExp);
  AstNodePtr& split = varmap[expname];
  if (split == 0) {
     AstNodeType t =  fa.GetExpressionType(arrayExp);
     string tname;
     fa.GetTypeInfo( t, 0, &tname);
     string splitname = fa.NewVar( fa.GetType(tname), name, true );
     if (model == 0) {
        split = fa.CreateVarRef(splitname);
     }
     else {
        split = fa.CreateVarRef(splitname);
     }
     AstNodePtr splitStmt = fa.CreateAssignment( split, fa.CopyAstTree( arrayExp));
     newStmts.push_back(splitStmt); 
     return split;
  }
  else {
     AstNodePtr r = fa.CopyAstTree(split);
     return r;
  }
}

SymbolicVal RewriteConstructArrayAccess :: 
operator()( const SymbolicVal& orig)
{
  SymbolicVal result;
  SymbolicVal dim;
  SymbolicFunction::Arguments args;
  AstNodePtr arrayExp;
  if (ArrayAnnotation::get_inst()->is_access_array_elem( orig, &arrayExp, &args)) {
    anal.set_array_dimension( arrayExp, args.size());
    SymbolicFunctionDeclarationGroup elem;
    if (anal.is_array_construct_op( arrayExp, 0, 0, 0, &elem)) {
      if (! elem.get_val( args, result))
	assert(false);
      result = ReplaceVal( result, *this);
    }
    else if (!AstInterface::IsVarRef(arrayExp)) {
      AstNodePtr split = create_tmp_array( fa, arrayExp, "array");
      result = ArrayAnnotation::get_inst()->create_access_array_elem( split, args);
    }
  }
  else if (ArrayAnnotation::get_inst()->is_access_array_length( orig, &arrayExp, &dim)) {
    SymbolicFunctionDeclarationGroup len;
    if (anal.is_array_construct_op( arrayExp, 0, 0, &len)) {
      args.clear();
      args.push_back(dim);
      if (! len.get_val( args, result))
	assert(false);
      result = ReplaceVal( result, *this);
    }
    else if (!AstInterface::IsVarRef(arrayExp)) {
      AstNodePtr split = create_tmp_array( fa, arrayExp, "array");
      result = ArrayAnnotation::get_inst()->create_access_array_length( split, dim);
    }
  }
  return result;
}

bool CollectArrayRef( AstInterface& fa, ArrayInterface& anal, 
                      const AstNodePtr& array, AstInterface::AstNodeList& col)
{
    if (fa.IsVarRef(array)) {
       fa.ListAppend(col, array);
       return true;
    }
    AstInterface::AstNodeList cur;
    if (!anal.is_array_construct_op( array, &cur))
       return false;
    for (AstInterface::AstNodeListIterator p = fa.GetAstNodeListIterator(cur);
         !p.ReachEnd(); ++p) {
        if (!CollectArrayRef(fa, anal, *p, col))
             return false;
    }
    return true;
}

bool RewriteArrayModOp::
operator () ( AstInterface& fa, const AstNodePtr& orig, AstNodePtr& result)
{
  if (!fa.IsStatement(orig))
    return false;
  
  AstNodePtr modArray;
  int dimension = 0;
  SymbolicFunctionDeclarationGroup len, elem;
  bool reshape = false;
  if (!anal.is_array_mod_op( orig, &modArray, &dimension, &len, &elem, &reshape)) 
    return false;

  SymbolicFunction::Arguments ivarList;
  for (int i = 0; i < dimension; ++i ) {
      AstNodeType t= fa.GetType("int");
      string ivarname = fa.NewVar( t);
      AstNodePtr ivar = fa.CreateVarRef( ivarname);
      ivarList.push_back( SymbolicAstWrap(ivar));
  }
  SymbolicVal rhs;
  if (!elem.get_val( ivarList, rhs))
    assert(false);

  map<string,AstNodePtr> varmap;
  list<AstNodePtr> newStmts;
  RewriteConstructArrayAccess constructArrayRewrite(fa, anal, varmap, newStmts);
  if (!constructArrayRewrite.rewritable( rhs ))
       return false;

  SymbolicVal lhs = ArrayAnnotation::get_inst()->create_access_array_elem( modArray, ivarList);
  if (!fa.IsVarRef(modArray)) 
      lhs = ReplaceVal(lhs, constructArrayRewrite);
  rhs = ReplaceVal( rhs, constructArrayRewrite);

  AstNodePtr lhsast = lhs.CodeGen(fa);
  AstNodePtr rhsast = rhs.CodeGen(fa);
  AstNodePtr stmt = fa.CreateAssignment(lhsast, rhsast), body = stmt;
  SymbolicFunction::Arguments lenlist;
  for (int i = 0; i < dimension; ++i ) {
    SymbolicFunction::Arguments args;
    args.push_back( SymbolicConst(i));
    SymbolicVal ubval;
    if (!len.get_val(args, ubval))
      assert(false);
    ubval = ReplaceVal( ubval, constructArrayRewrite);
    ubval = ubval -1;
    lenlist.push_back(ubval);
    AstNodePtr ub = ubval.CodeGen(fa);
    AstNodePtr lb = fa.CreateConstInt(0), step = fa.CreateConstInt(1);
    AstNodePtr ivarAst = ivarList[i].ToAst();
    assert(ivarAst != 0);
    body = fa.CreateLoop( ivarAst, lb, ub, step, body);
  }

  RewriteModArrayAccess modArrayRewrite( fa, anal, stmt, lhsast, varmap, newStmts);
  TransformAstTraverse( fa, rhsast, modArrayRewrite);

  if (!reshape && newStmts.size() == 0) 
      result = body;
  else {
      result = fa.CreateBasicBlock();
      for (list<AstNodePtr>::iterator p = newStmts.begin(); p != newStmts.end();
           ++p) {
         AstNodePtr cur = (*p);
         AstNodePtr ncur = cur;
         if (operator()(fa, cur, ncur))
	     fa.BasicBlockAppendStmt( result, ncur);
         else
	     fa.BasicBlockAppendStmt( result, cur);
      }
      if (reshape) {
         AstInterface::AstNodeList argList = fa.CreateList();
         for (int i = 0; i < dimension; ++i) {
	   AstNodePtr curlen = lenlist[i].CodeGen(fa);
	   fa.ListAppend(argList, curlen);
         }
         AstNodePtr reshapeStmt = ArrayAnnotation::get_inst()->create_reshape_array(fa,modArray, argList);
         fa.BasicBlockAppendStmt(result, reshapeStmt);
      }
      fa.BasicBlockAppendStmt(result, body);
  }
  return true;
}

bool RewriteToArrayAst::
operator() ( AstInterface& fa, const AstNodePtr& orig, AstNodePtr& result)
{
  RewriteArrayModOp op1(anal);
  if (op1(fa, orig, result))
       return true;
 
  return false;  
}

bool RewriteFromArrayAst::
operator() ( AstInterface& fa, const AstNodePtr& orig, AstNodePtr& result)
{
  AstNodePtr array;
  AstNodePtr decl, body;
  int  dim;
  AstInterface::AstNodeList args, vars;
  if (fa.IsVariableDecl( orig, &vars)) {
     for (AstInterface::AstNodeListIterator pv = fa.GetAstNodeListIterator(vars);
           !pv.ReachEnd(); ++pv) {
        AstNodePtr cur = *pv;
        if (! ArrayAnnotation::get_inst()->known_array( cur))
           break; 
        AstNodePtr initdefs = anal.impl_array_opt_init(fa, cur);
        fa.InsertStmt( orig, initdefs, false, true);
     }
   }
   else  if (fa.IsFunctionDefinition( orig, &decl, &body)) {
      if (!fa.IsFunctionDecl( decl, 0, 0, 0, &vars) )
        assert(false);
      for (AstInterface::AstNodeListIterator pv = fa.GetAstNodeListIterator(vars);
           !pv.ReachEnd(); ++pv) {
        AstNodePtr cur = *pv;
        if (! ArrayAnnotation::get_inst()->known_array( cur))
           break; 
        anal.impl_array_opt_init(fa, cur, true);
      }
  }
  else if (ArrayAnnotation::get_inst()->is_access_array_elem( orig, &array, &args )) {
     result = anal.impl_access_array_elem( fa, array, args);
     return true;
  }
  else if (ArrayAnnotation::get_inst()->is_reshape_array(  orig, &array, &args) ) {
    assert(fa.IsVarRef(array));
    AstNodePtr reshape = anal.impl_reshape_array( fa, array, args);
    reshape = TransformAstTraverse( fa, reshape, *this);
    result = fa.CreateBasicBlock();
    fa.BasicBlockAppendStmt( result, reshape);
    AstNodePtr initdefs = anal.impl_array_opt_init(fa, array);
    fa.BasicBlockAppendStmt( result, initdefs);
    return true;
  }
  else if (ArrayAnnotation::get_inst()->is_access_array_length( orig, &array, 0, &dim)) {
    assert(fa.IsVarRef(array));
    result = anal.impl_access_array_length( fa, array, dim);
    return true;
  }
  return false;
}


