#ifndef bROwSE_WToggleButton_H
#define bROwSE_WToggleButton_H

#include <bROwSE/bROwSE.h>
#include <Wt/WContainerWidget>

namespace bROwSE {

class WToggleButton: public Wt::WContainerWidget {
public:
    enum State { Normal=0, Depressed=1 };

private:
    Wt::WStackedWidget *wStack_;
    Wt::WImage *wImageNormal_, *wImagePressed_;
    State state_;
    Wt::Signal<State> stateChanged_;
public:
    explicit WToggleButton(const Wt::WLink &normalImage, const Wt::WLink &pressedImage, Wt::WContainerWidget *parent = NULL)
        : Wt::WContainerWidget(parent), wStack_(NULL), wImageNormal_(NULL), wImagePressed_(NULL), state_(Normal) {
        init(normalImage, pressedImage);
    }

    State state() const { return state_; }
    void setState(State);

    Wt::Signal<State>& stateChanged() { return stateChanged_; }

private:
    void init(const Wt::WLink &normalImage, const Wt::WLink &pressedImage);
};


} // namespace
#endif
