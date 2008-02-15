
#include <general.h>

#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>

#include <StmtInfoCollect.h>
#include <AstInterface.h>
#include <AnalysisInterface.h>
#include <assert.h>
#include <CommandOptions.h>

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// DQ (3/8/2006): Since this is not used in a heade file it is OK here!
#define Boolean int

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
  for (AstInterface::AstNodeListIterator p1 = fa.GetAstNodeListIterator(args);
       !p1.ReachEnd(); ++p1) {
    AstNodePtr cur = *p1;
    if ( cur == 0)
        continue;
    operator()(fa, cur); 
  }
}
void StmtInfoCollect ::
AppendFuncCallWrite( AstInterface& fa, const AstNodePtr& fc)
{
  AstInterface::AstNodeList args;
  if (!fa.IsFunctionCall(fc, 0, &args))
      assert(false);
  for (AstInterface::AstNodeListIterator p2 = fa.GetAstNodeListIterator(args);
       !p2.ReachEnd(); ++p2) {
    AstNodePtr cur = *p2;
    if (cur == 0)
       continue;
    if (fa.IsMemoryAccess(cur))
       AppendModLoc( fa, cur, 0);
  }
}

void StmtInfoCollect::operator()( AstInterface& fa, const AstNodePtr& h)
{ 
   AstNodePtr curstmttmp = curstmt;
   ProcessAstTreeBase::operator()(fa, h); 
   curstmt = curstmttmp;
}

Boolean StmtInfoCollect :: 
ProcessTree( AstInterface &fa, const AstNodePtr& s, 
                       AstInterface::TraversalVisitType t) 
{
 if (t == AstInterface::PreVisit) {

   if (fa.IsStatement(s)) {
      if (DebugLocalInfoCollect())
         cerr << "previsiting cur statement " << fa.AstToString(s) << "\n";
      curstmt = s;
   }
   else  {
      if (DebugLocalInfoCollect()) 
         cerr << "previsiting cur node " << fa.AstToString(s) << "\n";
      if (curstmt == 0) {
        AstNodePtr s1 = fa.GetParent(s); 
        for ( ; s1 != 0 && !fa.IsStatement(s1); s1 = fa.GetParent(s1)) {}
        curstmt = s1;
      } 
   }

   AstNodePtr lhs, rhs;
   AstInterface::AstNodeList vars, args;
   bool readlhs = false;

   if (fa.IsAssignment(s, &lhs, &rhs, &readlhs)) {
     ModMap *mp = modstack.size()?  &modstack.back().modmap : 0;
     if (mp == 0 || mp->find(lhs) == mp->end()) {
        modstack.push_back(s);
        modstack.back().modmap[lhs] =  ModRecord( rhs,readlhs); 
     }
   }
   else if (fa.IsVariableDecl( s, &vars, &args)) {
      AstInterface::AstNodeListIterator pv = fa.GetAstNodeListIterator(vars);
      AstInterface::AstNodeListIterator pa = fa.GetAstNodeListIterator(args);
      modstack.push_back(s);
      while (!pv.ReachEnd()) {
         modstack.back().modmap[*pv] = ModRecord(*pa,false);
         ++pv;
         ++pa;
      }
   }
   else  if (fa.IsIOInputStmt(s, &args)) {
     fa.ListReverse(args);
     modstack.push_back(s);
     for (AstInterface::AstNodeListIterator p = fa.GetAstNodeListIterator(args);
           !p.ReachEnd(); ++p) {
        AstNodePtr cur = *p;
        modstack.back().modmap[cur]= ModRecord(0,false);
     }
   }
   else {
     if (fa.IsFunctionCall(s)) {
         if (DebugLocalInfoCollect()) 
             cerr << " append function call " << fa.AstToString(s) << endl;
         AppendFuncCall(fa, s);
         Skip(s);
     }
     if ( fa.IsMemoryAccess(s)) {
        ModMap *mp = modstack.size()?  &modstack.back().modmap : 0;
        if (mp == 0 || mp->find(s) == mp->end() || (*mp)[s].readlhs)
           AppendReadLoc(fa, s);
        if (fa.IsArrayAccess(s, 0, &args))  {
           for (AstInterface::AstNodeListIterator p = 
                 fa.GetAstNodeListIterator(args); !p.ReachEnd(); ++p) {
               AstNodePtr cur = *p;
               operator()(fa, cur);
           }
           Skip(s);
        }
      }
   }   
 }
 else {
      if (DebugLocalInfoCollect()) 
         cerr << "postvisiting cur node " << fa.AstToString(s) << "\n";
       if (modstack.size() && modstack.back().root == s) {
          const ModMap &modmap = modstack.back().modmap;
          for ( ModMap::const_iterator p = modmap.begin();
               p != modmap.end(); ++p) { 
              pair<AstNodePtr,ModRecord> cur = *p;
             AppendModLoc( fa, cur.first, cur.second.rhs);
          }
          modstack.pop_back();
       }
     if (s == curstmt) {
        curstmt = 0;
     }
 }
 return true;
}

class CollectReadRefWrap : public CollectObject<AstNodePtr>
{
  CollectObject< pair< AstNodePtr, AstNodePtr> >* collect;
  FunctionSideEffectInterface* func;
 protected:
  AstNodePtr stmt;
  AstInterface& fa;
 public:
  CollectReadRefWrap( AstInterface& _fa, FunctionSideEffectInterface* f, 
                      const AstNodePtr& _stmt, 
                      CollectObject< pair< AstNodePtr, AstNodePtr> >* c)
    : collect(c), func(f), stmt(_stmt), fa(_fa) {}
  Boolean operator() ( const AstNodePtr& ref)
   {
      AstInterface::AstNodeList args;
      if (fa.IsFunctionCall(ref))
         func->get_read(fa, ref, this); 
      else if (fa.IsArrayAccess(ref, 0, &args)) {
        for (AstInterface::AstNodeListIterator p = 
               fa.GetAstNodeListIterator(args); !p.ReachEnd(); ++p) {
               AstNodePtr cur = *p;
               operator()(cur);
        }
      }
      if (fa.IsMemoryAccess(ref)) {
        if (DebugLocalInfoCollect()) 
           cerr << "appending reading " << fa.AstToString(ref) << " : " << fa.AstToString(stmt) << endl;
        if (collect != 0)
          (*collect)( pair<AstNodePtr, AstNodePtr>(ref, stmt));
      }
      return true;
   }
};

class CollectModRefWrap : public CollectReadRefWrap
{
  CollectObject< pair< AstNodePtr, AstNodePtr> >* mod;
 public:
  CollectModRefWrap( AstInterface& _fa, FunctionSideEffectInterface* f,
                     const AstNodePtr& _stmt,
                      CollectObject< pair<AstNodePtr,AstNodePtr> >* read,
                      CollectObject< pair<AstNodePtr,AstNodePtr> >* m)
    : CollectReadRefWrap(_fa, f, _stmt, read), mod(m) {}
  Boolean operator() ( const AstNodePtr& ref)
   {
      AstInterface::AstNodeList args;
      if (fa.IsFunctionCall(ref, 0,&args) || fa.IsArrayAccess(ref, 0, &args)) {
        CollectReadRefWrap read(*this);
        for (AstInterface::AstNodeListIterator p = 
               fa.GetAstNodeListIterator(args); !p.ReachEnd(); ++p) {
               AstNodePtr cur = *p;
               read(cur);
        }
      }
      if (DebugLocalInfoCollect()) 
          cerr << "appending modifying " << fa.AstToString(ref) << " : " << fa.AstToString(stmt) << endl;
      if (mod != 0)
        (*mod)( pair<AstNodePtr, AstNodePtr>(ref, stmt));
      return true;
   }
};

void StmtSideEffectCollect::
AppendModLoc( AstInterface& fa, const AstNodePtr& mod, const AstNodePtr& rhs)
    {  
      if (DebugLocalInfoCollect()) {
          cerr << "appending modifying " << fa.AstToString(mod) << " = " << fa.AstToString(rhs) << endl;
      }
       if (killcollect != 0 && rhs != 0)
            (*killcollect)( pair<AstNodePtr,AstNodePtr>(mod, curstmt));
       if (modcollect != 0)
            (*modcollect)( pair<AstNodePtr,AstNodePtr>(mod, curstmt)); 
    }
void StmtSideEffectCollect::
AppendReadLoc( AstInterface& fa, const AstNodePtr& read)
    {   
      if (DebugLocalInfoCollect()) {
          cerr << "appending reading " << fa.AstToString(read) << endl;
      }
       if (readcollect != 0)
               (*readcollect)(pair<AstNodePtr,AstNodePtr>(read, curstmt)); 
    }

void StmtSideEffectCollect::
AppendFuncCall( AstInterface& fa, const AstNodePtr& fc)
{
 CollectReadRefWrap read(fa, funcanal, curstmt, readcollect);
 if (funcanal == 0 || !funcanal->get_read(fa, fc, &read))  {
      readunknown = true;
      if (DebugLocalInfoCollect()) 
         cerr << "no interprecedural read info for : " << fa.AstToString(fc) << endl;
      AppendFuncCallArguments(fa, fc);
  }

  CollectModRefWrap mod(fa, funcanal, curstmt, readcollect, modcollect);
  if (funcanal == 0 || !funcanal->get_modify( fa, fc, &mod))  {
      if (DebugLocalInfoCollect()) 
         cerr << "no interprecedural mod info for : " << fa.AstToString(fc) << endl;
       AppendFuncCallWrite(fa, fc);
       modunknown = true;
  }
}


class ModifyAliasMap : public CollectObject< pair<AstNodePtr, int> >
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

  Boolean operator() ( const pair<AstNodePtr, int>& cur)
  {
    string varname;
    AstNodePtr scope;
    if (cur.first == 0 || !fa.IsVarRef(cur.first, 0, &varname, &scope))
      return false;
    if (cur.second == index) {
      aliasmap.get_alias_map(varname, scope).union_with(repr);
      if (DebugAliasAnal()) 
         cerr << "aliasing with: " << varname << endl;
    }
    else {
      assert( cur.second > index); 
      repr = &aliasmap.get_alias_map(varname, scope);
      if (DebugAliasAnal()) 
         cerr << "cur alias-var repr:" << varname << endl;
      index = cur.second;
    }
    return true;
  }
};

string ScopeStringMap :: get_scope_string( const AstNodePtr& s)
{
  assert( s != 0);
  map<AstNodePtr,string>::const_iterator p = scopemap.find(s);
  if (p != scopemap.end()) 
    return (*p).second; 
  stringstream buf;
  ++cur;
  buf <<  cur;
  string r= buf.str();
  scopemap[s] = r;
  return r;
}

string ScopeStringMap :: get_scope_string( const AstNodePtr& s) const
{
  assert( s != 0);
  map<AstNodePtr,string>::const_iterator p = scopemap.find(s);
  if (p == scopemap.end()) {
     cerr << "Error: cannot find scope: " << s << AstInterface::AstToString(s) << endl;
     for (p = scopemap.begin(); p != scopemap.end(); ++p) {
          cerr << " : " << (*p).first << ":" << (*p).second << endl;
     }
     assert(false);
  }
  return (*p).second;
}

UF_elem&  StmtVarAliasCollect::VarAliasMap::
get_alias_map( const string& varname, const AstNodePtr& scope)
{
  string scopename = scopemap.get_scope_string(scope);
  string name = varname + scopename;
  return aliasmap[name];
}

void StmtVarAliasCollect::
AppendModLoc( AstInterface& fa, const AstNodePtr& mod,
                              const AstNodePtr& rhs)
{
  string modname;
  AstNodeType modtype;
  AstNodePtr modscope;
  if (rhs == 0 || !fa.IsVarRef(mod, &modtype, &modname, &modscope) || fa.IsScalarType(modtype))
    return;
  AstInterface::AstNodeList args;
  if (fa.IsFunctionCall( rhs, 0, &args) ) {
    ModifyAliasMap collect(fa, aliasmap);
    if (funcanal != 0 && funcanal->may_alias( fa, rhs, mod, collect))
        return;
    hasunknown = true;
    if (DebugAliasAnal()) {
        cerr << "unknown alias info for function call : " << fa.AstToString(rhs) << endl;
        cerr << "aliasing all parameters with " << fa.AstToString(mod) << endl;;
    }
    collect.reset();
    collect( pair<AstNodePtr,int>(mod, 0));
    for (AstInterface::AstNodeListIterator p = fa.GetAstNodeListIterator(args);
          !p.ReachEnd(); ++p) {
       collect( pair<AstNodePtr,int>(*p, 0));
    }
  }
  else {
    string rhsname;
    AstNodeType rhstype;
    AstNodePtr rhsscope;
    if (fa.IsVarRef(rhs, &rhstype, &rhsname, &rhsscope)) {
      if (!fa.IsScalarType(rhstype)) 
         aliasmap.get_alias_map(modname, modscope).union_with(&aliasmap.get_alias_map(rhsname, rhsscope));
    }
  }
}

void StmtVarAliasCollect::
AppendFuncCall( AstInterface& fa, const AstNodePtr& fc)
{
  ModifyAliasMap collect(fa, aliasmap);
  if (funcanal == 0 || !funcanal->may_alias( fa, fc, 0, collect)) {
     hasunknown = true;
    if (DebugAliasAnal()) {
        cerr << "unknown alias info. for function call : " << fa.AstToString(fc) << endl;
        cerr << "aliasing all parameters \n";
     }
     collect.reset();
     AstInterface::AstNodeList args;
     if (!fa.IsFunctionCall( fc, 0, &args)) 
         assert(false);
     for (AstInterface::AstNodeListIterator p = fa.GetAstNodeListIterator(args);
           !p.ReachEnd(); ++p) {
         collect( pair<AstNodePtr,int>(*p, 0));
     }
  }
}

bool StmtVarAliasCollect::
may_alias(AstInterface& fa, const AstNodePtr& r1, 
	  const AstNodePtr& r2)
{
  AstNodeType t1, t2;
  if (!fa.IsExpression(r1, &t1) || !fa.IsExpression(r2, &t2))
    assert(false);
  if (!hasresult) {
    if (DebugAliasAnal()) {
       cerr << "no alias analysis performed \n";
       cerr << "has alias between " << fa.AstToString(r1) << " and " << fa.AstToString(r2) << endl;
    }
    return true;
  }
  string varname1, varname2;
  AstNodePtr scope1, scope2;
  bool global1, global2;
  if (!fa.IsVarRef(r1, 0, &varname1, &scope1, &global1) 
      || !fa.IsVarRef(r2, 0, &varname2, &scope2, &global2)) {
    if (DebugAliasAnal()) {
       cerr << "no alias analysis between non_variables performed \n";
       cerr << "has alias between " << fa.AstToString(r1) << " and " << fa.AstToString(r2) << endl;
    }
    return true;
  }

  if (global1 && global2 && hasunknown) {
    if (DebugAliasAnal()) {
       if (global1) {
          cerr << varname1 << " and " << varname2 << " are global \n";
          cerr << "has alias between " << fa.AstToString(r1) << " and " << fa.AstToString(r2) << endl;
       }
    }
    return true;
  }
  if ( aliasmap.get_alias_map(varname1, scope1).in_same_group( &aliasmap.get_alias_map(varname2, scope2))) {
    if (DebugAliasAnal()) {
       cerr << "alias analysis performed \n";
       cerr << "has alias between " << fa.AstToString(r1) << " and " << fa.AstToString(r2) << endl;
     }
     return true;
  }
  return false;
}

void StmtVarAliasCollect::
operator()( AstInterface& fa, const AstNodePtr& funcdef)
{
  assert(!hasresult);
  hasresult = true;
  AstNodePtr funcdecl, body;
  if (!fa.IsFunctionDefinition(funcdef, &funcdecl, &body)) {
     cerr << "Error: alias analysis requires function definition as input instead of " << fa.AstToString(funcdef) << endl;
     assert(false);
  }

  ModifyAliasMap collect(fa, aliasmap);
  if (funcanal == 0 || !funcanal->allow_alias( fa, funcdecl, collect)) {
     AstInterface::AstNodeList params;
     if (!fa.IsFunctionDecl( funcdecl, 0, 0, 0, &params))
       assert(false);
     for (AstInterface::AstNodeListIterator p = fa.GetAstNodeListIterator(params);
          !p.ReachEnd(); ++p) {
         collect( pair<AstNodePtr,int>(*p, 0));
     }
  }
  StmtInfoCollect::operator()(fa, body);
}

