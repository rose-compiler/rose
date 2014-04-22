#ifndef Sawyer_MarkupRoff_H
#define Sawyer_MarkupRoff_H

#include <sawyer/Markup.h>

#include <sstream>

namespace Sawyer {
namespace Markup {

typedef boost::shared_ptr<class RoffFormatter>  RoffFormatterPtr;

class RoffFormatter: public Formatter {
    std::string pageName_;                              // command name, usually upper case. E.g., "LS"
    std::string chapterNumber_;                         // usually a number. E.g., "1" for commands
    std::string chapterName_;                           // E.g., "User Commands"
    std::string dateString_;                            // E.g., "April 2010"
    std::string versionString_;                         // E.g., "GNU coreutils 8.5"
    std::vector<TagPtr> tagStack_;                      // tags that are currently active
    TagPtr inArg_;                                      // are we inside an nroff macro argument?
    int convertToUpper_;                                // if non-zero, then convert text to upper case
    std::ostringstream bufferedOutput_;                 // buffer output so we can control blank lines
    bool startOfLine_;                                  // true if we should strip leading white space
protected:
    RoffFormatter(const std::string &pageName, const std::string &chapterNumber, const std::string &chapterName)
        : pageName_(pageName), chapterNumber_(chapterNumber), chapterName_(chapterName), convertToUpper_(0),
          startOfLine_(true) {}
public:
    static RoffFormatterPtr instance(const std::string pageName, const std::string &chapterNumber="1",
                                     const std::string chapterName="User Commands") {
        return RoffFormatterPtr(new RoffFormatter(pageName, chapterNumber, chapterName));
    }

    RoffFormatterPtr title(const std::string &pageName, const std::string &chapterNumber="1",
                           const std::string &chapterName="User Commands", const std::string &versionString="",
                           const std::string &dateString="");
    RoffFormatterPtr version(const std::string &versionString, const std::string &dateString="");

    RoffFormatterPtr pageName(const std::string &s) { pageName_=s; return self(); }
    const std::string& pageName() const { return pageName_; }
    RoffFormatterPtr chapterNumber(const std::string &s) { chapterNumber_=s; return self(); }
    RoffFormatterPtr chapterNumber(int i);
    const std::string& chapterNumber() const { return chapterNumber_; }
    RoffFormatterPtr chapterName(const std::string &s) { chapterName_=s; return self(); }
    const std::string& chapterName() const { return chapterName_; }
    RoffFormatterPtr dateString(const std::string &s) { dateString_=s; return self(); }
    const std::string& dateString() const { return dateString_; }
    RoffFormatterPtr versionString(const std::string &s) { versionString_=s; return self(); }
    const std::string& versionString() const { return versionString_; }

    // internal stuff
    void checkArgs(const TagPtr&, size_t nArgs, const TagArgs&) const;
    void checkIsInside(const TagPtr&, const std::string &required) const;
    void checkNotInside(const TagPtr&, const std::string &prohibited) const;
    void checkNotInArg(const TagPtr&) const;
    void nextLine();
    std::string escapeArg(const std::string&) const;
    std::string escapeText(const std::string&) const;
    RoffFormatterPtr self() { return boost::dynamic_pointer_cast<RoffFormatter>(shared_from_this()); }
    virtual void beginDocument(std::ostream&) /*override*/;
    virtual void endDocument(std::ostream&) /*override*/;
    virtual bool beginTag(std::ostream&, const TagPtr&, const TagArgs&) /*override*/;
    virtual void endTag(std::ostream&, const TagPtr&, const TagArgs&) /*overide*/;
    virtual void text(std::ostream&, const std::string&) /*override*/;
};

} // namespace
} // namespace

#endif
