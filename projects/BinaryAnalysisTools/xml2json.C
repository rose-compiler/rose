static const char *purpose = "convert XML to JSON";
static const char *description =
    "Converts XML to JSON in the fastest, most memory-efficient way possible, handling only that XML which is produced "
    "by the ROSE binary analysis state serialization.";

#include <rose.h>                                       // Must be first ROSE include file
#include <Diagnostics.h>                                // ROSE

#include <boost/algorithm/string/trim.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/lexical_cast.hpp>
#include <fcntl.h>
#include <Sawyer/LineVector.h>
#include <Sawyer/ProgressBar.h>

using namespace Sawyer::Message::Common;

Sawyer::Message::Facility mlog;

struct Settings {
    std::string xmlFileName;                            // input file name
    std::string jsonFileName;                           // output file name
    bool check;                                         // perform extra input checking

    Settings(): check(false) {}
};

Settings
parseCommandLine(int argc, char *argv[]) {
    using namespace Sawyer::CommandLine;
    Settings retval;

    SwitchGroup tool("Tool-specific switches");
    tool.name("tool");
    /*Rose*/::CommandlineProcessing::insertBooleanSwitch(tool, "check", retval.check, "Perform extra input checking.");
    
    Parser p = /*Rose*/::CommandlineProcessing::createEmptyParser(purpose, description);
    p.with(/*Rose*/::CommandlineProcessing::genericSwitches());
    p.with(tool);
    p.doc("Synopsis", "@prop{programName} [@v{switches}] @v{xml_input_file} @v{json_output_file}");
    p.errorStream(mlog[FATAL]);

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

std::string
stringify(TokenType tt) {
    switch (tt) {
        case TOK_EOF:    return "TOK_EOF";
        case TOK_LT:     return "TOK_LT";
        case TOK_GT:     return "TOK_GT";
        case TOK_QMARK:  return "TOK_QMARK";
        case TOK_EQ:     return "TOK_EQ";
        case TOK_BANG:   return "TOK_BANG";
        case TOK_SLASH:  return "TOK_SLASH";
        case TOK_SYMBOL: return "TOK_SYMBOL";
        case TOK_STRING: return "TOK_STRING";
        case TOK_TEXT:   return "TOK_TEXT";
        case TOK_OTHER:  return "TOK_OTHER";
    }
    ASSERT_not_reachable("invalid token type " + boost::lexical_cast<std::string>(tt));
}

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

class Token {
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
    std::vector<Token> tokens_;                         // token stream filled on demand

public:
    // Create a token stream that will process the specified file. The file is memory mapped rather than read.
    explicit TokenStream(const std::string &fileName)
        : fileName_(fileName), content_(fileName), at_(0) {}

    // Create a token stream using the specified content buffer. The file name is only used for diagnostics.
    TokenStream(const std::string &fileName, const Sawyer::Container::Buffer<size_t, char>::Ptr &buffer)
        : fileName_(fileName), content_(buffer), at_(0) {}

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
        if (possibleText && tokens_.size() > lookahead) {
            at_ = tokens_[lookahead].begin_;
            tokens_.resize(lookahead);
        }

        while (lookahead >= tokens_.size()) {
            makeNextToken(possibleText && tokens_.size()==lookahead ? PossibleText::YES : PossibleText::NO);
            ASSERT_require(!tokens_.empty());
            if (tokens_.back().type() == TOK_EOF)
                return tokens_.back();
        }
        return tokens_[lookahead];
    }
    
    // Consume some tokens, advancing the current token pointer to a later token. The consumed tokens are forever lost.
    void consume(size_t nTokens = 1) {
        if (nTokens >= tokens_.size()) {
            tokens_.clear();
        } else {
            tokens_.erase(tokens_.begin(), tokens_.begin()+nTokens);
        }
    }

    // Return token lexeme as a shared string. I.e., an object that points directly into the file.
    SharedString lexeme(const Token &t) const {
        return SharedString(content_.characters(t.begin_), t.end_ - t.begin_);
    }
    
    // Return the string of a token for diagnostics. This includes the token type, a space, and the lexeme.
    std::string toString(const Token &t) const {
        return stringify(t.type()) + " " + lexeme(t).toString();
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
    
    // Return the line number and column for the start of the specified token. The line number is 1-origin, and the column
    // number is 0-origin.
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
        if (!tokens_.empty() && tokens_.back().type() == TOK_EOF)
            return;

        // Skip white space
        int c = content_.character(at_);
        while (isspace(c))
            c = content_.character(++at_);

        // End of file?
        if (EOF == c) {
            tokens_.push_back(Token(TOK_EOF, at_, at_));
            return;
        }

        if (possibleText) {
            if ('<' == c) {
                tokens_.push_back(Token(TOK_LT, at_, at_+1));
                ++at_;
            } else {
                tokens_.push_back(Token(TOK_TEXT, at_, at_+1));
                scanText();
                tokens_.back().end_ = at_;
            }
        } else {
            switch (int c = content_.character(at_)) {
                case EOF:
                    tokens_.push_back(Token(TOK_EOF, at_, at_));
                    break;
                case '<':
                    tokens_.push_back(Token(TOK_LT, at_, at_+1));
                    ++at_;
                    break;
                case '>':
                    tokens_.push_back(Token(TOK_GT, at_, at_+1));
                    ++at_;
                    break;
                case '=':
                    tokens_.push_back(Token(TOK_EQ, at_, at_+1));
                    ++at_;
                    break;
                case '?':
                    tokens_.push_back(Token(TOK_QMARK, at_, at_+1));
                    ++at_;
                    break;
                case TOK_EQ:
                    tokens_.push_back(Token(TOK_EQ, at_, at_+1));
                    ++at_;
                    break;
                case '!':
                    tokens_.push_back(Token(TOK_BANG, at_, at_+1));
                    ++at_;
                    break;
                case '/':
                    tokens_.push_back(Token(TOK_SLASH, at_, at_+1));
                    ++at_;
                    break;
                case '"': // exclude the delimiting quotes
                    tokens_.push_back(Token(TOK_STRING, at_+1, at_+2));
                    scanString();
                    tokens_.back().end_ = at_-1;
                    break;
                default:
                    if (isalpha(c)) {
                        tokens_.push_back(Token(TOK_SYMBOL, at_, at_+1));
                        scanSymbol();
                        tokens_.back().end_ = at_;
                    } else {
                        tokens_.push_back(Token(TOK_OTHER, at_, at_+1));
                        ++at_;
                    }
                    break;
            }
        }
    }
};

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream &out, const XmlLexer::SharedString &s) {
    out <<s.toString();
    return out;
}

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
            if (ftruncate(fd_, highwater_) == -1 || close(fd_) == -1)
                throw std::runtime_error(strerror(errno) + (": " + fileName_));
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
class XmlParser {
    enum PlaceHolder { LEFT_ARRAY=0, LEFT_OBJECT=1, NPLACEHOLDERS };
    typedef std::map<std::string, XmlLexer::Token> AttributeMap;

    struct PathElement {
        XmlLexer::Token tag;
        size_t placeholders;                            // position of the spaces LEFT_ARRAY and LEFT_OBJECT
        size_t nChildren;
        XmlLexer::SharedString lastChild;               // name of the latest-added child
        size_t lastChildPlaceholders;                   // position of latest-added child's placeholders
        AttributeMap childTagMap;

        PathElement(): placeholders(0), nChildren(0), lastChildPlaceholders(0) {} // needed by std::vector
        
        explicit PathElement(const XmlLexer::Token &tag)
            : tag(tag), placeholders(0), nChildren(0), lastChildPlaceholders(0) {}
    };

    XmlLexer::TokenStream tokens_;
    std::vector<PathElement> path_;                     // nested tags from root to current
    bool trace_;
    JsonOutput json_;
    Sawyer::ProgressBar<size_t> progress_;
    size_t nTags_;
    bool check_;
    char const* const PLACEHOLDERS;

public:
    XmlParser(const std::string &xmlFileName, const std::string &jsonFileName)
        : tokens_(xmlFileName), trace_(false), json_(jsonFileName, tokens_.fileSize()+4096),
          progress_(tokens_.fileSize(), mlog[MARCH], "bytes read"), nTags_(0), check_(false), PLACEHOLDERS("  ") {
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
        if (tokens_[0].type() == XmlLexer::TOK_QMARK ||
            tokens_[0].type() == XmlLexer::TOK_BANG ||
            tokens_[0].type() == XmlLexer::TOK_SLASH) {
            ASSERT_require(tokens_[1].type() == XmlLexer::TOK_SYMBOL);
            XmlLexer::TokenType type = tokens_[0].type();
            XmlLexer::Token name = tokens_[1];
            tokens_.consume(2);
            return std::make_pair(type, name);
        } else {
            ASSERT_require(tokens_[0].type() == XmlLexer::TOK_SYMBOL);
            XmlLexer::TokenType type = XmlLexer::TOK_SYMBOL;
            XmlLexer::Token name = tokens_[0];
            tokens_.consume(1);
            return std::make_pair(type, name);
        }
    }

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
    
    // This is called when we see "<tag", "<!tag" or "<?tag" in the input XML
    void tagStartCallback(const XmlLexer::Token &tag) {
        if (++nTags_ % 10000 == 0)
            progress_.value(tag.offset());

        if (trace_) {
            mlog[DEBUG] <<std::string(path_.size(), ' ') <<"starting \""
                        <<tokens_.lexeme(tag) <<"\" at level " <<path_.size() <<"\n";
        }

        if (!path_.empty()) {
            // Parent must be an object
            json_.write("{", path_.back().placeholders+LEFT_OBJECT, 1, Escape::NO);

            // If we have siblings, then we need a comma
            if (++path_.back().nChildren > 1)
                json_ <<",";

            if (check_)
                isUniqueSibling(tag);
        }

        // Append a new path element
        path_.push_back(PathElement(tag));
        if (tag.size() == 0) {                          // top-level, anonymous object?
            ASSERT_require(path_.size()==1);
            path_.back().placeholders = json_.curpos();
            json_ <<PLACEHOLDERS;
        } else {
            ASSERT_forbid(path_.empty());
            PathElement &parent = path_[path_.size()-2];
            if (parent.lastChild == tokens_.lexeme(tag)) {
                // If this tag has the same name as the previous tag, then we're encountering an array. For instance, the input
                // might be:
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
                json_ <<"\"" <<tokens_.lexeme(tag) <<"\":";
                path_.back().placeholders = json_.curpos();
                json_ <<PLACEHOLDERS;
            }
        }

        // Parent needs to store info about latest child
        if (path_.size() > 1) {
            PathElement &parent = path_[path_.size()-2];
            parent.lastChild = tokens_.lexeme(tag);
            parent.lastChildPlaceholders = path_.back().placeholders;
        }
    }

    void tagEndCallback(const XmlLexer::Token &tag) {
        ASSERT_require(!path_.empty());
        ASSERT_require(tokens_.lexeme(path_.back().tag) == tokens_.lexeme(tag));

        if (trace_) {
            mlog[DEBUG] <<std::string(path_.size()-1, ' ') <<"ending   \"" <<tokens_.lexeme(tag)
                        <<"\" at level " <<path_.size() <<"\n";
        }

        // Finish this path element and pop it from the path
        if (0 == path_.back().nChildren) {
            json_ <<"null";
        }
        if ('{' == json_[path_.back().placeholders+LEFT_OBJECT])
            json_ <<"}";
        if ('[' == json_[path_.back().placeholders+LEFT_ARRAY])
            json_ <<"]";
        path_.pop_back();
    }

    void boolPropertyCallback(const XmlLexer::Token &name) {
        valuePropertyCallback(name, name);
    }

    void valuePropertyCallback(const XmlLexer::Token &name, const XmlLexer::Token &value) {
        ASSERT_require(!path_.empty());
        if (trace_) {
            mlog[DEBUG] <<std::string(path_.size(), ' ')
                        <<"property \"" <<tokens_.lexeme(name) <<"\" = " <<tokens_.lexeme(value) <<"\n";
        }

        // Make sure the output is an object, i.e., the curly brace in '"object_name":{...'
        json_.write("{", path_.back().placeholders+LEFT_OBJECT, 1, Escape::NO);

        if (path_.back().nChildren++ > 0)
            json_ <<",";

        json_ <<"\"@" <<tokens_.lexeme(name) <<"\":";
#if 0 // good place for line-feeds if you need to debug malformed JSON
        json_ <<"\n";
#endif
        json_ <<"\"";
        json_.append(tokens_.lexeme(value), Escape::YES);
        json_ <<"\"";
    }

    void textCallback(const XmlLexer::Token &t) {
        ASSERT_require(!path_.empty());
        if (trace_)
            mlog[DEBUG] <<std::string(path_.size(), ' ') <<"text = \"" <<tokens_.lexeme(t) <<"\"\n";

        if (++path_.back().nChildren > 1)
            json_ <<",\"#text\":";
        json_ <<"\"";
        json_.append(tokens_.lexeme(t), Escape::YES);
        json_ <<"\"";
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

    XmlParser xml(settings.xmlFileName, settings.jsonFileName);
    xml.check(settings.check);
    xml.parse();
}
