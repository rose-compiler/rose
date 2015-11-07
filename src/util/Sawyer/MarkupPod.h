// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#ifndef Sawyer_Markup_MarkupPod_H
#define Sawyer_Markup_MarkupPod_H

#include <Sawyer/Markup.h>
#include <Sawyer/Sawyer.h>
#include <string>
#include <vector>

namespace Sawyer {
namespace Markup {

class SAWYER_EXPORT PodFormatter: public Formatter {
#include <Sawyer/WarningsOff.h>
    std::string pageName_;                              // command name, usually upper case. E.g., "LS"
    std::string chapterNumber_;                         // usually a number. E.g., "1" for commands
    std::string chapterName_;                           // E.g., "User Commands"
    std::string dateString_;                            // E.g., "April 2010"
    std::string versionString_;                         // E.g., "GNU coreutils 8.5"
    std::vector<Tag::Ptr> tagStack_;                    // tags currently processed
    bool atBeginningOfLine_;                            // true when output cursor is at the beginning of a line
#include <Sawyer/WarningsRestore.h>
protected:
    PodFormatter(): pageName_("Command"), chapterNumber_("1"), chapterName_("User Commands"), atBeginningOfLine_(true) {}

public:
    typedef SharedPointer<PodFormatter> Ptr;

    static Ptr instance() { return Ptr(new PodFormatter); }

    Ptr title(const std::string &pageName, const std::string &chapterNumber="1",
              const std::string &chapterName="User Commands");
    Ptr version(const std::string &versionString, const std::string &dateString="");

    Ptr pageName(const std::string &s) { pageName_=s; return self(); }
    const std::string& pageName() const { return pageName_; }
    Ptr chapterNumber(const std::string &s) { chapterNumber_=s; return self(); }
    Ptr chapterNumber(int i);
    const std::string& chapterNumber() const { return chapterNumber_; }
    Ptr chapterName(const std::string &s) { chapterName_=s; return self(); }
    const std::string& chapterName() const { return chapterName_; }
    Ptr dateString(const std::string &s) { dateString_=s; return self(); }
    const std::string& dateString() const { return dateString_; }
    Ptr versionString(const std::string &s) { versionString_=s; return self(); }
    const std::string& versionString() const { return versionString_; }

    std::string toNroff(const ParserResult&);
    void emit(const ParserResult&);

protected:
    Ptr self() { return sharedFromThis().dynamicCast<PodFormatter>(); }
    size_t nested() const;
    void checkArgs(const Tag::Ptr &tag, size_t nArgs, const TagArgs &args) const;
    std::string escape(const std::string&) const;

public: // used internally
    virtual void beginDocument(std::ostream&) /*override*/;
    virtual void endDocument(std::ostream&) /*override*/;
    virtual bool beginTag(std::ostream&, const Tag::Ptr&, const TagArgs&) /*override*/;
    virtual void endTag(std::ostream&, const Tag::Ptr&, const TagArgs&) /*override*/;
    virtual void text(std::ostream&, const std::string &text) /*override*/;
};


} // namespace
} // namespace

#endif
