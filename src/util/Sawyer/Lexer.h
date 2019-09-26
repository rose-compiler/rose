// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#ifndef Sawyer_Lexer_H
#define Sawyer_Lexer_H

#include <Sawyer/AllocatingBuffer.h>
#include <Sawyer/LineVector.h>
#include <Sawyer/Optional.h>
#include <Sawyer/Sawyer.h>

#include <boost/filesystem.hpp>

namespace Sawyer {

namespace Lexer {

/** Represents one token of input.
 *
 *  Each token has a user-defined type which is some enumerated constant, or no type at all which means the token represents
 *  the end of the input stream.  Tokens do not store their own lexeme, but rather point to the beginning and end of their
 *  lexeme in the input stream. */
template<typename T>
class SAWYER_EXPORT Token {
public:
    typedef T TokenEnum;

private:
    Optional<TokenEnum> type_;                          // empty means EOF
    size_t begin_;                                      // beginning character position in the input
    size_t end_;                                        // ending (exclusive) character position in the input

public:
    /** Construct an EOF token. */
    Token(): begin_(0), end_(0) {}

    /** Construct a token.
     *
     *  The token has the specified type and its lexeme are the characters at the specified position in the input stream. The
     *  @p begin points to the first character of the lexeme and @p end points to one past the last character.  A token's
     *  lexeme is allowed to be the empty string by specifying the same value for @p begin and @p end, however, @p end must
     *  never be less than @p begin. */
    Token(TokenEnum type, size_t begin, size_t end)
        : type_(type), begin_(begin), end_(end) {
        ASSERT_require(end >= begin);
    }

    /** Whether this is an EOF token.
     *
     *  An EOF token is a special token that has no type and an empty lexeme.  EOF tokens are constructed by the default
     *  constructor. */
    bool isEof() const {
        if (type_)
            return false;
        return true;
    }

    /** Returns the token.
     *
     *  Since EOF tokens have no type, this must not be called for an EOF token. */
    TokenEnum type() const {
        return *type_;
    }

    /** Token lexeme starting position.
     *
     *  This is the starting offset in the input for the first character of this token's lexeme. */
    size_t begin() const {
        return begin_;
    }

    /** Token lexeme ending position.
     *
     *  This is the offset in the input for one position past the last character of this token's lexeme. It is guaranteed to be
     *  greater than or equal to the @ref begin position. EOF tokens will always have a @ref begin equal to the @ref end, but
     *  other empty non-EOF tokens are also possible. */
    size_t end() const {
        return end_;
    }
};

/** An ordered list of tokens scanned from input.
 *
 *  A token stream is an ordered list of tokens scanned from an unchanging input stream and consumed in the order they're
 *  produced. */
template<class T>
class SAWYER_EXPORT TokenStream {
public:
    typedef T Token;

private:
    std::string name_;                                  // name of stream (e.g., a file name)
    Container::LineVector content_;                     // line-oriented character contents of the stream
    size_t at_;                                         // cursor position in stream
    std::vector<Token> tokens_;                         // current token at [0] plus lookahead

public:
    virtual ~TokenStream() {}

    /** Create a token stream from the contents of a file. */
    explicit TokenStream(const boost::filesystem::path &fileName)
        : name_(fileName.string()), content_(fileName.string()), at_(0) {}

    /** Create a token stream from a string.
     *
     *  The string content is copied into the lexer and thus can be modified after the lexer returns without affecting the
     *  token stream. */
    explicit TokenStream(const std::string &inputString)
        : name_("string"), content_(Container::AllocatingBuffer<size_t, char>::instance(inputString)), at_(0) {}

    /** Create a token stream from a buffer.
     *
     *  The token stream uses the specified buffer, which should not be modified while the token stream is alive. */
    explicit TokenStream(const Container::Buffer<size_t, char>::Ptr &buffer)
        : name_("string"), content_(buffer), at_(0) {}

    /** Property: Name of stream. */
    const std::string& name() const {
        return name_;
    }

    /** Return the current token.
     *
     *  The current token will be an EOF token when all tokens are consumed. */
    const Token& current() {
        return (*this)[0];
    }

    /** Returns true if the stream is at the end.
     *
     *  This is equivalent to obtaining the current toking and checking whether it's the EOF token. */
    bool atEof() {
        return current().isEof();
    }

    /** Return the current or future token.
     *
     *  The array operator obtains a token from a virtual array whose first element is the current token, second element is one
     *  past the current token, etc.  The array is infinite in length, padded with EOF tokens. */
    const Token& operator[](size_t lookahead) {
        static const Token eof_;
        while (lookahead >= tokens_.size()) {
            if (!tokens_.empty() && tokens_.back().isEof())
                return eof_;
            tokens_.push_back(scanNextToken(content_, at_/*in,out*/));
        }
        return tokens_[lookahead];
    }

    /** Consume some tokens.
     *
     *  Consumes tokens by shifting @p n tokens off the low-end of the virtual array of tokens. It is permissible to consume
     *  EOF tokens since more will be generated once the end-of-input is reached. */
    void consume(size_t n = 1) {
        const Token &t = current();
        if (t.isEof()) {
            // void
        } else if (n >= tokens_.size()) {
            tokens_.clear();
        } else {
            tokens_.erase(tokens_.begin(), tokens_.begin() + n);
        }
    }

    /** Return the lexeme for a token.
     *
     *  Consults the input stream to obtain the lexeme for the specified token and converts that part of the stream to a string
     *  which is returned.  The lexeme for an EOF token is an empty string, although other tokens might also have empty
     *  lexemes.  One may query the lexeme for any token regardless of whether it's been consumed; in fact, one can even query
     *  lexemes for tokens that have never even been seen by the token stream.
     *
     *  The no-argument version returns the lexeme of the current token.
     *
     *  If you're trying to build a fast lexical analyzer, don't call this function to compare a lexeme against some known
     *  string. Instead, use @ref match, which doesn't require copying.
     *
     *  @{ */
    std::string lexeme(const Token &t) {
        if (const char *s = content_.characters(t.begin())) {
            return std::string(s, t.end() - t.begin());
        } else {
            return "";
        }
    }
    std::string lexeme() {
        return lexeme(current());
    }
    /** @} */

    /** Determine whether token is a specific type.
     *
     *  This is sometimes easier to call since it gracefully handles EOF tokens.  If called with only one argument, the desired
     *  type, then it checks the current token.
     *
     * @{ */
    bool isa(const Token &t, typename Token::TokenEnum type) {
        return !t.isEof() && t.type() == type;
    }

    bool isa(typename Token::TokenEnum type) {
        return isa(current(), type);
    }
    /** @} */

    /** Determine whether a token matches a string.
     *
     *  Compares the specified string to a token's lexeme and returns true if they are the same. This is faster than obtaining
     *  the lexeme from a token and comparing to a string since there's no string copying involved with this function.
     *
     *  The no-argument version compares the string with the current tokens' lexeme.
     *
     * @{ */
    bool match(const Token &t, const char *s) {
        ASSERT_not_null(s);
        size_t n1 = t.end() - t.begin();
        size_t n2 = strlen(s);
        if (n1 != n2)
            return false;
        const char *lexeme = content_.characters(t.begin());
        return 0 == strncmp(lexeme, s, n1);
    }
    bool match(const char *s) {
        return match(current(), s);
    }
    /** @} */
    
    /** Return the line number and offset for an input position.
     *
     *  Returns the zero-origin line number (a.k.a., line index) for the line containing the specified character position, and
     *  the offset of that character with respect to the beginning of the line. */
    std::pair<size_t, size_t> location(size_t position) {
        return content_.location(position);
    }

    /** Returns the last line index and character offset. */
    std::pair<size_t, size_t> locationEof() {
        size_t nChars = content_.nCharacters();
        return nChars > 0 ? content_.location(nChars-1) : content_.location(0);
    }
    
    /** Return the entire string for some line index. */
    std::string lineString(size_t lineIdx) {
        return content_.lineString(lineIdx);
    }

    /** Function that obtains the next token.
     *
     *  Subclasses implement this function to obtain the next token that starts at or after the specified input position. Upon
     *  return, the function should adjust @p at to point to the next position for scanning a token, which is usually the first
     *  character after the returned token's lexeme. If the scanner reaches the end of input or any condition that it deems to
     *  be the end then it should return the EOF token (a default-constructed token), after which this function will not be
     *  called again. */
    virtual Token scanNextToken(const Container::LineVector &content, size_t &at /*in,out*/) = 0;
};

} // namespace
} // namespace

#endif

