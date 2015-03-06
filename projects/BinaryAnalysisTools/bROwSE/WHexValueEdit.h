#ifndef bROwSE_WHexValueEdit_H
#define bROwSE_WHexValueEdit_H

#include <bROwSE/bROwSE.h>
#include <Wt/WContainerWidget>

namespace bROwSE {

class WHexValueEdit: public Wt::WContainerWidget {
    rose_addr_t value_;
    AddressInterval limits_;
    Wt::WText *wLabel_;
    Wt::WLineEdit *wValue_;
    Wt::WText *wError_;
    Wt::WPushButton *wSave_, *wCancel_;
    Wt::Signal<rose_addr_t> saved_;                     // emitted when the "Save" button is clicked
    Wt::Signal<> canceled_;                             // emitted when the "Cancel" button is clicked
    Wt::Signal<rose_addr_t> changed_;                   // emitted when value changes and is valid

public:
    explicit WHexValueEdit(const Wt::WString &label="", Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), value_(0), wLabel_(NULL), wValue_(NULL), wError_(NULL), wSave_(NULL), wCancel_(NULL) {
        init(label);
    }

    /** Cause an error to be shown. The error is displayed until another key is pressed. */
    void setError(const Wt::WString&);

    /** Set initial value and limits. */
    void value(rose_addr_t, const AddressInterval &limits);

    /** Returns initial or saved value. */
    rose_addr_t value() const { return value_; }

    const AddressInterval& limits() const { return limits_; }
    void limits(const AddressInterval&);

    void label(const Wt::WString &s);

    Wt::Signal<rose_addr_t>& saved() { return saved_; }
    Wt::Signal<>& canceled() { return canceled_; }
    Wt::Signal<rose_addr_t>& changed() { return changed_; }

private:
    void init(const Wt::WString&);
    void handleSave();
    void handleCancel();
    void handleKeyPress();
    Sawyer::Optional<rose_addr_t> parse();
};

} // namespace
#endif
