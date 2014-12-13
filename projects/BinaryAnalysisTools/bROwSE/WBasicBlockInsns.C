#include <bROwSE/WBasicBlockInsns.h>

#include <bROwSE/InstructionListModel.h>
#include <bROwSE/WInstructionList.h>

namespace bROwSE {

void
WBasicBlockInsns::init(Context &ctx) {
    model_ = new InstructionListModel(ctx);
    insnList_ = new WInstructionList(model_, this);
}

void
WBasicBlockInsns::changeBasicBlock(const P2::BasicBlock::Ptr &bblock) {
    if (bblock == bblock_)
        return;
    bblock_ = bblock;
    if (NULL==bblock) {
        model_->clear();
        return;
    }
    model_->changeInstructions(bblock->instructions());
}

} // namespace
