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

/** Function control flow graph.
 *
 *  This widget displays a function control flow graph with clickable elements. It also maintains a forward/back style
 *  navigation bar that gets updated each time the current function is changed (via @ref changeFunction). */
class WFunctionCfg: public Wt::WContainerWidget {
    Context &ctx_;
    P2::Function::Ptr function_;                        // currently-displayed function
    WFunctionNavigation *wNavigation_;
    WAddressComboBox *wAddresses_;                      // addresses related to current basic block, etc.
    Wt::WScrollArea *wScrollArea_;                      // contains a CFG image or a message
    Wt::WImage *wImage_;                                // image for the CFG
    Wt::WText *wMessage_;
    Wt::WAnchor *wDownloadAnchor_;                      // Download link for CFG GraphViz *.dot file
    typedef std::pair<Wt::WRectArea*, rose_addr_t> AreaAddr;
    std::vector<AreaAddr> areas_;
    Wt::Signal<P2::Function::Ptr> functionChanged_;     // emitted when the current function changes
    Wt::Signal<P2::BasicBlock::Ptr> basicBlockClicked_; // emitted when a basic block node is clicked
    Wt::Signal<P2::Function::Ptr> functionClicked_;     // emitted when a function node is clicked
    Wt::Signal<rose_addr_t> addressClicked_;            // emitted when an address is clicked
public:
    WFunctionCfg(Context &ctx, Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), ctx_(ctx),
          wNavigation_(NULL), wAddresses_(NULL), wScrollArea_(NULL), wImage_(NULL), wMessage_(NULL), wDownloadAnchor_(NULL) {
        init();
    }

    /** Current function. */
    P2::Function::Ptr function() const {
        return function_;
    }

    /** Reset the navigation information.
     *
     *  The navigation is cleared and re-initialized to contain only the current function (if any). */
    void clearNavigation();

    /** Change displayed information.
     *
     *  Changes the widget so it's displaying information about the specified function.  If this is different than the function
     *  it was displaying then a @ref functionChanged signal is emitted. */
    void changeFunction(const P2::Function::Ptr &function);

    /** Signal emitted when the current function changes.
     *
     *  This is emitted whenever @ref changeFunction changes the current function. */
    Wt::Signal<P2::Function::Ptr>& functionChanged() {
        return functionChanged_;
    }

    /** Signal emitted when a basic block node is clicked. */
    Wt::Signal<P2::BasicBlock::Ptr>& basicBlockClicked() {
        return basicBlockClicked_;
    }

    /** Signal emitted when a function node is clicked. */
    Wt::Signal<P2::Function::Ptr>& functionClicked() {
        return functionClicked_;
    }

    /** Signal emitted when an address is clicked.
     *
     *  The CFG toolbar contains a list of addresses and when one of them is activated then this signal is generated. */
    Wt::Signal<rose_addr_t>& addressClicked() {
        return addressClicked_;
    }
    
private:
    void init();

    // Just like changeFunction but doesn't emit the functionChanged signal
    void changeFunctionNoSignal(const P2::Function::Ptr&);

    // Called when a basic block node is clicked. Emits a basicBlockClicked signal.
    void selectBasicBlock(const Wt::WMouseEvent &event);

    // Called when a function node is clicked. Emits a functionClicked signal.
    void selectFunction(const Wt::WMouseEvent &event);

    // Forward the wAddresses_ activated signal to emit an addressClicked signal.
    void emitAddressClicked(rose_addr_t, Wt::WString, size_t);
};

} // namespace
#endif
