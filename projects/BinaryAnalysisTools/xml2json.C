static const char *purpose = "convert XML to JSON";
static const char *description =
    "Converts XML to JSON in the fastest, most memory-efficient way possible, handling only that XML which is produced "
    "by the ROSE binary analysis state serialization.";

#include <rose.h>                                       // Must be first ROSE include file
#include <CommandLine.h>
#include <Diagnostics.h>                                // ROSE

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/lexical_cast.hpp>
#include <fcntl.h>
#include <Sawyer/LineVector.h>
#include <Sawyer/ProgressBar.h>

#if 0 // [Robb P Matzke 2017-06-22]: define on command-line if you need this. Disabling saves ~2% elapsed time.
#define XML2JSON_SUPPORT_CHECK                          // if defined, then the --check switch is supported.
#endif

using namespace Sawyer::Message::Common;

Sawyer::Message::Facility mlog;

struct Settings {
    std::string xmlFileName;                            // input file name
    std::string jsonFileName;                           // output file name
    bool check;                                         // perform extra XML input checking
    std::vector<std::string> deletions;                 // paths of subtrees to delete
    std::vector<std::string> echoes;                    // paths that should be echoed for debugging
    std::vector<std::string> renames;                   // paths whose final matching element should be renamed

    Settings(): check(false) {}
};

Settings
parseCommandLine(int argc, char *argv[]) {
    using namespace Sawyer::CommandLine;
    Settings retval;

    Parser p = Rose::CommandLine::createEmptyParser(purpose, description);
    p.with(Rose::CommandLine::genericSwitches());
    p.doc("Synopsis", "@prop{programName} [@v{switches}] @v{xml_input_file} @v{json_output_file}");
    p.errorStream(mlog[FATAL]);
    SwitchGroup tool("Tool-specific switches");
    tool.name("tool");

#ifdef XML2JSON_SUPPORT_CHECK
    Rose::CommandLine::insertBooleanSwitch(tool, "check", retval.check, "Perform extra input checking.");
#endif

    tool.insert(Switch("delete")
                .argument("path", anyParser(retval.deletions))
                .whichValue(SAVE_ALL)
                .doc("Path of subtrees to delete. See Path Specification for details. This switch may appear more than once."));

    tool.insert(Switch("echo")
                .argument("path[:output]", anyParser(retval.echoes))
                .whichValue(SAVE_ALL)
                .doc("Emit @v{output} [or the rule name] to standard output whenever @v{path} matches. See Path Specification "
                     "for details. This switch may appear more than once."));

    tool.insert(Switch("rename")
                .argument("path:name", anyParser(retval.renames))
                .whichValue(SAVE_ALL)
                .doc("For each XML input path that matches @v{path}, the XML tag is renamed to @v{name} before being "
                     "emitted as JSON.  If an XML path is matched by both @s{delete} and @s{rename} patterns, the deletion "
                     "takes precedence. If an XML path is matched by multiple @s{rename} patterns with conflicting "
                     "new names, a warning is reported and the first rename is used. See Path Specification for "
                     "details."));

    p.with(tool);
    p.doc("Path Specifications",
          "Operations such as echo, delete, and rename are triggered by matching a path specification against an actual "
          "tree path in the XML input. The path specification is written as one or more dot-separated components with "
          "each component matching one level of the XML tree starting at the root. The special component named \"*\" "
          "is a wildcard that matches zero or more levels of the XML tree.\n\n"

          "Path specifications components match XML tags, properties, or text. When matching properties the names should "
          "be prefixed with an \"@\" as they normally appear in the JSON output. When matching text that occurs between "
          "XML tags (such as \"<tag>this is text</tag>\") the component should be named \"#text\"."

          "@named{Example 1:}{The path specification \"foo\" matches the XML tag \"foo\" if it appears at the root of "
          "the document.}"

          "@named{Example 2:}{The spec \"foo.bar\" matches \"bar\" only when it appears within \"foo\" and \"foo\" is at "
          "the root.}"

          "@named{Example 3:}{The spec \"*.foo.*.bar\" matches \"bar\" anywhere as long as it's a descendant (possibly "
          "an immediate child) of \"foo\" and \"foo\" can appear at any level of the tree.}");


    std::vector<std::string> args = p.parse(argc, argv).apply().unreachedArgs();
    if (args.size() != 2) {
        mlog[FATAL] <<"incorrect usage; see --help\n";
        exit(1);
    }

    retval.xmlFileName = args[0];
    retval.jsonFileName = args[1];
    return retval;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace XmlLexer {

enum TokenType {
    TOK_EOF,                                            // end of file
    TOK_LT,                                             // '<'
    TOK_GT,                                             // '>'
    TOK_QMARK,                                          // '?'
    TOK_EQ,                                             // '='
    TOK_BANG,                                           // '!'
    TOK_SLASH,                                          // '/'
    TOK_SYMBOL,                                         // a word, like "xml", "boost_serialization", "progressTotal", etc.
    TOK_STRING,                                         // string literal, like '"-1"' and '"foo bar"'
    TOK_TEXT,                                           // the "abc" and "def" in "<a href="foo">abc<img src="xyz">def</a>"
    TOK_OTHER                                           // anything else
};

namespace PossibleText {
enum Boolean { NO=0, YES=1 };
} // namespace

// An std::string-like object that points directly into a shared storage area (such as a memory-mapped file).
class SharedString {
    const char *s_;
    size_t size_;
public:
    SharedString(): s_(""), size_(0) {}
    explicit SharedString(const char *s): s_(s), size_(strlen(s)) {}
    SharedString(const char *s, size_t size): s_(s), size_(size) {}
    std::string toString() const { return std::string(s_, size_); }
    const char* data() const { return s_; }
    size_t size() const { return size_; }
    bool operator==(const SharedString &other) const {
        return size_ == other.size_ && 0 == memcmp(s_, other.s_, size_);
    }
};

// Warning: This must be defined in such a way that bitblit will work (e.g., no virtual functions).
struct Token {
    friend class TokenStream;

    TokenType type_;                                    // type of this token
    size_t begin_;                                      // offset in stream to start of token
    size_t end_;                                        // offset in stream to one-past-end of token

public:
    Token(): type_(TOK_EOF), begin_(0), end_(0) {}      // for std::vector, otherwise not used

    Token(TokenType type, size_t begin, size_t end)
        : type_(type), begin_(begin), end_(end) {
        ASSERT_require(end >= begin);
    }

    TokenType type() const { return type_; }
    size_t size() const { return end_ - begin_; }
    size_t offset() const { return begin_; }
};

class TokenStream {
    std::string fileName_;                              // name of source file
    Sawyer::Container::LineVector content_;             // content of source file
    size_t at_;                                         // cursor position in buffer
    enum { MAX_LOOKAHEAD = 3 };
    Token tokens_[MAX_LOOKAHEAD];                       // current token and a few lookahead tokens
    size_t nTokens_;                                    // number of tokens in the tokens_ array

public:
    // Create a token stream that will process the specified file. The file is memory mapped rather than read.
    explicit TokenStream(const std::string &fileName)
        : fileName_(fileName), content_(fileName), at_(0), nTokens_(0) {}

    // Create a token stream using the specified content buffer. The file name is only used for diagnostics.
    TokenStream(const std::string &fileName, const Sawyer::Container::Buffer<size_t, char>::Ptr &buffer)
        : fileName_(fileName), content_(buffer), at_(0), nTokens_(0) {}

    const std::string& fileName() const {
        return fileName_;
    }

    // Total size of the input stream
    size_t fileSize() const {
        return content_.nCharacters();
    }

    // Return the specified token offset from the current token. Only non-negative offsets are allowed.
    const Token& operator[](size_t lookahead) {
        return get(lookahead, PossibleText::NO);
    }

    // Similar to operator[], except allows the specified token to be TOK_TEXT
    const Token& get(size_t lookahead, PossibleText::Boolean possibleText) {
        // If we're allowing the token to be TEXT, then we should undo things if we've already parsed tokens as non-TEXT.
        if (possibleText && nTokens_ > lookahead) {
            at_ = tokens_[lookahead].begin_;
            nTokens_ = lookahead;
        }

        while (lookahead >= nTokens_) {
            makeNextToken(possibleText && nTokens_==lookahead ? PossibleText::YES : PossibleText::NO);
            ASSERT_forbid(nTokens_ == 0);
            if (tokens_[nTokens_-1].type() == TOK_EOF)
                return tokens_[nTokens_-1];
        }
        return tokens_[lookahead];
    }

    // Consume some tokens, advancing the current token pointer to a later token. The consumed tokens are forever lost.
    void consume(size_t n = 1) {
        if (n >= nTokens_) {
            nTokens_ = 0;
        } else {
            nTokens_ -= n;
            memmove(tokens_, tokens_+n, nTokens_*sizeof(tokens_[0]));
        }
    }

    // Return token lexeme as a shared string. I.e., an object that points directly into the file.
    SharedString lexeme(const Token &t) const {
        return SharedString(content_.characters(t.begin_), t.end_ - t.begin_);
    }

    // Returns the line of source in which the token appears, including line termination if present.
    std::string line(const Token &t) const {
        if (t.type() == TOK_EOF)
            return "";
        size_t lineIdx = content_.lineIndex(t.begin_);
        const char *s = content_.lineChars(lineIdx);
        size_t n = content_.nCharacters(lineIdx);
        return std::string(s, n);
    }

    // Determines if the current token matches the specified string. This is the preferred, fast way of testing a token -- use
    // matches(token,"foo") instead of lexeme(token) == "foo" -- because it never actually needs to create a string.
    bool matches(const Token &t, const char *s) const {
        size_t n1 = t.end_ - t.begin_;
        size_t n2 = strlen(s);
        if (n1 != n2)
            return false;
        const char *lexeme = content_.characters(t.begin_);
        return 0 == strncmp(s, lexeme, n1);
    }

    // Emit an error message that shows the entire line containing the token, and a pointer to the token within the line.
    void emit(const std::string &fileName, const Token &t, const std::string &message) const {
        std::pair<size_t, size_t> loc = content_.location(t.begin_);
        mlog[ERROR] <<fileName <<":" <<(loc.first+1) <<":" <<(loc.second+1) <<": " <<message <<"\n";
        const char *line = content_.lineChars(loc.first);
        if (line) {
            std::string str(line, content_.nCharacters(loc.first));
            boost::trim_right(str);
            mlog[ERROR] <<"       |" <<str <<"|\n";
            mlog[ERROR] <<"        " <<std::string(loc.second, ' ') <<"^\n";
        }
    }

    // Return the line number and column for the start of the specified token. The line number and column number are 0-origin.
    std::pair<size_t, size_t> location(const Token &t) const {
        return content_.location(t.begin_);
    }

private:
    // Scans over a quoted string literal
    void scanString() {
        int q = content_.character(at_);
        ASSERT_require('"' == q);
        int c = content_.character(++at_);
        while (EOF != c && c != q)
            c = content_.character(++at_);
        ++at_;                                          // skip closing quote
    }

    // Scans over a symbol
    void scanSymbol() {
        int c = content_.character(at_);
        ASSERT_require(isalpha(c));
        while (EOF != c && (isalnum(c) || '_' == c))
            c = content_.character(++at_);
    }

    // Scan over text between XML tags
    void scanText() {
        int c = content_.character(at_);
        while (EOF != c && '<' != c)
            c = content_.character(++at_);
    }

    // Add the next token to the end of the tokens_ vector
    void makeNextToken(PossibleText::Boolean possibleText) {
        if (nTokens_>0 && tokens_[nTokens_-1].type() == TOK_EOF)
            return;
        ASSERT_require(nTokens_ < MAX_LOOKAHEAD);

        // Skip white space
        int c = content_.character(at_);
        while (isspace(c))
            c = content_.character(++at_);

        // End of file?
        if (EOF == c) {
            tokens_[nTokens_++] = Token(TOK_EOF, at_, at_);
            return;
        }

        if (possibleText) {
            if ('<' == c) {
                tokens_[nTokens_++] = Token(TOK_LT, at_, at_+1);
                ++at_;
            } else {
                tokens_[nTokens_] = Token(TOK_TEXT, at_, at_);
                scanText();
                tokens_[nTokens_++].end_ = at_;
            }
        } else {
            switch (int c = content_.character(at_)) {
                case EOF:
                    tokens_[nTokens_++] = Token(TOK_EOF, at_, at_);
                    break;
                case '<':
                    tokens_[nTokens_++] = Token(TOK_LT, at_, at_+1);
                    ++at_;
                    break;
                case '>':
                    tokens_[nTokens_++] = Token(TOK_GT, at_, at_+1);
                    ++at_;
                    break;
                case '=':
                    tokens_[nTokens_++] = Token(TOK_EQ, at_, at_+1);
                    ++at_;
                    break;
                case '?':
                    tokens_[nTokens_++] = Token(TOK_QMARK, at_, at_+1);
                    ++at_;
                    break;
                case TOK_EQ:
                    tokens_[nTokens_++] = Token(TOK_EQ, at_, at_+1);
                    ++at_;
                    break;
                case '!':
                    tokens_[nTokens_++] = Token(TOK_BANG, at_, at_+1);
                    ++at_;
                    break;
                case '/':
                    tokens_[nTokens_++] = Token(TOK_SLASH, at_, at_+1);
                    ++at_;
                    break;
                case '"': // exclude the delimiting quotes
                    tokens_[nTokens_] = Token(TOK_STRING, at_+1, at_+1);
                    scanString();
                    tokens_[nTokens_++].end_ = at_-1;
                    break;
                default:
                    if (isalpha(c)) {
                        tokens_[nTokens_] = Token(TOK_SYMBOL, at_, at_);
                        scanSymbol();
                        tokens_[nTokens_++].end_ = at_;
                    } else {
                        tokens_[nTokens_++] = Token(TOK_OTHER, at_, at_+1);
                        ++at_;
                    }
                    break;
            }
        }
    }
};

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 0 // [Robb P Matzke 2017-06-22]
std::ostream& operator<<(std::ostream &out, const XmlLexer::SharedString &s) {
    out <<s.toString();
    return out;
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Escape {
enum Boolean { NO=0, YES=1 };
} // namespace

class JsonOutput {
    std::string fileName_;
    boost::iostreams::mapped_file file_;
    int fd_;                                            // file descriptor
    char *data_;                                        // allocated memory for the file contents
    size_t at_;                                         // location of cursor, adjusted after each write
    size_t highwater_;                                  // one past highest address ever written
    size_t end_;                                        // total size of allocated memory

public:
    JsonOutput(const std::string fileName, size_t maxSize)
        : fileName_(fileName), fd_(-1), data_(NULL), at_(0), highwater_(0), end_(0) {
        ASSERT_require(maxSize > 0);
        fd_ = open(fileName.c_str(), O_RDWR|O_CREAT|O_TRUNC, 0666);
        if (fd_ < 0 ||
            lseek(fd_, maxSize-1, SEEK_SET) == off_t(-1) ||
            ::write(fd_, "\0", 1) != 1)
            throw std::runtime_error(strerror(errno) + (": " + fileName));

        file_.open(fileName, boost::iostreams::mapped_file::readwrite, maxSize);
        data_ = file_.data();
        end_ = maxSize;
    }

    ~JsonOutput() {
        if (fd_ != -1) {
            file_.close();
            if (-1 == ftruncate(fd_, highwater_))
                mlog[ERROR] <<"ftruncate failed\n";
            close(fd_);
        }
    }

    // Current position, adjusted after each write
    size_t curpos() const {
        return at_;
    }

    // Offset to EOF marker; one past last byte written
    size_t eof() const {
        return highwater_;
    }

    void truncate(size_t n) {
        if (at_ > n)
            at_ = n;
        if (highwater_ > n)
            highwater_ = n;
    }

    // Writes N bytes of DATA into the output at the specified POSITION without updating the current position.
    JsonOutput& write(const char *data, size_t position, size_t n, Escape::Boolean doEscape) {
        ASSERT_require(position + n <= end_);
        if (doEscape) {
            at_ = position;
            for (size_t i=0; i<n; ++i) {
                switch (data[i]) {
                    case '"':
                    case '\\':
                        data_[at_++] = '\\';
                        data_[at_++] = data[i];
                        break;
                    case '&':
                        if (i+5 < n && 0 == memcmp(data+i+1, "quot;", 5)) {
                            data_[at_++] = '\\';
                            data_[at_++] = '"';
                            i += 5;
                        } else if (i+5 < n && 0 == memcmp(data+i+1, "apos;", 5)) {
                            data_[at_++] = '\'';
                            i += 5;
                        } else if (i+3 < n && 0 == memcmp(data+i+1, "lt;", 3)) {
                            data_[at_++] = '<';
                            i += 3;
                        } else if (i+3 < n && 0 == memcmp(data+i+1, "gt;", 3)) {
                            data_[at_++] = '>';
                            i += 3;
                        } else if (i+4 < n && 0 == memcmp(data+i+1, "amp;", 4)) {
                            data_[at_++] = '&';
                            i += 4;
                        } else {
                            data_[at_++] = '&';
                        }
                        break;
                    default:
                        data_[at_++] = data[i];
                        break;
                }
            }
        } else {
            memcpy(data_ + position, data, n);
            at_ = position + n;
        }
        highwater_ = std::max(highwater_, at_);
        return *this;
    }

    // Add data to the end of the file with or without escaping it for XML
    JsonOutput& append(const char *data, size_t n, Escape::Boolean doEscape) {
        write(data, highwater_, n, doEscape);
        return *this;
    }
    JsonOutput& append(const char *data, Escape::Boolean doEscape) {
        return append(data, strlen(data), doEscape);
    }
    JsonOutput& append(const std::string &s, Escape::Boolean doEscape) {
        return append(s.c_str(), s.size(), doEscape);
    }
    JsonOutput& append(const XmlLexer::SharedString &s, Escape::Boolean doEscape) {
        return append(s.data(), s.size(), doEscape);
    }

    // Add data to end of file without escaping special XML characters
    template<class T>
    JsonOutput& operator<<(T data) {
        return append(data, Escape::NO);
    }

    // Character at specified position
    char operator[](size_t at) const {
        ASSERT_require(at < end_);
        return data_[at];
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Support for paths, which is a way of naming a subtree by describing how to get to its root from the main tree.

// Path specification. This is used for paths specified on the command-line. It is too slow and heavy for paths encountered
// during parsing. All path specifications are terminated by a special component that describes what to do when the path
// matches in the XML input. This last component must be something other than MATCH.
enum PathAction { BEGIN, MATCH, ECHO, DELETE, RENAME };

typedef std::pair<PathAction, std::string> PathComponent;
typedef std::list<PathComponent> PathSpec;
typedef std::vector<PathSpec> PathSpecs;

bool
consecutiveWildcards(const std::string &a, const std::string &b) {
    return a == "*" && a == b;
}

PathSpec
parsePathSpec(const std::string &name, const std::string &pstr, PathAction action, const std::string &arg = "") {
    using namespace Rose::StringUtility;
    ASSERT_forbid(MATCH == action);

    std::vector<std::string> parts;
    boost::split(parts /*out*/, pstr, boost::is_any_of("."));
    size_t n = parts.size();
    parts.erase(std::unique(parts.begin(), parts.end(), consecutiveWildcards), parts.end());
    if (n != parts.size())
        throw std::runtime_error("consecutive wildcards in \"" + cEscape(pstr) + "\"");

    PathSpec retval;
    retval.push_back(std::make_pair(BEGIN, name));
    BOOST_FOREACH (const std::string &part, parts) {
        if (part.empty())
            throw std::runtime_error("invalid path \"" + cEscape(pstr) + "\"");
        retval.push_back(std::make_pair(MATCH, part));
    }
    retval.push_back(std::make_pair(action, arg));
    return retval;
}

// Return the first component of the path spec, which must have type BEGIN
PathSpec::const_iterator
pathBegin(PathSpec::const_iterator component) {
    while (component->first != BEGIN)
        --component;
    return component;
}

// Accumulates matching path actions and checks for conflicting actions
class ActionAccumulator {
    bool deleted_;                                      // a DELETE action was merged?
    std::string renamed_;                               // replacement for a RENAME action
    size_t nActions_;                                   // total number of actions merged

public:
    ActionAccumulator()
        : deleted_(false), nActions_(0) {}

    void merge(PathSpec::const_iterator action) {
        using namespace Rose::StringUtility;
        switch (action->first) {
            case ECHO:
                // Echo never conflicts, so we can do it right away
                SAWYER_MESG(mlog[WHERE]) <<"echo \"" <<cEscape(action->second) <<"\"\n";
                std::cout <<action->second <<"\n";
                ++nActions_;
                break;

            case DELETE:
                // Delete takes precedence over rename
                SAWYER_MESG(mlog[WHERE]) <<"deleting path " <<pathBegin(action)->second <<"\n";
                deleted_ = true;
                renamed_.clear();
                ++nActions_;
                break;

            case RENAME:
                if (deleted_) {
                    // Delete takes precedence over rename
                } else if (!renamed_.empty() && renamed_ != action->second) {
                    SAWYER_MESG(mlog[WARN]) <<"conflicting rename operations \"" <<cEscape(renamed_) <<"\" and \""
                                            <<cEscape(action->second) <<"\"\n";
                } else {
                    SAWYER_MESG(mlog[WHERE]) <<"renaming path " <<pathBegin(action)->second <<"\n";
                    renamed_ = action->second;
                }
                ++nActions_;
                break;

            case MATCH:
            case BEGIN:
                // Not an action; nothing to merge
                break;
        }
    }

    bool deleted() const {
        return deleted_;
    }

    size_t nActions() const {
        return nActions_;
    }

    const std::string& renamed() const {
        return renamed_;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class XmlParser {
    enum PlaceHolder { LEFT_ARRAY=0, LEFT_OBJECT=1, NPLACEHOLDERS };
    typedef std::map<std::string, XmlLexer::Token> AttributeMap;

    struct PathElement {
        XmlLexer::Token tag;
        size_t placeholders;                            // position of the spaces LEFT_ARRAY and LEFT_OBJECT
        size_t nChildren;
        XmlLexer::SharedString lastChild;               // name of the latest-added child
        size_t lastChildPlaceholders;                   // position of latest-added child's placeholders
        std::vector<PathSpec::const_iterator> nextMatch;// next part of path to match
        bool suppressOutput;                            // avoid emitting anything for this subtree

#ifdef XML2JSON_SUPPORT_CHECK
        AttributeMap childTagMap;
#endif

        PathElement()                                   // needed by std::vector
            : placeholders(0), nChildren(0), lastChildPlaceholders(0), suppressOutput(false) {}

        explicit PathElement(const XmlLexer::Token &tag)
            : tag(tag), placeholders(0), nChildren(0), lastChildPlaceholders(0), suppressOutput(false) {}
    };

    XmlLexer::TokenStream tokens_;
    std::vector<PathElement> path_;                     // nested tags from root to current
    JsonOutput json_;
    Sawyer::ProgressBar<size_t> progress_;
    size_t nTags_;
    bool check_;
    char const* const PLACEHOLDERS;
    PathSpecs pathsToMatch_;                            // source paths that should not be emitted

public:
    XmlParser(const std::string &xmlFileName, const std::string &jsonFileName, const PathSpecs &pathsToMatch)
        : tokens_(xmlFileName), json_(jsonFileName, tokens_.fileSize()+4096),
          progress_(tokens_.fileSize(), mlog[MARCH], "bytes read"), nTags_(0), check_(false), PLACEHOLDERS("  "),
          pathsToMatch_(pathsToMatch) {
        ASSERT_require(strlen(PLACEHOLDERS) == NPLACEHOLDERS);
    }

    // Perform extra input checking?
    bool check() const { return check_; }
    void check(bool b) { check_ = b; }

    // TagName := TOK_QMARK TOK_SYMBOL
    //          | TOK_BANG TOK_SYMBOL
    //          | TOK_SLASH TOK_SYMBOL
    //          | TOK_SYMBOL
    // Returns the type of the first token and the TOK_SYMBOL token.
    std::pair<XmlLexer::TokenType, XmlLexer::Token> parseTagName() {
        const XmlLexer::TokenType tt = tokens_[0].type();
        if (XmlLexer::TOK_SYMBOL == tt) {
            const std::pair<XmlLexer::TokenType, XmlLexer::Token> retval(tt, tokens_[0]);
            tokens_.consume(1);
            return retval;
        } else {
            ASSERT_require(XmlLexer::TOK_QMARK==tt || XmlLexer::TOK_BANG==tt || XmlLexer::TOK_SLASH==tt);
            ASSERT_require(tokens_[1].type() == XmlLexer::TOK_SYMBOL);
            const std::pair<XmlLexer::TokenType, XmlLexer::Token> retval(tt, tokens_[1]);
            tokens_.consume(2);
            return retval;
        }
    }

#ifdef XML2JSON_SUPPORT_CHECK
    // Returns true if the specified tag is unique for the parent and then remembers it. If not unique, prints an
    // error message and returns false.  As a special case, returns true if the specified tag is not unique but happens
    // to match its previous sibling (because that's an array).
    bool isUniqueSibling(const XmlLexer::Token &tag) {
        if (path_.empty())
            return true;
        PathElement &parent = path_.back();
        if (tokens_.lexeme(tag) == parent.lastChild)
            return true;
        std::pair<AttributeMap::iterator, bool> inserted =
            parent.childTagMap.insert(std::make_pair(tokens_.lexeme(tag).toString(), tag));
        if (!inserted.second) {
            tokens_.emit(tokens_.fileName(), tag, "object was already defined as a previous sibling");
            tokens_.emit(tokens_.fileName(), inserted.first->second, "previous definition was here");
            return false;
        }
        return true;
    }
#endif

    // Show the XML path, augmented by the optional token at the end.
    void showXmlPath(std::ostream &out, const XmlLexer::Token &token = XmlLexer::Token()) {
        for (size_t i=1; i<path_.size(); ++i) {         // skip root since it's synthesized as "" at line 0 col 0
            std::pair<size_t, size_t> loc = tokens_.location(path_[i].tag);
            out <<"  \"" <<Rose::StringUtility::cEscape(tokens_.lexeme(path_[i].tag).toString()) <<"\""
                <<" at line " <<(loc.first+1) <<" col " <<(loc.second+1) <<"\n";
        }
        if (token.type() != XmlLexer::TOK_EOF) {
            std::pair<size_t, size_t> loc = tokens_.location(token);
            out <<"  \"" <<Rose::StringUtility::cEscape(tokens_.lexeme(token).toString()) <<"\""
                <<" at line " <<(loc.first+1) <<" col " <<(loc.second+1) <<"\n";
        }
    }

    // Match tokenFirstChar+token against the component pattern. The match success if component is the last matching component
    // and the next component in the list is some action like DELETE, ECHO, or RENAME.
    bool matchPathPattern(char tokenFirstChar, const XmlLexer::Token &token, PathSpec::const_iterator component) {
        ASSERT_require(MATCH == component->first);
        ASSERT_forbid(component->second.empty());
        return  (component->second == "*" ||
                 (component->second[0] == tokenFirstChar && tokens_.matches(token, component->second.c_str()+1)));
    }

    // Matches the specified path component against a specific input token.
    bool matchPathPattern(const XmlLexer::Token &token, PathSpec::const_iterator component) {
        ASSERT_require(MATCH == component->first);
        ASSERT_forbid(component->second.empty());
        return component->second == "*" || tokens_.matches(token, component->second.c_str());
    }

    // Mathces the specified path component against a specific string
    bool matchPathPattern(const std::string &haystack, PathSpec::const_iterator component) {
        ASSERT_require(MATCH == component->first);
        ASSERT_forbid(component->second.empty());
        return component->second == "*" || component->second == haystack;
    }

    // Insert matching paths for the root tag, which is the synthesized "" at line 0 col 0 of the input.  All paths match at
    // the root. Updates the action accumulator for those paths that completely matched at the root.
    bool insertMatchingPaths(PathElement &elmt /*out*/, const PathSpecs &pathsToMatch, ActionAccumulator &actions /*out*/) {
        bool retval = false;
        elmt.nextMatch.reserve(pathsToMatch_.size());
        BOOST_FOREACH (const PathSpec &pathSpec, pathsToMatch_) {
            ASSERT_forbid(pathSpec.empty());
            PathSpec::const_iterator component = pathSpec.begin();
            ASSERT_require(BEGIN == component->first);  // all paths start with a BEGIN
            ++component;
            if (MATCH == component->first) {
                elmt.nextMatch.push_back(component);    // what to match at the next lower level of the tree
            } else {
                actions.merge(component);
                retval = true;
            }
        }
        return retval;
    }

    // Insert matching paths into ELMT based on partial path matches at previous element. Returns true if anything matched, and
    // updates the action accumulator.
    bool insertMatchingPaths(PathElement &elmt /*out*/, const PathElement &prevElmt, ActionAccumulator &actions /*out*/) {
        bool retval = false;
        if (prevElmt.suppressOutput)
            return false;                               // no need to match further if we're suppressing output
        BOOST_FOREACH (PathSpec::const_iterator component, prevElmt.nextMatch) {
            if (matchPathPattern(elmt.tag, component)) {
                if (component->second == "*")
                    elmt.nextMatch.push_back(component);// next level could match the '*' again
                ++component;
                if (MATCH == component->first) {
                    elmt.nextMatch.push_back(component);
                } else {
                    actions.merge(component);
                    retval = true;
                }
            }
        }
        return retval;
    }

    // This is called when we see "<tag", "<!tag" or "<?tag" in the input XML
    void tagStartCallback(const XmlLexer::Token &tag) {
        if ((++nTags_ & 0x1fffff) == 0)                 // try not to adjust progress_ too often since this is hot
            progress_.value(tag.offset());

        // Append a new path element
        ActionAccumulator actions;                      // actions for matched path specs
        {
            PathElement elmt(tag);
            bool matchesFound = false;
            if (path_.empty()) {
                matchesFound = insertMatchingPaths(elmt /*out*/, pathsToMatch_, actions /*out*/);
            } else {
                elmt.suppressOutput = path_.back().suppressOutput;
                matchesFound = insertMatchingPaths(elmt /*out*/, path_.back(), actions /*out*/);
            }
            if (matchesFound && mlog[WHERE]) {
                mlog[WHERE] <<Rose::StringUtility::plural(actions.nActions(), "matches", "match") <<" occurred here:\n";
                showXmlPath(mlog[WHERE], tag);
            }
            if (actions.deleted())
                elmt.suppressOutput = true;
            path_.push_back(elmt);
        }

        // Back-patch the parent if necessary
        if (path_.size() > 1) {
            if (!path_.back().suppressOutput) {
                PathElement &parent = path_[path_.size()-2];

                // Parent must be an object
                json_.write("{", parent.placeholders+LEFT_OBJECT, 1, Escape::NO);

                // If we have siblings, then we need a comma
                if (++parent.nChildren > 1)             // don't count the child if it's not emitted
                    json_ <<",";
            }

#ifdef XML2JSON_SUPPORT_CHECK
            if (check_)
                isUniqueSibling(tag);
#endif
        }

        if (!path_.back().suppressOutput) {
            if (tag.size() == 0) {                          // top-level, anonymous object?
                ASSERT_require(path_.size()==1);
                path_.back().placeholders = json_.curpos();
                json_ <<PLACEHOLDERS;
            } else {
                ASSERT_forbid(path_.empty());
                PathElement &parent = path_[path_.size()-2];
                if (parent.lastChild == tokens_.lexeme(tag)) {
                    // If this tag has the same name as the previous tag, then we're encountering an array. For instance, the
                    // input might be:
                    //   XML  |<vector>
                    //   XML  |  <elmt>0</elmt>
                    //   XML  |  <elmt>1</elmt>
                    //   XML  |  <elmt>2</elmt>
                    //   XML  |</vector>
                    // in which case we should generate
                    //   JSON |"vector":{
                    //   JSON |  "elmt":["0","1","2"]
                    //   JSON |}
                    size_t eof = json_.eof();
                    if (eof>=2 && ']'==json_[eof-2]) {      // file ends with "]," (the comma we added just above)
                        json_.truncate(eof-2);
                        json_ <<",";
                    } else {                                // we just learned this is an array (i.e., we're at the 2nd elmt)
                        json_.write("[", parent.lastChildPlaceholders+LEFT_ARRAY, 1, Escape::NO);
                    }
                    path_.back().placeholders = parent.lastChildPlaceholders;
                    if ('{' == json_[parent.lastChildPlaceholders+LEFT_OBJECT])
                        json_ <<"{";
                } else {
                    if (actions.renamed().empty()) {
                        json_ <<"\"" <<tokens_.lexeme(tag) <<"\":";
                    } else {
                        json_ <<"\"" <<actions.renamed() <<"\":";
                    }
                    path_.back().placeholders = json_.curpos();
                    json_ <<PLACEHOLDERS;
                }
            }
        }

        // Parent needs to store info about latest emitted child
        if (path_.size() > 1 && !path_.back().suppressOutput) {
            PathElement &parent = path_[path_.size()-2];
            parent.lastChild = tokens_.lexeme(tag);
            parent.lastChildPlaceholders = path_.back().placeholders;
        }
    }

    void tagEndCallback(const XmlLexer::Token &tag) {
        ASSERT_require(!path_.empty());
        ASSERT_require(tokens_.lexeme(path_.back().tag) == tokens_.lexeme(tag));

        // Finish this path element and pop it from the path
        if (!path_.back().suppressOutput) {
            if (0 == path_.back().nChildren) {
                json_ <<"null";
            }
            if ('{' == json_[path_.back().placeholders+LEFT_OBJECT])
                json_ <<"}";
            if ('[' == json_[path_.back().placeholders+LEFT_ARRAY])
                json_ <<"]";
        }
        path_.pop_back();
    }

    void boolPropertyCallback(const XmlLexer::Token &name) {
        valuePropertyCallback(name, name);
    }

    // Find path patterns that match completely at the specified XML property
    bool matchProperty(char firstChar, const XmlLexer::Token &name, ActionAccumulator &actions /*out*/) {
        BOOST_FOREACH (PathSpec::const_iterator component, path_.back().nextMatch) {
            if (matchPathPattern('@', name, component)) {
                ++component;
                actions.merge(component);
            }
        }
        return actions.nActions() > 0;
    }

    // Ditto
    bool matchProperty(const std::string &name, ActionAccumulator &actions /*out*/) {
        BOOST_FOREACH (PathSpec::const_iterator component, path_.back().nextMatch) {
            if (matchPathPattern(name, component)) {
                ++component;
                actions.merge(component);
            }
        }
        return actions.nActions() > 0;
    }

    void valuePropertyCallback(const XmlLexer::Token &name, const XmlLexer::Token &value) {
        ASSERT_require(!path_.empty());
        if (path_.back().suppressOutput)
            return;

        ActionAccumulator actions;
        if (matchProperty('@', name, actions /*out*/) && mlog[WHERE].enabled()) {
            mlog[WHERE] <<Rose::StringUtility::plural(actions.nActions(), "matches", "match") <<" occurred here:\n";
            showXmlPath(mlog[WHERE], name);
        }

        if (!actions.deleted()) {
            // Make sure the output is an object, i.e., the curly brace in '"object_name":{...'
            json_.write("{", path_.back().placeholders+LEFT_OBJECT, 1, Escape::NO);

            if (path_.back().nChildren++ > 0)
                json_ <<",";

            if (actions.renamed().empty()) {
                json_ <<"\"@" <<tokens_.lexeme(name) <<"\":";
            } else {
                json_ <<"\"" <<actions.renamed() <<"\":";
            }
#if 0 // good place for line-feeds if you need to debug malformed JSON
            json_ <<"\n";
#endif
            json_ <<"\"";
            json_.append(tokens_.lexeme(value), Escape::YES);
            json_ <<"\"";
        }
    }

    void textCallback(const XmlLexer::Token &t) {
        ASSERT_require(!path_.empty());
        if (path_.back().suppressOutput)
            return;

        ActionAccumulator actions;
        if (matchProperty("#text", actions) && mlog[WHERE].enabled()) {
            mlog[WHERE] <<Rose::StringUtility::plural(actions.nActions(), "matches", "match") <<" occurred here:\n";
            showXmlPath(mlog[WHERE]);                   // FIXME[Robb Matzke 2017-08-01]: what about "#text" location?
        }

        if (!actions.deleted()) {
            if (++path_.back().nChildren > 1) {
                if (actions.renamed().empty()) {
                    json_ <<",\"#text\":";
                } else {
                    json_ <<",\"" <<actions.renamed() <<"\":";
                }
            }
            json_ <<"\"";
            json_.append(tokens_.lexeme(t), Escape::YES);
            json_ <<"\"";
        }
    }

    // Parse a '<' .... '>' construct
    void parseTag() {
        ASSERT_require(tokens_[0].type() == XmlLexer::TOK_LT);
        tokens_.consume(1);

        std::pair<XmlLexer::TokenType, XmlLexer::Token> tag = parseTagName();

        if (tag.first != XmlLexer::TOK_SLASH)
            tagStartCallback(tag.second);

        bool terminated = false;
        while (true) {
            // Look for endings: '>' or '?>' or '/>'
            if (tokens_[0].type() == XmlLexer::TOK_GT) {
                tokens_.consume(1);
                break;
            }
            if (tokens_[0].type() == XmlLexer::TOK_QMARK && tokens_[1].type() == XmlLexer::TOK_GT) {
                tokens_.consume(2);
                break;
            }
            if (tokens_[0].type() == XmlLexer::TOK_SLASH && tokens_[1].type() == XmlLexer::TOK_GT) {
                terminated = true;
                tokens_.consume(2);
                break;
            }

            // If we saw "</tag" then we must see a ">" next.
            if (tag.first == XmlLexer::TOK_SLASH) {
                tokens_.emit(tokens_.fileName(), tokens_[0], "'>' expected");
                return;
            }

            // We must be inside a '<tag ...>' so look for SYMBOL = STRING, or just a SYMBOL
            if (tokens_[0].type() != XmlLexer::TOK_SYMBOL) {
                tokens_.emit(tokens_.fileName(), tokens_[0], "property name expected");
                return;
            }
            if (tokens_[1].type() == XmlLexer::TOK_EQ) {
                if (tokens_[2].type() != XmlLexer::TOK_STRING) {
                    tokens_.emit(tokens_.fileName(), tokens_[2], "string value expected");
                    return;
                }
                valuePropertyCallback(tokens_[0], tokens_[2]);
                tokens_.consume(3);
            } else {
                boolPropertyCallback(tokens_[0]);
                tokens_.consume(1);
            }
        }

        if (tag.first != XmlLexer::TOK_SYMBOL || terminated)
            tagEndCallback(tag.second);
    }

    void parse() {
        // Top-level, anonymous object
        tagStartCallback(XmlLexer::Token());

        // Zero or more objects created from XML
        while (true) {
            const XmlLexer::Token &t0 = tokens_.get(0, XmlLexer::PossibleText::YES);
            if (t0.type() == XmlLexer::TOK_LT) {
                parseTag();
            } else if (t0.type() == XmlLexer::TOK_TEXT) {
                textCallback(t0);
                tokens_.consume(1);
            } else if (t0.type() == XmlLexer::TOK_EOF) {
                break;
            } else {
                tokens_.emit(tokens_.fileName(), tokens_[0], "unexpected token");
                break;
            }
        }

        // Finish top-level, anonymous object
        ASSERT_require(path_.size() == 1);
        tagEndCallback(XmlLexer::Token());
        json_ <<"\n";

        mlog[INFO] <<"bytes read:    " <<tokens_.fileSize() <<"\n";
        mlog[INFO] <<"XML tags:      " <<nTags_ <<"\n";
        mlog[INFO] <<"bytes written: " <<json_.eof() <<"\n";
    }
};

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Rose::Diagnostics::initAndRegister(&mlog, "tool");
    Settings settings = parseCommandLine(argc, argv);

    PathSpecs pathsToMatch;

    // Deletions
    for (size_t i=0; i<settings.deletions.size(); ++i) {
        std::string name = "cmdline --delete #" + boost::lexical_cast<std::string>(i);
        try {
            pathsToMatch.push_back(parsePathSpec(name, settings.deletions[i], DELETE));
        } catch (const std::runtime_error &e) {
            mlog[FATAL] <<"for --delete switch, " <<e.what() <<"\n";
            exit(1);
        }
    }

    // Renames
    for (size_t i=0; i<settings.renames.size(); ++i) {
        std::string name = "cmdline --rename #" + boost::lexical_cast<std::string>(i);
        std::string spec = settings.renames[i];
        size_t colon = spec.find(':');
        std::string newName;
        if (std::string::npos == colon || colon+1 >= spec.size()) {
            mlog[FATAL] <<"for --rename switch, no new name specified in \"" <<Rose::StringUtility::cEscape(spec) <<"\"\n";
            exit(1);
        } else {
            newName = spec.substr(colon+1);
            spec = spec.substr(0, colon);
            ASSERT_forbid(newName.empty());
        }
        try {
            pathsToMatch.push_back(parsePathSpec(name, spec, RENAME, newName));
        } catch (const std::runtime_error &e) {
            mlog[FATAL] <<"for --rename switch, " <<e.what() <<"\n";
            exit(1);
        }
    }

    // Echoing
    for (size_t i=0; i<settings.echoes.size(); ++i) {
        std::string name = "cmdline --echo #" + boost::lexical_cast<std::string>(i);
        std::string spec = settings.echoes[i];
        size_t colon = spec.find(':');
        std::string emission;
        if (std::string::npos == colon) {
            emission = "found " + name + ": " + spec;
        } else {
            emission = spec.substr(colon+1);
            spec = spec.substr(0, colon);
        }
        try {
            pathsToMatch.push_back(parsePathSpec(name, spec, ECHO, emission));
        } catch (const std::runtime_error &e) {
            mlog[FATAL] <<"for --echo switch, " <<e.what() <<"\n";
            exit(1);
        }
    }

    XmlParser xml(settings.xmlFileName, settings.jsonFileName, pathsToMatch);
    xml.check(settings.check);
    xml.parse();
}
