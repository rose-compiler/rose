#ifndef bROwSE_WBasicBlockSummary_H
#define bROwSE_WBasicBlockSummary_H

#include <bROwSE/bROwSE.h>
#include <Partitioner2/BasicBlock.h>
#include <Wt/WContainerWidget>
#include <Wt/WTable>
#include <Wt/WText>

namespace bROwSE {

class WBasicBlockSummary: public Wt::WContainerWidget {
    Context &ctx_;
    P2::BasicBlock::Ptr bblock_;
    Wt::WTable *table_;
    Wt::WText *wHasOpaquePredicates_;                   // FIXME[Robb P. Matzke 2014-09-15]: should be a bb list
    Wt::WText *wDataBlocks_;                            // FIXME[Robb P. Matzke 2014-09-15]: should be a db list
    Wt::WText *wIsFunctionCall_;                        // does this block look like a function call?
    Wt::WText *wIsFunctionReturn_;                      // does this block appear to return from a function call?
    Wt::WText *wStackDeltaIn_;                          // symbolic expression for the block's incoming stack delta
    Wt::WText *wStackDeltaOut_;                         // symbolic expression for the block's outgoing stack delta
public:

    WBasicBlockSummary(Context &ctx, Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), ctx_(ctx), table_(NULL),
          wHasOpaquePredicates_(NULL), wDataBlocks_(NULL), wIsFunctionCall_(NULL), wIsFunctionReturn_(NULL),
          wStackDeltaIn_(NULL), wStackDeltaOut_(NULL) {
        init(ctx);
    }

    void changeBasicBlock(const P2::BasicBlock::Ptr &bblock);

private:
    void init(Context&);
};

} // namespace
#endif
