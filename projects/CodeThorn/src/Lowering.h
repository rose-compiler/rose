#ifndef NORMALIZATION_H
#define NORMALIZATION_H

#include <list>
#include <utility>

class SgWhileStmt;
class SgDoWhileStmt;
class SgForStatement;
class SgSwitchStatement;

namespace SPRAY {

  class LoweringOp {
  public:
    LoweringOp();
    virtual void analyse();
    virtual void transform()=0;
    ~LoweringOp();
  };
  class WhileStmtLoweringOp : public LoweringOp {
  public:
    WhileStmtLoweringOp(SgWhileStmt* node);
    void analyse();
    void transform();
    virtual ~WhileStmtLoweringOp();
  private:
    SgWhileStmt* node;
  };

  class DoWhileStmtLoweringOp : public LoweringOp {
  public:
    DoWhileStmtLoweringOp(SgDoWhileStmt* node);
    void transform();
  private:
    SgDoWhileStmt* node;
  };
  class ForStmtLoweringOp : public LoweringOp {
  public:
    ForStmtLoweringOp(SgForStatement* node);
    void transform();
  private:
    SgForStatement* node;
  };
  class SwitchStmtLoweringOp : public LoweringOp {
  public:
    SwitchStmtLoweringOp(SgSwitchStatement* node);
    void transform();
  private:
    SgSwitchStatement* node;
  };

  class Lowering {
  public:
    void lowerAst(SgNode* root);
    size_t inlineFunctions(SgNode* root);
    static void createGotoStmtAtEndOfBlock(SgBasicBlock* block, SgStatement* target);
    static SgGotoStatement* createGotoStmtAndInsertLabel(SgBasicBlock* block, SgStatement* target);
    static void setLabelPrefix(std::string prefix);
    static std::string newLabelName();
  private:
    void createLoweringSequence(SgNode* root);
    void applyLoweringSequence();
    void normalizeExpressions(SgNode* node);
    void normalizeExpression(SgExprStatement* stmt, SgExpression* node);
    void generateTmpVarAssignment(SgExprStatement* stmt, SgExpression* expr);
    void normalizeBlocks(SgNode* node);
    // Finds needle in haystack and returns true if found.  Needle is a single node (possibly an invalid pointer and will not be
    // dereferenced) and haystack is the root of an abstract syntax (sub)tree.
    static bool isAstContaining(SgNode *haystack, SgNode *needle);
    void convertAllForsToWhiles (SgNode* top);
    /* If the given statement contains any break statements in its body,
       add a new label below the statement and change the breaks into
       gotos to that new label.
    */
    void changeBreakStatementsToGotos (SgNode *ast);

    // counter for generating new variable names (currently not used)
    static int32_t tmpVarNr;
    // counter for generating new label names
    static int labelNr;
    static std::string labelPrefix;

    typedef std::list<std::pair<SgStatement*,SgExpression*> > TransformationList;
    TransformationList transformationList;
    std::list<LoweringOp*> loweringSequence;
  };
  
} // end of namespace SPRAY

#endif
