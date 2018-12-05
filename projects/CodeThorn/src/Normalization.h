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

namespace SPRAY {

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
      // turns single statements into blocks with one statement
      bool normalizeSingleStatements=true;
      // replace for with while
      bool eliminateForStatements=true;
      // replace while with if/goto
      bool eliminateWhileStatements=true; 
      // eliminate operators '||', '&&' (not supported yet)
      const bool eliminateShortCircuitOperators=false;
      // eliminate operator '?' (not supported yet)
      const bool eliminateConditionalExpressionOp=false;
      bool hoistConditionExpressions=true;
      bool normalizeExpressions=true;
      bool normalizeVariableDeclarations=true;

      // puts the sequence of normalized expressions in a block. This
      // way the scrope of temporary variables ends right after the
      // last initialization (or assignment) of the normalized
      // expression.
      // requires: normalizeVariableDeclarations==true
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

    // applies normalization on entire AST with normalization level 0-3.
    // level 0: no normalization
    // level 1: all expressions with a function call (and all implied normalizations)
    // level 2: all expressions (and all implied normalizations)
    // level 3: all expresisons and lowering of all control constructs (only if+gotos remain)
    void normalizeAst(SgNode* root, unsigned int normalizationLevel);

    static void setLabelPrefix(std::string prefix);
    static SgLabelStatement* createLabel(SgStatement* target);
    static std::string newLabelName();

    static bool hasFunctionCall(SgExpression* expr);
    static bool isWithinBlockStmt(SgExpression* exp);

    // enable/disable inling. By default off.
    void setInliningOption(bool flag);
    bool getInliningOption();
    // calls ROSE SageInterface function for inlining
    SPRAY::InlinerBase* getInliner();
    // allows to use custom inliner. Default is the inliner from the SageInterface.
    void setInliner(SPRAY::InlinerBase*);

    static void createGotoStmtAtEndOfBlock(SgLabelStatement* newLabel, SgBasicBlock* block, SgStatement* target);
    static SgGotoStatement* createGotoStmtAndInsertLabel(SgLabelStatement* newLabel, SgStatement* target);
    // transforms Label1: Label2: LabelN: Stmt; ==> Label1:; Label2:; LabelN:; Stmt;
    // requires: normalizeSingleStatementsToBlocks()
    // the associated stmt node (= the label node's child) remains unchanged, except for the update of its parent pointer.
    static void normalizeLabel(SgLabelStatement* label);

  protected:
    // assumes correctly configured options (invoked by normalizeAst(root,level))
    void normalizeAst(SgNode* root);

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

    // normalizes variable declarations T x=init to T x; x=init; 
    void normalizeAllVariableDeclarations(SgNode* node);
    /* Given 'Type x=init;' is transformed into 'Type x;' and returns 'x=init;'
       return nullptr if provided declaration is in global scope (cannot be normalized) */
    SgStatement* buildNormalizedVariableDeclaration(SgVariableDeclaration* varDecl);
    // create a lowering sequence of NormalizationOp objects

    void createLoweringSequence(SgNode* root);
    // apply lowering sequence generated by createLoweringSequence
    void applyLoweringSequence();

    typedef std::list<std::pair<SgStatement*,SgExpression*> > SubExprTransformationList;
    typedef std::list<SubExprTransformationList> ExprTransformationList;
    ExprTransformationList exprTransformationList;

    // transform expression with root ExprStatement into a list of
    // separate assignments the generated sequence of temporary
    // variable assignments are inside a block this function also
    // generates a block where all the subexpressions of the
    // expression are linearized into a temporyary variable
    // initialization list
    void normalizeExpression(SgExprStatement* stmt, SgExpression* node);

    // transform subexpression with root ExprStatement into a list of separate assignments
    // this function is used by normalizeExpression to normalize all sub-expressions of an expression
    void normalizeSubExpression(SgExprStatement* stmt, SgExpression* node, SubExprTransformationList& subExprTransformationList);
    void generateTmpVarAssignment(SgExprStatement* stmt, SgExpression* expr, SubExprTransformationList& subExprTransformationList);

    /* If the given statement contains any break statements in its body,
       add a new label below the statement and change the breaks into
       gotos to that new label. Addresses for,while,switch stmts.
       Transforms continue to gotos in while-loops (not for-loops).
    */
    void normalizeBreakAndContinueStmts(SgNode *ast);
    void transformContinueToGotoStmts(SgWhileStmt* whileStmt);
    void transformContinueToGotoStmts(SgDoWhileStmt* whileStmt);

    // counter for generating new variable names (currently not used)
    static int32_t tmpVarNr;
    // counter for generating new label names
    static int labelNr;
    static std::string labelPrefix;

    std::list<NormalizationOp*> loweringSequence;

    void removeDefaultInliner();
    SPRAY::InlinerBase* _inliner=0;
    bool _defaultInliner=true;

  };
  
} // end of namespace SPRAY

#endif
