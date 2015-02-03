#include <bROwSE/WFunctionNavigation.h>
#include <Wt/WComboBox>
#include <Wt/WPushButton>

namespace bROwSE {

void
WFunctionNavigation::init() {
    wBackward_ = new Wt::WPushButton("Back", this);
    wBackward_->clicked().connect(this, &WFunctionNavigation::backward);

    wFunctions_ = new Wt::WComboBox(this);
    wFunctions_->resize(Wt::WLength(25, Wt::WLength::FontEm), Wt::WLength());
    wFunctions_->activated().connect(this, &WFunctionNavigation::setFromCombo);

    wForward_ = new Wt::WPushButton("Forward", this);
    wForward_->clicked().connect(this, &WFunctionNavigation::forward);

    adjustEnabledStates();
}

void
WFunctionNavigation::adjustEnabledStates() {
    wBackward_->setEnabled(currentIdx_ > 0);
    wForward_->setEnabled(currentIdx_+1 < functions_.size());
    wFunctions_->setEnabled(!functions_.empty());
}

void
WFunctionNavigation::clear() {
    functions_.clear();
    currentIdx_ = 0;
    wFunctions_->clear();
    adjustEnabledStates();
}

P2::Function::Ptr
WFunctionNavigation::current() const {
    return currentIdx_ >= functions_.size() ? P2::Function::Ptr() : functions_[currentIdx_];
}

P2::Function::Ptr
WFunctionNavigation::setCurrent(size_t idx) {
    ASSERT_require((idx < functions_.size()) || (0==idx && functions_.empty()));
    if (idx != currentIdx_) {
        currentIdx_ = idx;
        int comboIdx = functions_.size() - (currentIdx_+1);
        wFunctions_->setCurrentIndex(comboIdx);
        adjustEnabledStates();
        functionChanged_.emit(functions_[currentIdx_]);
    }
    return functions_[currentIdx_];
}

void
WFunctionNavigation::setFromCombo(int comboIdx) {
    if (!functions_.empty()) {
        size_t idx = functions_.size() - (comboIdx + 1);
        setCurrent(idx);
    }
}

P2::Function::Ptr
WFunctionNavigation::backward() {
    if (currentIdx_ > 0) {
        --currentIdx_;
        adjustEnabledStates();
        int comboIdx = functions_.size() - (currentIdx_+1);
        wFunctions_->setCurrentIndex(comboIdx);
        P2::Function::Ptr function = functions_[currentIdx_];
        functionChanged_.emit(function);
        return function;
    } else {
        return P2::Function::Ptr();
    }
}

P2::Function::Ptr
WFunctionNavigation::forward() {
    if (currentIdx_+1 < functions_.size()) {
        ++currentIdx_;
        adjustEnabledStates();
        int comboIdx = functions_.size() - (currentIdx_+1);
        wFunctions_->setCurrentIndex(comboIdx);
        P2::Function::Ptr function = functions_[currentIdx_];
        functionChanged_.emit(function);
        return function;
    } else {
        return P2::Function::Ptr();
    }
}

void
WFunctionNavigation::push(const P2::Function::Ptr &function) {
    ASSERT_not_null(function);

    // Sometimes we don't have to add anything
    if (currentIdx_<functions_.size() && functions_[currentIdx_]==function)
        return;
    if (currentIdx_+1 < functions_.size() && functions_[currentIdx_+1]==function) {
        ++currentIdx_;
        int comboIdx = functions_.size() - (currentIdx_+1);
        wFunctions_->setCurrentIndex(comboIdx);
        adjustEnabledStates();
        P2::Function::Ptr function = functions_[currentIdx_];
        functionChanged_.emit(function);
        return;
    }

    // Remove functions that are later in the list (or earlier in the combo box)
    while (currentIdx_+1 < functions_.size()) {
        functions_.pop_back();
        wFunctions_->removeItem(0);
    }

    // Add the new function to the end of the list (beginning of combo box) and make it current
    currentIdx_ = functions_.size();
    functions_.push_back(function);
    wFunctions_->insertItem(0, function->printableName());
    wFunctions_->setCurrentIndex(0);
    adjustEnabledStates();
    functionChanged_.emit(function);
}

} // namespace
