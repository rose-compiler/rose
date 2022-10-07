#include "DFAnalysisBase.h"

namespace CodeThorn {

class DFSolver2 : public DFAbstractSolver {
 public:
    DFSolver2();
    DFSolver2(DFAnalysisBase* dfAnalysisBase);
    virtual void runSolver();
    virtual void setDFAnalysis(DFAnalysisBase* analysis);
    void setId(int);
    int getId();
    void computeCombinedPreInfo(Label lab,Lattice& info) override;
    void computePostInfo(Label lab,Lattice& info);
 protected:
    DFAnalysisBase* _dfAnalysisBase = nullptr;
 private:
    int _id;
};
}
