
#include <rose.h>
#include <general.h>
#include <stdlib.h>

#include <iostream>
#include <string.h>
#include <AstInterface.h>
#include <CommandOptions.h>

#include "AstTraversal.h"
#include "astPostProcessing.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// DQ (3/8/2006): Since this is not used in a heade file it is OK here!
#define Boolean int

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
#if 0
//   Sg_File_Info *r = new Sg_File_Info( "default_file_name", 1,1);

  // DQ (9/5/2005): This is a depricated function
  // r->set_isPartOfTransformation(true);
//   r->setTransformation();

  // DQ (9/5/2005): Mark this as a transformation plus
  // mark as OutputInCodeGeneration so that the information 
  // will be progated to the interior statements of functions, 
  // blocks etc.
  // r->setCompilerGenerated();
  // r->setOutputInCodeGeneration();

  // r->display("output file info: debug");
  // ROSE_ASSERT(false);
     return r;
#endif

  // DQ (3/8/2006): This is the easiest way to represent a transformation
  // since we have to both mark the file info object as a transformation
  // AND to be output in the code generation phase as well.
     return Sg_File_Info::generateDefaultFileInfoForTransformationNode();
   }

SgStatement* ToStatement( SgNode* _stmts)
{
    SgStatement *stmts = isSgStatement(_stmts);
    if (stmts == 0) {
       SgExpression *exp = isSgExpression(_stmts);
       assert(exp != 0);
       stmts = new SgExprStatement(GetFileInfo(), exp);
       stmts->set_endOfConstruct(GetFileInfo());
       exp->set_parent(stmts);
    }
    return stmts;
}

SgClassDefinition* GetClassDefinition( SgNamedType *classtype)
{
    if (classtype->variantT() == V_SgTypedefType) {
         return GetClassDefinition(isSgNamedType(isSgTypedefType(classtype)->get_base_type()));
    }
    SgDeclarationStatement *decl = classtype->get_declaration();
    if (decl->variantT() == V_SgClassDeclaration) 
        return isSgClassDeclaration(decl)->get_definition();
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

                      // DQ (10/1/2007): This is a fix suggested in email by Brian White on 6/19/2007.
                      // cur = statement->get_scope();
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

#if 0
  while (loc != 0 && stmt == 0) {
      SgNode *p = loc->get_parent();
      ROSE_ASSERT(p != NULL);
   // printf ("Inside of GetScope(%p = %s) parent = %p = %s \n",loc,loc->class_name().c_str(),p,p->class_name().c_str());
      int t = p->variantT();
      if (t == V_SgDotExp  || t == V_SgArrowExp) {
        SgExpression* lhs = isSgBinaryOp(p)->get_lhs_operand();
        if (lhs != loc) {
           SgType* lhstype = lhs->get_type();
           SgNamedType* lhsntype = isSgNamedType(lhstype);
           while (lhsntype == 0) {
             switch (lhstype->variantT()) {
              case V_SgReferenceType: 
                    lhstype = isSgReferenceType(lhstype)->get_base_type();
                    lhsntype = isSgNamedType(lhstype);
                    break;
              case V_SgPointerType:
                    lhstype = isSgPointerType(lhstype)->get_base_type();
                    lhsntype = isSgNamedType(lhstype);
                    break;
              default: 
                 assert(false);      
              }
           }
           p = GetClassDefinition(lhsntype);
        }
      }
      loc = p;
      stmt = isSgStatement(loc);
  }
  SgScopeStatement *cur = (stmt == 0)? 0 : isSgScopeStatement(stmt);
// printf ("stmt = %p cur = %p \n",stmt,cur);
  if (stmt != 0 && cur == 0) {
      if (stmt->variantT() == V_SgFunctionDeclaration && stmt != loc) 
         cur = isSgFunctionDeclaration(stmt)->get_definition()->get_body(); 
      else
       {
      // printf ("Calling stmt->get_scope() \n");
         cur = stmt->get_scope();
       }
  }
#endif

#if 0
  printf ("Returning from GetScope(%p = %s) return value = %p = %s \n",
     loc,(loc != NULL) ? loc->class_name().c_str() : "NULL",
     cur,(cur != NULL) ? cur->class_name().c_str() : "NULL");

  if (cur != NULL)
     {
       SgNode *parent = cur->get_parent();
       printf ("Returning from GetScope(%p = %s) parent of return value = %p = %s \n",
            loc,(loc != NULL) ? loc->class_name().c_str() : "NULL",
            parent,(parent != NULL) ? parent->class_name().c_str() : "NULL");

       SgScopeStatement* previousScope = cur;
       int counter = 0;
       while (previousScope != NULL)
          {
            printf ("   Scope stack[%d] = %p = %s \n",counter++,previousScope,previousScope->class_name().c_str());
            previousScope->get_file_info()->display("location of scope: debug");
            previousScope = (isSgGlobal(previousScope) != NULL) ? NULL : previousScope->get_scope();
          }
     }
#endif

  return cur;
}

SgVarRefExp* ToVarRef( const AstInterface& fa, const AstNodePtr& exp)
{
// SgInitializedName *decl = 0;
  switch (exp->variantT()) {
  case V_SgVarRefExp:
     return isSgVarRefExp( exp );
     break;
  case V_SgInitializedName:
    {
      SgInitializedName* var = isSgInitializedName(exp);
      SgName varname = var->get_name();
      return isSgVarRefExp(fa.CreateVarRef( varname.getString(), exp));
      break;
    }
  default: {}
  }
  return 0;
}

SgExpression* ToExpression( const AstInterface& fa, const AstNodePtr& s)
{
  SgExpression *exp = ToVarRef(fa, s);
  if (exp == 0)
      exp = isSgExpression(s);
  return exp;
}

string StripParameterType( const string& name)
{
  char *const_start = strstr( name.c_str(), "const");
  string r = (const_start == 0)? name : string(const_start + 5);
  int end = r.size()-1;
  if (r[end] == '&') {
       r[end] = ' ';
  }
  string result = "";
  for (unsigned int i = 0; i < r.size(); ++i) {
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
     SgVariableSymbol* r = cdef->lookup_var_symbol(start);
     if (r != 0)
         return r;
  // DQ (9/4/2005): SgBaseClassList is not a list of pointers and renamed to be SgBaseClassPtrList
  // SgBaseClassList& l = cdef->get_inheritances();
  // for (SgBaseClassList::iterator p = l.begin(); p != l.end(); ++p) {
  //    SgBaseClass& cur = *p;
  //    SgClassDeclaration *decl = cur.get_base_class();
     SgBaseClassPtrList& l = cdef->get_inheritances();
     for (SgBaseClassPtrList::iterator p = l.begin(); p != l.end(); ++p) {
        SgBaseClass* cur = *p;
        SgClassDeclaration *decl = cur->get_base_class();
        assert(decl != 0);
        SgClassDefinition *def = decl->get_definition();
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
        f = loc->lookup_var_symbol(start);
     // printf ("In LookupVar() for non class member: loc = %s loc->lookup_var_symbol(%s) = %p \n",loc->class_name().c_str(),start,loc->lookup_var_symbol(start));
        if (loc->variantT() == V_SgGlobal || f != 0)
             break;
        loc = loc->get_scope();
     }
     while ( loc != 0 && f == 0);
     return f;
  }
}

class SageResetParent : public AstTopDownProcessing< AstNodePtr >
{
   AstNodePtr evaluateInheritedAttribute(SgNode* astNode, AstNodePtr inheritedValue)
     { if ( inheritedValue != 0) { 
          //assert(astNode->get_parent() == inheritedValue || astNode->get_parent() == 0);
	  ROSE_ASSERT (inheritedValue);
          astNode->set_parent( inheritedValue );
       }
       return astNode;
     }
  public:
   SageResetParent() {}
   void operator() ( SgNode* node )
       {
         return AstTopDownProcessing<AstNodePtr>::traverse(node, node->get_parent());
       }
};

class CheckSymbolTable : public AstTopDownProcessing<AstNodePtr>
{
 public:
  void operator() ( SgNode* n)
  { AstTopDownProcessing<AstNodePtr>::traverse( n, n->get_parent() ); }

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
              cerr << "Adding symbol for variable: " << name.getString() << endl;
           scope->insert_symbol(name, new SgVariableSymbol( n ) );
	   n->set_scope(scope);
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
           cerr << "Adding symbol for class: " << name.getString() << endl;
        scope->insert_symbol(name, new SgClassSymbol( d1 ) );
        sb = scope->lookup_class_symbol(name);
        assert(sb != 0);
   }
  }

#if 0
  private:
  AstNodePtr evaluateInheritedAttribute(SgNode* ast, AstNodePtr v)
  {
     assert(ast->get_parent() == v);
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
         string name = var->get_symbol()->get_name().getString();

         SgVariableSymbol *r =  LookupVar(name, scope);
         if (r == 0) {
             if (DebugSymbol()) 
                cerr << "failed to find symbol for variable: " << name << " in scope " << scope << endl;
             assert(false);
         }
         var->set_symbol( r);
        }
        break;

     default:
        {
       // DQ (8/20/2005): Added default to avoid compiler warnings about unrepresented cases
        }
     }
    return ast;
  }
#endif

  private:
  AstNodePtr evaluateInheritedAttribute(SgNode* ast, AstNodePtr v)
  {
     assert(ast->get_parent() == v);
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
         ROSE_ASSERT(scope != NULL);
         string name = var->get_symbol()->get_name().getString();
#if 0
         printf ("variable name = %s in scope = %p = %s \n",name.c_str(),scope,scope->class_name().c_str());
         SgScopeStatement* previousScope = scope;
         int counter = 0;
         while (previousScope != NULL)
            {
              printf ("   Scope stack[%d] = %p = %s \n",counter++,previousScope,previousScope->class_name().c_str());
              previousScope->get_file_info()->display("location of scope: debug");
              previousScope = (isSgGlobal(previousScope) != NULL) ? NULL : previousScope->get_scope();
            }
#endif
         SgVariableSymbol *r =  LookupVar(name, scope);
         if (r == 0) {
          // if (DebugSymbol()) 
                cerr << "failed to find symbol for variable: " << name << " in scope " << scope << endl;
             assert(false);
         }
         var->set_symbol( r);
        }
        break;
     default: {}
     }
    return ast;
  }
};


void FixSgTree( SgNode *r)
   {
  // fix everything except parent pointers
  // subTemporaryAstFixes(r);

  // DQ (9/4/2005): Call new function AstPostProcessing() instead of subTemporaryAstFixes()
     assert(r!=0);
     AstPostProcessing(r);

  // DQ (9/4/2005): Skip these tests since they are done in AST consistancy tests
  // perform cycle test
  // assert(r!=0);
  // AstCycleTest cycTest;
  // cycTest.traverse(r);

  // SageResetParent op; op(r); 

     if (r->get_parent() != 0 && isSgScopeStatement(r) != 0)
        {
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

void AstNodePtr::Dump() const
{
  cerr << AstInterface::AstToString(*this);
}

class AstInterfaceImpl
{
  SgNode *top;
  SgScopeStatement* scope;
  SgGlobal* global;
  int newVarIndex;
 public:
  AstInterfaceImpl( SgNode* _top)  : newVarIndex(0)
   { set_top(_top); }

  SgNode* get_top() const { return top; }  
  SgScopeStatement* get_scope() const { return scope; }
  void set_top( SgNode* _top) 
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
 
  SgFunctionSymbol* LookupFunction(const char* start) const
   {
     SgScopeStatement *cur = scope;
     SgFunctionSymbol* f = 0;
     do {
        f = cur->lookup_function_symbol(start);
        if (cur->variantT() == V_SgGlobal)
             break;
        cur = cur->get_scope();
     }
     while ( cur != 0 && f == 0); 
     return f;
    }

  SgClassSymbol* LookupClass(const char* start) const
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

  SgSymbol* CreateDeclarationStmts( const string& _decl);

  SgSymbol* AddDecls ( const SgDeclarationStatementPtrList& l) ;
  SgClassSymbol* GetClass( const string& name, char** start = 0);
  SgClassSymbol* NewClass( const string& name);
  SgClassSymbol* AddClass( SgClassDeclaration* d) 
  {
 // DQ (9/28/2005): This function assumes that the new class is added to global 
 // scope which would seem to be very dangerous (if I understand it correctly).

    assert(global != 0);
    SgDeclarationStatementPtrList& l = global->get_declarations();
    l.insert( l.begin(), d);
    SgClassSymbol *c = new SgClassSymbol(d);
    SgName n =  c->get_name();
    global->insert_symbol( n, c);
    return c;
  }

  SgFunctionSymbol* GetFunc( const string& name);
  SgFunctionSymbol* NewFunc( const string& name, SgType* rtype, 
                             const list<SgInitializedName*>& args );
  SgFunctionSymbol* AddFunc( SgFunctionDeclaration *d)
   {
     assert(global != 0);
     SgDeclarationStatementPtrList& l = global->get_declarations();
     l.insert( l.begin(), d);
     SgFunctionSymbol* f = new SgFunctionSymbol(d);
     global->insert_symbol( f->get_name(), f);
     return f;
   }

  SgMemberFunctionSymbol* GetMemberFunc( SgClassSymbol *decl, const string& name,
                                         SgExpressionPtrList* args = 0);
  SgMemberFunctionSymbol* NewMemberFunc( SgClassSymbol *decl, const string& name,
                                 SgType* rtype, const list<SgInitializedName*>& args );

  SgMemberFunctionSymbol* AddMemberFunc( SgClassDefinition *def, 
                                      SgMemberFunctionDeclaration *d)
   {
     ROSE_ASSERT (def);
     d->set_parent(def);
     SgMemberFunctionSymbol* f = new SgMemberFunctionSymbol(d);
     def->insert_symbol(d->get_name(),f);
     return f;
   }

  SgVariableSymbol* NewVar ( SgType* type, const string& _name, bool makeunique, 
                             SgScopeStatement* loc=0);
  SgVariableSymbol* GetVar( const string& name, SgScopeStatement* loc = 0)
   {
     if (loc == 0)
        loc = scope;
     return LookupVar(name, loc);
   }

  void AddVarDecl( SgVariableDeclaration *d, SgScopeStatement* curscope = 0)
   {
     if (curscope == 0)
        curscope = scope;
     if (curscope->variantT() == V_SgFunctionDefinition)
         curscope = isSgFunctionDefinition(curscope)->get_body();

     ROSE_ASSERT(curscope != NULL);
     d->set_parent(curscope);

  // DQ & QY (5/29/2005): Marked variable declaration as part of transformation
     d->get_file_info()->setTransformation();

  // DQ (3/8/2006): This is also required since all transformed code is not output (e.g. template stuff)
     d->get_file_info()->setOutputInCodeGeneration();

     curscope->insertStatementInScope(d, true);
   }

  SgVariableSymbol* AddVar( SgVariableDeclaration *d, SgScopeStatement* curscope = 0)
   {
     printf ("Adding a variable declaration = %p to scope = %p \n",d,curscope);
     if (curscope == 0)
        curscope = scope;
     ROSE_ASSERT (curscope);
     d->set_parent(curscope);

  // DQ & QY (5/29/2005): Marked variable declaration as part of transformation
     d->get_file_info()->setTransformation();

  // DQ (3/8/2006): This is also required since all transformed code is not output (e.g. template stuff)
     d->get_file_info()->setOutputInCodeGeneration();

     curscope->insertStatementInScope(d, true);
     SgInitializedNamePtrList & l = d->get_variables();
     SgVariableSymbol *v = 0;
     for (SgInitializedNamePtrList::iterator p = l.begin(); p != l.end(); ++p) {
         SgInitializedName* cur = *p;
         v = new SgVariableSymbol(cur);
         curscope->insert_symbol( cur->get_name(), v);
	 cur->set_scope(curscope);
     }
     return v;
   }

SgVariableSymbol* AddVar( SgInitializedName *d, SgScopeStatement* curscope = 0)
   {
   // SgScopeStatement* altScope = d->get_scope();

#if 0
     printf ("In AddVar: SgInitializedName = %p = %s \n",d,d->get_name().getString().c_str());
     printf ("In AddVar: curscope          = %p = %s \n",curscope,(curscope != NULL) ? curscope->class_name().c_str() : "NULL");
     printf ("In AddVar: scope             = %p = %s \n",scope,(scope != NULL) ? scope->class_name().c_str() : "NULL");
     printf ("In AddVar: scope (get_scope) = %p = %s \n",altScope,(altScope != NULL) ? altScope->class_name().c_str() : "NULL");
#endif
     ROSE_ASSERT(d->get_file_info() != NULL);

     if (curscope == 0)
          curscope = scope;
     ROSE_ASSERT (curscope);
#if 0
  // I would expect that this is a problem 
     if (altScope != curscope)
        {
          printf ("Warning scope of SgInitializedName being changed, will effect symbol table handling! \n");
        }
#endif

  // DQ (9/4/2005): I think we might need this as well! No, don't do this!
  // this code appears to be fragile and this is a problem.
  // d->set_parent(curscope);

  // DQ & QY (5/29/2005): Added initialization of scope
     d->set_scope(curscope);

  // printf ("AddVar: initialized name = %p = %s  curscope = %p = %s \n",d,d->get_name().getString().c_str(),curscope,curscope->class_name().c_str());

     ROSE_ASSERT(d->get_file_info() != NULL);

     SgVariableSymbol *v = new SgVariableSymbol(d);
     ROSE_ASSERT(v != NULL);
     curscope->insert_symbol( d->get_name(), v);

     return v;
   }
};

#if 0
SgSymbol* AstInterfaceImpl::CreateDeclarationStmts( const string& _decl)
{
     string decl = "echo \"" + _decl + "\"  > _astInterface_Tmp.c \n";
     system(decl.c_str());
     int argc = 2, error;
     char* argv[2] = {"AddDecl", "_astInterface_Tmp.c"};
     char** argp = argv;
     SgFile addDecls(argc, argp,error);
     system( "rm _astInterface_Tmp.c");
     SgGlobal *declRoot = addDecls.get_root();
     SgDeclarationStatementPtrList& decls = declRoot->get_declarations ();
     SgSymbol* r = AddDecls(decls);
     decls.clear();
     return r;
}
#else
// DQ (9/5/2005):
// This is the new version of the function that only looks in the source file!
// This is required since each global scope has numerous builtin symbols
// (functions and typedefs).
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
     vector<string> argv(2);

  // Build the command line (for input to ROSE translator)
     argv[0] = "AddDecl";
     argv[1] = uniqueFilename;

  // Build the AST
     SgFile* addDecls = new SgFile(argv, error);

  // system( "rm _astInterface_Tmp.c");
     unlink( uniqueFilename );
  // string systemString = string("rm '") + uniqueFilename + "'";
  // system( systemString.c_str() );

     SgGlobal *declRoot = addDecls->get_root();
     SgDeclarationStatementPtrList& decls = declRoot->get_declarations ();

  // SgSymbol* r = AddDecls(decls);
     SgDeclarationStatementPtrList localList;
     SgDeclarationStatementPtrList::iterator i = decls.begin();
     while (i != decls.end())
        {
       // DQ (9/5/2005):
       // This is set to "_astInterface_Tmp.c" for the input file and the full path for include files
       // if this is a transformation then it is a questionable how much since it makes to ask what 
       // the filename is.  ROSE will return a detail name for the filename request but will also issue 
       // a warning that it makes no sense to ask.  The sematics is that users should check if the IR 
       // node is part of either a transformation or compiler generated before calling get_filename().
       // I'm not yet clear if this semantics is a particularly good idea.
       // if ((*i)->get_file_info()->isTransformation() == false)
             {
               string filename = (*i)->get_file_info()->get_filename();

            // DQ (8/31/2006): Need to strip off the path since EDG now adds such things automatically (filename normalization to absolute paths).
               filename = StringUtility::stripPathFromFileName(filename);

            // if ( filename == "_astInterface_Tmp.c")
               if ( filename == uniqueFilename)
                  {
                 // DQ (9/5/2005): Mark this as a transformation (this appears to make no difference!)
                    (*i)->get_file_info()->setTransformation();
                 // (*i)->get_file_info()->setCompilerGenerated();
                 // (*i)->get_file_info()->setOutputInCodeGeneration();

                 // DQ (3/8/2006): This is also required since all transformed code is not output (e.g. template stuff)
                 // Since we call the markTransformationsForOutput I don't think this is required here!
                    (*i)->get_file_info()->setOutputInCodeGeneration();

                 // DQ (9/5/2005): This is required!
                 // Fixup the child nodes in the declaration to be marked as a transformation 
                 // (not really required to call the full FixSgTree function).
                    markTransformationsForOutput(*i);

                    localList.push_back(*i);
                  }
             }
          i++;
        }

  // printf ("In CreateDeclarationStmts: built new statements list (size = %zu) string = \n%s\n \n",localList.size(),_decl.c_str());

#if 0
     printf ("Exiting in CreateDeclarationStmts \n");
     ROSE_ASSERT(false);
#endif

     SgSymbol* r = AddDecls(localList);
     decls.clear();
     return r;
   }
#endif

SgVariableSymbol* AstInterfaceImpl::
NewVar ( SgType* type, const string& _name, bool makeunique, SgScopeStatement* loc)
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

  // DQ (6/24/2006): We need a Sg_File_Info object for this to be a valid SgInitializedName
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
  d->set_endOfConstruct(GetFileInfo());
  for (list<SgInitializedName*>::const_iterator p = args.begin(); p != args.end();
       ++p) {
     SgInitializedName* cur = *p;
     d->append_arg(cur);
     ROSE_ASSERT (d);
     cur->set_parent(d);
  }
  return AddFunc(d);
}

SgClassSymbol* AstInterfaceImpl :: GetClass( const string& val, char** start)
{
    string classname = "";
    for (unsigned int size = 0 ; size < val.size(); ++size) { 
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
 SgClassDeclaration *decl = new SgClassDeclaration( GetFileInfo(), classname.c_str());
 return AddClass(decl);
}

SgMemberFunctionSymbol * AstInterfaceImpl::
GetMemberFunc( SgClassSymbol* c, const string& funcname, SgExpressionPtrList* args)
{
  SgClassDeclaration* decl = c->get_declaration();
  SgName classname = decl->get_name();
  SgClassDefinition * def = decl->get_definition();
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
        if ( name.getString() != funcname)
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
	   AstInterfaceBase::GetTypeInfo(tp, 0, &partype);
           AstInterfaceBase::GetTypeInfo(ta, 0, &argtype);
           if (partype != argtype) {
              match = false;
              break;
           }
        }
        if (match) {
           SgMemberFunctionSymbol* f = new SgMemberFunctionSymbol(md);
           return f;
        }
     }
  }
  return 0;
}

SgMemberFunctionSymbol * AstInterfaceImpl :: 
NewMemberFunc( SgClassSymbol* c, const string& name, SgType*  rtype, 
               const list<SgInitializedName*>& args)
{
  SgClassDeclaration* decl = c->get_declaration();
  const char * start = name.c_str();
  SgClassDefinition* classDefn = decl->get_definition();
  if (classDefn == 0) {
     classDefn = new SgClassDefinition(GetFileInfo(), decl);
     classDefn->set_parent(scope);
     decl->set_definition(classDefn);
  }

  SgMemberFunctionType *ft = new SgMemberFunctionType(rtype, false);
// DQ (11/17/2004): removed scope parameter so that it could be set more generally for all functions
// SgMemberFunctionDeclaration  *d = new SgMemberFunctionDeclaration(GetFileInfo(), start, ft, 0, classDefn);
  SgMemberFunctionDeclaration  *d = new SgMemberFunctionDeclaration(GetFileInfo(), start, ft, 0);
  d->set_scope(classDefn);
  for (list<SgInitializedName*>::const_iterator p = args.begin(); p != args.end();
       ++p) {
     SgInitializedName* cur = *p;
     d->append_arg(cur);
     ROSE_ASSERT (d);
     cur->set_parent(d);
  }

  return AddMemberFunc( classDefn, d);
}

SgSymbol* AstInterfaceImpl :: AddDecls( const SgDeclarationStatementPtrList& decls)
{
     SgSymbol* result = 0;
     for (SgDeclarationStatementPtrList::const_iterator p = decls.begin(); p != decls.end(); ++p) {
         SgDeclarationStatement* cur = *p;
         cur->set_file_info( GetFileInfo());
         SgFunctionDeclaration* d1 = isSgFunctionDeclaration(cur);
         if (d1 != 0) {
            result = AddFunc(d1);
         }
         else {
            SgVariableDeclaration* d2 = isSgVariableDeclaration(cur);
            if (d2 != 0) {
                result = AddVar(d2);
            }
            else  {
               SgClassDeclaration* d3 = isSgClassDeclaration(cur);
               if (d3 != 0)
                   result = AddClass(d3);
               else
                {
                  printf ("Error, not supported: cur->class_name() = %s \n",cur->class_name().c_str());
                  assert(false);
                }
            }
         }
    }
    return result;
}

AstInterface :: AstInterface( const AstNodePtr& root) 
   {  impl = new AstInterfaceImpl(root); }

AstInterface :: ~AstInterface()
{ delete impl; }


void AstInterface :: SetRoot( const AstNodePtr& root)
{ impl->set_top(root); }

AstNodePtr AstInterface :: GetRoot() const 
{ return impl->get_top(); }

void AstInterface::AddDeclaration( const string& decl)
{
  impl->CreateDeclarationStmts(decl);
}

string AstInterfaceBase::AstToString( const AstNodePtr& s)
        {
          if (s == 0) 
             return "";
          string r = "";
          if (!IsVarRef(s, 0, &r)) 
                r = string(s->sage_class_name()) + ":" + s->unparseToString();
          else 
                r = string(s->sage_class_name()) + ":" + r;
          return r;
        }

void AstInterfaceBase::write( const AstNodePtr& s, ostream& out) 
        { 
          if (s != 0) 
             out << s->unparseToString(); 
        }
void AstInterfaceBase::DumpAst( const AstNodePtr& s)
        {
          if (s != 0) {
             cerr << s->sage_class_name() << "::" << s->unparseToString();
          }
        }

/*
Boolean AstInterfaceBase::
AstTreeIdentical( const AstNodePtr& n1, const AstNodePtr& n2)
{  
  if (! AstNodeIdentical(n1,n2))
      return false;
   vector<SgNode*> childvec1 = n1->get_traversalSuccessorContainer();
   vector<SgNode*> childvec2 = n2->get_traversalSuccessorContainer();
   if (childvec1.size() != childvec2.size())
       return false;
   for (int i = 0; i < childvec1.size(); ++i) {
      AstNodePtr c1 = childvec1[i];
      AstNodePtr c2 = childvec2[i];
      if (AstNodeIdentical(c1, c2))
         return false;
   }
   return true;
}
Boolean AstInterfaceBase:: 
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

void AstInterfaceBase::FreeAstTree( const AstNodePtr& n)
{ }

void NotifyTreeCopy ( AstInterface& fa, const AstNodePtr& orig, const AstNodePtr& n) 
{
  vector<SgNode*> childvec = orig->get_traversalSuccessorContainer();
  vector<SgNode*> childvec1 = n->get_traversalSuccessorContainer();
  assert( childvec.size() == childvec1.size());
  for (unsigned int i = 0; i < childvec.size(); ++i) {
      AstNodePtr c = childvec[i], c1 = childvec1[i];
      if (c != c1)
         NotifyTreeCopy( fa, c, c1);
  } 
  CopyAstRecord info(fa, orig, n);
  fa.Notify(info);
};

AstNodePtr AstInterface :: CopyAstTree( const AstNodePtr &orig) 
{
 if (orig->variantT() == V_SgInitializedName) {
   return ToVarRef(*this, orig);
 }
 ROSE_ASSERT (orig);
 ROSE_ASSERT (orig->get_parent());
 SgTreeCopy tc;
 SgNode *r = orig->copy(tc) ;
 ROSE_ASSERT (r != orig);
 // cerr << "Copied node 0x" << hex << orig << " is a " << orig->class_name() << " ----> 0x" << hex << r << dec << endl;
 ROSE_ASSERT (orig->get_parent());
 r->set_parent(orig->get_parent()); // Needs to be set for FixSgTree()
 // r->set_parent(NULL); // JJW 10-26-2007 Commented this out so parents would be some AST node, even if not the correct one
 if ( NumberOfObservers() )
    NotifyTreeCopy( *this, orig, r);
 return r;
}

AstInterfaceBase::AstNodeList AstInterfaceBase :: GetChildrenList( const AstNodePtr &n)
{
   vector<SgNode*> childvec = n->get_traversalSuccessorContainer();
   AstNodeList childlist;
   for (unsigned int i = 0; i < childvec.size(); ++i) {
      AstNodePtr c = childvec[i]; 
      childlist.push_back(c);
   }
   return childlist;
}

AstNodePtr AstInterface :: GetParent( const AstNodePtr &n)
{
  SgNode *node = n;
  if (node == GetRoot())
      return 0;
  return node->get_parent();
}

Boolean AstInterfaceBase:: IsDecls( const AstNodePtr& s) 
        { 
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
Boolean AstInterfaceBase:: IsStatement( const AstNodePtr& s)
{
  return isSgStatement(s) != 0;
}

Boolean AstInterfaceBase::IsExecutableStmt( const AstNodePtr& s) 
        { 
          switch (s->variantT()) {
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
	     case V_SgLabelStatement:
	     //case V_SgVariableDeclaration: 
             //        QY:    Do not consider vardecl as executable or it will break loopProcessor
                  return true;
             default: return false;
            }
        } 

AstNodePtr AstInterfaceBase::GetPrevStmt( const AstNodePtr& s)
{
  SgNode *n = s;
   SgNode *p = n->get_parent();
   assert(p != 0);
   // vector<SgNode*> succs = p->get_traversalSuccessorContainer();
   // size_t i2 = std::find(succs.begin(), succs.end(), n) - succs.begin();
   size_t i = p->get_childIndex(n);
   // ROSE_ASSERT (i == (size_t)(-1) || i == i2);
   if (i == 0) {
     return 0;
   } else if (i == (size_t)(-1)) {
     ROSE_ASSERT (!"GetPrevStmt: not found");
     return p->get_traversalSuccessorByIndex(p->get_numberOfTraversalSuccessors() - 1);
   } else {
     SgNode *r = p->get_traversalSuccessorByIndex(i-1);
     return r;    
   }
}

AstNodePtr AstInterfaceBase::GetNextStmt( const AstNodePtr& s)
{
  SgNode *n = s;
   SgNode *p = n->get_parent();
   assert(p != 0);
   // vector<SgNode*> succs = p->get_traversalSuccessorContainer();
   // size_t i2 = std::find(succs.begin(), succs.end(), n) - succs.begin();
   size_t i = p->get_childIndex(n);
   // ROSE_ASSERT (i == (size_t)(-1) || i == i2);
   if (i == p->get_numberOfTraversalSuccessors()-1) {
     return 0;
   } else if (i == (size_t)(-1)) {
     ROSE_ASSERT (!"GetNextStmt: not found");
     return p->get_traversalSuccessorByIndex(0);
   } else {
     SgNode *r = p->get_traversalSuccessorByIndex(i+1);
     return r;
   }
}

Boolean AstInterfaceBase::IsIf( const AstNodePtr& s, AstNodePtr* cond,
				AstNodePtr* truebody, AstNodePtr* falsebody) 
{ 
  int t = s->variantT();
  switch (t) {
  case V_SgIfStmt:
    {
      SgIfStmt *is = isSgIfStmt(s);
      if (cond != 0)
	*cond = is->get_conditional();
      if (truebody != 0)
	*truebody = is->get_true_body();
      if (falsebody != 0)
	*falsebody = is->get_false_body();
    }
    break;
  case V_SgCaseOptionStmt:
    {
      SgCaseOptionStmt* cs = isSgCaseOptionStmt(s);
      if (cond != 0)
	*cond = cs->get_key();
      if (truebody != 0)
	*truebody = cs->get_body();
      if (falsebody != 0)
	*falsebody = 0;
    }
    break;
  default:
    return false;
  }
  return true;
}

Boolean AstInterfaceBase::IsLabelStatement( const AstNodePtr& s)
{
  return s->variantT() == V_SgLabelStatement;
}

Boolean AstInterfaceBase::
IsGoto( const AstNodePtr& s, AstNodePtr* dest) 
{ 
  switch (s->variantT()) {
  case V_SgGotoStatement:
    if (dest != 0) {
      SgLabelStatement *label = isSgGotoStatement(s)->get_label();
      *dest = label; 
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
      *dest = scope;
    }
    break;
  case V_SgContinueStmt:
    if (dest != 0) {
      SgNode* scope = 0;
      for (scope = s->get_parent(); ; scope = scope->get_parent()){
	int t = scope->variantT();
	if (t == V_SgForStatement || t == V_SgWhileStmt || 
	    t == V_SgDoWhileStmt)
	  break;
      }
      if (scope->variantT() == V_SgForStatement)
	scope = isSgForStatement(scope)->get_increment();
      *dest = scope;
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
      *dest =  scope;
    }
    break;
  default: return false;
  } 
  return true;
}

Boolean AstInterfaceBase::IsGotoBefore( const AstNodePtr& s)
{ 
  switch (s->variantT()) {
  case V_SgGotoStatement:
  case V_SgContinueStmt:
    return true;
  default: return false;
  }
}
Boolean AstInterfaceBase::IsGotoAfter( const AstNodePtr& s)
{ 
  switch (s->variantT()) {
  case V_SgReturnStmt:
  case V_SgBreakStmt:
    return true;
  default: return false;
  }
}

AstNodePtr AstInterfaceBase::GetFunctionDecl( const AstNodePtr& _s)
{
    SgNode *s = _s;
    int t = s->variantT();
    switch (t) {
    case V_SgFunctionDefinition: 
         return isSgFunctionDefinition(s)->get_declaration();
    case V_SgFunctionDeclaration:
    case V_SgMemberFunctionDeclaration:
        return s;
    case V_SgMemberFunctionRefExp:
         return isSgMemberFunctionRefExp(s)->get_symbol()->get_declaration();
    case V_SgFunctionSymbol:
          return isSgFunctionSymbol(s)->get_declaration();
    case V_SgFunctionRefExp:
          return isSgFunctionRefExp(s)->get_symbol()->get_declaration();
    case V_SgMemberFunctionSymbol:
         return isSgMemberFunctionSymbol(s)->get_declaration();
    case V_SgConstructorInitializer:
         return isSgConstructorInitializer(s)->get_declaration();
    case V_SgDotExp:
         return GetFunctionDecl( isSgDotExp(s)->get_rhs_operand());
    }
    cerr << "Error: not recognizable function type : " << s->sage_class_name() << endl;
    assert(false);
}

Boolean AstInterfaceBase::
IsFunctionDecl( const AstNodePtr& s, string* name, 
	     AstNodeType* returntype, AstTypeList* paramtype, AstNodeList* params)
{
  SgNode* d = s;
  SgFunctionParameterList *l = 0;
  switch (d->variantT()) {
  case V_SgFunctionDeclaration: 
    { 
      SgFunctionDeclaration *decl = isSgFunctionDeclaration(d);
      if (returntype != 0)
	*returntype = decl->get_type()->get_return_type();
      if (name != 0) 
	*name =  decl->get_name().getString();
      if (paramtype != 0 || params != 0) 
	l = decl->get_parameterList();
      break;
    }
  case V_SgMemberFunctionDeclaration:
    {
      SgMemberFunctionDeclaration* decl = isSgMemberFunctionDeclaration(d);
      if (returntype != 0)
	*returntype = decl->get_type()->get_return_type();
      if (name != 0) {
	SgName cn = decl->get_scope()->get_qualified_name(); 
	SgName fn = decl->get_name();
	*name =  cn.getString() + "::" + fn.getString();
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
         paramtype->push_back(cur->get_type()); 
      if (params != 0)
         params->push_back(cur);
    }
  }
  return true;
}

Boolean AstInterfaceBase::
IsFunctionDefinition(  const AstNodePtr& s, AstNodePtr* decl, AstNodePtr* body)
{
  switch (s->variantT()) {
  case V_SgFunctionDefinition:
    {
      SgFunctionDefinition *def =  isSgFunctionDefinition(s);
      if (decl != 0) 
	*decl = def->get_declaration();
      if (body != 0)
	*body = def->get_body();
      return true;
    }
  default:
    return false;
  }
}

Boolean AstInterfaceBase::
IsAssignment( const AstNodePtr& s, AstNodePtr* lhs, AstNodePtr* rhs, bool *readlhs) 
{ 
  SgExprStatement *n = isSgExprStatement(s);
  SgExpression *exp = (n != 0)? n->get_expression() : isSgExpression(s);
  if (exp != 0) {
    switch (exp->variant()) {
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
	  *lhs = s2->get_lhs_operand();
	if (rhs != 0) {
	  SgNode *init = s2->get_rhs_operand();
	  if ( init->variantT() == V_SgAssignInitializer) 
            init = isSgAssignInitializer(init)->get_operand();
	  *rhs = init;
	}
        if (readlhs != 0)
           *readlhs = (exp->variant() != V_SgAssignOp);
	return true;
      }
    case V_SgMinusMinusOp:
    case V_SgPlusPlusOp:
       {
        SgUnaryOp* s2 = isSgUnaryOp(exp);
        if (lhs != 0)
          *lhs = s2->get_operand();
        if (readlhs != 0)
           *readlhs = true;
        if (rhs != 0) 
           *rhs = exp;
        return true;
      }
    }
  }
  return false;
}

Boolean AstInterfaceBase:: 
IsVariableDecl(const AstNodePtr& s, AstNodeList* vars, AstNodeList* init)
{
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
           vars->push_back(var);
         if (init != 0)
            init->push_back(def);
     }
     return true;
  }
  return false;
}

AstNodePtr AstInterface:: 
AllocateArray( const AstNodePtr& arr, const AstNodeType& elemtype,
               const vector<AstNodePtr>& indexsize)
{
   SgType* atype = elemtype; 
   for (unsigned int i = 0; i < indexsize.size(); ++i) {
      SgExpression* exp = isSgExpression(indexsize[i]);
      assert(exp != 0);
      atype = new SgArrayType(atype, exp);
   }

// DQ (9/5/2005): Need to specify that the internal "p_associated_class_unknown" 
// data field is true (last parameter in list).
// SgNewExp* rhs = new SgNewExp(GetFileInfo(), atype, 0, new SgConstructorInitializer(GetFileInfo()));
// SgNewExp* rhs = new SgNewExp(GetFileInfo(), atype, 0, new SgConstructorInitializer(GetFileInfo(),NULL,NULL,NULL,false,false,false,true));   
   SgType* baseType = elemtype;
   assert(baseType != NULL);
   SgNewExp* rhs = new SgNewExp(GetFileInfo(), atype, 0, new SgConstructorInitializer(GetFileInfo(),NULL,NULL,baseType,false,false,false,true));
   return CreateAssignment( arr, rhs);
}

AstNodePtr AstInterface:: DeleteArray( const AstNodePtr& arr)
{
  SgExpression* var = isSgExpression(arr);
  assert(var != 0);
  return new SgDeleteExp(GetFileInfo(), var, true);
}

AstNodePtr AstInterface::
CreateAssignment( const AstNodePtr& lhs, const AstNodePtr& rhs)
{
  SgExpression *exp = 0;
  SgExpression *lhsexp = ToExpression(*this, lhs);
  SgType* lhstype = lhsexp->get_type(); 
  SgExpression* rhsexp = ToExpression(*this, rhs);
  SgType* rhstype = rhsexp->get_type();
  assert(rhstype != 0);

// SgVarRefExp *var = isSgVarRefExp( lhs );
  if (lhstype->variantT() == V_SgClassType) {
    SgClassType *lhstype1 = isSgClassType(lhstype);
    SgName classname = lhstype1->get_name();
    SgClassSymbol *c = impl->GetClass( classname.getString());
    assert (c != 0);
    SgExpressionPtrList args;
    args.push_back( rhsexp);
    SgMemberFunctionSymbol *f = impl->GetMemberFunc( c, "operator=", &args);
    if (f != 0) {
        SgClassDefinition *def = c->get_declaration()->get_definition();
        SgMemberFunctionRefExp *fr = 
           new SgMemberFunctionRefExp(GetFileInfo(), f);
        fr->set_need_qualifier(false);
        SgExpression *func = new SgDotExp( GetFileInfo(), lhsexp, fr ); 
        fr->set_parent(func);
        lhs->set_parent(func);
        SgExprListExp *argexp = new SgExprListExp(GetFileInfo());
	argexp->set_endOfConstruct(GetFileInfo());
        rhsexp->set_parent(argexp);
        SgExpressionPtrList &l = argexp->get_expressions();
        l = args;
        exp = new SgFunctionCallExp(GetFileInfo(), func, argexp);
	exp->set_endOfConstruct(GetFileInfo());
        func->set_parent(exp);
        argexp->set_parent(exp);
    } 
  }
  if (exp == 0) {
     exp = new SgAssignOp(GetFileInfo(), lhsexp, rhsexp);
     lhs->set_parent(exp); 
     rhs->set_parent(exp);
  }
  if ( NumberOfObservers() ) {
    CopyAstRecord info(*this, rhs, lhs);
    Notify(info);
  }
  return exp;
}

Boolean AstInterfaceBase::
IsIOInputStmt( const AstNodePtr& s, AstNodeList* varlist) 
{ return false; }
Boolean AstInterfaceBase::
IsIOOutputStmt( const AstNodePtr& s, AstNodeList* explist) 
{ return false; }

Boolean AstInterfaceBase::IsConstInt( const AstNodePtr& exp, int *val) 
{ 
  if (exp->variantT() == V_SgIntVal) {
    if (val != 0) 
      *val = isSgIntVal(exp)->get_value();
    return true;
  }
  return false;
}

Boolean AstInterfaceBase::IsConstant( const AstNodePtr& exp, string *val)
{
  switch (exp->variantT()) {
  case V_SgStringVal:
  case V_SgShortVal:
  case V_SgCharVal:
  case V_SgUnsignedCharVal:
  case V_SgWcharVal:
  case V_SgUnsignedShortVal:
  case V_SgIntVal:
  case V_SgEnumVal:
  case V_SgUnsignedIntVal:
  case V_SgLongIntVal:
  case V_SgLongLongIntVal:
  case V_SgUnsignedLongLongIntVal:
  case V_SgUnsignedLongVal:
  case V_SgFloatVal:
  case V_SgDoubleVal:
  case V_SgLongDoubleVal:
       break;
  default:
     return false;
  };
  if (val != 0)
     *val = exp->unparseToString();
  return true;
}

Boolean AstInterfaceBase::
IsSameVarRef( const AstNodePtr& n1, const AstNodePtr& n2)
{
   string name1, name2;
   AstNodePtr scope1, scope2;
   if (IsVarRef(n1, 0,&name1, &scope1) && IsVarRef(n2, 0, &name2, &scope2))
       return name1 == name2 && scope1 == scope2;
   return false;
}

Boolean AstInterfaceBase::
IsVarRef( const AstNodePtr& exp, AstNodeType* vartype, string* varname,
          AstNodePtr* _scope, bool *isglobal ) 
{ 
  SgNode *decl = 0;
  switch (exp->variantT()) {
  case V_SgVarRefExp:
    {
      SgVarRefExp *var = isSgVarRefExp( exp );
      SgVariableSymbol *sb = var->get_symbol();
      if (vartype != 0)
           *vartype = sb->get_type();
      if (varname != 0)
           *varname = sb->get_name().getString();
      decl = sb->get_declaration();
    }
     break;
  case V_SgThisExp:
    {
      SgThisExp *var = isSgThisExp( exp );
      if (vartype != 0)
        *vartype = var->get_type();
      if (varname != 0)
        *varname = "this";
      decl = exp;
      do {decl = decl->get_parent();} while (decl->variantT() != V_SgFunctionDefinition);
    }
     break;
  case V_SgInitializedName:
    {
      SgInitializedName* var = isSgInitializedName(exp);
      if (var->get_name().getString() == "")
           return false;
      SgType *t = var->get_type();
      assert( t != 0);
      if (vartype != 0)
	*vartype = t;
      if (varname != 0) {
        *varname = var->get_name().getString();
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
        *vartype = sb2->get_type();
     if (varname != 0)
        *varname = string(sb1->get_name().getString()) + "." + string(sb2->get_name().getString());
     decl = sb1->get_declaration();
     break;
   }
  default:
    return false;
  }

  if (_scope != 0 || isglobal != 0) {
 // printf ("decl = %p = %s \n",decl,decl->class_name().c_str());
    SgScopeStatement *scope = GetScope(decl);
 // printf ("scope = %p = %s \n",scope,scope->class_name().c_str());   
    assert(scope != 0);
    if (_scope != 0)
       *_scope =  scope;
    if (isglobal != 0)
       *isglobal = (scope->variantT() == V_SgGlobal);
  }
  return true;
}

string AstInterfaceBase::GetVarName( const AstNodePtr& exp)
{
  string name;
  bool succ = IsVarRef(exp, 0, &name);
  assert(succ);
  return name;
}

AstNodeType AstInterfaceBase::GetExpressionType( const AstNodePtr& s)
{
  AstNodeType t;
  if (!IsExpression(s, &t))
     assert(false);
  return t;
}


string AstInterface:: 
NewVar( const AstNodeType& type, const string& name, bool makeunique,
        const AstNodePtr& declLoc, const AstNodePtr& _init)
{
  SgScopeStatement *scope = (declLoc == 0)? 0 : GetScope(declLoc);
  SgVariableSymbol *sb = impl->NewVar( isSgType(type), name, makeunique, scope);
  SgName n =  sb->get_name();
  string varname =  n.getString();
  SgInitializedName* def = sb->get_declaration();

  if (def != 0 && def->get_parent() == 0) { 
      //variable declaration has not been inserted
    SgVariableDeclaration *decl = new SgVariableDeclaration( GetFileInfo());
    decl->set_definingDeclaration(decl);
    decl->set_endOfConstruct(GetFileInfo());
    SgAssignInitializer *init = 0;
    if (_init != 0) {
       SgExpression *e = ToExpression( *this, _init);
       init = new SgAssignInitializer(GetFileInfo(), e, e->get_type() );
       init->set_endOfConstruct(GetFileInfo());
    }
    decl->append_variable(def, init);
    def->set_parent(decl);
    impl->AddVarDecl( decl, scope);
    ROSE_ASSERT (decl->get_parent());
    SgInitializedName* ndef =  decl->get_variables().front();
    sb->set_declaration(ndef );
    ndef->set_parent(decl);
    ROSE_ASSERT (decl->get_scope());
    ndef->set_scope(decl->get_scope());
    // cerr << "Have good decl 0x" << hex << decl << endl;
  }
  else
     assert(_init == 0);
  return varname;
}

AstNodePtr AstInterface::CreateVarRef( string varname, const AstNodePtr& loc) const
   {
  // printf ("In AstInterface::CreateVarRef(): varname = %s \n",varname.c_str());
     int hasdot = varname.rfind(".", varname.size()-1);
     if (hasdot > 0) {
         string name1 = varname.substr(0, hasdot);
         string name2 = varname.substr( hasdot+1, varname.size()-hasdot);
         AstNodePtr obj = CreateVarRef(name1, loc);
         AstNodeType vartype = GetExpressionType(obj);
         string tname;
         GetTypeInfo(vartype, 0, &tname);
         return CreateBinaryOP( ".", obj, CreateConst( tname + "::" + name2, "field"));
      }

  // DQ (3/23/2006): It seems that this is sometimes NULL, but I don't know why!
  // ROSE_ASSERT(loc != NULL);
  // printf ("loc = %s \n",loc != NULL ? loc->class_name().c_str() : "NULL");

     SgScopeStatement *scope = NULL;
     if (loc != NULL)
          scope = GetScope(loc);

  // ROSE_ASSERT(scope != NULL);
  // printf ("scope = %p = %s \n",scope,scope != NULL ? scope->class_name().c_str() : "NULL");

     SgVariableSymbol *sym = impl->GetVar(varname, scope);
     if (sym == 0) {
         cerr << "Error : variable " << varname << " not found. \n";
         assert(false);
      }
     SgVarRefExp *r = new SgVarRefExp( GetFileInfo(), sym);
     r->set_endOfConstruct(GetFileInfo());
     return r;
   }

AstNodeType AstInterface::GetType(string name) 
{
  if (name[name.size()-1] == '*') {
    string name1 = name.substr(0, name.size()-1);
    SgType *t = isSgType(GetType(name1));
    SgPointerType *ptr = t->get_ptr_to();
    if (ptr == 0) {
       ptr = new SgPointerType(t);
       t->set_ptr_to(ptr);
    }
    return ptr;
  }
  else if (name == "char") 
      return new SgTypeChar();
  else if (name == "int")
      return new SgTypeInt();
  else if (name == "long")
       return new SgTypeLong();
  else if (name == "void")
       return new SgTypeVoid();
  else if (name == "float")
       return new SgTypeFloat();
  else if (name == "double")
       return new SgTypeDouble();
  else if (name == "string")
       return new SgTypeString();
  else if (name == "bool")
        return new SgTypeBool();
  else {
       SgClassSymbol *c = impl->GetClass(name);
       if (c == 0) {
          cerr << "Error: not recognize type name : " << name << endl;
          assert(false);
       }
       else
          return new SgClassType(c->get_declaration());
  }
} 

AstNodeType AstInterface::GetArrayType( const AstNodeType& base, const vector<AstNodePtr>& index)
{
  for (unsigned int i = 0; i < index.size(); ++i) {
     if (index[i]->variantT() != V_SgIntVal) {
        return new SgPointerType(base);
     }
  }
  SgType* r = base;
  for (unsigned int i = 0; i < index.size(); ++i) {
     SgExpression* ie = isSgExpression( index[i]);
     assert( ie != 0);
     r = new SgArrayType(r, ie);
  }
  return r;
}

Boolean AstInterfaceBase::
IsMemoryAccess( const AstNodePtr& s)
{  
  if (IsVarRef(s) || IsArrayAccess(s))
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
    {
     AstNodeType t;
     if (s->variantT() == V_SgFunctionCallExp && IsExpression(s,&t)
         && t->variantT() == V_SgReferenceType) {
        break;
     }
     return false;
    }
  }
  return true;
}

Boolean AstInterfaceBase::
IsArrayAccess( const AstNodePtr& s, AstNodePtr* array, AstNodeList* index)
{
  switch (s->variantT()) {
   case V_SgPntrArrRefExp:
    {
      if (index != 0 || array != 0) {
        SgNode* n = s;
        while (true) {
          SgPntrArrRefExp *arr = isSgPntrArrRefExp(n);
          if (arr == 0)
            break;
          n = arr->get_lhs_operand();
          if (array != 0)
            *array = n;
          if (index != 0)
            index->push_back(arr->get_rhs_operand());
        }
      }
      return true;
    }
  case V_SgVarRefExp:
  {
      SgVarRefExp *var = isSgVarRefExp( s );
      assert (var != 0);
      SgVariableSymbol *sb = var->get_symbol();
      SgType* vt = sb->get_type(); 
      if (array != 0)
          *array = s;
      return vt->variantT() == V_SgPointerType;
  }
  default: {}
  }
  return false;
}

/*
Boolean AstInterfaceBase::
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

Boolean AstInterfaceBase::
IsBinaryOp( const AstNodePtr& exp, AstNodePtr* opd1, AstNodePtr* opd2, string* fname) 
{ 
  SgBinaryOp *op = isSgBinaryOp(exp);
  if (op != 0) {
    if (opd1 != 0) 
      *opd1 = op->get_lhs_operand(); 
    if (opd2 != 0)
      *opd2 = op->get_rhs_operand(); 
    if (fname != 0) {
      switch (op->variantT()) {
      case V_SgDivideOp:
         *fname = "/"; break;
      default:
         *fname = "";
      }
    }
    return true;
  }
  else {
    AstNodePtr func;
    AstNodeList args;
    if (IsFunctionCall( exp, &func, &args) && args.size() == 2) {  
      if (opd1 != 0) 
	*opd1 = args.front();
      if (opd2 != 0)
	*opd2 = args.back();
      if (fname != 0)
	*fname = GetFunctionName(func);
      return true;
    }
  }
  return false;
}

Boolean AstInterfaceBase::
IsRelEQ( const AstNodePtr& s, AstNodePtr* opd1,  AstNodePtr* opd2)
{ 
  string fname = "";
  return IsBinaryOp(s, opd1, opd2, &fname) && 
    (s->variantT() == V_SgEqualityOp || fname == "==");
}

Boolean AstInterfaceBase::
IsRelNE( const AstNodePtr& s, AstNodePtr* opd1, AstNodePtr* opd2)
{
  string fname = "";
  if ( IsBinaryOp(s, opd1, opd2, &fname)) { 
     if (s->variantT() == V_SgNotEqualOp || fname == "!=" || fname == "operator!=")
          return true;
  }
  return false;
}

Boolean AstInterfaceBase::IsBinaryTimes( const AstNodePtr& exp, 
					 AstNodePtr* opd1, AstNodePtr* opd2) 
{ 
  string fname = "";
  return IsBinaryOp(exp, opd1, opd2, &fname) && 
    (exp->variantT() == V_SgMultiplyOp || fname == "*");
}

Boolean AstInterfaceBase::
IsBinaryPlus( const AstNodePtr& exp, AstNodePtr* opd1, AstNodePtr* opd2)  
{ 
  string fname = "";
  return IsBinaryOp(exp, opd1, opd2, &fname) && 
    (exp->variantT() == V_SgAddOp || fname == "+");
}

Boolean AstInterfaceBase::
IsBinaryMinus( const AstNodePtr& exp, AstNodePtr* opd1, AstNodePtr* opd2) 
{ 
  string fname = "";
  return IsBinaryOp(exp, opd1, opd2, &fname) && 
    (exp->variantT() == V_SgSubtractOp || fname == "-");
}

Boolean AstInterfaceBase::
IsUnaryOp( const AstNodePtr& exp, AstNodePtr* opd, string* fname) 
{ 
  SgUnaryOp *op = isSgUnaryOp(exp);
  if (op != 0) {
    if (opd != 0) 
      *opd = op->get_operand(); 
    if (fname != 0)
      *fname = "";
    return true;
  }
  else {
    AstNodePtr func;
    AstNodeList args;
    if (IsFunctionCall( exp, &func, &args) && args.size() == 1) {  
      if (opd != 0) 
	*opd = args.front();
      if (fname != 0)
	*fname = GetFunctionName(func);
      return true;
    }
  }
  return false;
}

Boolean AstInterfaceBase::
IsUnaryMinus( const AstNodePtr& exp, AstNodePtr* opd)
{
  string fname;
  return IsUnaryOp(exp, opd, &fname) && 
    (exp->variantT() == V_SgMinusOp || fname == "-"); 
}

Boolean AstInterfaceBase::IsBasicBlock( const AstNodePtr& exp)
{  
  switch (exp->variantT()) {
  case V_SgBasicBlock:
  case V_SgSwitchStatement:
  case V_SgForInitStatement:
    return true;
  default: {}
  };
  return false;
}

Boolean AstInterfaceBase::
IsFunctionCall( const AstNodePtr& s, AstNodePtr* func, AstNodeList* args)
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
      f = fs->get_function();
      argexp = fs->get_args();
    }
    break;
  case V_SgConstructorInitializer:
    f = exp;
    argexp =  isSgConstructorInitializer(exp)->get_args();
    break;
  default:
    return false;
  }
  switch (f->variantT()) {
  case V_SgDotExp: 
      { 
        SgDotExp* dot = isSgDotExp(f);
        SgNode* cur = dot->get_lhs_operand();
        f = dot->get_rhs_operand();
        if (args != 0)
          args->push_back( cur ); 
      }
      break;
  case V_SgArrowExp:
      { 
        SgArrowExp* arrow = isSgArrowExp(f);
        SgNode* cur = arrow->get_lhs_operand();
        f = arrow->get_rhs_operand();
        if (args != 0)
          args->push_back( cur ); 
      }
      break;
  case V_SgMemberFunctionRefExp:
      {
        if (args != 0)
          args->push_back(0);
         break;
      }
  default: {}
  }
  if (argexp != 0) {
     SgExpressionPtrList l = argexp->get_expressions();
     for ( SgExpressionPtrList::iterator p = l.begin(); p != l.end(); ++p) {
       if (args != 0)  {
         args->push_back(*p); 
       }
     }
  }
  if (func != 0)
     *func = f;
  return true;
}


string AstInterfaceBase::GetFunctionName( const AstNodePtr& func) 
{ 
  SgFunctionRefExp *fr = isSgFunctionRefExp(func);
  if (fr != 0) {
    SgFunctionSymbol *sb = fr->get_symbol();
    return sb->get_name().getString();
  }
  SgMemberFunctionRefExp *mf = isSgMemberFunctionRefExp(func);
  if (mf != 0)
    return mf->get_symbol()->get_name().getString();
  
  return "";
}

AstNodePtr AstInterface::GetVarDecl( const string& varname)
{
  SgVariableSymbol* s = impl->GetVar(varname);
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

void AstInterfaceBase::
GetTypeInfo(const AstNodeType& t, string *tname, string* stripname, int* size)
{
  string r1 = get_type_name(t);
  string result = "";
  for (unsigned int i = 0; i < r1.size(); ++i) {
    if (r1[i] != ' ')
      result.push_back(r1[i]);
  }
  if (tname != 0)
    *tname = result;
  if (stripname != 0)
    *stripname = StripParameterType(result);
  if (size != 0)
    *size = 4;
}

Boolean AstInterfaceBase::
IsScalarType( const AstNodeType& type)
{
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
// DQ (8/27/2006): change name of SgComplex to SgTypeComplex (for consistancy) and added SgTypeImaginary (C99 type).
  case V_SgTypeComplex:
  case V_SgTypeImaginary:
     return true;
  default: {}
  }
  return false;
}

string AstInterfaceBase:: GetTypeName( const AstNodeType& t)
{
  string name;
  assert( t != 0);
  GetTypeInfo(t, 0, &name);
  return name;
}

string AstInterfaceBase:: GetTypeSpec( const AstNodeType& t)
{
  string name;
  GetTypeInfo(t, &name);
  return name;
}


Boolean AstInterfaceBase::GetArrayBound( const AstNodePtr& arrayref, int dim, int &lb, int &ub) 
    { 
      SgArrayType *t = 0;
      if (dim == 0) {
         SgVarRefExp *var = isSgVarRefExp( arrayref );
         if (var == 0)
             return false;
         SgVariableSymbol *sb = var->get_symbol();
         SgType* vt = sb->get_type(); 
         t = isSgArrayType( vt);
      }
      else {
         SgNode *n = arrayref;
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
      return IsConstInt( exp, &ub);
    }

Boolean AstInterfaceBase::
IsExpression( const AstNodePtr& s, AstNodeType* exptype)
{
  if (IsVarRef(s, exptype))
     return true;
  SgExpression* exp = isSgExpression(s);

// DQ (12/13/2006): Test this to see if it could be a SgExprStatement.
  ROSE_ASSERT(isSgExprStatement(s) == NULL);

  if (exp != 0) {
    if (exptype != 0)
      *exptype = exp->get_type();
    if (DebugType()) 
      cerr << "expression " << AstToString(s) << " has type " << GetTypeName(*exptype) << endl;
  }
  return exp != 0;
}

Boolean AstInterfaceBase::
IsLoop( const AstNodePtr& s, AstNodePtr* init, AstNodePtr* cond,
	AstNodePtr* incr, AstNodePtr* body)
{
  switch (s->variantT()) {
  case V_SgForStatement:
    {
      SgForStatement *f = isSgForStatement(s);

   // DQ (3/16/2006): Added to make fur this is valid
      if (f->get_for_init_stmt() == NULL)
         {
           printf ("Warning: In AstInterfaceBase::IsLoop(): f->get_for_init_stmt() == NULL \n");
         }
   // ROSE_ASSERT(f->get_for_init_stmt() != NULL);


      if (init != 0)
         *init = f->get_for_init_stmt(); 
      if (incr != 0)
         *incr = f->get_increment();

      if (cond != 0)
         {
        // DQ (3/16/2006): the test is now a SgStatement instead of a SgExpression
        // (has to be in C and C++ because that is what the language requires).
        // get_test_expr() is depricated.
        // *cond = f->get_test_expr();
           *cond = f->get_test();
         }

      if (body != 0)
         *body = f->get_loop_body();
    }
    break;
  case V_SgWhileStmt:
    {
      SgWhileStmt* w = isSgWhileStmt(s);
      if (init != 0)
         *init = 0;
      if (incr != 0)
         *incr = 0;
      if (cond != 0)
         *cond = w->get_condition();
      if (body != 0)
         *body = w->get_body();
    }
    break;
  case V_SgDoWhileStmt:
    {
      SgDoWhileStmt *w = isSgDoWhileStmt(s);
      if (init != 0)
         *init = 0;
      if (incr != 0)
         *incr = 0;
      if (cond != 0)
         {
        // DQ (3/16/2006): This will be a SgExpression, instead of a SgStatement someday
           *cond = w->get_condition();
         }
      if (body != 0)
         *body = w->get_body();
    }
    break;
  default:
    return false;
  }
  return true;
}

Boolean AstInterfaceBase::IsPostTestLoop( const AstNodePtr& s)
{
  switch (s->variantT()) {
  case V_SgDoWhileStmt:
    return true;
  default: {}
  }
  return false;
}

AstNodePtr AstInterface::
CreateLoop( const AstNodePtr& ivar, const AstNodePtr& lb, const AstNodePtr& ub, 
	    const AstNodePtr& step, const AstNodePtr& stmts)
{ 
  SgExpression* ivarexp = ToExpression(*this, ivar);
  SgExpression* ubexp = ToExpression(*this, ub);

  SgExpression* testExp = NULL;
  SgExpression* stepexp = ToExpression(*this, step);
  string stepString = step->unparseToString();
  if (atoi(stepString.c_str()) < 0) {
    testExp = new SgGreaterOrEqualOp( GetFileInfo(), ivarexp, ubexp);
  } else {
    testExp = new SgLessOrEqualOp( GetFileInfo(), ivarexp, ubexp);
  }
  assert (testExp);
  ivarexp->set_parent(testExp);
  ubexp->set_parent(testExp);
  SgExprStatement* test = new SgExprStatement (GetFileInfo(), testExp );
  testExp->set_parent(test);

  SgExpression* lbexp = (lb == 0)? 0 : ToExpression(*this, lb);
  SgNode *init = (lbexp == 0)? 0 : CreateAssignment(ivarexp, lbexp);
  SgStatement *initstmt = (init == 0)? 0 : isSgStatement(init);
  if ( initstmt == 0 && init != 0) {
     SgExpression *initexp = ToExpression(*this, init);
     initstmt = new SgExprStatement(GetFileInfo(), initexp);
  }
  SgExpression* ivarexp1 = isSgExpression(CopyAstTree(ivarexp));
  ROSE_ASSERT (ivarexp1);
  SgPlusAssignOp *incr = new SgPlusAssignOp( GetFileInfo(), ivarexp1, stepexp);
  ivarexp1->set_parent(incr);
  stepexp->set_parent(incr);

  SgBasicBlock *b = isSgBasicBlock(stmts);
  if (b == 0) {
    b = new SgBasicBlock(GetFileInfo());
    printf ("CreateLoop: Building SgBasicBlock = %p \n",b);
    SgStatement* stmt = ToStatement(stmts);
    ROSE_ASSERT (stmt);
    b->append_statement( stmt);
    stmt->set_parent(b);
  }
  ROSE_ASSERT (b);

  SgForStatement *result = new SgForStatement( GetFileInfo(), test, incr, b);
  test->set_parent(result);
  incr->set_parent(result);
  b->set_parent(result);
  if (initstmt) {
    result->append_init_stmt(initstmt);
    initstmt->set_parent(result->get_for_init_stmt());
  }
// printf ("CreateLoop: Building SgForStatement = %p \n",result);
  return result;
}


AstInterfaceBase::AstNodeList AstInterfaceBase::GetBasicBlockStmtList( const AstNodePtr& n)
{
  AstNodeList result;
  SgStatementPtrList l;
  switch (n->variantT()) {
  case V_SgBasicBlock:
    l = isSgBasicBlock(n)->get_statements();
    break;
  case V_SgForInitStatement:
    l = isSgForInitStatement(n)->get_init_stmt();
    break;
  case V_SgSwitchStatement:
       result.push_back(isSgSwitchStatement(n)->get_body());
       return result;
  default:  
      assert(false);
  }
  for (SgStatementPtrList::iterator p = l.begin(); p != l.end(); ++p) {
     result.push_back(*p);
  }

// printf ("result.size() = %zu in GetBasicBlockStmtList \n",result.size());
// ROSE_ASSERT(result.size() > 0);

  return result;
}

int AstInterfaceBase::GetBasicBlockSize( const AstNodePtr& n)
{
  SgStatementPtrList l;
  switch (n->variantT()) {
  case V_SgBasicBlock:
    l = isSgBasicBlock(n)->get_statements();
    break;
  case V_SgForInitStatement:
    l = isSgForInitStatement(n)->get_init_stmt();
    break;
  case V_SgSwitchStatement:
       return 1;
  default:
      assert(false);
  }

//printf ("l.size() = %zu in GetBasicBlockSize \n",l.size());

  return l.size();
}
                                                                                                 

AstNodePtr AstInterfaceBase::GetBasicBlockFirstStmt( const AstNodePtr& n)
{
  SgStatementPtrList l;
  switch (n->variantT()) {
  case V_SgBasicBlock:
    l = isSgBasicBlock(n)->get_statements();
    break;
  case V_SgForInitStatement:
    l = isSgForInitStatement(n)->get_init_stmt();
    break;
  case V_SgSwitchStatement:
       return isSgSwitchStatement(n)->get_body();
  default:  
      assert(false);
  }

//printf ("l.size() = %zu in GetBasicBlockFirstStmt \n",l.size());

  return (l.size() == 0)? 0 : l.front();
}

AstNodePtr AstInterfaceBase::GetBasicBlockLastStmt( const AstNodePtr& n)
{
  SgStatementPtrList l;
  switch (n->variantT()) {
  case V_SgBasicBlock:
    l = isSgBasicBlock(n)->get_statements();
    break;
  case V_SgForInitStatement:
    l = isSgForInitStatement(n)->get_init_stmt();
    break;
  case V_SgSwitchStatement:
       return isSgSwitchStatement(n)->get_body();
  default:  
      assert(false);
  }

//printf ("l.size() = %zu in GetBasicBlockLastStmt \n",l.size());

  if (l.size() > 0)
     return l.back();
  return 0;
}

AstNodePtr AstInterfaceBase::CreateConstInt( int val) {
  SgIntVal* iv = new SgIntVal(GetFileInfo(),val);
  iv->set_endOfConstruct(GetFileInfo());
  return iv;
}

AstNodePtr AstInterface::CreateConst( const string& val, const string& valtype) const
{
  if (valtype == "int") {
    int intval = atoi(val.c_str());
    SgIntVal* iv = new SgIntVal(GetFileInfo(),intval);
    iv->set_endOfConstruct(GetFileInfo());
    return iv; 
  } 
  else if (valtype == "bool") {
      int i = atoi(val.c_str());
      if (val == "true" || i != 0)
         return new SgBoolValExp(GetFileInfo(), -1);
      else
         return new SgBoolValExp(GetFileInfo(), 0);
  }
  else if (valtype == "string") {
         char *r = new char[val.size() + 1];
         strcpy( r, val.c_str());
      // return new SgStringVal(GetFileInfo(), r);
         SgStringVal *tmp = new SgStringVal(GetFileInfo(), r);
         ROSE_ASSERT(tmp != NULL);
         printf ("AstInterface::CreateConst (copying SgStringVal original = %p copy = %p \n",r,tmp);
         return tmp;
  }
  else if (valtype == "char") {
         return new SgCharVal(GetFileInfo(), val[0]);
  }
  else if (valtype == "float") {
         istringstream in(val);
         float num = 0;
         in >> num;
         return new SgFloatVal(GetFileInfo(), num);
  }
  else if (valtype == "double") {
         istringstream in(val);
         double num = 0;
         in >> num;
         return new SgDoubleVal(GetFileInfo(), num);
  }
  else if (valtype == "function") {
      SgFunctionSymbol *f = impl->GetFunc(val);
      if ( f == 0) {
         return 0;
      }
      SgFunctionRefExp *fr = new SgFunctionRefExp(GetFileInfo(), f);
      fr->set_endOfConstruct(GetFileInfo());
      return fr;
  }
  else if (valtype == "memberfunction") {
      char *start = 0;
      SgClassSymbol *c = impl->GetClass(val, &start);
      if (c == 0) {
         cerr << "Error: cannot find class declaration for " << val << endl;
         assert(false);
      }
      SgMemberFunctionSymbol *f1 = impl->GetMemberFunc( c, string(start));
      if (f1 == 0) {
         cerr << "Warning: cannot find member function " << val << endl;
         f1 = impl->NewMemberFunc(c, string(start), new SgTypeInt(), 
                                   list<SgInitializedName*>()); 
         //assert(false);
      SgName f1name = f1->get_name();
      }
      SgMemberFunctionRefExp *fr = new SgMemberFunctionRefExp(GetFileInfo(), f1);
      fr->set_need_qualifier(false);
      return fr;
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
      SgClassDefinition *def = decl->get_definition();
      assert(def != 0);
      SgVariableSymbol *vs = impl->GetVar( start, def);
      assert(vs != 0);
      return new SgVarRefExp(GetFileInfo(), vs);
  }
  else {
       cerr << "Error: non-recognized value type for creating constant AST: " << valtype << endl;
        assert(false);
  }
}

AstNodePtr AstInterface :: CreateFunction( const string& name, const string& decl)
{
     SgFunctionSymbol* f = impl->GetFunc( name);
     if (f == 0) {
        SgSymbol *r = impl->CreateDeclarationStmts(decl);
        f = isSgFunctionSymbol(r);
        assert( f != 0);
     }
     SgFunctionRefExp* result = new SgFunctionRefExp(GetFileInfo(), f);
     result->set_endOfConstruct(GetFileInfo());
     return result;
}

AstNodePtr AstInterface::
CreateUnaryOP( const string& op, const AstNodePtr& a0)
{
  if (op == "&") {
     SgExpression *e = ToExpression(*this, a0);
     SgAddressOfOp *addr = new SgAddressOfOp( GetFileInfo(), e, e->get_type());
     return addr; 
  }
  else if (op == "-") {
     SgExpression *e = ToExpression(*this, a0);
     SgMinusOp *r = new SgMinusOp( GetFileInfo(), e, e->get_type());
     return r; 
  }
  assert(false);
}

AstNodePtr AstInterface:: 
CreateBinaryOP( const string& op, const AstNodePtr& a0, const AstNodePtr& a1) const
{
  SgExpression *e0 = ToExpression(*this, a0);
  SgExpression *e1 = ToExpression(*this, a1);
  assert( e0 != 0 && e1 != 0);
  if (op == ".") {
      SgDotExp* n = new SgDotExp(GetFileInfo(), e0, e1, e1->get_type());
      return n;
  }
  else if (op == "+=") {
      SgNode* r = new SgPlusAssignOp( GetFileInfo(), e0, e1);
      return r;
  }
  else if (op == "[]") {
      return new SgPntrArrRefExp(GetFileInfo(), e0,e1);
  }
  else if (op == "/") {
      return new SgDivideOp( GetFileInfo(), e0,e1);
  }
  else if (op == "*") {
      return new SgMultiplyOp(GetFileInfo(), e0, e1);
  }
  cerr << "Error: non-recognized binary operator: " << op << endl;
  assert(false);
}

AstNodePtr AstInterface::
CreateArrayAccess( const AstNodePtr& arr, const AstNodeList& index)
{
  AstNodePtr r = arr;
  for (AstNodeList::const_reverse_iterator p = index.rbegin(); p != index.rend();
       ++p) {
     r = CreateBinaryOP("[]", r, (*p));
  }
  return r;
}

AstNodePtr AstInterface::
CreateBinaryPlus( const AstNodePtr& a0, const AstNodePtr& a1) 
{ 
  SgExpression *e0 = ToExpression(*this, a0);
  SgExpression *e1 = ToExpression(*this, a1);
  SgAddOp* r = new SgAddOp(GetFileInfo(), e0,e1);
  e0->set_parent(r);
  e1->set_parent(r);
  r->set_endOfConstruct(GetFileInfo());
  return r;
}
AstNodePtr AstInterface::
CreateBinaryTimes( const AstNodePtr& a0, const AstNodePtr& a1) 
{  
  SgExpression *e0 = ToExpression(*this, a0);
  SgExpression *e1 = ToExpression(*this, a1);
  SgMultiplyOp* r = new SgMultiplyOp(GetFileInfo(), e0,e1);
  e0->set_parent(r);
  e1->set_parent(r);
  r->set_endOfConstruct(GetFileInfo());
  return r;
}

AstNodePtr AstInterface::
CreateBinaryMinus( const AstNodePtr& a0, const AstNodePtr& a1)
{
  SgExpression *e0 = ToExpression(*this, a0);
  SgExpression *e1 = ToExpression(*this, a1);
  SgSubtractOp* r = new SgSubtractOp(GetFileInfo(), e0,e1);
  e0->set_parent(r);
  e1->set_parent(r);
  r->set_endOfConstruct(GetFileInfo());
  return r;
}

AstNodePtr GetOverloadOperatorOpd1( const AstNodePtr& exp)
{
  SgFunctionCallExp *fs = isSgFunctionCallExp(exp);
  assert(fs != 0);
  SgExpression *func = fs->get_function();
  if (func->variantT() == V_SgDotExp) {
    return   isSgDotExp(func)->get_lhs_operand();
  }
  SgExpressionPtrList &l = fs->get_args()->get_expressions();
  return l.front();
}

AstNodePtr GetOverloadOperatorOpd2( const AstNodePtr& exp)
{
  SgFunctionCallExp *fs = isSgFunctionCallExp(exp);
  assert(fs != 0);
  SgExpressionPtrList &l = fs->get_args()->get_expressions();
  return l.back();
}

AstNodePtr AstInterface::
CreateRelEQ( const AstNodePtr& a0, const AstNodePtr& a1) 
{ 
  SgExpression *e0 = ToExpression(*this, a0);
  SgExpression *e1 = ToExpression(*this, a1);
  SgLocatedNode* r = new SgEqualityOp( GetFileInfo(), e0,e1);
  e0->set_parent(r);
  e1->set_parent(r);
  r->set_endOfConstruct(GetFileInfo());
  return r;
}

AstNodePtr AstInterface::
CreateRelNE( const AstNodePtr& a0, const AstNodePtr& a1) 
{ 
  SgExpression *e0 = ToExpression(*this, a0);
  SgExpression *e1 = ToExpression(*this, a1);
  SgLocatedNode* r = new SgNotEqualOp( GetFileInfo(), e0,e1);
  e0->set_parent(r);
  e1->set_parent(r);
  r->set_endOfConstruct(GetFileInfo());
  return r;
}

AstNodePtr AstInterface::CreateRelLT( const AstNodePtr& a0, const AstNodePtr& a1) 
{ 
  SgExpression *e0 = ToExpression(*this, a0);
  SgExpression *e1 = ToExpression(*this, a1);
  SgLocatedNode* r = new SgLessThanOp( GetFileInfo(), e0,e1);
  e0->set_parent(r);
  e1->set_parent(r);
  r->set_endOfConstruct(GetFileInfo());
  return r;
}

AstNodePtr AstInterface::CreateRelGT( const AstNodePtr& a0, const AstNodePtr& a1) 
{ 
  SgExpression *e0 = ToExpression(*this, a0);
  SgExpression *e1 = ToExpression(*this, a1);

  SgLocatedNode* r = new SgGreaterThanOp(GetFileInfo(), e0,e1);
  e0->set_parent(r);
  e1->set_parent(r);
  r->set_endOfConstruct(GetFileInfo());
  return r;
}

AstNodePtr AstInterface::CreateRelLE( const AstNodePtr& a0, const AstNodePtr& a1) 
{ 
  SgExpression *e0 = ToExpression(*this, a0);
  SgExpression *e1 = ToExpression(*this, a1);
  SgLocatedNode* r = new SgLessOrEqualOp(GetFileInfo(), e0,e1);
  e0->set_parent(r);
  e1->set_parent(r);
  r->set_endOfConstruct(GetFileInfo());
  return r;
}

AstNodePtr AstInterface::CreateRelGE( const AstNodePtr& a0, const AstNodePtr& a1) 
{ 
  SgExpression *e0 = ToExpression(*this, a0);
  SgExpression *e1 = ToExpression(*this, a1);
  SgLocatedNode* r = new SgGreaterOrEqualOp(GetFileInfo(), e0,e1);
  e0->set_parent(r);
  e1->set_parent(r);
  r->set_endOfConstruct(GetFileInfo());
  return r;
}

AstNodePtr AstInterface::
CreateLogicalAND( const AstNodePtr& a0, const AstNodePtr& a1) 
{ 
  SgExpression *e0 = ToExpression(*this, a0);
  SgExpression *e1 = ToExpression(*this, a1);
  SgLocatedNode* r = new SgAndOp(GetFileInfo(), e0,e1);
  e0->set_parent(r);
  e1->set_parent(r);
  r->set_endOfConstruct(GetFileInfo());
  return r;
}

AstNodePtr AstInterface::
CreateFunctionCall( const string& func, const string& decl, AstNodeList args)
 {
    AstNodePtr f = CreateFunction(func, decl);
    return AstInterfaceBase::CreateFunctionCall(f, args);
 }

AstNodePtr AstInterfaceBase::
CreateFunctionCall( const AstNodePtr& func, AstNodeList args)
    {
      SgExpression *fr = isSgExpression(func);

      SgExprListExp *argexp = new SgExprListExp(GetFileInfo());
      argexp->set_endOfConstruct(GetFileInfo());
      SgExpressionPtrList &l = argexp->get_expressions();
      AstNodeList::iterator p = args.begin(); 
      if (fr->variantT() == V_SgMemberFunctionRefExp) {
         SgExpression* obj = isSgExpression(*p);
         assert(obj != 0);
         ++p;
         SgExpression* oldFr = fr;
         if (obj->get_type()->variantT() == V_SgPointerType)
            fr = new SgArrowExp(GetFileInfo(), obj, fr);
         else 
            fr = new SgDotExp(GetFileInfo(), obj, fr);
         oldFr->set_parent(fr);
         obj->set_parent(fr);
         fr->set_endOfConstruct(GetFileInfo());
      }
      for ( ;p != args.end(); ++p) {
         SgExpression *pr = isSgExpression( (*p) );
	 ROSE_ASSERT (argexp);
         pr->set_parent(argexp);
         l.push_back( pr );
      }
      SgFunctionCallExp *result = new SgFunctionCallExp(GetFileInfo(), fr, argexp);
      fr->set_parent(result);
      argexp->set_parent(result);
      result->set_endOfConstruct(GetFileInfo());
      return result;
    }

AstInterfaceBase::AstNodeList AstInterfaceBase::CreateList() 
    { return AstNodeList(); } 
AstInterfaceBase::AstNodeListIterator 
AstInterfaceBase::GetAstNodeListIterator( AstNodeList& l)
    { return AstNodeListIterator(l); }
AstInterfaceBase::AstTypeListIterator AstInterfaceBase::GetAstTypeListIterator( AstTypeList& l)
    { return AstTypeListIterator(l); }
void AstInterfaceBase::ListAppend(  AstNodeList& list, const AstNodePtr& s) 
    {  list.push_back(s); }
void AstInterfaceBase::ListPrepend(  AstNodeList& list, const AstNodePtr& s) 
    {  list.push_front(s); }
AstNodePtr AstInterfaceBase::CreateIf( const AstNodePtr& cond, const AstNodePtr& stmts) 
    { 
      SgStatement* c = ToStatement(cond);
      SgBasicBlock *b = isSgBasicBlock(stmts);
      if (b == 0) {
         b = new SgBasicBlock( GetFileInfo());
         b->set_endOfConstruct(GetFileInfo());
         printf ("CreateIf: Building SgBasicBlock = %p \n",b);
         SgStatement* stmt = ToStatement(stmts);
         b->append_statement(stmt );
         stmt->set_parent(b);
      }
      SgBasicBlock* elseBb = new SgBasicBlock(GetFileInfo());
      elseBb->set_endOfConstruct(GetFileInfo());
      SgIfStmt* r = new SgIfStmt( GetFileInfo(), c, b, elseBb);
      r->set_endOfConstruct(GetFileInfo());
      c->set_parent(r);
      b->set_parent(r);
      elseBb->set_parent(r);
      return r;
    }

AstNodePtr AstInterfaceBase::CreateBasicBlock( const AstNodePtr& orig) 
 { 
    SgBasicBlock *r = new SgBasicBlock( GetFileInfo()); 
    printf ("CreateBasicBlock: Building SgBasicBlock = %p \n",r);
    if (orig != 0) {
       SgBasicBlock *r1 = isSgBasicBlock(orig);
       if (r1 != 0)
          r->getAttachedPreprocessingInfo() = r1->getAttachedPreprocessingInfo();
    }
    return r;
 }

void AstInterfaceBase::BasicBlockAppendStmt( AstNodePtr& b, const AstNodePtr& s)
    { 
      SgBasicBlock *basicBlock = isSgBasicBlock(b), *sb = isSgBasicBlock(s);
      ROSE_ASSERT (basicBlock);
      if (sb == 0)  {
	SgStatement* sStmt = ToStatement(s);
         basicBlock->append_statement( sStmt); 
	 sStmt->set_parent(basicBlock);
	 ROSE_ASSERT (s->get_parent());
      } else {
         SgStatementPtrList l = sb->get_statements();
         for (SgStatementPtrList::iterator p = l.begin(); p != l.end(); ++p) {
              basicBlock->append_statement(*p);
	      (*p)->set_parent(basicBlock);
         } 
         sb->get_statements().clear();
         sb->set_parent(0);
         delete sb;
      }
    }
 
void AstInterfaceBase::
BasicBlockPrependStmt( AstNodePtr& b, const AstNodePtr& s)
    { 
      SgBasicBlock *basicBlock = isSgBasicBlock(b), *sb = isSgBasicBlock(s);
      if (sb == 0) {
        SgStatement* sStmt = ToStatement(s);
         basicBlock->prepend_statement( sStmt); 
         sStmt->set_parent(basicBlock);
	 ROSE_ASSERT (s->get_parent());
      } else {
         SgStatementPtrList l = sb->get_statements();
         for (SgStatementPtrList::iterator p = l.begin(); p != l.end(); ++p) {
              basicBlock->prepend_statement(*p);
	      (*p)->set_parent(basicBlock);
         } 
      }
    }
void AstInterfaceBase::
InsertStmt(AstNodePtr const & orig, AstNodePtr const &n, bool insertbefore,
           bool extractfromBasicBlock)
{
   SgStatement *s = isSgStatement(orig), *ns = ToStatement(n);
   assert(s != 0);
   SgStatement *p = isSgStatement(s->get_parent());
   assert(p != 0);
   ns->set_parent(p);
   if (extractfromBasicBlock)
      p->insert_statement_from_basicBlock(s, ns, insertbefore);
   else
      p->insert_statement(s, ns, insertbefore);
}

bool AstInterface::RemoveStmt( const AstNodePtr& n)
{
   SgStatement* s = isSgStatement(n);
   assert (s != 0); 
   SgStatement* p = isSgStatement(n->get_parent());
   assert( p != 0);
   p->remove_statement(s);
   // s->set_parent(0); // JJW 10-26-2007 Commented out on suggestion from Dan to make AST tests work temporarily
   cerr << "Removed statement " << s << endl;

// DQ (9/4/2005): this should return something
   return false;
}

bool AstInterface::ReplaceAst( const AstNodePtr& orig, const AstNodePtr& n)
   { 
     SgStatement *s = isSgStatement(orig);
     if (s != 0)
        {
          SgStatement *p = isSgStatement(s->get_parent());
          assert(p != 0);
          SgStatement *ns = ToStatement(n);
          p->replace_statement_from_basicBlock(s, ns);
        }
       else
        {
          SgExpression *e = isSgExpression(orig), *ne = isSgExpression(n);
          assert(e != 0 && ne != 0);
          SgNode *p = e->get_parent();
          SgExpression *pe = isSgExpression(p);
          if (pe != 0)
             {
               pe->replace_expression(e, ne);
             }
            else
             {
#if 0
               SgInitializedName *pn = isSgInitializedName(p);
               if (pn == NULL)
                  {
                    printf ("What is this: e  = %p = %s \n",e,e->class_name().c_str());
                    printf ("What is this: ne = %p = %s \n",ne,ne->class_name().c_str());
                    assert(p != NULL);
                    printf ("What is this: p = %p = %s \n",p,p->class_name().c_str());
                    printf ("What is this: p->get_parent() = %p = %s \n",p->get_parent(),p->get_parent()->class_name().c_str());
                    e->get_file_info()->display("Location of problem code: debug");
                  }
               assert(pn != 0);
               SgAssignInitializer *init = new SgAssignInitializer(GetFileInfo(), ne, ne->get_type() );
               pn->set_initializer( init);
#else
#if 0
               printf ("What is this: p = %p = %s \n",p,p->class_name().c_str());
               printf ("What is this: e  = %p = %s \n",e,e->class_name().c_str());
               printf ("What is this: ne = %p = %s \n",ne,ne->class_name().c_str());
               assert(p != NULL);
               printf ("What is this: p = %p = %s \n",p,p->class_name().c_str());
               printf ("What is this: p->get_parent() = %p = %s \n",p->get_parent(),p->get_parent()->class_name().c_str());
               e->get_file_info()->display("Location of problem code: debug");
#endif
            // DQ (12/6/2006): Note that the for loop conditional is not longer an expression 
            // and so we have to handle the additional case of an expression statement here.
            // This is also required because we have eliminated the SgExpressionRoot, which 
            // permitted a more uniform handling of expressions without this additional case!
               SgStatement* statement = isSgStatement(p);
               if (statement != NULL)
                  {
                    statement->replace_expression(e, ne);
                  }
                 else
                  {
                    SgInitializedName *pn = isSgInitializedName(p);
#if 0
                    if (pn == NULL)
                       {
                         printf ("What is this: e  = %p = %s \n",e,e->class_name().c_str());
                         printf ("What is this: ne = %p = %s \n",ne,ne->class_name().c_str());
                         assert(p != NULL);
                         printf ("What is this: p = %p = %s \n",p,p->class_name().c_str());
                         printf ("What is this: p->get_parent() = %p = %s \n",p->get_parent(),p->get_parent()->class_name().c_str());
                         e->get_file_info()->display("Location of problem code: debug");
                       }
#endif
                    assert(pn != 0);
                    SgAssignInitializer *init = new SgAssignInitializer(GetFileInfo(), ne, ne->get_type() );
		    ne->set_parent(init);
		    init->set_endOfConstruct(GetFileInfo());
                    pn->set_initializer( init);
		    ROSE_ASSERT (pn);
		    init->set_parent(pn);
                  }
#endif
             }

        }

     cerr << "Replaced " << orig << " with " << n << endl;
     n->set_parent(orig->get_parent());
     // orig->set_parent(0); // JJW 10-26-2007 Commented this out because Dan said it would make the AST tests work (temporarily)
     return true;
   }

//typedef Boolean BoolAttribute;
class BoolAttribute
{
  Boolean val;
 public:
  BoolAttribute( Boolean v = true) : val(v) {}
  operator Boolean() const { return val; } 
};

class SageProcessAstNode : public AstTopDownBottomUpProcessing<BoolAttribute,BoolAttribute> 
{
   AstInterface *fa;
   AstInterface::TraversalOrderType t;
   ProcessAstNode& op;
   BoolAttribute evaluateInheritedAttribute(SgNode* astNode, BoolAttribute inheritedValue)
     { if (t == AstInterface::PostOrder)
          return inheritedValue;
       return inheritedValue? op.Traverse( *fa, astNode, AstInterface::PreVisit) : false; 
     }
   BoolAttribute evaluateSynthesizedAttribute(SgNode* astNode, BoolAttribute inheritedValue, 
                                                 SynthesizedAttributesList l)
    {   if (t == AstInterface::PreOrder)
           return inheritedValue; 
        if (! inheritedValue)
            return false;
        for (unsigned int i = 0; i < l.size(); ++i) 
           if (!l[i])
              return false;
        return op.Traverse( *fa, astNode, AstInterface::PostVisit); 
    }
  public:
   SageProcessAstNode( ProcessAstNode& _op) : op(_op) {}
   Boolean Traverse( AstInterface *_fa, SgNode* node, AstInterface::TraversalOrderType _t)
       { fa = _fa;
         t = _t; 
         return AstTopDownBottomUpProcessing<BoolAttribute,BoolAttribute>::traverse(node, true);
       }
};


Boolean 
ReadAstTraverse( AstInterface& fa, const AstNodePtr& root, ProcessAstNode& op,
                 AstInterface::TraversalOrderType t)
{
   return SageProcessAstNode(op).Traverse(&fa, root, t);
}

template <class Transform>
class PerformPreTransformationTraversal 
 : public AstTopDownBottomUpProcessing<_DummyAttribute, AstNodePtr>
{
 // DQ (9/4/2005): Swapped order of data members to avoid compiler warnings
    AstInterface& fa;
    Transform& op;
    AstNodePtr result, orig;
    bool succ;
    SgNode* head;

    _DummyAttribute evaluateInheritedAttribute(SgNode* astNode, _DummyAttribute a)
     {
        cerr << "PerformPreTransformationTraversal::evaluateInheritedAttribute " << astNode->class_name() << endl;
        if (!succ) {
          succ = op(fa, astNode, result);
          if (succ) {
               assert(result != 0);
	       if (!result->get_parent()) result->set_parent(astNode->get_parent()); // JJW 10-26-2007 Is this the right way to solve the problem of NULL parent pointers in the results output from op()?
	       ROSE_ASSERT (result->get_parent());
               orig = astNode;
          }
        }
       return _DummyAttribute();
     }
   AstNodePtr evaluateSynthesizedAttribute(SgNode* astNode, _DummyAttribute a, 
                                               SynthesizedAttributesList l)
      {
        cerr << "PerformPreTransformationTraversal::evaluateSynthesizedAttribute " << astNode->class_name() << endl;
          if (astNode == orig) {
              orig = 0;
              succ = false;
              if (result != astNode && (astNode != head || astNode->get_parent() != 0)) {
                 result->set_parent(NULL); // JJW 10-26-2007 This is required by ReplaceAst
                 fa.ReplaceAst( astNode, result);
                 ROSE_ASSERT (result->get_parent());
              }
              return result;
          }
          else 
            return astNode;
      }

  public:
    PerformPreTransformationTraversal( AstInterface& _fa, Transform& _op)
       : fa(_fa), op(_op), result(0), orig(0), succ(false) { }
    AstNodePtr operator() ( SgNode* n)
          {
             cerr << "PerformPreTransformationTraversal::operator()" << endl;
             succ = false;
             head = n;
             return AstTopDownBottomUpProcessing<_DummyAttribute, AstNodePtr>::
                           traverse(n, _DummyAttribute());
          }
};

template <class Transform>
class PerformPostTransformationTraversal : public AstBottomUpProcessing<AstNodePtr>
{
  SgNode *head;
    AstInterface& fa;
    Transform& op;
  AstNodePtr evaluateSynthesizedAttribute(SgNode* astNode, SynthesizedAttributesList l)
      {
	cerr << "PerformPostTransformationTraversal::evaluateSynthesizedAttribute " << astNode->class_name() << endl;
          AstNodePtr r  = astNode;
          if (op(fa, astNode, r) && r != astNode) {
              assert(r != 0);
              if (r != astNode && (astNode != head || astNode->get_parent() != 0))
                 fa.ReplaceAst( astNode, r);
              return r;
          }
          else 
            return astNode;
      }

  public:
    PerformPostTransformationTraversal( AstInterface& _fa, Transform& _op)
       : fa(_fa), op(_op) { }
    AstNodePtr operator() ( SgNode* n)
      {
         head = n;
         return AstBottomUpProcessing<AstNodePtr>::traverse(n);
      }
};

AstNodePtr TransformAstTraverse( AstInterface& fa, const AstNodePtr& r, 
          bool (*op)( AstInterface&, const AstNodePtr&, AstNodePtr&),
                                AstInterface::TraversalVisitType t )
{
  cerr << "TransformAstTraverse on function" << endl;
  if (t == AstInterface::PreVisit) {
     PerformPreTransformationTraversal<bool (*)(AstInterface&, const AstNodePtr&, 
                                                AstNodePtr& ) > traverse(fa, op);
     return traverse(r);
  }
  else {
     PerformPostTransformationTraversal<bool (*)(  AstInterface&, const AstNodePtr&, 
                                                    AstNodePtr&) > traverse(fa, op);
     return traverse(r);
  }
}

AstNodePtr TransformAstTraverse( AstInterface& fa, const AstNodePtr& r, 
                              TransformAstTree& op, 
                              AstInterface::TraversalVisitType t) 
{
  cerr << "TransformAstTraverse on TransformAstTree " << r << endl;
  if (t == AstInterface::PreVisit) {
     cerr << "PreVisit" << endl;
     PerformPreTransformationTraversal<TransformAstTree> traverse(fa, op);
     AstNodePtr result = traverse(r);
     return result;
  }
  else {
     cerr << "PostVisit" << endl;
     PerformPostTransformationTraversal<TransformAstTree> traverse(fa, op);
     AstNodePtr result = traverse(r);
     return result;
  }
}

// DQ (12/28/2005): ROSE can't compile these lines and they appear to not make any sence, 
// and it seems that we don't need them, so let's comment them out.
// template BoolAttribute * __uninitialized_copy_aux<BoolAttribute const *, BoolAttribute *>(BoolAttribute const *, BoolAttribute const *, BoolAttribute *, __false_type);
// template BoolAttribute * __uninitialized_copy_aux<BoolAttribute *, BoolAttribute *>(BoolAttribute *, BoolAttribute *, BoolAttribute *, __false_type);
template class PerformPreTransformationTraversal<bool (*)(AstInterface &, AstNodePtr const &, AstNodePtr &)>;
template class PerformPostTransformationTraversal<bool (*)(AstInterface &, AstNodePtr const &, AstNodePtr &)>;
template class PerformPreTransformationTraversal<TransformAstTree>;
template class PerformPostTransformationTraversal<TransformAstTree>;
template class list<SgExpression *, allocator<SgExpression *> >;
template class vector<AstNodePtr, allocator<AstNodePtr> >;
template class AstTopDownBottomUpProcessing<_DummyAttribute, AstNodePtr>;
template class AstBottomUpProcessing<AstNodePtr>;
template class SgTreeTraversal<_DummyAttribute, AstNodePtr>;
