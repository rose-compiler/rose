#ifndef bROwSE_WAddressComboBox_H
#define bROwSE_WAddressComboBox_H

#include <bROwSE/bROwSE.h>
#include <Sawyer/Map.h>
#include <Wt/WContainerWidget>

namespace bROwSE {

/** A combo box containing labels corresponding to addresses.
 *
 *  The entries in the combo box are sorted by label, and activating an entry will emit a signal with the corresponding
 *  address and label. Also, a number of push buttons may follow the combo box and clicking on one of them will also emit a
 *  signal. */
class WAddressComboBox: public Wt::WContainerWidget {
public:
    struct ButtonInfo {
        size_t id;                                      // button ID to pass to the activated callback
        Wt::WString label;                              // label to use for button
        Wt::WPushButton *wButton;                       // the button itself
        ButtonInfo(const Wt::WString &label, int id, Wt::WPushButton *button=NULL)
            : id(id), label(label), wButton(button) {}
    };

private:
    Wt::WText *wTitle_;
    Wt::WComboBox *wComboBox_;
    std::vector<ButtonInfo> buttons_;
    bool buttonsCreated_;
    
    typedef Sawyer::Container::Map<Wt::WString, rose_addr_t> Map;
    Map map_;                                           // maps WComboBox labels to their addresses
    Wt::Signal<rose_addr_t, Wt::WString, size_t> clicked_; // emitted when an address is chosen or button clicked

public:
    explicit WAddressComboBox(const Wt::WString &title="", Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), wTitle_(NULL), wComboBox_(NULL), buttonsCreated_(false) {
        init(title);
    }

    /** Adjust push button info.  Buttons should not be inserted or erased after the first call to @ref redraw.
     *
     * @{ */
    const std::vector<ButtonInfo>& buttons() const { return buttons_; }
    std::vector<ButtonInfo>& buttons() { return buttons_; }
    /** @} */

    /** Add another button. This should only be called before the first @ref redraw. Returns the button ID number. */
    size_t addButton(const Wt::WString &label);

    /** Remove all addresses from the combo box. */
    void clear();

    /** Insert one address into the map. Does not adjust the combo box. */
    void insertAddress(rose_addr_t, Wt::WString label="");

    /** Add addresses found by traversing an AST. Added to map; does not adjust the combo box.  If a non-empty @p restrict
     *  interval set is specified then only those constants that exist in @p restrict are inserted.  This can be used, for
     *  instance, to insert only those constants that appear to be mapped addresses. */
    void insertAst(SgNode *ast, const AddressIntervalSet &restrict = AddressIntervalSet());

    /** Add addresses for a basic block.
     *
     *  Adds addresses mentioned in instructions, and the starting address of each interval of the basic block. */
    void insertBasicBlock(const P2::Partitioner&, const P2::BasicBlock::Ptr&);

    /** Rebuild combo box from map. */
    void redraw();

    /** Signal emitted when an address is selected or a button is pressed.
     *
     *  Signal contains the address and its label plus an indication of which button was pressed.  If the signal is generated
     *  by selecting a new address in the combo box then the button index will be -1 (as a size_t) otherwise it will be the
     *  index of the button that was clicked. */
    Wt::Signal<rose_addr_t, Wt::WString, size_t>& clicked() { return clicked_; }

private:
    void init(const Wt::WString &title);

    void handleButtonClicked(size_t buttonIdx);

    void handleAddressActivated(const Wt::WString &label);
};

} // namespace
#endif
