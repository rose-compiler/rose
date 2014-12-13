#ifndef bROwSE_WFunctionCfg_H
#define bROwSE_WFunctionCfg_H

#include <bROwSE/bROwSE.h>
#include <Partitioner2/BasicBlock.h>
#include <Partitioner2/Function.h>
#include <Wt/WContainerWidget>
#include <Wt/WImage>
#include <Wt/WRectArea>
#include <Wt/WText>

namespace bROwSE {

// Control flow graph for a function

class WFunctionCfg: public Wt::WContainerWidget {
    Context &ctx_;
    P2::Function::Ptr function_;                        // currently-displayed function
    Wt::WImage *wImage_;                                // image for the CFG
    Wt::WText *wMessage_;
    typedef std::pair<Wt::WRectArea*, rose_addr_t> AreaAddr;
    std::vector<AreaAddr> areas_;
    Wt::Signal<P2::BasicBlock::Ptr> basicBlockClicked_;
    Wt::Signal<P2::Function::Ptr> functionClicked_;
public:
    WFunctionCfg(Context &ctx, Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), ctx_(ctx), wImage_(NULL) {
        init();
    }

    // Returns currently displayed function
    P2::Function::Ptr function() const {
        return function_;
    }

    // Display information for the specified function
    void changeFunction(const P2::Function::Ptr &function);

    // Emitted when a basic block vertex is clicked.
    Wt::Signal<P2::BasicBlock::Ptr>& basicBlockClicked() {
        return basicBlockClicked_;
    }

    // Emitted when a function vertex is clicked.
    Wt::Signal<P2::Function::Ptr>& functionClicked() {
        return functionClicked_;
    }
    
private:
    void init();

    void selectBasicBlock(const Wt::WMouseEvent &event);

    void selectFunction(const Wt::WMouseEvent &event);
};

} // namespace
#endif
