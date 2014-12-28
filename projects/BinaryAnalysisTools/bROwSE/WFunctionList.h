#ifndef bROwSE_WFunctionList_H
#define bROwSE_WFunctionList_H

#include <bROwSE/bROwSE.h>
#include <Wt/WTableView>

namespace bROwSE {

class FunctionListModel;
class WAddressSpace;

// Presents a list of functions
class WFunctionList: public Wt::WContainerWidget {
    enum { SEGMENT_BAR=0, FUNCTION_BAR=1 };

    Context &ctx_;
    P2::Function::Ptr function_;                                // current function
    WAddressSpace *wAddressSpace_;                              // visual representation of the address space
    Wt::WTableView *tableView_;                                 // function list table
    FunctionListModel *model_;                                  // model for the function list table
    Wt::Signal<P2::Function::Ptr> functionChanged_;             // current function changed
    Wt::Signal<P2::Function::Ptr> functionRowClicked_;          // clicked on a row of the function table
    Wt::Signal<P2::Function::Ptr> functionRowDoubleClicked_;    // double click on a row of the function table
    Wt::Signal<P2::Function::Ptr, rose_addr_t, Wt::WMouseEvent> functionAddressClicked_;
    Wt::Signal<rose_addr_t, Wt::WMouseEvent> segmentAddressClicked_;

public:
    WFunctionList(Context &ctx, Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), ctx_(ctx), wAddressSpace_(NULL), tableView_(NULL), model_(NULL) {
        init();
    }

    /** Load functions from the partitioner.
     *
     *  This may be called at any time when the partitioner's list of functions changes.  It invalidates all data in the table
     *  model, so it may not be very fast. */
    void reload();

    /** Functions being displayed.
     *
     *  Returns a vector of the functions that are being displayed in the order they're being displayed. */
    const std::vector<P2::Function::Ptr>& functions() const;

    /** Currently function.
     *
     *  Returns the currently selected function, or a null pointer if no function is currently selected. The currently selected
     *  function is highlighted in the address space and in the function table. */
    P2::Function::Ptr currentFunction() const { return function_; }

    /** Change the currently selected function.
     *
     *  Makes the specified function the currently selected function. If the specified function was originally different than
     *  the current function then a @ref functionChanged signal is emitted. */
    void changeFunction(const P2::Function::Ptr&);      // also private version

    /** Redraw the address space based on functions' ATTR_Heat. */
    void updateFunctionHeatMaps();

    /** Signal emitted when the current function changes.
     *
     *  This signal is emitted when the current function changes. The new current function is the argument. */
    Wt::Signal<P2::Function::Ptr>& functionChanged() { return functionChanged_; }

    /** Signal emitted when a table row is clicked.
     *
     *  If the clicked row does not represent the currently selected function then changeFunction is called first, which emits
     *  a @ref functionChanged event.  The @ref functionChanged event will be emitted before the @ref functionRowClicked
     *  event. */
    Wt::Signal<P2::Function::Ptr>& functionRowClicked() { return functionRowClicked_; }

    /** Signal emitted when a table row is double clicked.
     *
     *  If the clicked row does not represent the currently selected function then changeFunction is called first, which emits
     *  a @ref functionChanged event.  The @ref functionChanged event will be emitted before the @ref functionRowDoubleClicked
     *  event. */
    Wt::Signal<P2::Function::Ptr>& functionRowDoubleClicked() { return functionRowDoubleClicked_; }

    /** Signal emitted when a function address is clicked.
     *
     *  This signal is emitted when one clicks on the lower gutter in the address space widget. If the clicked function does
     *  not represent the currently selected function then changeFunction is called first, which emits a @ref functionChanged
     *  event.  The @ref functionChanged event will be emitted before the @ref functionAddressClicked event.
     *
     *  This signal is emitted only when a valid function address is clicked; clicking on areas of the address map that don't
     *  correspond to a function has no effect. */
    Wt::Signal<P2::Function::Ptr, rose_addr_t, Wt::WMouseEvent>& functionAddressClicked() { return functionAddressClicked_; }

    /** Signal emitted when a segment address is clicked.
     *
     *  This signal is emitted when one clicks on the upper gutter in the address space widget.  If the address corresponds to
     *  a mapped address then the address space widget is zoomed to encompass only that memory segment and then this signal is
     *  emitted. */
    Wt::Signal<rose_addr_t, Wt::WMouseEvent>& segmentAddressClicked() { return segmentAddressClicked_; }

private:
    void init();
    
    /** Select the address segment containing the specified address.
     *
     *  This gets called when a segment address is clicked in the top gutter of the address space. It zooms the address space
     *  to show that segment and emits a @ref segmentAddressClicked signal. */
    void selectSegmentByAddress(rose_addr_t va, const Wt::WMouseEvent&);

    /** Change current function by address.
     *
     *  Changes the current function to be that function which contains the specified code or data address.  If more than one
     *  function contains the address then the one with the lowest entry address is used. If the current function changes then
     *  a @ref functionChanged signal is emitted.  Emits a @ref functionAddressClicked signal (after the optional @ref
     *  functionChanged signal). */
    void selectFunctionByAddress(rose_addr_t va, const Wt::WMouseEvent&);

    /** Select function by table row.
     *
     *  Changes the current function to be that which is represented by the specified table row, eventually emitting a @ref
     *  functionChanged signal if necessary.  Also emits either a @ref functionRowClicked or @ref functionRowDoubleClicked
     *  signal (1=click, 2=double-click) following any @ref functionChanged signal
     *
     * @{ */
    void selectFunctionByRow1(const Wt::WModelIndex&);
    void selectFunctionByRow2(const Wt::WModelIndex&);
    /** @} */
};

} // namespace
#endif
