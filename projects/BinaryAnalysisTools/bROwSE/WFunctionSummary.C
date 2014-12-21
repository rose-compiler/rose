#include <bROwSE/WFunctionSummary.h>

#include <Color.h>                                      // ROSE
#include <Wt/WTable>
#include <Wt/WText>

using namespace rose;

namespace bROwSE {

static const size_t NCOLS = 3;                          // number of columns for analysis results

void
WFunctionSummary::init() {
    analyzers().push_back(FunctionEntryAddress::instance());
    analyzers().push_back(FunctionName::instance());
    analyzers().push_back(FunctionSizeBytes::instance());
    analyzers().push_back(FunctionSizeInsns::instance());
    analyzers().push_back(FunctionSizeBBlocks::instance());
    analyzers().push_back(FunctionSizeDBlocks::instance());
    analyzers().push_back(FunctionImported::instance());
    analyzers().push_back(FunctionExported::instance());
    analyzers().push_back(FunctionNCallers::instance());
    analyzers().push_back(FunctionNReturns::instance());
    analyzers().push_back(FunctionMayReturn::instance());
    analyzers().push_back(FunctionStackDelta::instance());

    // Build a table to hold analysis results. The results will be organized into NCOLS each occupying two table columns (one
    // for the name and one for the value).
    const size_t NROWS = (analyzers_.size() + NCOLS - 1) / NCOLS;
    wAnalysisResultTable_ = new Wt::WTable(this);
    //wAnalysisResultTable_->setWidth("100%");
    Wt::WCssDecorationStyle labelDecor;
    labelDecor.setBackgroundColor(toWt(Color::HSV(0, 0, 0.95)));
    for (size_t col=0, i=0; col<NCOLS && i<analyzers_.size(); ++col) {
        for (size_t row=0; row<NROWS && i<analyzers_.size(); ++row) {
            Wt::WText *label = new Wt::WText("<b>" + analyzers_[i]->header() + "</b>");
            label->setToolTip(analyzers_[i]->toolTip());
            wAnalysisResultTable_->elementAt(row, 2*col+0)->addWidget(label);
            wAnalysisResultTable_->elementAt(row, 2*col+0)->setContentAlignment(Wt::AlignRight);
            wAnalysisResultTable_->elementAt(row, 2*col+0)->setDecorationStyle(labelDecor);
            analyzerResults_.push_back(new Wt::WText);
            analyzerResults_.back()->setToolTip(analyzers_[i]->toolTip());
            wAnalysisResultTable_->elementAt(row, 2*col+1)->addWidget(analyzerResults_.back());
            wAnalysisResultTable_->elementAt(row, 2*col+1)->setPadding(Wt::WLength(1, Wt::WLength::FontEm), Wt::Left);
            wAnalysisResultTable_->elementAt(row, 2*col+1)->setPadding(Wt::WLength(10, Wt::WLength::FontEm), Wt::Right);
            ++i;
        }
    }
}

void
WFunctionSummary::changeFunction(const P2::Function::Ptr &function) {
    if (function == function_)
        return;
    function_ = function;

    const size_t NROWS = (analyzers_.size() + NCOLS - 1) / NCOLS;
    for (size_t col=0, i=0; col<NCOLS && i<analyzers_.size(); ++col) {
        for (size_t row=0; row<NROWS && i<analyzers_.size(); ++row) {
            Wt::WString str;
            if (function) {
                boost::any value = analyzers_[i]->data(ctx_.partitioner, function);
                str = analyzers_[i]->toString(value);
            }
            analyzerResults_[i]->setText(str);
            ++i;
        }
    }
}

} // namespace
