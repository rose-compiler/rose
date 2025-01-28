// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://gitlab.com/charger7534/sawyer.git.




#include <Sawyer/DocumentBaseMarkup.h>
#include <Sawyer/Message.h>
#include <boost/algorithm/string/trim.hpp>

#if 0 // [Robb Matzke 2016-09-18]: see its use below
#include <boost/date_time/gregorian/gregorian.hpp>
#elif defined(BOOST_WINDOWS)
// not written yet
#else // POSIX
#include <time.h>
#endif

namespace Sawyer {
namespace Document {

// Function with one required argument
class Fr1: public Markup::Function {
protected:
    Fr1(const std::string &name): Markup::Function(name) {}
public:
    static Ptr instance(const std::string &name) {
        return Ptr(new Fr1(name))->arg("arg1");
    }
    std::string eval(BaseMarkup::Grammar &/*grammar*/, const std::vector<std::string> &args) override {
        ASSERT_always_require(args.size() == 1);        // so args is always used
        throw Markup::SyntaxError("function \"" + name() + "\" defined in Sawyer::Document::BaseMarkup "
                                  "should have been implemented in a subclass");
    }
};

// Function with two required arguments
class Fr2: public Markup::Function {
protected:
    Fr2(const std::string &name): Markup::Function(name) {}
public:
    static Ptr instance(const std::string &name) {
        return Ptr(new Fr2(name))->arg("arg1")->arg("arg2");
    }
    std::string eval(BaseMarkup::Grammar &/*grammar*/, const std::vector<std::string> &args) override {
        ASSERT_always_require(args.size() == 2);        // so args is always used
        throw Markup::SyntaxError("function \"" + name() + "\" defined in Sawyer::Document::BaseMarkup "
                                  "should have been implemented in a subclass");
    }
};

// Function with one required argument and one optional argument
class Fr1o1: public Markup::Function {
protected:
    Fr1o1(const std::string &name): Markup::Function(name) {}
public:
    static Ptr instance(const std::string &name) {
        return Ptr(new Fr1o1(name))->arg("arg1")->arg("arg2", "");
    }
    std::string eval(BaseMarkup::Grammar &/*grammar*/, const std::vector<std::string> &args) override {
        ASSERT_always_require(args.size() == 2);        // so args is always used
        throw Markup::SyntaxError("function \"" + name() + "\" defined in Sawyer::Document::BaseMarkup "
                                  "should have been implemented in a subclass");
    }
};

SAWYER_EXPORT void
BaseMarkup::init() {
    with(Fr1::instance("b"));                           // @b{bold text}
    with(Fr1::instance("bullet"));                      // @bullet{body}
    with(Fr1::instance("c"));                           // @c{line oriented code}
    with(Fr1o1::instance("link"));                      // @link{url}{title} where title is optional
    with(Fr2::instance("named"));                       // @named{item}{body}
    with(Fr1::instance("numbered"));                    // @numbered{body}
    with(Fr2::instance("section"));                     // @section{title}{body}
    with(Fr1::instance("v"));                           // @v{variable}
}

SAWYER_EXPORT std::string
BaseMarkup::operator()(const std::string &s) {
    return finalizeDocument(Markup::Grammar::operator()(s));
}

SAWYER_EXPORT const std::string&
BaseMarkup::chapterTitleOrDefault() const {
    static const std::string dflt = "Command-line Tools";
    return chapterTitle_.empty() ? dflt : chapterTitle_;
}

SAWYER_EXPORT const std::string&
BaseMarkup::chapterNumberOrDefault() const {
    static const std::string dflt = "1";
    return chapterNumber_.empty() ? dflt : chapterNumber_;
}

SAWYER_EXPORT const std::string&
BaseMarkup::versionStringOrDefault() const {
    static const std::string dflt = "alpha";            // or else the author would have set this property ;-)
    return versionStr_.empty() ? dflt : versionStr_;
}

SAWYER_EXPORT const std::string&
BaseMarkup::versionDateOrDefault() const {
    static std::string dflt;
    if (versionDate_.empty()) {
        if (dflt.empty()) {
            // An important Sawyer customer doesn't want to depend on <boost/date_time/gregorian/gregorian.hpp>'s library,
            // therefore do this the hardway and support only POSIX for now.
#if 0 // [Robb Matzke 2016-09-18]
            dflt = boost::gregorian::to_iso_extended_string(boost::gregorian::day_clock::local_day());
#elif defined(BOOST_WINDOWS)
            TODO("Not implemented yet");
#else
            time_t t = Message::now();
            const struct tm *tm = localtime(&t);
            ASSERT_not_null(tm);
            char buf[128];
            snprintf(buf, sizeof(buf), "%04d-%02d-%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
            dflt = buf;
#endif
        }
        return dflt;
    }
    return versionDate_;
}

SAWYER_EXPORT BaseMarkup&
BaseMarkup::title(const std::string &pageName, const std::string &chapterNumber, const std::string &chapterTitle) {
    pageName_ = pageName;
    chapterNumber_ = chapterNumber;
    chapterTitle_ = chapterTitle;
    return *this;
}

SAWYER_EXPORT BaseMarkup&
BaseMarkup::version(const std::string &versionString, const std::string &versionDate) {
    versionStr_ = versionString;
    versionDate_ = versionDate;
    return *this;
}

// class method
SAWYER_EXPORT bool
BaseMarkup::hasNonSpace(const std::string &s) {
    return !boost::trim_copy(s).empty();
}

// class method
SAWYER_EXPORT std::string
BaseMarkup::makeOneLine(const std::string &s) {
    std::string retval;
    for (char ch: s) {
        if ('\n' != ch && '\r' != ch && '\f' != ch)
            retval += ch;
    }
    return retval;
}

// class method
SAWYER_EXPORT std::string
BaseMarkup::leftJustify(const std::string &s, size_t width) {
    return s.size() >=width ? s : s + std::string(width-s.size(), ' ');
}

// class method
SAWYER_EXPORT std::string
BaseMarkup::escapeSingleQuoted(const std::string &s) {
    std::string retval;
    for (char ch: s) {
        if ('\'' == ch) {
            retval += "\\'";
        } else {
            ASSERT_always_require(isprint(ch));
            retval += ch;
        }
    }
    return retval;
}

} // namespace
} // namespace
