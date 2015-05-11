#include <bROwSE/WStringDetail.h>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <stringify.h>                                  // ROSE
#include <Wt/WBreak>
#include <Wt/WPanel>
#include <Wt/WPushButton>
#include <Wt/WText>

using namespace rose::BinaryAnalysis;

namespace bROwSE {

void
WStringDetail::init() {
    wButton_ = new Wt::WPushButton("Back to table", this);
    new Wt::WBreak(this);

    wValuePanel_ = new Wt::WPanel(this);
    wValuePanel_->setTitle("String value");

    wStringValue_ = new Wt::WText;
    wValuePanel_->setCentralWidget(wStringValue_);
}

void
WStringDetail::changeString(const Strings::EncodedString &string) {
    using namespace rose;
    using namespace StringUtility;

    std::string title = numberToString(string.length()) + "-character " + string.encoder()->name();
    title += " string occupying " + plural(string.size(), "bytes") + " at " + addrToString(string.address());

    wValuePanel_->setTitle(title);

    std::string value = StringUtility::htmlEscape(string.narrow());
    boost::replace_all(value, " ", "&nbsp;");
    boost::replace_all(value, "\n", "<br/>");
    wStringValue_->setTextFormat(Wt::XHTMLText);
    wStringValue_->setText("<pre style=\"white-space: nowrap;\">" + value + "</pre>");
}

} // namespace
