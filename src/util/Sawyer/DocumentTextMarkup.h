// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#ifndef Sawyer_Document_TextMarkup_H
#define Sawyer_Document_TextMarkup_H

#include <Sawyer/DocumentBaseMarkup.h>
#include <Sawyer/Sawyer.h>

namespace Sawyer {
namespace Document {

/** Renders markup as plain text.
 *
 *  This isn't fancy--it's got just enough smarts so a user can read the documentation when a more capable formatter like
 *  perldoc is not available. */
class SAWYER_EXPORT TextMarkup: public BaseMarkup {
    bool doPageHeader_, doPageFooter_;
public:
    TextMarkup()
        : doPageHeader_(true), doPageFooter_(true) {
        init();
    }

    virtual bool emit(const std::string &doc) /*override*/;

    /** Property: Whether to create a page header.
     *
     * @{ */
    bool doingPageHeader() const { return doPageHeader_; }
    TextMarkup& doingPageHeader(bool b) { doPageHeader_ = b; return *this; }
    /** @} */

    /** Property: Whether to create a page footer.
     *
     * @{ */
    bool doingPageFooter() const { return doPageFooter_; }
    TextMarkup& doingPageFooter(bool b) { doPageFooter_ = b; return *this; }
    /** @} */

private:
    void init();
    std::string finalizeDocument(const std::string &s_);
};

} // namespace
} // namespace

#endif
