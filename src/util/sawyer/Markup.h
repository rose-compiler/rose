// A very siple markup language
#ifndef Sawyer_Markup_H
#define Sawyer_Markup_H

#include <sawyer/Map.h>

#include <cstring>
#include <ostream>
#include <sawyer/Sawyer.h>
#include <sawyer/SharedPointer.h>
#include <string>
#include <vector>

// Any header that #defines words that are this common is just plain stupid!
#if defined(emit)
# ifdef _MSC_VER
#  pragma message("Undefining common words from the global namespace: emit")
# else
#  warning "Undefining common words from the global namespace: emit"
# endif
# undef emit
#endif

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

class SAWYER_EXPORT Input {
    const std::string &content_;
    size_t at_;
public:
    explicit Input(const std::string &s): content_(s), at_(0) {}
    virtual ~Input() {}
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
    virtual ~ExcursionGuard() { if (!canceled_) input_.at(at_); }
    void cancel() { canceled_ = true; }
};

typedef SharedPointer<class Content> ContentPtr;

typedef std::string Word;
typedef std::vector<Word> Words;
typedef std::vector<ContentPtr> TagArgs;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                                      Tags
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Tag types.
 *
 *  Tags come in two flavors:
 *
 *  @li Tags that span some text within a paragraph.
 *  @li Tags that start new paragraphs.
 *
 *  Examples of spanning tags are tags that emphasize text, tags that mark text as being a variable, etc.  Examples of dividing
 *  tags are tags that delineate sections and subsections, tags that deliniate prose versus non-prose, etc. */
enum DivSpan {
    DIVIDING,                                           /**< A tag that divides output into paragraphs. */
    SPANNING,                                           /**< A tag that appears within a paragraph. */
    VERBATIM,                                           /**< Tags are not parsed as tags. */
};

/** Declaration for tag arguments.
 *
 *  This class describes how to parse each argument for a tag instance.
 *
 *  An argument type of DIVIDING means that any tags can appear in the argument, while SPANNING means that only spanning-type
 *  tags can appear in the argument.  If no tags are to be parsed, as in a comment or verbatim argument, then set the argument
 *  type to VERBATIM -- this causes all tags to be parsed as plain textual data. Otherwise, all arguments of a SPANNING tag
 *  must be SPANNING arguments, and arguments of a DIVIDING tag can be either SPANNING or DIVIDING.  For instance, a "Section"
 *  tag that takes two arguments, a section name and the section body, declares the first argument as a SPANNING argument since
 *  the name cannot be multiple paragraphs. */
class ArgumentDeclaration {
    DivSpan type_;                                      /**< What to allow when parsing the argument. */
public:
    explicit ArgumentDeclaration(DivSpan type): type_(type) {}
    DivSpan type() const { return type_; }
};

class SAWYER_EXPORT Tag: public SharedObject, public SharedFromThis<Tag> {
#include <sawyer/WarningsOff.h>
    DivSpan divspan_;                                   // tag type
    std::string name_;                                  // tag name without the leading "@"

    // Number and types of arguments.  An argument that is SPANNING cannot have a DIVIDING tag within it.
    std::vector<ArgumentDeclaration> argDecls_;
#include <sawyer/WarningsRestore.h>

public:
    typedef SharedPointer<Tag> Ptr;
protected:
    Tag(DivSpan divspan, const std::string &name)
        : divspan_(divspan), name_(name) {}
    Tag(DivSpan divspan, const std::string &name, DivSpan arg1)
        : divspan_(divspan), name_(name) {
        argDecls_.push_back(ArgumentDeclaration(arg1));
        checkArgDecls();
    }
    Tag(DivSpan divspan, const std::string &name, DivSpan arg1, DivSpan arg2)
        : divspan_(divspan), name_(name) {
        argDecls_.push_back(ArgumentDeclaration(arg1));
        argDecls_.push_back(ArgumentDeclaration(arg2));
        checkArgDecls();
    }
    Tag(DivSpan divspan, const std::string &name, DivSpan arg1, DivSpan arg2, DivSpan arg3)
        : divspan_(divspan), name_(name) {
        argDecls_.push_back(ArgumentDeclaration(arg1));
        argDecls_.push_back(ArgumentDeclaration(arg2));
        argDecls_.push_back(ArgumentDeclaration(arg3));
        checkArgDecls();
    }
private:
    void checkArgDecls() const;
public:
    virtual ~Tag() {}
    Ptr self() { return sharedFromThis(); }
    virtual ContentPtr eval(const TagArgs&);
    const std::string &name() const { return name_; }
    DivSpan type() const { return divspan_; }
    size_t nArgsDeclared() const { return argDecls_.size(); }
    DivSpan argType(size_t argNum) const { return argDecls_[argNum].type(); }
};

class SAWYER_EXPORT NullTag: public Tag {
#include <sawyer/WarningsOff.h>
    std::string text_;
#include <sawyer/WarningsRestore.h>
protected:
    explicit NullTag(const std::string &text): Tag(DIVIDING, "null"), text_(text) {}
public:
    typedef SharedPointer<NullTag> Ptr;
    static Ptr instance(const std::string &text) { return Ptr(new NullTag(text)); }
    virtual ContentPtr eval(const TagArgs&);
    const std::string& text() const { return text_; }
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Dividing constructs
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Prose.
 *
 *  A dividing tag that takes one argument and renders it as prose. One can assume that the top level of documentation is
 *  surrounded by a \@prose construct since this is the default rendering mode. The \@prose construct can also appear inside a
 *  non-prose construct (i.e., \@code) to temporarily switch from verbatim mode to prose mode. */
class ProseTag: public Tag {
protected:
    ProseTag(): Tag(DIVIDING, "prose", DIVIDING) {}
public:
    typedef SharedPointer<ProseTag> Ptr;
    static Ptr instance() { return Ptr(new ProseTag); }
};

/** Non-prose.
 *
 *  A dividing construct that takes one argument and renders it vertatim, possibly indented.  Leading and trailing linefeeds
 *  are removed.
 *
 * @code
 *  Example
 *
 *  \@nonprose{
 *    class CodeTag: public Tag {
 *    protected:
 *        CodeTag(): Tag("code", 1) {}
 *    public:
 *        static CodeTagPtr instance() {
 *            return CodeTagPtr(new CodeTag);
 *        }
 *    };
 *  }
 * @endcode */
class NonProseTag: public Tag {
protected:
    NonProseTag(): Tag(DIVIDING, "nonprose", VERBATIM) {}
public:
    typedef SharedPointer<NonProseTag> Ptr;
    static Ptr instance() { return Ptr(new NonProseTag); }
};

/** Section, subsection, etc.
 *
 *  A dividing construct that takes two arguments and renders the second argument as a section, subsection, etc. within the
 *  enclosing text. The first argument is the name of the new section.  Some backends limit the permissible nesting level, but
 *  three levels is safe.
 *
 * @code
 *  \@section{Installation}{
 *    \@section{Prerequisites}{
 *      \@section{Boost}{
 *        First install boost.
 *      }
 *      \@section{Aptitude}{
 *        Then install aptitude.
 *      }
 *    }
 *  }
 * @endcode */
class SectionTag: public Tag {
protected:
    SectionTag(): Tag(DIVIDING, "section", SPANNING, DIVIDING) {}
public:
    typedef SharedPointer<SectionTag> Ptr;
    static Ptr instance() { return Ptr(new SectionTag); }
};

// Used internally to group Bullet, Numbered, and Named tags. Any number of arguments, each of which is a list item.
class ListTag: public Tag {
protected:
    ListTag(): Tag(DIVIDING, "list") {}
public:
    typedef SharedPointer<ListTag> Ptr;
    static Ptr instance() { return Ptr(new ListTag); }
};

// Internal: Base class for list items with one or two arguments.
class ListItemTag: public Tag {
protected:
    ListItemTag(const std::string &name, DivSpan arg1): Tag(DIVIDING, name, arg1) {}
    ListItemTag(const std::string &name, DivSpan arg1, DivSpan arg2): Tag(DIVIDING, name, arg1, arg2) {}
public:
    typedef SharedPointer<ListItemTag> Ptr;
};

/** Bullet list item.
 *
 *  A dividing construct that takes one argument that represents a bullet item in a list.  A list consists of all the
 *  neighboring bullet constructs.
 *
 * @code
 *  Some things to remember:
 *  \@bullet{Don't use \@section within a bullet item.}
 *  \@bullet{Blank lines are optional around these since they're division constructs that start their own paragraph.}
 * @endcode */
class BulletTag: public ListItemTag {
protected:
    BulletTag(): ListItemTag("bullet", DIVIDING) {}
public:
    typedef SharedPointer<BulletTag> Ptr;
    static Ptr instance() { return Ptr(new BulletTag); }
};

/** Numbered list item.
 *
 *  A dividing construct that takes one argument that represents a numbered item in a list.  A list consists of all the
 *  neighboring numbered item constructs.
 *
 * @code
 *  There are 10 types of people in the world:
 *  \@numbered{Those who count in binary}
 *  \@numbered{And those who don't}
 * @endcode */
class NumberedTag: public ListItemTag {
protected:
    NumberedTag(): ListItemTag("numbered", DIVIDING) {}
public:
    typedef SharedPointer<NumberedTag> Ptr;
    static Ptr instance() { return Ptr(new NumberedTag); }
};

/** Named list item.
 *
 *  A dividing construct that takes two arguments that represents a named item in a list. The first argument is the name of the
 *  item and the second argument is its text. A list consists of all the neighboring named item constructs.  
 *
 * @code
 *  Common number systems
 *  @named{binary}{Counting with a pair of symbols, usually "0" and "1".}
 *  @named{octal}{Counting using eight symbols, usually the digits "0" through "7".}
 *  @named{decimal}{What most ordinary people use.}
 *  @named{hexadecimal}{Counting using sixteen symbols. Convenient because each symbol represents four bits of information.}
 *  @named{base64}{Counting using 64 symbols. Common way of encoding binary data for transmission over a text-only medium
 *  because each symbol represents exactly six bits and there are approximately 64 characters of the ASCII sequence that have
 *  standard printable glyphs.}
 * @endcode */
class NamedTag: public ListItemTag {
protected:
    NamedTag(): ListItemTag("named", SPANNING, DIVIDING) {}
public:
    typedef SharedPointer<NamedTag> Ptr;
    static Ptr instance() { return Ptr(new NamedTag); }
};
    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Spanning constructs
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Comment.
 *
 *  A spanning construct that takes one argument which will not appear in the output.
 *
 *  @code
 *   The frobnicator does to frob what the communicator does to comm. \@comment{FIXME: We need to expound on this}.
 *  @endcode */
class SAWYER_EXPORT CommentTag: public Tag {
protected:
    CommentTag(): Tag(SPANNING, "comment", VERBATIM) {}
public:
    typedef SharedPointer<CommentTag> Ptr;
    static Ptr instance() { return Ptr(new CommentTag); }
    virtual ContentPtr eval(const TagArgs&);
};

/** Emphasis.
 *
 *  A spanning construct that emphasizes its single argument.
 *
 * @code
 *  One should always write documentation for @em{all} parts of the public interface.
 * @endcode */
class EmphasisTag: public Tag {
protected:
    EmphasisTag(): Tag(SPANNING, "emphasis", SPANNING) {}
public:
    typedef SharedPointer<EmphasisTag> Ptr;
    static Ptr instance() { return Ptr(new EmphasisTag); }
};

/** Variable.
 *
 *  A spanning tag used to represent a variable.
 *
 * @code
 *  This function counts from 1 to @var{N}.
 * @endcode */
class VariableTag: public Tag {
protected:
    VariableTag(): Tag(SPANNING, "variable", SPANNING) {}
public:
    typedef SharedPointer<VariableTag> Ptr;
    static Ptr instance() { return Ptr(new VariableTag); }
};

/** Link.
 *
 *  A spanning construct that takes two arguments and renders them as a link.  The first argument is the link target and the
 *  second argument is the text that will appear in the output.  If the second argument is empty then the link text will be the
 *  same as the link target.
 *
 * @code
 *  See the \@a{http://boost.org/documentation}{Boost} website for documentation.
 * @endcode */
class LinkTag: public Tag {
protected:
    LinkTag(): Tag(SPANNING, "link", SPANNING, SPANNING) {}
public:
    typedef SharedPointer<LinkTag> Ptr;
    static Ptr instance() { return Ptr(new LinkTag); }
};

/** Inline verbatim.
 *
 *  A spanning construct that inserts its argument verbatim.
 *
 *  @code
 *   The emphasis markup takes one argument, like this: \@c{\@em{\@v{text}}}
 *  @endcode */
class VerbatimTag: public Tag {
protected:
    VerbatimTag(): Tag(SPANNING, "verbatim", VERBATIM) {}
public:
    typedef SharedPointer<VerbatimTag> Ptr;
    static Ptr instance() { return Ptr(new VerbatimTag); }
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                                      Formatter
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


enum TextMode { PROSE, NONPROSE };

/** Base class for formatting markup. */
class SAWYER_EXPORT Formatter: public SharedObject, public SharedFromThis<Formatter> {
#include <sawyer/WarningsOff.h>
    std::vector<TextMode> textModeStack_;
#include <sawyer/WarningsRestore.h>
protected:
    Formatter(): textModeStack_(1, PROSE) {}

public:
    virtual ~Formatter() {}

public:
    typedef SharedPointer<Formatter> Ptr;
    void textModePush(TextMode tm) { textModeStack_.push_back(tm); }
    TextMode textMode() const { return textModeStack_.back(); }
    void textModePop() { textModeStack_.pop_back(); }

public: // used internally
    virtual void beginDocument(std::ostream&) = 0;
    virtual void endDocument(std::ostream&) = 0;
    virtual bool beginTag(std::ostream&, const Tag::Ptr&, const TagArgs&) = 0;
    virtual void endTag(std::ostream&, const Tag::Ptr&, const TagArgs&) = 0;
    virtual void text(std::ostream&, const std::string&) = 0;
};

// Formatter used internally by the asText() methods.
class TextFormatter: public Formatter {
protected:
    TextFormatter() {}
public:
    typedef SharedPointer<TextFormatter> Ptr;
    static Ptr instance() { return Ptr(new TextFormatter); }
    virtual void beginDocument(std::ostream&) /*override*/ {}
    virtual void endDocument(std::ostream&) /*override*/ {}
    virtual bool beginTag(std::ostream&, const Tag::Ptr&, const TagArgs&) /*override*/ { return true; }
    virtual void endTag(std::ostream&, const Tag::Ptr&, const TagArgs&) /*override*/ {}
    virtual void text(std::ostream &stream, const std::string &text) /*override*/ { stream <<text; }
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Tag Instance
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SAWYER_EXPORT TagInstance: public SharedObject {
private:
#include <sawyer/WarningsOff.h>
    Tag::Ptr tag_;
    TagArgs args_;
#include <sawyer/WarningsRestore.h>
public:
    typedef SharedPointer<TagInstance> Ptr;
protected:
    explicit TagInstance(const Tag::Ptr &tag): tag_(tag) {}
    TagInstance(const Tag::Ptr &tag, const TagArgs &args): tag_(tag), args_(args) {}
public:
    static Ptr instance(const Tag::Ptr &tag) {
        return Ptr(new TagInstance(tag));
    }
    static Ptr instance(const Tag::Ptr &tag, const TagArgs &args) {
        return Ptr(new TagInstance(tag, args));
    }
    static Ptr instance(const Tag::Ptr &tag, const ContentPtr &arg0) {
        TagArgs args;
        args.push_back(arg0);
        return instance(tag, args);
    }
    void append(const ContentPtr &arg) { args_.push_back(arg); }
    const Tag::Ptr tag() const { return tag_; }
    const TagArgs& args() const { return args_; }
    ContentPtr eval() const;
    void emit(std::ostream&, const Formatter::Ptr&) const;
};

// A sequence of tag instances
class SAWYER_EXPORT Content: public SharedObject {
private:
#include <sawyer/WarningsOff.h>
    std::vector<TagInstance::Ptr> elmts_;
#include <sawyer/WarningsRestore.h>
protected:
    Content() {}
public:
    typedef SharedPointer<Content> Ptr;
    static Ptr instance() { return Ptr(new Content); }
    void append(const TagInstance::Ptr &elmt) { elmts_.push_back(elmt); }
    void append(const std::string &s);
    void append(const Ptr &other) { elmts_.insert(elmts_.end(), other->elmts_.begin(), other->elmts_.end()); }
    Ptr eval() const;
    void emit(std::ostream&, const Formatter::Ptr&) const;
    void print(std::ostream&) const;
    std::string asText() const;
    Ptr fixupLists() const;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                              Parser
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
        
class SAWYER_EXPORT ParserResult {
#include <sawyer/WarningsOff.h>
    Content::Ptr content_;
#include <sawyer/WarningsRestore.h>
public:
    ParserResult() {}
    ParserResult(const Content::Ptr &content): content_(content) {}
    void print(std::ostream&) const;
    void emit(std::ostream&, const Formatter::Ptr&) const;
};

class SAWYER_EXPORT Parser {
#include <sawyer/WarningsOff.h>
    typedef Container::Map<std::string, Tag::Ptr> SymbolTable;
    SymbolTable symtab_;
#include <sawyer/WarningsRestore.h>
public:
    Parser() { init(); }
    Parser& registerTag(const Tag::Ptr&, const std::string &name="");
    ParserResult parse(const std::string&);
private:
    void init();
    static std::string location(const Input&);
    static void warn(const std::string&);
    static char closingFor(char opening);
    std::string parseSymbol(Input&);
    std::string parseWord(Input&);
    std::string parseNested(Input&, char left, char right);
    Content::Ptr parseTagArgument(Input&, DivSpan allowed);
    TagInstance::Ptr parseTag(Input&, DivSpan allowed);
    Content::Ptr parseContent(Input&, ParseTerminator&, DivSpan allowed);
    Content::Ptr fixupLists(const Content::Ptr &in) const;
};

} // namespace
} // namespace

#endif
