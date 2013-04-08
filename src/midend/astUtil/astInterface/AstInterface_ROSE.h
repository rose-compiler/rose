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
class AstNodePtrImpl : public AstNodePtr {
 public:
  AstNodePtrImpl( SgNode* n = 0) { AstNodePtr::repr = n; }
  AstNodePtrImpl( const AstNodePtr& that) : AstNodePtr(that) {}
  AstNodePtr& operator = (const AstNodePtr &that) 
      { AstNodePtr::operator = (that); return *this; }
  ~AstNodePtrImpl() {}
  SgNode* get_ptr() const { return static_cast<SgNode*>(repr); }
  SgNode* operator -> () const { return static_cast<SgNode*>(repr); }
};


class AstNodeTypeImpl : public AstNodeType {
 public:
  AstNodeTypeImpl( SgType* n = 0) { AstNodeType::repr = n; }
  AstNodeTypeImpl( const AstNodeType& that) : AstNodeType(that) {}
  AstNodeType& operator = (const AstNodeType &that) 
      { AstNodeType::operator = (that); return *this; }
  ~AstNodeTypeImpl() {}
  SgType* get_ptr() const { return static_cast<SgType*>(repr); }
  SgType* operator -> () const { return static_cast<SgType*>(repr); }
};

#define AstNodePtr2Sage(a)  AstNodePtrImpl(a).get_ptr()

class SgScopeStatement;
class SgGlobal;
class SgSymbol;
class SgFunctionSymbol;
class SgVariableSymbol;
class SgStatement;
class SgClassSymbol;
class SgMemberFunctionSymbol;
class SgClassDeclaration;
class SgVariableDeclaration;
class SgMemberFunctionDeclaration;
class SgInitializedName;
class SgFunctionDeclaration;
class SgClassDefinition;
class ROSE_DLL_API AstInterfaceImpl : public ObserveObject< AstObserver>
{
  typedef AstInterfaceImpl _Self;

 public:
  AstInterfaceImpl( SgNode* _top)  : newVarIndex(0), delayNewVarInsert(0)
   { set_top(_top); }

  ~AstInterfaceImpl() { }


  typedef AstInterface::AstNodeList AstNodeList;
  typedef AstInterface::AstTypeList AstTypeList;

  SgNode* get_top() const { return top; }  
  void set_top( SgNode* _top);
  SgScopeStatement* get_scope() const { return scope; }

  static void GetTypeInfo( const AstNodeType& t, std:: string* name = 0, 
                           std:: string* stripname = 0, int* size = 0);
  SgFunctionSymbol* LookupFunction(const char* start) const;
  SgClassSymbol* LookupClass(const char* start) const;
  SgSymbol* CreateDeclarationStmts( const std:: string& _decl);
  SgClassSymbol* GetClass( const std:: string& name, char** start = 0);
  SgClassSymbol* NewClass( const std:: string& name);
  SgClassSymbol* AddClass( SgClassDeclaration* d) ;

  SgFunctionSymbol* GetFunc( const std:: string& name);
  SgFunctionSymbol* NewFunc( const std:: string& name, SgType* rtype, 
                             const std:: list<SgInitializedName*>& args );
  SgFunctionSymbol* AddFunc( SgFunctionDeclaration *d);

  SgNode* CreateFunction( std::string name, int numOfPars);
  SgNode* CreateFunctionCall( SgNode* func, const AstNodeList& args);
  bool IsFunctionCall( SgNode* s, SgNode** func, AstNodeList* args);

  SgMemberFunctionSymbol* NewMemberFunc( SgClassSymbol *decl, const std:: string& name,
                                 SgType* rtype, const std:: list<SgInitializedName*>& args );
  SgMemberFunctionSymbol* AddMemberFunc( SgClassDefinition *def, 
                                      SgMemberFunctionDeclaration *d);
  SgVariableSymbol* NewVar ( SgType* type, const std:: string& _name, bool makeunique, 
                             SgScopeStatement* loc=0);
  SgVariableSymbol* GetVar( const std:: string& name, SgScopeStatement* loc = 0);
  SgVariableSymbol* AddVar( SgVariableDeclaration *d, SgScopeStatement* curscope = 0);
  SgVariableSymbol* AddVar( SgInitializedName *d, SgScopeStatement* curscope=0);
  SgNode* GetVarDecl( const std:: string& varname);
  bool ReplaceAst( SgNode* orig, SgNode* n);

  void delay_newVarInsert() { ++delayNewVarInsert; }
  void apply_newVarInsert() {
      --delayNewVarInsert;
      if (delayNewVarInsert > 0) return;
      assert(delayNewVarInsert == 0);
      AddNewVarDecls();
   } 

 void AddNewVarDecls();
 void AddNewVarDecls(SgScopeStatement* nblock, SgScopeStatement* oldblock);

public:
  ///  Arbitary debug function.
  void
  _debug(const AstNodePtr& __x) const;

 private:
  SgNode *top;
  SgScopeStatement* scope;
  SgGlobal* global;
  int newVarIndex;
  int delayNewVarInsert;
  std::vector< std::pair<SgScopeStatement*,SgStatement*> > newVarList;
 friend class AstInterface;
};

#endif
