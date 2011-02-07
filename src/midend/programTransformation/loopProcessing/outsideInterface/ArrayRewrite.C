


#include <ArrayRewrite.h>
#include <AstInterface.h>
#include <DepInfoAnal.h>
#include <DomainInfo.h>

bool RecognizeArrayOp( CPPAstInterface& fa, ArrayInterface& anal,
                      const AstNodePtr& orig)
{
  if (!fa.IsFunctionCall(orig))  {
      return false;
  }
  if (anal.is_array_mod_op( fa, orig)) {
    std::cerr << "recognized array mod op: " << AstToString(orig) << "\n";
    return true;
  }
  else if ( anal.is_array_construct_op( fa, orig)) {
    std::cerr << "recognized array construct op: " << AstToString(orig) << "\n";
    return true;
  }
  else  {
    std::cerr << "not recognize array op: " << AstToString(orig) << "\n";
  }
  return false;
}

class HasDependence : public CollectObject<DepInfo>
{
  bool result;
 public:
  HasDependence() : result(false) {}
  virtual bool operator()( const DepInfo& info) 
   { result = true; return true; }
  bool& get_result() { return result; }
};

class RewriteModArrayAccess : public CreateTmpArray, public TransformAstTree
{
  // rewrite reads of modified array by creating explicit temperaries
  AstNodePtr stmt, lhs, modarray;
  size_t size;
  LoopTransformInterface la;
  DepInfoAnal depAnal;
  ArrayInterface& anal;
public:
  RewriteModArrayAccess( CPPAstInterface& ai, ArrayInterface& a, 
                         const AstNodePtr& _stmt, const AstNodePtr& _lhs,
                         std::map<std::string, AstNodePtr>& _varmap,
                         std::list<AstNodePtr>& _newstmts)
    : CreateTmpArray(_varmap, _newstmts),
      stmt(_stmt), lhs(_lhs),
      la( ai, a, ArrayAnnotation::get_inst(), &a),
      depAnal(la), anal(a)
     { 
        AstInterface::AstNodeList subs;
        if (!ArrayAnnotation::get_inst()->is_access_array_elem( ai, lhs, &modarray, &subs))
           assert(false);
        size = subs.size();
      }
  bool operator() (AstInterface& _fa, const AstNodePtr& orig, AstNodePtr& result)
 {
  CPPAstInterface& fa = static_cast<CPPAstInterface&>(_fa);
  AstNodePtr array;
  AstInterface::AstNodeList subs;
  if (!ArrayAnnotation::get_inst()->is_access_array_elem(fa, orig, &array, &subs) || subs.size() == 0)
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
      std::string splitname = fa.GetVarName(array) + "_tmp";
      result = create_tmp_array( fa, array, splitname);
      AstInterface::AstNodeList subscopy;
      for (AstInterface::AstNodeList::iterator p = subs.begin(); 
           p != subs.end(); ++p) {
         subscopy.push_back( fa.CopyAstTree(*p));
      }   
      result = ArrayAnnotation::get_inst()->create_access_array_elem( fa, result, subscopy);
      return true;
  }
  return false;
 }
};

bool RewriteConstructArrayAccess:: 
rewritable( const SymbolicVal& orig)
{
  AstNodePtr arrayExp;
  if (ArrayAnnotation::get_inst()->is_access_array_elem( fa, orig, &arrayExp) ||
      ArrayAnnotation::get_inst()->is_access_array_length( fa, orig, &arrayExp)) {
      if (ArrayAnnotation::get_inst()->is_array_construct_op( fa, arrayExp) 
          || fa.IsVarRef(arrayExp))
          return true;
  }
  return false;
}

AstNodePtr CreateTmpArray::
create_tmp_array( AstInterface& fa, const AstNodePtr& arrayExp, const std::string name)
{
  std::string expname;
  if (!fa.IsVarRef(arrayExp,0,&expname))
      assert(false);
  AstNodePtr& split = varmap[expname];
  if (split == 0) {
     AstNodeType t =  fa.GetExpressionType(arrayExp);
     std::string tname;
     fa.GetTypeInfo( t, 0, &tname);
     std::string splitname = fa.NewVar( fa.GetType(tname), name, true );
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
  std::vector<SymbolicVal> args;
  AstNodePtr arrayExp;
  if (ArrayAnnotation::get_inst()->is_access_array_elem( fa, orig, &arrayExp, &args)) {
    anal.set_array_dimension( arrayExp, args.size());
    SymbolicFunctionDeclarationGroup elem;
    if (anal.is_array_construct_op( fa, arrayExp, 0, 0, 0, &elem)) {
      if (! elem.get_val( args, result))
        assert(false);
      result = ReplaceVal( result, *this);
    }
    else if (!fa.IsVarRef(arrayExp)) {
      AstNodePtr split = create_tmp_array( fa, arrayExp, "array");
      result = ArrayAnnotation::get_inst()->create_access_array_elem( split, args);
    }
  }
  else if (ArrayAnnotation::get_inst()->is_access_array_length( fa, orig, &arrayExp, &dim)) {
    SymbolicFunctionDeclarationGroup len;
    if (anal.is_array_construct_op(fa, arrayExp, 0, 0, &len)) {
      args.clear();
      args.push_back(dim);
      if (! len.get_val( args, result))
        assert(false);
      result = ReplaceVal( result, *this);
    }
    else if (!fa.IsVarRef(arrayExp)) {
      AstNodePtr split = create_tmp_array( fa, arrayExp, "array");
      result = ArrayAnnotation::get_inst()->create_access_array_length( split, dim);
    }
  }
  return result;
}

bool CollectArrayRef( CPPAstInterface& fa, ArrayInterface& anal, 
                      const AstNodePtr& array, AstInterface::AstNodeList& col)
{
    if (fa.IsVarRef(array)) {
       col.push_back(array);
       return true;
    }
    AstInterface::AstNodeList cur;
    if (!anal.is_array_construct_op( fa, array, &cur))
       return false;
    for (AstInterface::AstNodeList::iterator p = cur.begin();
         p != cur.end(); ++p) {
        if (!CollectArrayRef(fa, anal, *p, col))
             return false;
    }
    return true;
}

bool RewriteArrayModOp::
operator () ( AstInterface& _fa, const AstNodePtr& orig, AstNodePtr& result)
{
  CPPAstInterface& fa = static_cast<CPPAstInterface&>(_fa);
  if (!fa.IsStatement(orig)) {
    return false;
  }
  
  AstNodePtr modArray;
  int dimension = 0;
  SymbolicFunctionDeclarationGroup len, elem;
  bool reshape = false;
  if (!anal.is_array_mod_op( fa, orig, &modArray, &dimension, &len, &elem, &reshape)) 
    return false;

  SymbolicFunction::Arguments ivarList;
  for (int i = 0; i < dimension; ++i ) {
      AstNodeType t= fa.GetType("int");
      std:: string ivarname = fa.NewVar( t);
      AstNodePtr ivar = fa.CreateVarRef( ivarname);
      ivarList.push_back( SymbolicAstWrap(ivar));
  }
  SymbolicVal rhs;
  if (!elem.get_val( ivarList, rhs))
    assert(false);

  std::map<std::string,AstNodePtr> varmap;
  std::list<AstNodePtr> newStmts;
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
    AstNodePtr ivarAst;
    if (! ivarList[i].isAstWrap(ivarAst))
        assert(false);
    body = fa.CreateLoop( ivarAst, lb, ub, step, body, false);
  }

  RewriteModArrayAccess modArrayRewrite( fa, anal, stmt, lhsast, varmap, newStmts);
  TransformAstTraverse( fa, rhsast, modArrayRewrite);

  if (!reshape && newStmts.size() == 0) 
      result = body;
  else {
      result = fa.CreateBlock();
      for (std::list<AstNodePtr>::iterator p = newStmts.begin(); p != newStmts.end();
           ++p) {
         AstNodePtr cur = (*p);
         AstNodePtr ncur = cur;
         if (operator()(fa, cur, ncur))
             fa.BlockAppendStmt( result, ncur);
         else
             fa.BlockAppendStmt( result, cur);
      }
      if (reshape) {
         AstInterface::AstNodeList argList;
         for (int i = 0; i < dimension; ++i) {
           AstNodePtr curlen = lenlist[i].CodeGen(fa);
           argList.push_back( curlen);
         }
         AstNodePtr reshapeStmt = ArrayAnnotation::get_inst()->create_reshape_array(fa,modArray, argList);
         fa.BlockAppendStmt(result, reshapeStmt);
      }
      fa.BlockAppendStmt(result, body);
  }
std::cerr << "modarray rewrite: result = " << AstToString(result) << "\n";
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
operator() ( AstInterface& _fa, const AstNodePtr& orig, AstNodePtr& result)
{
  AstNodePtr array;
  AstNodePtr decl, body;
  int  dim;
  CPPAstInterface& fa = static_cast<CPPAstInterface&>(_fa);
  AstInterface::AstNodeList args, vars;
  if (fa.IsVariableDecl( orig, &vars)) {
     for (AstInterface::AstNodeList::iterator pv = vars.begin();
           pv!= vars.end(); ++pv) {
        AstNodePtr cur = *pv;
        if (! ArrayAnnotation::get_inst()->known_array( fa, cur))
           break; 
        AstNodePtr initdefs = anal.impl_array_opt_init(fa, cur);
        fa.InsertStmt( orig, initdefs, false, true);
     }
   }
   else  if (fa.IsFunctionDefinition( orig, 0, &vars,0, &body) && body !=0) {
      for (AstInterface::AstNodeList::iterator pv = vars.begin();
           pv!=vars.end(); ++pv) {
        AstNodePtr cur = *pv;
        if (! ArrayAnnotation::get_inst()->known_array( fa, cur))
           break; 
        anal.impl_array_opt_init(fa, cur, true);
      }
  }
  else if (ArrayAnnotation::get_inst()->is_access_array_elem( fa, orig, &array, &args )) {
     result = anal.impl_access_array_elem( fa, array, args);
     return true;
  }
  else if (ArrayAnnotation::get_inst()->is_reshape_array( fa, orig, &array, &args) ) {
    assert(fa.IsVarRef(array));
    AstNodePtr reshape = anal.impl_reshape_array( fa, array, args);
    reshape = TransformAstTraverse( fa, reshape, *this);
    result = fa.CreateBlock();
    fa.BlockAppendStmt( result, reshape);
    AstNodePtr initdefs = anal.impl_array_opt_init(fa, array);
    fa.BlockAppendStmt( result, initdefs);
    return true;
  }
  else if (ArrayAnnotation::get_inst()->is_access_array_length( fa, orig, &array, 0, &dim)) {
    assert(fa.IsVarRef(array));
    result = anal.impl_access_array_length( fa, array, dim);
    return true;
  }
  return false;
}


