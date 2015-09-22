#include <sage3basic.h>

#include <SymbolicExprParser.h>
#include <InsnSemanticsExpr.h>
#include <Sawyer/BitVector.h>
#include <Sawyer/Map.h>
#include <integerOps.h>
#include <rose_strtoull.h>

#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

using namespace rose::BinaryAnalysis::InsnSemanticsExpr;

namespace rose {
namespace BinaryAnalysis {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      SyntaxError
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SymbolicExprParser::SyntaxError::SyntaxError(const std::string &mesg, const std::string &filename,
                                             unsigned lineNumber, unsigned columnNumber)
    : std::runtime_error(filename + ":" + boost::lexical_cast<std::string>(lineNumber) + "." +
                         boost::lexical_cast<std::string>(columnNumber) + ": " + mesg) {}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Token
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Represents one token from the input, including the token's string representation and location.
struct Token {
    enum Type { NONE = 0, LTPAREN, RTPAREN, SYMBOL, BITVECTOR };
    Type type;
    std::string string;                                 // lexeme
    size_t width;                                       // width of value in bits
    Sawyer::Container::BitVector bits;                  // bits representing constant terms
    unsigned lineNumber, columnNumber;                  // for start of token

    Token()
        : type(NONE), width(0), lineNumber(0), columnNumber(0) {}
    Token(Type type, size_t width, const std::string &string, unsigned lineNumber, unsigned columnNumber)
        : type(type), string(string), width(width), lineNumber(lineNumber), columnNumber(columnNumber) {}
    Token(const Sawyer::Container::BitVector &bv, const std::string &string, unsigned lineNumber, unsigned columnNumber)
        : type(BITVECTOR), string(string), width(bv.size()), bits(bv), lineNumber(lineNumber), columnNumber(columnNumber) {}

    SymbolicExprParser::SyntaxError syntaxError(const std::string &mesg, const std::string &name) const {
        return SymbolicExprParser::SyntaxError(mesg, name, lineNumber, columnNumber);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Tokens
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// A vector-like container of tokens. This vector is conceptually infinite size padded with EOF tokens, although in practice it
// usually holds only one or two tokens at a time.  Tokens are consumed by shifting them off the beginning of the vector.
class Tokens {
    std::istream &input_;
    std::string name_;
    unsigned lineNumber_, columnNumber_;
    const Token endToken_;
    std::vector<Token> tokens_;
    int readAhead_;
    boost::regex hexLiteralRe_, signedDecimalLiteralRe_;

public:
    Tokens(std::istream &input, const std::string &name, unsigned lineNumber, unsigned columnNumber)
        : input_(input), name_(name), lineNumber_(lineNumber), columnNumber_(columnNumber), readAhead_(EOF) {
        hexLiteralRe_ = boost::regex("0x([0-9a-f]+)");
        signedDecimalLiteralRe_ = boost::regex("[-+]?\\d+");
    }

    // Return the specified token without consuming it.
    const Token& operator[](size_t idx) {
        fillTokenList(idx);
        return idx < tokens_.size() ? tokens_[idx] : endToken_;
    }

    // Consume the specified number of tokens.
    void shift(size_t n=1) {
        if (n >= tokens_.size()) {
            tokens_.clear();
        } else {
            for (size_t i=0; i+n < tokens_.size(); ++i)
                tokens_[i] = tokens_[i+n];
            tokens_.resize(tokens_.size() - n);
        }
    }

private:
    // Return the next character of input (or EOF) without consuming it.
    int nextCharacter() {
        if (EOF == readAhead_)
            readAhead_ = input_.get();
        return readAhead_;
    }

    // Advance over the next character, adjusting line and column information.  Returns the character consumed.
    int consumeCharacter() {
        switch (readAhead_) {
            case EOF:
                break;
            case '\n':
                ++lineNumber_;
                columnNumber_ = 0;
                break;
            default:
                ++columnNumber_;
                break;
        }
        int retval = readAhead_;
        readAhead_ = EOF;
        return retval;
    }

    // Skip over characters until a non-white-space character is encountered.
    void consumeWhiteSpace() {
        while (isspace(nextCharacter()))
            consumeCharacter();
    }

    // Skip over an escape sequence and return the escaped character.
    int consumeEscapeSequence() {
        ASSERT_require('\\' == nextCharacter());
        unsigned startLine = lineNumber_;
        unsigned startColumn = columnNumber_;
        consumeCharacter();
        switch (nextCharacter()) {
            case EOF:
                throw SymbolicExprParser::SyntaxError("end of input reached in an escape sequence",
                                                      name_, startLine, startColumn);
            case 'a':
                consumeCharacter();
                return '\a';
            case 'b':
                consumeCharacter();
                return '\b';
            case 't':
                consumeCharacter();
                return '\t';
            case 'n':
                consumeCharacter();
                return '\n';
            case 'v':
                consumeCharacter();
                return '\v';
            case 'f':
                consumeCharacter();
                return '\f';
            case 'r':
                consumeCharacter();
                return '\r';
            default:
                if (strchr("01234567", nextCharacter())) {
                    int c = 0;
                    for (size_t i=0; i<3; ++i) {
                        if (EOF == nextCharacter() || !strchr("01234567", nextCharacter())) {
                            throw SymbolicExprParser::SyntaxError("octal escape sequences need three digits",
                                                                  name_, startLine, startColumn);
                        }
                        c = (8 * c) + (nextCharacter() - '0');
                        consumeCharacter();
                    }
                    return c;
                }
                break;
        }
        return consumeCharacter();                      // escaping any other character just returns that character
    }
    
    // Skip over angle-bracket comments like <this is a comment>. Any right angle brackets that are part of the comment must be
    // escaped.
    void consumeInlineComment() {
        ASSERT_require('<' == nextCharacter());
        unsigned startLine = lineNumber_;
        unsigned startColumn = columnNumber_;
        while (1) {
            switch (nextCharacter()) {
                case EOF:
                    throw SymbolicExprParser::SyntaxError("end of comment not found before end of input",
                                                          name_, startLine, startColumn);
                case '>':
                    consumeCharacter();
                    return;
                case '\'':
                    consumeEscapeSequence();
                    break;
                default:
                    consumeCharacter();
                    break;
            }
        }
    }

    // Skip over white space and/or inline comments.
    void consumeWhiteSpaceAndComments() {
        while (isspace(nextCharacter()) || nextCharacter() == '<') {
            if (isspace(nextCharacter())) {
                consumeWhiteSpace();
            } else {
                consumeInlineComment();
            }
        }
    }
    
    // Parse and consume a term. Terms contain any characters except unescaped parentheses, angle brackets, square brackets,
    // and white space. This includes symbols and numeric constants.
    std::string consumeTerm() {
        ASSERT_require(EOF!=nextCharacter());
        std::string retval;
        while (EOF != nextCharacter() && !isspace(nextCharacter()) && !strchr("()<>[]", nextCharacter())) {
            if ('\\' == nextCharacter()) {
                retval += (char)consumeEscapeSequence();
            } else {
                retval += (char)consumeCharacter();
            }
        }
        ASSERT_forbid2(retval.empty(), "term expected");
        return retval;
    }

    // Parse and consume a width specification, a decimal number is square brackets.
    size_t consumeWidth() {
        consumeWhiteSpaceAndComments();
        unsigned startLine = lineNumber_;
        unsigned startColumn = columnNumber_;
        if (nextCharacter() != '[') {
            throw SymbolicExprParser::SyntaxError("expected '[' to start a width specification",
                                                  name_, startLine, startColumn);
        }
        consumeCharacter();
        consumeWhiteSpaceAndComments();
        std::string s;
        while (isdigit(nextCharacter()))
            s += consumeCharacter();
        consumeWhiteSpaceAndComments();
        if (s.empty()) {
            throw SymbolicExprParser::SyntaxError("expected decimal integer in width specification",
                                                  name_, startLine, startColumn);
        }
        if (nextCharacter() == EOF)
            throw SymbolicExprParser::SyntaxError("end of input reached while parsing width", name_, startLine, startColumn);
        if (nextCharacter() != ']')
            throw SymbolicExprParser::SyntaxError("missing closing ']' in width specification", name_, startLine, startColumn);
        consumeCharacter();
        return boost::lexical_cast<size_t>(s);
    }
    
    // Parse the next token (consuming its characters) and return it. Returns the special NONE token at end-of-input.
    Token scan() {
        consumeWhiteSpaceAndComments();
        unsigned startLine = lineNumber_;
        unsigned startColumn = columnNumber_;
        switch (nextCharacter()) {
            case EOF:
                return Token();
            case '<':
                ASSERT_not_reachable("'<' should have been consumed by consumeWhiteSpaceAndComments");
            case '>':
                throw SymbolicExprParser::SyntaxError("found '>' outside inline comment", name_, startLine, startColumn);
            case '(':
                consumeCharacter();
                return Token(Token::LTPAREN, 0, "(", startLine, startColumn);
            case ')':
                consumeCharacter();
                return Token(Token::RTPAREN, 0, ")", startLine, startColumn);
            case '[':
                throw SymbolicExprParser::SyntaxError("unexpected width specification", name_, startLine, startColumn);
            case ']':
                throw SymbolicExprParser::SyntaxError("found ']' outside width specification", name_, startLine, startColumn);
            default: {
                std::string s = consumeTerm();
                boost::smatch matches;
                if (boost::regex_match(s, matches, hexLiteralRe_)) {
                    size_t nbits = consumeWidth();
                    Sawyer::Container::BitVector bv(nbits);
                    bv.fromHex(matches.str(1));         // hex digits without leading "0x"
                    return Token(bv, s, startLine, startColumn);
                } else if (boost::regex_match(s, signedDecimalLiteralRe_)) {
                    int n = boost::lexical_cast<int>(s);
                    consumeWhiteSpaceAndComments();
                    size_t nbits = nextCharacter() == '[' ? consumeWidth() : 8*sizeof(int);
                    Sawyer::Container::BitVector bv(nbits);
                    bv.fromInteger(IntegerOps::signExtend2((uint64_t)n, 8*sizeof(int), 64));
                    return Token(bv, s, startLine, startColumn);
                } else {
                    consumeWhiteSpaceAndComments();
                    size_t nbits = nextCharacter() == '[' ? consumeWidth() : 0;
                    return Token(Token::SYMBOL, nbits, s, startLine, startColumn);
                }
            }
        }
        return Token();
    }

    // Try to fill the token vector so it contains tokens up through at least [idx]
    void fillTokenList(size_t idx) {
        for (size_t i = tokens_.size(); i <= idx; ++i) {
            Token token = scan();
            if (Token::NONE == token.type)
                return;
            tokens_.push_back(token);
        }
    }
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      SymbolicExprParser
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
SymbolicExprParser::init() {
    ops_.insert("add",          OP_ADD);
    ops_.insert("and",          OP_AND);
    ops_.insert("asr",          OP_ASR);
    ops_.insert("bv-and",       OP_BV_AND);
    ops_.insert("bv-or",        OP_BV_OR);
    ops_.insert("bv-xor",       OP_BV_XOR);
    ops_.insert("concat",       OP_CONCAT);
    ops_.insert("eq",           OP_EQ);
    ops_.insert("extract",      OP_EXTRACT);
    ops_.insert("invert",       OP_INVERT);
    ops_.insert("ite",          OP_ITE);
    ops_.insert("lssb",         OP_LSSB);
    ops_.insert("mssb",         OP_MSSB);
    ops_.insert("ne",           OP_NE);
    ops_.insert("negate",       OP_NEGATE);
    ops_.insert("noop",         OP_NOOP);
    ops_.insert("or",           OP_OR);
    ops_.insert("read",         OP_READ);
    ops_.insert("rol",          OP_ROL);
    ops_.insert("ror",          OP_ROR);
    ops_.insert("sdiv",         OP_SDIV);
    ops_.insert("sextend",      OP_SEXTEND);
    ops_.insert("sge",          OP_SGE);
    ops_.insert("sgt",          OP_SGT);
    ops_.insert("shl0",         OP_SHL0);
    ops_.insert("shl1",         OP_SHL1);
    ops_.insert("shr0",         OP_SHR0);
    ops_.insert("shr1",         OP_SHR1);
    ops_.insert("sle",          OP_SLE);
    ops_.insert("slt",          OP_SLT);
    ops_.insert("smod",         OP_SMOD);
    ops_.insert("smul",         OP_SMUL);
    ops_.insert("udiv",         OP_UDIV);
    ops_.insert("uextend",      OP_UEXTEND);
    ops_.insert("uge",          OP_UGE);
    ops_.insert("ugt",          OP_UGT);
    ops_.insert("ule",          OP_ULE);
    ops_.insert("ult",          OP_ULT);
    ops_.insert("umod",         OP_UMOD);
    ops_.insert("umul",         OP_UMUL);
    ops_.insert("write",        OP_WRITE);
    ops_.insert("zerop",        OP_ZEROP);
}

TreeNodePtr
SymbolicExprParser::parse(const std::string &input) {
    std::istringstream stream(input);
    return parse(stream, "string");
}

struct PartialInternalNode {
    InsnSemanticsExpr::Operator op;
    size_t nBits;
    std::vector<TreeNodePtr> operands;
    Token ltparen;                                      // left paren token needed for error message position info

    PartialInternalNode(InsnSemanticsExpr::Operator op, size_t nBits, const Token &ltparen)
        : op(op), nBits(nBits), ltparen(ltparen) {}
};

TreeNodePtr
SymbolicExprParser::parse(std::istream &input, const std::string &inputName, unsigned lineNumber, unsigned columnNumber) {
    Tokens tokens(input, inputName, lineNumber, columnNumber);
    std::vector<PartialInternalNode> stack;
    while (tokens[0].type != Token::NONE) {
        switch (tokens[0].type) {
            case Token::LTPAREN: {
                if (tokens[1].type!=Token::SYMBOL)
                    throw tokens[0].syntaxError("expected operator after left paren", inputName);
                if (!ops_.exists(tokens[1].string))
                    throw tokens[1].syntaxError("unknown operator name", inputName);
                stack.push_back(PartialInternalNode(ops_[tokens[1].string], tokens[1].width, tokens[0]));
                tokens.shift(2);
                break;
            }
            case Token::SYMBOL: {
                LeafNodePtr leaf;
                boost::smatch matches;
                if (0 == tokens[0].width) {
                    throw tokens[0].syntaxError("variable \""+StringUtility::cEscape(tokens[0].string)+"\" must have "
                                                "a non-zero width specified", inputName);
                }
                if (boost::regex_match(tokens[0].string, matches, boost::regex("v(\\d+)"))) {
                    uint64_t varId = rose_strtoull(matches.str(1).c_str(), NULL, 10);
                    leaf = LeafNode::create_existing_variable(tokens[0].width, varId);
                } else {
                    leaf = LeafNode::create_variable(tokens[0].width, tokens[0].string);
                }
                tokens.shift();
                if (stack.empty())
                    return leaf;
                stack.back().operands.push_back(leaf);
                break;
            }
            case Token::BITVECTOR: {
                LeafNodePtr leaf = LeafNode::create_constant(tokens[0].bits);
                tokens.shift(1);
                if (stack.empty())
                    return leaf;
                stack.back().operands.push_back(leaf);
                break;
            }
            case Token::RTPAREN: {
                tokens.shift();
                if (stack.empty())
                    throw tokens[0].syntaxError("unexpected right parenthesis", inputName);
                TreeNodePtr inode = InternalNode::create(stack.back().nBits, stack.back().op, stack.back().operands);
                stack.pop_back();
                if (stack.empty())
                    return inode;
                stack.back().operands.push_back(inode);
                break;
            }
            case Token::NONE:
                ASSERT_not_reachable("end of input");
        }
    }
    if (stack.empty())
        throw SyntaxError("empty input", inputName, lineNumber, columnNumber);
    throw stack.back().ltparen.syntaxError("expression not closed before end of input", inputName);
}

} // namespace
} // namespace
