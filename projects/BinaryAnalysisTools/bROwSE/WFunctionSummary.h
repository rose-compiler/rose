#ifndef bROwSE_WFunctionSummary_H
#define bROwSE_WFunctionSummary_H

#include <bROwSE/bROwSE.h>
#include <Partitioner2/Function.h>
#include <Wt/WContainerWidget>

namespace bROwSE {

// Presents function summary information
class WFunctionSummary: public Wt::WContainerWidget {
    Context &ctx_;
    P2::Function::Ptr function_;
    Wt::WTable *table_;
    Wt::WText *wName_, *wEntry_, *wBBlocks_, *wDBlocks_, *wInsns_, *wBytes_, *wDiscontig_, *wCallers_, *wCallsInto_;
    Wt::WText *wCallees_, *wCallsOut_, *wRecursive_, *wMayReturn_, *wStackDelta_;
public:

    WFunctionSummary(Context &ctx, Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), ctx_(ctx), table_(NULL), wName_(NULL), wEntry_(NULL), wBBlocks_(NULL), wDBlocks_(NULL),
          wInsns_(NULL), wBytes_(NULL), wDiscontig_(NULL), wCallers_(NULL), wCallsInto_(NULL), wCallees_(NULL),
          wCallsOut_(NULL), wRecursive_(NULL), wMayReturn_(NULL), wStackDelta_(NULL) {
        init();
    }

    void changeFunction(const P2::Function::Ptr &function);

private:
    void init();
};


} // namespace
#endif
