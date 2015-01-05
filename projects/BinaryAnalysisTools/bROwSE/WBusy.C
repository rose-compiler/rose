#include <bROwSE/WBusy.h>

#include <Wt/WBreak>
#include <Wt/WProgressBar>
#include <Wt/WText>

namespace bROwSE {

void
WBusy::init() {
    wTitle_ = new Wt::WText("I'm busy, please wait.", this);
    new Wt::WBreak(this);
    wBar_ = new Wt::WProgressBar(this);
}

// Internal (lock already acquired)
void
WBusy::redraw() {
    // Don't update the progress bar too often (each update is a round trip to the browser)
    static const double minimumUpdateInterval = 1.5;    // seconds
    double now = Sawyer::Message::now();
    if (workStack_.empty() == lastUpdateWasEmpty_) {
        if (now - lastUpdateTime_ < minimumUpdateInterval)
            return;
    }

    if (workStack_.empty()) {
        // This stuff is just placeholders since the widget is not displayed when it isn't busy.
        wTitle_->setText("Busy, please wait.");
        wBar_->setValue(wBar_->maximum());
        wBar_->hide();
    } else {
        wTitle_->setText(workStack_.back().title);
        if (workStack_.back().hi > 0) {
            wBar_->setRange(0, workStack_.back().hi);
            wBar_->setValue(workStack_.back().cur);
            wBar_->show();
        } else {
            wBar_->hide();
        }
    }
    ctx_.application->triggerUpdate();
    lastUpdateTime_ = now;
    lastUpdateWasEmpty_ = workStack_.empty();
}

// This can be called from any thread
void
WBusy::pushWork(const std::string &title, size_t nWork) {
    Wt::WApplication::UpdateLock lock(ctx_.application);
    if (lock) {
        workStack_.push_back(WorkItem(title, nWork));
        if (1==workStack_.size())
            workStarted_.emit();
        redraw();
    }
}

// This can be called from any thread
void
WBusy::replaceWork(const std::string &title, size_t nWork) {
    Wt::WApplication::UpdateLock lock(ctx_.application);
    if (lock) {
        ASSERT_forbid(workStack_.empty());
        workStack_.back() = WorkItem(title, nWork);
        redraw();
    }
}

// This can be called from any thread
void
WBusy::popWork() {
    Wt::WApplication::UpdateLock lock(ctx_.application);
    if (lock) {
        ASSERT_forbid(workStack_.empty());
        workStack_.pop_back();
        if (workStack_.empty())
            workFinished_.emit();
        redraw();
    }
}


// This can be called from any thread
void
WBusy::setValue(size_t n) {
    Wt::WApplication::UpdateLock lock(ctx_.application);
    if (lock) {
        ASSERT_forbid(workStack_.empty());
        workStack_.back().cur = n;
        redraw();
    }
}

// This can be called from any thread
bool
WBusy::isBusy() const {
    Wt::WApplication::UpdateLock lock(ctx_.application);
    return lock ? !workStack_.empty() : false;
}

} // namespace
