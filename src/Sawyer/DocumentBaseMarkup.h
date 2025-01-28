// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://gitlab.com/charger7534/sawyer.git.




#ifndef Sawyer_Document_BaseMarkup_H
#define Sawyer_Document_BaseMarkup_H

#include <Sawyer/DocumentMarkup.h>
#include <Sawyer/Sawyer.h>

namespace Sawyer {
namespace Document {

/** Base class for various documentation markup systems. */
class SAWYER_EXPORT BaseMarkup: public Markup::Grammar {
    std::string pageName_;                              // name of document page, usually one word
    std::string chapterNumber_;                         // usually a number like "1", but not necessarily
    std::string chapterTitle_;                          // like "Command-line tools"
    std::string versionStr_;                            // version string
    std::string versionDate_;                           // date string

protected:
    BaseMarkup() {
        init();
    }

public:
    /** Property: Page name.
     *
     *  The page name is normally a single word, like "emacs".
     *
     * @{ */
    const std::string& pageName() const { return pageName_; }
    BaseMarkup& pageName(const std::string &s) { pageName_ = s; return *this; }
    /** @} */

    /** Property: Chapter number.
     *
     *  The chapter "number", which could also be a non-numeric string.  Man pages have standard numbers such as "1" for
     *  command-line tools.  The @ref chapterNumberOrDefault returns either the value previously set, or some default.
     *
     * @{ */
    const std::string& chapterNumber() const { return chapterNumber_; }
    BaseMarkup& chapterNumber(const std::string &s) { chapterNumber_ = s; return *this; }
    const std::string& chapterNumberOrDefault() const;
    /** @} */

    /** Property: Chapter title.
     *
     *  The chapter name should be something short enough to fit centered at the top of the man page, sharing the first line
     *  with the page name. It should have Title Capitalization.  The @ref chapterTitleOrDefault returns either the value
     *  previously set, or some default.
     *
     * @{ */
    const std::string& chapterTitle() const { return chapterTitle_; }
    BaseMarkup& chapterTitle(const std::string &s) { chapterTitle_ = s; return *this; }
    const std::string& chapterTitleOrDefault() const;
    /** @} */

    /** Property: Version string.
     *
     *  The version string usually appears on the left side of the bottom line of the man page, sharing that area also with the
     *  version date and page name.  The @ref versionStringOrDefault returns either the value previously set, or some default.
     *
     * @{ */
    const std::string& versionString() const { return versionStr_; }
    BaseMarkup& versionString(const std::string &s) { versionStr_ = s; return *this; }
    const std::string& versionStringOrDefault() const;
    /** @} */

    /** Property: Version date.
     *
     *  The version date usually appears centered on the last line of output, sharing that line with the version string and
     *  page name.  The @ref versionDateOrDefault returns either the value previously set, or some default.
     *
     * @{ */
    const std::string& versionDate() const { return versionDate_; }
    BaseMarkup& versionDate(const std::string &s) { versionDate_ = s; return *this; }
    const std::string& versionDateOrDefault() const;
    /** @} */

    /** Set title properties.
     *
     *  This functions is a shortcut for setting the @ref pageName, @ref chapterNumber, and @ref chapterTitle properties. */
    BaseMarkup& title(const std::string &pageName, const std::string &chapterNumber, const std::string &chapterTitle);

    /** Set version properties.
     *
     *  This function is a shortcut for setting the @ref versionString and @ref versionDate properties. */
    BaseMarkup& version(const std::string &versionString, const std::string &versionDate);

    /** Parse input to generate POD. */
    virtual std::string operator()(const std::string&) override;

    /** Emit documentation to output */
    virtual bool emit(const std::string &doc) = 0;

    /** True if this string contains any non-blank characters. */
    static bool hasNonSpace(const std::string&);

    /** Remove linefeeds. */
    static std::string makeOneLine(const std::string&);

    /** Left justify a string in a field of width N (or more). String should not contain linefeeds. */
    static std::string leftJustify(const std::string&, size_t width);

    /** Escape single quoted string content. */
    static std::string escapeSingleQuoted(const std::string&);

protected:
    // Last thing called before the rendered document is returend
    virtual std::string finalizeDocument(const std::string &s) { return s; }

private:
    void init();
};

} // namespace
} // namespace

#endif
