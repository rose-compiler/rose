#ifndef SgNodeHelper_H
#define SgNodeHelper_H

/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 *************************************************************/

#include <set>
#include <list>
#include <vector>
#include <string>

class SgNode;
class SgProject;
class SgLocatedNode;

class SgStatement;

class SgScopeStatement;
class SgGlobal;
class SgFunctionDefinition;

class SgDeclarationStatement;
class SgVariableDeclaration;
class SgFunctionDeclaration;
class SgInitializedName;
class SgClassDeclaration;

class SgExpression;
class SgVarRefExp;
class SgFunctionRefExp;
class SgFunctionCallExp;

class SgSymbol;
class SgVariableSymbol;
class SgFunctionSymbol;

class SgType;
class SgPointerType;
class SgReferenceType;
class SgRvalueReferenceType;
class SgFunctionType;

class SgContinueStmt;
class SgCaseOptionStmt;
class SgDefaultOptionStmt;

class SgPragmaDeclaration;
class SgOmpClauseBodyStatement;

namespace SgNodeHelper {
  
  /// defines if extended normalized call matching (functions+ctors) is enabled
  extern const bool WITH_EXTENDED_NORMALIZED_CALL;

/*! \brief Functions for simplifying access to SgNode information

  * \author Markus Schordan
  * \date 2012, 2013.

  \details A collection of functions which simplify access to SgNode
  information. All functions are side-effect free and do not store any
  data. All data is returned as pointer to data which already existed
  before calling this function or it is returned by value.


 */
  typedef std::pair<int,int> LineColPair;
  
  //! returns the initializer expression of a variable declaration. If no initializer exists it returns 0.
  SgExpression* getInitializerExpressionOfVariableDeclaration(SgVariableDeclaration* decl);

  //! returns the initialized name object of a variable declaration. Otherwise it throws an exception.
  SgInitializedName* getInitializedNameOfVariableDeclaration(SgVariableDeclaration* decl);

  //! returns the declaration statement found for a given variable symbol.
  SgDeclarationStatement* findVariableDeclarationWithVariableSymbol(SgNode* node);

  //! returns the function declaration statement found for a given function symbol.
  SgFunctionDeclaration* findFunctionDeclarationWithFunctionSymbol(SgNode* node);

  //! returns filename+line+column information of AST fragment in format "filename:line:column". Used for generating readable output
  std::string sourceFilenameLineColumnToString(SgNode* node);

  //! returns a std::pair of line and column number. If no file info exists at this node it returns  (-1,-1).
  SgNodeHelper::LineColPair lineColumnPair(SgNode* node);

  //! returns filename as stored in AST node. Used for generating readable output.
  std::string sourceFilenameToString(SgNode* node);

  //! returns filename followed by line:column in one string. Used for generating readable output.
  std::string sourceLineColumnToString(SgNode* node);
  //! returns filename followed by line, separator, and column in one string. Used for generating readable output.
  std::string sourceLineColumnToString(SgNode* node, std::string separator);

  //! returns line, column, and unparsed node in one string.
  std::string lineColumnNodeToString(SgNode* node);

  //! returns filename, line, column, and unparsed node in one string.
  //! Abbreviates unparsed source if too long
  std::string sourceLocationAndNodeToString(SgNode* node);

  //! determines all VarRefExp in the subtree of 'node'. The order in the vector corresponds to the traversal order on the AST.
  std::vector<SgVarRefExp*> determineVariablesInSubtree(SgNode* node);

  /*! computes a list representing the nesting structure of classes (including structs and unions).
    It traverses the AST upwards and collects SgClassDeclaration(s) only. This covers nested classes, nested structs, and nested unions,
    and combinations of those.
  */
  std::list<SgClassDeclaration*> classDeclarationNestingSequence(SgDeclarationStatement*);

  /*! computes for a given node at which scope nesting level this node is in its AST */
  int scopeNestingLevel(SgNode* node);

  /*! computes for a given node at which scope nesting level this node is in its AST */
  int scopeSequenceNumber(SgNode* node);

  /*! computes for a given node the index number of it from the parent.
    e.g. node1(node2,node3,node4) : node4 has index 2 (starting at 0)
    For the root node of an AST (e.g. SgProject) this function returns -1.
   */
  size_t determineChildIndex(SgNode* node);


  //! returns the initializer-list of For.
  std::vector<SgStatement *> & getForInitList(SgNode* node);

  //! returns the incr/derc-expr of For.
  SgExpression * getForIncExpr(SgNode* node);

  //! determines whether a node is the root node of an AST representing the inc-expr
  //! in a SgForStatement. This function is helpful to deal with this special case
  //! in the ROSE AST where an expression does not have a root node which can be
  //! easily determined to be a root node of an expression (i.e. here it can be *any* binary or unary node
  //! in constrast to all other expressions in the ROSE AST which are either a SgExprStatement or have a SgExpressionRoot node.
  bool isForIncExpr(SgNode* node);

  //! returns the root node representing the AST of the condition of If, While, DoWhile, For, CondOperator, switch.
  SgNode* getCond(SgNode* node);

  //! returns the string representing the condition (removes trailing ';')
  std::string unparseCond(SgNode* node);

  //! returns the root node representing the AST of the true branch of If, CondOperator.
  SgNode* getTrueBranch(SgNode* node);

  //! returns the root node representing the AST of the false branch of If, CondOperator.
  SgNode* getFalseBranch(SgNode* node);

  //! returns the root node representing the AST of the loop body of While, DoWhile, For.
  SgNode* getLoopBody(SgNode* node);

  //! returns the first Statement of SgBasicBlock (throws exception if numChildren==0)
  SgNode* getFirstOfBlock(SgNode* node);

  //! returns the last Statement of SgBasicBlock (throws exception if numChildren==0)
  SgNode* getLastOfBlock(SgNode* node);

  //! returns the label name of a SgLabelStatement without trailing ":"
  std::string getLabelName(SgNode* node);

  //! returns function name of SgFunctionDefinition, SgFunctionDeclaration, SgFunctionCall.
  std::string getFunctionName(SgNode* node);

  /*! This function determines for a given function-call-expression
     its corresponding function-definition (by using
     get_definingDeclaration).  This function has constant
     complexity. It does not perform a search, but uses the
     information as present in the AST. If this information is not
     sufficient to determine the definition of a function it returns
     0. For a consistent AST this will find all definitions in the
     same file, but not in a other SgFile.

     For an inter-procedural analysis a more elaborate mechanism is
     required to perform a static function call lresolution (also
     handling function pointers and virtual functions).
  */
  SgFunctionDefinition* determineFunctionDefinition(SgFunctionCallExp* fCall);

  //! Determines whether a provided function declaration is a forward declaration
  bool isForwardFunctionDeclaration(SgNode* declaration);

  /*! this function should only be called for a node in the subtree of
     a SgFunctionDefinition node. For a given 'node' it determines the
     correspondnig functionDefinition node when searching upwards
     in the AST for such a SgFunctionDefinition node. It is useful as a
     simple lookup function from inside the AST subtree of a
     SgFunctionDefinition.  Returns 0 if no SgFunctionDefinition is found (e.g. global scope).
  */
  SgFunctionDefinition* correspondingSgFunctionDefinition(SgNode* node);

  //! checks whether the node 'node' is the root node of the AST by using the get_parent function.
  bool isAstRoot(SgNode* node);

  //! is true if 'node' is the root node of the AST representing a Loop construct (While, DoWhile, For).
  bool isLoopStmt(SgNode* node);

  //! is true if 'node' is the root node of the AST representing the condition of a Loop construct (While, DoWhile, For).
  bool isLoopCond(SgNode* node);

  //! is true if 'node' is the root node of the AST representing If, While, DoWhile, For, switch, CondExp.
  bool isCondStmtOrExpr(SgNode* node);

  //! is true if 'node' is the root node of the AST representing If, While, DoWhile, For, switch.
  bool isCondStmt(SgNode* node);

  //! is true if 'node' is the root node of the AST representing the condition of If, While, DoWhile, For, switch.
  bool isCondInBranchStmt(SgNode* node);

  //! is true if 'node' is the root node of the AST representing the condition of If, While, DoWhile, For, switch, CondExp.
  bool isCond(SgNode* node);

  //! sets 'cond' as the root node of the AST representing the condition in statements if, while, dowhile, for, switch.
  void setCond(SgStatement* stmt, SgNode* cond);


  //! returns true for --Expr and ++Expr, otherwise false.
  bool isPrefixIncDecOp(SgNode* node);

  //! returns true for Expr-- and Expr--, otherwise false;
  bool isPostfixIncDecOp(SgNode* node);

  //! returns the SgSymbol* of the variable in a variable declaration
  SgSymbol* getSymbolOfVariableDeclaration(SgVariableDeclaration* decl);

  //! returns the SgSymbol* of the variable in a function declaration
  SgFunctionSymbol* getSymbolOfFunctionDeclaration(SgFunctionDeclaration* decl);

  //! returns the SgSymbol* of the variable in a SgVarRefExp
  SgSymbol* getSymbolOfVariable(SgVarRefExp* varRefExp);

  //! returns the SgSymbol* of the function in a SgFunctionRefExp
  SgFunctionSymbol* getSymbolOfFunction(SgFunctionRefExp* funcRefExp);

  //! returns the SgSymbol* of a SgInitializedName
  SgSymbol* getSymbolOfInitializedName(SgInitializedName* initName);

  //! returns name of symbol as string
  std::string symbolToString(SgSymbol* symbol);

  /*! \brief Creates a long unique variable name for a given node of type SgVariableDeclaration or SgVarRefExp

     If node is not one of those two types an exception is thrown
     The long variable name consists $functionName$scopeLevel$varName
     In case of global scope functionName is empty, giving a string: $$scopeLevel$varName
     Note: this function only considers C-functions. Classes are recognized.
  */
  std::string uniqueLongVariableName(SgNode* node);

  /*! \brief returns a set of SgNode where each node is a break node, but
     properly excludes all nested loops.

     @param [in] node can point directly to the AST construct (e.g. SgIfStatement) or a basic block of the respective loop construct.

     The only property this function maintains during traversal of the
     AST is that it does not collect break nodes from nested loops but
     only from the current scope. Only those breaks are loop-relevant,
     meaning only those can force an exit of the loop. Break
     statements inside if statements or Conditional Expressions inside
     the loop are handled properly. Excluded are only constructs where
     a break statement refers to that nested loop but not the current
     (=relevant) loop. This function can be used for:
     SgWhile,SgDoWhile,SgForStatement, SgSwitch.
  */
  std::set<SgNode*> loopRelevantBreakStmtNodes(SgNode* node);
  std::set<SgContinueStmt*> loopRelevantContinueStmtNodes(SgNode* node);

  //! collects all case labels from the switch it started in (excludes nested switch stmts)
  std::set<SgCaseOptionStmt*> switchRelevantCaseStmtNodes(SgNode* node);

  /*! returns the default stmt if it exists. Otherwise return 0 and can
     be used to test whether a default stmt exists in a given switch
     stmt. */
  SgDefaultOptionStmt* switchRelevantDefaultStmtNode(SgNode* node);

  //! returns the first child of an arbitrary AST node (throws exception if numChildren==0)
  SgNode* getFirstChild(SgNode* node);

  //! return a function-call's argument list
  std::vector<SgExpression *> & getFunctionCallActualParameterList(SgNode* node);

  // schroder3 (2016-07-27): Returns the callee of the given call expression
  SgExpression* getCalleeOfCall(/*const*/ SgFunctionCallExp* call);

  // schroder3 (2016-06-24): Returns the function type of the callee of the given call expression
  SgFunctionType* getCalleeFunctionType(/*const*/SgFunctionCallExp* call);

  //! return a function-definition's list of formal paramters
  std::vector<SgInitializedName *> & getFunctionDefinitionFormalParameterList(SgNode* node);

  //! return a function-definition's return type
  SgType* getFunctionReturnType(SgNode* node);

  //! returns the set of all local variable-declarations of a function
  std::set<SgVariableDeclaration*> localVariableDeclarationsOfFunction(SgFunctionDefinition* funDef);

  //! schroder3 (2016-07-22): Returns the closest function definition that contains the given node
  SgFunctionDefinition* getClosestParentFunctionDefinitionOfLocatedNode(SgLocatedNode* locatedNode);

  //! returns the child of SgExprStatement (which is guaranteed to be unique and to exist)
  SgNode* getExprStmtChild(SgNode* node);

  //! returns the child of SgExpressionRoot (which is guaranteed to be unique and to exist)
  SgNode* getExprRootChild(SgNode* node);

  //! returns the child of a SgUnaryExp (which is guaranteed to be unique and to exist)
  SgNode* getUnaryOpChild(SgNode* node);

  /*! returns the number of children as int (intentionally not as t_size)
     ensures that the number of children fits into an int, otherwise throws exception.
  */
  int numChildren(SgNode* node);

  /*! computes a new string from s1 where each doublequote is replaced with a backslash followed by the doublequote. This is helpful when printing
   * unparsed program fragments which contain doublequoted strings to a dot file (used by nodeToString).
   * This function also replaces <,<=,>=,> with the corresponding HTML codes.
   */
  std::string doubleQuotedEscapedString(std::string s1);

  /*!
    Same as doubleQuotedEscapedString but also replaces <,<=,>=,> with the corresponding HTML codes.
    This is required when printing program code inside HTML tables of a dot file.
  */
  std::string doubleQuotedEscapedHTMLString(std::string s1);

  //! checks whether a SgVariableSymbol is representing a variable in
  //a forward declaration. This case no declaration for the variable
  //exists. This is currently not possible in the ROSE AST.
  bool isVariableSymbolInFunctionForwardDeclaration(SgNode* node);

  //! checks whether a SgVariableSymbol is representing a function parameter (this does not apply for forward declarations)
  SgVariableSymbol* isFunctionParameterVariableSymbol(SgNode* node);

  //! returns a string representing the node (excluding the subtree)
  std::string nodeToString(SgNode* node);

  //! return lhs of a binary node (if it is not a binary node it throws an exception)
  SgNode* getLhs(SgNode* node);

  //! return rhs of a binary node (if it is not a binary node it throws an exception)
  SgNode* getRhs(SgNode* node);

  /*! returns the parent of a node. Essentially a wrapper function of the ROSE get_parent() function, but throws
     an exception if no parent exists. For SgProject node  no exception is thrown if no parent exists because it is the root node of a ROSE AST.
  */
  SgNode* getParent(SgNode* node);

  /*! searches in the provided Project for SgGlobal nodes */
  std::list<SgGlobal*> listOfSgGlobal(SgProject* project);

  /*! identifies the list of global variables
     Note: static/external can be resolved by further processing those objects
   */
  std::list<SgVariableDeclaration*> listOfGlobalVars(SgProject* project);
#if __cplusplus > 199711L
  std::list<SgVariableDeclaration*> listOfGlobalFields(SgProject* project);
#endif
  /*! identifies the list of global variables
     Note: static/external can be resolved by further processing those objects
   */
  std::list<SgVariableDeclaration*> listOfGlobalVars(SgGlobal* global);
#if __cplusplus > 199711L
  std::list<SgVariableDeclaration*> listOfGlobalFields(SgGlobal* global);
#endif

  std::list<SgFunctionDefinition*> listOfFunctionDefinitions(SgNode* node);
#if __cplusplus > 199711L
  std::list<SgFunctionDeclaration*> listOfFunctionDeclarations(SgNode* node);
#endif
  std::list<SgVarRefExp*> listOfUsedVarsInFunctions(SgProject* SgProject);

  /*! identifies the list of SgFunctionDefinitions in global scope
     Functions/methods of classes are NOT included in this list.
     Note: static/external can be resolved by further processing those objects
  */
  std::list<SgFunctionDefinition*> listOfGlobalFunctionDefinitions(SgGlobal* global);

  /*!
    checks whether the expression 'node' represents an assignment to an array's element
    considers all assignment operators and arrays of any size
  */
  bool isArrayElementAssignment(SgNode* node);
  bool isFloatingPointAssignment(SgNode* exp);
  bool isArrayAccess(SgNode* node);
  bool isPointerVariable(SgVarRefExp* var);

  // checks for float, double, long double
  bool isFloatingPointType(SgType* type);

  // schroder3 (2016-07-22): Modified version of SageInterface::isPointerType(...) that returns the
  //  underlying pointer type.
  const SgPointerType* isPointerType(const SgType* t);

  // schroder3 (2016-08-17): Added support for rvalue reference types. See isLvalueReferenceType(...)
  //  if only lvalue references are desired.
  // schroder3 (2016-07-22): Modified version of SageInterface::isReferenceType(...) that
  //  returns the underlying reference type.
  //
  // Returns the underlying (without typedefs and mofifiers) rvalue OR lvalue reference type if the
  //  given type is such a reference type. Returns 0 otherwise.
  const SgType* isReferenceType(const SgType* t);

  // schroder3 (2016-08-22): Modified version of SageInterface::isReferenceType(...) that
  //  returns the underlying LVALUE reference type.
  const SgReferenceType* isLvalueReferenceType(const SgType* t);

  // schroder3 (2016-08-22): Returns the underlying RVALUE reference type if the given type is a
  //  rvalue reference type. Returns 0 otherwise.
  const SgRvalueReferenceType* isRvalueReferenceType(const SgType* t);

  // schroder3 (2016-08-22): Wrapper around SgReferenceType::get_base_type(...) and
  //  SgRvalueReferenceType::get_base_type(...) that works for both reference types.
  //  (This is a workaround for the missing mutual base class of SgReferenceType and
  //  SgRvalueReferenceType.)
  SgType* getReferenceBaseType(const SgType* t);

  // schroder3 (2016-07-26): Returns the given type as a SgPointerType if it is a
  //  function pointer type. Returns 0 otherwise.
  const SgPointerType* isFunctionPointerType(const SgType* type);

  // schroder3 (2016-07-26): Returns the (underlying) function type of the given type if the given
  //  type is eligible for function-to-pointer conversion. Returns 0 otherwise.
  const SgFunctionType* isTypeEligibleForFunctionToPointerConversion(const SgType* type);

  // schroder3 (2016-07-27): Returns the underlying function type of the given expression if it
  //  is callable. Returns 0 otherwise.
  SgFunctionType* isCallableExpression(/*const*/ SgExpression* expr);

  // schroder3 (2016-07-27): Returns the underlying function type if the given type
  //  is callable i.e. a expression of this type could be called. Returns 0 otherwise.
  SgFunctionType* isCallableType(/*const*/ SgType* type);

  // determines whether decl declares an array
  bool isArrayDeclaration(SgVariableDeclaration* decl);

  // determines whether decl is an array or a struct
  bool isAggregateDeclaration(SgVariableDeclaration* decl);

  // returns the list of initializers of an array or struct (e.g. for int a[]={1,2,3} it return the list 1,2,3)
  std::vector<SgExpression *> & getInitializerListOfAggregateDeclaration(SgVariableDeclaration* decl);

  /*! replaces expression e1 by expression e2. Currently it uses the
     SageInterface::rewriteExpression function but wraps around some
     addtional checks that significantly improve performance of the
     replace operation.
  */
  void replaceExpression(SgExpression* e1, SgExpression* e2, bool mode=false);

  /*! replaces the ast with root 'node' with the string 's'. The
     string is attached to the AST and the unparser uses string s
     instead of unparsing this substree. This function can be used to
     generate C++ extensions.
  */
  void replaceAstWithString(SgNode* node, std::string s);

  /*! collects all pragmas with name 'pragmaName' and creates a list
     of all pragma strings (with stripped off prefix) and the
     associated SgNode. */
  typedef std::list<std::pair<std::string,SgNode*> > PragmaList;
  PragmaList collectPragmaLines(std::string pragmaName,SgNode* root);

  /*! return the verbatim pragma string as it is found in the source
     code this string includes the leading "#pragma".
  */
  std::string getPragmaDeclarationString(SgPragmaDeclaration* pragmaDecl);

  //! replace in string 'str' each string 'from' with string 'to'.
  void replaceString(std::string& str, const std::string& from, const std::string& to);

  //! checks whether prefix 'prefix' is a prefix in string 's'.
  bool isPrefix(const std::string& prefix, const std::string& s);

  //! checks whether 'elem' is the last child (in traversal order) of node 'parent'.
  bool isLastChildOf(SgNode* elem, SgNode* parent);

  /*! Returns for a given class/struct/union a list with the variable declarations of the member variables.
    Note this is a filtered list returned by the SgType::returnDataMemberPointers function which also returns
    pointers to methods
   */
  std::list<SgVariableDeclaration*> memberVariableDeclarationsList(SgClassType* sgType);
  
#if __cplusplus > 199711L
  //! Checks if an OpenMP construct is marked with a nowait clause
  bool hasOmpNoWait(SgOmpClauseBodyStatement *ompNode);

  typedef std::vector<SgOmpSectionStatement *> OmpSectionList;
  OmpSectionList getOmpSectionList(SgOmpSectionsStatement *sectionsStmt);
#endif

  //! Provides functions which match a certain AST pattern and return a pointer to a node of interest inside that pattern.
  namespace Pattern {
    //! tests several patterns and returns pointer to FunctionCallExp inside that matched pattern, otherwise 0.
    SgFunctionCallExp* matchFunctionCall(SgNode*);
    //! tests pattern SgReturnStmt(FunctionCallExp) and returns pointer to FunctionCallExp, otherwise 0.
    SgFunctionCallExp* matchReturnStmtFunctionCallExp(SgNode*);

    //! tests pattern SgExprStatement(FunctionCallExp) and returns pointer to FunctionCallExp, otherwise 0.
    SgFunctionCallExp* matchExprStmtFunctionCallExp(SgNode*);

    //! tests pattern SgExprStatement(SgAssignOp(VarRefExp,FunctionCallExp)) and returns pointer to FunctionCallExp otherwise 0.
    SgFunctionCallExp* matchExprStmtAssignOpVarRefExpFunctionCallExp(SgNode*);

    //! tests pattern for function call in variable declaration and returns pointer to FunctionCallExp otherwise 0.
    SgFunctionCallExp* matchFunctionCallExpInVariableDeclaration(SgNode* node);

    //! checks variable declaration with function call, returns variable declaration. Otherwise 0. e.g. int x=f();
    SgVariableDeclaration* matchVariableDeclarationWithFunctionCall(SgNode* node);
    //! checks variable declaration with function call, returns both in a pair, or a with (0,0).
    std::pair<SgVariableDeclaration*,SgFunctionCallExp*> matchVariableDeclarationWithFunctionCall2(SgNode* node);

    std::pair<SgVarRefExp*,SgFunctionCallExp*> matchExprStmtAssignOpVarRefExpFunctionCallExp2(SgNode* node);

    //! tests pattern for an assert
    bool matchAssertExpr(SgNode* node);

    //! tests pattern SgFunctionCall(...) where the name of the function is scanf with 2 params
    SgVarRefExp* matchSingleVarScanf(SgNode* node);
    //! tests pattern SgFunctionCall(...) where the name of the function is printf with 2 params
    SgVarRefExp* matchSingleVarPrintf(SgNode* node);
    //! tests pattern SgFunctionCall(...) where the name of the function is fprintf with 3 params
    SgVarRefExp* matchSingleVarFPrintf(SgNode* node,bool showWarnings=false);

    struct OutputTarget {
      bool isKnown();
      enum OType { VAR,INT,UNKNOWNPRINTF,UNKNOWNOPERATION};
      OutputTarget():varRef(0),intVal(0),outType(UNKNOWNOPERATION){}
      SgVarRefExp* varRef;
      int intVal;
      OType outType;
    };
    OutputTarget matchSingleVarOrValuePrintf(SgNode* node);


  } // end of namespace Pattern

#if __cplusplus > 199711L
  // Can a given node be changed? (aka transformed)
  bool nodeCanBeChanged(SgLocatedNode * lnode);
#endif

  struct ExtendedCallInfo
  {
      ExtendedCallInfo()
      : rep(NULL)
      {}   
    
      ExtendedCallInfo(SgLocatedNode& callnode)
      : rep(&callnode)
      {}   
    
      SgLocatedNode*            representativeNode() const;
      SgFunctionCallExp*        callExpression()     const;              
      SgConstructorInitializer* ctorInitializer()    const;              
      SgPointerDerefExp*        functionPointer()    const; 
      
      operator bool() const { return rep != NULL; }
      
    private:
      SgLocatedNode* rep;
  };
  
  ExtendedCallInfo
  matchExtendedNormalizedCall(SgNode*);
} // end of namespace SgNodeHelper

#endif
