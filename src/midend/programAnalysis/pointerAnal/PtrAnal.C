

#include "sage3basic.h"
#include <iostream>
#include <sstream>

#include <CommandOptions.h>
#include <PtrAnal.h>

using namespace std;

extern bool DebugAliasAnal ();

inline void stmts_pushback(std::vector<PtrAnal::Stmt>& stmts, PtrAnal::Stmt s)
{
  if (s != 0)
     stmts.push_back(s);
}

inline PtrAnal::Stmt stmts_back(std::vector<PtrAnal::Stmt>& stmts)
{
  if (stmts.size())
     return stmts.back();
  return 0;
}

static std::string func_return_name(const std::string fname)  
   { return InterProcVariableUniqueRepr::get_unique_name(fname,0); }

#if 0
static bool is_constant(const std::string& name)
{
  if ( name != "" && name[0] == 'c') {
     if (DebugAliasAnal())
        std::cerr << " constant : " << name << "\n";
     return true;
  }
  if (DebugAliasAnal())
     std::cerr << " not constant : " << name << "\n";
  return false;
}
#endif

static std::string 
Local_GetFieldName(AstInterface& fa, const AstNodePtr& field)
{
   std::string name;
   if (fa.IsVarRef(field, 0, &name)) {
       assert(name != "");
       return "d:" + name;
    }
    std::cerr << "Not field name: " << AstToString(field) << "\n";
    assert(false);

 /* Avoid MSVC warning */
    return "error";
}

static std::string 
Local_GetVarName(AstInterface& fa, const AstNodePtr& scope, std::string name)
{
   assert(name != "");
   return "v:"+InterProcVariableUniqueRepr::get_unique_name (fa, scope, name);
}

static std::string 
Local_GetConstName(AstInterface& fa, std::string val)
{
   return "c:"+val;
}

PtrAnal::VarRef PtrAnal::translate_exp(const AstNodePtr& exp) const
{
  VarRef cur;
  NameMap::const_iterator p = namemap.find(exp.get_ptr());
  if (p != namemap.end()) {
     cur = (*p).second;
  }
  return cur;
}

PtrAnal::StmtRef PtrAnal::translate_stmt(const AstNodePtr& s) const
{
  StmtRef cur;
  StmtMap::const_iterator p = stmtmap.find(s.get_ptr());
  if (p != stmtmap.end()) {
     std::pair<size_t,size_t> i = (*p).second;
     for (size_t j = i.first; j <= i.second; ++j) {
        cur.push_back(stmts[j]);
     }
  }
  return cur;
}
    
static std::string 
Local_GetVarName(AstInterface& fa, const AstNodePtr& var)
{
   std::string res = 
    "v:"+InterProcVariableUniqueRepr::get_unique_name (fa, var);
  return res;
}

std::string PtrAnal:: 
Get_VarName(AstInterface& fa, const AstNodePtr& rhs)
{
  NameMap::const_iterator p = namemap.find(rhs.get_ptr());
  if (p != namemap.end()) {
     VarRef cur = (*p).second;
     return cur.name;
  }

   std::string readname, res;
   AstNodePtr readscope; 
   AstNodePtr lhs1, rhs1;
   bool readlhs;

   if (fa.IsVarRef(rhs, 0, &readname, &readscope))  {
       assert(readname != "");
       res = Local_GetVarName(fa, readscope, readname); 
    }
   else if (fa.IsConstant(rhs,&res,&readname)) {
      if (res == "string") {
         readname =  Local_GetVarName(fa, rhs);
         Stmt stmt = allocate_x(readname);
         stmts_pushback(stmts,stmt);
         res= readname;
      }
      else
         res = Local_GetConstName(fa, readname);
   }
   else if (fa.IsUnaryOp(rhs) || fa.IsBinaryOp(rhs) || fa.IsFunctionCall(rhs)) {
        readname = Local_GetVarName(fa, rhs);
        ProcessExpression(fa, readname, rhs);
        res = readname;
   }
   else if (fa.IsAssignment(rhs,&lhs1,&rhs1,&readlhs)) {
        ProcessAssign(fa, lhs1, rhs1, readlhs);
        VarRef cur = namemap[lhs1.get_ptr()];
        if (cur.name == "") {
           std::cerr << "does not have map for " << AstToString(lhs1) << " in " << AstToString(rhs) << "\n";
           assert(0);
        }
        namemap[rhs.get_ptr()] = cur;
        res = cur.name;
   }
   else  {
      std::cerr << "No name found for " << AstToString(rhs) << "\n";
      assert(false);
   }
   if (res == "") {
      std::cerr << "No name found for " << AstToString(rhs) << "\n";
      assert(false);
   }
   return res;
}

void PtrAnal::
ProcessExpression( AstInterface& fa, const std::string& _modname, const AstNodePtr& rhs)
{
  AstInterface::OperatorEnum op;
  std::string modname = _modname;
  AstNodePtr p,p1,p2;
  AstInterface::AstNodeList args, out;
  if (fa.IsFunctionCall(rhs, &p, &args, &out)) {
     typedef std::pair<AstNodePtr,std::string>  RefRec;
     std::list<RefRec>  refs;
     std::string fname = Get_VarName(fa,p);
     refs.push_back(RefRec(p,fname));
     std::list<std::string> pargs, pres;
     for (AstInterface::AstNodeList::const_iterator p = args.begin(); 
          p != args.end(); ++p) {
         AstNodePtr cur = *p;
         std::string curname = Get_VarName(fa, cur); 
         pargs.push_back(curname);
         refs.push_back(RefRec(cur,curname));
     }
     pres.push_back(modname);
     for (AstInterface::AstNodeList::const_iterator p2 = out.begin(); 
          p2 != out.end(); ++p2) {
         AstNodePtr cur = *p2;
         std::string curname = Get_VarName(fa, cur); 
         pres.push_back(curname);
         refs.push_back(RefRec(cur,curname));
     }
     Stmt stmt = funccall_x(fname, pargs, pres); 
     stmts_pushback(stmts,stmt);
     for (std::list<RefRec>::const_iterator p3 = refs.begin();
          p3 != refs.end(); ++p3) {
        RefRec cur = *p3;
        namemap[cur.first.get_ptr()] = PtrAnal::VarRef(stmt,cur.second);
     }
  }
  else if (fa.IsUnaryOp(rhs,&op, &p) ) {
        std::string readname;
        if (modname == "") {
           modname = Local_GetVarName(fa, rhs);
        }
        switch (op) {
        case AstInterface::UOP_ADDR: {
           readname= Get_VarName(fa, p); 
           Stmt stmt = x_eq_addr_y(modname, readname); 
           stmts_pushback(stmts,stmt);
           namemap[p.get_ptr()] = VarRef(stmt,readname);
           namemap[rhs.get_ptr()] = VarRef(stmt,modname);
           break;
        }
        case AstInterface::UOP_DEREF:  {
           readname= Get_VarName(fa, p); 
           Stmt stmt = x_eq_deref_y(modname, "", readname);
           stmts_pushback(stmts,stmt);
           namemap[p.get_ptr()] = VarRef(stmt,readname);
           namemap[rhs.get_ptr()] = VarRef(stmt,modname);
           break;
        }
        case AstInterface::UOP_ALLOCATE: {
           Stmt stmt = allocate_x(modname) ; 
           stmts_pushback(stmts,stmt);
           break;
         }
        default: {
           readname = Get_VarName(fa,p);
           std::list<std::string> opds;
           opds.push_back(readname);
           Stmt stmt_last = x_eq_op_y(OTHER,modname, opds);
           stmts_pushback(stmts,stmt_last);
           namemap[p.get_ptr()] = VarRef(stmt_last,readname);
           namemap[rhs.get_ptr()] = VarRef(stmt_last,modname);
           break;
         }
        }
   }
   else if (fa.IsBinaryOp(rhs, &op, &p1, &p2)) {
      std::list<std::string> opds;
      std::string readname , refname;
      if (modname == "") {
           modname = Local_GetVarName(fa, rhs);
      }
      Stmt stmt_last=0;
      switch (op) {
      case  AstInterface::BOP_DOT_ACCESS:  {
          readname = Get_VarName(fa, p1);
          refname = Local_GetFieldName(fa, p2);
          stmt_last = x_eq_field_y( modname, refname, readname);
          stmts_pushback(stmts,stmt_last);
             break;
       }
      case  AstInterface::BOP_ARROW_ACCESS:  {
          readname = Get_VarName(fa, p1);
          refname = Local_GetFieldName(fa, p2);
          stmt_last = x_eq_deref_y(modname, refname, readname);
          stmts_pushback(stmts,stmt_last);
             break;
       }
      default: {
         readname = Get_VarName(fa,p1);
         refname = Get_VarName(fa,p2);
         opds.push_back(readname);
         opds.push_back(refname);
         OpType opt = OTHER;
         switch (op) {
         case AstInterface::BOP_EQ: opt = EQ; break;
         case AstInterface::BOP_LT: opt = LT; break;
         case AstInterface::BOP_GT: opt = GT; break;
         case AstInterface::BOP_LE: opt = LE; break;
         case AstInterface::BOP_GE: opt = GE; break;
         case AstInterface::BOP_NE: opt = NE; break;
         default: break;
         }
         stmt_last = x_eq_op_y( opt, modname, opds);
         stmts_pushback(stmts,stmt_last);
      }
     }
      namemap[p1.get_ptr()] = VarRef(stmt_last,readname);
      namemap[p2.get_ptr()] = VarRef(stmt_last,refname);
      namemap[rhs.get_ptr()] = VarRef(stmt_last,modname);
   }
   else {
      std::string cur = Get_VarName(fa, rhs);
      assert  (cur != "");
      if (modname != "") { 
         Stmt stmt_last = x_eq_y(modname, cur); 
         stmts_pushback(stmts,stmt_last);
         namemap[rhs.get_ptr()] = VarRef(stmt_last,cur);
      }
   }
}

void PtrAnal::
ProcessMod(AstInterface& fa, const std::string& readname, 
          std::list<std::string>& fields, const AstNodePtr& mod)
{
  std::string modname;
#if 0   // Liao, 8/16/2010, change code based on Qing's suggestion
  // Stmt first = 0;
  if (fa.IsVarRef(mod)) {
#else
   AstNodePtr p = mod;
   if (fa.IsVarRef(mod) || fa.IsArrayAccess(mod, &p)) {
#endif    
      modname = Get_VarName(fa,mod);
      Stmt stmt_last = fields.size()?
                      field_x_eq_y(modname, fields, readname)
                     : x_eq_y(modname, readname);
      stmts_pushback(stmts,stmt_last);
      namemap[mod.get_ptr()] = VarRef(stmt_last,modname);
  }
  else {
     AstInterface::OperatorEnum op;
     AstNodePtr p, p2;
     if (fa.IsUnaryOp(mod,&op, &p) && op == AstInterface::UOP_DEREF) {
            std::string lhs = Get_VarName(fa,p); 
            Stmt stmt_last  = deref_x_eq_y(lhs, fields, readname); 
            stmts_pushback(stmts,stmt_last);
            namemap[p.get_ptr()] = VarRef(stmt_last,lhs);
            namemap[mod.get_ptr()] = VarRef(stmt_last, readname);
     }
    else if (fa.IsBinaryOp(mod,&op,&p,&p2)) { 
         if (op==AstInterface::BOP_DOT_ACCESS) {
            std::string field = Local_GetFieldName(fa, p2);
            fields.push_front(field);
            ProcessMod(fa, readname, fields, p);
            Stmt stmt_last = stmts_back(stmts);
            namemap[mod.get_ptr()] = VarRef(stmt_last, readname);
         }
         else if (op==AstInterface::BOP_ARROW_ACCESS) {
            std::string lhs = Get_VarName(fa, p), field = Local_GetFieldName(fa, p2);
            fields.push_front(field);
            Stmt stmt_last  = deref_x_eq_y(lhs,fields,readname);
            stmts_pushback(stmts,stmt_last);
            namemap[p.get_ptr()] = VarRef(stmt_last,lhs);
            namemap[mod.get_ptr()] = VarRef(stmt_last, readname);
         }
         else {
            std::cerr << "can't handle " << AstToString(mod) << "\n";
            assert(false); // other operations? to be handled later  
         }
     }
     else if (fa.IsFunctionCall(mod)) {
         std::string lhs = Get_VarName(fa, mod);
         Stmt stmt_last = deref_x_eq_y(lhs, fields, readname);
         stmts_pushback(stmts,stmt_last);
         namemap[mod.get_ptr()] = VarRef(stmt_last,lhs);
     }
     else {
       std::cerr << "cannot process " << AstToString(mod) << "\n";
       assert(false); // other operations? to be handled later  
    }
  }
}

void PtrAnal::
ProcessAssign( AstInterface& fa, const AstNodePtr& mod, const AstNodePtr& rhs, bool readlhs)
{
  std::string modname, readname;
  AstNodePtr modscope, readscope;
 
  readname = Get_VarName(fa, rhs); 
  if (!fa.IsVarRef(mod, 0, &modname, &modscope) )  {
     if (readlhs)  {
          modname = Get_VarName(fa,mod);
          std::list<std::string> opds;
          opds.push_back(modname);
          opds.push_back(readname);
          Stmt stmt_last = x_eq_op_y(OTHER, readname, opds);
          stmts_pushback(stmts,stmt_last);
     }
     std::list<std::string> fields;
     ProcessMod(fa, readname, fields, mod);
     Stmt stmt_last = stmts_back(stmts);
     namemap[rhs.get_ptr()] = VarRef(stmt_last,readname);
  }
  else if (!readlhs) {
       assert (rhs != 0) ;
       modname = Local_GetVarName(fa, modscope, modname);
       ProcessExpression(fa, modname, rhs);
       Stmt stmt_last = stmts_back(stmts);
       namemap[mod.get_ptr()] = VarRef(stmt_last,modname);    
  }
  else {
       std::string cur = Get_VarName(fa,rhs);
       modname = Local_GetVarName(fa, modscope, modname);
       std::list<std::string> opds;
       opds.push_back(modname);
       opds.push_back(readname);
       Stmt stmt_last = x_eq_op_y(OTHER, modname, opds);
       stmts_pushback(stmts,stmt_last);
       namemap[mod.get_ptr()] = VarRef(stmt_last, modname);
  }
}

bool PtrAnal::
may_alias(AstInterface& fa, const AstNodePtr& _r1, const AstNodePtr& _r2)
{
  AstNodePtr r1 = fa.IsExpression(_r1);
  AstNodePtr r2 = fa.IsExpression(_r2);
  if (r1 == AST_NULL || r2 == AST_NULL)
    assert(false);
  std::string varname1 = Get_VarName(fa, r1), varname2 = Get_VarName(fa, r2);
  return may_alias(varname1, varname2);
}

void PtrAnal:: operator()( AstInterface& fa, const AstNodePtr& funcdef)
{
  AstNodePtr  body;
  AstInterface::AstNodeList params, outpars;
  std::string fname;
  if (!fa.IsFunctionDefinition(funcdef, &fname, &params, &outpars, &body)) {
     std::cerr << "Error: analysis requires function definition as input instead of " << AstToString(funcdef) << std::endl;
     assert(false);
  }
  typedef std::pair<AstNodePtr,std::string>  RefRec;
  std::list<RefRec>  refs;

  fname = Local_GetVarName(fa, AST_NULL, fname);
  fdefined.push_back(fname);

  std::list<std::string> pnames, pres;
  for (AstInterface::AstNodeList::const_iterator p = params.begin(); p != params.end(); ++p) {
       std::string curname = Get_VarName(fa, *p);
       pnames.push_back(curname);
       refs.push_back(RefRec(*p, curname));
  }
  pres.push_back( func_return_name(fname));
  for (AstInterface::AstNodeList::const_iterator p2 = outpars.begin(); 
        p2 != outpars.end(); ++p2) {
       std::string curname = Get_VarName(fa, *p2);
       pres.push_back(curname);
       refs.push_back(RefRec(*p2, curname));
  }
  Stmt stmt_last = funcdef_x(fname, pnames, pres);
  stmts_pushback(stmts,stmt_last);
  for (std::list<RefRec>::const_iterator p3 = refs.begin();
        p3 != refs.end(); ++p3) {
      RefRec cur = *p3;
      namemap[cur.first.get_ptr()] = PtrAnal::VarRef(stmt_last,cur.second);
  }
  ProcessAstTreeBase::operator()(fa, body);
  ControlFlowAnalysis(fa, body, stmt_last);
}

bool PtrAnal::
ProcessTree( AstInterface &fa, const AstNodePtr& s, AstInterface::TraversalVisitType t)
{
 if (t == AstInterface::PreVisit) {
    AstNodePtr lhs, rhs;
    AstInterface::AstNodeList vars, args;
    if (fa.IsStatement(s)) {
       if (DebugAliasAnal()) 
           std::cerr << "pre visiting " << AstToString(s) << "\n";
      stmt_active.push_back(stmts.size());
    }

    if (fa.IsReturn(s,&rhs)) {
       // size_t stmt_firstIndex = stmts.size();
       std::string fname = fdefined.back(); 
       if (rhs != AST_NULL) {
          std::string rhsname = Get_VarName(fa, rhs);
          Stmt stmt_last = x_eq_y(func_return_name(fname), rhsname);
          stmts_pushback(stmts,stmt_last);
          namemap[rhs.get_ptr()] = VarRef(stmt_last, rhsname);
       }
       Stmt stmt_last = funcexit_x(fname);
       stmts_pushback(stmts,stmt_last);
       Skip(s);
    }
    else if (fa.IsVariableDecl( s, &vars, &args)) {
       // size_t stmt_firstIndex = stmts.size();
       AstInterface::AstNodeList::const_iterator pv = vars.begin();
       AstInterface::AstNodeList::const_iterator pa = args.begin();
       while (pv != vars.end()) {
         AstNodePtr v = *pv;
         AstNodePtr a = *pa;
         if (a != AST_NULL) {
           ProcessAssign(fa, v, a);
         }
         ++pv;
         ++pa;
      }
       Skip(s);
   }
   else if ( (lhs = fa.IsExpression(s)) != AST_NULL) {
       ProcessExpression(fa, "", lhs);
       Skip(s);
   }
 }
 else {
   if (DebugAliasAnal()) 
      std::cerr << "post visiting " << AstToString(s) << "\n";
   if (fa.IsStatement(s)) {
       size_t stmt_firstIndex = stmt_active.back();
       stmt_active.pop_back();
       if (stmt_firstIndex < stmts.size()) {
          if (DebugAliasAnal()) 
              std::cerr << "setting stmt mapping \n";
          stmtmap[s.get_ptr()] = pair<size_t,size_t>(stmt_firstIndex, stmts.size()-1);
       }
       else
          if (DebugAliasAnal()) 
             std::cerr << "no translation: " << AstToString(s) << "\n"; 
  }
 }
 return true;
}

