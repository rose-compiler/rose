
#include <iostream>
#include <sstream>

#include "StmtInfoCollect.h"
#include "AnalysisInterface.h"
#include <assert.h>
#include "CommandOptions.h"
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */


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

void StmtInfoCollect ::
AppendFuncCallArguments( AstInterface& fa, const AstNodePtr& fc)
{
  AstInterface::AstNodeList args;
  if (!fa.IsFunctionCall(fc, 0, &args))
      assert(false);

  for (AstInterface::AstNodeList::const_iterator p1 = args.begin();
       p1 != args.end(); ++p1) {
    AstNodePtr c = *p1;
    if ( c == AstNodePtr())
        continue;
    operator()(fa, c);
  }
}

void StmtInfoCollect ::
AppendFuncCallWrite( AstInterface& fa, const AstNodePtr& fc)
{
  AstInterface::AstNodeList args;
  if (!fa.IsFunctionCall(fc, 0, &args))
      assert(false);
  for (AstInterface::AstNodeList::const_iterator p2 = args.begin();
       p2 != args.end(); ++p2) {
    AstNodePtr c = *p2;
    if (c == AstNodePtr())
       continue;
    if (fa.IsMemoryAccess(c))
       AppendModLoc( fa, c, AstNodePtr());
  }
}

void StmtInfoCollect::operator()( AstInterface& fa, const AstNodePtr& h)
{
   AstNodePtr curstmttmp = curstmt;
   ProcessAstTreeBase::operator()(fa, h);
   curstmt = curstmttmp;
}

bool StmtInfoCollect ::
ProcessTree( AstInterface &fa, const AstNodePtr& s,
                       AstInterface::TraversalVisitType t)
{
 if (t == AstInterface::PreVisit) {

   if (fa.IsStatement(s)) {
      if (DebugLocalInfoCollect())
         std::cerr << "previsiting cur statement " << AstInterface::AstToString(s) << "\n";
      curstmt = s;
   }
   else  {
      if (DebugLocalInfoCollect())
         std::cerr << "previsiting cur node " << AstInterface::AstToString(s) << "\n";
      if (curstmt == AST_NULL) {
        AstNodePtr s1 = fa.GetParent(s);
        for ( ; s1 != AST_NULL && !fa.IsStatement(s1); s1 = fa.GetParent(s1));
        if (s1 != AST_NULL)
           curstmt = s1;
        else if (DebugLocalInfoCollect())
            std::cerr << "curstmt = 0\n";

      }
   }

   AstNodePtr lhs, rhs;
   AstInterface::AstNodeList vars, args;
   AstInterface::OperatorEnum opr;
   bool readlhs = false;

   if (fa.IsAssignment(s, &lhs, &rhs, &readlhs)) {
     ModMap *mp = modstack.size()?  &modstack.back().modmap : 0;
     if (mp == 0 || mp->find(lhs) == mp->end()) {
        modstack.push_back(s);
        modstack.back().modmap[lhs] =  ModRecord( rhs,readlhs);
     }
   }
   else if (fa.IsUnaryOp(s, &opr, &lhs) &&
           (opr == AstInterface::UOP_INCR1 || opr == AstInterface::UOP_DECR1)){
     ModMap *mp = modstack.size()?  &modstack.back().modmap : 0;
     if (mp == 0 || mp->find(lhs) == mp->end()) {
        modstack.push_back(s);
        modstack.back().modmap[lhs] =  ModRecord( lhs,true);
     }
   }
   else if (fa.IsVariableDecl( s, &vars, &args)) {
      AstInterface::AstNodeList::const_iterator pv = vars.begin();
      AstInterface::AstNodeList::const_iterator pa = args.begin();
      modstack.push_back(s);
      while (pv != vars.end()) {
         modstack.back().modmap[*pv] = ModRecord(*pa,false);
         ++pv;
         ++pa;
      }
   }
   else  if (fa.IsIOInputStmt(s, &args)) {
     modstack.push_back(s);
     for (AstInterface::AstNodeList::reverse_iterator p = args.rbegin();
           p != args.rend(); ++p) {
        AstNodePtr c = *p;
        modstack.back().modmap[c]= ModRecord(AST_NULL,false);
     }
   }
   else {
     if (fa.IsFunctionCall(s)) {
         if (DebugLocalInfoCollect())
             std::cerr << " append function call " << AstInterface::AstToString(s) << std::endl;
         AppendFuncCall(fa, s);
         Skip(s);
     }
     if ( fa.IsMemoryAccess(s)) {
        if (!fa.IsSameVarRef(s, fa.GetParent(s))) { /*QY: skip s if it refers to the same thing as parent*/
          ModMap *mp = modstack.size()?  &modstack.back().modmap : 0;
          if (mp == 0 || mp->find(s) == mp->end() || (*mp)[s].readlhs)
             AppendReadLoc(fa, s);
        }
        AstNodeList arglist;
        if (fa.IsArrayAccess(s, 0, &arglist))  {
           for (AstNodeList::const_iterator p = arglist.begin();
                p != arglist.end();  ++p) {
               AstNodePtr c = *p;
               operator()(fa, c);
           }
           Skip(s);
        }
      }
   }
 }
 else {
      if (DebugLocalInfoCollect())
         std::cerr << "postvisiting cur node " << AstInterface::AstToString(s) << "\n";
       if (modstack.size() && modstack.back().root == s) {
          const ModMap &modmap = modstack.back().modmap;
          for ( ModMap::const_iterator p = modmap.begin();
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

class CollectReadRefWrap : public CollectObject<AstNodePtr>
{
  CollectObject< std::pair< AstNodePtr, AstNodePtr> >* collect;
  FunctionSideEffectInterface* func;
 protected:
  AstNodePtr stmt;
  AstInterface& fa;
 public:
  CollectReadRefWrap( AstInterface& _fa, FunctionSideEffectInterface* f,
                      const AstNodePtr& _stmt,
                      CollectObject< std::pair< AstNodePtr, AstNodePtr> >* c)
    : collect(c), func(f), stmt(_stmt), fa(_fa) {}
  bool operator() ( const AstNodePtr& ref)
   {
      AstNodeList args;
      if (fa.IsFunctionCall(ref))
         func->get_read(fa, ref, this);
      else if (fa.IsArrayAccess(ref, 0, &args)) {
        for (AstNodeList::const_iterator p = args.begin();
             p != args.end(); ++p) {
               AstNodePtr c = *p;
               operator()(c);
        }
      }
      if (fa.IsMemoryAccess(ref)) {
        if (DebugLocalInfoCollect())
           std::cerr << "appending reading " << AstInterface::AstToString(ref) << " : " << AstInterface::AstToString(stmt) << std::endl;
        if (collect != 0)
          (*collect)( std::pair<AstNodePtr, AstNodePtr>(ref, stmt));
      }
      return true;
   }
};

class CollectModRefWrap : public CollectReadRefWrap
{
  CollectObject< std::pair< AstNodePtr, AstNodePtr> >* mod;
 public:
  CollectModRefWrap( AstInterface& _fa, FunctionSideEffectInterface* f,
                     const AstNodePtr& _stmt,
                      CollectObject< std::pair<AstNodePtr,AstNodePtr> >* read,
                      CollectObject< std::pair<AstNodePtr,AstNodePtr> >* m)
    : CollectReadRefWrap(_fa, f, _stmt, read), mod(m) {}
  bool operator() ( const AstNodePtr& ref)
   {
      AstNodeList args;
      if (fa.IsFunctionCall(ref, 0,&args) || fa.IsArrayAccess(ref, 0, &args)) {
        CollectReadRefWrap read(*this);
        for (AstInterface::AstNodeList::const_iterator p = args.begin();
             p != args.end(); ++p) {
               AstNodePtr c = *p;
               read(c);
        }
      }
      if (DebugLocalInfoCollect())
          std::cerr << "appending modifying " << AstInterface::AstToString(ref) << " : " << AstInterface::AstToString(stmt) << std::endl;
      if (mod != 0)
        (*mod)( std::pair<AstNodePtr, AstNodePtr>(ref, stmt));
      return true;
   }
};

void StmtSideEffectCollect::
AppendModLoc( AstInterface& fa, const AstNodePtr& mod, const AstNodePtr& rhs)
    {
      if (DebugLocalInfoCollect()) {
          std::cerr << "appending modifying " << AstInterface::AstToString(mod) << " = " << AstInterface::AstToString(rhs) << std::endl;
      }
if(curstmt == 0) return;
       assert(curstmt != AST_NULL);
       if (killcollect != 0 && rhs != AST_NULL)
            (*killcollect)( std::pair<AstNodePtr,AstNodePtr>(mod, curstmt));
       if (modcollect != 0)
            (*modcollect)( std::pair<AstNodePtr,AstNodePtr>(mod, curstmt));
    }
void StmtSideEffectCollect::
AppendReadLoc( AstInterface& fa, const AstNodePtr& read)
    {
      if (DebugLocalInfoCollect()) {
          std::cerr << "appending reading " << AstInterface::AstToString(read) << std::endl;
      }
       if (readcollect != 0)
               (*readcollect)(std::pair<AstNodePtr,AstNodePtr>(read, curstmt));
    }

void StmtSideEffectCollect::
AppendFuncCall( AstInterface& fa, const AstNodePtr& fc)
{
 CollectReadRefWrap read(fa, funcanal, curstmt, readcollect);
 if (funcanal == 0 || !funcanal->get_read(fa, fc, &read))  {
      readunknown = true;
      if (DebugLocalInfoCollect())
         std::cerr << "no interprecedural read info for : " << AstInterface::AstToString(fc) << std::endl;
      AppendFuncCallArguments(fa, fc);
  }

  CollectModRefWrap mod(fa, funcanal, curstmt, readcollect, modcollect);
  if (funcanal == 0 || !funcanal->get_modify( fa, fc, &mod))  {
      if (DebugLocalInfoCollect())
         std::cerr << "no interprecedural mod info for : " << AstInterface::AstToString(fc) << std::endl;
       AppendFuncCallWrite(fa, fc);
       modunknown = true;
  }
}

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
    if (cur.first == AST_NULL || !fa.IsVarRef(cur.first, 0, &varname, &scope))
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
     assert(false);
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
         assert(false);
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
    assert(false);
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
     assert(false);
  }

  ModifyAliasMap collect(fa, aliasmap);
  if (funcanal == 0 || !funcanal->allow_alias( fa, funcdef, collect)) {
     for (AstInterface::AstNodeList::const_iterator p = params.begin();
          p != params.end(); ++p) {
         collect( std::pair<AstNodePtr,int>(*p, 0));
     }
  }
  StmtInfoCollect::operator()(fa, body);
}
