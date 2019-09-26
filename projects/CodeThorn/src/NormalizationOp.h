#ifndef NORMALIZATION_OP_H
#define NORMALIZATION_OP_H

namespace CodeThorn {

  class NormalizationOp {
  public:
    NormalizationOp();
    virtual void analyse();
    virtual void transform()=0;
    virtual ~NormalizationOp();
  };

  class NormalizationOpWhileStmt : public NormalizationOp {
  public:
    NormalizationOpWhileStmt(SgWhileStmt* node);
    void transform();
  private:
    SgWhileStmt* node;
  };

  class NormalizationOpDoWhileStmt : public NormalizationOp {
  public:
    NormalizationOpDoWhileStmt(SgDoWhileStmt* node);
    void transform();
  private:
    SgDoWhileStmt* node;
  };

  class ForStmtLoweringOp : public NormalizationOp {
  public:
    ForStmtLoweringOp(SgForStatement* node);
    void transform();
  private:
    SgForStatement* node;
  };
  class SwitchStmtLoweringOp : public NormalizationOp {
  public:
    SwitchStmtLoweringOp(SgSwitchStatement* node);
    void transform();
  private:
    SgSwitchStatement* node;
  };
  
}

#endif
