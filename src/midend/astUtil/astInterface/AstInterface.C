#include "AstInterface.h"
#include <rose.h>
#include "AstInterface_ROSE.h"
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <CommandOptions.h>
#include <stdexcept>

#include "AstTraversal.h"
#include "astPostProcessing.h"

// jichi (9/29/2009): Add test for Fortran language
#define IS_FORTRAN_LANGUAGE() \
        SageInterface::is_Fortran_language()

#define NEW_EXPR_STMT(stmt,exp)  \
        stmt = new SgExprStatement(GetFileInfo(), exp); \
        exp->set_parent(stmt); stmt->set_endOfConstruct(stmt->get_file_info())

#define NEW_SYMBOL(sym, className, scope, name) \
        sym = new className(name); name->set_scope(scope); \
        scope->insert_symbol( name->get_name(), sym)

#define NEW_BLOCK(block) \
        block = new SgBasicBlock(GetFileInfo()); \
        block->set_endOfConstruct(block->get_file_info())

#define NEW_BLOCK1(block,stmt) \
        NEW_BLOCK(block); \
        block->append_statement(stmt); stmt->set_parent(block)

#define NEW_FUNCTION_REF(fr, fsym) \
        fr = new SgFunctionRefExp(GetFileInfo(), fsym); \
        fr->set_endOfConstruct(fr->get_file_info())

#define NEW_MFUNCTION_REF(fr, fsym) \
        fr = new SgMemberFunctionRefExp(GetFileInfo(), fsym); \
        fr->set_endOfConstruct(fr->get_file_info()); \
        fr->set_need_qualifier(false)

#define NEW_EXPR_LIST(explist)   \
        explist = new SgExprListExp(GetFileInfo()); \
        explist->set_endOfConstruct(explist->get_file_info())

#define NEW_FUNCTION_CALL(fcall, fref, args)  \
        fcall = new SgFunctionCallExp(GetFileInfo(), fref, args); \
        fcall->set_endOfConstruct(fcall->get_file_info()); \
        fref->set_parent(fcall); args->set_parent(fcall)

#define NEW_VAR_INIT(init, var, exp) \
        init = new SgAssignInitializer(GetFileInfo(), exp, exp->get_type() ); \
        init->set_endOfConstruct(init->get_file_info()); exp->set_parent(init); \
        var->set_initializer( init); init->set_parent(var) \

#define NEW_BIN_OP(op,className,lhs,rhs) \
    op = new className( GetFileInfo(), lhs, rhs); \
    op->set_endOfConstruct(op->get_file_info()); \
    lhs->set_parent(op); rhs->set_parent(op)


#define NEW_IF(r, cond, tbody)   \
    r = new SgIfStmt(GetFileInfo(), cond, tbody,new SgBasicBlock(GetFileInfo())); \
    r->set_has_end_statement(true); \
    r->set_endOfConstruct(r->get_file_info()); \
    r->get_false_body()->set_endOfConstruct(r->get_file_info()); \
    cond->set_parent(r); tbody->set_parent(r); r->get_false_body()->set_parent(r) 

#define NEW_IF_ELSE(r, cond, tbody, fbody)   \
    r = new SgIfStmt(GetFileInfo(), cond, tbody, fbody); \
    r->set_has_end_statement(true); \
    r->set_endOfConstruct(r->get_file_info()); \
    cond->set_parent(r); tbody->set_parent(r); fbody->set_parent(r) 

SgType* GetTypeInt()
{
  static SgType* typeint = 0;
  if (typeint == 0)
     typeint = new SgTypeInt();
  return typeint;
}

void AstInterface :: SetRoot( const AstNodePtr& root)
{ impl->set_top(AstNodePtrImpl(root).get_ptr()); }

AstNodePtr AstInterface :: GetRoot() const 
{ return AstNodePtrImpl(impl->get_top()); }

void AstInterface :: AttachObserver( AstObserver* ob)
{
  impl->AttachObserver(ob);
}

void AstInterface :: DetachObserver( AstObserver* ob)
{
  impl->DetachObserver(ob);
}

AstNodePtr AstInterface::GetFunctionDefinition( const AstNodePtr &n, std::string* name)
{
  AstNodePtr r=n;
  while (r != AST_NULL && !IsFunctionDefinition(r, name)) {
     r = GetParent(r);
  }
  return r;
}

std::string get_type_name( SgType* t);
using namespace std;

// Removing the leading "::" from a qualified name
string StripGlobalQualifier(string name)
{
   if (name.size() > 2 && name[0] == ':' && name[1] == ':') {
      return string(name.c_str()+2);
   }
   return name;
}

bool DebugNewVar()
{
  static int r = 0;
  if (r == 0) {
    if (CmdOptions::GetInstance()->HasOption("-debugnewvar"))
       r = 1;
    else
       r = -1;
  }
  return r == 1;
}

bool DebugType()
{
  static int r = 0;
  if (r == 0) {
    if (CmdOptions::GetInstance()->HasOption("-debugtype"))
       r = 1;
    else
       r = -1;
  }
  return r == 1;
}

bool DebugSymbol()
{
  static int r = 0;
  if (r == 0) {
    if (CmdOptions::GetInstance()->HasOption("-debugsymbol"))
       r = 1;
    else
       r = -1;
  }
  return r == 1;
}

Sg_File_Info* GetFileInfo()
   {
  // DQ (3/8/2006): This is the easiest way to represent a transformation
  // since we have to both mark the file info object as a transformation
  // AND to be output in the code generation phase as well.
     return Sg_File_Info::generateDefaultFileInfoForTransformationNode();
   }

SgScopeStatement* GetNullScope()
{
  static SgGlobal* global = 0;
  if (global == 0) {
    global = new SgGlobal();
  }
  return global;
}

inline bool HasNullParent(SgNode* n)
{
  return n->get_parent() == 0 || n->get_parent() == GetNullScope();
}


SgStatement* ToStatement( SgNode* _stmts)
{
    SgStatement *stmts = isSgStatement(_stmts);
    if (stmts == 0) {
       SgExpression *exp = isSgExpression(_stmts);
       assert(exp != 0);
       NEW_EXPR_STMT(stmts,exp);
    }
    return stmts;
}

SgClassDefinition* GetClassDefn( SgClassDeclaration* classDecl)
{
  SgDeclarationStatement* decl = classDecl->get_definingDeclaration();
  assert(decl != 0);
  classDecl = isSgClassDeclaration(decl);
  assert(classDecl != 0);
  SgClassDefinition* classDefn = classDecl->get_definition();
  return classDefn;
}

SgClassDefinition* GetClassDefinition( SgNamedType *classtype)
{
    if (classtype->variantT() == V_SgTypedefType) {
         return GetClassDefinition(isSgNamedType(isSgTypedefType(classtype)->get_base_type()));
    }
    SgDeclarationStatement *decl = classtype->get_declaration();
    if (decl->variantT() == V_SgClassDeclaration) 
        return GetClassDefn(isSgClassDeclaration(decl));
    else {
       cerr << "unexpected class declaration type: " << decl->sage_class_name() << endl;
       assert(false);
    }
}

SgScopeStatement* GetScope( SgNode* loc)
{
  // DQ (3/23/2006): It is particularly dangerous in C++ to
  // interperate the scope from the structure.  Places where this
  // could be a problem now carry the scope explicitly (this step
  // was introduced after Qing's work on the AstInterface).
  // I have fixed up this code to report the correct scope
  // using the virtual get_scope() function for SgStatements.
      
     ROSE_ASSERT(loc != NULL);
  // printf ("Inside of GetScope(%p = %s) \n",loc,(loc != NULL) ? loc->class_name().c_str() : "NULL");
        
     SgScopeStatement *cur = NULL;
           
  // Handle special case of the scope being correct
     SgScopeStatement* scopeStatement = isSgScopeStatement(loc);
     if (scopeStatement != NULL)
        {           
          cur = scopeStatement;
        }           
       else   
        {           
          SgStatement* stmt = isSgStatement(loc);
          if (stmt != NULL)
             {
               cur = stmt->get_scope();
             }
            else
             { 
               SgInitializedName* initializedName = isSgInitializedName(loc);
            // ROSE_ASSERT(initializedName != NULL);
               if (initializedName != NULL)
                  {
                    cur = initializedName->get_scope();
                  }
                 else
                  {
                    SgExpression* expression = isSgExpression(loc);
                    if (expression != NULL)
                       {
                         SgStatement* statement = TransformationSupport::getStatement(expression);
                         ROSE_ASSERT(statement != NULL);
                         cur = GetScope(statement);
                       }
                      else
                       {
                         printf ("Error: Unprepared for this case in GetScope(%p = %s) \n",loc,(loc != NULL) ? loc->class_name().c_str() : "NULL");
                         ROSE_ASSERT(false);
                       }
                  }
             }
        }

     ROSE_ASSERT(cur != NULL);
     return cur;
}

SgVarRefExp* ToVarRef( AstInterface& fa, const AstNodePtr& _exp)
{
  SgNode* exp = AstNodePtrImpl(_exp).get_ptr();
  switch (exp->variantT()) {
  case V_SgVarRefExp:
     return isSgVarRefExp( exp );
  case V_SgInitializedName:
     {
      SgInitializedName* var = isSgInitializedName(exp);
      SgName varname = var->get_name();
      SgNode* r = AstNodePtrImpl(fa.CreateVarRef( string(varname.str()), _exp)).get_ptr();
      r->set_parent(GetNullScope()); 
      return isSgVarRefExp(r);
     }
  default: break;
  }
  return 0;
}

SgExpression* ToExpression( AstInterface& fa, const AstNodePtr& _s)
{
  SgNode* s = AstNodePtrImpl(_s).get_ptr(); 
  SgExpression *exp = ToVarRef(fa, _s);
  if (exp == 0)
      exp = isSgExpression(s);
  return exp;
}

// Strip leading "const" and tailing '&'
string StripParameterType( const string& name)
{
  string r = name;
  if (name.substr(0,5) == "const") 
     r = name.substr(5, name.size()-5);
  ROSE_ASSERT (!r.empty());
  size_t end = r.size()-1;
  if (r[end] == '&') {
       r[end] = ' ';
  }
  string result = "";
  for (size_t i = 0; i < r.size(); ++i) {
    if (r[i] != ' ')
      result.push_back(r[i]);
  }
  return result; 
} 

SgVariableSymbol* LookupVar( const string& name, SgScopeStatement* loc)
{
  const char* start = name.c_str();
  SgClassDefinition *cdef = isSgClassDefinition(loc);
  if (cdef != 0) {
     SgVariableSymbol* r = cdef->lookup_variable_symbol(start);
     if (DebugSymbol()) {
           if (r == 0) 
              std:: cerr << "failed to find variable " << start;
           else
              std:: cerr << "found variable " << start;
           std:: cerr << " in scope " << AstToString(AstNodePtrImpl(loc)) << "\n";
           std::cerr << " symbols of which include: ";
           for (SgSymbol* p = cdef->first_any_symbol(); p != 0; p = cdef->next_any_symbol())
               std::cerr << p->get_name().str() << ";";
           std::cerr << "\n"; 
     }
     if (r != 0)
         return r;
     SgBaseClassPtrList& l = cdef->get_inheritances();
     for (SgBaseClassPtrList::iterator p = l.begin(); p != l.end(); ++p) {
        SgBaseClass* cur = *p;
        SgClassDeclaration *decl = cur->get_base_class();
        assert(decl != 0);
        SgClassDefinition *def = GetClassDefn(decl);
        assert(def != 0);
        r = LookupVar(name, def);
        if (r != 0)
          return r;
      }
     return 0;
  }
  else {
     SgVariableSymbol* f = 0;
     do {
        f = loc->lookup_variable_symbol(start);
        if (DebugSymbol()) {
           if (f == 0) 
              std:: cerr << "failed to find variable ";
           else
              std:: cerr << "found variable ";
           std:: cerr << start << " in scope " << AstToString(AstNodePtrImpl(loc)) << "\n"; 
        }
        if (loc->variantT() == V_SgGlobal || f != 0)
             break;
        loc = loc->get_scope();
     }
     while ( loc != 0 && f == 0);
     return f;
  }
}

class SageSetTransformation: public AstTopDownProcessing< AstNodePtrImpl >
{
   AstNodePtrImpl evaluateInheritedAttribute(SgNode* astNode, AstNodePtrImpl inheritedValue)
     { 
         Sg_File_Info* r = astNode->get_file_info();
         r->setTransformation();
         r->setCompilerGenerated();
         r->setOutputInCodeGeneration();
       return astNode;
     }
  public:
   SageSetTransformation() {}
   void operator() ( SgNode* node )
       {
         AstTopDownProcessing<AstNodePtrImpl>::traverse(node, node->get_parent());
       }
};
class SageResetParent : public AstTopDownProcessing< AstNodePtrImpl >
{
   AstNodePtrImpl evaluateInheritedAttribute(SgNode* astNode, AstNodePtrImpl inheritedValue)
     { if ( inheritedValue != 0) { 
          //assert(astNode->get_parent() == inheritedValue || astNode->get_parent() == 0);
          astNode->set_parent( inheritedValue.get_ptr() );
       }
       return astNode;
     }
  public:
   SageResetParent() {}
   void operator() ( SgNode* node )
       {
         AstTopDownProcessing<AstNodePtrImpl>::traverse(node, node->get_parent());
       }
};

SgSymbol* AddDecls( AstInterfaceImpl* scope, const SgDeclarationStatementPtrList& decls)
{
     SgSymbol* result = 0;
     for (SgDeclarationStatementPtrList::const_iterator p = decls.begin(); p != decls.end(); ++p) {
         SgDeclarationStatement* cur = *p;
         cur->set_file_info( GetFileInfo());
         SgFunctionDeclaration* d1 = isSgFunctionDeclaration(cur);
         if (d1 != 0) {
            result = scope->AddFunc(d1);
         }
         else {
            SgVariableDeclaration* d2 = isSgVariableDeclaration(cur);
            if (d2 != 0) {
                result = scope->AddVar(d2);
            }
            else  {
               SgClassDeclaration* d3 = isSgClassDeclaration(cur);
               if (d3 != 0)
                   result = scope->AddClass(d3);
               else
                   assert(false);
            }
         }
    }
    return result;
}

SgMemberFunctionSymbol * 
GetMemberFunc( AstInterfaceImpl* scope, SgClassSymbol* c, 
               const string& funcname, SgExpressionPtrList* args = 0)
{
  SgClassDeclaration* decl = c->get_declaration();
  SgName classname = decl->get_name();
  SgClassDefinition * def = GetClassDefn(decl);
  if (def == 0) {
      cerr << "no definition in locating member function " << funcname << endl;
       return 0;
  }
  const char* start = funcname.c_str();
  if (args == 0) {
     SgFunctionSymbol* f = def->lookup_function_symbol(start);
     if (f != 0) {
        SgMemberFunctionSymbol* mf = isSgMemberFunctionSymbol(f);
        assert(mf != 0);
        return mf;
     }
     else
     {
       cerr <<"AstInterface.C GetMemberFunc() cannot find a symbol for "<<funcname<<" within a class "<<classname<< endl;
       return 0;
       //isSgScopeStatement(def)->print_symboltable("Dump the symbol table of the class definition:");
     }
  }
  else {
     SgDeclarationStatementPtrList & decls = def->get_members ();
     for (SgDeclarationStatementPtrList::iterator p = decls.begin(); p != decls.end();
          ++p) { 
        SgDeclarationStatement *cur = *p;
        if (cur->variantT() != V_SgMemberFunctionDeclaration)
          continue;
        SgMemberFunctionDeclaration *md = isSgMemberFunctionDeclaration(cur);
        SgName name = md->get_name();
        if ( string(name.str()) != funcname)
            continue;
        SgInitializedNamePtrList &pars = md->get_args ();
        if (pars.size() != args->size())
           continue;
        SgInitializedNamePtrList::iterator pp = pars.begin();
        SgExpressionPtrList::iterator pa = args->begin();
        bool match = true;
        for ( ; pp != pars.end(); ++pp, ++pa) {
           SgType* tp = (*pp)->get_type();
           SgType* ta = (*pa)->get_type();
           string partype, argtype;
	   scope->GetTypeInfo(AstNodeTypeImpl(tp), 0, &partype);
           scope->GetTypeInfo(AstNodeTypeImpl(ta), 0, &argtype);
           if (partype != argtype) {
              match = false;
              break;
           }
        }
        if (match) {
           //QY:1/7/08: this should be only temporary. should not create a new symbol if the symbol is already in the symbol table (no search mechanism available yet?)
           SgMemberFunctionSymbol* f = new SgMemberFunctionSymbol(md);
           return f;
        }
     }
  }
  return 0;
}

void AstInterfaceImpl:: set_top( SgNode* _top) 
  { 
      top = _top; 
      global = 0;
      scope = 0;
      if (top != 0) {
        scope = GetScope(top);
        SgStatement *cur = scope;   
        while (cur != 0 && global == 0) {
          global = isSgGlobal(cur);
          if (cur->get_parent() != 0)
             cur = cur->get_scope();
          else
             cur = 0;
        } 
      }
  }
 
SgFunctionSymbol* AstInterfaceImpl::LookupFunction(const char* start) const
   {
     SgScopeStatement *cur = scope;
     SgFunctionSymbol* f = 0;
     do {
        f = cur->lookup_function_symbol(start);
        if (DebugSymbol()) {
           if (f == 0) 
              std::cerr << "failed to find function symbol " << start << " in scope " << cur->sage_class_name() << " : " << cur->unparseToString() << "\n";
           else
              std::cerr << "found function symbol " << start << " in scope " << cur->sage_class_name() << " : " << cur->unparseToString() << "\n";
        }
       
        if (cur->variantT() == V_SgGlobal)
             break;
        assert(cur->get_scope() != cur);
        cur = cur->get_scope();
     }
     while ( cur != 0 && f == 0); 
     if (DebugSymbol()) {
        if (cur == 0 || cur == GetNullScope()) 
           std::cerr << "exit with cur = " << (cur == 0? "NULL" : "NULL scope") << "\n";
        else 
           std::cerr << "exit with cur = " << cur->sage_class_name() << "\n";
     }
     return f;
    }

SgClassSymbol* AstInterfaceImpl:: LookupClass(const char* start) const
   {
     SgScopeStatement *cur = scope;
     SgClassSymbol* f = 0;
     do {
        f = cur->lookup_class_symbol(start);
        if (cur->variantT() == V_SgGlobal)
             break;
        cur = cur->get_scope();
     }
     while ( cur != 0 && f == 0); 
     return f;
    }

SgClassSymbol* AstInterfaceImpl:: AddClass( SgClassDeclaration* d) 
  {
    assert(global != 0);
    SgDeclarationStatementPtrList& l = global->get_declarations();
    l.insert( l.begin(), d);
    SgClassSymbol *NEW_SYMBOL(c,SgClassSymbol, global,d);
    return c;
  }

SgFunctionSymbol* AstInterfaceImpl:: AddFunc( SgFunctionDeclaration *d)
   {
     assert(global != 0);
     SgDeclarationStatementPtrList& l = global->get_declarations();
     l.insert( l.begin(), d);
     SgFunctionSymbol* NEW_SYMBOL(f,SgFunctionSymbol, global, d);
     d->set_parent(global);
     return f;
   }

SgMemberFunctionSymbol* AstInterfaceImpl:: 
AddMemberFunc( SgClassDefinition *def, SgMemberFunctionDeclaration *d)
   {
     SgMemberFunctionSymbol* NEW_SYMBOL(f, SgMemberFunctionSymbol, def, d);
     d->set_parent(def);
     return f;
   }

SgVariableSymbol* AstInterfaceImpl::
GetVar( const string& name, SgScopeStatement* loc)
   {
     if (loc == 0)
        loc = scope;
     return LookupVar(name, loc);
   }

SgVariableSymbol* AstInterfaceImpl::
AddVar( SgVariableDeclaration *d, SgScopeStatement* curscope)
   {
     if (curscope == 0)
        curscope = scope;

     d->set_parent(curscope);
     if (!delayNewVarInsert) 
        curscope->insertStatementInScope(d, true);
     else
        newVarList.push_back(std::pair<SgScopeStatement*,SgStatement*>(curscope, d));
     SgInitializedNamePtrList & l = d->get_variables();
     SgVariableSymbol *v = 0;
     for (SgInitializedNamePtrList::iterator p = l.begin(); p != l.end(); ++p) {
         SgInitializedName* cur = *p;
         NEW_SYMBOL(v, SgVariableSymbol, curscope, cur);
     }
     return v;
   }

SgVariableSymbol* AstInterfaceImpl::
AddVar( SgInitializedName *d, SgScopeStatement* curscope)
   {
     if (curscope == 0)
        curscope = scope;
     SgVariableSymbol *NEW_SYMBOL(v, SgVariableSymbol, curscope, d); 
     return v;
   }

//QY:1/3/08: this function is no longer invoked b/c calling EDG front end
// multiple times no longer seems to work.
SgSymbol* AstInterfaceImpl::CreateDeclarationStmts( const string& _decl)
   {
  // DQ (1/2/2007): The use of _astInterface_Tmp.c does not provide a unique filename
  // to support testing of the loop processor in parallel.  This is modified below to
  // make the name unique for each process.
     char uniqueFilename[] = "/tmp/_astInterface_Tmp_XXXXXX.c";
     int fd = mkstemp(uniqueFilename);
     if (fd == -1) {
       perror("mkstemp: ");
       abort();
     }

     write(fd, _decl.c_str(), _decl.size());
     write(fd, "\n", 1);

     int error = 0;
     vector<string> argv;

  // Build the command line (for input to ROSE translator)
     argv.push_back("AddDecl");
     argv.push_back("-rose:verbose");
     argv.push_back("9");
     argv.push_back(uniqueFilename);

  // std::cerr << "generating declaration: \n";
     //SgSourceFile* addDecls = new SgSourceFile(argv, error);
     //AS (10/04/08) using factory function to determine file type. This is due to
     //cleanup of ROSE code
     SgSourceFile* addDecls = isSgSourceFile(determineFileType(argv,error,0));
  // std::cerr << "Finished generating declaration \n";

     unlink( uniqueFilename );
  // string systemString = string("rm '") + uniqueFilename + "'";
  // system( systemString.c_str() );

     SgGlobal *declRoot = addDecls->get_globalScope();
     SgDeclarationStatementPtrList& decls = declRoot->get_declarations ();

     SgDeclarationStatementPtrList localList;
     SgDeclarationStatementPtrList::iterator i = decls.begin();
     while (i != decls.end())
        {
             {
               string filename = (*i)->get_file_info()->get_filename();

            // DQ (8/31/2006): Need to strip off the path since EDG now adds such things automatically (filename normalization to absolute paths).
               filename = StringUtility::stripPathFromFileName(filename);

               if ( filename == uniqueFilename)
                  {
                    (*i)->get_file_info()->setTransformation();
                    (*i)->get_file_info()->setOutputInCodeGeneration();
                    markTransformationsForOutput(*i);

                    localList.push_back(*i);
                 }
           }
          i++;
       }
     SgSymbol* r = AddDecls(this, localList);
     decls.clear();
     return r;
   }

SgVariableSymbol* AstInterfaceImpl::
NewVar( SgType* type, const string& _name, bool makeunique, SgScopeStatement* loc)
{ 
  string varname = _name;
  if (varname == "" ) {
     varname = "_var_";
     makeunique = true;
  }
  if (makeunique) {
     char buf[20]; 
     sprintf(buf, "%d", newVarIndex);
     varname = varname + string(buf);
     ++newVarIndex; 
  }

  SgVariableSymbol *v = GetVar(varname, loc);
  if (v == 0) {
     SgName name(varname.c_str());
     SgType *t = isSgType( type);
     assert(t != 0);
     SgInitializedName *d = new SgInitializedName( name,  t);
     d->set_file_info(GetFileInfo());

     v = AddVar(d, loc);
     if (DebugNewVar())
        cerr << "creating new variable : " << varname << endl;
  }
  return v;
}

SgFunctionSymbol* AstInterfaceImpl::GetFunc( const string& name)
{
  const char* start = name.c_str();
  SgFunctionSymbol* f = LookupFunction(start);
  return f;
}

SgFunctionSymbol* AstInterfaceImpl::
NewFunc( const string& name, SgType*  rtype, const list<SgInitializedName*>& args)
{
  const char *start = name.c_str();
  SgFunctionType *ft = new SgFunctionType(rtype, false);
  SgFunctionDeclaration  *d = new SgFunctionDeclaration(GetFileInfo(), start, ft);
  for (list<SgInitializedName*>::const_iterator p = args.begin(); p != args.end();
       ++p) {
     SgInitializedName* cur = *p;
     d->append_arg(cur);
  }
  return AddFunc(d);
}

SgClassSymbol* AstInterfaceImpl :: GetClass( const string& val, char** start)
{
    string classname = "";
    for ( size_t size = 0 ; size < val.size(); ++size) { 
      if (val[size] == ' ' || val[size] == '&' || val[size] == ':')
           break;
      classname.push_back(val[size]);
    }
    if (start != 0) {
      *start = strstr( val.c_str(), "::");
      *start += 2;
    }

  SgClassSymbol* classSym = LookupClass(classname.c_str());
  if (classSym != 0)
       return classSym;
  return 0;
}

SgClassSymbol* AstInterfaceImpl :: NewClass( const string& classname)
{
 if (DebugSymbol())
    std:: cerr << "adding new class " << classname << "\n";
 SgClassDeclaration *decl = new SgClassDeclaration( GetFileInfo(), classname.c_str());
 
 return AddClass(decl);
}

SgMemberFunctionSymbol * AstInterfaceImpl :: 
NewMemberFunc( SgClassSymbol* c, const string& name, SgType*  rtype, 
               const list<SgInitializedName*>& args)
{
  SgClassDeclaration* classDecl = c->get_declaration();
  const char * start = name.c_str();
  SgClassDefinition* classDefn = GetClassDefn(classDecl);
  if (classDefn == 0) {
     if (DebugSymbol())
         std:: cerr << " creating new class defn " << classDecl->get_name().str() << "when member function " << start << "was not found. \n"; 
     classDefn = new SgClassDefinition(GetFileInfo(), classDecl);
     classDefn->set_endOfConstruct(classDefn->get_file_info());
     classDecl->set_parent(scope);
     classDecl->set_definition(classDefn);
  }

  SgMemberFunctionType *ft = new SgMemberFunctionType(rtype, false);
  SgMemberFunctionDeclaration  *d = new SgMemberFunctionDeclaration(GetFileInfo(), start, ft, 0);
  d->set_scope(classDefn);
  for (list<SgInitializedName*>::const_iterator p = args.begin(); p != args.end();
       ++p) {
     SgInitializedName* cur = *p;
     d->append_arg(cur);
  }

  return AddMemberFunc( classDefn, d);
}

// jichi (10/8/2009): modified from @code globalUnparseToString_OpenMP in @file unparser.h.
// Add in Fortran support by replacing u_exprStmt with u_fortran_locatedNode in roseUnparser.
string
globalUnparseToString_Fortran (const SgNode* astNode, SgUnparse_Info* inputUnparseInfoPointer = NULL )
   {
     // jichi (10/8/2009): Used as dummy source file to deceive Fortran unparser.
     static SgFile* recentFile = NULL;
     if (recentFile == NULL) {
       SgFile* currentFile = SageInterface::getEnclosingFileNode(const_cast<SgNode*>(astNode));
       if (currentFile != NULL)
         recentFile = currentFile;
     }
     // jichi (10/8/2009): Error message when recent file is needed but absent.
     static const char* NullRecentFileErr = "globalUnparseToString_Fortran: Missing source file to unparse Fortran AST.";

  // This global function permits any SgNode (including it's subtree) to be turned into a string

  // DQ (3/2/2006): Let's make sure we have a valid IR node!
     ROSE_ASSERT(astNode != NULL);

     string returnString;

  // all options are now defined to be false. When these options can be passed in
  // from the prompt, these options will be set accordingly.
     bool _auto                         = false;
     bool linefile                      = false;
     bool useOverloadedOperators        = false;
     bool num                           = false;

  // It is an error to have this always turned off (e.g. pointer = this; will not unparse correctly)
     bool _this                         = true;

     bool caststring                    = false;
     bool _debug                        = false;
     bool _class                        = false;
     bool _forced_transformation_format = false;
     bool _unparse_includes             = false;

  // printf ("In globalUnparseToString(): astNode->sage_class_name() = %s \n",astNode->sage_class_name());

     Unparser_Opt roseOptions( _auto,
                               linefile,
                               useOverloadedOperators,
                               num,
                               _this,
                               caststring,
                               _debug,
                               _class,
                               _forced_transformation_format,
                               _unparse_includes );

  // DQ (7/19/2007): Remove lineNumber from constructor parameter list.
  // int lineNumber = 0;  // Zero indicates that ALL lines should be unparsed

     // Initialize the Unparser using a special string stream inplace of the usual file stream 
     ostringstream outputString;

     const SgLocatedNode* locatedNode = isSgLocatedNode(astNode);
     string fileNameOfStatementsToUnparse;
     if (locatedNode == NULL)
        {
       // printf ("WARNING: applying AST -> string for non expression/statement AST objects \n");
          fileNameOfStatementsToUnparse = "defaultFileNameInGlobalUnparseToString";
        }
       else
        {
          ROSE_ASSERT (locatedNode != NULL);

       // DQ (5/31/2005): Get the filename from a traversal back through the parents to the SgFile
       // fileNameOfStatementsToUnparse = locatedNode->getFileName();
       // fileNameOfStatementsToUnparse = ROSE::getFileNameByTraversalBackToFileNode(locatedNode);
          if (locatedNode->get_parent() == NULL)
             {
            // DQ (7/29/2005):
            // Allow this function to be called with disconnected AST fragments not connected to 
            // a previously generated AST.  This happens in Qing's interface where AST fragements 
            // are built and meant to be unparsed.  Only the parent of the root of the AST 
            // fragement is expected to be NULL.
            // fileNameOfStatementsToUnparse = locatedNode->getFileName();
               fileNameOfStatementsToUnparse = locatedNode->getFilenameString();
             }
            else
             {
            // DQ (2/20/2007): The expression being unparsed could be one contained in a SgArrayType
               SgArrayType* arrayType = isSgArrayType(locatedNode->get_parent());
               if (arrayType != NULL)
                  {
                 // If this is an index of a SgArrayType node then handle as a special case
                    fileNameOfStatementsToUnparse = "defaultFileNameInGlobalUnparseToString";
                  }
                 else
                  {
#if 1
                    fileNameOfStatementsToUnparse = ROSE::getFileNameByTraversalBackToFileNode(locatedNode);
#else
                    SgSourceFile* sourceFile = TransformationSupport::getSourceFile(locatedNode);
                    ROSE_ASSERT(sourceFile != NULL);
                    fileNameOfStatementsToUnparse = sourceFile->getFileName();
#endif
                  }
             }
        }

     ROSE_ASSERT (fileNameOfStatementsToUnparse.size() > 0);

  // Unparser roseUnparser ( &outputString, fileNameOfStatementsToUnparse, roseOptions, lineNumber );
     Unparser roseUnparser ( &outputString, fileNameOfStatementsToUnparse, roseOptions );

  // jichi (10/8/2009): Set dummy currentFile to deceive Fortran unparser.
  if (roseUnparser.currentFile == NULL) 
    roseUnparser.currentFile = recentFile;

  // Information that is passed down through the tree (inherited attribute)
  // Use the input SgUnparse_Info object if it is available.
     SgUnparse_Info* inheritedAttributeInfoPointer = NULL;
     if (inputUnparseInfoPointer != NULL)
        {
       // printf ("Using the input inputUnparseInfoPointer object \n");

       // Use the user provided SgUnparse_Info object
          inheritedAttributeInfoPointer = inputUnparseInfoPointer;
        }
       else
        {
       // DEFINE DEFAULT BEHAVIOUR FOR THE CASE WHEN NO inputUnparseInfoPointer (== NULL) IS 
       // PASSED AS ARGUMENT TO THE FUNCTION
       // printf ("Building a new Unparse_Info object \n");

       // If no input parameter has been specified then allocate one
       // inheritedAttributeInfoPointer = new SgUnparse_Info (NO_UNPARSE_INFO);
          inheritedAttributeInfoPointer = new SgUnparse_Info();
          ROSE_ASSERT (inheritedAttributeInfoPointer != NULL);

       // MS: 09/30/2003: comments de-activated in unparsing
          ROSE_ASSERT (inheritedAttributeInfoPointer->SkipComments() == false);

       // Skip all comments in unparsing
          inheritedAttributeInfoPointer->set_SkipComments();
          ROSE_ASSERT (inheritedAttributeInfoPointer->SkipComments() == true);
       // Skip all whitespace in unparsing (removed in generated string)
          inheritedAttributeInfoPointer->set_SkipWhitespaces();
          ROSE_ASSERT (inheritedAttributeInfoPointer->SkipWhitespaces() == true);

       // Skip all directives (macros are already substituted by the front-end, so this has no effect on those)
          inheritedAttributeInfoPointer->set_SkipCPPDirectives();
          ROSE_ASSERT (inheritedAttributeInfoPointer->SkipCPPDirectives() == true);

#if 1
       // DQ (8/1/2007): Test if we can force the default to be to unparse fully qualified names.
       // printf ("Setting the default to generate fully qualified names, astNode = %p = %s \n",astNode,astNode->class_name().c_str());
          inheritedAttributeInfoPointer->set_forceQualifiedNames();

       // DQ (8/6/2007): Avoid output of "public", "private", and "protected" in front of class members.
       // This does not appear to have any effect, because it it explicitly set in the unparse function 
       // for SgMemberFunctionDeclaration.
          inheritedAttributeInfoPointer->unset_CheckAccess();

       // DQ (8/1/2007): Only try to set the current scope to the SgGlobal scope if this is NOT a SgProject or SgFile
          if ( (isSgProject(astNode) != NULL || isSgFile(astNode) != NULL ) == false )
             {
            // This will be set to NULL where astNode is a SgType!
               inheritedAttributeInfoPointer->set_current_scope(TransformationSupport::getGlobalScope(astNode));
             }
#endif
        }

     ROSE_ASSERT (inheritedAttributeInfoPointer != NULL);
     SgUnparse_Info & inheritedAttributeInfo = *inheritedAttributeInfoPointer;

  // DQ (5/27/2007): Commented out, uncomment when we are ready for Robert's new hidden list mechanism.
     if (inheritedAttributeInfo.get_current_scope() == NULL)
        {
       // printf ("In globalUnparseToString(): inheritedAttributeInfo.get_current_scope() == NULL astNode = %p = %s \n",astNode,astNode->class_name().c_str());

       // DQ (6/2/2007): Find the nearest containing scope so that we can fill in the current_scope, so that the name qualification can work.
#if 1
          SgStatement* stmt = TransformationSupport::getStatement(astNode);
#else
          SgStatement* stmt = NULL;
       // DQ (6/27/2007): SgProject and SgFile are not contained in any statement
          if (isSgProject(astNode) == NULL && isSgFile(astNode) == NULL)
               stmt = TransformationSupport::getStatement(astNode);
#endif

          if (stmt != NULL)
             {
               SgScopeStatement* scope = stmt->get_scope();
               ROSE_ASSERT(scope != NULL);
               inheritedAttributeInfo.set_current_scope(scope);
             }
            else
             {
            // DQ (6/27/2007): If we unparse a type then we can't find the enclosing statement, so 
            // assume it is SgGlobal. But how do we find a SgGlobal IR node to use?  So we have to 
            // leave it NULL and hand this case downstream!
               inheritedAttributeInfo.set_current_scope(NULL);
             }
#if 1
          const SgTemplateArgument* templateArgument = isSgTemplateArgument(astNode);
          if (templateArgument != NULL)
             {
            // debugging code!
            // printf ("Exiting to debug case of SgTemplateArgument \n");
            // ROSE_ASSERT(false);

               SgScopeStatement* scope = templateArgument->get_scope();
               printf ("SgTemplateArgument case: scope = %p = %s \n",scope,scope->class_name().c_str());
               inheritedAttributeInfo.set_current_scope(scope);
             }
#endif
       // stmt->get_startOfConstruct()->display("In unparseStatement(): info.get_current_scope() == NULL: debug");
       // ROSE_ASSERT(false);
        }
  // ROSE_ASSERT(info.get_current_scope() != NULL);

  // Turn ON the error checking which triggers an error if the default SgUnparse_Info constructor is called
  // SgUnparse_Info::forceDefaultConstructorToTriggerError = true;

#if 1
  // DQ (10/19/2004): Cleaned up this code, remove this dead code after we are sure that this worked properly
  // Actually, this code is required to be this way, since after this branch the current function returns and
  // some data must be cleaned up differently!  So put this back and leave it this way, and remove the
  // "Implementation Note".

  // Both SgProject and SgFile are handled via recursive calls
     if ( (isSgProject(astNode) != NULL) || (isSgSourceFile(astNode) != NULL) )
        {
       // printf ("Implementation Note: Put these cases (unparsing the SgProject and SgFile into the cases for nodes derived from SgSupport below! \n");

       // Handle recursive call for SgProject
          const SgProject* project = isSgProject(astNode);
          if (project != NULL)
             {
               for (int i = 0; i < project->numberOfFiles(); i++)
                  {
                 // SgFile* file = &(project->get_file(i));
                    SgFile* file = project->get_fileList()[i];
                    ROSE_ASSERT(file != NULL);
                    string unparsedFileString = globalUnparseToString_Fortran(file,inputUnparseInfoPointer);
                 // string prefixString       = string("/* TOP:")      + string(ROSE::getFileName(file)) + string(" */ \n");
                 // string suffixString       = string("\n/* BOTTOM:") + string(ROSE::getFileName(file)) + string(" */ \n\n");
                    string prefixString       = string("/* TOP:")      + file->getFileName() + string(" */ \n");
                    string suffixString       = string("\n/* BOTTOM:") + file->getFileName() + string(" */ \n\n");
                    returnString += prefixString + unparsedFileString + suffixString;
                  }
             }

       // Handle recursive call for SgFile
          const SgSourceFile* file = isSgSourceFile(astNode);
          if (file != NULL)
             {
               SgGlobal* globalScope = file->get_globalScope();
               ROSE_ASSERT(globalScope != NULL);
               returnString = globalUnparseToString_Fortran(globalScope,inputUnparseInfoPointer);
             }
        }
       else
#endif
        {
       // DQ (1/12/2003): Only now try to trap use of SgUnparse_Info default constructor
       // Turn ON the error checking which triggers an error if the default SgUnparse_Info constructor is called
       // GB (09/27/2007): Took this out because it breaks parallel traversals that call unparseToString. It doesn't
       // seem to have any other effect (whatever was debugged with this seems to be fixed now).
       // SgUnparse_Info::set_forceDefaultConstructorToTriggerError(true);

          if (isSgStatement(astNode) != NULL)
             {
               const SgStatement* stmt = isSgStatement(astNode);

            // DQ (2/2/2007): Note that we should modify the unparser to take the IR nodes as const pointers, but this is a bigger job than I want to do now!

               // jichi (10/8/2009): Force using Fortran unparser.
               //roseUnparser.u_exprStmt->unparseStatement ( const_cast<SgStatement*>(stmt), inheritedAttributeInfo );
               if (recentFile != NULL) {
                 ROSE_ASSERT(roseUnparser.u_fortran_locatedNode != NULL);
                 roseUnparser.u_fortran_locatedNode->unparseStatement ( const_cast<SgStatement*>(stmt), inheritedAttributeInfo );
               } else {
                 std::cerr << NullRecentFileErr << std::endl;

                 // jichi (12/9/2009): Use exception to signal parsing failed.
                 //return outputString.str();
                 throw NullRecentFileErr;
               }
             }

          if (isSgExpression(astNode) != NULL)
             {
               const SgExpression* expr = isSgExpression(astNode);

            // DQ (2/2/2007): Note that we should modify the unparser to take the IR nodes as const pointers, but this is a bigger job than I want to do now!

               // jichi (10/8/2009): Force using Fortran unparser.
               //roseUnparser.u_exprStmt->unparseExpression ( const_cast<SgExpression*>(expr), inheritedAttributeInfo );
               if (recentFile != NULL) {
                 ROSE_ASSERT(roseUnparser.u_fortran_locatedNode != NULL);
                 roseUnparser.u_fortran_locatedNode->unparseExpression ( const_cast<SgExpression*>(expr), inheritedAttributeInfo );
               } else {
                 std::cerr << NullRecentFileErr << std::endl;
                 // jichi (12/9/2009): Use exception to signal parsing failed.
                 //return outputString.str();
                 throw NullRecentFileErr;
               }
             }

          if (isSgType(astNode) != NULL)
             {
               const SgType* type = isSgType(astNode);

            // DQ (2/2/2007): Note that we should modify the unparser to take the IR nodes as const pointers, but this is a bigger job than I want to do now!
               roseUnparser.u_type->unparseType ( const_cast<SgType*>(type), inheritedAttributeInfo );
             }

          if (isSgSymbol(astNode) != NULL)
             {
               const SgSymbol* symbol = isSgSymbol(astNode);

            // DQ (2/2/2007): Note that we should modify the unparser to take the IR nodes as const pointers, but this is a bigger job than I want to do now!
               roseUnparser.u_sym->unparseSymbol ( const_cast<SgSymbol*>(symbol), inheritedAttributeInfo );
             }

          // jichi (10/8/2009): Forbid unparse Cxx AST.
          ROSE_ASSERT(isSgSupport(astNode) == NULL);

          if (isSgSupport(astNode) != NULL)
             {
            // Handle different specific cases derived from SgSupport 
            // (e.g. template parameters and template arguments).
               switch (astNode->variantT())
                  {
#if 0
                    case V_SgProject:
                       {
                         SgProject* project = isSgProject(astNode);
                         ROSE_ASSERT(project != NULL);
                         for (int i = 0; i < project->numberOfFiles(); i++)
                            {
                              SgFile* file = &(project->get_file(i));
                              ROSE_ASSERT(file != NULL);
                              string unparsedFileString = globalUnparseToString_OpenMPSafe(file,inputUnparseInfoPointer);
                              string prefixString       = string("/* TOP:")      + string(ROSE::getFileName(file)) + string(" */ \n");
                              string suffixString       = string("\n/* BOTTOM:") + string(ROSE::getFileName(file)) + string(" */ \n\n");
                              returnString += prefixString + unparsedFileString + suffixString;
                            }
                         break;
                       }
#error "DEAD CODE!"
                 // case V_SgFile:
                       {
                         SgFile* file = isSgFile(astNode);
                         ROSE_ASSERT(file != NULL);
                         SgGlobal* globalScope = file->get_globalScope();
                         ROSE_ASSERT(globalScope != NULL);
                         returnString = globalUnparseToString_OpenMPSafe(globalScope,inputUnparseInfoPointer);
                         break;
                       }
#endif
                    case V_SgTemplateParameter:
                       {
                         const SgTemplateParameter* templateParameter = isSgTemplateParameter(astNode);

                      // DQ (2/2/2007): Note that we should modify the unparser to take the IR nodes as const pointers, but this is a bigger job than I want to do now!
                         roseUnparser.u_exprStmt->unparseTemplateParameter(const_cast<SgTemplateParameter*>(templateParameter),inheritedAttributeInfo);
                         break;
                       }
                    case V_SgTemplateArgument:
                       {
                         const SgTemplateArgument* templateArgument = isSgTemplateArgument(astNode);

                      // DQ (2/2/2007): Note that we should modify the unparser to take the IR nodes as const pointers, but this is a bigger job than I want to do now!
                         roseUnparser.u_exprStmt->unparseTemplateArgument(const_cast<SgTemplateArgument*>(templateArgument),inheritedAttributeInfo);
                         break;
                       }

                    case V_SgInitializedName:
                       {
                      // DQ (8/6/2007): This should just unparse the name (fully qualified if required).
                      // QY: not sure how to implement this
                      // DQ (7/23/2004): This should unparse as a declaration (type and name with initializer).
                         const SgInitializedName* initializedName = isSgInitializedName(astNode);
                      // roseUnparser.get_output_stream() << initializedName->get_qualified_name().str();
                         SgScopeStatement* scope = initializedName->get_scope();
                         if (isSgGlobal(scope) == NULL && scope->containsOnlyDeclarations() == true)
                              roseUnparser.get_output_stream() << roseUnparser.u_exprStmt->trimGlobalScopeQualifier ( scope->get_qualified_name().getString() ) << "::";
                         roseUnparser.get_output_stream() << initializedName->get_name().str();
                         break;
                       }

                    case V_Sg_File_Info:
                       {
                      // DQ (8/5/2007): This is implemented above as a special case!
                      // DQ (5/11/2006): Not sure how or if we should implement this
                         break;
                       }

                    case V_SgPragma:
                       {
                         const SgPragma* pr = isSgPragma(astNode);
                         SgPragmaDeclaration* decl = isSgPragmaDeclaration(pr->get_parent());
                         ROSE_ASSERT (decl);
                         roseUnparser.u_exprStmt->unparseStatement ( decl, inheritedAttributeInfo );
                         break;
                       }

                 // Perhaps the support for SgFile and SgProject shoud be moved to this location?
                    default:
                         printf ("Error: default reached in node derived from SgSupport astNode = %s \n",astNode->sage_class_name());
                         ROSE_ABORT();
                }
             }

       // Turn OFF the error checking which triggers an if the default SgUnparse_Info constructor is called
       // GB (09/27/2007): Removed this error check, see above.
       // SgUnparse_Info::set_forceDefaultConstructorToTriggerError(false);

       // MS: following is the rewritten code of the above outcommented 
       //     code to support ostringstream instead of ostrstream.
          returnString = outputString.str();

       // Call function to tighten up the code to make it more dense
          if (inheritedAttributeInfo.SkipWhitespaces() == true)
             {
               returnString = roseUnparser.removeUnwantedWhiteSpace ( returnString );
             }

       // delete the allocated SgUnparse_Info object
          if (inputUnparseInfoPointer == NULL)
               delete inheritedAttributeInfoPointer;
        }
     return returnString;
   }

//! Sage_class_name:unparseToString()
//   e.g: SgPntrArrRefExp:a[i]
//   Don't change the output format since the returned string will be compared to pre-built results 
//   when running tests/roseTests/programAnalysisTests/PtrAnalTest
string AstToString( const AstNodePtr& _s)
{
  // jichi (9/20/2009): Refresh original codes, add fortran support.
  SgNode* s = AstNodePtrImpl(_s).get_ptr();
  if (s == 0) 
    return "";
  string r = "";
  r = string(s->sage_class_name()) + ":";
  switch(s->variantT()) {
  case V_SgVarRefExp:
    {
      SgVarRefExp *var = isSgVarRefExp( s );
      SgVariableSymbol *sb = var->get_symbol();
      r = r +  sb->get_name().str();
    }
    break;
  case V_SgInitializedName:
    {
      SgInitializedName* var = isSgInitializedName(s);
      r = r + var->get_name();
    }
    break;
  default:
    {
      // jichi (10/8/2009): Force using Fortran unparser while parsing Fortran file.
      assert(isSgType(s) == 0);

      if (IS_FORTRAN_LANGUAGE()) {	// Fortran AST
        // jichi (12/9/2009): Use C/C++ unparser as fallback.
        try {
          r = r + globalUnparseToString_Fortran(s);
        } catch (const char*) {
          r = r + s->unparseToString();
        }
      } else	// General AST.
        r = r + s->unparseToString();
    }
  }
  return r;
}

// Return "@line_number:column_number" for an AST node  
// Used for debugging or pretty-printing an node
std::string getAstLocation(const AstNodePtr& _s)
{
   SgNode* s = AstNodePtrImpl(_s).get_ptr();
  if (s == 0) 
     return "";
  string r = "";

  // Add line:column info.
  Sg_File_Info * fileInfo = s->get_file_info();
  stringstream sline, scol;
  sline<<fileInfo->get_line();
  scol<<fileInfo->get_col();
  r = r+"@"+sline.str()+":"+scol.str();
  return r;
}

/*
bool AstInterface::
AstTreeIdentical( const AstNodePtr& n1, const AstNodePtr& n2)
{  
  if (! AstNodeIdentical(n1,n2))
      return false;
   vector<SgNode*> childvec1 = n1->get_traversalSuccessorContainer();
   vector<SgNode*> childvec2 = n2->get_traversalSuccessorContainer();
   if (childvec1.size() != childvec2.size())
       return false;
   for (size_t i = 0; i < childvec1.size(); ++i) {
      AstNodePtr c1 = childvec1[i];
      AstNodePtr c2 = childvec2[i];
      if (AstNodeIdentical(c1, c2))
         return false;
   }
   return true;
}
bool AstInterface:: 
AstNodeIdentical( const AstNodePtr& n1, const AstNodePtr& n2)
{ 
   if ( n1->variantT() != n2->variantT() )
        return false;
   string name1, name2;
   AstNodePtr scope1, scope2;
   if (IsVarRef(n1, 0,&name1, &scope1) && IsVarRef(n2, 0, &name2, &scope2))
       return name1 == name2 && scope1 == scope2; 
   else
       return true;
}
*/

void AstInterface::FreeAstTree( const AstNodePtr& n)
{ }

void NotifyTreeCopy ( AstInterfaceImpl& fa, const AstNodePtr& _orig, const AstNodePtr& _n) 
{
  AstNodePtrImpl orig(_orig), n(_n);
  vector<SgNode*> childvec = orig->get_traversalSuccessorContainer();
  vector<SgNode*> childvec1 = n->get_traversalSuccessorContainer();
  assert( childvec.size() == childvec1.size());
  for (size_t i = 0; i < childvec.size(); ++i) {
      AstNodePtrImpl c = childvec[i], c1 = childvec1[i];
      if (c != c1)
         NotifyTreeCopy( fa, c, c1);
  } 
  CopyAstRecord info(fa, orig, n);
  fa.Notify(info);
};

bool AstInterface :: get_fileInfo(const AstNodePtr& _n, std:: string* fname, int* lineno )
{
  SgNode* n = AstNodePtrImpl(_n).get_ptr();
  Sg_File_Info *f =n->get_file_info(); 
  if (fname == 0) {
     *fname = f->get_filename();
  }
  if (lineno != 0) 
     *lineno = f->get_line();
  return true;
}

AstNodePtr AstInterface :: CopyAstTree( const AstNodePtr &_orig) 
{
 AstNodePtrImpl orig(_orig);
 if (orig->variantT() == V_SgInitializedName) {
   AstNodePtrImpl r(ToVarRef(*this, orig));
   return r;
 }
 SgTreeCopy copyOption;
 SgNode* r = orig->copy( copyOption);
 //QY: FixSgTree(r); 
 if ( impl->NumberOfObservers() )
    NotifyTreeCopy( *impl, _orig, AstNodePtrImpl(r));
 return AstNodePtrImpl(r);
}

AstInterface::AstNodeList AstInterface :: GetChildrenList( const AstNodePtr &_n)
{
  AstNodePtrImpl n(_n);
   vector<SgNode*> childvec = n->get_traversalSuccessorContainer();
   AstNodeList childlist;
   for (size_t i = 0; i < childvec.size(); ++i) {
      AstNodePtrImpl c = childvec[i]; 
      childlist.push_back(c);
   }
   return childlist;
}

bool AstInterface::IsFortranLanguage()
{ return IS_FORTRAN_LANGUAGE(); }

void AstInterface :: SetParent(const AstNodePtr& n, const AstNodePtr& p)
{
  SgNode* node = AstNodePtrImpl(n).get_ptr();
  assert(node != NULL);
  SgNode* parent = AstNodePtrImpl(p).get_ptr();
  node->set_parent(parent);
}

AstNodePtr AstInterface :: GetParent( const AstNodePtr &n)
{
  AstNodePtrImpl node(n);
  if (node == GetRoot())
      return AST_NULL;
  return AstNodePtrImpl(node->get_parent());
}

bool AstInterface:: IsDecls( const AstNodePtr& _s) 
        { 
          AstNodePtrImpl s(_s);
          switch (s->variantT()) {
            case V_SgVariableDeclaration:
	    case V_SgClassDeclaration:
	    case V_SgFunctionDeclaration:
	    case V_SgDeclarationStatement:
	    case V_SgEnumDeclaration:
	    case V_SgTypedefDeclaration: 
	    case V_SgTemplateDeclaration:
		 return true;
	    default:
		 return false;
	    }
	  }
bool AstInterface:: IsStatement( const AstNodePtr& _s)
{
  SgNode* s = AstNodePtrImpl(_s).get_ptr();
  return isSgStatement(s) != 0;
}

bool AstInterface::IsExecutableStmt( const AstNodePtr& _s) 
{ 
  // jichi(9/11/2009): Add in support for fortran loops.
  AstNodePtrImpl s(_s);
  switch (s->variantT()) {
  case V_SgFortranDo:
  //case V_SgFortranNonBlockedDo:	// This kind of Fortran block is temporarily not supported.

  case V_SgForStatement:
  case V_SgCaseOptionStmt:
  case V_SgExprStatement:
  case V_SgExpression:
  case V_SgGotoStatement: 
  case V_SgIfStmt: 
  case V_SgWhileStmt:
  case V_SgDoWhileStmt:
  case V_SgTryStmt:
  case V_SgBreakStmt:
  case V_SgContinueStmt:
  case V_SgReturnStmt:
  case V_SgSpawnStmt:
  //case V_SgVariableDeclaration: 
  // QY: Do not consider vardecl as executable or it will break loopProcessor
    return true;

  case V_SgLabelStatement:
    // jichi (10/9/2009): Disable process of empty Fortran label statement.
    // Mostly the node is replaced from SgContinueStmt paired with FortranDo.
    if (IS_FORTRAN_LANGUAGE()
        && isSgLabelStatement(s.get_ptr())->get_statement() == NULL)
      return false;
    else
      return true;

  default:
    return false;
  }
} 

AstNodePtr AstInterface::GetPrevStmt( const AstNodePtr& s)
{
  SgNode* n = AstNodePtrImpl(s).get_ptr();
   SgNode *p = n->get_parent();
   assert(p != 0);
   vector<SgNode*> childvec = p->get_traversalSuccessorContainer();
   size_t i = 0;
   for (; i < childvec.size(); ++i) 
       if (childvec[i] == n)
           break;
   if (i == 0) 
     return AST_NULL;
   else {
     AstNodePtrImpl r = childvec[i-1];
       return r;    
   }
}

AstNodePtr AstInterface::GetNextStmt( const AstNodePtr& s)
{
   SgNode* n = AstNodePtrImpl(s).get_ptr();
   SgNode *p = n->get_parent();
   assert(p != 0);
   vector<SgNode*> childvec = p->get_traversalSuccessorContainer();
   size_t i = 0;
   for (; i < childvec.size(); ++i)
       if (childvec[i] == n)
           break;
   if (i == childvec.size()-1)
     return AST_NULL;
   else {
     AstNodePtrImpl r = childvec[i+1];
       return r;
   }
}

bool AstInterface::IsIf( const AstNodePtr& _s, AstNodePtr* cond,
				AstNodePtr* truebody, AstNodePtr* falsebody) 
{ 
  SgNode* s = AstNodePtrImpl(_s).get_ptr();
  int t = s->variantT();
  switch (t) {
  case V_SgIfStmt:
    {
      SgIfStmt *is = isSgIfStmt(s);
      if (cond != 0)
	*cond = AstNodePtrImpl(is->get_conditional());
      if (truebody != 0)
	*truebody = AstNodePtrImpl(is->get_true_body());
      if (falsebody != 0)
	*falsebody = AstNodePtrImpl(is->get_false_body());
    }
    break;
  case V_SgCaseOptionStmt:
    {
      SgCaseOptionStmt* cs = isSgCaseOptionStmt(s);
      if (cond != 0)
	*cond = AstNodePtrImpl(cs->get_key());
      if (truebody != 0)
	*truebody = AstNodePtrImpl(cs->get_body());
      if (falsebody != 0)
	*falsebody = AST_NULL;
    }
    break;
  default:
    return false;
  }
  return true;
}

bool AstInterface::IsLabelStatement( const AstNodePtr& _s)
{
  AstNodePtrImpl s(_s);
  return s->variantT() == V_SgLabelStatement;
}

bool AstInterface::
IsReturn(const AstNodePtr& _s, AstNodePtr* val)
{
  SgNode* s = AstNodePtrImpl(_s).get_ptr();
  switch (s->variantT()) {
  case V_SgReturnStmt: 
      if (val != 0) {
          *val = AstNodePtrImpl(isSgReturnStmt(s)->get_expression());
      }
      return true;
  default: return false;
  }
}

bool AstInterface::
IsGoto( const AstNodePtr& _s, AstNodePtr* dest) 
{ 
  // TODO jichi(9/11/2009): Add in support for fortran loops.
  SgNode* s = AstNodePtrImpl(_s).get_ptr();
  switch (s->variantT()) {
  case V_SgGotoStatement:
    if (dest != 0) {
      SgLabelStatement *label = isSgGotoStatement(s)->get_label();
      *dest = AstNodePtrImpl(label); 
    }
    break;
  case V_SgReturnStmt: 
    if (dest != 0) {
      SgNode *scope = 0;
      for (scope = s->get_parent(); 
	   scope->variantT() != V_SgFunctionDefinition;
	   scope = scope->get_parent()){
	assert(scope != 0);
      }
      *dest = AstNodePtrImpl(scope);
    }
    break;
  case V_SgContinueStmt:
    // jichi (10/9/2009): Add in FortranDo support
    if (dest != 0) {
      SgNode* scope = 0;
      for (scope = s->get_parent(); ; scope = scope->get_parent()){
	int t = scope->variantT();
	if (t == V_SgForStatement || t == V_SgWhileStmt || 
	    t == V_SgDoWhileStmt)
	  break;
      }
      if (scope->variantT() == V_SgFortranDo)
        return false;

      if (scope->variantT() == V_SgForStatement)
	scope = isSgForStatement(scope)->get_increment();
      *dest = AstNodePtrImpl(scope);
    }
    break;
  case V_SgBreakStmt:
    if (dest != 0) {
      SgNode* scope = 0;
      for (scope = s->get_parent(); ; scope = scope->get_parent()){
	int t = scope->variantT();
	if (t == V_SgForStatement || t == V_SgWhileStmt || 
	    t == V_SgDoWhileStmt || t == V_SgSwitchStatement)
	  break;
      }
      *dest =  AstNodePtrImpl(scope);
    }
    break;
  default: return false;
  } 
  return true;
}
// goto the point before destination
bool AstInterface::IsGotoBefore( const AstNodePtr& _s)
{ 
  AstNodePtrImpl s(_s);
  switch (s->variantT()) {
  case V_SgGotoStatement:
  case V_SgContinueStmt:
    return true;
  default: return false;
  }
}
bool AstInterface::IsGotoAfter( const AstNodePtr& _s)
{ 
  AstNodePtrImpl s(_s);
  switch (s->variantT()) {
  case V_SgReturnStmt:
  case V_SgBreakStmt:
    return true;
  default: return false;
  }
}

AstNodePtr GetFunctionDecl( const AstNodePtr& _s)
{
    SgNode* s = AstNodePtrImpl(_s).get_ptr();
    int t = s->variantT();
    switch (t) {
    case V_SgFunctionDefinition: 
         return AstNodePtrImpl(isSgFunctionDefinition(s)->get_declaration());
    case V_SgFunctionDeclaration:
    case V_SgMemberFunctionDeclaration:
        return _s;
    case V_SgMemberFunctionRefExp:
         return AstNodePtrImpl(isSgMemberFunctionRefExp(s)->get_symbol()->get_declaration());
    case V_SgFunctionSymbol:
          return AstNodePtrImpl(isSgFunctionSymbol(s)->get_declaration());
    case V_SgFunctionRefExp:
          return AstNodePtrImpl(isSgFunctionRefExp(s)->get_symbol()->get_declaration());
    case V_SgMemberFunctionSymbol:
         return AstNodePtrImpl(isSgMemberFunctionSymbol(s)->get_declaration());
    case V_SgConstructorInitializer:
         return AstNodePtrImpl(isSgConstructorInitializer(s)->get_declaration());
    case V_SgDotExp:
         return GetFunctionDecl( AstNodePtrImpl(isSgDotExp(s)->get_rhs_operand()));
    }
    cerr << "Error: not recognizable function type : " << s->sage_class_name() << endl;
    assert(false);
}

bool AstInterface::
IsFunctionDefinition(  const AstNodePtr& _s, std:: string* name,
                    AstNodeList* params, AstNodeList* outpars, AstNodePtr* body,
                    AstTypeList* paramtype, AstNodeType* returntype) 

{
  SgNode* s = AstNodePtrImpl(_s).get_ptr();
  SgFunctionParameterList *l = 0;
  SgNode* d = s;
  if (s->variantT() ==  V_SgFunctionDefinition) 
    {
      SgFunctionDefinition *def =  isSgFunctionDefinition(s);
      if (body != 0)
	*body = AstNodePtrImpl(def->get_body());
      d = def->get_declaration();
  }
  
  switch (d->variantT()) {
  case V_SgFunctionDeclaration: 
    { 
      SgFunctionDeclaration *decl = isSgFunctionDeclaration(d);
      if (returntype != 0)
	*returntype = AstNodeTypeImpl(decl->get_type()->get_return_type());
      if (name != 0) 
	*name =  string(decl->get_name().str());
      if (paramtype != 0 || params != 0) 
	l = decl->get_parameterList();
      break;
    }
  case V_SgMemberFunctionDeclaration:
    {
      SgMemberFunctionDeclaration* decl = isSgMemberFunctionDeclaration(d);
      if (returntype != 0)
	*returntype = AstNodeTypeImpl(decl->get_type()->get_return_type());
      if (name != 0) {
	SgName cn = decl->get_scope()->get_qualified_name(); 
	SgName fn = decl->get_name();
	*name =  StripGlobalQualifier(string(cn.str())) + "::" + StripGlobalQualifier(string(fn.str()));
      }
      if (paramtype != 0 || params != 0) 
	l = decl->get_parameterList();
      break;
    }
  // Liao, 11/18/2008: add support for instantiated template (member) function declarations  
  case V_SgTemplateInstantiationMemberFunctionDecl: 
  {
     SgTemplateInstantiationMemberFunctionDecl* decl = isSgTemplateInstantiationMemberFunctionDecl(d);
     if (returntype != 0)
	*returntype = AstNodeTypeImpl(decl->get_type()->get_return_type());
      if (name != 0) {
	SgName cn = decl->get_scope()->get_qualified_name(); 
	SgName fn = decl->get_name();
	*name =  StripGlobalQualifier(string(cn.str())) + "::" + StripGlobalQualifier(string(fn.str()));
      }
      if (paramtype != 0 || params != 0) 
	l = decl->get_parameterList();
      break;
  }  
  case V_SgTemplateInstantiationFunctionDecl: 
  {
     SgTemplateInstantiationFunctionDecl* decl = isSgTemplateInstantiationFunctionDecl(d);
     if (returntype != 0)
	*returntype = AstNodeTypeImpl(decl->get_type()->get_return_type());
      if (name != 0) {
	SgName cn = decl->get_scope()->get_qualified_name(); 
	SgName fn = decl->get_name();
	*name =  StripGlobalQualifier(string(cn.str())) + "::" + StripGlobalQualifier(string(fn.str()));
      }
      if (paramtype != 0 || params != 0) 
	l = decl->get_parameterList();
      break;
  }  
  
  default: 
    return false;
  }
  if (l != 0) {
    SgInitializedNamePtrList& names = l->get_args();
    for (SgInitializedNamePtrList::iterator p = names.begin(); 
	 p != names.end(); ++p) {
      SgInitializedName* cur = *p;
      if (paramtype != 0)
         paramtype->push_back(AstNodeTypeImpl(cur->get_type())); 
      if (params != 0)
         params->push_back(AstNodePtrImpl(cur));
      if (outpars != 0 && cur->get_type()->variantT() == V_SgReferenceType)
         outpars->push_back(AstNodePtrImpl(cur));
    }
  }
  return true;
}

//! Check if a node is an assignment statement/expression, grab its lhs and rhs.
//! Use readlhs to tell whether the value of lhs is read before being modified 
//! in the assignment (e.g., whether the assignment is +=, -= etc.)
bool AstInterface::
IsAssignment( const AstNodePtr& _s, AstNodePtr* lhs, AstNodePtr* rhs, bool *readlhs) 
{ 
  SgNode* s = AstNodePtrImpl(_s).get_ptr(); 
  SgExprStatement *n = isSgExprStatement(s);
  SgExpression *exp = (n != 0)? n->get_expression() : isSgExpression(s);
  if (exp != 0) {
    switch (exp->variantT()) {
    case V_SgPlusAssignOp:
    case V_SgMinusAssignOp:
    case V_SgAndAssignOp:
    case V_SgIorAssignOp:
    case V_SgMultAssignOp:
    case V_SgDivAssignOp:
    case V_SgModAssignOp:
    case V_SgXorAssignOp:
    case V_SgAssignOp:
      {
	SgBinaryOp* s2 = isSgBinaryOp(exp);
	if (lhs != 0)
	  *lhs = AstNodePtrImpl(s2->get_lhs_operand());
	if (rhs != 0) {
	  SgNode* init = s2->get_rhs_operand();
	  if ( init->variantT() == V_SgAssignInitializer) 
            init = isSgAssignInitializer(init)->get_operand();
	  *rhs = AstNodePtrImpl(init);
	}
        if (readlhs != 0)
           *readlhs = (exp->variantT() != V_SgAssignOp);
	return true;
      }
    default: return false;
    }
  }
  return false;
}

//! Check if $_s$ is a variable declaration node; 
//! If yes, return the declared variables and their initial values
bool AstInterface:: 
IsVariableDecl(const AstNodePtr& _s, AstNodeList* vars, AstNodeList* init)
{
  SgNode* s = AstNodePtrImpl(_s).get_ptr(); 
   SgVariableDeclaration *decl = isSgVariableDeclaration(s);
   if (decl != 0) {
      if (vars == 0 && init == 0)
         return true;
      SgInitializedNamePtrList& names = decl->get_variables();
      for ( SgInitializedNamePtrList::iterator p = names.begin(); 
            p != names.end(); ++p) {
         SgInitializedName* var = (*p);
         SgExpression* def = var->get_initializer();
         if (def != 0 && def->variantT() == V_SgAssignInitializer)
            def = isSgAssignInitializer(def)->get_operand();
         if (vars != 0)
           vars->push_back(AstNodePtrImpl(var));
         if (init != 0)
            init->push_back(AstNodePtrImpl(def));
     }
     return true;
  }
  return false;
}

AstNodePtr AstInterface:: 
CreateAllocateArray( const AstNodePtr& _arr, const AstNodeType& _elemtype,
               const AstNodeList& indexsize)
{
  AstNodePtrImpl arr(_arr);
  SgType* elemtype = AstNodeTypeImpl(_elemtype).get_ptr();
   SgType* atype = elemtype; 
   for (AstNodeList::const_iterator p = indexsize.begin(); 
        p != indexsize.end(); ++p) {
      SgExpression* exp = isSgExpression(AstNodePtrImpl(*p).get_ptr());
      assert(exp != 0);
      atype = new SgArrayType(atype, exp);
   }
   SgType* baseType = elemtype;
   assert(baseType != NULL);
   SgNewExp* rhs = new SgNewExp(GetFileInfo(), atype, 0, new SgConstructorInitializer(GetFileInfo(),NULL,NULL,baseType,false,false,false,true));
   return CreateAssignment( arr, AstNodePtrImpl(rhs));
}

AstNodePtr AstInterface:: CreateDeleteArray( const AstNodePtr& _arr)
{
  SgNode* arr = AstNodePtrImpl(_arr).get_ptr();
  SgExpression* var = isSgExpression(arr);
  assert(var != 0);
  return AstNodePtrImpl(new SgDeleteExp(GetFileInfo(), var, true));
}

AstNodePtr AstInterface::
CreateLoop( const AstNodePtr& _cond, const AstNodePtr& _body)
{
  SgStatement* cond = isSgStatement(AstNodePtrImpl(_cond).get_ptr());
  assert(cond != 0);
  SgStatement* bstmt = isSgStatement(AstNodePtrImpl(_body).get_ptr());
  assert(bstmt != 0);
  SgBasicBlock* body = isSgBasicBlock(AstNodePtrImpl(_body).get_ptr());
  if (body == 0) {
     NEW_BLOCK1(body,bstmt);
  }
  SgNode* result = new SgWhileStmt(GetFileInfo(), cond, body);
  cond->set_parent(result); body->set_parent(result);
  return AstNodePtrImpl(result);
}

AstNodePtr AstInterface::
CreateAssignment( const AstNodePtr& _lhs, const AstNodePtr& _rhs)
{
  SgNode* lhs = AstNodePtrImpl(_lhs).get_ptr(), *rhs = AstNodePtrImpl(_rhs).get_ptr();
  assert(HasNullParent(lhs));
  assert(HasNullParent(rhs));
  SgExpression *exp = 0;
  SgExpression *lhsexp = ToExpression(*this, _lhs);
  SgType* lhstype = lhsexp->get_type(); 
  SgExpression* rhsexp = ToExpression(*this, _rhs);

  isSgVarRefExp( lhs );
  if (lhstype->variantT() == V_SgClassType) {
    SgClassType *lhstype1 = isSgClassType(lhstype);
    SgName classname = lhstype1->get_name();
    SgClassSymbol *c = impl->GetClass( string(classname.str()));
    assert (c != 0);
    SgExpressionPtrList args;
    args.push_back( rhsexp);
    SgMemberFunctionSymbol *f = GetMemberFunc( impl, c, "operator=", &args);
    if (f != 0) {
        GetClassDefn(c->get_declaration());
        SgMemberFunctionRefExp *NEW_MFUNCTION_REF(fr,f);
        SgExpression *NEW_BIN_OP(func, SgDotExp, lhsexp, fr);
        SgExprListExp *NEW_EXPR_LIST(argexp);
        SgExpressionPtrList &l = argexp->get_expressions();
        l = args;
        NEW_FUNCTION_CALL(exp, func, argexp);
    } 
  }
  if (exp == 0) {
     //QY:1/2/08, removed the type argument b/c ROSE requires this to be NULL 
     //exp = new SgAssignOp(GetFileInfo(), lhsexp, rhsexp, lhstype);
     exp = new SgAssignOp(GetFileInfo(), lhsexp, rhsexp);
     exp->set_endOfConstruct(exp->get_file_info());
     lhsexp->set_parent(exp); 
     rhsexp->set_parent(exp);
  }
  if ( impl->NumberOfObservers() ) {
    CopyAstRecord info(*impl, _rhs, _lhs);
    impl->Notify(info);
  }
  return AstNodePtrImpl(exp);
}

bool AstInterface::
IsIOInputStmt( const AstNodePtr& s, AstNodeList* varlist) 
{ return false; }
bool AstInterface::
IsIOOutputStmt( const AstNodePtr& s, AstNodeList* explist) 
{ return false; }

//! Check if $_exp$ is a single integer constant; if yes, return the constant value in $val$.
bool AstInterface::IsConstInt( const AstNodePtr& _exp, int *val) 
{ 
  SgNode* exp = AstNodePtrImpl(_exp).get_ptr();
  if (exp->variantT() == V_SgIntVal) {
    if (val != 0) 
      *val = isSgIntVal(exp)->get_value();
    return true;
  }
  return false;
}

bool AstInterface::
IsConstant( const AstNodePtr& _exp, string* valtype, string *val)
{
  SgNode* exp = AstNodePtrImpl(_exp).get_ptr();
  switch (exp->variantT()) {
  case V_SgStringVal:
      if (valtype != 0) *valtype = "string";
      break;
  case V_SgCharVal:
  case V_SgWcharVal:
  case V_SgUnsignedCharVal:
      if (valtype != 0) *valtype = "char";
      break;
  case V_SgShortVal:
  case V_SgUnsignedShortVal:
  case V_SgIntVal:
  case V_SgEnumVal:
  case V_SgUnsignedIntVal:
  case V_SgLongIntVal:
  case V_SgLongLongIntVal:
  case V_SgUnsignedLongLongIntVal:
  case V_SgUnsignedLongVal:
      if (valtype != 0) *valtype = "int";
      break;
  case V_SgFloatVal:
      if (valtype != 0) *valtype = "float";
      break;
  case V_SgDoubleVal:
  case V_SgLongDoubleVal:
      if (valtype != 0) *valtype = "double";
      break;
  default:
     return false;
  };
  if (val != 0) {
     assert(isSgType(exp) == 0);
     *val = exp->unparseToString();
  }
  return true;
}

//! Two references are the same if they have the same name and same scope
bool AstInterface::
IsSameVarRef( const AstNodePtr& _n1, const AstNodePtr& _n2)
{
   AstNodePtrImpl n1(_n1), n2(_n2);
   string name1, name2;
   AstNodePtr scope1, scope2;
   if (IsVarRef(n1, 0,&name1, &scope1) && IsVarRef(n2, 0, &name2, &scope2))
       return name1 == name2 && scope1 == scope2;
   return false;
}

bool AstInterface:: IsMin( const AstNodePtr& _exp)
{
   std::string name;
   if (!IsVarRef(_exp, 0, &name, 0, 0))
{
      return false;
}
   if (name == "min" || name == "min2" || name=="min3")
      return true;
   return false;
}

bool AstInterface:: IsMax( const AstNodePtr& _exp)
{
   std::string name;
   if (!IsVarRef(_exp, 0, &name, 0, 0))
      return false;
   if (name=="max" || name=="max2" || name=="max3")
      return true;
   return false;
}

//! Check if $_exp$ is a variable reference (including all name references which may have
//! functions or objects have values)
bool AstInterface::IsVarRef( const AstNodePtr& _exp, AstNodeType* vartype, string* varname,
          AstNodePtr* _scope, bool *isglobal ) 
{ 
  SgNode* exp=AstNodePtrImpl(_exp).get_ptr();
  SgNode *decl = 0;
  switch (exp->variantT()) {
    case V_SgMemberFunctionRefExp: 
      {
         SgMemberFunctionSymbol *sb = isSgMemberFunctionRefExp(exp)->get_symbol();
         SgClassDefinition *cdef = sb->get_scope();
         if (varname != 0) {
            *varname = StripGlobalQualifier(cdef->get_qualified_name())+"::"+StripGlobalQualifier(sb->get_name().str());
         }
         if (vartype != 0)
	   *vartype = AstNodeTypeImpl(sb->get_type());
      }
      break;
  case V_SgFunctionRefExp:
    {
      SgFunctionRefExp *var = isSgFunctionRefExp( exp );
      SgFunctionSymbol *sb = var->get_symbol();
      if (vartype != 0)
	*vartype = AstNodeTypeImpl(sb->get_type());
      if (varname != 0) {
	*varname = sb->get_name().str();
      }
      decl = 0; // sb->get_declaration();
    }
    break;
  case V_SgVarRefExp:
    {
      SgVarRefExp *var = isSgVarRefExp( exp );
      SgVariableSymbol *sb = var->get_symbol();
      if (vartype != 0)
	*vartype = AstNodeTypeImpl(sb->get_type());
      if (varname != 0)
	*varname = sb->get_name().str();
      decl = sb->get_declaration();
    }
     break;
  case V_SgThisExp:
    {
      SgThisExp *var = isSgThisExp( exp );
      if (vartype != 0)
        *vartype = AstNodeTypeImpl(var->get_type());
      if (varname != 0)
        *varname = "this";
      decl = exp;
      do {decl = decl->get_parent();} while (decl->variantT() != V_SgFunctionDefinition);
    }
     break;
  case V_SgConstructorInitializer: 
    {
      SgClassDeclaration *decl = isSgConstructorInitializer(exp)->get_class_decl();
      if (decl != 0) {
        if (varname != 0) {
           *varname = StripGlobalQualifier(decl->get_qualified_name());
           *varname = *varname + "::" + (*varname);
        }
        if (vartype != 0) {
            *vartype = AstNodeTypeImpl(decl->get_type());
        } 
      }
      else return false;
    }
    break;
  case V_SgInitializedName:
    {
      SgInitializedName* var = isSgInitializedName(exp);
      if (var->get_name().str() == 0) {
     std::cerr << "no name for initname " << var->unparseToString() << "\n";
           return false;
      }
      SgType* t = var->get_type();
      assert( t != 0);
      if (vartype != 0)
	*vartype = AstNodeTypeImpl(t);
      if (varname != 0) {
        *varname = var->get_name().str();
      }
      decl = var;
    }
    break;
  case V_SgDotExp:
   {
     SgDotExp *exp1 = isSgDotExp(exp);
     SgVarRefExp* var1 = isSgVarRefExp(exp1->get_lhs_operand());
     SgVarRefExp* var2 = isSgVarRefExp(exp1->get_rhs_operand());
     if (var1 == 0 || var2 == 0)
        return false;
     SgVariableSymbol *sb1 = var1->get_symbol();
     SgVariableSymbol *sb2 = var2->get_symbol();
     if (vartype != 0)
        *vartype = AstNodeTypeImpl(sb2->get_type());
     if (varname != 0)
        *varname = string(sb1->get_name().str()) + "." + string(sb2->get_name().str());
     decl = sb1->get_declaration();
     break;
   }
  default:
    return false;
  }
  if (_scope != 0 || isglobal != 0) {
    SgScopeStatement *scope = (decl == 0)? 0 : GetScope(decl);
    if (_scope != 0)
       *_scope =  AstNodePtrImpl(scope);
    if (isglobal != 0)
       *isglobal = scope == 0 || (scope->variantT() == V_SgGlobal);
  }
  return true;
}

//! Strip the casting operations to get to the real expression.
AstNodePtr SkipCasting(const AstNodePtr & _exp)
{
  SgNode* exp=AstNodePtrImpl(_exp).get_ptr();
  SgCastExp* cast_exp = isSgCastExp(exp);
   if (cast_exp != NULL)
   {
      SgExpression* operand = cast_exp->get_operand();
      assert(operand != 0);
      return SkipCasting(AstNodePtrImpl(operand));
   }
  else      
    return _exp;
}

string AstInterface::GetVarName( const AstNodePtr& _exp)
{
  AstNodePtrImpl exp(_exp);
  string name;
  bool succ = IsVarRef(exp, 0, &name);
  assert(succ);
  return name;
}

AstNodeType AstInterface::GetExpressionType( const AstNodePtr& s)
{
  AstNodeType t;
  if (IsExpression(s, &t) == AST_NULL)
     assert(false);
  return t;
}


string AstInterface:: 
NewVar( const AstNodeType& _type, const string& name, bool makeunique,
        const AstNodePtr& _declLoc, const AstNodePtr& _init)
{
  if (DebugNewVar()) std::cerr << "Enter NewVar:" << name << "\n";
  SgType* type = AstNodeTypeImpl(_type).get_ptr();
  SgNode* declLoc = AstNodePtrImpl(_declLoc).get_ptr();

  if (DebugNewVar()) std::cerr << "declLoc=" << declLoc << "\n";
  SgScopeStatement *scope = (declLoc == 0)? 0 : GetScope(declLoc);
  if (DebugNewVar()) std::cerr << "scope=" << scope << "\n";
  SgVariableSymbol *sb = impl->NewVar( isSgType(type), name, makeunique, scope);
  SgName n =  sb->get_name();
  string varname =  string( n.str());
  SgInitializedName* def = sb->get_declaration();

  if (def != 0 && HasNullParent(def)) {
      //variable declaration has not been inserted
    if (DebugNewVar()) std::cerr << "creating new variable declaration\n";
    SgVariableDeclaration *decl = new SgVariableDeclaration( GetFileInfo());
    decl->set_endOfConstruct(decl->get_file_info());
    def->set_parent(decl);
    SgAssignInitializer *init = 0;
    if (_init != AST_NULL) {
       SgExpression *e = ToExpression( *this, _init);
       NEW_VAR_INIT(init, def, e);
       if (DebugNewVar()) std::cerr << "adding new variable init\n";
    }
    decl->append_variable(def, init);
    impl->AddVar( decl, scope);
    if (DebugNewVar()) std::cerr << "finish creating new variable declaration\n";
  }
  else {
     std::cerr << "Warning: new var has already been initialized: " << name << "\n";
     //assert(_init == AST_NULL);
  }
  if (DebugNewVar()) std::cerr << "Finish creating NewVar:" << name << "\n";
  return varname;
}

void AstInterfaceImpl::
AddNewVarDecls(SgScopeStatement* nblock, SgScopeStatement* oldblock)
    {
      for ( size_t i = newVarList.size(); i > 0; --i) {
            std::pair<SgScopeStatement*, SgStatement*> cur = newVarList[i - 1];
            if (cur.first == oldblock)
               nblock->insertStatementInScope(cur.second, true);
      } 
    }

void AstInterface::
AddNewVarDecls(const AstNodePtr& _nblock, const AstNodePtr& _oldblock)
{
   SgBasicBlock* nblock = isSgBasicBlock(AstNodePtrImpl(_nblock).get_ptr());
   SgBasicBlock* oldblock = isSgBasicBlock(AstNodePtrImpl(_oldblock).get_ptr());
   assert(nblock != 0 && oldblock != 0);
   impl->AddNewVarDecls(nblock,oldblock);
}

void AstInterfaceImpl:: AddNewVarDecls()
   {
      for ( size_t i = newVarList.size(); i > 0; --i) {
            std::pair<SgScopeStatement*, SgStatement*> cur = newVarList[i - 1];
            cur.first->insertStatementInScope(cur.second, true);
      } 
      newVarList.clear();
   } 

// Since 12/25/09, jichi
// This implementation is changed from \fn CreateVarRef below.
bool
AstInterface::HasVarRef(string __varname, const AstNodePtr& __loc) 
{
  SgNode* loc = AstNodePtrImpl(__loc).get_ptr();
  int dotpos = __varname.rfind(".", __varname.size() - 1);
  if (dotpos > 0) {
    string name1 = __varname.substr(0, dotpos);
    string name2 = __varname.substr(dotpos + 1, __varname.size() - dotpos);

    // FIXME (12/25/09): name2 is not checked and used to build a new \a __loc.
    return HasVarRef(name1, __loc);
  }

  SgScopeStatement *scope = (loc == 0)? 0: GetScope(loc);
  SgVariableSymbol *var = impl->GetVar(__varname, scope);
  return (var != 0);
}

AstNodePtr AstInterface::CreateVarRef( string varname, const AstNodePtr& _loc) 
   {
      SgNode* loc = AstNodePtrImpl(_loc).get_ptr();
      int hasdot = varname.rfind(".", varname.size()-1);
      if (hasdot > 0) {
         string name1 = varname.substr(0, hasdot);
         string name2 = varname.substr( hasdot+1, varname.size()-hasdot);
         AstNodePtr obj = CreateVarRef(name1, _loc);
         AstNodeType vartype = GetExpressionType(obj);
         string tname;
         GetTypeInfo(vartype, 0, &tname);
         return CreateBinaryOP( BOP_DOT_ACCESS, obj, CreateConstant( "field", tname + "::" + name2));
      }
      SgScopeStatement *scope = (loc == 0)? 0 : GetScope(loc);
      SgVariableSymbol *sym = impl->GetVar(varname, scope);
      if (sym == 0) {
         cerr << "Error : variable " << varname << " not found in scope " << scope << ". \n";
         assert(false); 
      }
      SgVarRefExp *r = new SgVarRefExp( GetFileInfo(), sym);
      r->set_endOfConstruct(r->get_file_info());
      return AstNodePtrImpl(r);
    }

AstNodeType AstInterface::GetType(const string& name) 
{
  if (name[name.size()-1] == '*') {
    string name1 = name.substr(0, name.size()-1);
    SgType *t = isSgType(AstNodeTypeImpl(GetType(name1)).get_ptr());
    SgPointerType *ptr = t->get_ptr_to();
    if (ptr == 0) {
       ptr = new SgPointerType(t);
       t->set_ptr_to(ptr);
    }
    return AstNodeTypeImpl(ptr);
  }
  else if (name == "char") 
      return AstNodeTypeImpl(new SgTypeChar());
  else if (name == "int")
      return AstNodeTypeImpl(GetTypeInt());
  else if (name == "long")
       return AstNodeTypeImpl(new SgTypeLong());
  else if (name == "void")
       return AstNodeTypeImpl(new SgTypeVoid());
  else if (name == "float")
       return AstNodeTypeImpl(new SgTypeFloat());
  else if (name == "double")
       return AstNodeTypeImpl(new SgTypeDouble());
  else if (name == "string")
       return AstNodeTypeImpl(new SgTypeString());
  else if (name == "bool")
        return AstNodeTypeImpl(new SgTypeBool());
  else {
       SgClassSymbol *c = impl->GetClass(name);
       if (c == 0) {
          cerr << "Error: not recognize type name : " << name << endl;
          assert(false);
       }
       else
          return AstNodeTypeImpl(new SgClassType(c->get_declaration()));
  }
} 

AstNodeType
AstInterface::GetArrayType(const AstNodeType& base, const AstNodeList& index)
{
  if (IsFortranLanguage()) {
    SgType* btype = AstNodeTypeImpl(base).get_ptr();
    SgArrayType* atype = new SgArrayType(btype);

    SgExprListExp*  NEW_EXPR_LIST(dim);
    for (AstNodeList::const_iterator p = index.begin(); p != index.end(); ++p) {
      SgExpression* i = isSgExpression(AstNodePtrImpl(*p).get_ptr());
      assert(i);
      dim->append_expression(i);
      i->set_parent(dim);
    }
    atype->set_dim_info(dim);
    dim->set_parent(atype);

    atype->set_rank(1);
    return AstNodeTypeImpl(atype);

  } else {

    SgType* r = AstNodeTypeImpl(base).get_ptr();
    for (AstNodeList::const_iterator p = index.begin(); p != index.end();
         ++p) {
       if (AstNodePtrImpl(*p)->variantT() != V_SgIntVal) {
          return AstNodeTypeImpl(new SgPointerType(r));
       }
    }
    for (AstNodeList::const_iterator p1 = index.begin(); p1 != index.end();
         ++p1) {
       SgExpression* ie = isSgExpression( AstNodePtrImpl(*p1).get_ptr());
       assert( ie != 0);
       r = new SgArrayType(r, ie);
    }
  
    return AstNodeTypeImpl(r);
  }
}

bool AstInterface::
IsMemoryAccess( const AstNodePtr& _s)
{  
  SgNode* s = AstNodePtrImpl(_s).get_ptr();
  if (IsVarRef(_s) || IsArrayAccess(_s))
    return true;
  switch (s->variantT()) {
  case V_SgPntrArrRefExp:
  case V_SgPointerDerefExp:
     break;
  case V_SgDotExp:
  case V_SgArrowExp:
   {
     if (isSgBinaryOp(s)->get_rhs_operand()->variantT() == V_SgVarRefExp) {
        break;
     }
   }
  default:
    { // Function call returning C++ reference type is a memory access
     AstNodeTypeImpl t;
     if (s->variantT() == V_SgFunctionCallExp && IsExpression(_s,&t) != AST_NULL)
     {
      //member function's return type may have several levels of typedef
      //Strip SgTypedefType off to get the real base type
       SgType* base_type= t.get_ptr();
       assert(base_type!=0);
       while (isSgTypedefType(base_type))
           base_type = isSgTypedefType(base_type)->get_base_type();
       if (base_type->variantT() == V_SgReferenceType)
          break;
     }
     return false;
    }
  } // end switch
  return true;
}

//! Check if _s is an array access.
//If so, store array name in array, and subscripts into index[]
bool AstInterface::
IsArrayAccess( const AstNodePtr& _s, AstNodePtr* array, AstNodeList* index)
{
  SgNode* s = AstNodePtrImpl(_s).get_ptr();
  if (s->variantT() == V_SgPntrArrRefExp) {
      if (index != 0 || array != 0) {
        SgNode* n = s;
        while (true) {
          SgPntrArrRefExp *arr = isSgPntrArrRefExp(n);
          if (arr == 0)
            break;
          n = arr->get_lhs_operand();
          if (array != 0)
            *array = AstNodePtrImpl(n);
          if (index != 0) {
            // jichi(9/25/2009): Add Support for SgExprListExp for Fortran array index.
            SgNode* exp = arr->get_rhs_operand();
            switch(exp->variantT()) {
            case V_SgExprListExp:	// Fortan indices as expression list.
              {
                SgExprListExp* indexexp = isSgExprListExp(exp);
                assert(indexexp);

                SgExpressionPtrList &l = indexexp->get_expressions();
                SgExpressionPtrList::const_iterator p = l.begin(); 
                for ( ;p != l.end(); ++p) {
                  SgExpression *pr = isSgExpression( AstNodePtrImpl(*p).get_ptr() );
                  assert(pr);
                  index->push_back(AstNodePtrImpl(pr));
                }
              }
              break;
            default:
              index->push_back(AstNodePtrImpl(exp));
            }
          }
        }
      }
      return true;
  }
  return false;
}

/*
bool AstInterface::
IsArrayType( const AstNodeType& s, AstNodeType* base)
{
  if (s->variantT() ==  V_SgArrayType) {
      if (base != 0) {
        SgType* n = s;
        while (true) {
          SgArrayType *arr = isSgArrayType(n);
          if (arr == 0)
            break;
          n = arr->get_base_type();
        }
        *base = n;
      }
      return true;
  }
  return false;
}
*/

bool AstInterface::
IsBinaryOp( const AstNodePtr& _exp, OperatorEnum* opr,
            AstNodePtr* opd1, AstNodePtr* opd2)
{ 
  SgNode* exp = AstNodePtrImpl(_exp).get_ptr();
  SgBinaryOp *op = isSgBinaryOp(exp);
  switch (exp->variantT()) {
    case V_SgEqualityOp:
         if (opr != 0)  *opr = BOP_EQ;
         break;
    case V_SgNotEqualOp:
         if (opr != 0)  *opr = BOP_NE;
         break;
    case V_SgGreaterOrEqualOp:
         if (opr != 0)  *opr = BOP_GE;
         break;
    case V_SgLessOrEqualOp:
         if (opr != 0)  *opr = BOP_LE;
         break;
    case V_SgLessThanOp:
         if (opr != 0)  *opr = BOP_LT;
         break;
    case V_SgGreaterThanOp:
         if (opr != 0)  *opr = BOP_GT;
         break;
    case V_SgAndOp:
         if (opr != 0)  *opr = BOP_AND;
         break;
    case V_SgOrOp:
         if (opr != 0)  *opr = BOP_OR;
         break;
    case V_SgMultiplyOp:
         if (opr != 0)  *opr = BOP_TIMES;
         break;
    case V_SgDivideOp:
         if (opr != 0)  *opr = BOP_DIVIDE; 
         break;
    case V_SgModOp:
         if (opr != 0)  *opr = BOP_MOD; 
         break;
    case V_SgAddOp:
         if (opr != 0)  *opr = BOP_PLUS; 
         break;
    case V_SgSubtractOp:
         if (opr != 0)  *opr = BOP_MINUS; 
         break;
    case V_SgDotExp:
        if (opr != 0)  *opr = BOP_DOT_ACCESS;
        break;
    case V_SgArrowExp:
        if (opr != 0)  *opr = BOP_ARROW_ACCESS;
        break;
   case V_SgBitOrOp:
        if (opr != 0)  *opr = BOP_BIT_OR;
        break;
   case V_SgBitAndOp:
        if (opr != 0)  *opr = BOP_BIT_AND;
        break;
   case V_SgRshiftOp:
        if (opr != 0)  *opr = BOP_BIT_RSHIFT;
        break;
   case V_SgLshiftOp:
        if (opr != 0)  *opr = BOP_BIT_LSHIFT;
        break;
    default:
       return false;
  }
  if (opd1 != 0) *opd1 = AstNodePtrImpl(op->get_lhs_operand()); 
  if (opd2 != 0) *opd2 = AstNodePtrImpl(op->get_rhs_operand()); 
  return true;
}

//! Check if $_exp$ is an unary operation; if yes, return its operation type and operand 
bool AstInterface::
IsUnaryOp( const AstNodePtr& _exp, OperatorEnum* opr, AstNodePtr* opd) 
{ 
  SgNode* exp = AstNodePtrImpl(_exp).get_ptr();
  switch (exp->variantT()) {
    case V_SgMinusOp: 
       if (opd != 0) *opd = AstNodePtrImpl(isSgMinusOp(exp)->get_operand()); 
       if (opr != 0)  { *opr = UOP_MINUS;  }
       return true;
    case V_SgAddressOfOp :
       if (opd != 0) *opd = AstNodePtrImpl(isSgAddressOfOp(exp)->get_operand()); 
       if (opr != 0) *opr = UOP_ADDR; 
       return true;
    case V_SgPointerDerefExp: 
       if (opr != 0) *opr = UOP_DEREF; 
       if (opd != 0) *opd = AstNodePtrImpl(isSgPointerDerefExp(exp)->get_operand()); 
       return true;
    case V_SgNewExp:
       if (opr != 0) *opr = UOP_ALLOCATE; 
       if (opd != 0) *opd = AstNodePtrImpl(isSgNewExp(exp)->get_constructor_args());
       return true;
    case V_SgCastExp:
       if (opr != 0) *opr = UOP_CAST; 
       if (opd != 0) *opd = AstNodePtrImpl(isSgCastExp(exp)->get_operand()); 
       return true;
    case V_SgMinusMinusOp:
        if (opr != 0) *opr = UOP_DECR1; 
        if (opd != 0) *opd = AstNodePtrImpl(isSgMinusMinusOp(exp)->get_operand());
        return true;
    case V_SgPlusPlusOp:
        if (opr != 0) *opr = UOP_INCR1; 
        if (opd != 0) *opd = AstNodePtrImpl(isSgPlusPlusOp(exp)->get_operand());
        return true;
    case V_SgAsmOp:
        if (opd != 0) *opd = AstNodePtrImpl(isSgAsmOp(exp)->get_expression());
        if (opr != 0) *opr = OP_UNKNOWN; 
        return true;
    default: 
       return false;
  }
}

bool AstInterface::IsBlock( const AstNodePtr& _exp)
{  
  AstNodePtrImpl exp(_exp);
  switch (exp->variantT()) {
  case V_SgBasicBlock:
  case V_SgSwitchStatement:
  case V_SgForInitStatement:
    return true;
  default: break;
  };
  return false;
}

//! Check if $s$ is a function call; if yes, return the function and arguments
bool AstInterfaceImpl::
IsFunctionCall( SgNode* s, SgNode** func, AstNodeList* args)
{
  SgNode *exp = s;
  SgNode *f = 0;
  SgExprListExp *argexp = 0;
  
  switch (exp->variantT()) {
  case V_SgExprStatement:
     exp = isSgExprStatement(exp)->get_expression();
     return IsFunctionCall(exp, func, args);
  case V_SgAssignInitializer:
    exp = isSgAssignInitializer(exp)->get_operand();
    return IsFunctionCall(exp, func, args);
  case V_SgFunctionCallExp:
    {
      SgFunctionCallExp *fs = isSgFunctionCallExp(exp);
      f = fs->get_function(); //Should be SgFunctionRefExp
      argexp = fs->get_args(); // SgExprListExp
    }
    break;
  case V_SgConstructorInitializer: {
     SgConstructorInitializer* isinit_exp = isSgConstructorInitializer(exp);
    if (isinit_exp->get_class_decl() != 0 && isinit_exp->get_declaration() != 0) {
       f = exp;
       argexp =  isSgConstructorInitializer(exp)->get_args();
    }
    else return false;
    } 
    break;
  default:
    return false;
  }
  
  switch (f->variantT()) {
  case V_SgDotExp: 
      { 
        SgDotExp* dot = isSgDotExp(f);
        AstNodePtrImpl cur = dot->get_lhs_operand();
        f = dot->get_rhs_operand();
        if (args != 0)
          args->push_back( cur ); 
      }
      break;
  case V_SgArrowExp:
      { 
        SgArrowExp* arrow = isSgArrowExp(f);
        AstNodePtrImpl cur = arrow->get_lhs_operand();
        f = arrow->get_rhs_operand();
        if (args != 0)
          args->push_back( cur ); 
      }
      break;
  case V_SgMemberFunctionRefExp:
      {
        if (args != 0)
          args->push_back(AST_NULL);
         break;
      }
  default: break;
  }
  if (argexp != 0) {
     SgExpressionPtrList l = argexp->get_expressions();
     for ( SgExpressionPtrList::iterator p = l.begin(); p != l.end(); ++p) {
       if (args != 0)  {
         args->push_back(AstNodePtrImpl(*p)); 
       }
     }
  }
  if (func != 0)
    *func = f;
  return true;
}

//! Check if $_s$ is a function call; if yes, return its function, input arguments,
//! output arguments (arguments passed by reference in C++), parameter types, return type
bool AstInterface::
IsFunctionCall( const AstNodePtr& _s, AstNodePtr* fname, AstNodeList* args, 
                AstNodeList* outargs, AstTypeList* paramtypes, AstNodeType* returntype)
{
  AstNodePtrImpl s(_s);
  AstNodeList Args;
  if (outargs != 0 && args == 0)
      args = &Args;
  SgNode* f;
  // Grab functionRefExp and argument expression list
  if (!impl->IsFunctionCall(s.get_ptr(), &f, args))
     return false;
     
  if (f->variantT() == V_SgPointerDerefExp)
     f = isSgPointerDerefExp(f)->get_operand();
  if (fname != 0) {
    *fname = AstNodePtrImpl(f);
  }
  if (outargs != 0 || paramtypes != 0 || returntype != 0) {
     AstTypeList PTlist;
     if (paramtypes == 0)
         paramtypes = &PTlist;
     AstNodeType _ftype;
     if (!IsVarRef(AstNodePtrImpl(f), &_ftype))
        assert(false);
     SgType* t = AstNodeTypeImpl(_ftype).get_ptr();
     if (t->variantT() == V_SgPointerType)
        t = static_cast<SgPointerType*>(t)->get_base_type();
     SgFunctionType* ftype = isSgFunctionType(t);
     if (ftype != 0) {
        SgTypePtrList atypes = ftype->get_arguments();
        for (SgTypePtrList::const_iterator p = atypes.begin(); p != atypes.end(); ++p) {
           paramtypes->push_back(AstNodeTypeImpl(*p));
        }
        if (returntype != 0)
           *returntype = AstNodeTypeImpl(ftype->get_return_type());
     }
     else { // not a function type
        AstNodePtr fdecl = GetFunctionDecl(AstNodePtrImpl(f));
        if (fdecl == 0) {
            std::cerr << "func has no decl: " << AstToString(s) << "\n";
           assert(0);
        }
        if (!IsFunctionDefinition(fdecl, 0,0,0,0,paramtypes,returntype))
         assert(false);
     }
     // Store arguments of reference types into outargs
     if (outargs != 0) {
        AstNodeList::const_iterator p1 = args->begin();
        for (AstTypeList::const_iterator p = paramtypes->begin(); 
             p != paramtypes->end(); ++p,++p1) {
           SgType* t = AstNodeTypeImpl(*p).get_ptr();
           if (t->variantT() == V_SgReferenceType)
              outargs->push_back(*p1); 
        }
     }
  } 
  return true;
}



SgNode* AstInterfaceImpl::GetVarDecl( const string& varname)
{
  SgVariableSymbol* s = GetVar(varname);
  SgInitializedName* n = s->get_declaration();
  SgNode *decl = n;
  while (decl != 0 && decl->variantT() != V_SgVariableDeclaration) { 
     if (decl->variantT() == V_SgFunctionDeclaration) {
         decl = isSgFunctionDeclaration(decl)->get_definition();
         break;
     }
    decl = decl->get_parent();
  }
  return decl;
}

void AstInterfaceImpl::
GetTypeInfo(const AstNodeType& _t, string *tname, string* stripname, int* size)
{
  SgType* t = AstNodeTypeImpl(_t).get_ptr();
  std::string typeName = get_type_name(t);
  // for instantiated template types, return the original template type name
  // TODO: need a better way to handle this
  if (isSgClassType(t))
  {
    SgDeclarationStatement * decl = isSgClassType(t)->get_declaration();
    SgTemplateInstantiationDecl* insDecl= isSgTemplateInstantiationDecl(decl);
    if (insDecl)
      typeName=insDecl->get_templateDeclaration()->get_qualified_name();
  }

  string r1 = StripGlobalQualifier(typeName);
  string result = "";
  for (size_t i = 0; i < r1.size(); ++i) {
    if (r1[i] != ' ')
      result.push_back(r1[i]);
    else if (i + 2 < r1.size() && r1[i+1]==':' && r1[i+2]==':') {
        i+= 2; 
    }
  }
  if (tname != 0) {
    *tname = result;
  }
  if (stripname != 0)
    *stripname = StripParameterType(result);
  if (size != 0)
    *size = 4;
}

void AstInterface::
GetTypeInfo(const AstNodeType& t, string *tname, string* stripname, int* size)
{ AstInterfaceImpl::GetTypeInfo(t, tname, stripname, size); }

bool
AstInterface::IsPointerType(const AstNodeType& __type)
{
  AstNodeTypeImpl type(__type);
  return type.get_ptr()->variantT() == V_SgPointerType;
}

bool
AstInterface::IsArrayType(const AstNodeType& __type, int* __dim,
                          AstNodeType* __base_type)
{
  AstNodeTypeImpl type(__type);
  SgArrayType* t = isSgArrayType(type.get_ptr());
  if (!t)
    return false;

  if (__base_type)
    (*__base_type) = AstNodeTypeImpl(t->get_base_type());
  if (__dim)
    (*__dim) = t->get_rank();
  return true;
}

bool
AstInterface::IsScalarType(const AstNodeType& __type)
{
  AstNodeTypeImpl type(__type);
  switch(type->variantT()) {
  case V_SgTypeChar :
  case V_SgTypeSignedChar :
  case V_SgTypeUnsignedChar :
  case V_SgTypeShort :
  case V_SgTypeSignedShort :
  case V_SgTypeUnsignedShort :
  case V_SgTypeInt :
  case V_SgTypeSignedInt :
  case V_SgTypeUnsignedInt :
  case V_SgTypeLong :
  case V_SgTypeSignedLong :
  case V_SgTypeUnsignedLong :
  case V_SgTypeVoid :
  case V_SgTypeWchar:
  case V_SgTypeFloat:
  case V_SgTypeDouble:
  case V_SgTypeLongLong:
  case V_SgTypeUnsignedLongLong:
  case V_SgTypeLongDouble:
  case V_SgTypeString:
  case V_SgTypeBool:
  case V_SgTypeComplex:
  case V_SgTypeImaginary:
     return true;
  default: break;
  }
  return false;
}

bool AstInterface::GetArrayBound( const AstNodePtr& _arrayref, int dim, int &lb, int &ub) 
    { 
      AstNodePtrImpl arrayref(_arrayref);
      SgNode *n = arrayref.get_ptr();
      SgArrayType *t = 0;
      if (dim == 0) {
         SgVarRefExp *var = isSgVarRefExp( n );
         if (var == 0)
             return false;
         SgVariableSymbol *sb = var->get_symbol();
         SgType* vt = sb->get_type(); 
         t = isSgArrayType( vt);
      }
      else {
         for (int i = 0; i < dim; ++i) {
           n = n->get_parent();
         }
         SgPntrArrRefExp *ar = isSgPntrArrRefExp(n);
         if (ar == 0)
            return false;
         t = isSgArrayType(ar->get_type());
      }
      if (t == 0)
          return false;
      SgExpression *exp = t->get_index();
      lb = 0;
      return IsConstInt( AstNodePtrImpl(exp), &ub);
    }

//! Check whether $_s$ is an expression; If yes, return the expression 
// (strip off SgExpressionRoot) and grab its type
AstNodePtr AstInterface::
IsExpression( const AstNodePtr& _s, AstNodeType* exptype)
{
  AstNodePtrImpl s(_s);
  if (IsVarRef(s, exptype))
     return s;
  SgExpression* exp = isSgExpression(s.get_ptr());
  if (exp != 0) {
      switch (exp->variantT()) {
      case V_SgExpressionRoot:
           exp = isSgExpressionRoot(exp)->get_operand();
           break;
      default: break;
      }
    //std::cerr << "IsExpresssion: " << exp->sage_class_name() << "\n";
   if (exptype != 0)
       *exptype = AstNodeTypeImpl(exp->get_type());
    return AstNodePtrImpl(exp);
  }
  return AST_NULL;
}

// Check whether $_s$ is a loop; if yes, grab init, condition, increment, and body
bool AstInterface::
IsLoop( const AstNodePtr& _s, AstNodePtr* init, AstNodePtr* cond,
	AstNodePtr* incr, AstNodePtr* body)
{
  AstNodePtrImpl s(_s);
  switch (s->variantT()) {
  case V_SgForStatement:
    {
      SgForStatement *f = isSgForStatement(s.get_ptr());
      if (init != 0) {
	SgForInitStatement* pinit = f->get_for_init_stmt(); 
        if (pinit != 0 && pinit->get_init_stmt().size() == 0)
           pinit = 0;
	*init = AstNodePtrImpl(pinit);
      }
      if (incr != 0)
	*incr = AstNodePtrImpl(f->get_increment());
      if (cond != 0)
	*cond = AstNodePtrImpl(f->get_test_expr());
      if (body != 0)
	*body = AstNodePtrImpl(f->get_loop_body());
    }
    break;
  case V_SgWhileStmt:
    {
      SgWhileStmt* w = isSgWhileStmt(s.get_ptr());
      if (init != 0)
	*init = AST_NULL;
      if (incr != 0)
	*incr = AST_NULL;
      if (cond != 0)
	*cond = AstNodePtrImpl(w->get_condition());
      if (body != 0)
	*body = AstNodePtrImpl(w->get_body());
    }
    break;
  case V_SgDoWhileStmt:
    {
      SgDoWhileStmt *w = isSgDoWhileStmt(s.get_ptr());
      if (init != 0)
	*init = AST_NULL;
      if (incr != 0)
	*incr = AST_NULL;
      if (cond != 0)
	*cond = AstNodePtrImpl(w->get_condition());
      if (body != 0)
	*body = AstNodePtrImpl(w->get_body());
    }
    break;

  case V_SgFortranDo:
    // jichi (9/11/2009): Add in fortran loop recognition support
    // FIXME: increment/bound in fortran are not equivalent to incr/cond in Cxx.
    {
      SgFortranDo *f = isSgFortranDo(s.get_ptr());
      if (init != 0)
	*init = AstNodePtrImpl(f->get_initialization());
      if (incr != 0)
	*incr = AstNodePtrImpl(f->get_increment());
      if (cond != 0)
	*cond = AstNodePtrImpl(f->get_bound());
      if (body != 0)
	*body = AstNodePtrImpl(f->get_body());
    }
    break;

  default:
    return false;
  }
  return true;
}

//! Check whether $_s$ is a loop that can be easily converted to the FORTRAN style.
// The loop must be in the format: for (ivar=lb; ivar <= ub; ivar += step)
// The NormalizeForLoop function can be invoked to convert some loops to this style.
bool AstInterface::IsFortranLoop( const AstNodePtr& _s, AstNodePtr* ivar , AstNodePtr* lb , AstNodePtr* ub, AstNodePtr* step, AstNodePtr* body)
{ 
  AstNodePtrImpl s(_s);

  switch (s->variantT()) {
  // jichi (9/11/2009): Add in fortran loop recognition support.
  case V_SgFortranDo:
    {
      SgFortranDo *f = isSgFortranDo(s.get_ptr());

      // parse initialization for ivar and lb
      SgExpression *initialization = f->get_initialization();
      AstNodePtrImpl ivarast, lbast;
      AstInterface::IsAssignment( AstNodePtrImpl(initialization), &ivarast, &lbast);

      // Use following statements to output varname
      //string __varname;
      //AstInterface::IsVarRef(ivarast, 0, &__varname);
      //std::cerr << "varname = " << __varname << std::endl;
        
      if (ivar != 0)
        *ivar = ivarast;
      if (lb != 0)
        *lb = lbast;
      if (ub != 0)
        *ub = AstNodePtrImpl(f->get_bound());

      // jichi (10/9/2009): Default increment should be 1 in Fortran,
      // which is not considered here, but processed in NomalizeLoop.
      if (step != 0)
        *step = AstNodePtrImpl(f->get_increment());

      if (body != 0)
        *body = AstNodePtrImpl(f->get_body());
    }
    return true;

  // jichi (9/11/2009): Reformat original function.
  case V_SgForStatement:
    {
      SgForStatement *fs = isSgForStatement(s.get_ptr());
  
      // Must have initialization statements
      SgStatementPtrList &init = fs->get_init_stmt();
      if (init.size() != 1)
        return false;
      
      AstNodePtrImpl ivarast, lbast, ubast, stepast;
      if (!AstInterface::IsAssignment( AstNodePtrImpl(init.front()), &ivarast, &lbast))
        return false;
        
      string varname;
      if (!AstInterface::IsVarRef(ivarast, 0, &varname))
        return false; 
  
      SgExpression* test = fs->get_test_expr();
      int t = test->variantT();
      switch (t) {
      case V_SgLessOrEqualOp:
      case V_SgGreaterOrEqualOp:
      case V_SgNotEqualOp:
        break;
      default:  
        return false;
      }
  
      AstNodePtrImpl testlhs = isSgBinaryOp(test)->get_lhs_operand();
      string testvarname;
      if (!AstInterface::IsVarRef(SkipCasting(testlhs), 0, &testvarname) ||
           varname != testvarname)
        return false;
  
      ubast = isSgBinaryOp(test)->get_rhs_operand();
      SgExpression* incr = fs->get_increment();
      switch (incr->variantT()) {
      case V_SgPlusAssignOp:
        break;
      default:
        return false;
       }
  
      AstNodePtrImpl incrlhs = isSgBinaryOp(incr)->get_lhs_operand();
      string incrvarname;
      if ( !AstInterface::IsVarRef(SkipCasting(incrlhs), 0, &incrvarname) ||
          varname != incrvarname) 
        return false;
      stepast = isSgBinaryOp(incr)->get_rhs_operand();
      if (ivar != 0)
        *ivar = ivarast;
      if (lb != 0)
        *lb = lbast;
      if (ub != 0)
        *ub = ubast;
      if (step != 0)
        *step = stepast;
      if (body != 0)
        *body = AstNodePtrImpl(fs->get_loop_body());
          
    }
    return true;

  default:
    return false; 
  }
}

bool AstInterface::IsPostTestLoop( const AstNodePtr& _s)
{
  AstNodePtrImpl s(_s);
  switch (s->variantT()) {
  case V_SgDoWhileStmt:
    return true;
  default: break;
  }
  return false;
}

AstNodePtr AstInterface::
CreateLoop( const AstNodePtr& _ivar, const AstNodePtr& _lb, const AstNodePtr& _ub,
	    const AstNodePtr& _step, const AstNodePtr& _stmts, bool decrementIvar)
{ 
  // jichi(9/11/2009): Add in support for SgFortranDo.
  if (IS_FORTRAN_LANGUAGE()) { // Generate fortran loop.
    AstNodePtrImpl ivar(_ivar), lb(_lb), ub(_ub), step(_step), stmts(_stmts);
    assert(lb != 0);

    // Create new loop.
    SgFortranDo *result = new SgFortranDo(GetFileInfo());
    result->set_endOfConstruct(result->get_file_info());

    // Set loop expressions.
    SgExpression* ivarexp = ToExpression(*this, ivar);
    SgExpression* lbexp = ToExpression(*this, lb);
    SgExpression* ubexp = ToExpression(*this, ub);
    SgExpression* stepexp = ToExpression(*this, step);

    AstNodePtrImpl init = CreateAssignment(AstNodePtrImpl(ivarexp), AstNodePtrImpl(lbexp));
    SgExpression *initexp = isSgExpression(init.get_ptr());
    assert(initexp);
    ivarexp->set_parent(initexp);
    ubexp->set_parent(initexp);

    result->set_initialization(initexp);
    initexp->set_parent(result);

    result->set_bound(ubexp);
    ubexp->set_parent(result);

    result->set_increment(stepexp);
    stepexp->set_parent(result);
    
    // set loop body
    SgStatement* stmtptr = ToStatement(stmts.get_ptr());
    assert(stmtptr != 0);
    SgBasicBlock *b = isSgBasicBlock(stmtptr);

    if (b == 0) {
      NEW_BLOCK1(b, stmtptr);
    } else
      assert( HasNullParent(b));

    result->set_body(b);
    b->set_parent(result);

    //TODO? result->set_end_numeric_label(...);
    result->set_has_end_statement(true);
    result->set_parent(GetNullScope());
    return AstNodePtrImpl(result);

  } else {
    // default: Generate c/c++ loop.
    // There are also other languages rather than c/c++ and fortran.
    // See rose source 'sageInterface.h' for details.
    // Codes reformatted from original function.
    AstNodePtrImpl ivar(_ivar), lb(_lb), ub(_ub), step(_step), stmts(_stmts);
    SgForStatement *result = new SgForStatement(GetFileInfo());
    result->set_endOfConstruct(result->get_file_info());
    SgExpression* ivarexp = ToExpression(*this, ivar);
    SgExpression* lbexp = (lb == 0)? 0 : ToExpression(*this, lb);
    SgExpression* ubexp = ToExpression(*this, ub);
    SgExpression* stepexp = ToExpression(*this, step);
    AstNodePtrImpl init = (lbexp == 0)? AST_NULL : CreateAssignment(AstNodePtrImpl(ivarexp), AstNodePtrImpl(lbexp));
    SgStatement *initstmt = (init == AST_NULL)? 0 : isSgStatement(init.get_ptr());
    if (initstmt == 0 && init != 0) {
      SgExpression *initexp = ToExpression(*this, init);
      NEW_EXPR_STMT(initstmt, initexp);
    }
    if (initstmt != 0) {
      result->append_init_stmt( initstmt );
      initstmt->set_parent(result->get_for_init_stmt());
    }
    SgExpression* ivarexp1 = isSgExpression(AstNodePtrImpl(CopyAstTree(AstNodePtrImpl(ivarexp))).get_ptr());
    if (decrementIvar) {
      assert( HasNullParent(ubexp));
  
      SgExpression* testExp = new SgGreaterOrEqualOp( GetFileInfo(), ivarexp1, ubexp);
      ivarexp1->set_parent(testExp); ubexp->set_parent(testExp);
      SgExprStatement* NEW_EXPR_STMT(test,testExp);
      result->set_test(test); test->set_parent(result);

    } else { 
      assert( HasNullParent(ubexp));
  
      SgExpression* NEW_BIN_OP(testExp,SgLessOrEqualOp, ivarexp1, ubexp);
      SgExprStatement* NEW_EXPR_STMT(test,testExp);
      result->set_test(test); test->set_parent(result);
    }
  
    SgExpression* ivarexp2 = isSgExpression(AstNodePtrImpl(CopyAstTree(AstNodePtrImpl(ivarexp))).get_ptr());
    assert( HasNullParent(stepexp));
    SgPlusAssignOp *NEW_BIN_OP(incr,SgPlusAssignOp, ivarexp2, stepexp);
    result->set_increment(incr); incr->set_parent(result);
    SgStatement* stmtptr = ToStatement(stmts.get_ptr());
    assert(stmtptr != 0);
    SgBasicBlock *b = isSgBasicBlock(stmtptr);

    if (b == 0) {
      NEW_BLOCK1(b, stmtptr);
    } else
      assert( HasNullParent(b));

    result->set_loop_body(b);
    b->set_parent(result);
    result->set_parent(GetNullScope());
    return AstNodePtrImpl(result);
  }
}
    
    
AstInterface::AstNodeList AstInterface::GetBlockStmtList( const AstNodePtr& _n)
{
  AstNodePtrImpl n(_n);
  AstNodeList result;
  SgStatementPtrList l;
  switch (n->variantT()) {
  case V_SgBasicBlock:
    l = isSgBasicBlock(n.get_ptr())->get_statements();
    break;
  case V_SgForInitStatement:
    l = isSgForInitStatement(n.get_ptr())->get_init_stmt();
    break;
  case V_SgSwitchStatement:
       result.push_back(AstNodePtrImpl(isSgSwitchStatement(n.get_ptr())->get_body()));
       return result;
  default:  
      assert(false);
  }
  for (SgStatementPtrList::iterator p = l.begin(); p != l.end(); ++p) {
     result.push_back(AstNodePtrImpl(*p));
  }
  return result;
}

int AstInterface::GetBlockSize( const AstNodePtr& _n)
{
  AstNodePtrImpl n(_n);
  SgStatementPtrList l;
  switch (n->variantT()) {
  case V_SgBasicBlock:
    l = isSgBasicBlock(n.get_ptr())->get_statements();
    break;
  case V_SgForInitStatement:
    l = isSgForInitStatement(n.get_ptr())->get_init_stmt();
    break;
  case V_SgSwitchStatement:
       return 1;
  default:
      assert(false);
  }
  return l.size();
}
                                                                                                 

AstNodePtr AstInterface::GetBlockFirstStmt( const AstNodePtr& _n)
{
  AstNodePtrImpl n(_n);
  SgStatementPtrList l;
  switch (n->variantT()) {
  case V_SgBasicBlock:
    l = isSgBasicBlock(n.get_ptr())->get_statements();
    break;
  case V_SgForInitStatement:
    l = isSgForInitStatement(n.get_ptr())->get_init_stmt();
    break;
  case V_SgSwitchStatement:
       return AstNodePtrImpl(isSgSwitchStatement(n.get_ptr())->get_body());
  default:  
      assert(false);
  }
  return (l.size() == 0)? AST_NULL : AstNodePtrImpl(l.front());
}

AstNodePtr AstInterface::GetBlockLastStmt( const AstNodePtr& _n)
{
  AstNodePtrImpl n(_n);
  SgStatementPtrList l;
  switch (n->variantT()) {
  case V_SgBasicBlock:
    l = isSgBasicBlock(n.get_ptr())->get_statements();
    break;
  case V_SgForInitStatement:
    l = isSgForInitStatement(n.get_ptr())->get_init_stmt();
    break;
  case V_SgSwitchStatement:
       return AstNodePtrImpl(isSgSwitchStatement(n.get_ptr())->get_body());
  default:  
      assert(false);
  }
  if (l.size() > 0)
     return AstNodePtrImpl(l.back());
  return AST_NULL;
}

AstNodePtr AstInterface::CreateConstInt( int val)  
{ 
  SgIntVal* res = new SgIntVal(GetFileInfo(),val);
  res->set_endOfConstruct(res->get_file_info());
  return AstNodePtrImpl(res);
}

AstNodePtr AstInterface::
CreateConstant( const string& valtype, const string& val) 
{
  if (valtype == "int") {
    int intval = atoi(val.c_str());
    return CreateConstInt(intval);
  } 
  else if (valtype == "bool") {
      int i = atoi(val.c_str());
      if (val == "true" || i != 0)
         return AstNodePtrImpl(new SgBoolValExp(GetFileInfo(), -1));
      else
         return AstNodePtrImpl(new SgBoolValExp(GetFileInfo(), 0));
  }
  else if (valtype == "string") {
         char *r = new char[val.size() + 1];
         strcpy( r, val.c_str());
      // return new SgStringVal(GetFileInfo(), r);
         SgStringVal *tmp = new SgStringVal(GetFileInfo(), r);
         ROSE_ASSERT(tmp != NULL);
         printf ("AstInterface::CreateConstant (copying SgStringVal original = %p copy = %p \n",r,tmp);
         return AstNodePtrImpl(tmp);
  }
  else if (valtype == "char") {
         return AstNodePtrImpl(new SgCharVal(GetFileInfo(), val[0]));
  }
  else if (valtype == "float") {
         istringstream in(val);
         float num = 0;
         in >> num;
         return AstNodePtrImpl(new SgFloatVal(GetFileInfo(), num));
  }
  else if (valtype == "double") {
         istringstream in(val);
         double num = 0;
         in >> num;
         return AstNodePtrImpl(new SgDoubleVal(GetFileInfo(), num));
  }
  else if (valtype == "function") {
      SgFunctionSymbol *fsym = impl->GetFunc(val);
      if ( fsym == 0) {
         return AST_NULL;
      }
      SgFunctionRefExp* NEW_FUNCTION_REF(fr,fsym);
      return AstNodePtrImpl(fr);
  }
  else if (valtype == "memberfunction") {
      char *start = 0;
      SgClassSymbol *c = impl->GetClass(val, &start);
      if (c == 0) {
         cerr << "Error: cannot find class declaration for " << val << endl;
         assert(false);
      }
      SgMemberFunctionSymbol *f1 = GetMemberFunc( impl, c, string(start));
      if (f1 == 0) {
         cerr << "Warning: cannot find member function " << val << endl;
         f1 = impl->NewMemberFunc(c, string(start), GetTypeInt(), 
                                   list<SgInitializedName*>()); 
         //assert(false);
      SgName f1name = f1->get_name();
      }
      SgMemberFunctionRefExp *NEW_MFUNCTION_REF(fr,f1);
      return AstNodePtrImpl(fr);
  }
  else if (valtype == "field") {
      char *start = 0;
      SgClassSymbol *c = impl->GetClass(val, &start);
      if (c == 0) {
         cerr << "Error: cannot find class declaration for " << val << endl;
         assert(false);
      }
      SgClassDeclaration *decl = c->get_declaration();
      assert( decl != 0);
      SgClassDefinition *def = GetClassDefn(decl);
      assert(def != 0);
      SgVariableSymbol *vs = impl->GetVar( start, def);
      if (vs == 0) {
         cerr << "Error : variable " << start << " not found in " << def->unparseToString() << ". \n";
         assert(false); 
      }
      return AstNodePtrImpl(new SgVarRefExp(GetFileInfo(), vs));
  }
  else {
       cerr << "Error: non-recognized value type for creating constant AST: " << valtype << endl;
        assert(false);
  }
}

SgFunctionSymbol* 
CreateMinMaxFunction(AstInterfaceImpl* impl, const std::string& name, int numOfPars, bool isMin)
{
  // jichi (9/30/2009): Add Fortran support
  if (IS_FORTRAN_LANGUAGE()) {	// Fortran max/min
    // jichi (9/30/2009): In Fortran, b/c max/min are built-in functions, no new function is defined, but
    // the description of the function (to construct SgFunctionRefExp) is returned instead.
    // Here're the diferences between Fortran implementation and Cxx:
    //   1. there is no needed to define the function body;
    //   2. there is no needed to declaration declaration to global.

    // NOTE: non-built-in min/max function creation for Fortran is not implemented.
    assert(name == (isMin? "min": "max"));
    assert(numOfPars >= 2);	// Case where argc equals to 1 is not accepted by Fortran max/min.

    SgType* typeint = GetTypeInt();	// assume all args are int
    std::list<SgInitializedName*> pars; 
    for (int i = 0; i < numOfPars; ++i) { 
      std::string parname = "a";
      parname.push_back(i + '0');
      SgName curname(parname.c_str());
      SgInitializedName* curVar = new SgInitializedName(GetFileInfo(), curname, typeint, 0, 0, 0, 0); 
      pars.push_back(curVar);
    }

    // jichi (10/2/2009): Following codes expand functions call chains below.
    // <code>SgFunctionSymbol* funcSymbol = impl->NewFunc(name, typeint, pars);</code>

    SgType* rtype = typeint;
    const bool has_ellipses = false;
    SgFunctionType *ft = new SgFunctionType(rtype, has_ellipses);

    //SgFunctionDeclaration  *d = new SgFunctionDeclaration(GetFileInfo(), name.c_str(), ft);
    SgFunctionDefinition* fd = NULL;	// No definition for built-in function.

    // SgProcedureHeaderStatement extends SgFunctionDeclaration
    SgProcedureHeaderStatement *d = new SgProcedureHeaderStatement(name.c_str(), ft, fd);

    /* Remove function params specification, since min/max can accept arbitary number of params.
    for (list<SgInitializedName*>::const_iterator p = pars.begin(); p != pars.end(); ++p) {
      SgInitializedName* cur = *p;
      d->append_arg(cur);
    }
    */

    SgFunctionSymbol* funcSymbol = new SgFunctionSymbol(d);

    /* Remove changes to global declaration, since min/max are built-in functions
    d->set_scope(global);
    global->insert_symbol( d->get_name(), f);
    d->set_parent(global);

    SgFunctionDeclaration* funcDecl = funcSymbol->get_declaration();
    funcDecl->set_requiresNameQualificationOnReturnType(false);
    funcDecl->set_definingDeclaration(funcDecl);
    SgBasicBlock* NEW_BLOCK(funcBody);
    SgFunctionDefinition* funcDefn = new SgFunctionDefinition(GetFileInfo(), funcBody);
    */

    return funcSymbol;

  } else {	// Cxx max/min

    SgType* typeint = GetTypeInt();
    std::list<SgInitializedName*> pars; 
    for (int i = 0; i < numOfPars; ++i) { 
      std::string parname = "a";
      parname.push_back(i + '0');
      SgName curname(parname.c_str());
      SgInitializedName* curVar = new SgInitializedName(GetFileInfo(), curname, typeint, 0, 0, 0, 0); 
      pars.push_back(curVar);
    }
    SgFunctionSymbol* funcSymbol = impl->NewFunc(name, typeint, pars); 
    SgFunctionDeclaration* funcDecl = funcSymbol->get_declaration();
    funcDecl->set_requiresNameQualificationOnReturnType(false);
    funcDecl->set_definingDeclaration(funcDecl);
    SgBasicBlock* NEW_BLOCK(funcBody);
    SgFunctionDefinition* funcDefn = new SgFunctionDefinition(GetFileInfo(), funcBody);
    funcBody->set_parent(funcDefn); funcDefn->set_parent(funcDecl);

    std::list<SgVariableSymbol*> parSymbols; 
    for (std::list<SgInitializedName*>::const_iterator iterPars=pars.begin(); iterPars != pars.end(); ++iterPars) { 
      SgInitializedName* curPar = *iterPars; 
      SgVariableSymbol* NEW_SYMBOL(curSymbol, SgVariableSymbol, funcDefn, curPar);
      parSymbols.push_back(curSymbol);
    }
    funcDecl->set_definition(funcDefn);
    funcDecl->set_endOfConstruct(funcDecl->get_file_info());
    if (numOfPars == 2) {
      SgVarRefExp* v1 = new SgVarRefExp( GetFileInfo(), parSymbols.front());
      SgVarRefExp* v2 = new SgVarRefExp( GetFileInfo(), parSymbols.back());
      SgExpression * cond = 0;
      if (isMin) cond = new SgLessThanOp(GetFileInfo(), v1,v2) ;
      else cond = new SgGreaterThanOp(GetFileInfo(), v1,v2); 
      v1->set_parent(cond); v2->set_parent(cond);
      v1 = new SgVarRefExp( GetFileInfo(), parSymbols.front());
      v2 = new SgVarRefExp( GetFileInfo(), parSymbols.back());
      SgExpression* returnExp = new SgConditionalExp(GetFileInfo(), cond, v1, v2, NULL);
      cond->set_parent(returnExp); v1->set_parent(returnExp); v2->set_parent(returnExp);
      SgStatement* returnStmt = new SgReturnStmt(GetFileInfo(), returnExp);
      funcBody->append_statement(returnStmt);
      returnStmt->set_parent(funcBody); 
    }
    else {
      SgName  resName("res");
      SgInitializedName* resVar = new SgInitializedName(GetFileInfo(), resName,typeint, 0, 0, 0, 0); 
      SgVariableSymbol * NEW_SYMBOL(resSymbol,SgVariableSymbol, funcBody,resVar);
      std::list<SgVariableSymbol*>::const_iterator iterParSymbols = parSymbols.begin();
      SgVarRefExp* parRef = new SgVarRefExp(GetFileInfo(), *iterParSymbols); 
      ++iterParSymbols; 
      SgAssignInitializer *NEW_VAR_INIT(resInit,resVar,parRef);
      SgVariableDeclaration *resDecl = new SgVariableDeclaration(GetFileInfo());
      resDecl->append_variable(resVar,resInit); 
      resVar->set_parent(resDecl);
      resDecl->set_endOfConstruct(resDecl->get_file_info());
      funcBody->append_statement(resDecl); 

      for (int i = 1; i < numOfPars; ++i) {
         SgVarRefExp* resRef = new SgVarRefExp(GetFileInfo(), resSymbol);
         parRef = new SgVarRefExp(GetFileInfo(), *iterParSymbols); 
         SgExpression * cond = 0;
         if (isMin) cond = new SgLessThanOp(GetFileInfo(), parRef,resRef); 
         else cond = new SgGreaterThanOp(GetFileInfo(), parRef,resRef); 
         resRef->set_parent(cond); parRef->set_parent(cond);
         SgStatement* NEW_EXPR_STMT(condStmt,cond);
         resRef = new SgVarRefExp(GetFileInfo(), resSymbol);
         parRef = new SgVarRefExp(GetFileInfo(), *iterParSymbols); 
         ++iterParSymbols; 
         SgExpression* assignExp = new SgAssignOp(GetFileInfo(), resRef, parRef);
         resRef->set_parent(assignExp); parRef->set_parent(assignExp);
         SgStatement* NEW_EXPR_STMT(assignStmt,assignExp);
         SgBasicBlock* NEW_BLOCK1(assignBlock,assignStmt);
         SgIfStmt* NEW_IF(ifStmt, condStmt, assignBlock);
         funcBody->append_statement(ifStmt);
      }
      SgVarRefExp* resRef = new SgVarRefExp(GetFileInfo(), resSymbol);
      SgStatement* returnStmt = new SgReturnStmt(GetFileInfo(), resRef);
      resRef->set_parent(returnStmt);
      funcBody->append_statement(returnStmt);
    }
    return funcSymbol;
  }
}

SgNode* AstInterfaceImpl :: 
CreateFunction( string name, int numOfPars)
{
  // jichi (9/30/2009): Add Fortran support
  // In SLICE, currently only max/min function is needed to create.
  bool isMin = (name == "min");
  bool isMax = (name == "max");
  if ((isMin || isMax) && ! IS_FORTRAN_LANGUAGE())	// Fortran built-in max/min can take arbitary number of params
    name.push_back( (numOfPars == 2)? '2' : '3');
  SgFunctionSymbol* funcSymbol = GetFunc( name);
  if (funcSymbol == 0) {
     if (isMin || isMax) {
       funcSymbol = CreateMinMaxFunction(this,name, numOfPars, isMin);
     }
     else  {
       std::cerr << "Unknown function: " << name << "\n";
       assert(false);
     }
  }
  SgFunctionRefExp* NEW_FUNCTION_REF(result,funcSymbol);
  return result;
}

AstNodePtr AstInterface::
CreateUnaryOP( OperatorEnum op, const AstNodePtr& _a0)
{
  AstNodePtrImpl a0(_a0);
  assert( HasNullParent(a0.get_ptr()));
  SgExpression *e = ToExpression(*this, a0);
  SgNode* result = 0;
  switch (op) {
   case UOP_ADDR: 
     result = new SgAddressOfOp( GetFileInfo(), e, e->get_type());
     break;
   case UOP_MINUS: 
     result = new SgMinusOp( GetFileInfo(), e, e->get_type());
     break;
   default:
     assert(false);
  }
  e->set_parent(result);
  return AstNodePtrImpl(result);
}

AstNodePtr AstInterface:: 
CreateBinaryOP( OperatorEnum op, const AstNodePtr& _a0, const AstNodePtr& _a1) 
{
  SgNode* a0 = AstNodePtrImpl(_a0).get_ptr(); 
  SgNode* a1 = AstNodePtrImpl(_a1).get_ptr(); 
  assert( HasNullParent(a1) && HasNullParent(a0));
  SgExpression *e0 = ToExpression(*this, _a0);
  SgExpression *e1 = ToExpression(*this, _a1);
  assert( e0 != 0 && e1 != 0);
  SgBinaryOp* n = 0;
  switch (op) {
  case BOP_DOT_ACCESS: 
      n = new SgDotExp(GetFileInfo(), e0, e1, e1->get_type()); break;
  case BOP_ARROW_ACCESS: 
      n = new SgArrowExp(GetFileInfo(), e0, e1, e1->get_type()); break;
  case BOP_DIVIDE:
      n = new SgDivideOp( GetFileInfo(), e0,e1); break;
  case BOP_TIMES: 
      n = new SgMultiplyOp(GetFileInfo(), e0, e1); break;
  case BOP_PLUS: 
      n = new SgAddOp(GetFileInfo(), e0,e1); break;
  case BOP_MINUS: 
      n = new SgSubtractOp(GetFileInfo(), e0,e1); break;
  case BOP_EQ:
      n =  new SgEqualityOp( GetFileInfo(), e0,e1); break;
  case BOP_NE:
      n = new SgNotEqualOp( GetFileInfo(), e0,e1); break;
  case BOP_LT:
      n = new SgLessThanOp( GetFileInfo(), e0,e1); break;
  case BOP_GT:
      n = new SgGreaterThanOp(GetFileInfo(), e0,e1); break;
  case BOP_LE:
      n = new SgLessOrEqualOp(GetFileInfo(), e0,e1); break;
  case BOP_GE:
      n = new SgGreaterOrEqualOp(GetFileInfo(), e0,e1); break;
  case BOP_AND:
      n =  new SgAndOp(GetFileInfo(), e0,e1); break;
  case BOP_OR:
      n =  new SgOrOp(GetFileInfo(), e0,e1); break;
  default:
      cerr << "Error: non-recognized binary operator: \n";
      assert(false);
  }
  e0->set_parent(n); e1->set_parent(n);
  n->set_endOfConstruct(n->get_file_info());
  return AstNodePtrImpl(n);
}

AstNodePtr AstInterface::
CreateArrayAccess( const AstNodePtr& arr, const AstNodeList& index)
{
  SgExpression* r = isSgExpression(AstNodePtrImpl(arr).get_ptr());
  assert(r);
  if (IsFortranLanguage()) {
    AstNodePtr arr_ref;
    AstNodeList arr_index;   

    // If arr is array access, append offsets to its first dimension.
    if (IsArrayAccess(arr, &arr_ref, &arr_index)) {
      //arr_index.insert(arr_index.end(), index.begin(), index.end());
      //return CreateArrayAccess(arr_ref, arr_index);

      assert(arr_index.size() >= 1);
      assert(index.size() == 1);

      SgExpression *e1 = ToExpression(*this, arr_index.front());
      SgExpression *e2 = ToExpression(*this, index.front());
      SgBinaryOp* i1 = new SgAddOp(GetFileInfo(), e1, e2);
      arr_index.front() = AstNodePtrImpl(i1);

      return CreateArrayAccess(arr_ref, arr_index);
    }

    // jichi(3/29/2010): Add Support for SgExprListExp for Fortran array index.

    SgExprListExp*  NEW_EXPR_LIST(indices);
    for (AstNodeList::const_iterator p = index.begin(); p != index.end(); ++p) {
      SgExpression* i = isSgExpression(AstNodePtrImpl(*p).get_ptr());
      assert(i);
      indices->append_expression(i);
      i->set_parent(indices);
    }

    SgExpression* aref = new SgPntrArrRefExp(GetFileInfo(), r, indices);
    assert(aref);
    aref->set_endOfConstruct(aref->get_file_info());

    r->set_parent(aref);
    indices->set_parent(aref);
    return AstNodePtrImpl(aref);

  } else {
    for (AstNodeList::const_reverse_iterator p = index.rbegin();
                                             p != index.rend(); ++p) {
       SgExpression* e = isSgExpression(AstNodePtrImpl(*p).get_ptr());
       assert(e != 0);
       SgExpression* r1 = new SgPntrArrRefExp(GetFileInfo(), r, e);
       r1->set_endOfConstruct(r1->get_file_info());
       r->set_parent(r1); e->set_parent(r1);
       r = r1;
    }
    return AstNodePtrImpl(r);
  }
}

AstNodePtr GetOverloadOperatorOpd1( const AstNodePtr& _exp)
{
  AstNodePtrImpl exp(_exp);
  SgFunctionCallExp *fs = isSgFunctionCallExp(exp.get_ptr());
  assert(fs != 0);
  SgExpression *func = fs->get_function();
  if (func->variantT() == V_SgDotExp) {
    return  AstNodePtrImpl( isSgDotExp(func)->get_lhs_operand() );
  }
  SgExpressionPtrList &l = fs->get_args()->get_expressions();
  return AstNodePtrImpl(l.front());
}

AstNodePtr GetOverloadOperatorOpd2( const AstNodePtr& _exp)
{
  AstNodePtrImpl exp(_exp);
  SgFunctionCallExp *fs = isSgFunctionCallExp(exp.get_ptr());
  assert(fs != 0);
  SgExpressionPtrList &l = fs->get_args()->get_expressions();
  return AstNodePtrImpl(l.back());
}

AstNodePtr AstInterface::
CreateFunctionCall( const AstNodePtr& func, const AstNodeList& args)
{
  return AstNodePtrImpl(impl->CreateFunctionCall(AstNodePtrImpl(func).get_ptr(), args));
}

AstNodePtr AstInterface::
CreateFunctionCall( const string& fname, const AstNodeList& args)
 {
  SgNode* f = impl->CreateFunction(fname, args.size());
  return AstNodePtrImpl(impl->CreateFunctionCall(f, args));
}

SgNode* AstInterfaceImpl::
CreateFunctionCall( SgNode* func, const AstNodeList& args)
{
  // jichi (9/30/2009): Add in Fortran support
  assert( HasNullParent(func));	// which implies func is not in the global AST now.
  SgExpression *fr = isSgExpression(func);

  SgExprListExp *NEW_EXPR_LIST(argexp);
  SgExpressionPtrList &l = argexp->get_expressions();
  AstNodeList::const_iterator p = args.begin(); 
  if (fr->variantT() == V_SgMemberFunctionRefExp) {
    SgExpression* obj = isSgExpression(AstNodePtrImpl(*p).get_ptr());
    assert(obj != 0 && HasNullParent(obj));
    ++p;
    SgExpression* fr1 = 0;
    if (obj->get_type()->variantT() == V_SgPointerType) {
      NEW_BIN_OP(fr1, SgArrowExp, obj, fr);
    } else {
      NEW_BIN_OP(fr1, SgDotExp, obj, fr);
    }
    obj->set_parent(fr1); fr->set_parent(fr1);
    fr = fr1;
  }
  for ( ;p != args.end(); ++p) {
    SgExpression *pr = isSgExpression( AstNodePtrImpl(*p).get_ptr() );
    if (pr != 0) {
       pr->set_parent(argexp);
       l.push_back( pr );
    }
    else throw AST_Error("Argument is NULL!\n");
  }
  SgFunctionCallExp *NEW_FUNCTION_CALL(result,fr, argexp);
  return result;
}

AstNodePtr
AstInterface::CreateReadStatement(const AstNodeList& __l) const
{
  assert(!__l.empty());

  SgExprListExp* NEW_EXPR_LIST(explist);
  for (AstNodeList::const_iterator p = __l.begin(); p != __l.end(); ++p) {
    SgExpression* e = isSgExpression(AstNodePtrImpl(*p).get_ptr());
    assert(e);
    explist->append_expression(e);
    e->set_parent(explist);
  }

  SgReadStatement* ret = new SgReadStatement(GetFileInfo());
  assert(ret);
  ret->set_endOfConstruct(ret->get_file_info());

  ret->set_io_stmt_list(explist);
  explist->set_parent(ret);

  SgExpression* f = new SgAsteriskShapeExp(GetFileInfo());
  f->set_endOfConstruct(f->get_file_info());
  ret->set_format(f);
  f->set_parent(ret);

  return AstNodePtrImpl(ret);
}

AstNodePtr
AstInterface::CreateNullStatement() const
{
  SgStatement* s = new SgNullStatement(GetFileInfo());
  s->set_endOfConstruct(s->get_file_info());
  return AstNodePtrImpl(s);
}

AstNodePtr
AstInterface::CreateWriteStatement(const AstNodeList& __l) const
{
  assert(!__l.empty());

  SgExprListExp* NEW_EXPR_LIST(explist);
  for (AstNodeList::const_iterator p = __l.begin(); p != __l.end(); ++p) {
    SgExpression* e = isSgExpression(AstNodePtrImpl(*p).get_ptr());
    assert(e);
    explist->append_expression(e);
    e->set_parent(explist);
  }

  SgWriteStatement* ret = new SgWriteStatement(GetFileInfo());
  assert(ret);
  ret->set_endOfConstruct(ret->get_file_info());

  ret->set_io_stmt_list(explist);
  explist->set_parent(ret);

  SgExpression* f = new SgAsteriskShapeExp(GetFileInfo());
  f->set_endOfConstruct(f->get_file_info());
  ret->set_format(f);
  f->set_parent(ret);

  return AstNodePtrImpl(ret);
}

AstNodePtr
AstInterface::CreatePrintStatement(const AstNodeList& __l) const
{
  assert(!__l.empty());

  SgExprListExp* NEW_EXPR_LIST(explist);
  for (AstNodeList::const_iterator p = __l.begin(); p != __l.end(); ++p) {
    SgExpression* e = isSgExpression(AstNodePtrImpl(*p).get_ptr());
    assert(e);
    explist->append_expression(e);
    e->set_parent(explist);
  }

  SgPrintStatement* ret = new SgPrintStatement(GetFileInfo());
  assert(ret);
  ret->set_endOfConstruct(ret->get_file_info());

  ret->set_io_stmt_list(explist);
  explist->set_parent(ret);

  SgExpression* f = new SgAsteriskShapeExp(GetFileInfo());
  f->set_endOfConstruct(f->get_file_info());
  ret->set_format(f);
  f->set_parent(ret);

  return AstNodePtrImpl(ret);
}

AstNodePtr
AstInterface::CreateIf(const AstNodePtr& __cond, const AstNodePtr& __istmt,
                       const AstNodePtr& __estmt) const
{ 
  AstNodePtrImpl cond(__cond), istmt(__istmt);
  assert( HasNullParent(cond.get_ptr())
          && HasNullParent(istmt.get_ptr()) );

  SgStatement* c = ToStatement(cond.get_ptr());
  SgBasicBlock *ib = isSgBasicBlock(istmt.get_ptr());    // if-block
  if (!ib) {
     SgStatement* p = ToStatement(istmt.get_ptr());
     NEW_BLOCK1(ib, p);
  }

  SgIfStmt* ret;
  if (__estmt == AST_NULL) {    // no else-statement
    NEW_IF(ret, c, ib);

  } else {
    AstNodePtrImpl estmt(__estmt);
    assert(HasNullParent(estmt.get_ptr()));

    SgBasicBlock *eb = isSgBasicBlock(estmt.get_ptr());    // else-block
    if (!eb) {
       SgStatement* p = ToStatement(estmt.get_ptr());
       NEW_BLOCK1(eb, p);
    }

    NEW_IF_ELSE(ret, c, ib, eb);
  }

  return AstNodePtrImpl(ret);
}

AstNodePtr AstInterface::CreateBlock( const AstNodePtr& _orig) 
 { 
    AstNodePtrImpl orig(_orig);
    SgBasicBlock *NEW_BLOCK(r);
    r->set_parent(GetNullScope());
    if (orig != 0) {
       SgBasicBlock *r1 = isSgBasicBlock(orig.get_ptr());
       if (r1 != 0) {
          r->getAttachedPreprocessingInfo() = r1->getAttachedPreprocessingInfo();
       }
    }
    return AstNodePtrImpl(r);
 }

void BlockPrependAppendStmt( AstInterfaceImpl* impl, AstNodePtr& _b, const AstNodePtr& _s, bool isAppend=false)
    { 
      AstNodePtrImpl b(_b), s(_s);
      SgStatement* stmt = ToStatement(s.get_ptr());
      assert(stmt != 0);
      SgBasicBlock *sb = isSgBasicBlock(stmt);
      SgBasicBlock *basicBlock = isSgBasicBlock(b.get_ptr());
      assert(basicBlock != 0);

      SgVariableDeclaration* vardecl = isSgVariableDeclaration(stmt);
      if (vardecl != 0) {
          impl->AddVar(vardecl, basicBlock);
          return; 
      }

      if (sb == 0)  {
         if (!isAppend)
            basicBlock->prepend_statement( stmt);
         else
            basicBlock->append_statement( stmt);
         stmt->set_parent(basicBlock);
      }
      else {
         SgStatementPtrList l = sb->get_statements();
         bool hasdecl = false;
         for (SgStatementPtrList::iterator p = l.begin(); p != l.end(); ++p) {
            if (isSgDeclarationStatement(*p) != 0) {
                hasdecl = true;
                break;
            } 
         } 
         if (hasdecl) {
            if (!isAppend)
               basicBlock->prepend_statement(sb);
            else
               basicBlock->append_statement(sb);
            sb->set_parent(basicBlock);
         }
         else
            for (SgStatementPtrList::iterator p = l.begin(); p != l.end(); ++p) {
              SgStatement * cur = *p;
              cur->set_parent(basicBlock);
              if (!isAppend)
                 basicBlock->prepend_statement(cur);
              else
                 basicBlock->append_statement(cur);
         } 
      }
    }

void AstInterface::BlockAppendStmt( AstNodePtr& _b, const AstNodePtr& _s)
{ BlockPrependAppendStmt(impl,_b, _s, true); }
 
void AstInterface::
BlockPrependStmt( AstNodePtr& _b, const AstNodePtr& _s)
{ BlockPrependAppendStmt(impl,_b, _s, false); }

void AstInterface::
InsertStmt(AstNodePtr const & _orig, AstNodePtr const &_n, bool insertbefore,
           bool extractfromBlock)
{
   AstNodePtrImpl n(_n), orig(_orig);
   assert( HasNullParent(n.get_ptr()));
   SgStatement *s = isSgStatement(orig.get_ptr()), *ns = ToStatement(n.get_ptr());
   assert(s != 0);
   SgStatement *p = isSgStatement(s->get_parent());
   assert(p != 0);
   SgBasicBlock *nb = isSgBasicBlock(ns);
   if (extractfromBlock && nb != 0) {
      p->insert_statement_from_basicBlock(s, nb, insertbefore);
      SgStatementPtrList l = nb->get_statements();
      for (SgStatementPtrList::iterator pn = l.begin(); pn != l.end(); ++pn) {
              SgStatement * cur = *pn;
              cur->set_parent(p);
      } 
   }
   else {
      p->insert_statement(s, ns, insertbefore);
      ns->set_parent(p);
   }
   //FixSgTree(p);
}

void AstInterface::
InsertAnnot(AstNodePtr const& _n, const std::string& annot, bool insertbefore)
{
  SgNode* n = AstNodePtrImpl(_n).get_ptr(); 
  SgLocatedNode* loc = isSgLocatedNode(n);
  assert (loc != 0);
   {
     Sg_File_Info* nf = loc->get_file_info();

  // DQ (7/19/2008): Modified interface to PreprocessingInfo
  // Note that this function could directly call SageInterface::attachComment(SgLocatedNode*,std::string);
     PreprocessingInfo * info = 
       new PreprocessingInfo( PreprocessingInfo::C_StyleComment, 
                              annot, nf->get_filename(),
                              nf->get_line(), nf->get_col(), 1, 
                              (insertbefore)? (PreprocessingInfo::before) 
                                            :  (PreprocessingInfo::after));
     loc->addToAttachedPreprocessingInfo( info);
   }

}

bool AstInterface::RemoveStmt( const AstNodePtr& _n)
{
   AstNodePtrImpl n(_n);
   SgStatement* s = isSgStatement(n.get_ptr());
   assert (s != 0); 
   SgStatement* p = isSgStatement(n->get_parent());
   assert( p != 0);
   p->remove_statement(s);
   s->set_parent(GetNullScope());
   return true;
}

bool AstInterfaceImpl::
ReplaceAst( SgNode* orig, SgNode* n)
{ 
    assert(HasNullParent(n));
    /*
    if (delayXform) {
        replList.push_back( std::pair<SgNode*,SgNode*>(orig,n));
        return false;
    }
    */
    SgNode *p = orig->get_parent();
    if (p == 0) return false;
    SgStatement *stmtOrig = isSgStatement(orig);
    SgStatement* stmtParent = isSgStatement(p);
    if (stmtOrig != 0) {
         SgStatement *stmtNew  = isSgStatement(n);
         assert(stmtParent != 0 && stmtNew != 0);
         stmtParent->replace_statement(stmtOrig, stmtNew);
         stmtNew->set_parent(stmtParent);
     }
     else {
         SgExpression *expOrig = isSgExpression(orig);
         SgExpression *expNew = isSgExpression(n);
         assert(expOrig != 0 && expNew != 0);
         if (stmtParent != 0)  {
            stmtParent->replace_expression(expOrig, expNew);
            expNew->set_parent(stmtParent);
         }
         else {
            SgExpression *expParent = isSgExpression(p);
            if (expParent != 0) {
               expParent->replace_expression(expOrig, expNew);
               expNew->set_parent(expParent);
            }
            else {
               SgInitializedName *nameParent = isSgInitializedName(p);
               assert(nameParent != 0);
               SgAssignInitializer *NEW_VAR_INIT(init, nameParent, expNew);
            }
         }
    }
    orig->set_parent(GetNullScope());
    return true;
 }
bool AstInterface::ReplaceAst( const AstNodePtr& _orig, const AstNodePtr& _n)
    { 
      SgNode*  orig = AstNodePtrImpl(_orig).get_ptr();
      SgNode*  n = AstNodePtrImpl(_n).get_ptr();
      return impl->ReplaceAst(orig, n);
    }

//typedef bool BoolAttribute;
class BoolAttribute
{
  bool val;
 public:
  BoolAttribute( bool v = true) : val(v) {}
  operator bool() const { return val; } 
};

class SageProcessAstNode : public AstTopDownBottomUpProcessing<BoolAttribute,BoolAttribute> 
{
   AstInterface *fa;
   AstInterface::TraversalOrderType t;
   ProcessAstNode& op;
   BoolAttribute evaluateInheritedAttribute(SgNode* astNode, 
                                            BoolAttribute inheritedValue)
     { if (t == AstInterface::PostOrder)
          return inheritedValue;
       return inheritedValue? op.Traverse( *fa, AstNodePtrImpl(astNode), AstInterface::PreVisit) : false; 
     }
   BoolAttribute evaluateSynthesizedAttribute(SgNode* astNode, 
                                              BoolAttribute inheritedValue, 
                                                 SynthesizedAttributesList l)
    {   if (t == AstInterface::PreOrder)
           return inheritedValue; 
        if (! inheritedValue)
            return false;
        for (size_t i = 0; i < l.size(); ++i) 
           if (!l[i])
              return false;
        return op.Traverse( *fa, AstNodePtrImpl(astNode), AstInterface::PostVisit); 
    }
  public:
   SageProcessAstNode( ProcessAstNode& _op) : op(_op) {}
   bool Traverse( AstInterface *_fa, SgNode* node, AstInterface::TraversalOrderType _t)
       { fa = _fa;
         t = _t; 
         return AstTopDownBottomUpProcessing<BoolAttribute,BoolAttribute>::traverse(node, true);
       }
};


bool 
ReadAstTraverse( AstInterface& fa, const AstNodePtr& _root, ProcessAstNode& op,
                 AstInterface::TraversalOrderType t)
{
   AstNodePtrImpl root(_root);
   return SageProcessAstNode(op).Traverse(&fa, root.get_ptr(), t);
}

template <class Transform>
class PerformPreTransformationTraversal 
 : public AstTopDownBottomUpProcessing<_DummyAttribute, AstNodePtrImpl>
{
    SgNode* head;
    AstNodePtrImpl result, orig;
    bool succ;
    Transform& op;
    AstInterface& fa;
    _DummyAttribute evaluateInheritedAttribute(SgNode* astNode, _DummyAttribute a)
     {
        if (!succ) {
          //std::cerr << "pre operating on " << astNode << "::" << astNode->unparseToString() << "\n";
          succ = op(fa, AstNodePtrImpl(astNode), result);
          if (succ) {
               assert(result != 0);
               orig = astNode;
          }
        }
       return _DummyAttribute();
     }
   AstNodePtrImpl evaluateSynthesizedAttribute(SgNode* astNode, _DummyAttribute a, 
                                               SynthesizedAttributesList l)
      {
          //std::cerr << "post operating on " << astNode << "::" << astNode->unparseToString() << "\n";
          if (orig == astNode) {
              orig = 0;
              succ = false;
              if (result != astNode && (astNode != head || astNode->get_parent() != 0)) {
                  fa.ReplaceAst(AstNodePtrImpl(astNode), result);
              }
              return result;
          }
          else 
            return astNode;
      }

  public:
    PerformPreTransformationTraversal( AstInterface& _fa, Transform& _op)
       : result(0), orig(0), succ(false), op(_op), fa(_fa) {}
    AstNodePtrImpl operator() ( SgNode* n)
          {
             succ = false;
             head = n;
             fa.get_impl()->delay_newVarInsert(); 
             AstNodePtrImpl r = 
                AstTopDownBottomUpProcessing<_DummyAttribute, AstNodePtrImpl>::
                           traverse(n, _DummyAttribute());
             fa.get_impl()->apply_newVarInsert();
             return r;
          }
};

template <class Transform>
class PerformPostTransformationTraversal : public AstBottomUpProcessing<AstNodePtrImpl>
{
  SgNode *head;
    AstInterface& fa;
    Transform& op;
  AstNodePtrImpl evaluateSynthesizedAttribute(SgNode* astNode, SynthesizedAttributesList l)
      {
          AstNodePtrImpl r  = astNode;
          if (op(fa, AstNodePtrImpl(astNode), r) && r != astNode) {
              assert(r != 0);
              if (r != astNode && (astNode != head || astNode->get_parent() != 0))
                       fa.ReplaceAst(AstNodePtrImpl(astNode), r);
              return r;
          }
          else 
            return astNode;
      }

  public:
    PerformPostTransformationTraversal( AstInterface& _fa, Transform& _op)
       : fa(_fa), op(_op) { }
    AstNodePtrImpl operator() ( SgNode* n)
      {
         head = n;
         fa.get_impl()->delay_newVarInsert();
         AstNodePtrImpl r = 
             AstBottomUpProcessing<AstNodePtrImpl>::traverse(n);
         fa.get_impl()->apply_newVarInsert();
         return r;
      }
};

AstNodePtr TransformAstTraverse( AstInterface& fa, const AstNodePtr& r, 
          bool (*op)( AstInterface&, const AstNodePtr&, AstNodePtr&),
                                AstInterface::TraversalVisitType t )
{
  if (t == AstInterface::PreVisit) {
     PerformPreTransformationTraversal<bool (*)(AstInterface&, const AstNodePtr&, 
                                                AstNodePtr& ) > traverse(fa, op);
     return traverse(AstNodePtrImpl(r).get_ptr());
  }
  else {
     PerformPostTransformationTraversal<bool (*)(  AstInterface&, const AstNodePtr&, 
                                                    AstNodePtr&) > traverse(fa, op);
     return traverse(AstNodePtrImpl(r).get_ptr());
  }
}

AstNodePtr TransformAstTraverse( AstInterface& fa, const AstNodePtr& r, 
                              TransformAstTree& op, 
                              AstInterface::TraversalVisitType t) 
{
  if (t == AstInterface::PreVisit) {
     PerformPreTransformationTraversal<TransformAstTree> traverse(fa, op);
     AstNodePtr result = traverse(AstNodePtrImpl(r).get_ptr());
     return result;
  }
  else {
     PerformPostTransformationTraversal<TransformAstTree> traverse(fa, op);
     AstNodePtr result = traverse(AstNodePtrImpl(r).get_ptr());
     return result;
  }
}

template class PerformPreTransformationTraversal<bool (*)(AstInterface &, AstNodePtr const &, AstNodePtr &)>;
template class PerformPostTransformationTraversal<bool (*)(AstInterface &, AstNodePtr const &, AstNodePtr &)>;
template class PerformPreTransformationTraversal<TransformAstTree>;
template class PerformPostTransformationTraversal<TransformAstTree>;
template class list<SgExpression *, allocator<SgExpression *> >;
template class vector<AstNodePtr, allocator<AstNodePtr> >;
template class AstTopDownBottomUpProcessing<_DummyAttribute, AstNodePtr>;
template class AstBottomUpProcessing<AstNodePtr>;
template class SgTreeTraversal<_DummyAttribute, AstNodePtr>;

// QY: 1/8/2008 no longer used
class CheckSymbolTable : public AstTopDownProcessing<AstNodePtrImpl>
{
 public:
  void operator() ( SgNode* n)
  { AstTopDownProcessing<AstNodePtrImpl>::traverse( n, n->get_parent() ); }

  static void fix_vardecl( SgVariableDeclaration* d )  
  {
   SgScopeStatement* scope = d->get_scope();
   if (DebugSymbol()) {
        cerr << "Adding symbol info for variable declaration: ";
        cerr << d->unparseToString() << " : into scope " << scope << endl;
        
   }
   SgInitializedNamePtrList & l = d->get_variables();
   for (SgInitializedNamePtrList::iterator p = l.begin(); p != l.end(); ++p) {
        SgInitializedName* n = *p;
        SgName name = n->get_name() ;
        SgVariableSymbol *sb = scope->lookup_var_symbol(name);
        if (sb == 0) {
           if (DebugSymbol())
              cerr << "Adding symbol for variable: " << name.str() << endl;
           NEW_SYMBOL(sb, SgVariableSymbol, scope, n);
        }
        else if (n != sb->get_declaration()) {
           assert(false);
        }
    }
  }

  static void fix_classdecl( SgClassDeclaration* d1 ) 
  {
   SgScopeStatement* scope = d1->get_scope();
   if (DebugSymbol()) {
     cerr << "Adding symbol info for class declaration: ";
     cerr << d1->unparseToString() << endl;
   }
   SgName name = d1->get_name();
   SgClassSymbol *sb = scope->lookup_class_symbol(name);
   if (sb == 0) {
        if (DebugSymbol())
           cerr << "Adding symbol for class: " << name.str() << endl;
        NEW_SYMBOL(sb, SgClassSymbol, scope, d1);
   }
  }

  private:
  AstNodePtrImpl evaluateInheritedAttribute(SgNode* ast, AstNodePtrImpl v)
  {
     if (v.get_ptr() != 0 && v != ast->get_parent()) {
        if (ast->get_parent() == NULL)
           ast->set_parent(v.get_ptr());
        std::cerr << "Incorrect parent for AST: " << AstToString(AstNodePtrImpl(ast)) << "\n";
        std::cerr << "It has parent : " << ((ast->get_parent() == v.get_ptr())? "NULL" : AstToString(AstNodePtrImpl(ast->get_parent()))) << "\n";
        std::cerr << "It should have parent: "  << AstToString(v) << "\n";
        assert(false);
     }
     switch (ast->variantT()) {
     case V_SgVariableDeclaration:
        fix_vardecl(isSgVariableDeclaration(ast));
        break;
     case V_SgClassDeclaration:
        fix_classdecl(isSgClassDeclaration(ast));
        break;
     case V_SgVarRefExp:
        {
         SgVarRefExp *var = isSgVarRefExp(ast);
         SgScopeStatement *scope = GetScope(ast);
         string name = var->get_symbol()->get_name().str();
         SgVariableSymbol *r =  LookupVar(name, scope);
         if (r == 0) {
             cerr << "failed to find symbol for variable: " << name << " in scope " << scope << endl;
             //assert(false);
         }
         else
            var->set_symbol( r);
        }
        break;
     default: break;
     }
    return ast;
  }
};


void FixSgTree( SgNode *r)
{  
     assert(r!=0);
     //AstPostProcessing(r);

  if (r->get_parent() != 0 && isSgScopeStatement(r) != 0) {
     CheckSymbolTable symbolfix;
     symbolfix(r);
  }
}

void FixSgProject( SgProject &sageProject)
{
   int filenum = sageProject.numberOfFiles();
   for (int i = 0; i < filenum; ++i) {
     SgFile &sageFile = sageProject.get_file(i);
     FixSgTree(&sageFile);
   }
}


namespace { // anonymous
  
  inline string
  fortran_function_fix(const string& __func)
  {
    const string C_PREFIX = "SgFunctionCallExp:";
    const string FORTRAN_PREFIX = "SgFunctionCallExp:CALL ";

    const int prefix_start = 0;
    const int prefix_len = FORTRAN_PREFIX.length();
    if (__func.compare(prefix_start, prefix_len, FORTRAN_PREFIX) == 0) {
      const string left = __func.substr(prefix_len);
      return C_PREFIX + left;
    } else
      return __func;
  }
} // anonymous

// Since 12/9/2009, jichi
void 
AstInterface::_debug(const AstNodePtr& __x) const
{ impl->_debug(__x); }

// Since 12/9/2009, jichi
void
AstInterfaceImpl::_debug(const AstNodePtr& __x) const
{ std::cerr << "AstInterface::_debug:" << ::AstToString(__x) << std::endl; }

/* EOF */
