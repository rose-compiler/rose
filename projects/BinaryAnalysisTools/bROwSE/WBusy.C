#include <bROwSE/WBusy.h>

namespace bROwSE {

void
WBusy::init() {
    new Wt::WText("I'm busy, please wait.", this);
}

void
WBusy::isBusy(bool b) {
    if (b != isBusy_) {
        isBusy_ = b;
        if (b) {
            workStarted_.emit();
        } else {
            workFinished_.emit();
        }
    }
}

} // namespace
