#include <bROwSE/WToggleButton.h>
#include <Wt/WImage>

namespace bROwSE {

void
WToggleButton::init(const Wt::WLink &link) {
    wImage_ = new Wt::WImage(link, this);
    wImage_->clicked().connect(boost::bind(&WToggleButton::toggle, this));
}

void
WToggleButton::setState(bool b) {
    if (b != state_) {
        state_ = b;
        stateChanged_.emit(b);
    }
}

} // namespace
