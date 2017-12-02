#ifndef NORMALIZATION_H
#define NORMALIZATION_H

#include <list>
#include <utility>

class SgWhileStmt;
class SgDoWhileStmt;
class SgForStatement;
class SgSwitchStatement;

namespace SPRAY {

  class Lowering {
  public:
    Lowering();
    virtual void analyse();
    virtual void transform()=0;
    ~Lowering();
  };
  class WhileStmtLowering : public Lowering {
  public:
    WhileStmtLowering(SgWhileStmt* node);
    void analyse();
    void transform();
    virtual ~WhileStmtLowering();
  private:
    SgWhileStmt* node;
  };

  class DoWhileStmtLowering : public Lowering {
  public:
    DoWhileStmtLowering(SgDoWhileStmt* node);
    void transform();
  private:
    SgDoWhileStmt* node;
  };
  class ForStmtLowering : public Lowering {
  public:
    ForStmtLowering(SgForStatement* node);
    void transform();
  private:
    SgForStatement* node;
  };
  class SwitchStmtLowering : public Lowering {
  public:
    SwitchStmtLowering(SgSwitchStatement* node);
    void transform();
  private:
    SgSwitchStatement* node;
  };

  class Normalization {
  public:
    void normalizeAst(SgNode* root);
    size_t inlineFunctions(SgNode* root);
    static void createGotoStmtAtEndOfBlock(SgBasicBlock* block, SgStatement* target);
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

    typedef std::list<std::pair<SgStatement*,SgExpression*> > TransformationList;
    TransformationList transformationList;
    std::list<Lowering*> loweringSequence;
  };
  
} // end of namespace SPRAY

#endif
