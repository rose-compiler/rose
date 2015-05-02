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
WStringDetail::changeString(const StringFinder::String &meta, std::string value) {
    using namespace rose;
    using namespace StringUtility;

    std::string title = numberToString(meta.nCharacters()) + "-character";
    switch (meta.lengthEncoding()) {
        case StringFinder::MAP_TERMINATED:      title += " mmap boundary";        break;
        case StringFinder::NUL_TERMINATED:      title += " NUL-terminated";       break;
        case StringFinder::SEQUENCE_TERMINATED: title += " sequence-terminated";  break;
        case StringFinder::BYTE_LENGTH:         title += " 8-bit length-encoded"; break;
        case StringFinder::LE16_LENGTH:         title += " LE16 length-encoded";  break;
        case StringFinder::BE16_LENGTH:         title += " BE16 length-encoded";  break;
        case StringFinder::LE32_LENGTH:         title += " LE32 length-encoded";  break;
        case StringFinder::BE32_LENGTH:         title += " BE32 length-encoded";  break;
        default: title += " " + stringifyBinaryAnalysisStringFinderLengthEncoding(meta.lengthEncoding()); break;
    }
    switch (meta.characterEncoding()) {
        case StringFinder::UTF8:                title += " UTF-8"; break;
        case StringFinder::UTF16:               title += " UTF-16"; break;
        case StringFinder::UTF32:               title += " UTF-32"; break;
        default: title += " " + stringifyBinaryAnalysisStringFinderCharacterEncoding(meta.characterEncoding()); break;
    }
    title += " string occupying " + plural(meta.nBytes(), "bytes") + " at " + addrToString(meta.address());

    wValuePanel_->setTitle(title);

    value = StringUtility::htmlEscape(value);
    boost::replace_all(value, " ", "&nbsp;");
    boost::replace_all(value, "\n", "<br/>");
    wStringValue_->setTextFormat(Wt::XHTMLText);
    wStringValue_->setText("<pre style=\"white-space: nowrap;\">" + value + "</pre>");
}

} // namespace
