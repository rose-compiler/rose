#ifndef bROwSE_WToggleButton_H
#define bROwSE_WToggleButton_H

#include <bROwSE/bROwSE.h>
#include <Wt/WContainerWidget>

namespace bROwSE {

class WToggleButton: public Wt::WContainerWidget {
    Wt::WImage *wImage_;
    bool state_;
    Wt::Signal<bool> stateChanged_;
public:
    explicit WToggleButton(const Wt::WLink &link, Wt::WContainerWidget *parent = NULL)
        : Wt::WContainerWidget(parent), wImage_(NULL), state_(false) {
        init(link);
    }

    bool state() const { return state_; }
    void setState(bool b);
    void toggle() { setState(!state()); }

    Wt::Signal<bool>& stateChanged() { return stateChanged_; }

private:
    void init(const Wt::WLink&);
};


} // namespace
#endif
