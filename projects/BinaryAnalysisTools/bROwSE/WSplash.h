#ifndef bROwSE_WSplash_H
#define bROwSE_WSplash_H

#include <bROwSE/bROwSE.h>
#include <Wt/WContainerWidget>

namespace bROwSE {

/** Splash screen, licence agreement, etc. */
class WSplash: public Wt::WContainerWidget {
    Wt::Signal<> clicked_;

public:
    explicit WSplash(Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent) {
        init();
    }

    Wt::Signal<>& clicked() { return clicked_; }

private:
    void init();
    void handleContinue();
};

} // namespace
#endif
