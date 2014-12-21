#ifndef bROwSE_WFunctionList_H
#define bROwSE_WFunctionList_H

#include <bROwSE/bROwSE.h>
#include <Wt/WTableView>

namespace bROwSE {

class FunctionListModel;
class WAddressSpace;

// Presents a list of functions
class WFunctionList: public Wt::WContainerWidget {
    Context &ctx_;
    FunctionListModel *model_;
    Wt::WTableView *tableView_;
    Wt::Signal<P2::Function::Ptr> tableRowClicked_;             // emitted when a table row is clicked
    Wt::Signal<P2::Function::Ptr> tableRowDoubleClicked_;       // emitted when a table row is double clicked
    Wt::Signal<P2::Function::Ptr> functionSelected_;            // emitted when a function is selected
    WAddressSpace *wAddressSpace_;
public:
    WFunctionList(Context &ctx, Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), ctx_(ctx), model_(NULL), wAddressSpace_(NULL) {
        init();
    }

    /** Select the address segment containing the specified address. */
    void selectSegment(rose_addr_t va, const Wt::WMouseEvent&);

    /** Select the function containing the specified address. If more than one matches then the one with the lowest entry
     * address is selected. Will ultimately emit a @ref functionSelected signal. */
    void selectFunction(rose_addr_t va, const Wt::WMouseEvent&);

    /** Select function by pointer.  Will ultimately emit a @ref functionSelected signal. */
    Wt::WModelIndex selectFunction(const P2::Function::Ptr&);

    /** Select function by table row. Will ultimately emit a @ref functionSelected signal. */
    void selectFunction(const Wt::WModelIndex&);

    /** Emitted when a new function is selected. This signal is emitted by the @ref selectFunction method, which is usually
     * called in response to the user clicking on a function in the address space or the table. */
    Wt::Signal<P2::Function::Ptr>& functionSelected();

    /** Emitted when a table row is clicked. The functionSelected signal is emitted first if necessary. */
    Wt::Signal<P2::Function::Ptr>& tableRowClicked();

    /** Emitted when a row of the table is double clicked. The functionSelected signal is emitted first if necessary. */
    Wt::Signal<P2::Function::Ptr>& tableRowDoubleClicked();

    /** Redraw the address space based on functions' ATTR_HEAT. */
    void updateFunctionHeatMaps();

private:
    void init();

    void clickRow(const Wt::WModelIndex &idx);

    void doubleClickRow(const Wt::WModelIndex &idx);
};

} // namespace
#endif
