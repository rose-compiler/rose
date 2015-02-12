#include <bROwSE/WBasicBlock.h>
#include <bROwSE/WBasicBlockInsns.h>
#include <bROwSE/WBasicBlockSummary.h>

namespace bROwSE {

void
WBasicBlock::init(Context &ctx) {
    wInsns_ = new WBasicBlockInsns(ctx, this);
    wSummary_ = new WBasicBlockSummary(ctx, this);
}

void
WBasicBlock::changeBasicBlock(const P2::BasicBlock::Ptr &bblock) {
    if (bblock == bblock_)
        return;
    bblock_ = bblock;
    wSummary_->changeBasicBlock(bblock);
    wInsns_->changeBasicBlock(bblock);
}


} // namespace
