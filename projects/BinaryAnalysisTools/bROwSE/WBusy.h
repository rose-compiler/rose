#ifndef bROwSE_WBusy_H
#define bROwSE_WBusy_H

#include <bROwSE/bROwSE.h>
#include <Wt/WContainerWidget>

namespace bROwSE {

/** Widget to indicate that the system is busy.
 *
 *  Most widget methods can be called from any thread and will push an update to the web browser.
 *
 *  The intention is that whenever some long-running action needs to be performed, the main thread increments the @ref isBusy
 *  condition, then starts the action in a separate thread and returns.  When the other thread finishes it decrements the @ref
 *  isBusy condition.  The expectation is that the application has one global WBusy object which takes over the whole screen
 *  and limits user interaction while it's in the busy state. */
class WBusy: public Wt::WContainerWidget {
    struct WorkItem {
        std::string title;
        size_t lo, hi, cur;
        WorkItem(const std::string &title, size_t hi): title(title), lo(0), hi(hi), cur(0) {}
    };
    typedef std::vector<WorkItem> WorkStack;

    Context &ctx_;
    WorkStack workStack_;
    Wt::WText *wTitle_;
    Wt::WProgressBar *wBar_;
    Wt::Signal<> workStarted_, workFinished_;
    double lastUpdateTime_;                             // time of last call to redraw() that wasn't a no-op
    bool lastUpdateWasEmpty_;                           // was the work list empty at the last redraw time?

public:
    explicit WBusy(Context &ctx, Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), ctx_(ctx), wTitle_(NULL), wBar_(NULL), lastUpdateTime_(0.0), lastUpdateWasEmpty_(true) {
        init();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // The following methods can be called from any thread.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void pushWork(const std::string &title, size_t nWork);
    void popWork();
    void replaceWork(const std::string &title, size_t nWork);

    void setValue(size_t);
    bool isBusy() const;

    Wt::Signal<>& workStarted() { return workStarted_; }
    Wt::Signal<>& workFinished() { return workFinished_; }

private:
    void init();
    void redraw();                                      // lock must already be acquired
};

} // namespace
#endif
