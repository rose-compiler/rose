#ifndef bROwSE_WFunctionSummary_H
#define bROwSE_WFunctionSummary_H

#include <bROwSE/bROwSE.h>
#include <bROwSE/FunctionUtil.h>
#include <Partitioner2/Function.h>
#include <Wt/WContainerWidget>

namespace bROwSE {

// Presents function summary information
class WFunctionSummary: public Wt::WContainerWidget {
    Context &ctx_;
    P2::Function::Ptr function_;                        // current function
    std::vector<FunctionAnalyzer::Ptr> analyzers_;
    Wt::WTable *wAnalysisResultTable_;
    std::vector<Wt::WText*> analyzerResults_;           // one per analyzer

public:
    WFunctionSummary(Context &ctx, Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), ctx_(ctx), wAnalysisResultTable_(NULL) {
        init();
    }

    /** Vector of analyzers whose results should be displayed.  The array should be initialized during construction and not
     * modified thereafter.
     *
     * @{ */
    const std::vector<FunctionAnalyzer::Ptr>& analyzers() const { return analyzers_; }
    std::vector<FunctionAnalyzer::Ptr>& analyzers() { return analyzers_; }
    /** @} */

    void changeFunction(const P2::Function::Ptr &function);

private:
    void init();
};


} // namespace
#endif
