#include <fstream>
#include <AutoTuningInterface.h>
#include <LoopTransformInterface.h>
#include <LoopTree.h>
#include <BlockingAnal.h>
#include <SymbolicExpr.h>

/*QY: poet header files */
#include <poetAST.h>
#include <ASTfactory.h>
/***********************************************************************/
/******QY: macros for creating POET instructions************************/
/***********************************************************************/

#define PAR_PARAM_NAME(handleName)  "pthread_" + handleName
#define PAR_BLOCK_NAME(handleName)  "psize_" + handleName

#define ANY  POETProgram::make_any()
#define STRING(name) POETProgram::make_string(name)
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
#define ATTR_VAR(scope,name)  POETProgram::make_localVar(scope->get_entry().get_symTable(), STRING(name), LVAR_ATTR)
#define ATTR(var,fd) POETProgram::make_attrAccess(var,STRING(fd))
#define XFORM_VAR(name)  POETProgram::make_xformVar(STRING(name))
#define CODE_VAR(name)  POETProgram::make_codeRef(STRING(name))
#define CODE_REF(name,args)  POETProgram::make_codeRef(STRING(name),args)
#define CODE_SET_ATTR(code,name,val)  code->get_entry().append_attr(poet->set_local_static(code->get_entry().get_symTable(),STRING("match"), LVAR_ATTR, val, val, true))

#define BOP(op,opd1,opd2) poet.make_Bop(op, opd1, opd2)
#define TOR(opd1,opd2) poet.make_typeTor(opd1, opd2)
#define ERASE(opd)  BOP(POET_OP_ERASE, opd, opd)
#define COPY(opd)  POETProgram::make_Uop(POET_OP_COPY, opd)
#define REPLACE(opd1,opd2,opd3)  POETProgram::make_Top(POET_OP_REPLACE, opd1, opd2,opd3)
#define DELAY(opd)  POETProgram::make_Uop(POET_OP_DELAY, opd)
#define APPLY(opd)  POETProgram::make_Uop(POET_OP_APPLY, opd)
#define TUPLE_ACC(var,acc) poet.make_tupleAccess(var,acc)
#define TUPLE_APPEND(v1,v2) poet.append_tuple(v1,v2)
#define PAIR(v1,v2) POETProgram::make_pair(v1,v2) 
#define TUPLE3(v1,v2,v3)  POETProgram::make_tuple3(v1,v2,v3)
#define TUPLE4(v1,v2,v3,v4)  POETProgram::make_tuple4(v1,v2,v3,v4)
#define TUPLE5(v1,v2,v3,v4,v5)  POETProgram::make_tuple5(v1,v2,v3,v4,v5)
#define LIST(head,tail) POETProgram::make_list(head,tail)
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
#define ARRAY_REF(arr,sub) CODE_REF("ArrayAccess", PAIR(arr,sub))

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

  CodeVar* const copyConfig = CODE_VAR("CopyConfig");
  LocalVar* const copyConfig_permute=CONFIG_VAR(copyConfig,"permute");
  LocalVar* const copyConfig_cpBlock=CONFIG_VAR(copyConfig,"cpBlock");

  XformVar* const block_xvar = XFORM_VAR("BlockLoops");
  LocalVar* const block_cleanup=CONFIG_VAR(block_xvar,"cleanup");
  LocalVar* const block_factor =CONFIG_VAR(block_xvar,"factor");
  LocalVar* const block_tracemod=CONFIG_VAR(block_xvar,"trace_mod");
  LocalVar* const block_tile=CONFIG_VAR(block_xvar,"trace_innerNest");
  LocalVar* const block_decl=CONFIG_VAR(block_xvar,"trace_decl");
  LocalVar* const block_nonperfect=CONFIG_VAR(block_xvar,"nonPerfect");

  XformVar* const traceNest=XFORM_VAR("TraceNest");
  POETCode* const traceNest_config=ASSIGN(CONFIG_VAR(traceNest,"list_of_handles"),One);

  XformVar* const modifyHandle= XFORM_VAR("ModifyTraceHandle");
  LocalVar* const modifyHandle_trace= CONFIG_VAR(modifyHandle,"trace");

};

ArrayAbstractionInterface* AutoTuningInterface::arrayInfo = 0;
POETCode* AutoTuningInterface::arrayAccess = 0;
CodeVar* const AutoTuningInterface::Array = CODE_VAR("ArrayAccess");
POETCode* const AutoTuningInterface::Array_var = ATTR(Array, "array");
POETCode* const AutoTuningInterface::Array_sub = ATTR(Array, "subscript");
CodeVar* const AutoTuningInterface::FunctionCall = CODE_VAR("FunctionCall");
POETCode* const AutoTuningInterface::FunctionCall_arg = ATTR(FunctionCall, "args");

CodeVar* const OptSpec::Nest = CODE_VAR("Nest");
CodeVar* const OptSpec::Loop = CODE_VAR("Loop");
CodeVar* const OptSpec::LoopBound = CODE_VAR("LoopBound");
CodeVar* const OptSpec::IntType = CODE_VAR("IntegerType");
POETCode* const OptSpec::Nest_ctrl = ATTR(OptSpec::Nest, "ctrl");
POETCode* const OptSpec::Nest_body = ATTR(OptSpec::Nest, "body");
POETCode* const OptSpec::Loop_ivar=ATTR(OptSpec::Loop,"i");
POETCode* const OptSpec::Loop_step=ATTR(OptSpec::Loop,"step");
POETCode* const OptSpec::Loop_ub=ATTR(OptSpec::Loop,"stop");
POETCode* const OptSpec::LoopBound_ivar=ATTR(OptSpec::LoopBound,"ivar");
POETCode* const OptSpec::LoopBound_step=ATTR(OptSpec::LoopBound,"step");
XformVar* const OptSpec::appendDecl=XFORM_VAR("AppendDecl");

XformVar* const OptSpec:: finiteDiff= XFORM_VAR("FiniteDiff");
LocalVar* const OptSpec:: fd_exp_type= CONFIG_VAR(finiteDiff,"exp_type");
LocalVar* const OptSpec:: fd_trace= CONFIG_VAR(finiteDiff,"trace");
LocalVar* const OptSpec:: fd_traceVar= CONFIG_VAR(finiteDiff,"trace_newVars");
LocalVar* const OptSpec:: fd_traceDecl= CONFIG_VAR(finiteDiff,"trace_decl");
LocalVar* const OptSpec:: fd_is_scalar= CONFIG_VAR(finiteDiff,"scalar");
LocalVar* const OptSpec:: fd_mod= CONFIG_VAR(finiteDiff,"trace_mod");
LocalVar* const OptSpec:: fd_permute= CONFIG_VAR(finiteDiff,"permute");


XformVar* const UnrollSpec::unroll = XFORM_VAR("UnrollLoops");
LocalVar* const UnrollSpec::unroll_factor=CONFIG_VAR(unroll,"factor");
LocalVar* const UnrollSpec::unroll_cleanup=CONFIG_VAR(unroll,"cleanup");

XformVar* const ParLoopSpec::parloop = XFORM_VAR("ParallelizeLoop");
LocalVar* const ParLoopSpec:: par_trace= CONFIG_VAR(parloop,"trace");
LocalVar* const ParLoopSpec:: par_include= CONFIG_VAR(parloop,"trace_include");
LocalVar* const ParLoopSpec:: par_thread= CONFIG_VAR(parloop,"threads");
LocalVar* const ParLoopSpec:: par_private= CONFIG_VAR(parloop,"private");

XformVar* const BlockSpec::unrollJam = XFORM_VAR("UnrollJam");
LocalVar* const BlockSpec::unrollJam_cleanup = CONFIG_VAR(unrollJam,"cleanup");
XformVar* const BlockSpec::cleanup = XFORM_VAR("CleanupBlockedNests");
LocalVar* const BlockSpec::cleanup_trace = CONFIG_VAR(cleanup,"trace");

LocalVar* const BlockSpec::unrollJam_factor=CONFIG_VAR(unrollJam,"factor");
LocalVar* const BlockSpec::unrollJam_trace=CONFIG_VAR(unrollJam,"trace");

XformVar* const CopyArraySpec::copyarray = XFORM_VAR("CopyRepl");
LocalVar* const CopyArraySpec::bufname=CONFIG_VAR(copyarray,"prefix");
LocalVar* const CopyArraySpec::init_loc=CONFIG_VAR(copyarray,"init_loc");
LocalVar* const CopyArraySpec::save_loc=CONFIG_VAR(copyarray,"save_loc");
LocalVar* const CopyArraySpec::delete_loc=CONFIG_VAR(copyarray,"delete_loc");
LocalVar* const CopyArraySpec::elem_type=CONFIG_VAR(copyarray,"data_type");
LocalVar* const CopyArraySpec::is_scalar=CONFIG_VAR(copyarray,"scalar");
LocalVar* const CopyArraySpec::trace=CONFIG_VAR(copyarray,"trace");
LocalVar* const CopyArraySpec::trace_decl=CONFIG_VAR(copyarray,"trace_decl");
LocalVar* const CopyArraySpec::trace_vars=CONFIG_VAR(copyarray,"trace_vars");
int CopyArraySpec::index=0;
LocalVar* const CopyArraySpec::trace_mod=CONFIG_VAR(copyarray,"trace_mod");

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
            res=STRING(AstToString(v.get_ast()));
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
        POETCode* args = 0;
        for (int i=v.NumOfArgs()-1; i >= 0; --i) {
           args = LIST(apply(v.get_arg(i)),args);
        }
        res = CODE_REF("FunctionCall", PAIR(STRING(v.GetOp()),args));
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
                   cur = CODE_REF(pt, PAIR(cur, apply(exp.Term2Val(p.Current()))));
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

void HandleMap::
ObserveCopyAst(AstInterfaceImpl& fa, 
      const AstNodePtr& orig, const AstNodePtr& n)
{
  AstMapType::const_iterator p = astMap.find(orig.get_ptr());
#ifdef DEBUG
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
#ifdef DEBUG
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
#ifdef DEBUG
std::cerr << "look for trace var: " << n << "\n";
#endif
      if (p != astMap.end()) {
        LocalVar* v = (*p).second;
        assert(v != 0);
#ifdef DEBUG
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
     POETCode* eval = SEQ3(poet,BOP(POET_OP_INSERT, handles[0], target),
                           ASSIGN(decl,EMPTY), ASSIGN(top,LIST(decl,handle)));
     int evalline = 3;
     for (int i = 1; i < handlenum; ++i) {
            eval = SEQ(poet,eval, 
                       INVOKE(POET_static::modifyHandle, 
                              ASSIGN(POET_static::modifyHandle_trace,target), 
                              PAIR(handles[i],EMPTY)));
            evalline ++;
     }
     MAIN_EVAL(poet, 
         SEQ(poet,eval,INVOKE(POET_static::modifyHandle, 
                              ASSIGN(POET_static::modifyHandle_trace, target), 
                              PAIR(handles[0],top))),
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

OptSpec::OptSpec(LocalVar* _handle)
   : handle(_handle), handleName(_handle->toString(OUTPUT_NO_DEBUG)) {} 

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
    return CAR(TUPLE_ACC(ref, TOR(FunctionCall_arg,Array_var)));
  }
  else return TUPLE_ACC(ref,Array_var);
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
                new SymbolicPOETWrap(ATTR_VAR(Array,"array")),
                new SymbolicPOETWrap(ATTR_VAR(Array,"subscript")));
          if (!access.IsNIL()) {
              SymbolicVal2POET val2poet(*poet);
              CODE_SET_ATTR(Array, "match", val2poet.apply(access));
          }
          poet->insert_codeDecl(Array); ++lineNo;
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
    for (int i = 1; i <= OptSpec::OPT_LEVEL_MAX; i <<= 1) {
      OptSpec::OptLevel curLevel = (OptSpec::OptLevel)i;
      if (!(curLevel & applyOpt)) continue;
      for (std::vector<OptSpec*>::const_iterator 
           p = optvec.begin(); p != optvec.end(); ++p) {
           OptSpec* curopt = *p;
           if (curopt->get_opt_level() & curLevel) 
               curopt->insert_paramDecl(*poet,curLevel, lineNo); 
      }
    }
    ++lineNo;

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
                  LocalVar* optvar = EVAL_VAR(*poet, curopt->get_opt_prefix(curLevel)+"_"+curopt->get_handleName()); 
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
      LoopBlocking* config, const FuseLoopInfo* nonperfect)
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

BlockSpec* AutoTuningInterface:: LoopBlocked(LocalVar* outerhandle)
{
   for (std::vector<OptSpec*>::const_iterator p = optvec.begin();
         p != optvec.end(); ++p) {
       BlockSpec* cur = dynamic_cast<BlockSpec*>(*p);
       if (cur != 0) {
          if (cur->get_handle() == outerhandle) return cur;   
       }
   }
   return 0;
}

void AutoTuningInterface::
CopyArray( CopyArrayConfig& config, LoopTreeNode* repl)
{
//std::cerr << "copy config=" << config.toString() << "\n";
//std::cerr << "copyroot=" << repl->toString() << "\n";
  LocalVar* outerHandle = 0, *handle = 0;
  if (repl->IncreaseLoopLevel()) {
     outerHandle = handleMap.GetLoopHandle(repl);
  }
  else {
     LoopTreeNode* repl1 = repl;
     while (repl1->ChildCount()==1 && !repl1->IncreaseLoopLevel())  
          { repl1=repl1->FirstChild(); }
     outerHandle = handle= handleMap.GetLoopHandle(repl1);
  }
  BlockSpec* block = LoopBlocked(outerHandle);
  if (block!=0 && !config.get_arr().scalar_repl()) {
         OptSpec* res= new BlockCopyArraySpec(outerHandle,config, *block);
         optvec.push_back(res);
  }
  else {
      if (handle == 0) handle = handleMap.GetBodyHandle(repl,outerHandle);
      OptSpec* res = new CopyArraySpec(handleMap,handle,config,repl);
      optvec.push_back(res);
   }
}

/***********************************************************************/
/******QY: finite differencing  impl **********************************/
/***********************************************************************/

POETCode* OptSpec::
gen_fdInvoke(POETProgram& poet, LocalVar* top,
         const std::string& name,POETCode* exp, POETCode* expType,
         const std::vector<POETCode*>& dimVec, 
         POETCode* permute, POETCode* traceMod, bool isScalar)
{
  if (dimVec.size() == 0) return 0;
  POETCode* config=SEQ4(poet,ASSIGN(fd_exp_type, expType),
                        ASSIGN(fd_trace,top),
                        ASSIGN(fd_is_scalar, ICONST(isScalar)),
                        ASSIGN(fd_traceDecl,HandleMap::GetTraceDecl(top)));
  LocalVar* privateTrace = HandleMap::FindTracePrivate(top);
  if (privateTrace != 0)
          config=SEQ(poet, config,ASSIGN(fd_traceVar,privateTrace));
  if (permute != 0) config = SEQ(poet,config, ASSIGN(fd_permute, permute));
  if (traceMod != 0) config = SEQ(poet,config, ASSIGN(fd_mod, traceMod));

  POETCode* dim = 0;
  for (int i = dimVec.size()-1; i >= 0;  --i) 
          dim = LIST(dimVec[i], dim);
  POETCode* args = TUPLE4(STRING(name),exp,dim, handle);
  POETCode* result =  INVOKE(finiteDiff, config, args);
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

UnrollSpec :: UnrollSpec (LocalVar* handle, int unrollSize) : OptSpec(handle)
{
    paramVar = get_unrollSizeVar(handleName);
    paramVar->get_entry().set_code(ICONST(unrollSize));
    paramVar->get_entry().set_restr(RANGE_TYPE(ICONST(1),ANY));
}


void UnrollSpec::insert_paramDecl(POETProgram& poet, OptLevel optLevel,
                                int& lineNo)
{
  InsertParamDecl(poet,paramVar, "Unroll factor for loop " + handleName, lineNo++);
}

POETCode* UnrollSpec::
gen_xformEval(POETProgram& poet, LocalVar* top, 
                   POETCode* traceMod, OptLevel optLevel, int& lineNo) 
{
  lineNo+=1;
  return INVOKE(unroll, 
          SEQ(poet,ASSIGN(unroll_factor,paramVar),ASSIGN(unroll_cleanup,ICONST(0))),
          PAIR(TUPLE_ACC(handle,Nest_body), handle));
}

/***********************************************************************/
/******QY: loop parallelization impl **********************************/
/***********************************************************************/

ParLoopSpec :: ParLoopSpec(LocalVar* handle, LoopTreeNode* outerLoop,int bsize) 
   : OptSpec(handle)
{
    parVar = PARAM_VAR(PAR_PARAM_NAME(handleName));
    parVar->get_entry().set_code(ICONST(1));
    parVar->get_entry().set_restr(RANGE_TYPE(ICONST(1),ANY));

    parblockVar = PARAM_VAR(PAR_BLOCK_NAME(handleName));
    parblockVar->get_entry().set_code(ICONST(bsize));
    parblockVar->get_entry().set_restr(RANGE_TYPE(ICONST(1),ANY));

   privateVars=0;
   std::string ivar;
   for (LoopTreeTraverseSelectLoop loops(outerLoop);
        !loops.ReachEnd(); loops.Advance()) {
      LoopTreeNode* cur = loops.Current();
      std::string curvar = cur->GetLoopInfo()->GetVar().toString();
      if (ivar == "") { 
         ivar = curvar; ivarName=STRING(ivar); bvarName=STRING(ivar+"_par"); 
         privateVars = LIST( bvarName, LIST(ivarName,privateVars));
      }
      else privateVars = LIST( STRING(curvar),privateVars);
   }
}

void ParLoopSpec::
insert_paramDecl(POETProgram& poet, OptLevel optLevel, int& lineNo)
{
  InsertParamDecl(poet,parVar, "number of threads to parallelize loop " + handleName, lineNo++);
  InsertParamDecl(poet,parblockVar, "number of loop iterations to run by different threads for " + handleName, lineNo++);
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

  POETCode* config=SEQ4(poet,ASSIGN(par_trace,top), 
                    ASSIGN(par_private,privateTrace), 
                    ASSIGN(par_include, HandleMap::GetTraceInclude()), 
                         ASSIGN(par_thread,parVar));
  POETCode* blockDim = CODE_REF("BlockDim", TUPLE3(ivarName, bvarName, parblockVar));
  POETCode* blockconfig= SEQ(poet, ASSIGN(POET_static::block_factor, blockDim),
                         ASSIGN(POET_static::block_cleanup, POET_static::Zero));
  if (traceMod!=0) blockconfig=SEQ(poet,blockconfig,
                         ASSIGN(POET_static::block_tracemod,traceMod));
  POETCode* blockInvoke = INVOKE(POET_static::block_xvar, blockconfig,
             PAIR(TUPLE_ACC(handle,Nest_body) , handle));
  if (traceMod != 0) blockInvoke=TRACE_EVAL(traceMod, blockInvoke);
  POETCode* invoke= 
     SEQ5(poet,blockInvoke,
        INVOKE(parloop, config, handle),
        BOP(POET_OP_ERASE, handle, top),
        ASSIGN(handle,TUPLE_ACC(handle,Nest_body)),
        REPLACE(ERASE(handle),handle,top));
  lineNo += 9;

  LocalVar* pcleanup = HandleMap::FindTraceCleanup(top);
  if (pcleanup != 0) { /*QY: modify cleanup trance */
      invoke=SEQ(poet,invoke,ASSIGN(pcleanup, handle));
      ++lineNo;
  }

  return SEQ3(poet,ASSIGN(privateTrace,privateVars),
     INVOKE(appendDecl,0,TUPLE3(IntType,bvarName,HandleMap::GetTraceDecl(top))),
           IFELSE(BOP(POET_OP_NE,parVar,ICONST(1)), invoke, EMPTY)); 
}

/***********************************************************************/
/******QY: loop blocking  impl **********************************/
/***********************************************************************/

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

POETCode* FuseLoopInfo::toPOET(HandleMap& handleMap, const FuseLoopInfo* info)
{
   if (info == 0) return 0;
   POETCode* nonperfect=0;
   if (info->loops.size() == 1) 
        nonperfect=handleMap.GetLoopHandle(info->loops[0].first);
/*QY: if need to consider alignment, should build a separate list  
                    PAIR(handleMap.GetLoopHandle(info->loops[i].first),
                         ICONST(info->loops[i].second));
*/
   else {
        for (int i = info->loops.size()-1; i>=0; --i) { 
           nonperfect=LIST(handleMap.GetLoopHandle(info->loops[i].first),nonperfect);
         }
   }
   POETCode* pivot = 0;
   if (info->pivotLoop==0) pivot=EMPTY;
   else pivot=handleMap.GetLoopHandle(info->pivotLoop);
   return CODE_REF("NonPerfectLoops", PAIR(pivot,nonperfect));
}

BlockSpec::BlockSpec(HandleMap& _handleMap, 
            LocalVar* outerHandle, LoopTreeNode* _innerLoop, 
            LoopBlocking* config, 
            const FuseLoopInfo* _nonperfect) 
   : OptSpec(outerHandle), handleMap(_handleMap), loopnum(config->NumOfLoops())
{
  assert(loopnum > 1); 
   if (_nonperfect == 0) { nonperfect=0; }
   else nonperfect=FuseLoopInfo::toPOET(handleMap,_nonperfect);
   for (unsigned i = 0; i < loopnum; ++i) loopVec.push_back(LoopInfo());

   /*QY: set up loopVec; */
   LoopTreeNode* p = _innerLoop;
   if (_nonperfect!=0) {
      assert(_nonperfect->loops[0].first == p);
      for (unsigned i = 1; i < _nonperfect->loops.size(); ++i) {
         LoopTreeNode* curloop = _nonperfect->loops[i].first;
         loopVec.push_back(LoopInfo(handleMap.GetLoopHandle(curloop),curloop->GetLoopInfo()->GetVar().GetVarName())); 
      }
   }
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

  /* QY: set up paramVar and loopVec*/
   POETCode* bsize = 0, *btype=0;
   for (int i = loopnum-1; i >= 0;  --i) {
       SymbolicVal cur_val = config->BlockSize(i);
       int cur = 0;
       if (!cur_val.isConstInt(cur)) assert(0);
       bsize=LIST(ICONST(cur),bsize); 
       btype = LIST(INT_TYPE, btype);
   }
   blockPar = get_blockSizeVar(handleName);
   blockPar->get_entry().set_code(bsize);
   blockPar->get_entry().set_restr(btype);

   ujPar = get_unrollJamSizeVar(handleName);
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
       blockDim = LIST(CODE_REF("BlockDim", 
                   TUPLE3(STRING(ivarname),STRING(ivarname+"_bk"),blockVec[i])),
                     blockDim);
    }
    return blockDim;
}


void BlockSpec::
insert_paramDecl(POETProgram& poet, OptLevel optLevel, int& lineNo)
{
  switch (optLevel) {
   case OPT_CACHE_LEVEL: 
      InsertParamDecl(poet, blockPar, "Blocking factor for loop nest " + handleName, lineNo++);
      return;
   case OPT_REG_LEVEL: 
      /*QY: unrolljam command-line parameter; default value set to 2 */
      InsertParamDecl(poet, ujPar, "Unroll and Jam factor for loop nest " + handleName, lineNo++);
      return;
   case OPT_CLEANUP_LEVEL: return;
   default: 
       std::cerr << "unexpected opt level: " << optLevel << "\n";
       assert(0);
  }
}

void BlockSpec::
insert_xformDecl(POETProgram& poet, LocalVar* top, POETCode*& traceMod, int& lineNo)
{
    DECL_TRACE(poet, get_blockTileVar(handleName),lineNo++); 
    LocalVar* cleanupvar = HandleMap::GetTraceCleanup(top);
    MAIN_EVAL(poet, ASSIGN(cleanupvar, top), lineNo++);
}

POETCode* BlockSpec::gen_xformEval(POETProgram& poet, LocalVar* top, 
                   POETCode* traceMod, OptLevel optLevel, int& lineNo)
{

   /*QY: set up blocking dimension configuration*/
  POETCode* decl = HandleMap::GetTraceDecl(top);
  LocalVar* innerHandle = loopVec[loopnum-1].handle;
  LocalVar* tileVar = get_blockTileVar(handleName);

  if (optLevel == OPT_CACHE_LEVEL) {
      POETCode* before= 
         SEQ(poet,ASSIGN(get_blockDimVar(handleName),compute_blockDim(blockPar)),
                 ASSIGN(get_blockTileVar(handleName), COPY(handle)));
      lineNo +=2;
      POETCode* config= 
        SEQ4(poet,ASSIGN(POET_static::block_factor,get_blockDimVar(handleName)),
                  ASSIGN(POET_static::block_cleanup,POET_static::Zero), 
                  ASSIGN(POET_static::block_tile, tileVar),
                  ASSIGN(POET_static::block_decl,decl));
      if (nonperfect != 0) 
           config = SEQ(poet,config, 
                      ASSIGN(POET_static::block_nonperfect, nonperfect));
      
      if (traceMod!=0) 
          config=SEQ(poet,config,
                     ASSIGN(POET_static::block_tracemod,traceMod));
      POETCode* blockInvoke = INVOKE(POET_static::block_xvar, config, PAIR(TUPLE_ACC(innerHandle,Nest_body), handle));
      if (traceMod!=0) 
         blockInvoke=TRACE_EVAL(traceMod,blockInvoke);

      POETCode* bvars = 0;
      for (unsigned i = 0; i < loopnum; ++i) {
           bvars = LIST(STRING(loopVec[i].ivarname+"_bk"),bvars);
      }
      lineNo += 3; 
      POETCode* res = SEQ3(poet,before,
                INVOKE(appendDecl,0,TUPLE3(IntType,bvars,decl)), 
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
                INVOKE(POET_static::traceNest, POET_static::traceNest_config, 
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
         POETCode* body_val = TUPLE_ACC(cur,Nest_body);
         if (body != 0) {
            before=SEQ3(poet,before,ASSIGN(body,body_val),
                        INVOKE(POET_static::modifyHandle,
                               ASSIGN(POET_static::modifyHandle_trace,cur),
                               PAIR(ERASE(body),body)));
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

      POETCode* config= SEQ3(poet,ASSIGN(unrollJam_factor, bsize),
                             ASSIGN(unrollJam_cleanup, ICONST(0)),
                             ASSIGN(unrollJam_trace, top));
      POETCode* param = PAIR(loopVec[loopnum-1].handle,loopVec[0].handle);
      lineNo ++;
      return SEQ(poet,before, INVOKE(unrollJam, config, param));
  }
  else if (optLevel == OPT_CLEANUP_LEVEL) {
      lineNo ++;
      return INVOKE(cleanup, ASSIGN(cleanup_trace,top), HandleMap::GetTraceCleanup(top));
  }
}

/***********************************************************************/
/******QY: array copying  impl **********************************/
/***********************************************************************/

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
CopyArraySpec(HandleMap& handleMap, LocalVar* handle, CopyArrayConfig& config, LoopTreeNode* root)
     : OptSpec(handle), sel(config.get_arr()), opt(config.get_opt()),permute(0)
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
   { return EVAL_VAR(poet, handleName + "_" + cur_id + "_" + arrname+"_dim"); }

/* QY: trace handle for array name */
LocalVar* CopyArraySpec:: get_arrVar
(POETProgram& poet, const std::string& arrname)
{  return EVAL_VAR(poet, handleName + "_" + cur_id + "_" + arrname); }

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

POETCode* CopyArraySpec::
compute_copySubscript(POETProgram& poet, bool afterCopy) 
{
      std::string arrname = sel.arr_name();
      SymbolicVal2POET val2poet(poet);

      std::vector<SymbolicVal> subscriptVec;
      int arrDim = sel.arr_dim();
      for (int i = 0; i < arrDim; ++i) 
             subscriptVec.push_back(sel.sel_start(i));

      LocalVar* dimVar=get_dimVar(poet,arrname);
      int i = 0;
      for (SelectArray::iterator p = sel.begin(); p != sel.end(); ++i,++p) {
         const SelectArray::ArrayDim& cur = *p;
         int minLoop=get_loopLevel(cur);
         if (minLoop >= 0) {
            std::string ivarname = loopVec[minLoop].ivarname; 
            SymbolicVar curivar(ivarname,AST_NULL);

            /*QY: set up subscript*/
            int seldim = cur.get_arrDim();

            if (!afterCopy) subscriptVec[seldim]=subscriptVec[seldim]+curivar*cur.get_incr();
            else subscriptVec[seldim]=subscriptVec[seldim]+curivar*SymbolicPOETWrap(TUPLE_ACC(dimVar,ICONST(i)));
         }
      }
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
             cpDimVec[i] = (CODE_REF("CopyDim", 
                           TUPLE4(STRING(ivarname),ICONST(1), ICONST(1), 
                                  TUPLE_ACC(dimVar,ICONST(i)))));
         }
         else if (!scalar) {
           int permuteIndex = placeVec[minLoop];
           LocalVar* loop = loopVec[minLoop].handle;
           std::string ivarname = loopVec[minLoop].ivarname;//get_cpIvarName(arrname,permuteIndex);
           cpDimVec[permuteIndex]=
               CODE_REF("CopyDim", TUPLE4(STRING(ivarname), ICONST(0), 
                                            //val2poet.apply(cur.get_size()), 
                       TUPLE_ACC(TUPLE_ACC(loop,Nest_ctrl),Loop_ub), 
                                            val2poet.apply(cur.get_incr())));
         }
         else {
             std::string ivarname = loopVec[minLoop].ivarname;
             LocalVar* loop = loopVec[minLoop].handle;
             cpDimVec[i] = (CODE_REF("CopyDim", 
                           TUPLE4(STRING(ivarname),
                                  TUPLE_ACC(TUPLE_ACC(loop,Nest_ctrl),TOR(Loop_ivar,LoopBound_ivar)), 
                                  TUPLE_ACC(TUPLE_ACC(loop,Nest_ctrl),TOR(Loop_step,LoopBound_step)),
                                  TUPLE_ACC(dimVar,ICONST(i)))));
         }
      }
      POETCode* cpDim = 0;
      for (int i = cpDimVec.size() - 1; i >= 0; --i) 
          cpDim = LIST(cpDimVec[i], cpDim);
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
   POETCode* res = INVOKE(appendDecl,0,TUPLE3(IntType,loopIvar,HandleMap::GetTraceDecl(top)));
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
               ASSIGN(bufname,STRING(get_bufName(arrname,scalar))),
               ASSIGN(elem_type,arrelemType), ASSIGN(is_scalar,ICONST(scalar)));
      if (opt & INIT_COPY) 
           cpconfig = SEQ(poet,cpconfig,ASSIGN(init_loc,cploc));
      if (opt & SAVE_COPY) 
           cpconfig = SEQ(poet,cpconfig,ASSIGN(save_loc,cploc));
      if (!scalar && (opt & ALLOC_COPY)) 
           cpconfig = SEQ(poet,cpconfig,ASSIGN(delete_loc,cploc));
      cpconfig=SEQ4(poet,cpconfig,
                    ASSIGN(trace,top), 
                    ASSIGN(trace_decl,HandleMap::GetTraceDecl(top)),
                    ASSIGN(trace_mod,traceMod));
      LocalVar* pvar = HandleMap::FindTracePrivate(top);
      if (pvar != 0)
          cpconfig=SEQ(poet,cpconfig,ASSIGN(trace_vars,pvar));

      POETCode* config_opt = 0;
      if (permute != 0)
          config_opt=ASSIGN(POET_static::copyConfig_permute,permute);
      if (cpblock != 0) {
          POETCode* setblock=ASSIGN(POET_static::copyConfig_cpBlock,cpblock);
          config_opt = (config_opt)? SEQ(poet,config_opt, setblock) : setblock;
      }

      /*QY: set up copy parameters and making the call */
      LocalVar* arrVar = get_arrVar(poet,arrname);
      if (cpDim==0) cpDim=EMPTY;
      cpDim = (config_opt==0)? CODE_REF("CopyConfig",PAIR(arrVar,cpDim))
            : BOP(POET_OP_POND,TUPLE_ACC(POET_static::copyConfig, config_opt),
                               PAIR(arrVar,cpDim)); 
      POETCode* args = PAIR(cpDim,cploc); 
      POETCode* copyinvoke = TRACE_EVAL(traceMod,INVOKE(copyarray, cpconfig,args));
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
     : CopyArraySpec(handle,config)
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
      POETCode* blockPar = BlockSpec::get_blockDimVar(handleName);
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
            cpblockVec[permuteIndex]=CODE_REF("CopyBlock",TUPLE3(STRING(ivarname), STRING(loopVec[minLoop].ivarname+"_bk"), CAR(curblock)));
      }
      POETCode* cpblock=0;
      for (int i = cpblockVec.size() - 1; i >= 0; --i) 
              cpblock = LIST(cpblockVec[i], cpblock);
     return cpblock;
}

void CopyArraySpec:: 
insert_paramDecl(POETProgram& poet, OptLevel optLevel, int& lineNo)
{
     std::string arrname = sel.arr_name();
     std::string prefix = get_opt_prefix(optLevel);
     std::string paramName = prefix + "_config_" + arrname;
     LocalVar* paramVar = PARAM_VAR(paramName);
     std::string optname=(optLevel==OPT_CACHE_LEVEL)?"copy":"scalarRepl";

     paramVar->get_entry().set_code(ICONST(1));
     paramVar->get_entry().set_restr(RANGE_TYPE(ICONST(0),ICONST(2)));
     InsertParamDecl(poet,paramVar,
           "configuration for " + optname + " array " + arrname + " at loop " + handleName + ": 0-no opt; 1-" + optname + "; 2-strength reduction.",
           lineNo++);
}

POETCode* CopyArraySpec::
gen_copyInvoke(POETProgram& poet, POETCode* cphandle, LocalVar* top, 
        POETCode* cpblock, bool scalar, POETCode* traceMod, int& lineNo)
{
  std::string arrname = sel.arr_name();
  if (opt & COPY_INSIDE) cphandle=TUPLE_ACC(cphandle,Nest_body);

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
  POETCode* copyInvoke=gen_copyInvoke(poet,handle,top,0,scalar,traceMod,lineNo);

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
  compute_fdConfig(poet,handle,scalar,expDimVec_cp,expDimVec_nocp);
  LocalVar* parHandle = 0;

  LocalVar* cphandle=handle;
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
     POETCode* fdInvoke = gen_fdInvoke(poet,top,get_fdName(bufname),arrexp,
          CODE_REF("PtrType",elemType), expDimVec_cp, 0, traceMod, false);
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
          gen_fdInvoke(poet,top,get_fdName(bufname),arrexp,
          CODE_REF("PtrType",elemType), expDimVec_nocp, permute,
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
                curdist = BlockSpec::get_blockSize(handleName,minLoop);
             else 
                curdist=STRING(get_cpIvarName(arrname,permuteIndex+1));
             curdist2 = val2poet.apply(cur.get_incr()); 
         } 
         else {
             curdist=curdist2=TUPLE_ACC(dimVar,ICONST(sel_index));
         }
         expDimVec_cp[permuteIndex] = 
                  CODE_REF("ExpDim",TUPLE3(loopVec[minLoop].handle,ICONST(1),curdist));
         expDimVec_nocp[permuteIndex]= CODE_REF("ExpDim",TUPLE3(loopVec[minLoop].handle,ICONST(1),curdist2));
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
          dimsize = dimsize *(new SymbolicPOETWrap(BlockSpec::get_blockSize(handleName,i)));
        cpDim=TUPLE_APPEND(cpDim,val2poet.apply(dimsize));
    }
    return cpDim;
}
