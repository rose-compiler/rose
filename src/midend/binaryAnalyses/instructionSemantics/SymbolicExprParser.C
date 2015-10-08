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

static boost::regex hexLiteralRe, signedDecimalLiteralRe;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      SyntaxError
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SymbolicExprParser::SyntaxError::SyntaxError(const std::string &mesg, const std::string &inputName,
                                             unsigned lineNumber, unsigned columnNumber)
    : std::runtime_error(mesg), inputName(inputName), lineNumber(lineNumber), columnNumber(columnNumber) {}

void
SymbolicExprParser::SyntaxError::print(std::ostream &out) const {
    if (!inputName.empty()) {
        out <<StringUtility::cEscape(inputName);
        if (lineNumber != 0)
            out <<":" <<lineNumber <<"." <<columnNumber;
        out <<": ";
    }
    if (what() && *what()) {
        out <<what();
    } else {
        out <<"syntax error";
    }
}

std::ostream&
operator<<(std::ostream &out, const SymbolicExprParser::SyntaxError &error) {
    error.print(out);
    return out;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Tokens
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
SymbolicExprParser::TokenStream::init() {
    hexLiteralRe = boost::regex("0x([0-9a-f]+)");
    signedDecimalLiteralRe = boost::regex("[-+]?\\d+");
}

const SymbolicExprParser::Token&
SymbolicExprParser::TokenStream::operator[](size_t idx) {
    fillTokenList(idx);
    return idx < tokens_.size() ? tokens_[idx] : endToken_;
}

void
SymbolicExprParser::TokenStream::shift(size_t n) {
    if (n >= tokens_.size()) {
        tokens_.clear();
    } else {
        for (size_t i=0; i+n < tokens_.size(); ++i)
            tokens_[i] = tokens_[i+n];
        tokens_.resize(tokens_.size() - n);
    }
}

int
SymbolicExprParser::TokenStream::nextCharacter() {
    if (EOF == readAhead_)
        readAhead_ = input_.get();
    return readAhead_;
}

int
SymbolicExprParser::TokenStream::consumeCharacter() {
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

void
SymbolicExprParser::TokenStream::consumeWhiteSpace() {
    while (isspace(nextCharacter()))
        consumeCharacter();
}

int
SymbolicExprParser::TokenStream::consumeEscapeSequence() {
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

void
SymbolicExprParser::TokenStream::consumeInlineComment() {
    ASSERT_require('<' == nextCharacter());
    unsigned startLine = lineNumber_;
    unsigned startColumn = columnNumber_;
    unsigned depth = 0;
    while (1) {
        switch (nextCharacter()) {
            case EOF:
                throw SymbolicExprParser::SyntaxError("end of comment not found before end of input",
                                                      name_, startLine, startColumn);
            case '<':
                consumeCharacter();
                ++depth;
                break;
            case '>':
                consumeCharacter();
                if (0 == --depth)
                    return;
                break;
            case '\'':
                consumeEscapeSequence();
                break;
            default:
                consumeCharacter();
                break;
        }
    }
}

void
SymbolicExprParser::TokenStream::consumeWhiteSpaceAndComments() {
    while (isspace(nextCharacter()) || nextCharacter() == '<') {
        if (isspace(nextCharacter())) {
            consumeWhiteSpace();
        } else {
            consumeInlineComment();
        }
    }
}

std::string
SymbolicExprParser::TokenStream::consumeTerm() {
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

size_t
SymbolicExprParser::TokenStream::consumeWidth() {
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

SymbolicExprParser::Token
SymbolicExprParser::TokenStream::scan() {
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
            if (boost::regex_match(s, matches, hexLiteralRe)) {
                size_t nbits = consumeWidth();
                Sawyer::Container::BitVector bv(nbits);
                bv.fromHex(matches.str(1));         // hex digits without leading "0x"
                return Token(bv, s, startLine, startColumn);
            } else if (boost::regex_match(s, signedDecimalLiteralRe)) {
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

void
SymbolicExprParser::TokenStream::fillTokenList(size_t idx) {
    for (size_t i = tokens_.size(); i <= idx; ++i) {
        Token token = scan();
        if (Token::NONE == token.type())
            return;
        tokens_.push_back(token);
    }
}


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
SymbolicExprParser::parse(const std::string &input, const std::string &inputName) {
    std::istringstream stream(input);
    return parse(stream, inputName);
}

TreeNodePtr
SymbolicExprParser::parse(std::istream &input, const std::string &inputName, unsigned lineNumber, unsigned columnNumber) {
    TokenStream tokens(input, inputName, lineNumber, columnNumber);
    return parse(tokens);
}

struct PartialInternalNode {
    InsnSemanticsExpr::Operator op;
    size_t nBits;
    std::vector<TreeNodePtr> operands;
    SymbolicExprParser::Token ltparen;                  // left paren token needed for error message position info

    PartialInternalNode(InsnSemanticsExpr::Operator op, size_t nBits, const SymbolicExprParser::Token &ltparen)
        : op(op), nBits(nBits), ltparen(ltparen) {}
};

TreeNodePtr
SymbolicExprParser::parse(TokenStream &tokens) {
    std::vector<PartialInternalNode> stack;
    while (tokens[0].type() != Token::NONE) {
        switch (tokens[0].type()) {
            case Token::LTPAREN: {
                if (tokens[1].type()!=Token::SYMBOL)
                    throw tokens[0].syntaxError("expected operator after left paren", tokens.name());
                if (!ops_.exists(tokens[1].lexeme()))
                    throw tokens[1].syntaxError("unknown operator name", tokens.name());
                stack.push_back(PartialInternalNode(ops_[tokens[1].lexeme()], tokens[1].width(), tokens[0]));
                tokens.shift(2);
                break;
            }
            case Token::SYMBOL: {
                LeafNodePtr leaf;
                boost::smatch matches;
                if (0 == tokens[0].width()) {
                    if (tokens[0].lexeme() == "...") {
                        throw tokens[0].syntaxError("input is an abbreviated expression; parts are missing", tokens.name());
                    } else {
                        throw tokens[0].syntaxError("variable \""+StringUtility::cEscape(tokens[0].lexeme())+"\" must have "
                                                    "a non-zero width specified", tokens.name());
                    }
                }
                if (boost::regex_match(tokens[0].lexeme(), matches, boost::regex("v(\\d+)"))) {
                    uint64_t varId = rose_strtoull(matches.str(1).c_str(), NULL, 10);
                    leaf = LeafNode::create_existing_variable(tokens[0].width(), varId);
                } else {
                    leaf = LeafNode::create_variable(tokens[0].width(), tokens[0].lexeme());
                }
                tokens.shift();
                if (stack.empty())
                    return leaf;
                stack.back().operands.push_back(leaf);
                break;
            }
            case Token::BITVECTOR: {
                LeafNodePtr leaf = LeafNode::create_constant(tokens[0].bits());
                tokens.shift(1);
                if (stack.empty())
                    return leaf;
                stack.back().operands.push_back(leaf);
                break;
            }
            case Token::RTPAREN: {
                tokens.shift();
                if (stack.empty())
                    throw tokens[0].syntaxError("unexpected right parenthesis", tokens.name());
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
        throw SyntaxError("empty input", tokens.name(), tokens.lineNumber(), tokens.columnNumber());
    throw stack.back().ltparen.syntaxError("expression not closed before end of input", tokens.name());
}

} // namespace
} // namespace
