#include <sstream>
#include <iostream>
#include <string>
#include <LoopTransformInterface.h>
#include <LoopTransformOptions.h>
#include <ProcessAstTree.h>

#include <BreakupStmt.h>
#include <LoopUnroll.h>
#include <CommandOptions.h>
#include <AutoTuningInterface.h>
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

//#define DEBUG 1

AstInterface* LoopTransformInterface::fa = 0;
int LoopTransformInterface::configIndex = 0;
AliasAnalysisInterface* LoopTransformInterface::aliasInfo = 0;
FunctionSideEffectInterface* LoopTransformInterface::funcInfo = 0;
ArrayAbstractionInterface* LoopTransformInterface::arrayInfo = 0;
AutoTuningInterface* LoopTransformInterface::tuning = 0;

using namespace std;

// This function is defined in TransformComputation.C.
extern bool LoopTransformation(const AstNodePtr& head, AstNodePtr& result);


//////////////////////
// Helper Functions //
//////////////////////

bool ArrayUseAccessFunction::
IsUniqueArray( AstInterface& fa, const AstNodePtr& array)
{
  if (prev != 0 &&  prev->IsUniqueArray(fa, array))
       return true;
  AstNodeType t;
  if (!fa.IsVarRef(array, &t)) assert(0);

  return false;
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

SymbolicVal ArrayUseAccessFunction::
CreateArrayAccess( const SymbolicVal& arr, const SymbolicVal& index)
{
   return SymbolicFunction(AstInterface::OP_ARRAY_ACCESS,funcname, arr,index);
}

AstNodePtr LoopTransformInterface::
CreateArrayAccess( const AstNodePtr& arr, AstInterface::AstList& index)
  { assert(fa != 0);
    AstNodePtr res;
    if (arrayInfo != 0){
        res = arrayInfo->CreateArrayAccess(*fa,arr,index); }
    if (res == AST_NULL) {
       res = arr;
       for (AstNodeList::const_reverse_iterator p = index.rbegin();
            p != index.rend(); ++p)
         { res = AstInterface::CreateArrayAccess(res, *p); }
    }
    return res;
  }

AstNodePtr ArrayUseAccessFunction::
CreateArrayAccess( AstInterface& fa, const AstNodePtr& arr,
                                const AstNodeList& index)
{
  AstNodePtr r;
  if (prev != 0)
     r =  prev->CreateArrayAccess(fa, arr, index);
  if (r == AST_NULL) {
    AstNodeList nindex;
    nindex.push_back(arr.get_ptr());
    for (AstNodeList::const_iterator p = index.begin(); p != index.end(); ++p)
       nindex.push_back(*p);
    r = fa.CreateFunctionCall(funcname,nindex.begin(), nindex.end()) ;
  }
  return r;
}

AstNodePtr LoopTransformInterface::
CreateArrayAccess(const std::string& arrname,
                            const std::vector<SymbolicVal>& arrindex)
  {
     assert(fa != 0);
     AstNodePtr res = fa->CreateVarRef(arrname);
     AstNodeList args;
     for (std::vector<SymbolicVal>::const_iterator indexp = arrindex.begin();
          indexp != arrindex.end(); ++indexp) {
        AstNodePtr cur = (*indexp).CodeGen(*fa);
        args.push_back(cur.get_ptr());
     }
    return CreateArrayAccess(res,args);
   }

class LoopTransformationWrap : public TransformAstTree
{
 public:
  bool operator()( AstInterface& fa, const AstNodePtr& head, AstNodePtr& result)
  {
#ifdef DEBUG
std::cerr << "LoopTransformationWrap:operator()\n";
#endif
     if (!fa.IsStatement(head))
         return false;
     fa.SetRoot( head);
     return LoopTransformation(head, result);
  }
};

void LoopTransformInterface:: set_astInterface( AstInterface& _fa)
{ fa = &_fa; }

void LoopTransformInterface::
set_tuningInterface(AutoTuningInterface* _tuning)
{ tuning = _tuning;
  if (arrayInfo != 0) tuning->set_arrayInfo(*arrayInfo);
}

void LoopTransformInterface::
cmdline_configure(std::vector<std::string>& argv)
{
  /*QY: in the following, argv will be modified to remove all the options
    not recognizable by ROSE, so that SLICE options won't be treated as    file names by the ROSE compiler */
  std::vector<std::string> unknown;

  LoopUnrolling::cmdline_configure(argv, &unknown) ;
  argv.clear();
  BreakupStatement::cmdline_configure(unknown,&argv);
  unknown.clear();
  LoopTransformOptions::GetInstance()->SetOptions(argv,&unknown) ;
  argv.clear();

  for (unsigned index=0; index < unknown.size(); ++index) {
        std::string opt=unknown[index];
        if (opt == "-arracc") {
           std::string name;
           if (index < unknown.size() && opt.size() == 7)
             name =  unknown[++index];
           else name = opt.substr(7);
           ArrayUseAccessFunction* r = new ArrayUseAccessFunction(name, arrayInfo, funcInfo);
//std::cerr << "SETTING ARR FUNC\n";
           funcInfo = r;
           arrayInfo = r;
           if (tuning != 0) tuning->set_arrayInfo(*r);
        }
        else if (opt == "-poet");
        else
        {
           argv.push_back(opt);
        }
      }
}

AstNodePtr LoopTransformInterface::
TransformTraverse(AstInterfaceImpl& scope,const AstNodePtr& head)
{
  assert(aliasInfo!=0);  /*QY: alias analysis should never be null*/
  AstInterface _fa(&scope);
  if (tuning != 0) tuning->set_astInterface(_fa);

  fa = &_fa;  /*QY: use static member variable to save the AstInterface*/

  NormalizeForLoop(_fa, head);
  AstNodePtr result = head;
  if (BreakupStatement::get_breaksize() > 0)
   {
       BreakupStatement bs;
       result = bs(head);
    }
  _fa.SetRoot(result);

/* QY 11/8/05 privatizeScalar should be initialized somewhere else
  PrivatizeScalar ps;
  result = head;
  if (ps.cmdline_configure())
       result = ps(l,head);
  _fa.SetRoot(result);
*/

  LoopTransformationWrap op;
  result = TransformAstTraverse(_fa, result, op, AstInterface::PreVisit);
  if (LoopUnrolling::get_unrollsize() > 1)
       result = LoopUnrolling()(result);
  _fa.SetRoot(result);

  if (tuning != 0)  tuning->ApplyOpt(_fa);
  fa = 0;
  return result;
}

void LoopTransformInterface::
PrintTransformUsage(std::ostream& __outstream)
{
  std::cerr << "-debugloop: print debugging information for loop transformations; \n"
            << "-debugdep: print debugging information for dependence analysis; \n"
            << "-tmloop: print timing information for loop transformations; \n"
            << "-arracc <funcname>: use function <funcname> to denote multi-dimensional array access;\n"
            << "opt <level=0>: the level of loop optimizations to apply; by default, only the outermost level is optimized;\n"
            << LoopUnrolling::cmdline_help() << std::endl
            << BreakupStatement::cmdline_help() << std::endl;
  LoopTransformOptions::GetInstance()->PrintUsage(__outstream);
}

//////////////////////////////////
// class LoopTransformInterface //
//////////////////////////////////

bool LoopTransformInterface::
GetFunctionCallSideEffect( const AstNodePtr& fc,
                     CollectObject<AstNodePtr>& collectmod,
                     CollectObject<AstNodePtr>& collectread)
{
    assert(fa != 0);
    return funcInfo!= 0
           && funcInfo->get_modify(*fa, fc, &collectmod)
           && funcInfo->get_read(*fa, fc, &collectread);
}


AstNodePtr LoopTransformInterface::
CreateDynamicFusionConfig( const AstNodePtr& groupNum, AstInterface::AstNodeList& args, int &id)
{ assert(fa != 0);
  std::string name = "DynamicFusionConfig";
  ++configIndex;
  args.push_back( fa->CreateConstInt(configIndex).get_ptr() );
  args.push_back( fa->CreateConstInt( args.size() ).get_ptr() );
  std::string funname = "DynamicFusionConfig";
  AstNodePtr invoc = fa->CreateFunctionCall( funname,  args.begin(), args.end());
  return fa->CreateAssignment ( groupNum, invoc) ;
}

AstNodePtr LoopTransformInterface::CreateDynamicFusionEnd( int id)
{ assert(fa != 0);
  AstInterface::AstNodeList args;
  args.push_back( fa->CreateConstInt(id).get_ptr());
  std::string funname = "DynamicFusionEnd";
  return fa->CreateFunctionCall(funname, args.begin(), args.end());
}

bool LoopTransformInterface::
IsDynamicFusionConfig( const AstNodePtr& n, AstNodePtr* configvar, int* configID,
                       AstInterface::AstNodeList* params)
{ assert(fa != 0);
  AstNodePtr invoc;
  if (!fa->IsAssignment(n, configvar, &invoc))
    return false;
  AstInterface::AstNodeList args;
  std::string sig;
  AstNodePtr f;
  if (!fa->IsFunctionCall(invoc, &f, &args) || !fa->IsVarRef(f, 0, &sig) )
    return false;
  if (sig == "DynamicFusionConfig") {
    if (configID != 0) {
      AstNodePtr idnode = args.front();
      bool isconst = fa->IsConstInt( idnode, configID);
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
  assert(fa != 0);
  std::string sig;
  AstNodePtr f;
  if (!fa->IsFunctionCall(n, &f) || !fa->IsVarRef(f, 0, &sig))
    return false;
  return sig == "DynamicFusionEnd";
}

bool
LoopTransformInterface::
IsLoop(const AstNodePtr& s, SymbolicVal* init , SymbolicVal* cond,
        SymbolicVal* incr, AstNodePtr* body)
{
  assert(fa != 0);
std::cerr << "### "  <<"--"<<init->toString()<<std::endl;
  AstNodePtr initast, condast, incrast;
  if (!fa->IsLoop(s, &initast, &condast, &incrast, body))
      return false;
  if (init != 0 && initast != AST_NULL)
     *init = SymbolicValGenerator::GetSymbolicVal(*fa,initast);
  if (cond != 0 && condast != AST_NULL)
     *cond = SymbolicValGenerator::GetSymbolicVal(*fa,condast);
  if (incr != 0 && incrast != AST_NULL)
       *incr = SymbolicValGenerator::GetSymbolicVal(*fa,incrast);
  return true;
}

