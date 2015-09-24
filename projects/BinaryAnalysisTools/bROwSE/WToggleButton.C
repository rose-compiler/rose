#include <rose.h>
#include <bROwSE/WToggleButton.h>
#include <Wt/WImage>
#include <Wt/WStackedWidget>

namespace bROwSE {

void
WToggleButton::init(const Wt::WLink &normalImage, const Wt::WLink &pressedImage) {
    wStack_ = new Wt::WStackedWidget(this);

    ASSERT_require(Normal == 0);
    wImageNormal_ = new Wt::WImage(normalImage);
    wImageNormal_->clicked().connect(boost::bind(&WToggleButton::setState, this, Depressed));
    wStack_->addWidget(wImageNormal_);

    ASSERT_require(Depressed == 1);
    wImagePressed_ = new Wt::WImage(pressedImage);
    wImagePressed_->clicked().connect(boost::bind(&WToggleButton::setState, this, Normal));
    wStack_->addWidget(wImagePressed_);

    wStack_->setCurrentIndex(Normal);
}

void
WToggleButton::setState(State state) {
    if (state != state_) {
        state_ = state;
        wImageNormal_->setHidden(state != Normal);
        wImagePressed_->setHidden(state != Depressed);
        stateChanged_.emit(state);
    }
}

} // namespace
