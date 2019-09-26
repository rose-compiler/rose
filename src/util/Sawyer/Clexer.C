// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#include <Sawyer/Assert.h>
#include <Sawyer/Clexer.h>

#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>

namespace Sawyer {
namespace Language {
namespace Clexer {

std::string
toString(TokenType tt) {
    switch (tt) {
        case TOK_EOF: return "eof";
        case TOK_LEFT: return "left";
        case TOK_RIGHT: return "right";
        case TOK_CHAR: return "char";
        case TOK_STRING: return "string";
        case TOK_NUMBER: return "number";
        case TOK_WORD: return "word";
        case TOK_CPP: return "cpp";
        case TOK_OTHER: return "other";
    }
    ASSERT_not_reachable("invalid token type");
}

const Token&
TokenStream::operator[](size_t lookahead) {
    while (lookahead >= tokens_.size()) {
        makeNextToken();
        ASSERT_require(!tokens_.empty());
        if (tokens_.back().type() == TOK_EOF)
            return tokens_.back();
    }
    return tokens_[lookahead];
}

void
TokenStream::consume(size_t n) {
    if (n >= tokens_.size()) {
        tokens_.clear();
    } else {
        tokens_.erase(tokens_.begin(), tokens_.begin()+n);
    }
}

std::string
TokenStream::lexeme(const Token &t) const {
    if (const char *s = content_.characters(t.begin_)) {
        return std::string(s, t.end_-t.begin_);
    } else {
        return "";
    }
}

std::string
TokenStream::toString(const Token &t) const {
    return Sawyer::Language::Clexer::toString(t.type()) + " " + lexeme(t);
}

std::string
TokenStream::line(const Token &t) const {
    if (t.type() == TOK_EOF)
        return "";
    size_t lineIdx = content_.lineIndex(t.begin_);
    if (const char *s = content_.lineChars(lineIdx)) {
        size_t n = content_.nCharacters(lineIdx);
        return std::string(s, n);
    } else {
        return "";
    }
}

bool
TokenStream::matches(const Token &token, const char *s2) const {
    size_t n1 = token.end_ - token.begin_;
    size_t n2 = strlen(s2);
    if (n1 != n2)
        return false;
    const char *s1 = content_.characters(token.begin_);
    return 0 == strncmp(s1, s2, n1);
}

void
TokenStream::emit(std::ostream &out, const std::string &fileName, const Token &token, const std::string &message) const {
    emit(out, fileName, token, token, token, message);
}

void
TokenStream::emit(std::ostream &out, const std::string &fileName, const Token &begin, const Token &locus, const Token &end,
                  const std::string &message) const {

    std::pair<size_t, size_t> loc1 = content_.location(begin.begin_);
    std::pair<size_t, size_t> loc2 = content_.location(locus.begin_);
    //std::pair<size_t, size_t> loc3 = content_.location(locus.end_);
    std::pair<size_t, size_t> loc4 = content_.location(end.end_);

    // Emit "NAME:LINE:COL: MESG" to show the beginning of the locus
    out <<fileName <<":" <<(loc2.first+1) <<":" <<(loc2.second+1) <<": " <<message <<"\n";

    // Emit context matched lines
    for (size_t lineIdx = loc1.first; lineIdx <= loc4.first; ++lineIdx) {
        const char *line = content_.lineChars(lineIdx);
        if (line) {
            // Line number right justified in a field of 7 characters
            std::string lineNumStr = boost::lexical_cast<std::string>(lineIdx+1);
            if (lineNumStr.size() > 7)
                lineNumStr = "+" + boost::lexical_cast<std::string>(lineIdx-loc1.first);
            if (lineNumStr.size() < 7)
                lineNumStr = std::string(7 - lineNumStr.size(), ' ') + lineNumStr;

            // The line from the source file
            std::string str(line, content_.nCharacters(lineIdx));
            boost::trim_right(str);
            out <<lineNumStr <<"|" <<str <<"\n";        // indentation preserves TABS when traditional stops are assumed

            // The matching part of the line is underlined with "~" characters except "^" is used at the start of the locus.
            out <<"       |";
            size_t col0 = content_.characterIndex(lineIdx);
            size_t colN = col0 + str.size();
            size_t cur = col0;

            // white space before first '~'
            if (col0 < begin.begin_) {
                size_t n = begin.begin_ - col0;
                out <<std::string(n, ' ');
                cur += n;
            }
            
            // '~' characters, up to '^' or EOL
            if (lineIdx < loc2.first) {
                size_t n = colN - cur;
                out <<std::string(n, '~');              // '~' to eol
                cur += n;
            } else if (lineIdx == loc2.first) {
                ASSERT_require(locus.begin_ >= cur);
                size_t n = locus.begin_ - cur;
                out <<std::string(n, '~');              // '~' to '^'
                cur += n;
            }

            // '^' at beginning of locus
            if (lineIdx == loc2.first) {
                out <<"^";
                ++cur;
            }
            
            // '~' characters, up to end of match or EOL
            if (lineIdx < loc4.first) {
                size_t n = colN - cur;
                out <<std::string(n, '~');              // '~' to eol
                cur += n;
            } else {
                ASSERT_require(lineIdx == loc4.first);
                ASSERT_require(end.end_ >= cur);
                size_t n = end.end_ - cur;
                out <<std::string(n, '~');              // '~' to end of match
                cur += n;
            }

            out <<"\n";
        }
    }
}

std::pair<size_t, size_t>
TokenStream::location(const Token &token) const {
    return content_.location(token.begin_);
}

void
TokenStream::scanString() {
    int q = content_.character(at_);
    ASSERT_require('\''==q || '"'==q);
    int c = content_.character(++at_);
    while (EOF != c && c != q) {
        if ('\\' == c)
            ++at_;                                      // skipping next char is sufficient
        c = content_.character(++at_);
    }
    ++at_;                                              // skip closing quote
}

void
TokenStream::makeNextToken() {
    if (!tokens_.empty() && tokens_.back().type() == TOK_EOF)
        return;
    while (isspace(content_.character(at_)))
           ++at_;
    int c = content_.character(at_);
    if (EOF == c) {
        tokens_.push_back(Token(TOK_EOF, at_, at_));
    } else if ('\'' == c || '"' == c) {
        size_t begin = at_;
        scanString();
        tokens_.push_back(Token('"'==c ? TOK_STRING : TOK_CHAR, begin, at_));
    } else if ('/' == c && '/' == content_.character(at_+1)) {
        at_ = content_.characterIndex(content_.lineIndex(at_) + 1);
        makeNextToken();
    } else if ('/' == c && '*' == content_.character(at_+1)) {
        at_ += 2;
        while (EOF != (c = content_.character(at_))) {
            if (content_.character(at_) == '*' && content_.character(at_+1) == '/') {
                at_ = at_ + 2;
                break;
            }
            ++at_;
        }
        makeNextToken();
    } else if (isalpha(c) || c=='_') {
        size_t begin = at_++;
        while (isalnum(c=content_.character(at_)) || '_'==c)
            ++at_;
        tokens_.push_back(Token(TOK_WORD, begin, at_));
    } else if ('('==c || '{'==c || '['==c) {
        ++at_;
        tokens_.push_back(Token(TOK_LEFT, at_-1, at_));
    } else if (')'==c || '}'==c || ']'==c) {
        ++at_;
        tokens_.push_back(Token(TOK_RIGHT, at_-1, at_));
    } else if (isdigit(c) || (('-'==c || '+'==c) && isdigit(content_.character(at_+1)))) {
        size_t begin = at_;
        if (!isdigit(c))
            ++at_;
        if ('0'==content_.character(at_) && 'x'==content_.character(at_+1)) {
            at_ += 2;
            while (isxdigit(content_.character(at_)))
                ++at_;
        } else if ('0'==content_.character(at_) && 'b'==content_.character(at_+1)) {
            at_ += 2;
            while (strchr("01", content_.character(at_)))
                ++at_;
        } else if ('0'==content_.character(at_)) {
            ++at_;
            while ((c=content_.character(at_)) >= '0' && c <= '7')
                ++at_;
        } else {
            ++at_;
            while (isdigit(content_.character(at_)))
                ++at_;
        }
        tokens_.push_back(Token(TOK_NUMBER, begin, at_));
    } else if ('#' == c) {
        size_t begin = at_;
        at_ = content_.characterIndex(content_.lineIndex(at_) + 1);
        while (at_>=2 && at_ < content_.nCharacters() && content_.character(at_-2)=='\\' && content_.character(at_-1)=='\n')
            at_ = content_.characterIndex(content_.lineIndex(at_) + 1);
        if (skipPreprocessorTokens_) {
            makeNextToken();
        } else {
            tokens_.push_back(Token(TOK_CPP, begin, at_));
        }
    } else if (('<' == c && content_.character(at_+1) == '<' && content_.character(at_+2) == '=') ||
               ('>' == c && content_.character(at_+1) == '>' && content_.character(at_+2) == '=') ||
               ('<' == c && content_.character(at_+1) == '=' && content_.character(at_+2) == '>') ||
               ('-' == c && content_.character(at_+1) == '>' && content_.character(at_+2) == '*')) {
        tokens_.push_back(Token(TOK_OTHER, at_, at_+3));
        at_ += 3;
    } else if ((content_.character(at_+1) == '=' && strchr("|&^*/%+-!<>=", c)) ||
               ('|' == c && content_.character(at_+1) == '|') ||
               ('&' == c && content_.character(at_+1) == '&') ||
               ('<' == c && content_.character(at_+1) == '<') ||
               ('>' == c && content_.character(at_+1) == '>') ||
               ('.' == c && content_.character(at_+1) == '*') ||
               ('+' == c && content_.character(at_+1) == '+') ||
               ('-' == c && content_.character(at_+1) == '-') ||
               ('-' == c && content_.character(at_+1) == '>') ||
               (':' == c && content_.character(at_+1) == ':')) {
        tokens_.push_back(Token(TOK_OTHER, at_, at_+2));
        at_ += 2;
    } else {
        tokens_.push_back(Token(TOK_OTHER, at_, at_+1));
        ++at_;
    }
}

} // namespace
} // namespace
} // namespace
