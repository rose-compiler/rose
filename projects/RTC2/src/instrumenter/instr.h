
#ifndef INSTR_H
#define INSTR_H 1

#include <list>
#include "rose.h"

#include "util.h"
#include "strDecl.h"
#include "funcDecl.h"
#include "traverse.h"

//#define RMM_TYPETRACKER
#define LOCK_KEY_INSERT
//#define INSTR_DEBUG


namespace Instr
{
  extern VariableSymbolMap_t classStructVarRemap;
  extern ScopeLockKeyMap     scopeLockMap;
  extern VariableSymbolMap_t varRemap;
  extern VariableSymbolMap_t reverseMap;
  extern int                 nodePosition;
  extern bool                STAGGERED;

  struct TempNameAttr : AstAttribute
  {
    private:
      SgStatement* stored;
      SgStatement* current;

    public:
      static
      std::string attributeName() { return "TempNameAttr"; }

      TempNameAttr(SgStatement* node, SgStatement* replacement)
      : stored(node), current(replacement)
      {}

      void swap()
      {
        namespace si = SageInterface;

        stored->set_parent(current->get_parent());

        SgStatement* temp = si::deepCopy(current);

        si::replaceStatement(stored, current);

        current = stored;
        stored = temp;
      }

      //
      // accessor functions

      void setCurrent(SgStatement* newC) { current = newC; }
      SgStatement* getStored() const     { return stored; }

      //
      // overridden functions from base class

      TempNameAttr* copy() const ROSE_OVERRIDE
      {
        return new TempNameAttr(stored, current);
      }

      std::string attribute_class_name() const ROSE_OVERRIDE
      {
        return attributeName();
      }

      OwnershipPolicy getOwnershipPolicy() const ROSE_OVERRIDE
      {
        return CONTAINER_OWNERSHIP;
      }
  };

  //
  // utilitity functions

  SgExpression* buildMultArgOverloadFn(SgName fn_name, SgExprListExp* parameter_list, SgType* retType, SgScopeStatement* scope, SgNode* pos, bool derived=false);
  SgExpression* createStructUsingAddressOf(SgExpression* exp, SgNode* pos);

  void instrument(SgProject* proj, int pos);
  void replaceWrapper(SgExpression* original, SgExpression* replacement);
  void replaceWrapper(SgStatement* original, SgStatement* replacement);
  void buildWrapper(std::string uninstrumented, SgLocatedNode* instrumented);

  void instr(SgPntrArrRefExp* array_ref);
  void instr(SgDotExp* dot_exp);
  void instr(SgArrowExp* arrow_exp);
  void instr(SgCommaOpExp* cop);
  void instrCond(SgBinaryOp* bop);
  void instrAddSub(SgBinaryOp* bop);
  void instr(SgAssignOp* aop);
  void instr(SgBinaryOp* bop);

  void instr(SgProject*, SgFunctionCallExp*);
  void instr(SgProject*, SgCallExpression*);

  void instr(SgVarRefExp* ref);
  void instr(SgPointerDerefExp* ptr_deref);
  void instr(SgCastExp* cast_exp);
  void instr(SgAddressOfOp* aop);
  void instrOvlUnary(SgUnaryOp* uop);
  void instr(SgUnaryOp* uop);

  void instr(SgThisExp* te);
  void instr(SgNewExp* ne);
  void instr(SgDeleteExp* del);
  void instr(SgSizeOfOp* sop);
  void instr(SgRefExp* ref);

  void instr(SgBasicBlock* bb);
  void instr(SgGlobal* global);
  void instr(SgScopeStatement* scope);

  void instr(SgVariableDeclaration* varDecl);
  void instr(SgFunctionDeclaration* fn_decl);
  void instr(SgFunctionParameterList* param_list);

  void instr(SgDeclarationStatement* decl);
  void instr(SgReturnStmt* retstmt);

  void instr(SgExpression* exp);
  void instr(SgStatement* stmt);
  void instr(SgProject*, SgNode* node);

  SgExpression* handleAssignOverloadOps(SgNode* node);
  SgExpression* handleCSAssignOverloadOps(SgExpression* assign_op);
  SgExpression* handleCastExp(SgCastExp* cast_exp);
  SgExpression* buildCreateEntryFnForArrays(SgExpression* array, SgScopeStatement* scope, SgStatement* pos);
  void insertCreateEntry(SgExpression* array, SgScopeStatement* scope, SgStatement* pos);
  bool existsInSLKM(SgScopeStatement* scope);
  LockKeyPair findInSLKM(SgScopeStatement* scope);
  SgType* getLockType();
  SgType* getKeyType();
  void handleSymbolTable(SgInitializedName* orig_name, SgInitializedName* str_name, bool addToMap);
  SgInitializedName* createStructVariableFor(SgInitializedName* name);
  void handleSymbolTable(SgVariableDeclaration* orig_decl, SgVariableDeclaration* str_decl, bool addToMap);

//RMM
  SgExpression* createArrayBoundCheckFn(SgExpression* arg1, SgExpression* arg2,
                  SgExpression* arg3, SgType* arg4, SgScopeStatement* scope,
                  SgStatement* insert,
                  bool array = false);

}

#endif /* INSTR_H */
