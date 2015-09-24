#include <rose.h>
#include <bROwSE/WHexValueEdit.h>

#include <rose_strtoull.h>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>

namespace bROwSE {

void
WHexValueEdit::init(const Wt::WString &label) {
    wLabel_ = new Wt::WText(label, this);
    wValue_ = new Wt::WLineEdit(this);
    wValue_->setTextSize(19);
    wSave_ = new Wt::WPushButton("Save", this);
    wCancel_ = new Wt::WPushButton("Cancel", this);
    new Wt::WText("&nbsp;&nbsp;&nbsp;");
    wError_ = new Wt::WText(this);
    wError_->setStyleClass("text-error");
    wError_->hide();

    wValue_->keyWentUp().connect(boost::bind(&WHexValueEdit::handleKeyPress, this));
    wValue_->enterPressed().connect(boost::bind(&WHexValueEdit::handleSave, this));
    wValue_->escapePressed().connect(boost::bind(&WHexValueEdit::handleCancel, this));
    wSave_->clicked().connect(boost::bind(&WHexValueEdit::handleSave, this));
    wCancel_->clicked().connect(boost::bind(&WHexValueEdit::handleCancel, this));
    
}

void
WHexValueEdit::setError(const Wt::WString &mesg) {
    wError_->setText(mesg);
    wError_->setHidden(mesg.empty());
    wSave_->setEnabled(mesg.empty());
}

void
WHexValueEdit::value(rose_addr_t v, const AddressInterval &limits) {
    value_ = v;
    limits_ = limits;
    if (v) {
        wValue_->setText(StringUtility::addrToString(v));
    } else {
        wValue_->setText("");
    }
    handleKeyPress();
}

void
WHexValueEdit::limits(const AddressInterval &limits) {
    limits_ = limits_;
    handleKeyPress();
}

void
WHexValueEdit::label(const Wt::WString &s) {
    wLabel_->setText(s);
}

Sawyer::Optional<rose_addr_t>
WHexValueEdit::parse() {
    std::string str = StringUtility::trim(wValue_->text().narrow());
    if (str.empty()) {
        wError_->setText("Enter a hexadecimal value.");
        return Sawyer::Nothing();
    }
    if (str.substr(0, 2) == "0x" || str.substr(0, 2) == "0X")
        str = str.substr(2);
    const char *s = str.c_str();
    char *rest = NULL;
    errno = 0;
    rose_addr_t retval = rose_strtoull(s, &rest, 16);
    if (*rest || EINVAL==errno) {
        wError_->setText("Invalid hexadecimal format.");
        return Sawyer::Nothing();
    } else if (errno) {
        wError_->setText("Overflow.");
        return Sawyer::Nothing();
    } else if (limits_.isEmpty()) {
        wError_->setText("INTERNAL ERROR: empty limits!");
        return Sawyer::Nothing();
    } else if (retval < limits_.least()) {
        wError_->setText("Must be at least " + StringUtility::addrToString(limits_.least()) + ".");
        return Sawyer::Nothing();
    } else if (retval > limits_.greatest()) {
        wError_->setText("Must not exceed " + StringUtility::addrToString(limits_.greatest()) + ".");
        return Sawyer::Nothing();
    }
    return retval;
}

// Parse the value and possibly emit a changed signal, but do not yet save the parsed value.
void
WHexValueEdit::handleKeyPress() {
    rose_addr_t va = 0;
    if (parse().assignTo(va)) {
        wSave_->setEnabled(true);
        wError_->hide();
        changed_.emit(va);
    } else {
        wSave_->setEnabled(false);
        wError_->show();
    }
}

void
WHexValueEdit::handleSave() {
    if (parse().assignTo(value_))
        saved_.emit(value_);
}

void
WHexValueEdit::handleCancel() {
    canceled_.emit();
}

} // namespace
