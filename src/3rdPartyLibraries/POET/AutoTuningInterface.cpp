#include <fstream>
#include <AutoTuningInterface.h>
#include <LoopTransformInterface.h>
#include <LoopTree.h>
#include <BlockingAnal.h>
#include <SymbolicExpr.h>

#define DEBUG

/*QY: poet header files */
#include <poet_AST.h>
#include <poet_ASTfactory.h>
/***********************************************************************/
/******QY: macros for creating POET instructions************************/
/***********************************************************************/

#define PAR_PARAM_NAME(handleName)  "pthread_" + handleName
#define PAR_BLOCK_NAME(handleName)  "psize_" + handleName

#define ICONST(val) POETProgram::make_Iconst(val)
#define RANGE_TYPE(lb,ub) POETProgram::make_rangeType(lb,ub)
#define LIST_TYPE(elem) POETProgram::make_listType(elem)
#define INT_TYPE POETProgram::make_atomType(TYPE_INT)
#define EVAL_VAR(poet, name) (poet).make_evalVar(STRING(name))
#define PARAM_VAR(name) POETProgram::make_paramVar(STRING(name))
#define HAS_PARAM(name) POETProgram::find_paramVar(STRING(name))
#define TRACE_VAR(name) POETProgram::make_traceVar(STRING(name))
#define HAS_TRACE(name) POETProgram::find_traceVar(STRING(name))
#define CONFIG_VAR(scope,name) POETProgram::make_localVar(scope->get_entry().get_symTable(),STRING(name), LVAR_TUNE)
#define ATTR(var,fd) POETProgram::make_attrAccess(var,STRING(fd))
#define XFORM_VAR(name)  POETProgram::make_xformVar(STRING(name))
#define CODE_SET_ATTR(code,name,val)  code->get_entry().append_attr(poet->set_local_static(code->get_entry().get_symTable(),STRING("match"), LVAR_ATTR, val, val, true))

#define BOP(op,opd1,opd2) poet.make_Bop(op, opd1, opd2)
#define TOR(opd1,opd2) poet.make_typeTor(opd1, opd2)
#define ERASE(opd)  BOP(POET_OP_ERASE, opd, opd)
#define COPY(opd)  POETProgram::make_Uop(POET_OP_COPY, opd)
#define REPLACE(opd1,opd2,opd3)  POETProgram::make_Top(POET_OP_REPLACE, opd1, opd2,opd3)
#define DELAY(opd)  POETProgram::make_Uop(POET_OP_DELAY, opd)
#define APPLY(opd)  POETProgram::make_Uop(POET_OP_APPLY, opd)
#define TUPLE_ACC(var,acc) POETProgram::make_tupleAccess(var,acc)
#define TUPLE_APPEND(v1,v2) poet.append_tuple(v1,v2)
#define TUPLE5(v1,v2,v3,v4,v5)  POETProgram::make_tuple5(v1,v2,v3,v4,v5)
#define ASSIGN(lhs,rhs) new POETAssign(lhs,rhs)
#define INVOKE(xform,config,param) new XformVarInvoke(xform->get_entry(),config,param)
#define CAR(v)  POETProgram::make_Uop(POET_OP_CAR, v)
#define CDR(v)  POETProgram::make_Uop(POET_OP_CDR, v)

#define STMT(exp,lineNo) poet.make_Uop(POET_OP_STMT,exp,lineNo)
#define SEQ(poet,s1,s2) (poet).make_Bop(POET_OP_SEQ,s1,s2)
#define SEQ3(poet,s1,s2,s3) SEQ(poet,(poet).make_Bop(POET_OP_SEQ,s1,s2),s3)
#define SEQ4(poet,s1,s2,s3,s4) SEQ3(poet,(poet).make_Bop(POET_OP_SEQ,s1,s2),s3,s4)
#define SEQ5(poet,s1,s2,s3,s4,s5) SEQ4(poet,(poet).make_Bop(POET_OP_SEQ,s1,s2),s3,s4,s5)
#define SEQ6(poet,s1,s2,s3,s4,s5,s6) SEQ5(poet,(poet).make_Bop(POET_OP_SEQ,s1,s2),s3,s4,s5,s6)
#define TRACE_EVAL(vars,op) poet.make_Bop(POET_OP_TRACE, vars, op)
#define IFELSE(cond,s1,s2) new POETTop(POET_OP_IFELSE, cond, s1, s2)
#define ARRAY_REF(arr,sub) CODE_ACC("ArrayAccess", PAIR(arr,sub))

#define MAIN_EVAL(poet, exp,lineNo) (poet).insert_evalDecl(exp, lineNo)
#define DECL_TRACE(poet,exp,lineNo) (poet).insert_traceDecl(exp,lineNo)

// POET initialization.
EvaluatePOET* evalPOET = 0;     // needed to link poet

inline void InsertParamDecl(POETProgram& poet, LocalVar* par, const std::string& msg, int lineNo)
{
   ParameterDecl* paramDecl = poet.insert_parameterDecl(par, lineNo);
   paramDecl->set_message(STRING(msg));
   paramDecl->set_type(par->get_entry().get_restr());
   paramDecl->set_default(par->get_entry().get_code());
}

/***********************************************************************/
/*******QY: static variables **************************************/
/***********************************************************************/

namespace POET_static
{
  POETCode* Zero = ICONST(0);
  POETCode* One = ICONST(1);

  XformVar* const copyarray = XFORM_VAR("CopyRepl");
  LocalVar* const copyarray_bufname=CONFIG_VAR(copyarray,"prefix");
  LocalVar* const copyarray_initloc=CONFIG_VAR(copyarray,"init_loc");
  LocalVar* const copyarray_saveloc=CONFIG_VAR(copyarray,"save_loc");
  LocalVar* const copyarray_deleteloc=CONFIG_VAR(copyarray,"delete_loc");
  LocalVar* const copyarray_elemtype=CONFIG_VAR(copyarray,"data_type");
  LocalVar* const copyarray_isscalar=CONFIG_VAR(copyarray,"scalar");
  LocalVar* const copyarray_trace=CONFIG_VAR(copyarray,"trace");
  LocalVar* const copyarray_tracedecl=CONFIG_VAR(copyarray,"trace_decl");
  LocalVar* const copyarray_tracevars=CONFIG_VAR(copyarray,"trace_vars");
  LocalVar* const copyarray_tracemod=CONFIG_VAR(copyarray,"trace_mod");
  LocalVar* const copyarray_permute=CONFIG_VAR(copyarray,"permute");
  LocalVar* const copyarray_cpBlock=CONFIG_VAR(copyarray,"cpBlock");

  XformVar* const block_xvar = XFORM_VAR("BlockLoops");
  LocalVar* const block_cleanup=CONFIG_VAR(block_xvar,"cleanup");
  LocalVar* const block_factor =CONFIG_VAR(block_xvar,"factor");
  LocalVar* const block_tracemod=CONFIG_VAR(block_xvar,"trace_mod");
  LocalVar* const block_tile=CONFIG_VAR(block_xvar,"trace_innerNest");
  LocalVar* const block_decl=CONFIG_VAR(block_xvar,"trace_decl");
  LocalVar* const block_nonperfect=CONFIG_VAR(block_xvar,"nonPerfect");

  CodeVar* const Array = CODE_VAR("ArrayAccess");
  POETCode* const Array_var = ATTR(Array, "array");
  POETCode* const Array_sub = ATTR(Array, "subscript");

  CodeVar* const FunctionCall = CODE_VAR("FunctionCall");
  POETCode* const FunctionCall_arg = ATTR(FunctionCall, "args");

  XformVar* const traceNest=XFORM_VAR("TraceNestedLoops");
  POETCode* const traceNest_trace=CONFIG_VAR(traceNest,"trace");

  XformVar* const moveHandle= XFORM_VAR("MoveTraceHandle");
  LocalVar* const moveHandle_trace= CONFIG_VAR(moveHandle,"trace");

  XformVar* const eraseHandle= XFORM_VAR("EraseTraceHandle");
  LocalVar* const eraseHandle_repl= CONFIG_VAR(eraseHandle,"repl");

  XformVar* const appendDecl=XFORM_VAR("AppendDecl");
  LocalVar* const appendDecl_trace= CONFIG_VAR(appendDecl,"trace");

  CodeVar* const Nest = CODE_VAR("Nest");
  CodeVar* const Loop = CODE_VAR("Loop");
  CodeVar* const IntType = CODE_VAR("IntegerType");
  POETCode* const Nest_ctrl = ATTR(Nest, "ctrl");
  POETCode* const Nest_body = ATTR(Nest, "body");
  POETCode* const Loop_ivar=ATTR(Loop,"i");
  POETCode* const Loop_step=ATTR(Loop,"step");
  POETCode* const Loop_ub=ATTR(Loop,"stop");

  XformVar* const  finiteDiff= XFORM_VAR("FiniteDiff");
  LocalVar* const  fd_exp_type= CONFIG_VAR(finiteDiff,"exp_type");
  LocalVar* const  fd_prefix= CONFIG_VAR(finiteDiff,"prefix");
  LocalVar* const  fd_trace= CONFIG_VAR(finiteDiff,"trace");
  LocalVar* const  fd_traceVar= CONFIG_VAR(finiteDiff,"trace_newVars");
  LocalVar* const  fd_traceDecl= CONFIG_VAR(finiteDiff,"trace_decl");
  LocalVar* const  fd_is_scalar= CONFIG_VAR(finiteDiff,"scalar");
  LocalVar* const  fd_mod= CONFIG_VAR(finiteDiff,"trace_mod");
  LocalVar* const  fd_permute= CONFIG_VAR(finiteDiff,"permute");

  XformVar* const unroll = XFORM_VAR("UnrollLoops");
  LocalVar* const unroll_factor=CONFIG_VAR(unroll,"factor");
  LocalVar* const unroll_cleanup=CONFIG_VAR(unroll,"cleanup");

  XformVar* const parloop = XFORM_VAR("ParallelizeLoop");
  LocalVar* const  par_trace= CONFIG_VAR(parloop,"trace");
  LocalVar* const  par_include= CONFIG_VAR(parloop,"trace_include");
  LocalVar* const  par_thread= CONFIG_VAR(parloop,"threads");
  LocalVar* const  par_private= CONFIG_VAR(parloop,"private");

  XformVar* const unrollJam = XFORM_VAR("UnrollJam");
  LocalVar* const unrollJam_cleanup = CONFIG_VAR(unrollJam,"cleanup");
  LocalVar* const unrollJam_factor=CONFIG_VAR(unrollJam,"factor");
  LocalVar* const unrollJam_trace=CONFIG_VAR(unrollJam,"trace");

  XformVar* const cleanup = XFORM_VAR("CleanupBlockedNests");
  LocalVar* const cleanup_trace = CONFIG_VAR(cleanup,"trace");

  /*QY loop-based strength reduction optimization; return the invocation */
  POETCode* gen_fdInvoke(POETProgram& poet, LocalVar* top, POETCode* target,
                   const std::string& nvarName, POETCode* exp,
                   POETCode* expType, const std::vector<POETCode*>& dimVec, 
                   POETCode* permute=0, POETCode* traceMod=0,
                   bool is_scalar=true); 
  /*QY: return the permutation configuration for strength reduction */
   POETCode* gen_permute(std::vector<int>& permuteVec);
};

ArrayAbstractionInterface* AutoTuningInterface::arrayInfo = 0;
POETCode* AutoTuningInterface::arrayAccess = 0;

int CopyArraySpec::index=0;

/***********************************************************************/
/*QY: symbolic value conversion to POET values */
/***********************************************************************/

class POET_AstNodePtr : public AstNodePtr
{
 public:
  POET_AstNodePtr( POETCode* n = 0) { AstNodePtr::repr = n; }
  POET_AstNodePtr( const AstNodePtr& that) : AstNodePtr(that) {}
  POET_AstNodePtr& operator = (const AstNodePtr &that)
      { AstNodePtr::operator = (that); return *this; }
  ~POET_AstNodePtr() {}
  POETCode* get_ptr() const { return static_cast<POETCode*>(repr); }
  POETCode* operator -> () const { return static_cast<POETCode*>(repr); }
};

class SymbolicPOETWrap : public SymbolicAstWrap
{
  SymbolicValImpl* Clone() const { return new SymbolicPOETWrap(*this); }
public:
  SymbolicPOETWrap(POETCode* code)
    : SymbolicAstWrap( POET_AstNodePtr(code)) {}

  POETCode* get_ast() const { return POET_AstNodePtr(SymbolicAstWrap::get_ast()).get_ptr(); }
  virtual std:: string GetTypeName() const { return "poet_astwrap"; }
  virtual void Dump() const { std::cerr << toString(); }
  virtual std:: string toString() const { return get_ast()->toString(); }
};

class SymbolicVal2POET : public SymbolicVisitor
{
  POETProgram& poet;
  POETCode* res;
  SymbolicVal val;
  virtual void Default() 
       { 
         std::cerr << "Cannot support this yet: " << val.GetTypeName() << ":" << val.toString() << "\n";
         assert(0);
       }
  virtual void VisitAstWrap( const SymbolicAstWrap& v) 
       { 
          const SymbolicPOETWrap* v2=dynamic_cast<const SymbolicPOETWrap*>(&v);
          if (v2 != 0) res = v2->get_ast();  
          else {
            res=STRING(AstInterface::AstToString(v.get_ast()));
          }
       }

  virtual void VisitConst(const SymbolicConst& v)
      {
         int ival = 0, frac=0;
         if (v.GetIntVal(ival,frac) && frac==1) 
              res = ICONST(ival);
         else res = STRING(v.GetVal());
      }
   virtual void VisitFunction(const SymbolicFunction& v)
     {
        if (v.GetOp() == "/") 
          { res = BOP(POET_OP_DIVIDE,apply(v.get_arg(0)),apply(v.get_arg(1))); return; }
        POETCode* args = 0;
        for (int i=v.NumOfArgs()-1; i >= 0; --i) {
           args = LIST(apply(v.get_arg(i)),args);
        }
        res = CODE_ACC("FunctionCall", PAIR(STRING(v.GetOp()),args));
     }
   virtual void VisitVar(const SymbolicVar& v)
      {  res = STRING(v.GetVarName()); }
   virtual void VisitExpr( const SymbolicExpr& exp)
      {
         SymOpType t = exp.GetOpType();
         switch (t) 
         {
         case SYMOP_MULTIPLY:
         case SYMOP_PLUS:
            {
                POETOperatorType pt = (t==SYMOP_MULTIPLY)? POET_OP_MULT : POET_OP_PLUS;
                SymbolicExpr::OpdIterator p = exp.GetOpdIterator();
                POETCode* cur = apply(exp.Term2Val(p.Current()));
                for (p.Advance(); !p.ReachEnd(); p.Advance())
                   cur = BOP(pt, cur, apply(exp.Term2Val(p.Current())));
                res = cur;
                return;
            } 
         case SYMOP_MIN:
         case SYMOP_MAX: 
            {
                std::string pt = (t==SYMOP_MIN)? "MIN" : "MAX";
                SymbolicExpr::OpdIterator p = exp.GetOpdIterator();
                POETCode* cur = apply(exp.Term2Val(p.Current()));
                for (p.Advance(); !p.ReachEnd(); p.Advance())
                   cur = CODE_ACC(pt, PAIR(cur, apply(exp.Term2Val(p.Current()))));
                res = cur;
                return;
            } 
         case SYMOP_POW: 
              std::cerr << "Not handled yet: " << exp.toString() << "\n";
              assert(0);
         }
      }
  public:
   SymbolicVal2POET(POETProgram& _poet) : poet(_poet), res(0) {}
   POETCode* apply(const SymbolicVal& _val)
   { res = 0; val = _val; val.Visit(this); return res; }
};

/***********************************************************************/
/******QY: Loop Handle Map **********************************/
/***********************************************************************/

LocalVar* HandleMap:: NewLoopHandle()
{
     ++loopindex; 
     std::stringstream out;
     out << "nest" << loopindex;
     LocalVar* res = TRACE_VAR(out.str());
     return res;
}

LocalVar* HandleMap:: NewBodyHandle()
{
     ++bodyindex; 
     std::stringstream out;
     out << "body" << bodyindex;
     LocalVar* res = TRACE_VAR(out.str());
     return res;
}

LocalVar* HandleMap:: 
GetLoopHandle(AstInterface &fa, const AstNodePtr& loop)
{
   AstMapType::const_iterator p = astMap.find(loop.get_ptr());
   if (p == astMap.end()) {
     LocalVar* res = NewLoopHandle();
     astMap[loop.get_ptr()] = res;
     std::string name = res->toString(OUTPUT_NO_DEBUG);
     fa.InsertAnnot(loop, "//@; BEGIN("+name+"=Nest)",true);
     return res;
   }
   return (*p).second;
}
 
LocalVar* HandleMap:: GetBodyHandle(LoopTreeNode* loop, LocalVar* loophandle)
{
   std::map<LocalVar*,LocalVar*>::const_iterator p = bodyMap.find(loophandle);
   if (p == bodyMap.end()) {
     LocalVar* res = NewBodyHandle();
     bodyMap[loophandle] = res;
     loophandle->get_entry().set_restr(res);
     std::string name = res->toString(OUTPUT_NO_DEBUG);
     if (!loop->set_postAnnot("/*@; BEGIN("+name+"=Stmt) @*/"))
         assert(0);
     return res;
   }
   return (*p).second;
}

void HandleMap::
ObserveCopyAst(AstInterfaceImpl& fa, 
      const AstNodePtr& orig, const AstNodePtr& n)
{
  AstMapType::const_iterator p = astMap.find(orig.get_ptr());
#ifdef DEBUG_COPY
std::cerr << "from AST copying " << orig.get_ptr() << " => " << n.get_ptr() << "\n";
#endif
  if (p != astMap.end()) {
     LocalVar* handle = (*p).second; 
     astMap[n.get_ptr()] = handle;
  }
}

void HandleMap::UpdateCodeGen(const LoopTreeCodeGenInfo& info)
{
   const LoopTreeNode* loop = info.GetObserveNode();
   AstNodePtr res = info.GetAST(); 
   LocalVar* handle = loopMap[loop];
   assert(handle != 0);
   astMap[res.get_ptr()] = handle;
#ifdef DEBUG_MAP
std::cerr << "from loop tree: maping " << loop->toString() << " => " << res.get_ptr() << " : " << handle->toString() << "\n";
#endif
}

void HandleMap::UpdateDeleteNode(LoopTreeNode* n)
{
  loopMap.erase(n);
}

HandleMap:: ~HandleMap()
{
/* 
  for (std::map<const LoopTreeNode*,LocalVar*>::const_iterator p = loopMap.begin();
       p != loopMap.end(); ++p) {
     const LoopTreeNode* cur = (*p).first;
     cur->DetachObserver(*this);
  }
*/
}

LocalVar* HandleMap:: GetLoopHandle(LoopTreeNode* loop)
{
   std::map<const LoopTreeNode*,LocalVar*>::const_iterator p = loopMap.find(loop);
   if (p == loopMap.end()) {
     LocalVar* res = NewLoopHandle();
     loopMap[loop] = res;
     loop->AttachObserver(*this);
     std::string name = res->toString(OUTPUT_NO_DEBUG);
     if (!loop->set_preAnnot("/*@; BEGIN("+name+"=Nest) @*/"))
     {
        std::cerr << "ERROR: cannot set trace for :" << loop->TreeToString() << "\n";
         assert(0);
     }
     return res;
   }
   return (*p).second;
}

/* QY: trace the input to properly order all trace handles created */
template <class MapType, class NodeType>
class SetTraceVarTraverse 
{
  LocalVar* last;
  LocalVar* curtop;
  const MapType& astMap;
  std::vector<LocalVar*>& top_handles;
 protected:
  const NodeType* topAst;
  SetTraceVarTraverse(const MapType& m, std::vector<LocalVar*>& handles)
       : last(0),curtop(0),astMap(m),top_handles(handles), topAst(0) {}
  void pre_visit( const NodeType* n) 
  {
      typename MapType::const_iterator p = astMap.find(n);
#ifdef DEBUG_TRACE
std::cerr << "look for trace var: " << n << "\n";
#endif
      if (p != astMap.end()) {
        LocalVar* v = (*p).second;
        assert(v != 0);
#ifdef DEBUG_TRACE
std::cerr << "found trace var: " << v->toString() << "\n";
#endif
        if (last != 0) {
            assert(last->get_entry().get_restr() == 0);
            last->get_entry().set_restr(v);
        }
        if (curtop==0) { 
            curtop = v; topAst=n;
            top_handles.push_back(curtop);
        }
        last = v;
        POETCode* body = last->get_entry().get_restr();
        if (body != 0) { /*QY: the loop body is also traced*/
            v = dynamic_cast<LocalVar*>(body); 
            assert(v != 0);
            last = v;
        }
     }
 }
 void post_visit(const NodeType* n) { 
   if (topAst == n) { topAst = 0; curtop = 0; }
 }
 public:
   const std::vector<LocalVar*>& get_top() { return top_handles; }
};

class SetTraceVarASTTraverse : public ProcessAstNode,  
   public SetTraceVarTraverse< HandleMap::AstMapType, void>
{
 public:
  SetTraceVarASTTraverse(const HandleMap::AstMapType& _m, std::vector<LocalVar*>& handles)
     : SetTraceVarTraverse<HandleMap::AstMapType,void>(_m,handles)
     {}
  void apply(AstInterface& fa)
  {
    ReadAstTraverse(fa,fa.GetRoot(), *this, AstInterface::PreAndPostOrder); 
  }
  virtual bool Traverse( AstInterface &fa, const AstNodePtr& n, 
                             AstInterface::TraversalVisitType t)
 {
   if (t == AstInterface::PreVisit) { pre_visit(n.get_ptr()); }
   else post_visit(n.get_ptr());
   return true;
 }
};

class SetTraceVarLoopTreeTraverse : public LoopTreeTraverse,  
   public SetTraceVarTraverse<std::map<const LoopTreeNode*, LocalVar*>,LoopTreeNode>
{
 public:
  SetTraceVarLoopTreeTraverse(const std::map<const LoopTreeNode*, LocalVar*>& _m, std::vector<LocalVar*>& handles, LoopTreeNode* r)
     : LoopTreeTraverse(r),SetTraceVarTraverse<std::map<const LoopTreeNode*, LocalVar*>,LoopTreeNode>(_m, handles) {}
  void apply() {
     const LoopTreeNode* nexttop = 0;
     for (LoopTreeNode* cur = 0; (cur=Current()) != 0; Advance()) {
         if (cur == nexttop)  post_visit(topAst);
         pre_visit(cur);
         if (nexttop == 0 && topAst != 0) {
            nexttop = topAst->NextSibling();
            if (nexttop == 0) nexttop = topAst; /*has a single top handle*/
         }
     }
  }
};
void HandleMap::GenTraceHandles(POETProgram& poet, AstInterface& fa)
{
    SetTraceVarASTTraverse op(astMap,topHandles); /* link all trace handles*/
    op.apply(fa);
}

void HandleMap::GenTraceHandles(POETProgram& poet, LoopTreeNode* r)
{
    SetTraceVarLoopTreeTraverse op(loopMap,topHandles,r); /* link all trace handles*/
    op.apply();
}

LocalVar* HandleMap:: GetTraceTop(const std::string& handleName)
{ return TRACE_VAR("top_"+handleName); }

LocalVar* HandleMap:: GetTraceDecl(LocalVar* top)
{ return TRACE_VAR("decl_"+top->toString(OUTPUT_NO_DEBUG)); }

LocalVar* HandleMap:: GetTracePrivate(LocalVar* top)
{ return TRACE_VAR("trace_private_"+top->toString(OUTPUT_NO_DEBUG)); }

LocalVar* HandleMap:: FindTracePrivate(LocalVar* top)
{ return HAS_TRACE("trace_private_"+top->toString(OUTPUT_NO_DEBUG)); }

LocalVar* HandleMap:: GetTraceInclude()
{ return TRACE_VAR("trace_include_files"); }

LocalVar* HandleMap:: FindTraceInclude()
{ return HAS_TRACE("trace_include_files"); }

LocalVar* HandleMap:: GetTraceTarget()
{ return TRACE_VAR("target"); }

LocalVar* HandleMap::DeclareTraceInclude(POETProgram& poet, int& lineNo)
{
  LocalVar* inclvar = FindTraceInclude();
  if (inclvar == 0) {
     inclvar = GetTraceInclude();
     LocalVar* target=GetTraceTarget();
     DECL_TRACE(poet, inclvar,lineNo++); 
     MAIN_EVAL(poet, SEQ(poet, ASSIGN(inclvar,EMPTY),
                         ASSIGN(target,LIST(inclvar,ERASE(target)))), lineNo);
     lineNo += 2;
  }
  return inclvar;
}

LocalVar* HandleMap:: GetTraceCleanup(LocalVar* top)
{ return TRACE_VAR("var_clnup_"+top->toString(OUTPUT_NO_DEBUG)); }

LocalVar* HandleMap:: FindTraceCleanup(LocalVar* top)
{ return HAS_TRACE("var_clnup_"+top->toString(OUTPUT_NO_DEBUG)); }

std::string HandleMap:: to_string() const
 {
      std::string res;
      int size = topHandles.size();
      if (size > 0)  {
        PrintNestedTraceVars print;
        for (int i = 0; i < size; ++i) {
          print.apply(topHandles[i]);
        }
        res = res + print.get_result();
      }
      return res;
 }


LocalVar* HandleMap::
GenTraceCommand(POETProgram& poet,const std::vector<LocalVar*>& handles,
                LocalVar* target,int &lineNo)
{
  int handlenum = handles.size();
 if (handlenum > 0) { /*QY: link top->decl->handleTop*/
     std::string handleName=handles[0]->toString(OUTPUT_NO_DEBUG);
     LocalVar* top = GetTraceTop(handleName), *decl = GetTraceDecl(top);
     top->get_entry().set_restr(decl); 
     decl->get_entry().set_restr(handles[0]);
     DECL_TRACE(poet, top,lineNo++); 
     POETCode* handle = 0;
     for (int i = handlenum-1; i >= 0; --i) 
            handle=LIST(handles[i], handle);
     POETCode* eval = SEQ(poet, ASSIGN(decl,EMPTY), ASSIGN(top,LIST(decl,handle)));
     int evalline = 3;
     MAIN_EVAL(poet, 
         SEQ(poet,eval,INVOKE(POET_static::eraseHandle, 
                              ASSIGN(POET_static::eraseHandle_repl, top), 
                              PAIR(handles[0],target))),
          lineNo);
     lineNo+=evalline+1;
     return top;
    }
 else {
    std::cerr << "Warning: no code is traced in AST!\n";
    assert(0);
    return(0);
 }
}

int configure_optimization()
{
    int applyOpt=OptSpec::OPT_NONE;
    LoopTransformOptions::OptType t = LoopTransformOptions::GetInstance()->GetOptimizationType();
    if (t & LoopTransformOptions::LOOP_NEST_OPT)
               applyOpt|=OptSpec::OPT_CACHE_CLEANUP_LEVEL; 
    if (t & LoopTransformOptions::MULTI_LEVEL_OPT) 
               applyOpt|=OptSpec::OPT_CACHE_PROC_CLEANUP_LEVEL;
    if (t & LoopTransformOptions::INNER_MOST_OPT) 
               applyOpt|=OptSpec::OPT_REG_LEVEL;
    if (t & LoopTransformOptions::DATA_OPT) 
               applyOpt|=OptSpec::OPT_CACHE_REG_CLEANUP_LEVEL;
    if (t & LoopTransformOptions::PAR_LOOP_OPT)
               applyOpt|=(OptSpec::OPT_PAR_LEVEL | OptSpec::OPT_POST_PAR_LEVEL);
   return applyOpt;
}
/***********************************************************************/
/******QY: autotuning interface impl **********************************/
/***********************************************************************/

void AutoTuningInterface::
set_astInterface(AstInterface& fa)
{ fa.AttachObserver(&handleMap); }

void AutoTuningInterface::
set_arrayInfo( ArrayAbstractionInterface& _arrayInfo)
{
  arrayInfo = &_arrayInfo;
}

POETCode* AutoTuningInterface:: CreateArrayRef(POETProgram& poet, POETCode* arr, POETCode* subscript, int dim)
{
  if (arrayInfo != 0 && dim > 1)
  {
    SymbolicVal access = arrayInfo->CreateArrayAccess(
                SymbolicPOETWrap(arr), SymbolicPOETWrap(subscript));
    SymbolicVal2POET val2poet(poet);
    arrayAccess= val2poet.apply(access);
    return arrayAccess;
  }
  else return ARRAY_REF(arr,subscript);
}
POETCode* AutoTuningInterface:: Access2Array(POETProgram& poet, POETCode* ref,int dim)
{
  if (arrayAccess!=0 && dim > 1)
  {
    return CAR(TUPLE_ACC(ref, TOR(POET_static::FunctionCall_arg,POET_static::Array_var)));
  }
  else return TUPLE_ACC(ref,POET_static::Array_var);
}

void AutoTuningInterface:: BuildPOETProgram()
{
    if (poet == 0) {
       target =  HandleMap::GetTraceTarget();
       poet = new POETProgram(inputName+".pt");
       poet->insert_includeFile(new POETProgram("opt.pi"));
       lineNo=1;

       if (arrayInfo != 0) {
          SymbolicVal access = arrayInfo->CreateArrayAccess(
                new SymbolicPOETWrap(PARAM_VAR("array")),
                new SymbolicPOETWrap(PARAM_VAR("subscript")));
          if (!access.IsNIL()) {
              SymbolicVal2POET val2poet(*poet);
              CODE_SET_ATTR(POET_static::Array, "match", val2poet.apply(access));
          }
          poet->insert_codeDecl(POET_static::Array); ++lineNo;
       }
       poet->insert_traceDecl(target,lineNo); ++lineNo;
       ReadInput* input = poet->insert_inputDecl(lineNo++);
       std::string filename = "rose_" + inputName;
       input->set_inputFiles(STRING(filename));
       input->set_var(target);
       input->set_syntaxFiles(STRING("Cfront.code"));
    }
}

void AutoTuningInterface:: Gen_POET_opt()
{
    const std::vector<LocalVar*>& tophandles=handleMap.GetTopTraceHandles();
    LocalVar* top = handleMap.GenTraceCommand(*poet,tophandles, target,lineNo);
    ++lineNo;

    /* insert xform decls*/
    int lineold = lineNo;
    POETCode* traceMod = 0;
    for (std::vector<OptSpec*>::const_iterator 
         p = optvec.begin(); p != optvec.end(); ++p) {
       OptSpec* curopt = *p;
       curopt->insert_xformDecl(*poet,top,traceMod,lineNo); 
    }
    if (lineNo > lineold) ++lineNo;

    /* insert xform parameter declarations */
    int applyOpt = configure_optimization();
    POETCode* cond = 0;
    for (int i = 1; i <= OptSpec::OPT_LEVEL_MAX; i <<= 1) {
      OptSpec::OptLevel curLevel = (OptSpec::OptLevel)i;
      if (!(curLevel & applyOpt)) continue;
      for (std::vector<OptSpec*>::const_iterator 
           p = optvec.begin(); p != optvec.end(); ++p) {
           OptSpec* curopt = *p;
           if (curopt->get_opt_level() & curLevel) 
           {
             POETCode* cur_cond = curopt->insert_paramDecl(*this, *poet,curLevel, lineNo); 
             if (cur_cond != 0)
                cond = (cond == 0)? cur_cond : poet->make_Bop(POET_OP_AND,cond,cur_cond);
           }
      }
    }
    ++lineNo;
    if (cond != 0) {
       poet->insert_condDecl(cond, lineNo++);
       ++lineNo;
    }

    /* gen xform invocations*/
    std::vector<LocalVar*> delay_opts;
    for (int i = 1; i <= OptSpec::OPT_LEVEL_MAX; i <<= 1) {
      OptSpec::OptLevel curLevel = (OptSpec::OptLevel)i;
      if (!(curLevel & applyOpt)) continue;
      for (std::vector<OptSpec*>::const_iterator 
           p = optvec.begin(); p != optvec.end(); ++p) {
            OptSpec* curopt = *p;
            if (curopt->get_opt_level() & curLevel) {
               int linenum=lineNo;
               POETCode* cureval=curopt->gen_xformEval(*poet,top,traceMod,curLevel, lineNo); 
               if (cureval != 0) {
                  LocalVar* optvar = EVAL_VAR(*poet, curopt->get_opt_prefix(curLevel)+"_"+curopt->get_targetName()); 
                  delay_opts.push_back(optvar);
                  MAIN_EVAL(*poet, ASSIGN(optvar,DELAY(cureval)), linenum);  
                  lineNo+=2;
               }
            }
      }
    }
    ++lineNo;
    
    /*QY: apply delayed optimiations*/
   std::vector<LocalVar*>::const_iterator p_opt = delay_opts.begin();
   POETCode* res = APPLY(*p_opt); 
   for ( ++p_opt; p_opt != delay_opts.end(); ++p_opt) {
      res = SEQ(*poet,res, APPLY(*p_opt)); 
   }
   MAIN_EVAL(*poet, res, ++lineNo);
}

void AutoTuningInterface::GenOutput()
{
  if (poet != 0) {
    WriteOutput* output = poet->insert_outputDecl(lineNo++);
    output->set_syntaxFiles(STRING("Cfront.code"));
    output->set_outputExp( target);

    std::fstream out; 
    out.open(poet->get_filename().c_str(), std::ios::out);
    poet->write_to_file(out);
    out.close();

    std::string xsname = inputName + ".xs";
    out.open(xsname.c_str(), std::ios::out);
    out << "Nests: " << handleMap.to_string() << "\n";
    for (int i = 1; i <= OptSpec::OPT_LEVEL_MAX; i <<= 1) {
      OptSpec::OptLevel curLevel = (OptSpec::OptLevel)i;
      for (std::vector<OptSpec*>::const_iterator 
           p = optvec.begin(); p != optvec.end(); ++p) {
         OptSpec* curopt = *p;
         out << curopt->to_string(curLevel);
      }
    }
    out.close();
  }
}

bool AutoTuningInterface::ApplyOpt(LoopTreeNode* r)
{
  if (optvec.size() == 0) return false;
  BuildPOETProgram();
  handleMap.GenTraceHandles(*poet,r);
  Gen_POET_opt();
  return true;
}

bool AutoTuningInterface::ApplyOpt(AstInterface& fa)
{
  if (optvec.size() == 0) return false;
  BuildPOETProgram();
  handleMap.GenTraceHandles(*poet,fa);
  Gen_POET_opt();
  return true;
}

void AutoTuningInterface::UnrollLoop(AstInterface& fa, const AstNodePtr& loop, int unrollsize)
{
   LocalVar* loopHandle = handleMap.GetLoopHandle(fa, loop);
   OptSpec* res = new UnrollSpec(loopHandle, unrollsize);
   optvec.push_back(res);
}  

void AutoTuningInterface::
ParallelizeLoop(LoopTreeNode* outerLoop, int bsize)
{
   LocalVar* outerHandle = handleMap.GetLoopHandle(outerLoop);

   OptSpec* res = new ParLoopSpec(outerHandle,outerLoop, bsize);
   optvec.push_back(res);
}

void AutoTuningInterface::
BlockLoops(LoopTreeNode* outerLoop, LoopTreeNode* innerLoop, 
      LoopBlocking* config, const std::vector<FuseLoopInfo>* nonperfect)
{
   LocalVar* outerHandle = handleMap.GetLoopHandle(outerLoop);
   OptSpec* res = new BlockSpec(handleMap,outerHandle,innerLoop,config,nonperfect);
   optvec.push_back(res);
}

AutoTuningInterface:: ~AutoTuningInterface() 
     {
       if (poet != 0) delete poet;
       for (std::vector<OptSpec*>::iterator p = optvec.begin();
           p != optvec.end(); ++p) {
               delete (*p);
       }
     }

BlockSpec* AutoTuningInterface:: LoopBlocked(LocalVar* loop, unsigned *index)
{
   for (std::vector<OptSpec*>::const_iterator p = optvec.begin();
         p != optvec.end(); ++p) {
       BlockSpec* cur = dynamic_cast<BlockSpec*>(*p);
       if (cur != 0) {
          //   if (cur->get_handle() == loop) return cur;   
          for (unsigned i = 0; i < cur->get_loopnum(); ++i) 
          {
             if (cur->get_loop(i).handle == loop) {
                 if (index != 0) *index = i;
                 return cur;   
             }
          }
       }
   }
   return 0;
}

void AutoTuningInterface::
CopyArray( CopyArrayConfig& config, LoopTreeNode* repl)
{
  LocalVar* outerHandle = 0, *handle = 0;

  LoopTreeNode* repl1 = repl, *repl2=repl;
  while (repl1->ChildCount()==1 && !repl1->IncreaseLoopLevel())  
        { repl1=repl1->FirstChild(); }
  outerHandle = handleMap.GetLoopHandle(repl1);

  if (repl != repl1) handle = outerHandle;
  else if (repl->ChildCount() > 1) {
     handle=handleMap.GetBodyHandle(repl1, outerHandle);
  }
  else  {
     do { repl2=repl2->FirstChild(); }
     while (repl2->ChildCount()==1 && !repl2->IncreaseLoopLevel()); 
     handle = handleMap.GetLoopHandle(repl2);
  }

  BlockSpec* block = LoopBlocked(outerHandle);
  if (block!=0 && !config.get_arr().scalar_repl()) {
         OptSpec* res= new BlockCopyArraySpec(outerHandle,config, *block);
         optvec.push_back(res);
  }
  else {
      OptSpec* res = new CopyArraySpec(handleMap,handle,handle->toString(OUTPUT_NO_DEBUG), config,repl);
      optvec.push_back(res);
   }
}

/***********************************************************************/
/******QY: finite differencing  impl **********************************/
/***********************************************************************/

POETCode* POET_static::
gen_fdInvoke(POETProgram& poet, LocalVar* top, POETCode* target,
         const std::string& name,POETCode* exp, POETCode* expType,
         const std::vector<POETCode*>& dimVec, 
         POETCode* permute, POETCode* traceMod, bool isScalar)
{
  if (dimVec.size() == 0) return 0;
  POETCode* config=SEQ5(poet,ASSIGN(POET_static::fd_exp_type, expType),
                        ASSIGN(POET_static::fd_prefix, STRING(name)),
                        ASSIGN(POET_static::fd_trace,top),
                        ASSIGN(POET_static::fd_is_scalar, ICONST(isScalar)),
                        ASSIGN(POET_static::fd_traceDecl,HandleMap::GetTraceDecl(top)));
  LocalVar* privateTrace = HandleMap::FindTracePrivate(top);
  if (privateTrace != 0)
          config=SEQ(poet, config,ASSIGN(POET_static::fd_traceVar,privateTrace));
  if (permute != 0) config = SEQ(poet,config, ASSIGN(POET_static::fd_permute, permute));
  if (traceMod != 0) config = SEQ(poet,config, ASSIGN(POET_static::fd_mod, traceMod));

  POETCode* dim = 0;
  for (int i = dimVec.size()-1; i >= 0;  --i) 
          dim = LIST(dimVec[i], dim);
  POETCode* args = TUPLE3(exp,dim, target);
  POETCode* result =  INVOKE(POET_static::finiteDiff, config, args);
  return result;
}

/***********************************************************************/
/******QY: loop unrolling  impl **********************************/
/***********************************************************************/

LocalVar* UnrollSpec:: get_unrollSizeVar(const std::string& handleName)
{
    std::string paramName = "usize_" + handleName;
    return PARAM_VAR(paramName);
}

std::string UnrollSpec:: to_string(OptLevel level)
{ 
  if (level & OPT_PROC_LEVEL)
     return "UnrollLoop: target=" + targetName + "\n";
  else return ""; 
}

UnrollSpec :: UnrollSpec (LocalVar* handle, int unrollSize) 
   : OptSpec(handle, handle->toString(OUTPUT_NO_DEBUG))
{
    paramVar = get_unrollSizeVar(targetName);
    paramVar->get_entry().set_code(ICONST(unrollSize));
    paramVar->get_entry().set_restr(RANGE_TYPE(ICONST(1),ANY));
}


POETCode* UnrollSpec::
insert_paramDecl(AutoTuningInterface& tune, POETProgram& poet, 
                 OptLevel optLevel, int& lineNo)
{
  InsertParamDecl(poet,paramVar, "Unroll factor for loop " + targetName, lineNo++);
  unsigned index = 0;
  BlockSpec* block = tune.LoopBlocked(static_cast<LocalVar*>(get_target()), &index); 
  if (block != 0 && block->get_loopnum() > index) 
  {
     POETCode* bsize = block->get_blockSize(block->get_targetName(), index);
     assert(bsize!=0);
     POETCode* res = BOP(POET_OP_AND, BOP(POET_OP_LE, paramVar, bsize),
                         BOP(POET_OP_EQ, BOP(POET_OP_MOD,bsize,paramVar), POET_static::Zero));
     return res;
  } 
  return 0;
}

POETCode* UnrollSpec::
gen_xformEval(POETProgram& poet, LocalVar* top, 
                   POETCode* traceMod, OptLevel optLevel, int& lineNo) 
{
  lineNo+=1;
  return INVOKE(POET_static::unroll, 
          SEQ(poet,ASSIGN(POET_static::unroll_factor,paramVar),ASSIGN(POET_static::unroll_cleanup,ICONST(0))),
          PAIR(TUPLE_ACC(target,POET_static::Nest_body), target));
}

/***********************************************************************/
/******QY: loop parallelization impl **********************************/
/***********************************************************************/

std::string print_list(POETCode* r)
{
  POETList* l = dynamic_cast<POETList*>(r);
  if (l == 0 || l->get_rest() == 0) return r->toString(OUTPUT_NO_DEBUG);
  return print_list(l->get_first()) + "," + print_list(l->get_rest()); 
}

std::string ParLoopSpec:: to_string(OptLevel level)
{ 
  if (level & OPT_PAR_LEVEL) {
     std::string res =  "ParallelizeLoop: target=" + targetName;
     if (privateVars != 0) 
        res = res + " private=" + print_list(privateVars);
     res = res + "\n";
     return res;
  }
  return "";
}

ParLoopSpec :: ParLoopSpec(LocalVar* handle, LoopTreeNode* outerLoop,int bsize) 
   : OptSpec(handle, handle->toString(OUTPUT_NO_DEBUG))
{
    parVar = PARAM_VAR(PAR_PARAM_NAME(targetName));
    parVar->get_entry().set_code(ICONST(1));
    parVar->get_entry().set_restr(RANGE_TYPE(ICONST(1),ANY));

    parblockVar = PARAM_VAR(PAR_BLOCK_NAME(targetName));
    parblockVar->get_entry().set_code(ICONST(bsize));
    parblockVar->get_entry().set_restr(RANGE_TYPE(ICONST(1),ANY));

   privateVars=0; ivarName = 0; bvarName = 0;
   for (LoopTreeTraverseSelectLoop loops(outerLoop);
        !loops.ReachEnd(); loops.Advance()) {
      LoopTreeNode* cur = loops.Current();
      std::string curvar = cur->GetLoopInfo()->GetVar().toString();
      if (ivarName == 0) { 
          ivarName=STRING(curvar); bvarName=STRING(curvar+"_par"); 
      }
      privateVars = LIST( STRING(curvar),privateVars);
   }
}

POETCode* ParLoopSpec::
insert_paramDecl(AutoTuningInterface& tune, POETProgram& poet, 
                 OptLevel optLevel, int& lineNo)
{
  InsertParamDecl(poet,parVar, "number of threads to parallelize loop " + targetName, lineNo++);
  InsertParamDecl(poet,parblockVar, "number of loop iterations to run by different threads for " + targetName, lineNo++);
  
  BlockSpec* block = tune.LoopBlocked(static_cast<LocalVar*>(get_target())); 
  if (block != 0) 
  {
     POETCode* bsize = block->get_blockSize(targetName, 0);
     assert(bsize!=0);
     POETCode* res = BOP(POET_OP_OR,BOP(POET_OP_EQ,parblockVar,ICONST(1)),
                      BOP(POET_OP_AND, BOP(POET_OP_GE, parblockVar, bsize),
                         BOP(POET_OP_EQ, BOP(POET_OP_MOD,parblockVar,bsize), POET_static::Zero)));
     return res;
  } 
  return 0;
}

void ParLoopSpec::
insert_xformDecl(POETProgram& poet, LocalVar* top, POETCode*& traceMod, int& lineNo)
{
   HandleMap::DeclareTraceInclude(poet,lineNo);
   LocalVar* pvar = HandleMap::GetTracePrivate(top);
   DECL_TRACE(poet, pvar,lineNo++); 
   MAIN_EVAL(poet,ASSIGN(pvar,EMPTY), lineNo++);
}

POETCode* ParLoopSpec::
gen_xformEval(POETProgram& poet, LocalVar* top, 
                   POETCode* traceMod, OptLevel optLevel, int& lineNo)
{
  LocalVar* privateTrace = HandleMap::GetTracePrivate(top);

  POETCode* config=SEQ4(poet,ASSIGN(POET_static::par_trace,top), 
                    ASSIGN(POET_static::par_private,privateTrace), 
                    ASSIGN(POET_static::par_include, HandleMap::GetTraceInclude()), 
                         ASSIGN(POET_static::par_thread,parVar));
  POETCode* blockDim = CODE_ACC("BlockDim", TUPLE3(ivarName, bvarName, parblockVar));
  POETCode* blockconfig= SEQ(poet, ASSIGN(POET_static::block_factor, blockDim),
                         ASSIGN(POET_static::block_cleanup, POET_static::Zero));
  if (traceMod!=0) blockconfig=SEQ(poet,blockconfig,
                         ASSIGN(POET_static::block_tracemod,traceMod));
  POETCode* blockInvoke = INVOKE(POET_static::block_xvar, blockconfig,
             PAIR(TUPLE_ACC(target,POET_static::Nest_body) , target));
  if (traceMod != 0) blockInvoke=TRACE_EVAL(traceMod, blockInvoke);
  POETCode* invoke= 
     SEQ6(poet,blockInvoke,
        INVOKE(POET_static::parloop, config, target),
        BOP(POET_OP_ERASE, target, top),
        ASSIGN(target,TUPLE_ACC(target,POET_static::Nest_body)),
        REPLACE(ERASE(target),target,top),
        INVOKE(POET_static::moveHandle,
             ASSIGN(POET_static::moveHandle_trace,HandleMap::GetTraceTarget()), 
             PAIR(top, target)));
  lineNo += 10;

  return SEQ3(poet,ASSIGN(privateTrace,LIST(bvarName,privateVars)),
     INVOKE(POET_static::appendDecl,ASSIGN(POET_static::appendDecl_trace,HandleMap::GetTraceDecl(top)),
                PAIR(POET_static::IntType,bvarName)),
           IFELSE(BOP(POET_OP_NE,parVar,ICONST(1)), invoke, EMPTY)); 
}

/***********************************************************************/
/******QY: loop blocking  impl **********************************/
/***********************************************************************/

std::string BlockSpec:: to_string(OptLevel level)
{ 
 std::string res;
 bool doit=false;
 if (level & OPT_CACHE_LEVEL) { doit = true; res = "BlockLoops: "; }
 else if (level & OPT_REG_LEVEL) { doit = true; res = "UnrollJam: "; }
 if (doit) 
    res = res + "target=" + targetName + " inner_loop=" + loopVec[loopVec.size()-1].handle->toString(OUTPUT_NO_DEBUG) + "\n";
  return res;
}

LocalVar* BlockSpec::get_blockDimVar(const std::string& handleName)
{
  /* QY: set up paramVar */
    std::string paramName = "bdim_" + handleName;
    return PARAM_VAR(paramName);
}

LocalVar* BlockSpec::get_blockSizeVar(const std::string& handleName)
{
  /* QY: set up paramVar */
    std::string paramName = "bsize_" + handleName;
    return PARAM_VAR(paramName);
}

POETCode* BlockSpec:: get_blockSize(const std::string& handleName, int level)
{
      POETCode* curblocksize = get_blockSizeVar(handleName);
      for (int i = level-1; i >= 0; --i) 
           curblocksize = CDR(curblocksize);
      return CAR(curblocksize);
}

LocalVar* BlockSpec::get_unrollJamSizeVar(const std::string& handleName)
{
    std::string paramName = "ujsize_" + handleName;
    return PARAM_VAR(paramName);
}

POETCode* BlockSpec:: get_ujSize(const std::string& handleName, int level)
{
      POETCode* cursize = get_unrollJamSizeVar(handleName);
      for (int i = level-2; i >= 0; --i) 
           cursize = CDR(cursize);
      return CAR(cursize);
}


POETCode* FuseLoopInfo::toPOET(HandleMap& handleMap, const FuseLoopInfo& info)
{
   POETCode* nonperfect=0;
   int size = info.loops.size();
   if (size == 1) 
        nonperfect=handleMap.GetLoopHandle(info.loops[0].first);
/*QY: if need to consider alignment, should build a separate list  
                    PAIR(handleMap.GetLoopHandle(info.loops[i].first),
                         ICONST(info.loops[i].second));
*/
   else {
        for (int i = size-1; i >= 0; --i) { 
           nonperfect=LIST(handleMap.GetLoopHandle(info.loops[i].first),nonperfect);
         }
   }
   POETCode* pivot = 0;
   if (info.pivotLoop==0) pivot=EMPTY;
   else pivot=handleMap.GetLoopHandle(info.pivotLoop);
   return CODE_ACC("NonPerfectLoops", PAIR(pivot,nonperfect));
}

BlockSpec::BlockSpec(HandleMap& _handleMap, 
            LocalVar* outerHandle, LoopTreeNode* _innerLoop, 
            LoopBlocking* config, 
            const std::vector<FuseLoopInfo>* _nonperfect) 
   : OptSpec(outerHandle,outerHandle->toString(OUTPUT_NO_DEBUG)), handleMap(_handleMap), loopnum(config->NumOfLoops())
{
   assert(loopnum > 1); 
   /*QY: set up loopVec; */
   for (unsigned i = 0; i < loopnum; ++i) loopVec.push_back(LoopInfo());
   LoopTreeNode* p = _innerLoop;
   for (int i = loopnum-1; i > 0;  --i) {
       assert(p != 0);
       loopVec[i] = LoopInfo(handleMap.GetLoopHandle(p), p->GetLoopInfo()->GetVar().GetVarName());
       p = p->EnclosingLoop();
   }
   if (p == 0) {
std::cerr << "The inner loop: " << _innerLoop->TreeToString() << " does not have " << loopnum-1 << " enclosing loops\n";
      assert(0);
   }
   loopVec[0] = LoopInfo(outerHandle, p->GetLoopInfo()->GetVar().GetVarName());

   unsigned nsize=0;
   if (_nonperfect == 0 || (nsize=_nonperfect->size()) == 0) { nonperfect=0; }
   else {
       nonperfect=FuseLoopInfo::toPOET(handleMap,(*_nonperfect)[nsize-1]);
       for (int i = nsize-2; i >= 0; --i)
       {
         const FuseLoopInfo& cur=(*_nonperfect)[i];
         nonperfect=LIST(FuseLoopInfo::toPOET(handleMap,cur),nonperfect);
       }
       for (unsigned i = 0; i < nsize; ++i) {
         const FuseLoopInfo& cur=(*_nonperfect)[i];
         for (unsigned j = 0; j < cur.loops.size(); ++j) { 
            LoopTreeNode* curloop = cur.loops[j].first;
            LocalVar* curhandle = handleMap.GetLoopHandle(curloop);
            int k = 0;
            for ( ; k < loopnum; ++k) 
                   { if (loopVec[k].handle==curhandle) break; }
            if (k == loopnum) { // curloop is not already in loopVec
               loopVec.push_back(LoopInfo(curhandle,curloop->GetLoopInfo()->GetVar().GetVarName())); 
            }
         }
       }
   }

  /* QY: set up paramVar*/
   POETCode* bsize = 0, *btype=0;
   for (int i = loopnum-1; i >= 0;  --i) {
       SymbolicVal cur_val = config->BlockSize(i);
       int cur = 0;
       if (!cur_val.isConstInt(cur)) assert(0);
       bsize=LIST(ICONST(cur),bsize); 
       btype = LIST(INT_TYPE, btype);
   }
   blockPar = get_blockSizeVar(targetName);
   blockPar->get_entry().set_code(bsize);
   blockPar->get_entry().set_restr(btype);

   ujPar = get_unrollJamSizeVar(targetName);
   bsize = 0; btype=0;
   for (unsigned i = 1; i < loopnum; ++i) {
          bsize=LIST(ICONST(2), bsize); 
          btype=LIST(INT_TYPE, btype);
   }
   ujPar->get_entry().set_code(bsize);
   ujPar->get_entry().set_restr(btype);
}

LocalVar* BlockSpec:: get_blockTileVar(const std::string& handleName)
{
    std::string tileName = "var_tile_" + handleName;
    return TRACE_VAR(tileName);
}

LocalVar* BlockSpec:: get_blockSplitVar(const std::string& handleName)
{
    std::string tileName = "var_split_" + handleName;
    return TRACE_VAR(tileName);
}

POETCode* BlockSpec::compute_blockDim(LocalVar* paramVar)
{
    std::vector<POETCode*> blockVec;
    POETCode* p_bsize = paramVar;
    for (unsigned i = 0; i < loopnum; ++i) {
       blockVec.push_back( CAR(p_bsize));
       p_bsize = CDR(p_bsize);
    } 
    POETCode* blockDim=0;
    for (int i = loopnum-1; i>=0; --i) {
       std::string ivarname = loopVec[i].ivarname;
       blockDim = LIST(CODE_ACC("BlockDim", 
                   TUPLE3(STRING(ivarname),STRING(ivarname+"_bk"),blockVec[i])),
                     blockDim);
    }
    return blockDim;
}


POETCode* BlockSpec::
insert_paramDecl(AutoTuningInterface& tune, POETProgram& poet, 
                 OptLevel optLevel, int& lineNo)
{
  switch (optLevel) {
   case OPT_CACHE_LEVEL: 
      InsertParamDecl(poet, blockPar, "Blocking factor for loop nest " + targetName, lineNo++);
      return 0;
   case OPT_REG_LEVEL: 
      {
      /*QY: unrolljam command-line parameter; default value set to 2 */
      InsertParamDecl(poet, ujPar, "Unroll and Jam factor for loop nest " + targetName, lineNo++);
      POETCode* res = 0; 
      for (int i = 0; i < loopnum-1; ++i)
      {
          POETCode* bsize = get_blockSize(targetName, i);
          POETCode* usize = get_ujSize(targetName, i);
          POETCode* cond = BOP(POET_OP_OR, BOP(POET_OP_EQ,bsize,ICONST(1)),
                            BOP(POET_OP_AND,BOP(POET_OP_LE,usize,bsize),
                             BOP(POET_OP_EQ,BOP(POET_OP_MOD,bsize,usize),POET_static::Zero)));
         res = (res == 0)? cond : BOP(POET_OP_AND,res,cond);
      }
      return res;
      }
   case OPT_CLEANUP_LEVEL: return 0;
   default: 
       std::cerr << "unexpected opt level: " << optLevel << "\n";
       assert(0);
  }
}

void BlockSpec::
insert_xformDecl(POETProgram& poet, LocalVar* top, POETCode*& traceMod, int& lineNo)
{
    DECL_TRACE(poet, get_blockTileVar(targetName),lineNo++); 
    LocalVar* cleanupvar = HandleMap::GetTraceCleanup(top);
    MAIN_EVAL(poet, ASSIGN(cleanupvar, top), lineNo++);
}

POETCode* BlockSpec::gen_xformEval(POETProgram& poet, LocalVar* top, 
                   POETCode* traceMod, OptLevel optLevel, int& lineNo)
{

   /*QY: set up blocking dimension configuration*/
  POETCode* decl = HandleMap::GetTraceDecl(top);
  LocalVar* innerHandle = loopVec[loopnum-1].handle;
  LocalVar* tileVar = get_blockTileVar(targetName);

  if (optLevel == OPT_CACHE_LEVEL) {
      POETCode* before= 
         SEQ(poet,ASSIGN(get_blockDimVar(targetName),compute_blockDim(blockPar)),
                 ASSIGN(get_blockTileVar(targetName), COPY(target)));
      lineNo +=2;
      POETCode* config= 
        SEQ4(poet,ASSIGN(POET_static::block_factor,get_blockDimVar(targetName)),
                  ASSIGN(POET_static::block_cleanup,POET_static::Zero), 
                  ASSIGN(POET_static::block_tile, tileVar),
                  ASSIGN(POET_static::block_decl,decl));
      if (nonperfect != 0) 
           config = SEQ(poet,config, 
                      ASSIGN(POET_static::block_nonperfect, nonperfect));
      
      if (traceMod!=0) 
          config=SEQ(poet,config,
                     ASSIGN(POET_static::block_tracemod,traceMod));
      POETCode* blockInvoke = INVOKE(POET_static::block_xvar, config, PAIR(TUPLE_ACC(innerHandle,POET_static::Nest_body), target));
      if (traceMod!=0) 
         blockInvoke=TRACE_EVAL(traceMod,blockInvoke);

      POETCode* bvars = 0;
      for (unsigned i = 0; i < loopnum; ++i) {
           bvars = LIST(STRING(loopVec[i].ivarname+"_bk"),bvars);
      }
      lineNo += 3; 
      POETCode* res = SEQ3(poet,before,
                INVOKE(POET_static::appendDecl,ASSIGN(POET_static::appendDecl_trace, decl),
                          PAIR(POET_static::IntType,bvars)),
                blockInvoke);
      LocalVar* privateTrace = HandleMap::FindTracePrivate(top);
      if (privateTrace != 0)
          res = SEQ(poet, res,ASSIGN(privateTrace, LIST(bvars,ERASE(privateTrace))));
      return res;
  }
  else if (optLevel == OPT_REG_LEVEL) {

      /*QY: setup loops to trace the tiled nest */
      POETCode* allloops = 0;/*QY: include non-perfectly nested loop handles*/
      for (unsigned i = loopVec.size(); i > 0; --i) {
          if (i < loopnum) {
             LocalVar* body = handleMap.HasBodyHandle(loopVec[i-1].handle);
             if (body != 0) allloops = LIST(body,allloops);
          }
          allloops = LIST(loopVec[i-1].handle,allloops);
      }
      POETCode* loops = 0;  /*QY: only blocked loop handles*/
      for (int i = loopnum-1; i >=0; --i) {
          loops = LIST(loopVec[i].handle,loops);
      }

      POETCode* before = 
          SEQ(poet,BOP(POET_OP_ERASE, allloops, top),
                INVOKE(POET_static::traceNest, ASSIGN(POET_static::traceNest_trace,top), 
                       PAIR(loops,ERASE(tileVar))));
      lineNo += 2;
      POETCode* nonperfect = 0;
      if (loopVec.size() > loopnum) {
          /*QY: assign all nonperfectly nested loops with the innermost tile*/
          LocalVar* outer_handle=loopVec[loopnum-1].handle;
          nonperfect= ERASE(outer_handle);
          for (unsigned i = loopVec.size(); i >loopnum; --i)
          {
              LocalVar* cur_handle = loopVec[i-1].handle;
              before = SEQ3(poet,before, ASSIGN(cur_handle,nonperfect),
                              REPLACE(nonperfect,cur_handle,outer_handle));
              lineNo+=2;
           }
      }
      for (unsigned i = 0; i < loopnum; ++i) {
         LocalVar* cur = loopVec[i].handle;
         LocalVar* body = handleMap.HasBodyHandle(cur);
         POETCode* body_val = TUPLE_ACC(cur,POET_static::Nest_body);
         if (body != 0) {
            before=SEQ3(poet,before,ASSIGN(body,body_val),
                        INVOKE(POET_static::eraseHandle,
                               ASSIGN(POET_static::eraseHandle_repl,body),
                               PAIR(ERASE(body),cur)));
            lineNo+=2;
         }
      }

      /*QY: use REPLACE to re-insert loop handles back to tiled AST*/
      before=SEQ(poet,before,REPLACE(ERASE(tileVar),loopVec[0].handle,top));

      /* QY: set up unroll-jam config */
      POETCode* bsize=ujPar;
      std::vector<POETCode*> blockVec;
      for (unsigned i = 1; i < loopnum; ++i) {
          blockVec.push_back( CAR(bsize));
          bsize = CDR(bsize);
      } 
      bsize = 0;
      for (int i = loopnum-2; i >=0; --i) 
         bsize = LIST(blockVec[i], bsize);

      POETCode* config= SEQ3(poet,ASSIGN(POET_static::unrollJam_factor, bsize),
                             ASSIGN(POET_static::unrollJam_cleanup, ICONST(0)),
                             ASSIGN(POET_static::unrollJam_trace, top));
      POETCode* param = PAIR(loopVec[loopnum-1].handle,loopVec[0].handle);
      lineNo ++;
      return SEQ(poet,before, INVOKE(POET_static::unrollJam, config, param));
  }
  else if (optLevel == OPT_CLEANUP_LEVEL) {
      lineNo ++;
      return INVOKE(POET_static::cleanup, ASSIGN(POET_static::cleanup_trace,top), HandleMap::GetTraceCleanup(top));
  }
}

/***********************************************************************/
/******QY: array copying  impl **********************************/
/***********************************************************************/

std::string CopyArraySpec:: to_string(OptLevel level)
{ 
  if (level & OPT_REG_LEVEL) {
    std::string res="ScalarRepl: "; /* use target Name only in array copying:target="+targetName;*/
    res = res + " aref=" + sel.arr_name();
    compute_copySubscript();
    for (int i = 0; i < subscriptVec.size(); ++i) 
       res = res + "[" + subscriptVec[i].toString() + "]";
    res = res + " data_type=\"" + AstInterface::GetBaseTypeName(sel.elem_type())+"\"";

    res = res + " dim="; 
    bool begin = true; 
    std::string scalarTarget;
    for (SelectArray::iterator p = sel.begin(); p != sel.end(); ++p){
       const SelectArray::ArrayDim& cur = *p;
       int level=get_loopLevel(cur);
       if (level >= 0) {
          if (!begin) res = res + ","; else begin = false;
          res = res + loopVec[level].handle->toString();
          scalarTarget = loopVec[level].handle->toString() + "[Nest.body]";
       }
    }     
    res = res + " target=" + scalarTarget;
    if (opt & INIT_COPY) res = res + " init=1"; else res = res + " init=0"; 
    if (opt & SAVE_COPY) res = res + " save=1"; else res = res + " save=0";
    res = res + "\n";
    return res;
  }
  else return "";
}

int CopyArraySpec::get_loopLevel(const SelectArray::ArrayDim& cur)
{
    int minLoop=cur.get_minLoopLevel(), maxLoop=cur.get_maxLoopLevel();
    if (minLoop < maxLoop) 
      { std::cerr << "Multiple acc loops\n"; assert(0); }
    if (minLoop < 0) return minLoop; /*QY: loop is outside of the copy root*/ 

    int loopnum = loopVec.size();
    if ( minLoop == loopnum && (opt & COPY_INSIDE))
         minLoop--;
    if (minLoop >= loopnum){
       std::cerr << "loopnum:" << loopnum << "\n";
       std::cerr << "minloop: " << minLoop <<"\n";
       assert(0);
    }
    return minLoop;
} 

CopyArraySpec::  
CopyArraySpec(HandleMap& handleMap, POETCode* target, const std::string& targetName, CopyArrayConfig& config, LoopTreeNode* root)
     : OptSpec(target,targetName), sel(config.get_arr()), opt(config.get_opt()),permute(0)
{ 
  int copylevel = 0;
  SelectArray& sel = config.get_arr();
  for (SelectArray::iterator p = sel.begin(); p != sel.end(); ++p){
       const SelectArray::ArrayDim& cur = *p;
       int minLoop=cur.get_minLoopLevel();
       if (copylevel < minLoop) copylevel = minLoop;
      }
  for (int i = 0; i < copylevel; ++i) {
    do {
      if (root->ChildCount() > 1) 
         { 
              std::cerr << "expecting loops perfectly nested: " << root->TreeToString() << "\n"; assert(0); 
         }
      if (root->ChildCount()==0) break;
      root = root->FirstChild();
    } while (!root->IncreaseLoopLevel());
     if (root->IncreaseLoopLevel())
        loopVec.push_back(LoopInfo(handleMap.GetLoopHandle(root), root->GetLoopInfo()->GetVar().GetVarName()));
     else 
        std::cerr << "expecting more loops inside: " << root->TreeToString() << "\n"; assert(0); 
  } 
  compute_config();
  opt = (CopyArrayOpt)(opt & (~ COPY_INSIDE));
}

/* QY: return variable name  used to trace array dimension to be copied */
LocalVar* CopyArraySpec:: 
get_dimVar (POETProgram& poet, const std::string& arrname) 
   { return EVAL_VAR(poet, targetName + "_" + cur_id + "_" + arrname+"_dim"); }

/* QY: trace handle for array name */
LocalVar* CopyArraySpec:: get_arrVar
(POETProgram& poet, const std::string& arrname)
{  return EVAL_VAR(poet, targetName + "_" + cur_id + "_" + arrname); }

void CopyArraySpec:: 
insert_xformDecl(POETProgram& poet, LocalVar* top, POETCode*& traceMod, int& lineNo)
{
      SymbolicVal2POET val2poet(poet);

      POETCode* cpDim = 0;
      for (SelectArray::iterator p = sel.begin(); p != sel.end(); ++p){
         const SelectArray::ArrayDim& cur = *p;
         cpDim=TUPLE_APPEND(cpDim, val2poet.apply(cur.get_incr())); 
      }
      if (cpDim==0) cpDim=ICONST(1);

      POETCode* subscript = compute_copySubscript(poet,false);

      std::string arrname = sel.arr_name();
      LocalVar* dimVar=get_dimVar(poet,arrname);
      LocalVar* arrVar = get_arrVar(poet,arrname);
      POETCode* arrref = AutoTuningInterface::CreateArrayRef(poet,
                             STRING(arrname),subscript,sel.arr_dim());
      traceMod=LIST(arrVar, traceMod);
      MAIN_EVAL(poet,SEQ(poet,ASSIGN(dimVar,cpDim), ASSIGN(arrVar, arrref)),lineNo);
      lineNo+=2;
}

void CopyArraySpec:: compute_copySubscript(LocalVar* dimVar) 
{
    if (dimVar == 0 && subscriptVec.size() > 0) return;

     int arrDim = sel.arr_dim();
     for (int i = 0; i < arrDim; ++i) 
          subscriptVec.push_back(sel.sel_start(i));
      int i = 0;
      for (SelectArray::iterator p = sel.begin(); p != sel.end(); ++i,++p) {
         const SelectArray::ArrayDim& cur = *p;
         int minLoop=get_loopLevel(cur);
         if (minLoop >= 0) {
            std::string ivarname = loopVec[minLoop].ivarname; 
            SymbolicVar curivar(ivarname,AST_NULL);

            /*QY: set up subscript*/
            int seldim = cur.get_arrDim();

            if (dimVar == 0) subscriptVec[seldim]=subscriptVec[seldim]+curivar*cur.get_incr();
            else subscriptVec[seldim]=subscriptVec[seldim]+curivar*SymbolicPOETWrap(TUPLE_ACC(dimVar,ICONST(i)));
         }
      }
}

POETCode* CopyArraySpec::
compute_copySubscript(POETProgram& poet, bool afterCopy) 
{
   std::string arrname = sel.arr_name();
   SymbolicVal2POET val2poet(poet);

   if (!afterCopy) compute_copySubscript();
   else {
      LocalVar* dimVar=get_dimVar(poet,arrname);
      compute_copySubscript(dimVar);
   }
   int arrDim = subscriptVec.size();
   assert (arrDim > 0); 
   POETCode* subscript = val2poet.apply(subscriptVec[arrDim-1]);
   for (int i = arrDim-2; i >= 0; --i) 
       subscript = LIST(val2poet.apply(subscriptVec[i]), subscript);
   return subscript;
}

POETCode* CopyArraySpec::
compute_copyDim(POETProgram& poet, bool scalar)
{
      std::string arrname = sel.arr_name();
      SymbolicVal2POET val2poet(poet);
      int selsize = sel.sel_dim();
      LocalVar* dimVar = get_dimVar(poet,arrname);

      std::vector<POETCode*>  cpDimVec(selsize); 
      for (int i = 0; i < selsize; ++i) cpDimVec.push_back(0);
      int i = 0;
      for (SelectArray::iterator p = sel.begin(); p != sel.end(); ++i,++p){
         const SelectArray::ArrayDim& cur = *p;
         int minLoop=get_loopLevel(cur);
         if (minLoop < 0) {
             std::string ivarname = get_cpIvarName(arrname,i);
             cpDimVec[i] = (CODE_ACC("CopyDim", 
                           TUPLE4(STRING(ivarname),ICONST(1), ICONST(1), 
                                  TUPLE_ACC(dimVar,ICONST(i)))));
         }
         else if (!scalar) {
           int permuteIndex = placeVec[minLoop];
           LocalVar* loop = loopVec[minLoop].handle;
           std::string ivarname = loopVec[minLoop].ivarname;//get_cpIvarName(arrname,permuteIndex);
           cpDimVec[permuteIndex]=
               CODE_ACC("CopyDim", TUPLE4(STRING(ivarname), ICONST(0), 
                                            //val2poet.apply(cur.get_size()), 
                       TUPLE_ACC(TUPLE_ACC(loop,POET_static::Nest_ctrl),POET_static::Loop_ub), 
                                            val2poet.apply(cur.get_incr())));
         }
         else {
             std::string ivarname = loopVec[minLoop].ivarname;
             LocalVar* loop = loopVec[minLoop].handle;
             cpDimVec[i] = (CODE_ACC("CopyDim", 
                           TUPLE4(STRING(ivarname),
                                  TUPLE_ACC(TUPLE_ACC(loop,POET_static::Nest_ctrl),POET_static::Loop_ivar), 
                                  TUPLE_ACC(TUPLE_ACC(loop,POET_static::Nest_ctrl),POET_static::Loop_step),
                                  TUPLE_ACC(dimVar,ICONST(i)))));
         }
      }
      POETCode* cpDim = 0;
      for (int i = cpDimVec.size() - 1; i >= 0; --i) {
        if (cpDimVec[i] != 0) 
          cpDim = LIST(cpDimVec[i], cpDim);
      }
      return cpDim; 
}

/*QY: compute internal configuration, including cur_id, permute and placeVec*/
void CopyArraySpec:: compute_config()
{
  /*QY: create a unique identifier for the current opt*/
  ++index;
  std::stringstream tmp;
  tmp << index;
  cur_id = tmp.str();

  int innerLevel = loopVec.size();
  if (innerLevel == 0) 
     { permute=0; return; }

  std::vector<int> permuteVec;
  for (int i = 0; i < innerLevel; ++i) {
       permuteVec.push_back(-1);
       placeVec.push_back(-1);
   }

  int sel_index=0;
  for (SelectArray::iterator p = sel.begin(); p != sel.end(); ++p,++sel_index){
         const SelectArray::ArrayDim& cur = *p;
         int minLoop=get_loopLevel(cur);
         if (minLoop < 0) continue;
         if (permuteVec[minLoop] >= 0){
           continue;
         }
         permuteVec[minLoop] = sel_index;
         placeVec[minLoop] = minLoop;
   }

   /*QY: condense the sorted permuteVec array*/
   int selsize = sel.sel_dim();
   if (innerLevel > selsize) {
      for (int i = 0, j = 0; j < innerLevel; ++j) {
         if (permuteVec[j] >= 0) {
              if (i < j) { 
                  placeVec[j] = i;
                  permuteVec[i++] = permuteVec[j]; permuteVec[j] = -1;
              }
              else ++i;
         }
      }
      permuteVec.resize(selsize);
      placeVec.resize(selsize);
   }

   bool needPermute=false;
   permute=0;
   for (int i = permuteVec.size() - 1; i >= 0; --i) {
          permute = LIST(ICONST(permuteVec[i]+1),permute);
          if (permuteVec[i] != i) needPermute=true; 
   }
   if (!needPermute) permute=0;
}

POETCode* CopyArraySpec:: gen_cpIvarDecl
  (POETProgram& poet, LocalVar* top, 
  const std::string& arrname, int dim, bool cpblock)
{
   POETCode* loopIvar = 0;
   if (!sel.scalar_repl()) {
     for (int i = dim - 1; i >= 0; --i) {
        loopIvar = LIST(STRING(get_cpIvarName(arrname,i)), loopIvar);
     }
   }
   POETCode* res = INVOKE(POET_static::appendDecl,
            ASSIGN(POET_static::appendDecl_trace,HandleMap::GetTraceDecl(top)),
                  PAIR(POET_static::IntType,loopIvar));
   LocalVar* privateTrace = HandleMap::FindTracePrivate(top);
   if (privateTrace != 0) 
       res = SEQ(poet, res, ASSIGN(privateTrace, LIST(loopIvar,ERASE(privateTrace))));
   return res;
}

POETCode* CopyArraySpec:: 
gen_copyInvoke(POETProgram& poet, POETCode* cploc, LocalVar* top,
        const std::string& arrname, 
        POETCode* arrelemType, CopyArrayOpt opt, 
        POETCode* cpDim, POETCode* cpblock, bool scalar, POETCode* traceMod)
{
      /*QY: set up element type and init/save/alloc configurations */
      POETCode* cpconfig= SEQ3(poet, 
               ASSIGN(POET_static::copyarray_bufname,STRING(get_bufName(arrname,scalar))),
               ASSIGN(POET_static::copyarray_elemtype,arrelemType), ASSIGN(POET_static::copyarray_isscalar,ICONST(scalar)));
      if (opt & INIT_COPY) 
           cpconfig = SEQ(poet,cpconfig,ASSIGN(POET_static::copyarray_initloc,cploc));
      if (opt & SAVE_COPY) 
           cpconfig = SEQ(poet,cpconfig,ASSIGN(POET_static::copyarray_saveloc,cploc));
      if (!scalar && (opt & ALLOC_COPY)) 
           cpconfig = SEQ(poet,cpconfig,ASSIGN(POET_static::copyarray_deleteloc,cploc));
      cpconfig=SEQ4(poet,cpconfig,
                    ASSIGN(POET_static::copyarray_trace,top), 
                    ASSIGN(POET_static::copyarray_tracedecl,HandleMap::GetTraceDecl(top)),
                    ASSIGN(POET_static::copyarray_tracemod,traceMod));
      LocalVar* pvar = HandleMap::FindTracePrivate(top);
      if (pvar != 0)
          cpconfig=SEQ(poet,cpconfig,ASSIGN(POET_static::copyarray_tracevars,pvar));

      if (permute != 0)
          cpconfig=SEQ(poet,cpconfig,ASSIGN(POET_static::copyarray_permute,permute));
      if (cpblock != 0) {
          POETCode* setblock=ASSIGN(POET_static::copyarray_cpBlock,cpblock);
          cpconfig = SEQ(poet,cpconfig, setblock);
      }

      /*QY: set up copy parameters and making the call */
      LocalVar* arrVar = get_arrVar(poet,arrname);
      if (cpDim==0) cpDim=EMPTY;
      POETCode* args = TUPLE3(arrVar,cpDim,cploc); 
      POETCode* copyinvoke = TRACE_EVAL(traceMod,INVOKE(POET_static::copyarray, cpconfig,args));
      return copyinvoke;
}

LocalVar* BlockCopyArraySpec::scalarRepl_handle(POETProgram& poet)
{
     int maxLoop = 0;
     for (SelectArray::iterator p = sel.begin(); p != sel.end(); ++p){
         const SelectArray::ArrayDim& cur = *p;
         int curloop=get_loopLevel(cur);
         if (maxLoop < curloop) maxLoop = curloop;
     }
     LocalVar* cphandle = loopVec[maxLoop].handle;
     return cphandle; 
}

BlockCopyArraySpec::  
BlockCopyArraySpec(LocalVar* handle, CopyArrayConfig& config, 
                     BlockSpec& _block)
     : CopyArraySpec(handle,handle->toString(OUTPUT_NO_DEBUG),config)
{ 
  int loopnum = _block.get_loopnum();
#ifdef DEBUG
std::cerr << "loopnum=" << loopnum << "\n";
#endif

  for (int i = 0; i < loopnum; ++i) {
     loopVec.push_back(_block.get_loop(i));
  }
  for (SelectArray::iterator p = sel.begin(); p != sel.end();){
       const SelectArray::ArrayDim& cur = *p;
       int minLoop = get_loopLevel(cur);
       if (minLoop < 0 || minLoop >= loopnum)  //QY: skip non-blocked dimension
       {
           SelectArray::iterator p1 = p;
           ++p;
           sel.erase(p1);
       }
       else ++p;
   }
   compute_config();
}


POETCode* BlockCopyArraySpec::
compute_copyBlock(POETProgram& poet)
{
      POETCode* blockPar = BlockSpec::get_blockDimVar(targetName);
      std::string arrname = sel.arr_name();
      int selsize = sel.sel_dim();

      std::vector<POETCode*>  cpblockVec(selsize); 
      for (int i = 0; i < selsize; ++i) cpblockVec.push_back(0);

      for (SelectArray::iterator p = sel.begin(); p != sel.end(); ++p) {
         const SelectArray::ArrayDim& cur = *p;
         int minLoop=get_loopLevel(cur);
         if (minLoop < 0) continue;
         int permuteIndex = placeVec[minLoop];
         std::string ivarname = get_cpIvarName(arrname,permuteIndex);

            /*QY: current blocking dimension */
            POETCode* curblock = blockPar;
            for (int i = minLoop-1; i >= 0; --i) 
                curblock = CDR(curblock);
            /*QY set up copy block dimension  */
            cpblockVec[permuteIndex]=CODE_ACC("CopyBlock",TUPLE3(STRING(ivarname), STRING(loopVec[minLoop].ivarname+"_bk"), CAR(curblock)));
      }
      POETCode* cpblock=0;
      for (int i = cpblockVec.size() - 1; i >= 0; --i) {
          if (cpblockVec[i] != 0)  /* QY: should we check at all here? */
              cpblock = LIST(cpblockVec[i], cpblock);
      }
     return cpblock;
}

POETCode* CopyArraySpec:: 
insert_paramDecl(AutoTuningInterface& tune, POETProgram& poet, 
                 OptLevel optLevel, int& lineNo)
{
     std::string arrname = sel.arr_name();
     std::string prefix = get_opt_prefix(optLevel);
     std::string paramName = prefix + "_config_" + arrname;
     LocalVar* paramVar = PARAM_VAR(paramName);
     std::string optname=(optLevel==OPT_CACHE_LEVEL)?"copy":"scalarRepl";

     paramVar->get_entry().set_code(ICONST(1));
     paramVar->get_entry().set_restr(RANGE_TYPE(ICONST(0),ICONST(2)));
     InsertParamDecl(poet,paramVar,
           "configuration for " + optname + " array " + arrname + " at loop " + targetName + ": 0-no opt; 1-" + optname + "; 2-strength reduction.",
           lineNo++);
     return 0;
}

POETCode* CopyArraySpec::
gen_copyInvoke(POETProgram& poet, POETCode* cphandle, LocalVar* top, 
        POETCode* cpblock, bool scalar, POETCode* traceMod, int& lineNo)
{
  std::string arrname = sel.arr_name();
  if (opt & COPY_INSIDE) cphandle=TUPLE_ACC(cphandle,POET_static::Nest_body);

  /* QY: compute copy configurations */
  POETCode* elemType=STRING(AstInterface::GetBaseTypeName(sel.elem_type()));

  int selsize = sel.sel_dim();
  std::string bufname = get_bufName(arrname,scalar);
  POETCode* cpDim = compute_copyDim(poet,scalar);
  POETCode* copyInvoke = gen_copyInvoke(poet,cphandle,top,
           arrname, elemType, opt, cpDim, cpblock, scalar, traceMod);
  lineNo++;

  if (!scalar) {
     copyInvoke=SEQ(poet,gen_cpIvarDecl(poet,top,arrname,selsize,true), copyInvoke);
     lineNo+=2;
  }

  return copyInvoke;
}

POETCode* CopyArraySpec::
gen_xformEval(POETProgram& poet, LocalVar* top, 
                   POETCode* traceMod, OptLevel optLevel, int& lineNo)
{
  /*QY: compute configuration*/
  bool scalar = do_scalarRepl(optLevel);
  /*QY: set up tuning parameters and trace vars*/
  POETCode* copyInvoke=gen_copyInvoke(poet,target,top,0,scalar,traceMod,lineNo);

  lineNo += 2;
  std::string arrname = sel.arr_name();
  std::string prefix = get_opt_prefix(optLevel);
  std::string paramName = prefix + "_config_" + arrname;
  LocalVar* paramVar = PARAM_VAR(paramName);
  return IFELSE(paramVar,copyInvoke,EMPTY);
}

POETCode* BlockCopyArraySpec::
gen_xformEval(POETProgram& poet, LocalVar* top, 
                   POETCode* traceMod, OptLevel optLevel, int& lineNo)
{
  /*QY: compute configuration*/
  bool scalar = do_scalarRepl(optLevel);
  bool scalar_only =sel.scalar_repl();
  std::string prefix = get_opt_prefix(optLevel);
  std::string arrname = sel.arr_name();
  std::string paramName = prefix + "_config_" + arrname;
  LocalVar* paramVar = PARAM_VAR(paramName);

  std::vector<POETCode*> expDimVec_cp, expDimVec_nocp;
  compute_fdConfig(poet,target,scalar,expDimVec_cp,expDimVec_nocp);
  LocalVar* parHandle = 0;

  POETCode* cphandle=target;
  if (scalar) { cphandle =  scalarRepl_handle(poet);
                   opt = (CopyArrayOpt)(opt | COPY_INSIDE); }
  else if (!scalar_only) opt = (CopyArrayOpt)(opt & (~ COPY_INSIDE));
  std::string bufname = get_bufName(arrname,scalar);
  LocalVar* arrVar = get_arrVar(poet,arrname);
  LocalVar* dimVar = get_dimVar(poet,arrname);
  POETCode* arrexp = AutoTuningInterface::Access2Array(poet,arrVar,sel.sel_dim());
  POETCode* elemType=STRING(AstInterface::GetBaseTypeName(sel.elem_type()));

  /*QY: set up tuning parameters and trace vars*/
  POETCode* copyInvoke=0;
  if (scalar || !scalar_only) {
     POETCode* copyblock =0;
     if (!scalar) copyblock = compute_copyBlock(poet);
     /*QY: generate copy invocation*/
     copyInvoke=gen_copyInvoke(poet,cphandle,top,copyblock,scalar,traceMod,lineNo);
     POETCode* fdInvoke = POET_static::gen_fdInvoke(poet,top,target,get_fdName(bufname),arrexp,
          CODE_ACC("PtrType",elemType), expDimVec_cp, 0, traceMod, false);
     if (fdInvoke!=0) {
        fdInvoke=TRACE_EVAL(traceMod,fdInvoke);
        copyInvoke = SEQ(poet,copyInvoke, fdInvoke);
        lineNo++;
     }
     copyInvoke=SEQ(poet,copyInvoke, ASSIGN(dimVar, AfterCopy_dimSize(poet)));
     ++lineNo;
  }
  if (copyInvoke == 0 && sel.arr_dim() > 1) return 0; //QY: don't do anything due to lack of info on dimension size 

  POETCode* fdInvoke2 = 
          POET_static::gen_fdInvoke(poet,top,target,get_fdName(bufname),arrexp,
          CODE_ACC("PtrType",elemType), expDimVec_nocp, permute,
          traceMod, false);
  if (fdInvoke2!=0) {
      fdInvoke2=IFELSE(BOP(POET_OP_EQ,paramVar,ICONST(2)),
                       TRACE_EVAL(traceMod,fdInvoke2),EMPTY);
      lineNo+=4;
  }
  else fdInvoke2=EMPTY;

  POETCode* invoke = 0;
  if (copyInvoke != 0) {
     invoke = IFELSE(BOP(POET_OP_EQ,paramVar,ICONST(1)),
                     copyInvoke,fdInvoke2);
     lineNo+=3;
  }
  else if (fdInvoke2!=0) { /*QY: finite diff only*/
     invoke = fdInvoke2;
     ++lineNo;
  }

  if (parHandle != 0){ /*QY: disable finiteDiff if the nest is parallelized*/ 
       invoke=IFELSE(BOP(POET_OP_EQ,parHandle,ICONST(1)),invoke,EMPTY);
       lineNo += 2;
  }
  return invoke;
}

int BlockCopyArraySpec:: compute_fdConfig
(POETProgram& poet, POETCode* handle, bool scalar,
       std::vector<POETCode*>& expDimVec_cp, /*QY:copy+strength reduction*/
       std::vector<POETCode*>& expDimVec_nocp) /*QY:no copy,just strength reduction*/
{
  if (sel.arr_dim() > 1) return -1; //QY: don't do anything to multi-dimensional arrays due to lack of info on dimension size 

  std::string arrname = sel.arr_name();
  SymbolicVal2POET val2poet(poet);

  LocalVar* dimVar = get_dimVar(poet,arrname);
  unsigned  selsize = sel.sel_dim();
  for (unsigned i= 0; i < selsize; ++i) { 
          expDimVec_cp.push_back(0);
          expDimVec_nocp.push_back(0);
  }
  int sel_index=0;
  int outermostLevel = selsize;
  /*QY set up array addr dimension for strength reduction*/
  for (SelectArray::iterator p = sel.begin(); p != sel.end(); ++sel_index,++p){
          const SelectArray::ArrayDim& cur = *p;
          int minLoop=get_loopLevel(cur);
          if (minLoop < 0) continue;
          if (minLoop < outermostLevel) outermostLevel = minLoop;
          int permuteIndex = placeVec[minLoop];
          POETCode* curdist = 0, *curdist2=0;
          if (selsize > 1 && !scalar) {
             if (permuteIndex + 1 == selsize) 
                curdist = BlockSpec::get_blockSize(targetName,minLoop);
             else 
                curdist=STRING(get_cpIvarName(arrname,permuteIndex+1));
             curdist2 = val2poet.apply(cur.get_incr()); 
         } 
         else {
             curdist=curdist2=TUPLE_ACC(dimVar,ICONST(sel_index));
         }
         expDimVec_cp[permuteIndex] = 
                  CODE_ACC("ExpDim",TUPLE3(loopVec[minLoop].handle,ICONST(1),curdist));
         expDimVec_nocp[permuteIndex]= CODE_ACC("ExpDim",TUPLE3(loopVec[minLoop].handle,ICONST(1),curdist2));
      }
    return outermostLevel;
}

POETCode* BlockCopyArraySpec::
AfterCopy_dimSize(POETProgram& poet)
{
   SymbolicVal2POET val2poet(poet);
   POETCode* cpDim = 0;
   int selsize=sel.sel_dim();
   for (SelectArray::iterator p = sel.begin(); p != sel.end(); ++p){
       const SelectArray::ArrayDim& cur = *p;
       int minLoop=get_loopLevel(cur);
       if (minLoop < 0) continue;
       int permuteIndex = placeVec[minLoop];
       SymbolicVal dimsize = 1;
       for (int i = permuteIndex+1; i < selsize; ++i) 
          dimsize = dimsize *(new SymbolicPOETWrap(BlockSpec::get_blockSize(targetName,i)));
        cpDim=TUPLE_APPEND(cpDim,val2poet.apply(dimsize));
    }
    return cpDim;
}
