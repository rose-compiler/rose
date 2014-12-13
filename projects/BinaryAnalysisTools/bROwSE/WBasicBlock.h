#ifndef bROwSE_WBasicBlock_H
#define bROwSE_WBasicBlock_H

#include <bROwSE/bROwSE.h>
#include <Wt/WContainerWidget>

namespace bROwSE {

class WBasicBlockInsns;
class WBasicBlockSummary;

// All information about basic blocks
class WBasicBlock: public Wt::WContainerWidget {
    P2::BasicBlock::Ptr bblock_;                        // currently displayed basic block
    WBasicBlockInsns *wInsns_;                          // instruction list for the basic block
    WBasicBlockSummary *wSummary_;                      // summary information for the basic block
public:
    WBasicBlock(Context &ctx, Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), wInsns_(NULL), wSummary_(NULL) {
        init(ctx);
    }

    void changeBasicBlock(const P2::BasicBlock::Ptr &bblock);

private:
    void init(Context&);
};


} // namespace
#endif
