#ifndef bROwSE_WStringDetail_H
#define bROwSE_WStringDetail_H

#include <bROwSE/bROwSE.h>

#include <BinaryString.h>                               // ROSE
#include <Wt/WContainerWidget>

namespace bROwSE {

class WStringDetail: public Wt::WContainerWidget {
    Wt::WPushButton *wButton_;                          // button to go back to the table view
    Wt::WPanel *wValuePanel_;                           // panel holding the string value
    Wt::WText *wStringValue_;                           // holds the value of the string

public:
    explicit WStringDetail(Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), wButton_(NULL), wValuePanel_(NULL), wStringValue_(NULL) {
        init();
    }

    void changeString(const rose::BinaryAnalysis::Strings::EncodedString&);

    Wt::WPushButton* button() { return wButton_; }

private:
    void init();
};

} // namespace
#endif
