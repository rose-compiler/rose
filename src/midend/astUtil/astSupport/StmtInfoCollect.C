#include <sage3basic.h>

#include <iostream>
#include <sstream>

#include "StmtInfoCollect.h"
#include "AnalysisInterface.h"
#include "AstInterface_ROSE.h"
#include <assert.h>
#include "CommandOptions.h"
#include <ROSE_ABORT.h>
#include <ROSE_ASSERT.h>

#ifndef TEMPLATE_ONLY
bool DebugLocalInfoCollect ()
{
  static int r = 0;
  if (r == 0) {
     if (CmdOptions::GetInstance()->HasOption("-debuglocalinfocollect"))
         r = 1;
     else
         r = -1;
  }
  return r == 1;
}

bool DebugAliasAnal ()
{
  static int r = 0;
  if (r == 0) {
     if (CmdOptions::GetInstance()->HasOption("-debugaliasanal"))
         r = 1;
     else
         r = -1;
  }
  return r == 1;
}
#else
extern bool DebugLocalInfoCollect();
extern bool DebugAliasAnal ();
#endif

#ifdef TEMPLATE_ONLY
template <class AstNodePtr>
void StmtInfoCollect<AstNodePtr>::
AppendFuncCallArguments( AstInterface& fa, const AstNodePtr& fc, AstNodePtr* callee)
{
  AstInterface::AstNodeList args;
  AstInterface::AstNodePtr p_callee;
  if (!fa.IsFunctionCall(fc, &p_callee, &args))
      ROSE_ABORT();
  if (callee != 0) {
    *callee = AstNodePtrImpl(p_callee).get_ptr();
  }

  for (AstInterface::AstNodeList::const_iterator p1 = args.begin();
       p1 != args.end(); ++p1) {
    AstNodePtr c = AstNodePtrImpl(*p1).get_ptr();
    if ( c == AstNodePtr())
        continue;
    if (DebugLocalInfoCollect()) {
      std::cerr << "Function Argument: " << AstInterface::AstToString(c) << "\n";
    }
    operator()(fa, c);
  }
}

template <class AstNodePtr>
void StmtInfoCollect<AstNodePtr>::
AppendFuncCallWrite( AstInterface& fa, const AstNodePtr& fc)
{
  AstInterface::AstNodeList args;
  if (!fa.IsFunctionCall(fc, 0, &args))
      ROSE_ABORT();
  for (AstInterface::AstNodeList::const_iterator p2 = args.begin();
       p2 != args.end(); ++p2) {
    AstNodePtr c = AstNodePtrImpl(*p2).get_ptr();
    if (c == AstNodePtr())
       continue;
    if (fa.IsMemoryAccess(c)) {
       AppendModLoc( fa, c, AstNodePtr());
    }
  }
}

template <class AstNodePtr>
void StmtInfoCollect<AstNodePtr>::
operator()( AstInterface& fa, const AstNodePtr& h)
{
   AstNodePtr curstmttmp = curstmt;
   ProcessAstTreeBase<AstInterface::AstNodePtr>::operator()(fa, h);
   curstmt = curstmttmp;
}

template <class AstNodePtr>
bool StmtInfoCollect<AstNodePtr> ::
ProcessTree( AstInterface &fa, const AstInterface::AstNodePtr& s,
                       AstInterface::TraversalVisitType t)
{
 if (t == AstInterface::PreVisit) {
   AstInterface::AstNodePtr lhs, rhs, body;
   AstInterface::AstNodeList vars, args;
   AstInterface::OperatorEnum opr;
   bool readlhs = false;

   AstNodePtr s_ptr = AstNodePtrImpl(s).get_ptr();
   if (fa.IsFunctionDefinition(s, 0, &vars, &args, &body)) {
      curstmt = AstNodePtrImpl(s).get_ptr();
      // Below is necessary for constructors that initialize member variables.
      for (AstInterface::AstNodePtr param : args) {
         if (fa.IsAssignment(param, &lhs, &rhs, &readlhs)) {
            if (rhs != AST_NULL) {
               operator()(fa, AstNodePtrImpl(rhs).get_ptr());
            }
            AppendModLoc(fa, AstNodePtrImpl(lhs).get_ptr(), AstNodePtrImpl(rhs).get_ptr());
         } 
      }
      SkipUntil(body);
      return true;
   }
   else if (fa.IsStatement(s)) {
      if (DebugLocalInfoCollect())
         std::cerr << "previsiting cur statement " << AstInterface::AstToString(s) << "\n";
      curstmt = AstNodePtrImpl(s).get_ptr();
   }
   else  {
      if (DebugLocalInfoCollect())
         std::cerr << "previsiting cur node " << AstInterface::AstToString(s) << "\n";
      if (curstmt == AST_NULL) {
        AstInterface::AstNodePtr s1 = fa.GetParent(s);
        for ( ; s1 != AST_NULL && !fa.IsStatement(s1); s1 = fa.GetParent(s1));
        if (s1 != AST_NULL)
           curstmt = AstNodePtrImpl(s1).get_ptr();
        else if (DebugLocalInfoCollect())
            std::cerr << "curstmt = 0\n";
      }
   }

   if (fa.IsAssignment(s, &lhs, &rhs, &readlhs)) {
       if (DebugLocalInfoCollect())
          std::cerr << "Is assignment.\n";
       ModMap *mp = modstack.size()?  &modstack.back().modmap : 0;
       if (mp == 0 || mp->find(AstNodePtrImpl(lhs).get_ptr()) == mp->end()) {
         modstack.push_back(s_ptr);
         modstack.back().modmap[AstNodePtrImpl(lhs).get_ptr()] =  ModRecord(AstNodePtrImpl(rhs).get_ptr(),readlhs);
       }
   }
   else if (fa.IsUnaryOp(s, &opr, &lhs) &&
           (opr == AstInterface::UOP_INCR1 || opr == AstInterface::UOP_DECR1)){
      if (DebugLocalInfoCollect())
        std::cerr << "Is unary operator.\n";
      ModMap *mp = modstack.size()?  &modstack.back().modmap : 0;
      if (mp == 0 || mp->find(AstNodePtrImpl(lhs).get_ptr()) == mp->end()) {
         modstack.push_back(s_ptr);
         modstack.back().modmap[AstNodePtrImpl(lhs).get_ptr()] =  ModRecord(AstNodePtrImpl(lhs).get_ptr(),true);
      }
   }
   else if (fa.IsVariableDecl( s, &vars, &args)) {
      if (DebugLocalInfoCollect())
        std::cerr << "Is variable declaration.\n";
      AstInterface::AstNodeList::const_iterator pv = vars.begin();
      AstInterface::AstNodeList::const_iterator pa = args.begin();
      modstack.push_back(s_ptr);
      while (pv != vars.end()) {
         AstNodePtr ast = AstNodePtrImpl(*pv).get_ptr();
         AstNodePtr read_ast = AstNodePtrImpl(*pa).get_ptr();
         if (read_ast != AST_NULL) {
            operator()(fa, read_ast);
         }
        AppendModLoc( fa, ast, read_ast);
         ++pv;
         ++pa;
      }
      Skip(s);
   }
   else  if (fa.IsIOInputStmt(s, &args)) {
     if (DebugLocalInfoCollect())
        std::cerr << "Is IOInput stmt.\n";
     modstack.push_back(s_ptr);
     for (AstInterface::AstNodeList::reverse_iterator p = args.rbegin();
           p != args.rend(); ++p) {
        AstNodePtr c = AstNodePtrImpl(*p).get_ptr();
        modstack.back().modmap[c]= ModRecord(AST_NULL,false);
     }
   }
   else {
     if (fa.IsFunctionCall(s)) {
         if (DebugLocalInfoCollect())
             std::cerr << " append function call " << AstInterface::AstToString(s) << std::endl;
         AppendFuncCall(fa, AstNodePtrImpl(s).get_ptr());
         Skip(s);
     } 
     // Jim Leek 2023/02/07  Added IsSgAddressOfOp because I want it
     // to behave the same as a memory access, although it isn't one exactly
     else if ( fa.IsMemoryAccess(s) || fa.IsAddressOfOp(s)) {
        if (DebugLocalInfoCollect())
             std::cerr << " append read set " << AstInterface::AstToString(s) << std::endl;
        if (!fa.IsSameVarRef(s, fa.GetParent(s))) { /*QY: skip s if it refers to the same thing as parent*/
          ModMap *mp = modstack.size()?  &modstack.back().modmap : 0;
          if (mp == 0 || mp->find(s_ptr) == mp->end() || (*mp)[s_ptr].readlhs) {
              AppendReadLoc(fa, s_ptr);
          }
        }
        AstNodeList arglist;
        if (fa.IsArrayAccess(s, 0, &arglist))  {
           for (AstNodeList::const_iterator p = arglist.begin();
                p != arglist.end();  ++p) {
               AstNodePtr c = AstNodePtrImpl(*p).get_ptr();
               operator()(fa, c);
           }
        }
        Skip(s);
      }
   }
 }
 else {
      if (DebugLocalInfoCollect())
         std::cerr << "postvisiting cur node " << AstInterface::AstToString(s) << "\n";
       if (modstack.size() && modstack.back().root == AstNodePtrImpl(s).get_ptr()) {
          const ModMap &modmap = modstack.back().modmap;
          for (typename ModMap::const_iterator p = modmap.begin();
               p != modmap.end(); ++p) {
              std::pair<const AstNodePtr,ModRecord> c = *p;
             AppendModLoc( fa, c.first, c.second.rhs);
          }
          modstack.pop_back();
       }
     if (s == curstmt) {
        curstmt = AST_NULL;
     }
 }
 return true;
}

template <class AstNodePtr>
class CollectReadRefWrap : public CollectObject<AstInterface::AstNodePtr>
{
  typedef typename SideEffectAnalysisInterface<AstNodePtr>::CollectObject CollectObject; 
  CollectObject* collect;
  FunctionSideEffectInterface* func;
 protected:
  AstNodePtr stmt;
  AstInterface& fa;
 public:
  CollectReadRefWrap( AstInterface& _fa, FunctionSideEffectInterface* f,
                      const AstNodePtr& _stmt,
                      CollectObject* c)
    : collect(c), func(f), stmt(_stmt), fa(_fa) {}
  bool operator() ( const AstInterface::AstNodePtr& ref)
   {
      AstInterface::AstNodeList args;
      AstInterface::AstNodePtr callee;
      if (fa.IsArrayAccess(ref, 0, &args)) {
        for (AstNodeList::const_iterator p = args.begin();
             p != args.end(); ++p) {
               operator()(*p);
        }
        if (DebugLocalInfoCollect())
           std::cerr << "appending reading " << AstInterface::AstToString(ref) << " : " << AstInterface::AstToString(stmt) << std::endl;
        if (collect != 0)
          (*collect)( AstNodePtrImpl(ref).get_ptr(), stmt);
      }
      else if (fa.IsMemoryAccess(ref)) {
        if (DebugLocalInfoCollect())
           std::cerr << "appending reading " << AstInterface::AstToString(ref) << " : " << AstInterface::AstToString(stmt) << std::endl;
        if (collect != 0)
          (*collect)( AstNodePtrImpl(ref).get_ptr(), stmt);
      } else {
        std::cerr << "Expecting a memory reference but getting:" << AstInterface::AstToString(ref) << "\n";
        ROSE_ABORT();
      }
      return true;
   }
};

template <class AstNodePtr>
class CollectCallRefWrap : public CollectObject<AstInterface::AstNodePtr>
{
  typedef typename SideEffectAnalysisInterface<AstNodePtr>::CollectObject CollectObject; 
  CollectObject* collect;
  FunctionSideEffectInterface* func;
 protected:
  AstNodePtr stmt;
  AstInterface& fa;
 public:
  CollectCallRefWrap( AstInterface& _fa, FunctionSideEffectInterface* f,
                      const AstNodePtr& _stmt,
                      CollectObject* c)
    : collect(c), func(f), stmt(_stmt), fa(_fa) {}
  bool operator() ( const AstInterface::AstNodePtr& ref)
   {
      AstInterface::AstNodeList args;
      AstInterface::AstNodePtr callee;
      if (fa.IsFunctionCall(ref, &callee)) {
         (*collect)(AstNodePtrImpl(callee).get_ptr(), stmt);
      } else {
        std::cerr << "Expecting a function call but getting:" << AstInterface::AstToString(ref) << "\n";
        ROSE_ABORT();
      }
      return true;
   }
};


template <class AstNodePtr>
class CollectModRefWrap : public CollectReadRefWrap<AstNodePtr>
{
  typedef typename SideEffectAnalysisInterface<AstNodePtr>::CollectObject CollectObject; 

  CollectObject* mod;
  using CollectReadRefWrap<AstNodePtr>::fa;
  using CollectReadRefWrap<AstNodePtr>::stmt;
 public:
  CollectModRefWrap( AstInterface& _fa, FunctionSideEffectInterface* f,
                     const AstNodePtr& _stmt,
                      CollectObject* read,
                      CollectObject* m)
    : CollectReadRefWrap<AstNodePtr>(_fa, f, _stmt, read), mod(m) {}
  bool operator() ( const AstInterface::AstNodePtr& ref)
   {
      AstNodeList args;
      if (fa.IsFunctionCall(ref, 0,&args) || fa.IsArrayAccess(ref, 0, &args)) {
        CollectReadRefWrap<AstNodePtr> read(*this);
        for (AstInterface::AstNodeList::const_iterator p = args.begin();
             p != args.end(); ++p) {
               read(*p);
        }
      }
      if (mod != 0)
        (*mod)(AstNodePtrImpl(ref).get_ptr(), stmt);
      return true;
   }
};

template <class AstNodePtr>
void StmtSideEffectCollect<AstNodePtr>::
AppendModLoc(AstInterface&, const AstNodePtr& mod, const AstNodePtr& rhs)
    {
      if (DebugLocalInfoCollect()) {
          std::cerr << "appending modifying " << AstInterface::AstToString(mod) << " = " << AstInterface::AstToString(rhs) << std::endl;
      }
     if(curstmt == 0) return;
       assert(curstmt != AST_NULL);
       if (killcollect != 0 && rhs != AST_NULL)
            (*killcollect)(mod, curstmt);
       if (modcollect != 0)
            (*modcollect)(mod, curstmt);
    }
template <class AstNodePtr>
void StmtSideEffectCollect<AstNodePtr>::
AppendReadLoc( AstInterface& fa, const AstNodePtr& read)
    {
      if (DebugLocalInfoCollect()) {
          std::cerr << "appending reading " << AstInterface::AstToString(read) << std::endl;
      }
      if (readcollect != 0) {          
               (*readcollect)(read, curstmt);
      }
      
    }

template <class AstNodePtr>
void StmtSideEffectCollect<AstNodePtr>::
AppendFuncCall( AstInterface& fa, const AstNodePtr& fc)
{
 CollectReadRefWrap<AstNodePtr> read(fa, funcanal, curstmt, readcollect);
 if (DebugLocalInfoCollect()) {
    std::cerr << "Checking function info: " << AstInterface::AstToString(fc) << std::endl;
 }
 if (funcanal == 0 || !funcanal->get_read(fa, fc, &read))  {
      readunknown = true;
      AstNodePtr callee;
      if (DebugLocalInfoCollect()) {
         std::cerr << "no interprecedural read info for : " << AstInterface::AstToString(fc) << std::endl;
         std::cerr << "adding function call arguments.\n";
      }
      AppendFuncCallArguments(fa, fc, &callee);
      if (callcollect != 0) {
         if (DebugLocalInfoCollect()) {
             std::cerr << "invoking collecting call\n";
         }
        (*callcollect)(callee, curstmt);
      }
  } else if (callcollect != 0) {
    assert(funcanal != 0);
    if (DebugLocalInfoCollect()) {
      std::cerr << "Has existing call analysis from annotation\n";
    }
    CollectCallRefWrap<AstNodePtr> call(fa, funcanal, curstmt, callcollect);
    if (!funcanal->get_call(fa, fc, &call)) {
       std::cerr << "Error: Expecting annotation to provide call information but getting none.";
       ROSE_ABORT();
    }
  }

  CollectModRefWrap<AstNodePtr> mod(fa, funcanal, curstmt, readcollect, modcollect);
  if (funcanal == 0 || !funcanal->get_modify( fa, fc, &mod))  {
      if (DebugLocalInfoCollect())
         std::cerr << "no interprecedural mod info for : " << AstInterface::AstToString(fc) << std::endl;
       AppendFuncCallWrite(fa, fc);
       modunknown = true;
  }
}
#else
class ModifyAliasMap : public CollectObject< std::pair<AstNodePtr, int> >
{
  AstInterface& fa;
  StmtVarAliasCollect::VarAliasMap& aliasmap;
  UF_elem *repr;
  int index;
public:
  ModifyAliasMap(AstInterface& _fa, StmtVarAliasCollect::VarAliasMap& m)
    : fa(_fa), aliasmap(m), repr(0), index(-1) {}

  void reset()
  {
    index = -1;
    repr = 0;
  }

  bool operator() ( const std::pair<AstNodePtr, int>& cur)
  {
    std::string varname;
    AstNodePtr scope;
    if (cur.first == AST_NULL || !fa.IsVarRef(cur.first, 0, &varname, &scope, 0, /*use_global_unique_name=*/true))
      return false;
    if (cur.second == index) {
      aliasmap.get_alias_map(varname, scope)->union_with(repr);
      if (DebugAliasAnal())  {
         std::cerr << "aliasing with: " << varname << std::endl;
      }
    }
    else {
      assert( cur.second > index);
      repr = aliasmap.get_alias_map(varname, scope);
      if (DebugAliasAnal())
         std::cerr << "cur alias-var repr:" << varname << std::endl;
      index = cur.second;
    }
    return true;
  }
};

Ast2StringMap* Ast2StringMap::handle = 0;

Ast2StringMap* Ast2StringMap::inst()
{
  if (handle == 0)
    handle = new Ast2StringMap();
  return handle;
}

std::string Ast2StringMap :: get_string( const AstNodePtr& s)
{
  if ( s == AST_NULL) return "";
  MapType::const_iterator p = astmap.find(s);
  if (p != astmap.end())
    return (*p).second;
  std::stringstream buf;
  ++cur;
  buf <<  cur;
  std::string r= buf.str();
  astmap[s] = r;
  return r;
}

std::string Ast2StringMap :: lookup_string( const AstNodePtr& s) const
{
  if ( s == AST_NULL) return "";
  MapType::const_iterator p = astmap.find(s);
  if (p == astmap.end())  return "";
  return (*p).second;
}

std::string Ast2StringMap :: get_string( const AstNodePtr& s) const
{
  if ( s == AST_NULL) return "";
  MapType::const_iterator p = astmap.find(s);
  if (p == astmap.end()) {
     std::cerr << "Error: cannot find ast: " << AstInterface::AstToString(s) << std::endl;
     for (p = astmap.begin(); p != astmap.end(); ++p) {
          std::cerr << " : " << AstInterface::AstToString((*p).first) << ":" << (*p).second << std::endl;
     }
     ROSE_ABORT();
  }
  return (*p).second;
}

std:: string InterProcVariableUniqueRepr::
get_unique_name(AstInterface& fa, const AstNodePtr& exp)
{
  std::string expname = Ast2StringMap::inst()->get_string(exp);
  return get_unique_name(fa, exp, expname);
}

std:: string InterProcVariableUniqueRepr::
get_unique_name(AstInterface& fa, const AstNodePtr& _scope, const std::string& varname)
    {
      if (_scope == AST_NULL) return varname;
      std::string fname;
      AstNodePtr func = fa.GetFunctionDefinition(_scope, &fname);
     if (func == AST_NULL) return varname;
     return get_unique_name(fname, func, varname);
 }


UF_elem*  StmtVarAliasCollect::VarAliasMap::
get_alias_map( const std::string& varname, const AstNodePtr& scope)
{
  std::string scopename = Ast2StringMap::inst()->get_string(scope);
  std::string name = varname + scopename;
  if (aliasmap.find(name) == aliasmap.end())
     aliasmap[name] = new UF_elem();
  return aliasmap[name];
}

void StmtVarAliasCollect::
AppendModLoc( AstInterface& fa, const AstNodePtr& mod,
                              const AstNodePtr& rhs)
{
  std::string modname;
  AstNodeType modtype;
  AstNodePtr modscope;
  if (rhs == AST_NULL || !fa.IsVarRef(mod, &modtype, &modname, &modscope) || fa.IsScalarType(modtype))
    return;
  AstInterface::AstNodeList args;
  if (fa.IsFunctionCall( rhs, 0, &args) ) {
    ModifyAliasMap collect(fa, aliasmap);
    if (funcanal != 0 && funcanal->may_alias( fa, rhs, mod, collect))
        return;
    hasunknown = true;
    if (DebugAliasAnal()) {
        std::cerr << "unknown alias info for function call : " << AstInterface::AstToString(rhs) << std::endl;
        std::cerr << "aliasing all parameters with " << AstInterface::AstToString(mod) << std::endl;;
    }
    collect.reset();
    collect( std::pair<AstNodePtr,int>(mod, 0));
    for (AstInterface::AstNodeList::const_iterator p = args.begin();
          p != args.end() ; ++p) {
       collect( std::pair<AstNodePtr,int>(*p, 0));
    }
  }
  else {
    std::string rhsname;
    AstNodeType rhstype;
    AstNodePtr rhsscope;
    if (fa.IsVarRef(rhs, &rhstype, &rhsname, &rhsscope)) {
      if (!fa.IsScalarType(rhstype))
         aliasmap.get_alias_map(modname, modscope)->union_with(aliasmap.get_alias_map(rhsname, rhsscope));
    }
  }
}

void StmtVarAliasCollect::
AppendFuncCall( AstInterface& fa, const AstNodePtr& fc)
{
  ModifyAliasMap collect(fa, aliasmap);
  if (funcanal == 0 || !funcanal->may_alias( fa, fc, AST_NULL, collect)) {
     hasunknown = true;
    if (DebugAliasAnal()) {
        std::cerr << "unknown alias info. for function call : " << AstInterface::AstToString(fc) << std::endl;
        std::cerr << "aliasing all parameters \n";
     }
     collect.reset();
     AstInterface::AstNodeList args;
     if (!fa.IsFunctionCall( fc, 0, &args))
         ROSE_ABORT();
     for (AstInterface::AstNodeList::const_iterator p = args.begin();
           p != args.end(); ++p) {
         collect( std::pair<AstNodePtr,int>(*p, 0));
     }
  }
}

bool StmtVarAliasCollect::
may_alias(AstInterface& fa, const AstNodePtr& r1,
          const AstNodePtr& r2)
{
  AstNodeType t1, t2;
  if (fa.IsExpression(r1, &t1) == AST_NULL || fa.IsExpression(r2, &t2)==AST_NULL)
    ROSE_ABORT();
  if (!hasresult) {
    if (DebugAliasAnal()) {
       std::cerr << "no alias analysis performed \n";
       std::cerr << "has alias between " << AstInterface::AstToString(r1) << " and " << AstInterface::AstToString(r2) << std::endl;
    }
    return true;
  }
  std::string varname1, varname2;
  AstNodePtr scope1, scope2;
  bool global1, global2;
  if (!fa.IsVarRef(r1, 0, &varname1, &scope1, &global1)
      || !fa.IsVarRef(r2, 0, &varname2, &scope2, &global2)) {
    if (DebugAliasAnal()) {
       std::cerr << "no alias analysis between non_variables performed \n";
       std::cerr << "has alias between " << AstInterface::AstToString(r1) << " and " << AstInterface::AstToString(r2) << std::endl;
    }
    return true;
  }

  if (global1 && global2 && hasunknown) {
    if (DebugAliasAnal()) {
       if (global1) {
          std::cerr << varname1 << " and " << varname2 << " are global \n";
          std::cerr << "has alias between " << AstInterface::AstToString(r1) << " and " << AstInterface::AstToString(r2) << std::endl;
       }
    }
    return true;
  }
  if ( aliasmap.get_alias_map(varname1, scope1)->in_same_group( aliasmap.get_alias_map(varname2, scope2))) {
    if (DebugAliasAnal()) {
       std::cerr << "alias analysis performed \n";
       std::cerr << "has alias between " << AstInterface::AstToString(r1) << " and " << AstInterface::AstToString(r2) << std::endl;
     }
     return true;
  }
  return false;
}

void StmtVarAliasCollect::
operator()( AstInterface& fa, const AstNodePtr& funcdef)
{ analyze(fa, funcdef);
  return; }


void StmtVarAliasCollect::
analyze( AstInterface& fa, const AstNodePtr& funcdef)
{
  assert(!hasresult);
  hasresult = true;
  AstNodePtr  body;
  AstInterface::AstNodeList params;
  if (!fa.IsFunctionDefinition(funcdef, 0, &params, 0, &body)) {
     std::cerr << "Error: alias analysis requires function definition as input instead of " << AstInterface::AstToString(funcdef) << std::endl;
     ROSE_ABORT();
  }

  ModifyAliasMap collect(fa, aliasmap);
  if (funcanal == 0 || !funcanal->allow_alias( fa, funcdef, collect)) {
     for (AstInterface::AstNodeList::const_iterator p = params.begin();
          p != params.end(); ++p) {
         collect( std::pair<AstNodePtr,int>(*p, 0));
     }
  }
  StmtInfoCollect<AstNodePtr>::operator()(fa, body);
}
#endif
