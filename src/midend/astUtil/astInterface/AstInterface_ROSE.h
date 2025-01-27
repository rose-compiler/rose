#ifndef AST_INTERFACE_IMPL_H
#define AST_INTERFACE_IMPL_H

#include <list>
#include <string>
#include <iostream>
#include <map>
#include <stdexcept>
#include "ObserveObject.h"
#include "AstInterface.h"

class SgNode;
class SgType;
class SgVarRefExp;
class SgExpression;
class SgDotExp;

typedef AstNodePtr AstNodePtrImpl;

class AstNodeTypeImpl : public AstNodeType {
 public:
  AstNodeTypeImpl( SgType* n = 0) : AstNodeType(n) {}
  AstNodeTypeImpl( const AstNodeType& that) : AstNodeType(that) {}
  AstNodeType& operator = (const AstNodeType &that) 
      { AstNodeType::operator = (that); return *this; }
  ~AstNodeTypeImpl() {}
  SgType* get_ptr() const { return static_cast<SgType*>(AstNodeType::get_ptr()); }
  SgType* operator -> () const { return static_cast<SgType*>(AstNodeType::get_ptr()); }
};

#define AstNodePtr2Sage(a)  AstNodePtrImpl(a).get_ptr()

class SgScopeStatement;
class SgBasicBlock;
class SgGlobal;
class SgSymbol;
class SgClassSymbol;
class SgMemberFunctionRefExp;
class SgFunctionSymbol;
class SgVariableSymbol;
class SgStatement;
class SgMemberFunctionSymbol;
class SgClassDeclaration;
class SgVariableDeclaration;
class SgMemberFunctionDeclaration;
class SgInitializedName;
class SgFunctionDeclaration;
class SgClassDefinition;
class AstInterfaceImpl : public ObserveObject< AstObserver>
{
 public:
  AstInterfaceImpl( SgNode* _top)  : newVarIndex(0)
   { set_top(_top); }

  ~AstInterfaceImpl() { }

  typedef AstInterface::AstNodeList AstNodeList;
  typedef AstInterface::AstTypeList AstTypeList;

  SgScopeStatement* get_scope(SgNode* l) { 
     if (scope == 0) 
        set_top(l);
     return scope; }  
  void set_top( SgNode* _top);

  static void GetTypeInfo( SgType* t, std:: string* name = 0, 
                           std:: string* stripname = 0, int* size = 0, bool use_global_unique_name=false);
  static SgType* GetTypeInt();

  SgVarRefExp* CreateFieldRef(SgNode* decl, std::string fieldname);
  SgMemberFunctionRefExp* CreateMethodRef(std::string classname, 
                           std::string fieldname, bool createIfNotFound);

  SgSymbol* CreateDeclarationStmts( const std:: string& _decl);
  SgClassSymbol* NewClass( const std:: string& name);
  SgClassSymbol* AddClass( SgClassDeclaration* d) ;

  SgFunctionSymbol* GetFunc( const std:: string& name);
  SgFunctionSymbol* NewFunc( const std:: string& name, SgType* rtype, 
                             const std:: list<SgInitializedName*>& args );
  SgFunctionSymbol* AddFunc( SgFunctionDeclaration *d);

  SgNode* CreateFunction( std::string name, int numOfPars);
  SgNode* CreateFunctionCall( SgNode* func, AstNodeList::const_iterator, AstNodeList::const_iterator e);
  static bool IsFunctionCall( SgNode* s, SgNode** func, AstNodeList* args, AstInterfaceImpl::AstTypeList*, AstNodeType*);

  /*QY: if yes, set ivar,lb,ub,step, and body accordingly */
  static bool IsFortranLoop( const SgNode* s, SgNode** ivar = 0,
                       SgNode** lb = 0, SgNode** ub=0,
                       SgNode** step =0, SgNode** body=0);

  /*QY: if yes, set lhs, rhs and readlhs accordingly */
  static bool IsAssignment( SgNode* s, SgNode** lhs = 0, 
                               SgNode** rhs = 0, bool* readlhs = 0); 

  /*QY: if yes, set vartype, varname,scope, and isglobal accordingly*/
  static bool IsVarRef( SgNode* exp, SgType** vartype = 0,
                   std::string* varname = 0, SgNode** scope = 0, 
                    bool *isglobal = 0, bool use_global_unique_name = false) ;

  SgMemberFunctionSymbol* 
  NewMemberFunc( SgClassDeclaration *decl, const std:: string& name,
                SgType* rtype, const std:: list<SgInitializedName*>& args );
  SgMemberFunctionSymbol* AddMemberFunc( SgClassDefinition *def, 
                                      SgMemberFunctionDeclaration *d);
  /* Create a new member function if not found */
  SgMemberFunctionSymbol* GetMemberFunc( SgClassDeclaration* c, 
                   const std::string& funcname, std::vector<SgExpression*>* args = 0);

  /*QY: if declDecl=true,variable declarations are saved for insertion later*/
  SgVariableSymbol* NewVar ( SgType* type, const std:: string& _name, 
        bool makeunique, bool delayDecl, SgExpression* initexp=0,
                             SgScopeStatement* loc=0);
  /*Insert and clear all the new variable declarations created so far*/
  void AddNewVarDecls(); 
  /* copy new var declarations to a new given basic block; by default, the new var declarations are removed from their original block*/
  void CopyNewVarDecls(SgBasicBlock* blk, bool clearNewVars=true);

  /* save d for future insertion */
  void SaveVarDecl( SgVariableDeclaration *d, SgScopeStatement* curscope);

  SgFunctionSymbol* LookupFunction(const char* start, SgScopeStatement* loc);
  static SgNode* LookupNestedDeclaration(const std::string& name, SgNode* loc); 
  static SgVariableSymbol* LookupVar(const std:: string& name, SgNode* loc);
  SgVariableSymbol* LookupVar(const std:: string& name);
  SgVariableSymbol* InsertVar(SgInitializedName *d, SgScopeStatement* curscope=0);

  SgExpression* CreateVarRef(std::string varname, SgNode* loc=0);


  SgDotExp* CreateVarMemberRef(std::string varname, std::string field, 
                                  SgNode* loc=0);

  bool ReplaceAst( SgNode* orig, SgNode* n);
  static SgScopeStatement* GetScope( SgNode* loc);

 private:
  SgGlobal* global;
  SgScopeStatement* scope;
  int newVarIndex;
  std::vector< std::pair<SgScopeStatement*,SgVariableDeclaration*> > newVarList;
 friend class AstInterface;
};

#endif
