#ifndef NORMALIZATION_H
#define NORMALIZATION_H

// Author: Markus Schordan, 2018

#include <list>
#include <utility>

class SgNode;
class SgWhileStmt;
class SgDoWhileStmt;
class SgForStatement;
class SgSwitchStatement;

#include "NormalizationOp.h"
#include "NormalizationInliner.h"

namespace CodeThorn {
  class Normalization {
    // Not supported yet: 
    // 1. condition hoisting in do-while (into the block, not before the loop)
    // 2. transformation of continue in while, do-while

  public:
    Normalization();
    ~Normalization();
    struct Options {
      // level=0: no normalization
      // level=1: only normalize expressions containing a function call
      // level=2: normalize all expressions
      // (level=3: normalize all expressions and lower loops
      //          (normalized programs has only if-goto constructs as control
      //          statements) - not fully supported yet)
      void configureLevel(unsigned int level);

      // allows to turn off all normalizations (at once)
      bool normalization=true;

      // only normalize expressions with function calls
      bool restrictToFunCallExpressions=true;

      // transforms single statements in if/while/do-while into blocks with one statement
      // transformation: if(Cond) S; => if(Cond) { S }
      bool normalizeSingleStatements=true;

      // transforms all labels into separate statements
      // each label becomes a separate statement (attached to an empty statement)
      // Lab: S; => Lab:; S; 
      bool normalizeLabels=true;

      // replace for-stmt with while-stmt
      // for(Init;Cond;Inc) S; => { Init; while(Cond) { S; Inc; } } 
      bool eliminateForStatements=true;

      // replace while with if/goto
      bool eliminateWhileStatements=false; // currently not used

      // eliminate operators '||', '&&'
      // logical binary operators are transformed into nested if-statements with temporary variable for boolean result
      bool eliminateShortCircuitOperators=false;

      // eliminate operator '?'
      // transformed into if-statement with result variable
      const bool eliminateConditionalExpressionOp=false;

      // if(Cond) S; => { T t=Cond; if(t) S } where T has type of Cond.
      // do {...} while (Cond); => do {...; T t=Cond; if(t) break; } where T has type of Cond.
      // alternative: while(C) {...} ==> T t=C; while(t) { ...; t=C; } (not implemented because it duplicates condition)
      // while(Cond) {...} => while(1) { T t=Cond; if(t) break; ...} where T has type of Cond.
      // alternative: do {...} while (C) ==> do {...; T t=C; } while(t); (not implemented because it violates C/C++ scoping rules)
      bool hoistConditionExpressions=true;

      // normalize expressions such that for every interemdiate result
      // a temporary variable is declared and its initializer is the
      // expression (operating on temporaries representing the results
      // of subexpressions).
      bool normalizeExpressions=true;

      bool normalizeVariableDeclarations=false;
      bool normalizeVariableDeclarationsWithFunctionCalls=true;

      // puts the sequence of normalized expressions in a block. This
      // way the scope of temporary variables ends right after the
      // last initialization (or assignment) of the normalized
      // expression.
      // requires: normalizeVariableDeclarations==true
      // E => { normalized(E) }
      bool encapsulateNormalizedExpressionsInBlocks=true;

      // transforms break in switch to gotos. This can cause unparsed
      // code to not compile because of special C++ rules of
      // by-passing initializaitons with gotos. It is not a problem
      // for the analysis, but for unparsing. Therefore this is off by
      // default.
      bool transformBreakToGotoInSwitchStmt=false;

      // lowering. Applies to {for, do, do-while}
      bool transformBreakToGotoInLoopStmts=true;

      // This transformation is applied to
      // while and do-while, not to for-loops.
      const bool transformContinueToGotoInWhileStmts=false; // not supported yet

      // using SgInterface inlining (to be replaced)
      bool inlining=false;
    } options;

    // type for tmp var counter
    typedef uint32_t TmpVarNrType;

    // applies normalization on entire AST with normalization level 0-3.
    // level 0: no normalization
    // level 1: all expressions with a function call (and all implied normalizations)
    // level 2: all expressions (and all implied normalizations)
    // level 3: all expresisons and lowering of all control constructs (only if+gotos remain)
    void normalizeAst(SgNode* root, unsigned int normalizationLevel);

    static void setLabelPrefix(std::string prefix);
    // increments internal counter and creates new name
    static std::string newLabelName();
    // creates new label using label counter and create a new label-stmt
    static SgLabelStatement* createLabel(SgStatement* target);
    // uses current label counter and returns existing name
    static std::string labelName();

    // default is '__xtmp'
    static void setTmpVarPrefix(std::string);
    static std::string newTmpVarName();

    static bool hasFunctionCall(SgExpression* expr);
    static bool isWithinBlockStmt(SgExpression* exp);

    // enable/disable inling. By default off.
    void setInliningOption(bool flag);
    bool getInliningOption();
    // calls ROSE SageInterface function for inlining
    CodeThorn::InlinerBase* getInliner();
    // allows to use custom inliner. Default is the inliner from the SageInterface.
    void setInliner(CodeThorn::InlinerBase*);

  protected:
    // assumes correctly configured options (invoked by normalizeAst(root,level))
    void normalizeAst(SgNode* root);

    /* normalize all label stmts in AST. Every label is attached to an
     * empty statement (instead to S). Inserting a statement before S
     * is then straight-forward.
     * Transformation:  L: S; => L; S;
     */

  public:
    static void initDiagnostics();
    static Sawyer::Message::Facility logger;

    static void normalizeLabelStmts(SgNode* root);
    // the associated stmt node (= the label node's child) remains unchanged, except for the update of its parent pointer.
    static void normalizeLabel(SgLabelStatement* label);

    // level 3 normalization
    static void createGotoStmtAtEndOfBlock(SgLabelStatement* newLabel, SgBasicBlock* block, SgStatement* target);
    static SgGotoStatement* createGotoStmtAndInsertLabel(SgLabelStatement* newLabel, SgStatement* target);
    // transforms Label1: Label2: LabelN: Stmt; ==> Label1:; Label2:; LabelN:; Stmt;
    // requires: normalizeSingleStatementsToBlocks()

  private:
    /* normalize all Expressions in AST. The original variables remain
     * in the program and are assign the last value of the sequence of
     * operations of an expression. */
    void normalizeExpressionsInAst(SgNode* node, bool onlyNormalizeFunctionCallExpressions=false);

    // moves conditions out of if and switch constructs
    void hoistConditionsInAst(SgNode* node, bool onlyNormalizeFunctionCallExpressions=false);
    // moves conditions out of if and switch constructs. Declares new
    // variable for intermediate result
    void hoistCondition(SgStatement* node);

    // converts for-stmts to while-stmts (uses SageInterface function)
    void convertAllForStmtsToWhileStmts(SgNode* top);

    // normalizes all single statements in if-statements to blocks (sage version)
    void normalizeSingleStatementsToBlocks(SgNode* node);

    void createLoweringSequence(SgNode* root);
    // apply lowering sequence generated by createLoweringSequence
    void applyLoweringSequence();

    enum SubExprTransformationEnum { GEN_UNDEFINED, GEN_TMP_VAR_INIT, GEN_FALSE_BOOL_VAR_DECL, GEN_BOOL_VAR_IF_ELSE_STMT, GEN_BOOL_VAR_IF_STMT, GEN_IF_ELSE_STMT, GEN_LOG_OP_REPLACEMENT, GEN_CONDOP_IF_ELSE_STMT, GEN_VOID_EXPR, GEN_TMP_VAR_ASSIGN, GEN_TMP_VAR_ASSIGN_WITH_EXPR, GEN_TMP_VAR_DECL,GEN_STMT_REMOVAL };
    struct RegisteredSubExprTransformation {
      SubExprTransformationEnum transformation;
      RegisteredSubExprTransformation(SubExprTransformationEnum, SgStatement* s, SgExpression* e);
      RegisteredSubExprTransformation(SubExprTransformationEnum, Normalization::TmpVarNrType tmpVarNrParam, SgStatement* s, SgExpression* e);
      RegisteredSubExprTransformation(SubExprTransformationEnum, Normalization::TmpVarNrType tmpVarNrParam, SgStatement* s, SgExpression* e, Normalization::TmpVarNrType declVarNr);
      RegisteredSubExprTransformation(SubExprTransformationEnum, SgStatement* s, SgExpression* e, Normalization::TmpVarNrType tmpVarNrParam);
      RegisteredSubExprTransformation(SubExprTransformationEnum, SgStatement* s, SgExpression* e, Normalization::TmpVarNrType tmpVarNrParam, SgStatement* trueBody, SgStatement* falseBody);
      SgStatement* stmt=0;
      SgExpression* expr=0;
      SgVariableDeclaration* decl=0;
      SgStatement* trueBody=0;
      SgStatement* falseBody=0;
      SgVariableDeclaration* tmpVarDeclaration=0;
      SgType* tmpVarDeclType=0;
      SgExpression* tmpVarReference=0;
      TmpVarNrType tmpVarNr=0;
      TmpVarNrType lhsTmpVarNr=0; // cond op uses all 3
      TmpVarNrType rhsTmpVarNr=0;
      TmpVarNrType unaryTmpVarNr=0;
      TmpVarNrType declVarNr=0;
      TmpVarNrType condVarNr=0;
    };
    typedef std::list<RegisteredSubExprTransformation> SubExprTransformationList;
    typedef std::list<SubExprTransformationList> ExprTransformationList;
    ExprTransformationList exprTransformationList;

    // transform expression with root ExprStatement into a list of
    // separate assignments the generated sequence of temporary
    // variable assignments are inside a block this function also
    // generates a block where all the subexpressions of the
    // expression are linearized into a temporyary variable
    // initialization list
    void normalizeExpression(SgStatement* stmt, SgExpression* node);
  
    // transformation register functions
    /* transform subexpression with root ExprStatement into a list of separate assignments
       this function is used by normalizeExpression to normalize all sub-expressions of an expression
    */
    Normalization::TmpVarNrType registerSubExpressionTempVars(SgStatement* stmt, SgExpression* node, SubExprTransformationList& subExprTransformationList,bool insideExprToBeEliminated);

    Normalization::TmpVarNrType registerTmpVarDeclaration(SgStatement* stmt, SgType* type, SubExprTransformationList& subExprTransformationList);
    Normalization::TmpVarNrType registerTmpVarInitialization(SgStatement* stmt, SgExpression* expr, SubExprTransformationList& subExprTransformationList);
    Normalization::TmpVarNrType registerTmpVarInitialization(SgStatement* stmt, SgExpression* expr, Normalization::TmpVarNrType unaryTmpVarNr, SubExprTransformationList& subExprTransformationList);
    Normalization::TmpVarNrType registerTmpVarInitialization(SgStatement* stmt, SgExpression* expr, Normalization::TmpVarNrType lhsTmpVarNr, Normalization::TmpVarNrType rhsTmpVarNr, SubExprTransformationList& subExprTransformationList);
    Normalization::TmpVarNrType registerTmpVarInitialization(SgStatement* stmt, SgExpression* expr, Normalization::TmpVarNrType condTmpVarNr, Normalization::TmpVarNrType tbTmpVarNr, Normalization::TmpVarNrType fbTmpVarNr, SubExprTransformationList& subExprTransformationList);
    Normalization::TmpVarNrType registerTmpFalseBoolVarDecl(SgStatement* stmt, SgExpression* node, SubExprTransformationList& subExprTransformationList);
    // uses provided tmpVarNr. Does not generate a new tmpvarnr. expr only passed through.
    void registerTmpVarAssignment(SgStatement* stmt,SgExpression* expr, Normalization::TmpVarNrType lhsVarNr, Normalization::TmpVarNrType rhsVarNr, SubExprTransformationList& subExprTransformationList);
    // uses provided tmpVarNr. Does not generate a new tmpvarnr.
    void registerTmpVarAssignmentWithExpression(SgStatement* stmt,SgExpression* expr, Normalization::TmpVarNrType tmpVarNr, SubExprTransformationList& subExprTransformationList);
    void registerVoidExpression(SgStatement* stmt, SgExpression* expr, SubExprTransformationList& subExprTransformationList);
    void registerLogOpReplacement(SgStatement* stmt, SgExpression* expr, Normalization::TmpVarNrType declVarNr, SubExprTransformationList& subExprTransformationList);
    void registerBoolVarIfElseStmt(SgStatement* stmt, SgExpression  * expr, Normalization::TmpVarNrType declVarNr, Normalization::TmpVarNrType condVarNr, SgStatement* true_body, SgStatement* false_body, SubExprTransformationList& subExprTransformationList);
    void registerBoolVarIfStmt(SgStatement* stmt, SgExpression  * expr, Normalization::TmpVarNrType declVarNr, SubExprTransformationList& subExprTransformationList);
    void registerIfElseStmt(SgStatement* stmt, SgExpression  * expr, Normalization::TmpVarNrType declVarNr, SgStatement* true_body, SgStatement* false_body, SubExprTransformationList& subExprTransformationList);
    void registerCondOpIfElseStmt(SgStatement* stmt, SgExpression  * expr, Normalization::TmpVarNrType condOpVarNr, SgStatement* trueBody, SgStatement* falseBody, SubExprTransformationList& subExprTransformationList);
    void registerStmtRemoval(SgStatement* stmt, SubExprTransformationList& subExprTransformationList);

    // code generation functions
    SgVariableDeclaration* generateFalseBoolVarDecl(SgScopeStatement* scope);
    SgVariableDeclaration* generateVarDecl(SgType* type,SgScopeStatement* scope);
    void insertNormalizedSubExpressionFragment(SgStatement* fragment, SgStatement* existing);
    SgIfStmt* generateBoolVarIfElseStmt(SgExpression* cond, SgVarRefExp* varRefExp, SgStatement* true_body, SgStatement* false_body, SgScopeStatement* scope);
    SgIfStmt* generateIfElseStmt(SgExpression* cond, SgStatement* true_body, SgStatement* false_body);
    SgExprStatement* generateTmpVarAssignment(Normalization::TmpVarNrType lhsVarNr, Normalization::TmpVarNrType rhsVarNr);
    SgExprStatement* generateTmpVarAssignmentWithExpr(Normalization::TmpVarNrType lhsVarNr, SgExpression* expr);

    // obsolete functions
    // normalizes variable declarations T x=init to T x; x=init; 
    // if option onlyFunctionCalls is true, then only transform: T x=f(...) => T x; x=f();
    // Note: static variable initialization is excluded from normalization (would be wrong)
    void normalizeAllVariableDeclarations(SgNode* node, bool onlyFunctionCalls);
    /* Given 'Type x=init;' is transformed into 'Type x;' and returns 'x=init;'
       return nullptr if provided declaration is in global scope (cannot be normalized) */
    SgStatement* buildNormalizedVariableDeclaration(SgVariableDeclaration* varDecl);
    // create a lowering sequence of NormalizationOp objects

    // utility functions
    /* check if variable declaration is of the form T x=f(...); 
       note:
       type casts are not considered, e.g.: T x=(T)f(...) does not
       match.
    */
    bool isVarDeclWithFunctionCall(SgNode* node);

    // LEVEL 3 normalization
    /* If the given statement contains any break statements in its body,
       add a new label below the statement and change the breaks into
       gotos to that new label. Addresses for,while,switch stmts.
       Transforms continue to gotos in while-loops (not for-loops).
    */
    void normalizeBreakAndContinueStmts(SgNode *ast);
    void transformContinueToGotoStmts(SgWhileStmt* whileStmt);
    void transformContinueToGotoStmts(SgDoWhileStmt* whileStmt);

  public:
  // create a new variable declaration with the type of the provided expression and if requested also initializes it with this expression.
  // initWithExpression: use expression as initializer for declared variable, otherwise with no initializer
  // shareInitializerExpression: shares the provided expression as initializer, otherwise the expression is cloned. This option has no effect if initWithExpression is false.
    SgVariableDeclaration* buildVariableDeclarationForExpression(SgExpression* expression, SgScopeStatement* scope, bool shareExpression=false);
    SgVariableDeclaration* buildVariableDeclarationWithInitializerForExpression(SgExpression* expression, SgScopeStatement* scope, bool shareExpression=false);

    SgVarRefExp* buildVarRefExpForVariableDeclaration(SgVariableDeclaration* decl);
    std::string generateUniqueVariableName(SgScopeStatement* scope, std::string baseName);
    void setUniqueVariablePrefix(std::string);
    void setUniqueVariablePostfix(std::string);
  private:
    
    SgVariableDeclaration* buildVariableDeclarationForExpression(SgExpression* expression, SgScopeStatement* scope, bool initWithExpression, bool shareExpression);
    // private member variables
    
    // counter for generating new variable names
    Normalization::TmpVarNrType getTmpVarNr();
    void incTmpVarNr();
    // true for tmpVarNrs that have been registered. 0 should never be registered.
    bool isValidRegisteredTmpVarNr(Normalization::TmpVarNrType tmpVarNr);
    // a tmpVarNr is valid if : tmpVarNr > 0 and tmpVarNrExists(nr)
    bool isValidGeneratedTmpVarDeclNr(Normalization::TmpVarNrType tmpVarNr);
    // true if tmp var with this number has been generated
    bool tmpVarNrDeclExists(Normalization::TmpVarNrType tmpVarNr);
    static TmpVarNrType tmpVarNrCounter;
    static std::string tmpVarPrefix;
    std::string _tmpVarBaseName="tmp";
    typedef TmpVarNrType TmpVarMappingType1;
    typedef SgVariableDeclaration* TmpVarMappingType2;
    typedef std::pair<TmpVarMappingType1,TmpVarMappingType2> TmpVarMappingPair;
    std::map<TmpVarMappingType1,TmpVarMappingType2> tmpVarMapping;
    void addToTmpVarMapping(TmpVarNrType tmpVarNr, SgVariableDeclaration* decl);
    SgVariableDeclaration* getVarDecl(TmpVarNrType tmpVarNr);
    SgVarRefExp* getVarRefExp(TmpVarNrType tmpVarNr);

    // counter for generating new label names
    static int32_t labelNr;
    static std::string labelPrefix;

    std::list<NormalizationOp*> loweringSequence;

    void removeDefaultInliner();
    CodeThorn::InlinerBase* _inliner=0;
    bool _defaultInliner=true;
    string _uniqueVarPrefix="__";
    string _uniqueVarPostfix="__";
    static int32_t uniqueVarCounter;
  };
  
} // end of namespace CodeThorn

#endif
