#ifndef bROwSE_WBasicBlockInsns_H
#define bROwSE_WBasicBlockInsns_H

#include <bROwSE/bROwSE.h>
#include <Partitioner2/BasicBlock.h>
#include <Wt/WContainerWidget>

namespace bROwSE {

class InstructionListModel;
class WInstructionList;

// The instructions for a basic block
class WBasicBlockInsns: public Wt::WContainerWidget {
    P2::BasicBlock::Ptr bblock_;                        // currently displayed basic block
    InstructionListModel *model_;
    WInstructionList *insnList_;
public:
    WBasicBlockInsns(Context &ctx, Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), model_(NULL), insnList_(NULL) {
        init(ctx);
    }

    void changeBasicBlock(const P2::BasicBlock::Ptr &bblock);

private:
    void init(Context&);
};

} // namespace
#endif
