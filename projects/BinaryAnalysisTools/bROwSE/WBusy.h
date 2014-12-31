#ifndef bROwSE_WBusy_H
#define bROwSE_WBusy_H

#include <bROwSE/bROwSE.h>
#include <Wt/WContainerWidget>

namespace bROwSE {

/** Busy cursor.
 *
 *  notifies the busy widget which then signals that all is done. */
class WBusy: public Wt::WContainerWidget {
    bool isBusy_;
    Wt::Signal<> workStarted_, workFinished_;

public:
    explicit WBusy(Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), isBusy_(false) {
        init();
    }

    bool isBusy() const { return isBusy_; }
    void isBusy(bool b);

    Wt::Signal<>& workStarted() { return workStarted_; }
    Wt::Signal<>& workFinished() { return workFinished_; }

private:
    void init();
};

} // namespace
#endif
