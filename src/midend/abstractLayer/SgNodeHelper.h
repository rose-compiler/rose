#ifndef SgNodeHelper_H
#define SgNodeHelper_H

/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 *************************************************************/

#include <set>
#include <string>

namespace SgNodeHelper {

/*! \brief Functions for simplifying access to SgNode information

  * \author Markus Schordan
  * \date 2012, 2013.

  \details A collection of functions which simplify access to SgNode
  information. All functions are side-effect free and do not store any
  data. All data is returned as pointer to data which already existed
  before calling this function or it is returned by value.


 */

  //! returns the initializer expression of a variable declaration. If no initializer exists it returns 0.
  SgExpression* getInitializerExpressionOfVariableDeclaration(SgVariableDeclaration* decl);

  //! returns the initialized name object of a variable declaration. Otherwise it throws an exception.
  SgInitializedName* getInitializedNameOfVariableDeclaration(SgVariableDeclaration* decl);

  //! returns the declaration statement found for a given variable symbol.
  SgDeclarationStatement* findVariableDeclarationWithVariableSymbol(SgNode* node);

  //! returns filename+line+column information of AST fragment in format "filename:line:column". Used for generating readable output
  std::string sourceFilenameLineColumnToString(SgNode* node);

  //! returns filename information of AST fragment in format "filename". Used for generating readable output
  std::string sourceFilenameToString(SgNode* node);

  //! returns filename information of AST fragment in format "line:col". Used for generating readable output
  std::string sourceLineColumnToString(SgNode* node);

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
  SgStatementPtrList& getForInitList(SgNode* node);
  //! returns the incr/derc-expr of For.
  SgExpression* getForIncExpr(SgNode* node);

  //! determines whether a node is the root node of an AST representing the inc-expr
  //! in a SgForStatement. This function is helpful to deal with this special case
  //! in the ROSE AST where an expression does not have a root node which can be
  //! easily determined to be a root node of an expression (i.e. here it can be *any* binary or unary node
  //! in constrast to all other expressions in the ROSE AST which are either a SgExprStatement or have a SgExpressionRoot node.
  bool isForIncExpr(SgNode* node);

  //! returns the root node representing the AST of the condition of If, While, DoWhile, For, CondOperator (does not handle switch).
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

  /*! This is a lookup function currently not available in ROSE: It
     determines for a given function-call-expression its corresponding
     function-definition if available in the AST. There are three cases:

     case 1: the associated declaration is a defining declaration. In
     this case this directly referenced definition is returned
     (available in ROSE).  

     case 2: the associated declaration is a forward declaration. In
     this case the entire AST is searched for the correct function
     definition (this lookup is currently not available in
     ROSE).

     case 3: no definition is available.  (e.g. this is the case for
     linked stdlib functions). In this case a null-pointer is
     returned. This is determined after case 2 has been
     checked.
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

  //! is true if 'node' is the root node of the AST representing the condition of a Loop construct (While, DoWhile, For).
  bool isLoopCond(SgNode* node);

  //! is true if 'node' is the root node of the AST representing the condition of If, While, DoWhile, For, CondExp. (does not handle switch).
  bool isCond(SgNode* node);

  //! returns true for --Expr and ++Expr, otherwise false.
  bool isPrefixIncDecOp(SgNode* node);

  //! returns true for Expr-- and Expr--, otherwise false;
  bool isPostfixIncDecOp(SgNode* node);
  
  //! returns the SgSymbol* of the variable in a variable declaration
  SgSymbol* getSymbolOfVariableDeclaration(SgVariableDeclaration* decl);

  //! returns the SgSymbol* of the variable in a SgVarRefExp
  SgSymbol* getSymbolOfVariable(SgVarRefExp* varRefExp);

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
  std::set<SgNode*> LoopRelevantBreakStmtNodes(SgNode* node);

  //! returns the first child of an arbitrary AST node (throws exception if numChildren==0)
  SgNode* getFirstChild(SgNode* node);

  //! return a function-call's argument list
  SgExpressionPtrList& getFunctionCallActualParameterList(SgNode* node);

  //! return a function-definition's list of formal paramters
  SgInitializedNamePtrList& getFunctionDefinitionFormalParameterList(SgNode* node);

  //! return a function-definition's return type
  SgType* getFunctionReturnType(SgNode* node);

  //! returns the set of all local variable-declarations of a function
  std::set<SgVariableDeclaration*> localVariableDeclarationsOfFunction(SgFunctionDefinition* funDef);

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
  /*! identifies the list of global variables
     Note: static/external can be resolved by further processing those objects
   */
  std::list<SgVariableDeclaration*> listOfGlobalVars(SgGlobal* global);

  std::list<SgFunctionDefinition*> listOfFunctionDefinitions(SgProject* SgProject);
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

  // determines whether decl declares an array
  bool isArrayDeclaration(SgVariableDeclaration* decl);

  // determines whether decl is an array or a struct
  bool isAggregateDeclaration(SgVariableDeclaration* decl);

  // returns the list of initializers of an array or struct (e.g. for int a[]={1,2,3} it return the list 1,2,3)
  SgExpressionPtrList& getInitializerListOfAggregateDeclaration(SgVariableDeclaration* decl);

  /* replaces expression e1 by expression e2. Currently it uses the SageInterface::rewriteExpression function
     but wraps around some addtional checks that significantly improve performance of the replace operation.
  */
  void replaceExpression(SgExpression* e1, SgExpression* e2, bool mode=false);

  /* replaces the ast with root 'node' with the string 's'. The string is attached to the AST and the unparser uses
     string s instead of unparsing this substree. This function can be used to generate C++ extensions.
  */
  void replaceAstWithString(SgNode* node, std::string s);

  //! Provides functions which match a certain AST pattern and return a pointer to a node of interest inside that pattern.
  namespace Pattern {
    //! tests several patterns and returns pointer to FunctionCallExp inside that matched pattern, otherwise 0.
    SgFunctionCallExp* matchFunctionCall(SgNode *);
    //! tests pattern SgReturnStmt(FunctionCallExp) and returns pointer to FunctionCallExp, otherwise 0.
    SgFunctionCallExp* matchReturnStmtFunctionCallExp(SgNode *);
    //! tests pattern SgExprStatement(FunctionCallExp) and returns pointer to FunctionCallExp, otherwise 0.
    SgFunctionCallExp* matchExprStmtFunctionCallExp(SgNode *);
    //! tests pattern SgExprStatement(SgAssignOp(VarRefExp,FunctionCallExp)) and returns pointer to FunctionCallExp, otherwise 0.
    SgFunctionCallExp* matchExprStmtAssignOpVarRefExpFunctionCallExp(SgNode *);

    //! tests pattern SgFunctionCall(...) where the name of the function is scanf with 2 params
    SgVarRefExp* matchSingleVarScanf(SgNode* node);
    //! tests pattern SgFunctionCall(...) where the name of the function is printf with 2 params
    SgVarRefExp* matchSingleVarPrintf(SgNode* node);
    //! tests pattern SgFunctionCall(...) where the name of the function is fprintf with 3 params
    SgVarRefExp* matchSingleVarFPrintf(SgNode* node);

    struct OutputTarget {
      bool isKnown();
      enum OType { VAR,INT,UNKNOWNPRINTF,UNKNOWNOPERATION};
      OutputTarget():varRef(0),intVal(0),outType(UNKNOWNOPERATION){}
      SgVarRefExp* varRef;
      int intVal;
      OType outType;
    };
    OutputTarget matchSingleVarOrValuePrintf(SgNode* node);

    //! tests pattern for an assert
    bool matchAssertExpr(SgNode* node);

  } // end of namespace Pattern

} // end of namespace SgNodeHelper

#endif
