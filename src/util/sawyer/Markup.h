// A very siple markup language
#ifndef Sawyer_Markup_H
#define Sawyer_Markup_H

#include <sawyer/Map.h>

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <cstring>
#include <string>
#include <vector>

namespace Sawyer {

/** Simple markup language.
 *
 *  <em>THIS NAMESPACE IS NOT YET READY FOR END USERS</em>  The original design was to provide a simple markup language for
 *  documenting command line switches.
 *
 *  Markup is indicated by a tag followed by zero or more arguments, usually enclosed in curly braces.  For instance, to
 *  indicate that some text is code, use `@``code{strtol(s, &rest, 0)}`.  Many of the commonly used tags have a
 *  one-character version (`@c` is the same as `@``code`).  Other delimiters can be used besides curly
 *  braces, but all arguments for a tag must use the same delimiters.  The opening delimiter determines the closing delimiter
 *  and can be any of these: "{", "(", "[", "<", "'" (single qoute), '"' (double quote), "`" (back tick, closed by single
 *  quote), or white space (closed by the next white space or end of input). White-space delimiters are somewhat special in
 *  that the the closing of an argument also serves as the opening of the next argument (if a next argument is expected);
 *  they're usually used to mark up a single word, such as this sentence, which marks a word as being a meta variable: "the @v
 *  username must start with a letter". */
namespace Markup {

class Input {
    const std::string &content_;
    size_t at_;
public:
    explicit Input(const std::string &s): content_(s), at_(0) {}
    size_t at() const { return at_; }
    void at(size_t offset) { at_ = std::min(offset, content_.size()); }
    bool atEnd() const { return at_ >= content_.size(); }
    char cur() const { return atEnd() ? '\0' : content_[at_]; }
    char next() { return atEnd() ? '\0' : content_[at_++]; }
    void inc() { if (at_ < content_.size()) ++at_; }
    std::string consume(size_t nchars=1);
    std::string consumeSpace();
    std::string consumeUntil(const char*);
    char operator()() const { return cur(); }
    char operator*() const { return cur(); }
    char operator[](size_t offset) const { return at_+offset >= content_.size() ? '\0' : content_[at_+offset]; }
    Input& operator++() { inc(); return *this; }
};

class ExcursionGuard {
    Input &input_;
    size_t at_;
    bool canceled_;
public:
    explicit ExcursionGuard(Input &input): input_(input), at_(input.at()), canceled_(false) {}
    ~ExcursionGuard() { if (!canceled_) input_.at(at_); }
    void cancel() { canceled_ = true; }
};

typedef boost::shared_ptr<class Tag>            TagPtr;
typedef boost::shared_ptr<class NullTag>        NullTagPtr;
typedef boost::shared_ptr<class CommentTag>     CommentTagPtr;
typedef boost::shared_ptr<class SectionTag>     SectionTagPtr;
typedef boost::shared_ptr<class SubSectionTag>  SubSectionTagPtr;
typedef boost::shared_ptr<class BoldTag>        BoldTagPtr;
typedef boost::shared_ptr<class ItalicTag>      ItalicTagPtr;
typedef boost::shared_ptr<class VariableTag>    VariableTagPtr;
typedef boost::shared_ptr<class NameBulletTag>  NameBulletTagPtr;

typedef boost::shared_ptr<class ProseTag>       ProseTagPtr;
typedef boost::shared_ptr<class CodeTag>        CodeTagPtr;
typedef boost::shared_ptr<class TagInstance>    TagInstancePtr;
typedef boost::shared_ptr<class Content>        ContentPtr;
typedef boost::shared_ptr<class Formatter>      FormatterPtr;
typedef boost::shared_ptr<class TextFormatter>  TextFormatterPtr;

typedef std::string Word;
typedef std::vector<Word> Words;
typedef std::vector<ContentPtr> TagArgs;

class Tag: public boost::enable_shared_from_this<Tag> {
    std::string name_;                                  // tag name without the leading "@"
    size_t nArgsDeclared_;                              // number of arguments declared
protected:
    Tag(const std::string &name, size_t nargs=1)
        : name_(name), nArgsDeclared_(nargs) {}
public:
    virtual ~Tag() {}
    TagPtr self() { return boost::dynamic_pointer_cast<Tag>(shared_from_this()); }
    virtual ContentPtr eval(const TagArgs&);
    const std::string &name() const { return name_; }
    size_t nArgsDeclared() const { return nArgsDeclared_; }
};

class NullTag: public Tag {
    std::string text_;
protected:
    explicit NullTag(const std::string &text): Tag("null", 0), text_(text) {}
public:
    static NullTagPtr instance(const std::string &text) { return NullTagPtr(new NullTag(text)); }
    virtual ContentPtr eval(const TagArgs&);
    const std::string& text() const { return text_; }
};

// Comment that will be emitted to the output as a comment
class CommentTag: public Tag {
protected:
    CommentTag(): Tag("comment", 1) {}
public:
    static CommentTagPtr instance() { return CommentTagPtr(new CommentTag); }
    virtual ContentPtr eval(const TagArgs&);
};

// First arg is section name; second is content.
class SectionTag: public Tag {
protected:
    SectionTag(): Tag("section", 2) {}
public:
    static SectionTagPtr instance() { return SectionTagPtr(new SectionTag); }
};

// Subsection. First arg is name, second is content.
class SubSectionTag: public Tag {
protected:
    SubSectionTag(): Tag("subsection", 2) {}
public:
    static SubSectionTagPtr instance() { return SubSectionTagPtr(new SubSectionTag); }
};

// Named bullet. First arg is the name, second is the content.
class NameBulletTag: public Tag {
protected:
    NameBulletTag(): Tag("namebullet", 2) {}
public:
    static NameBulletTagPtr instance() { return NameBulletTagPtr(new NameBulletTag); }
};
    
class BoldTag: public Tag {
protected:
    BoldTag(): Tag("bold", 1) {}
public:
    static BoldTagPtr instance() { return BoldTagPtr(new BoldTag); }
};

class ItalicTag: public Tag {
protected:
    ItalicTag(): Tag("italic", 1) {}
public:
    static ItalicTagPtr instance() { return ItalicTagPtr(new ItalicTag); }
};

class VariableTag: public Tag {
protected:
    VariableTag(): Tag("variable", 1) {}
public:
    static VariableTagPtr instance() { return VariableTagPtr(new VariableTag); }
};

    

class ProseTag: public Tag {
protected:
    ProseTag(): Tag("prose", 1) {}
public:
    static ProseTagPtr instance() { return ProseTagPtr(new ProseTag); }
};

class CodeTag: public Tag {
protected:
    CodeTag(): Tag("code", 1) {}
public:
    static CodeTagPtr instance() { return CodeTagPtr(new CodeTag); }
};


class TagInstance {
private:
    TagPtr tag_;
    TagArgs args_;
protected:
    explicit TagInstance(const TagPtr &tag): tag_(tag) {}
    TagInstance(const TagPtr &tag, const TagArgs &args): tag_(tag), args_(args) {}
public:
    static TagInstancePtr instance(const TagPtr &tag) {
        return TagInstancePtr(new TagInstance(tag));
    }
    static TagInstancePtr instance(const TagPtr &tag, const TagArgs &args) {
        return TagInstancePtr(new TagInstance(tag, args));
    }
    static TagInstancePtr instance(const TagPtr &tag, const ContentPtr &arg0) {
        TagArgs args;
        args.push_back(arg0);
        return instance(tag, args);
    }
    void append(const ContentPtr &arg) { args_.push_back(arg); }
    const TagPtr tag() const { return tag_; }
    const TagArgs& args() const { return args_; }
    ContentPtr eval() const;
    void emit(std::ostream&, const FormatterPtr&) const;
};


// A sequence of tag instances
class Content {
private:
    std::vector<TagInstancePtr> elmts_;
protected:
    Content() {}
public:
    static ContentPtr instance() { return ContentPtr(new Content); }
    void append(const TagInstancePtr &elmt) { elmts_.push_back(elmt); }
    void append(const std::string &s);
    void append(const ContentPtr &other) { elmts_.insert(elmts_.end(), other->elmts_.begin(), other->elmts_.end()); }
    ContentPtr eval() const;
    void emit(std::ostream&, const FormatterPtr&) const;
    void print(std::ostream&) const;
    std::string asText() const;
};


class ParseTerminator {
protected:
    bool isDone_;
public:
    ParseTerminator(): isDone_(false) {}
    virtual ~ParseTerminator() {}
    virtual bool isDone(char) { return false; }
};

class CharsetTerminator: public ParseTerminator {
    std::string stoppers_;
public:
    CharsetTerminator(const std::string &stoppers): stoppers_(stoppers) {}
    virtual bool isDone(char c) /*override*/ {
        return (isDone_ = strchr(stoppers_.c_str(), c)!=NULL);
    }
};

class ParenTerminator: public ParseTerminator {
    char opening_, closing_;
    int depth_;
public:
    ParenTerminator(char opening, char closing): opening_(opening), closing_(closing), depth_(0) {}
    virtual bool isDone(char c) /*override*/ {
        if (c==opening_) {
            ++depth_;
        } else if (c==closing_) {
            isDone_ = depth_ <= 0;
            --depth_;
        }
        return isDone_;
    }
};

        
        




class ParserResult {
    ContentPtr content_;
public:
    ParserResult(const ContentPtr &content): content_(content) {}
    void emit(std::ostream&, const FormatterPtr&);
};

class Parser {
    typedef Container::Map<std::string, TagPtr> SymbolTable;
    SymbolTable symtab_;
public:
    Parser() { init(); }
    Parser& registerTag(const TagPtr&, const std::string &name="");
    ParserResult parse(const std::string&);
private:
    void init();
    static std::string location(const Input&);
    static void warn(const std::string&);
    static char closingFor(char opening);
    std::string parseSymbol(Input&);
    std::string parseNested(Input&, char left, char right);
    ContentPtr parseTagArgument(Input&);
    TagInstancePtr parseTag(Input&);
    ContentPtr parseContent(Input&, ParseTerminator&);
};




class Formatter: public boost::enable_shared_from_this<Formatter> {
protected:
    Formatter() {}
public:
    virtual ~Formatter() {}
    virtual void beginDocument(std::ostream&) = 0;
    virtual void endDocument(std::ostream&) = 0;
    virtual bool beginTag(std::ostream&, const TagPtr&, const TagArgs&) = 0;
    virtual void endTag(std::ostream&, const TagPtr&, const TagArgs&) = 0;
    virtual void text(std::ostream&, const std::string&) = 0;
};

// Formatter used internally by the asText() methods.
class TextFormatter: public Formatter {
protected:
    TextFormatter() {}
public:
    static TextFormatterPtr instance() { return TextFormatterPtr(new TextFormatter); }
    virtual void beginDocument(std::ostream&) /*override*/ {}
    virtual void endDocument(std::ostream&) /*override*/ {}
    virtual bool beginTag(std::ostream&, const TagPtr&, const TagArgs&) /*override*/ { return true; }
    virtual void endTag(std::ostream&, const TagPtr&, const TagArgs&) /*override*/ {}
    virtual void text(std::ostream &stream, const std::string &text) /*override*/ { stream <<text; }
};
    

} // namespace
} // namespace

#endif
