
#include <rose.h>
#include <general.h>

#include <iostream>
#include <string>
#include <LoopTransformInterface.h>
#include <LoopTransformOptions.h>
#include <ProcessAstTree.h>

#include <BreakupStmt.h>
#include <LoopUnroll.h>
#include <PrivatizeScalar.h>
#include <CommandOptions.h>

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// DQ (3/8/2006): Since this is not used in a header file it is OK here!
#define Boolean int

Boolean LoopTransformation( LoopTransformInterface &fa, const AstNodePtr& head, 
                            AstNodePtr& result);  

int LoopTransformInterface::configIndex = 0;

Boolean DebugLoop();

void SetLoopTransformOptions( vector<string>& argv)
{ 
  LoopTransformOptions::GetInstance()->SetOptions(argv) ; 
}

Boolean ArrayUseAccessFunction::
IsArrayAccess( AstInterface& fa, const AstNodePtr& s, AstNodePtr* array, 
                                 AstInterface::AstNodeList* index) 
{
  AstNodePtr func;
  AstInterface::AstNodeList args;
  if (prev != 0 &&  prev->IsArrayAccess(fa, s, array, index))
       return true;
  if (fa.IsFunctionCall(s, &func,&args) && fa.GetFunctionName(func) == funcname) {
      AstInterface::AstNodeListIterator p = fa.GetAstNodeListIterator(args);
      if (array != 0)
        *array = *p;
      if (index != 0) {
         for (++p; !p.ReachEnd(); ++p) {
            fa.ListAppend(*index, *p);
         }
      }
      return true;
  }
  return false;
}

Boolean ArrayUseAccessFunction::
GetArrayBound( AstInterface& fa, const AstNodePtr& array,
                                 int dim, int &lb, int &ub)
{
  if (prev != 0)
     return prev->GetArrayBound(fa, array, dim, lb, ub);
  return fa.GetArrayBound(array, dim, lb, ub);
}

bool ArrayUseAccessFunction::get_modify(AstInterface& fa, const AstNodePtr& fc,
                               CollectObject<AstNodePtr>* collect)
{
  AstNodePtr func;
  AstInterface::AstNodeList args;
  if (prev1 != 0 && prev1->get_modify(fa, fc, collect))
       return true;
  else if (fa.IsFunctionCall(fc, &func,&args) && fa.GetFunctionName(func) == funcname) {
       return true;
  }
  return false;
}

bool ArrayUseAccessFunction::get_read(AstInterface& fa, const AstNodePtr& fc,
                               CollectObject<AstNodePtr>* collect)
{
  AstNodePtr func;
  AstInterface::AstNodeList args;
  if (prev1 != 0 && prev1->get_read(fa, fc, collect))
       return true;
  else if (fa.IsFunctionCall(fc, &func,&args) && fa.GetFunctionName(func) == funcname) {
       if (collect != 0)  {
           //(*collect)(fc);
           AstInterface::AstNodeListIterator argp = fa.GetAstNodeListIterator(args);
           for ( ++argp; !argp.ReachEnd(); ++argp) {
              (*collect)(*argp);
           }
       }
       return true;
  }
  return false;
}

AstNodePtr ArrayUseAccessFunction::
CreateArrayAccess( AstInterface& fa, const AstNodePtr& arr,
                                AstInterface::AstNodeList& index)
{
  if (prev != 0)
     return prev->CreateArrayAccess(fa, arr, index);
  if (index.size() > 1) {
     AstInterface::AstNodeList tmp = index;
     fa.ListPrepend(tmp, arr);
     return fa.CreateFunctionCall(funcname, "", tmp);
  }
  else 
     return fa.CreateArrayAccess(arr, index);
}

class NormalizeLoopTraverse : public ProcessAstTree
{
  bool succ;
  virtual Boolean ProcessLoop(AstInterface &fa, const AstNodePtr& s,
                               const AstNodePtr& body,
                               AstInterface::TraversalVisitType t)
   {
    if (t == AstInterface::PreVisit) {
      SgForStatement *fs = isSgForStatement(s);
      if (fs == 0) { succ = false; return true; }

      SgStatementPtrList &init = fs->get_init_stmt();
      if (init.size() != 1)  { succ = false; return true; }

      SgExpression* test = fs->get_test_expr();

   // DQ (12/13/2006): A SgForStatement should always have a valid 
   // test expression (SgExpressionStmt containing an expression).
      ROSE_ASSERT(test != NULL);

      AstNodePtr testlhs, testrhs;
      if (! fa.IsBinaryOp(test, &testlhs, &testrhs))
         return false;
      switch (test->variantT()) {
        case V_SgLessThanOp:
             fa.ReplaceAst( test,
                          fa.CreateRelLE( fa.CopyAstTree(testlhs),
                                       fa.CreateBinaryMinus( fa.CopyAstTree(testrhs),
                                                             fa.CreateConstInt(1)))); 
              break;
        case V_SgGreaterThanOp:
             fa.ReplaceAst( test,
                       fa.CreateRelGE( fa.CopyAstTree(testlhs), 
                                       fa.CreateBinaryPlus( fa.CopyAstTree(testrhs),
                                                           fa.CreateConstInt(1))));
             break;
        case V_SgLessOrEqualOp:
        case V_SgGreaterOrEqualOp:
        case V_SgNotEqualOp:
            break;
        default:
           succ = false; return true;
      }

      SgExpression* incr = fs->get_increment();
      switch (incr->variantT()) {
        case V_SgPlusPlusOp:
           if (! fa.IsSameVarRef(isSgPlusPlusOp(incr)->get_operand(), testlhs))
               { succ = false; return true; }
           fa.ReplaceAst( incr, fa.CreateBinaryOP("+=", fa.CopyAstTree(testlhs),
                                                  fa.CreateConstInt(1)));
           break;
        case V_SgMinusMinusOp:
           if (! fa.IsSameVarRef(isSgMinusMinusOp(incr)->get_operand(), testlhs))
               { succ = false; return true; }
           fa.ReplaceAst( incr, fa.CreateBinaryOP("+=", fa.CopyAstTree(testlhs),
                                                  fa.CreateConstInt(-1)));
           break;
        default:
           succ =  false; return true;
      }
      succ = true;
     }
     return true;
   }
 public:
   Boolean operator () ( AstInterface& li, const AstNodePtr& root)
    {
       succ = true;
       ProcessAstTree::operator()(li, root);
       return succ;
    }
};

class LoopTransformationWrap : public TransformAstTree
{
  AliasAnalysisInterface& aliasInfo;
  FunctionSideEffectInterface* funcInfo;
  ArrayAbstractionInterface* arrayInfo;
 public:
  LoopTransformationWrap(  AliasAnalysisInterface& alias, FunctionSideEffectInterface* func = 0,
                           ArrayAbstractionInterface* array = 0)
     : aliasInfo(alias), funcInfo(func), arrayInfo(array) 
   {
     if (DebugLoop()) {
       cerr << "Starting LoopTransformationWrap" << endl;
     }
     const vector<string>& opts = CmdOptions::GetInstance()->GetOptions();
     unsigned int p = CmdOptions::GetInstance()->HasOption("-arracc");
     if (p != 0) {
        // p now points to the NEXT argument
	if (p == opts.size()) {cerr << "Bad syntax in LoopTransformationWrap" << endl; ROSE_ABORT();}
        string name = opts[p];
        ArrayUseAccessFunction* r = new ArrayUseAccessFunction(name, array, func);
        funcInfo = r;
        arrayInfo = r;
     } 
   }
  ~LoopTransformationWrap()
   {
     //if (CmdOptions::GetInstance()->HasOption("-arracc"))
        //delete arrayInfo;
   }
  bool operator()( AstInterface& fa, const AstNodePtr& head, AstNodePtr& result)
  {  
     if (DebugLoop()) {
       cerr << "LoopTransformationWrap::operator() " << head << endl;
     }
     if (isSgStatement(head) == 0)
         return false;
     if (DebugLoop()) {
       cerr << "Found statement in LoopTransformationWrap" << endl;
     }
     fa.SetRoot( head);
     LoopTransformInterface l( fa, aliasInfo, funcInfo, arrayInfo);
     if (DebugLoop()) {
       cerr << "Before LoopTransformation/3" << endl;
     }
     return LoopTransformation(l, head, result);
  }
};

AstNodePtr LoopTransformTraverse( AstInterface& fa, SgNode *head, 
                            AliasAnalysisInterface& aliasInfo,
                            FunctionSideEffectInterface* funcInfo, 
                            ArrayAbstractionInterface* arrayInfo)
{
  AstNodePtr result = head;

  if (DebugLoop()) {
    cerr << "LoopTransformTraverse " << result << endl;
  }
  ROSE_ASSERT (result);
  ROSE_ASSERT (result->get_parent());
  LoopTransformInterface l(fa, aliasInfo, funcInfo, arrayInfo);
  NormalizeLoopTraverse()(l, result);
  ROSE_ASSERT (result->get_parent());

  BreakupStatement bs;
  ROSE_ASSERT (result);
  if (bs.cmdline_configure()) 
       result = bs(l,result);
  fa.SetRoot(result);
  ROSE_ASSERT (result);
  ROSE_ASSERT (result->get_parent());

  PrivatizeScalar ps;
  result = result;
  if (ps.cmdline_configure()) 
       result = ps(l,result);
  fa.SetRoot(result);
  ROSE_ASSERT (result);
  ROSE_ASSERT (result->get_parent());

  if (DebugLoop()) {
    cerr << "Before LoopTransformationWrap" << endl;
  }
  LoopTransformationWrap op(aliasInfo, funcInfo, arrayInfo);
  result = TransformAstTraverse( fa, result, op, AstInterface::PreVisit);
  fa.SetRoot(result);
  ROSE_ASSERT (result);
  ROSE_ASSERT (result->get_parent());

  LoopUnrolling lu; 
  if (lu.cmdline_configure()) 
       result = lu(l,result);
  ROSE_ASSERT (result->get_parent());

  cerr << "LoopTransformTraverse: changed " << head << " to " << result << endl;

  return result;
}

void PrintLoopTransformUsage( ostream& out)
{
  cerr << "-debugloop: print debugging information for loop transformations; \n";
  cerr << "-debugdep: print debugging information for dependence analysis; \n";
  cerr << "-tmloop: print timing information for loop transformations; \n";
  cerr << "-arracc <funcname>: use function <funcname> to denote multi-dimensional array access;\n";
  cerr << "opt <level=0>: the level of loop optimizations to apply; by default, only the outermost level is optimized;\n";
  cerr << LoopUnrolling::cmdline_help() << endl;
  cerr << BreakupStatement::cmdline_help() << endl;
  LoopTransformOptions::GetInstance()->PrintUsage(out);
}

Boolean LoopTransformInterface::
GetFunctionCallSideEffect( const AstNodePtr& fc,
                     CollectObject<AstNodePtr>& collectmod,
                     CollectObject<AstNodePtr>& collectread)
{ return funcInfo!= 0 &&
         funcInfo->get_modify( *this, fc, &collectmod) &&
         funcInfo->get_read( *this, fc, &collectread); 
}


bool LoopTransformInterface::
IsFortranLoop( const AstNodePtr& s, SymbolicVar* ivar ,
                                SymbolicVal* lb , SymbolicVal* ub,
                                SymbolicVal* step, AstNodePtr* body)
{ 
   SgForStatement *fs = isSgForStatement(s);
   if (fs == 0) {
      return false;
    }

    SgStatementPtrList &init = fs->get_init_stmt();
    if (init.size() != 1) return false;
    AstNodePtr ivarast, ivarscope, lbast, ubast, stepast;
    if (!AstInterface::IsAssignment( init.front(), &ivarast, &lbast)) {
      if (DebugLoop())
           cerr << "init not assignment: " << AstInterface::AstToString(init.front()) << endl;
         return false;
    }
      
    string varname;
      if (! AstInterface::IsVarRef(ivarast, 0, &varname, &ivarscope)) {
         if (DebugLoop())
            cerr << "lhs of init not variable: " << AstInterface::AstToString(ivarast) << endl;
         return false; 
      }

      SgExpression* test = fs->get_test_expr();

   // DQ (12/13/2006): A SgForStatement should always have a valid 
   // test expression (SgExpressionStmt containing an expression).
      ROSE_ASSERT(test != NULL);

      AstNodePtr testlhs;
      string testvarname;
      if (! AstInterface::IsBinaryOp(test, &testlhs, &ubast) 
          || !AstInterface::IsVarRef(testlhs, 0, &testvarname) ||
              varname != testvarname) {
         if (DebugLoop())
           cerr << "lhs of init same as lhs of test: " << endl;
         return false;
      }
      int t = test->variantT();
      switch (t) {
        case V_SgLessOrEqualOp:
        case V_SgGreaterOrEqualOp:
        case V_SgNotEqualOp:
            break;
        default:  
           if (DebugLoop())
             cerr << "test op not recognized " << endl;
           return false;
      }
      SgExpression* incr = fs->get_increment();
      AstNodePtr incrlhs;
      string incrvarname;
      if (! AstInterface::IsBinaryOp(incr, &incrlhs, &stepast) 
             || !AstInterface::IsVarRef(incrlhs, 0, &incrvarname) ||
              varname != incrvarname) {
        if (DebugLoop())
            cerr << "lhs of init same as lhs of test: " << endl;
         return false;
      }
      switch (incr->variantT()) {
        case V_SgPlusAssignOp:
            break;
        default:
           if (DebugLoop())
              cerr << "incr op not recognized " << endl;
           return false;
      }
     if (ivar != 0)
        *ivar = SymbolicVar(varname, ivarscope);
     if (lb != 0)
        *lb = SymbolicValGenerator::GetSymbolicVal(fa,lbast);
     if (ub != 0)
        *ub = SymbolicValGenerator::GetSymbolicVal(fa,ubast);
     if (step != 0)
        *step = SymbolicValGenerator::GetSymbolicVal(fa,stepast);

      if (body != 0) {
        *body = fs->get_loop_body();
      }
      return true;
    }

AstNodePtr LoopTransformInterface:: 
CreateDynamicFusionConfig( const AstNodePtr& groupNum, AstInterface::AstNodeList& args, int &id)
{ 
  AstNodePtr dtFusionConfig = fa.CreateFunction("DynamicFusionConfig", "int DynamicFusionConfig(int id, int num, ...);");

  for (AstInterface::AstNodeList::iterator p = args.begin(); p != args.end(); ++p) {
     SgNode *n = *p;
     *p = fa.CreateUnaryOP( "&", n);
  }
  id = ++configIndex;
  args.push_front( fa.CreateConstInt( args.size() ) );
  args.push_front( fa.CreateConstInt(id) );
  AstNodePtr invoc = fa.CreateFunctionCall( dtFusionConfig, args); 
  return fa.CreateAssignment ( groupNum, invoc) ;
}

AstNodePtr LoopTransformInterface::CreateDynamicFusionEnd( int id)
{
  AstNodePtr dtFusionEnd = fa.CreateFunction("DynamicFusionEnd", "int DynamicFusionEnd(int id);");

  AstInterface::AstNodeList args = fa.CreateList();
  args.push_back( fa.CreateConstInt(id));
  return fa.CreateFunctionCall(dtFusionEnd, args);
}

Boolean LoopTransformInterface::
IsDynamicFusionConfig( const AstNodePtr& n, AstNodePtr* configvar, int* configID,
                       AstInterface::AstNodeList* params)
{
  AstNodePtr invoc;
  if (!AstInterface::IsAssignment(n, configvar, &invoc))
    return false;
  AstNodePtr func; 
  AstInterface::AstNodeList args;
  if (!AstInterface::IsFunctionCall(invoc, &func, &args))
    return false;
  if (fa.GetFunctionName( func ) == "DynamicFusionConfig") {
    if (configID != 0) {
      AstNodePtr idnode = args.front();
      bool isconst = AstInterface::IsConstInt( idnode, configID);
      assert(isconst);
    } 
    if (params != 0) {
      *params = args;
      params->erase(params->begin()); 
      params->erase(params->begin());
    }
   return true;
  }
  return false;
}

Boolean LoopTransformInterface::IsDynamicFusionEnd(const AstNodePtr& n)
{
  SgExprStatement *stmt = isSgExprStatement(n);
  if (stmt == 0)
    return false;
  SgExpression *exp = stmt->get_expression();
  AstNodePtr func;
  if (!AstInterface::IsFunctionCall(exp, &func))
    return false;
  return fa.GetFunctionName( func) == "DynamicFusionEnd";
}

