// tps (12/09/2009) : Playing with precompiled headers in Windows. Requires rose.h as the first line in source files.
// tps : Switching from rose.h to sage3 
#include "sage3basic.h"
#include <sstream>
#include <iostream>
#include <string>
#include <LoopTransformInterface.h>
#include <LoopTransformOptions.h>
#include <ProcessAstTree.h>

#include <BreakupStmt.h>
#include <LoopUnroll.h>
#include <CommandOptions.h>

using namespace std;

bool LoopTransformation( LoopTransformInterface &fa, const AstNodePtr& head, 
                            AstNodePtr& result);  

int LoopTransformInterface::configIndex = 0;

void SetLoopTransformOptions( std::vector<std::string>& argvList)
{ 
  LoopTransformOptions::GetInstance()->SetOptions(argvList) ; 
}

bool ArrayUseAccessFunction::
IsArrayAccess( AstInterface& fa, const AstNodePtr& s, AstNodePtr* array, 
                                 AstInterface::AstNodeList* index) 
{
  AstInterface::AstNodeList args;
  if (prev != 0 &&  prev->IsArrayAccess(fa, s, array, index))
       return true;
  std::string sig;
  AstNodePtr f;
  if (fa.IsFunctionCall(s, &f,&args) && fa.IsVarRef(f,0,&sig) && sig == funcname) {
      AstInterface::AstNodeList::const_iterator p = args.begin();
      if (array != 0)
        *array = *p;
      if (index != 0) {
         for (++p; p != args.end(); ++p) {
            index->push_back(*p);
         }
      }
      return true;
  }
  return false;
}

bool ArrayUseAccessFunction::
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
  AstInterface::AstNodeList args;
  if (prev1 != 0 && prev1->get_modify(fa, fc, collect))
       return true;
  std::string sig;
  AstNodePtr f;
  if (fa.IsFunctionCall(fc, &f,&args) && fa.IsVarRef(f,0,&sig) && sig == funcname) {
       return true;
  }
  return false;
}

bool ArrayUseAccessFunction::get_read(AstInterface& fa, const AstNodePtr& fc,
                               CollectObject<AstNodePtr>* collect)
{
  AstInterface::AstNodeList args;
  if (prev1 != 0 && prev1->get_read(fa, fc, collect))
       return true;
  std::string sig;
  AstNodePtr f;
  if (fa.IsFunctionCall(fc, &f,&args) && fa.IsVarRef(f,0,&sig) && sig == funcname) {
       if (collect != 0)  {
           AstInterface::AstNodeList::const_iterator argp = args.begin();
           for ( ++argp; argp != args.end(); ++argp) {
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
     tmp.push_back(arr);
     return fa.CreateFunctionCall(funcname, tmp);
  }
  else 
     return fa.CreateArrayAccess(arr, index);
}

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
     vector<string>::const_iterator p = CmdOptions::GetInstance()->GetOptionPosition("-arracc");
     if (p != CmdOptions::GetInstance()->opts.end()) {
        string name;
        if (p->size() == 7) { // The argument is the next option
          ++p;
          assert (p != CmdOptions::GetInstance()->opts.end());
          name = *p;
        } else {
          name = p->substr(7);
        }
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
     if (!fa.IsStatement(head))
         return false;
     fa.SetRoot( head);
     LoopTransformInterface l( fa, aliasInfo, funcInfo, arrayInfo);
     return LoopTransformation(l, head, result);
  }
};

AstNodePtr LoopTransformTraverse( AstInterface& fa, const AstNodePtr& head, 
                            AliasAnalysisInterface& aliasInfo,
                            FunctionSideEffectInterface* funcInfo, 
                            ArrayAbstractionInterface* arrayInfo)
{
  LoopTransformInterface l(fa, aliasInfo, funcInfo, arrayInfo);

  BreakupStatement bs;
  AstNodePtr result = head;
  if (bs.cmdline_configure()) 
       result = bs(l,head);
  fa.SetRoot(result);

/* QY 11/8/05 privatizeScalar should be initialized somewhere else
  PrivatizeScalar ps;
  result = head;
  if (ps.cmdline_configure()) 
       result = ps(l,head);
  fa.SetRoot(result);
*/

  LoopTransformationWrap op(aliasInfo, funcInfo, arrayInfo);
  result = TransformAstTraverse( fa, result, op, AstInterface::PreVisit);
  fa.SetRoot(result);

  LoopUnrolling lu; 
  if (lu.cmdline_configure()) 
       result = lu(l,result);

  return result;
}

void PrintLoopTransformUsage( std::ostream& out)
{
  std::cerr << "-debugloop: print debugging information for loop transformations; \n";
  std::cerr << "-debugdep: print debugging information for dependence analysis; \n";
  std::cerr << "-tmloop: print timing information for loop transformations; \n";
  std::cerr << "-arracc <funcname>: use function <funcname> to denote multi-dimensional array access;\n";
  std::cerr << "opt <level=0>: the level of loop optimizations to apply; by default, only the outermost level is optimized;\n";
  std::cerr << LoopUnrolling::cmdline_help() << std::endl;
  std::cerr << BreakupStatement::cmdline_help() << std::endl;
  LoopTransformOptions::GetInstance()->PrintUsage(out);
}
// funcInfo could be an instance of ArrayAnnotation, which derives from FunctionSideEffectInterface
bool LoopTransformInterface::
GetFunctionCallSideEffect( const AstNodePtr& fc,
                     CollectObject<AstNodePtr>& collectmod,
                     CollectObject<AstNodePtr>& collectread)
{ return funcInfo!= 0 &&
         funcInfo->get_modify( *this, fc, &collectmod) &&
         funcInfo->get_read( *this, fc, &collectread); 
}

AstNodePtr LoopTransformInterface:: 
CreateDynamicFusionConfig( const AstNodePtr& groupNum, AstInterface::AstNodeList& args, int &id)
{ 
  std::string name = "DynamicFusionConfig";
  ++configIndex;
  args.push_front( fa.CreateConstInt( args.size() ) );
  args.push_front( fa.CreateConstInt(configIndex) );
  AstNodePtr invoc = fa.CreateFunctionCall( "DynamicFusionConfig",  args); 
  return fa.CreateAssignment ( groupNum, invoc) ;
}

AstNodePtr LoopTransformInterface::CreateDynamicFusionEnd( int id)
{
  AstInterface::AstNodeList args;
  args.push_back( fa.CreateConstInt(id));
  return fa.CreateFunctionCall("DynamicFusionEnd", args);
}

bool LoopTransformInterface::
IsDynamicFusionConfig( const AstNodePtr& n, AstNodePtr* configvar, int* configID,
                       AstInterface::AstNodeList* params)
{
  AstNodePtr invoc;
  if (!fa.IsAssignment(n, configvar, &invoc))
    return false;
  AstInterface::AstNodeList args;
  std::string sig;
  AstNodePtr f;
  if (!fa.IsFunctionCall(invoc, &f, &args) || !fa.IsVarRef(f, 0, &sig) )
    return false;
  if (sig == "DynamicFusionConfig") {
    if (configID != 0) {
      AstNodePtr idnode = args.front();
      bool isconst = fa.IsConstInt( idnode, configID);
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

bool LoopTransformInterface::IsDynamicFusionEnd(const AstNodePtr& n)
{
  std::string sig;
  AstNodePtr f;
  if (!fa.IsFunctionCall(n, &f) || !fa.IsVarRef(f, 0, &sig))
    return false;
  return sig == "DynamicFusionEnd";
}

bool LoopTransformInterface::
IsLoop( const AstNodePtr& s, SymbolicVal* init , SymbolicVal* cond,
                                SymbolicVal* incr, AstNodePtr* body)
{ 
  AstNodePtr initast, condast, incrast;
  if (!fa.IsLoop(s, &initast, &condast, &incrast, body))
      return false;
  if (init != 0 && initast != AST_NULL)
     *init = SymbolicValGenerator::GetSymbolicVal(*this,initast);
  if (cond != 0 && condast != AST_NULL)
     *cond = SymbolicValGenerator::GetSymbolicVal(*this,condast);
  if (incr != 0 && incrast != AST_NULL) 
       *incr = SymbolicValGenerator::GetSymbolicVal(*this,incrast);
  return true;
}
bool LoopTransformInterface::
IsFortranLoop( const AstNodePtr& s, SymbolicVar* ivar ,
                                SymbolicVal* lb , SymbolicVal* ub,
                                SymbolicVal* step, AstNodePtr* body)
{ 
  AstNodePtr ivarast, lbast, ubast, stepast, ivarscope;
  if (!fa.IsFortranLoop(s, &ivarast, &lbast, &ubast, &stepast, body))
      return false;
  std::string varname;
  if (! fa.IsVarRef(ivarast, 0, &varname, &ivarscope)) {
         return false; 
  }
  if (ivar != 0)
     *ivar = SymbolicVar(varname, ivarscope);
  if (lb != 0)
     *lb = SymbolicValGenerator::GetSymbolicVal(*this,lbast);
  if (ub != 0)
     *ub = SymbolicValGenerator::GetSymbolicVal(*this,ubast);
  if (step != 0) {
     if (stepast != AST_NULL)
       *step = SymbolicValGenerator::GetSymbolicVal(*this,stepast);
     else
       *step = SymbolicVal(1);
  }
  return true;
}
