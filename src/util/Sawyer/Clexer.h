// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




// Lexical analyzer for C-like languages
#ifndef Sawyer_Clexer_H
#define Sawyer_Clexer_H

#include <Sawyer/Sawyer.h>

#include <Sawyer/Assert.h>
#include <Sawyer/Buffer.h>
#include <Sawyer/LineVector.h>

#include <string>
#include <vector>

namespace Sawyer {
namespace Language {
namespace Clexer {

enum TokenType {
    TOK_EOF,                                            // end of file
    TOK_LEFT,                                           // '(', '[', or '{'
    TOK_RIGHT,                                          // ')', ']', or '}'
    TOK_CHAR,                                           // character literal
    TOK_STRING,                                         // string literal
    TOK_NUMBER,                                         // numeric constant, including optional leading sign
    TOK_WORD,                                           // word or symbol name
    TOK_CPP,                                            // preprocessor statement starting with '#'
    TOK_OTHER                                           // anything else
};

std::string toString(TokenType);

class Token {
    friend class TokenStream;

    TokenType type_;
    size_t begin_;
    size_t end_;

public:
    Token(): type_(TOK_EOF), begin_(0), end_(0) {} // for std::vector, otherwise not used
    
    Token(TokenType type, size_t begin, size_t end)
        : type_(type), begin_(begin), end_(end) {
        ASSERT_require(end >= begin);
    }

    TokenType type() const {
        return type_;
    }

    size_t begin() const {
        return begin_;
    }

    size_t end() const {
        return end_;
    }
};

class TokenStream {
    std::string fileName_;                              // name of source file
    Sawyer::Container::LineVector content_;             // contents of source file
    size_t at_;                                         // cursor position in buffer
    std::vector<Token> tokens_;                         // token stream filled on demand
    bool skipPreprocessorTokens_;                       // skip over '#' preprocessor directives
public:
    explicit TokenStream(const std::string &fileName)
        : fileName_(fileName), content_(fileName), at_(0), skipPreprocessorTokens_(true) {}

    explicit TokenStream(const std::string &fileName, const Sawyer::Container::Buffer<size_t, char>::Ptr &buffer)
        : fileName_(fileName), content_(buffer), at_(0), skipPreprocessorTokens_(true) {}

    const std::string fileName() const { return fileName_; }
    
    bool skipPreprocessorTokens() const { return skipPreprocessorTokens_; }
    void skipPreprocessorTokens(bool b) { skipPreprocessorTokens_ = b; }

    const Token& operator[](size_t lookahead);

    void consume(size_t n = 1);

    std::string lexeme(const Token &t) const;

    std::string toString(const Token &t) const;

    // Return the line of source in which this token appears, including line termination if present.
    std::string line(const Token &t) const;
    
    bool matches(const Token &token, const char *s2) const;

    void emit(std::ostream &out, const std::string &fileName, const Token &token, const std::string &message) const;

    void emit(std::ostream &out, const std::string &fileName, const Token &begin, const Token &locus, const Token &end,
              const std::string &message) const;

    std::pair<size_t, size_t> location(const Token &token) const;

    const Sawyer::Container::LineVector& content() const {
        return content_;
    }

private:
    void scanString();
    void makeNextToken();
};


} // namespace
} // namespace
} // namespace

#endif
