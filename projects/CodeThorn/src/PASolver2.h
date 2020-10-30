#include "DFAnalysisBase.h"

namespace CodeThorn {

class PASolver2 : public DFAbstractSolver {
 public:
    PASolver2();
    PASolver2(DFAnalysisBase* dfAnalysisBase);
    virtual void runSolver();
    virtual void setDFAnalysis(DFAnalysisBase* analysis);
    void setId(int);
    int getId();
    void setTrace(bool trace) ROSE_OVERRIDE { _trace=trace; }
    void computeCombinedPreInfo(Label lab,Lattice& info) override;
    void computePostInfo(Label lab,Lattice& info);
 protected:
    DFAnalysisBase* _dfAnalysisBase = nullptr;
 private:
    bool _trace;
    int _id;
};
}
