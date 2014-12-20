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
    WAddressSpace *wAddressSpace_;
public:
    WFunctionList(Context &ctx, FunctionListModel *model, Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), ctx_(ctx), model_(model), wAddressSpace_(NULL) {
        ASSERT_not_null(model);
        init();
    }

    /** Select the address segment containing the specified address. */
    void selectSegment(rose_addr_t va, const Wt::WMouseEvent&);

    /** Select the function containing the specified address. If more than one matches then the one with the lowest entry
     * address is selected. */
    void selectFunction(rose_addr_t va, const Wt::WMouseEvent&);

    /** Select function by pointer. */
    Wt::WModelIndex selectFunction(const P2::Function::Ptr&);

    /** Select function by table row. */
    void selectFunction(const Wt::WModelIndex&);

    /** Emitted when a table row is clicked. */
    Wt::Signal<P2::Function::Ptr>& tableRowClicked();

    /** Emitted when a row of the table is double clicked. */
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
