// Author: Markus Schordan, 2007

#ifndef H_IRCREATION
#define H_IRCREATION

#include <satire_rose.h>
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
  static SgFunctionRefExp* createFunctionRefExp(SgFunctionSymbol *functionSymbol);
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
// static CallStmt* createCallStmt(KFG_NODE_TYPE node_type, std::string name, CallBlock *parent);
  static FunctionCall* createFunctionCall(KFG_NODE_TYPE type, std::string func, CallBlock *parent);
  static FunctionReturn* createFunctionReturn(KFG_NODE_TYPE type, std::string func, CallBlock *parent);
  static FunctionEntry* createFunctionEntry(KFG_NODE_TYPE type, std::string func, CallBlock *parent);
  static FunctionExit* createFunctionExit(KFG_NODE_TYPE type, std::string func, CallBlock *parent);
  static DeclareStmt* createDeclareStmt(SgVariableSymbol *v, SgType *t);
  static UndeclareStmt* createUndeclareStmt(std::vector<SgVariableSymbol *> *v);
  static ExternalCall* createExternalCall(SgExpression *function, std::vector<SgVariableSymbol *> *params, SgType *type_);
  static ExternalReturn* createExternalReturn(SgExpression *function, std::vector<SgVariableSymbol *> *params, SgType *type_);
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
  // create a string with a tree representation of the IR fragment
  static std::string fragmentToTreeRepresentation(SgNode *node);
  static SgNode *deepCopy(SgNode *n, bool copyParentPointer = true);
  // for creating node lists for PAG to iterate over
  template <class T> static void **createNodeList(std::vector<T> &vec);
  static void **createNodeList(SgExprListExp *e);

// GB (2008-04-01): This class is used to collect various things that may be
// dynamically allocated and where we don't know how long they live. Such
// things can be freed when the garbage bin dies or when it is told to clear
// itself.
  class GarbageBin
  {
  public:
      void add_cString(char *str);
      void **findNodeList(void *address);
      void addNodeList(void *address, void **array);

      void clear();
      ~GarbageBin();

  private:
      std::vector<char *> cStrings;
      std::map<void *, void **> nodeLists;
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

// GB (2008-05-13): This template function must be defined in the header.
// This stuff is meant to solve the problem of memory leaks with list
// iterators. Following a suggestion from Florian Martin, store AST lists
// that PAG is supposed to iterate over as NULL-terminated arrays of
// pointers; the iterator can then be a simple pointer. Thus we only need to
// allocate memory for the array, and do that only once if we keep a map
// from the lists in the AST to any already allocated lists. The map is kept
// in the garbage bin and automatically emptied at some point.
// GB (2008-11-12): The identify-objects-by-their-address approach fails for
// stack-allocated vectors, of course. So we skip the lookup for now and
// always allocate a new array.
template <class T>
void **
Ir::createNodeList(std::vector<T> &vec)
{
    void *address = (void *) &vec;
    void **array = NULL; // garbageBin.findNodeList(address);
    if (array == NULL)
    {
     // allocate, fill, and null-terminate the array, put it in the map, and
     // finally return it
        array = new void *[vec.size() + 1];
     // Can't use the STL copy algorithm here because we need to cast to
     // (void *).
        typename std::vector<T>::iterator i;
        void **p = array;
        for (i = vec.begin(); i != vec.end(); ++i)
            *p++ = (void *) *i;
        *p = NULL;
        garbageBin.addNodeList(address, array);
    }
    return array;
}

#endif
