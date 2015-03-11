#ifndef bROwSE_WSemantics_H
#define bROwSE_WSemantics_H

#include <bROwSE/bROwSE.h>
#include <Partitioner2/DataFlow.h>
#include <Wt/WContainerWidget>

namespace bROwSE {

class AbstractLocationModel;

class WSemantics: public Wt::WContainerWidget {
public:
    enum Mode { REG_INIT, REG_FINAL, MEM_INIT, MEM_FINAL };

private:
    Context &ctx_;
    P2::Function::Ptr function_;
    P2::BasicBlock::Ptr bblock_;
    Mode mode_;

    Wt::WText *wAddress_;
    WToggleButton *wRegInit_, *wRegFinal_, *wMemInit_, *wMemFinal_;

    AbstractLocationModel *model_;
    Wt::WTableView *wTableView_;

public:
    explicit WSemantics(Context &ctx, Wt::WContainerWidget *parent = NULL)
        : Wt::WContainerWidget(parent), ctx_(ctx), mode_(REG_INIT), wAddress_(NULL), wRegInit_(NULL), wRegFinal_(NULL),
          wMemInit_(NULL), wMemFinal_(NULL), model_(NULL), wTableView_(NULL) {
        init();
    }

    void changeBasicBlock(const P2::BasicBlock::Ptr&, Mode);
    void changeBasicBlock(const P2::BasicBlock::Ptr&);
    void changeFunction(const P2::Function::Ptr&);
    void changeMode(Mode);

private:
    void init();
};

} // namespace
#endif
