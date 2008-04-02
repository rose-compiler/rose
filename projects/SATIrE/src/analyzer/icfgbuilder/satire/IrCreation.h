// Author: Markus Schordan, 2007

#ifndef H_IRCREATION
#define H_IRCREATION

#include <rose.h>
#include <string>
#include "cfg_support.h"

class Ir {
public:
  // ROSE nodes used in ICFG
  static SgPointerType* createPointerType(SgType* type);
  static SgExprStatement* createExprStatement(SgExpression* e);
  static SgAssignOp* createAssignOp(SgExpression* lhs, SgExpression* rhs);
  static SgArrowExp* createArrowExp(SgExpression* lhs, SgExpression* rhs);
  static SgBoolValExp* createBoolValExp(bool val);
  static SgAddressOfOp* createAddressOfOp(SgExpression* e, SgType* type);
  static SgVarRefExp* createVarRefExp(std::string name, SgType* type);
  static SgVarRefExp* createVarRefExp(SgVariableSymbol* variableSymbol);
  static SgVarRefExp* createVarRefExp(SgInitializedName* initializedName);
  static SgVariableSymbol* createVariableSymbol(std::string name,SgType* type);
  static SgVariableSymbol* createVariableSymbol(SgInitializedName* initName);
  static SgName createName(std::string name);
  static SgInitializedName* createInitializedName(std::string name,SgType* type);
  static SgNullStatement* createNullStatement();
  static SgExprListExp* createExprListExp();
  static SgConstructorInitializer* createConstructorInitializer(SgMemberFunctionDeclaration* mfd,SgType* type);
  static SgMemberFunctionDeclaration* createMemberFunctionDeclaration(std::string name);
  static SgIfStmt* createIfStmt(SgExprStatement* expStmt);
  static SgWhileStmt* createWhileStmt(SgExprStatement* expStmt);
  static SgDoWhileStmt* createDoWhileStmt(SgExprStatement* expStmt);
  static SgSwitchStatement* createSwitchStatement(SgExprStatement* expStmt);
  static SgClassType* createClassType();
  static SgThisExp* createThisExp(SgClassSymbol *class_symbol);
  static SgIntVal* createIntVal(int value);
  static SgCastExp* createNullPointerExp(SgType *type);
  // ICFG specific nodes that inherit from ROSE SgStatement and implement unparseToString
  static CallStmt* createCallStmt(KFG_NODE_TYPE node_type, std::string name, CallBlock *parent);
  static FunctionEntry* createFunctionEntry(KFG_NODE_TYPE type, std::string func, CallBlock *parent);
  static FunctionExit* createFunctionExit(KFG_NODE_TYPE type, std::string func, CallBlock *parent);
  static DeclareStmt* createDeclareStmt(SgVariableSymbol *v, SgType *t);
  static UndeclareStmt* createUndeclareStmt(std::vector<SgVariableSymbol *> *v);
  static ExternalCall* createExternalCall(SgExpression *function, std::vector<SgVariableSymbol *> *params, SgType *type_);
  static ConstructorCall* createConstructorCall(std::string name_, SgType *type_);
  static DestructorCall* createDestructorCall(std::string name_, SgType *type_);
  static ArgumentAssignment* createArgumentAssignment(SgVariableSymbol *l, SgExpression *r);
  static ArgumentAssignment* createArgumentAssignment(SgExpression *l, SgExpression *r);
  static ReturnAssignment* createReturnAssignment(SgVariableSymbol *l, SgVariableSymbol *r);
  static ParamAssignment* createParamAssignment(SgVariableSymbol *l, SgVariableSymbol *r);
  static LogicalIf* createLogicalIf(SgExpression *e);
  static IfJoin* createIfJoin();
  static WhileJoin* createWhileJoin();
  // support functions
  static const char* getConstCharPtr(SgName& n);
  static char* getCharPtr(SgName& n);
  static char* getCharPtr(const std::string &s);
  static std::string getString(SgName& n);
  static std::string getStrippedName(SgInitializedName* in);
  // for creating a string representation of IR fragments
  static std::string fragmentToString(const SgNode* node);
  static SgNode *deepCopy(SgNode *n, bool copyParentPointer = true);

// GB (2008-04-01): This class is used to collect various things that may be
// dynamically allocated and where we don't know how long they live. Such
// things can be freed when the garbage bin dies or when it is told to clear
// itself.
  class GarbageBin
  {
  public:
      void add_cString(char *str);
      void clear();
      ~GarbageBin();

  private:
      std::vector<char *> cStrings;
  };

// This garbage bin lives over the whole time the program is executed. It is
// emptied at program termination (at the latest).
  static GarbageBin garbageBin;

private:
  static void configLocatedNode(SgLocatedNode* ln, SgNode* s1, SgNode* s2);
  static void configLocatedNode(SgLocatedNode* ln, SgNode* s1);
  static void configLocatedNode(SgLocatedNode* ln);
  static void configInitializedName(SgInitializedName* n);
  static void configSupportNode(SgSupport* ln, SgNode* s1);
  static void configSupportNode(SgSupport* ln);
  static void configTypeNode(SgType* type);
  static void configSymbolNode(SgSymbol* ln, SgNode* s1);
  static void configSymbolNode(SgSymbol* ln);
  static Sg_File_Info* createFileInfo();
};

#endif
