#include <sage3basic.h>

#include <BinarySymbolicExprParser.h>
#include <Sawyer/BitVector.h>
#include <Sawyer/Map.h>
#include <integerOps.h>
#include <rose_strtoull.h>

#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

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
    if (EOF == nextCharacter()) {
    } else if (isalpha(nextCharacter()) || nextCharacter() == '_') {
        while (EOF != nextCharacter()) {
            if (isalpha(nextCharacter()) || isdigit(nextCharacter()) || nextCharacter() == '_') {
                retval += (char)consumeCharacter();
            } else if (nextCharacter() == '\\') {
                retval += (char)consumeEscapeSequence();
            } else {
                break;
            }
        }
    } else {
        while (EOF != nextCharacter() && !isspace(nextCharacter()) && !strchr("()<>[]", nextCharacter())) {
            if ('\\' == nextCharacter()) {
                retval += (char)consumeEscapeSequence();
            } else {
                retval += (char)consumeCharacter();
            }
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

// Throws an exception for functions named "..."
class AbbreviatedOperator: public SymbolicExprParser::OperatorExpansion {
public:
    static Ptr instance() {
        return Ptr(new AbbreviatedOperator);            // undocumented
    }
    SymbolicExpr::Ptr operator()(const SymbolicExprParser::Token &op, const SymbolicExpr::Nodes &args) {
        if (op.lexeme() == "...")
            throw op.syntaxError("input is an abbreviated expression; parts are missing");
        return SymbolicExpr::Ptr();
    }
};

// Throws an exception for atoms named "..."
class AbbreviatedAtom: public SymbolicExprParser::AtomExpansion {
public:
    static Ptr instance() {
        return Ptr(new AbbreviatedAtom);                // undocumented
    }
    SymbolicExpr::Ptr operator()(const SymbolicExprParser::Token &symbol) {
        if (symbol.lexeme() == "...")
            throw symbol.syntaxError("input is an abbreviated expression; parts are missing");
        return SymbolicExpr::Ptr();
    }
};

// Generates symbolic expressions for the SMT operators
class SmtOperators: public SymbolicExprParser::OperatorExpansion {
protected:
    Sawyer::Container::Map<std::string, SymbolicExpr::Operator> ops_;

    SmtOperators() {
        std::string doc;
        ops_.insert("add",          SymbolicExpr::OP_ADD);
        doc += "@named{add}"
               "{Adds operands together. All operands must have the same width, which is also the width of the result.}";

        ops_.insert("and",          SymbolicExpr::OP_AND);
        doc += "@named{and}"
               "{Boolean AND operation. All operands (one or more) are Boolean values, such as relational operators. For "
               "bit-wise AND use the \"bv-and\" operator.}";

        ops_.insert("asr",          SymbolicExpr::OP_ASR);
        doc += "@named{asr}"
               "{Arithmetic shift right. The second operand is interpreted as a signed value which is shifted right by "
               "the unsigned first argument. The result has the same width as the second operand.}";

        ops_.insert("bv-and",       SymbolicExpr::OP_BV_AND);
        doc += "@named{bv-and}"
               "{Bit-wise AND operation. All operands (one or more) must be the same width, which is also the width of "
               "the result.}";

        ops_.insert("bv-or",        SymbolicExpr::OP_BV_OR);
        doc += "@named{bv-or}"
               "{Bit-wise OR operation. All operands (one or more) must be the same width, which is also the width of "
               "the result.}";

        ops_.insert("bv-xor",       SymbolicExpr::OP_BV_XOR);
        doc += "@named{bv-xor}"
               "{Bit-wise XOR operation. All operands (one or more) must be the same width, which is also the width of "
               "the result.}";

        ops_.insert("concat",       SymbolicExpr::OP_CONCAT);
        doc += "@named{concat}"
               "{Concatenate operands to produce a result whose width is the sum of the widths of the operands. The "
               "operands are in bit-endian order: the first operand becomes the most significant bits of the result and "
               "the last operand becomes the least significant bits.}";

        ops_.insert("eq",           SymbolicExpr::OP_EQ);
        doc += "@named{eq}"
               "{Equality comparison. Takes two operands that must be the same width and returns a Boolean.}";

        ops_.insert("extract",      SymbolicExpr::OP_EXTRACT);
        doc += "@named{extract}"
               "{Extract part of a value. The bits of the third operand are numbered from zero (least significant) through "
               "N-1 (most significant) and those bits specified by the first operand (low bit) and second operand "
               "(one past high bit) are returned.  The width of the return value is the difference between the second "
               "and first operands, both of which must have constant numeric values.}";

        ops_.insert("invert",       SymbolicExpr::OP_INVERT);
        doc += "@named{invert}"
               "{Bit-wise invert. This operator takes one operand and returns a value having the same width but with "
               "each bit flipped.}";

        ops_.insert("ite",          SymbolicExpr::OP_ITE);
        doc += "@named{ite}"
               "{If-then-else.  Takes a Boolean condition (first operand) and two alternatives (second and third operands). "
               "If the condition is true, then evaluates to the second operand, otherwise the third operand. The second "
               "and third operand must have the same width, which is also the width of the result.}";

        ops_.insert("lssb",         SymbolicExpr::OP_LSSB);
        doc += "@named{lssb}"
               "{Least significant set bit.  Takes one operand and evaluates to a result having the same width. The result "
               "is the lowest index (least significant bit is zero) of a bit of the operand which is set.  Evaluates to "
               "zero if no bits are set.}";            // odd, but same behavior as Yices.

        ops_.insert("mssb",         SymbolicExpr::OP_MSSB);
        doc += "@named{mssb}"
               "{Most significant set bit.  Takes one operand and evaluates to a result having the same width. The result "
               "is the highest index (least significant bit is zero) of a bit of the operand which is set.  Evaluates to "
               "zero if no bits are set.}";            // odd, but same behavior as Yices.

        ops_.insert("ne",           SymbolicExpr::OP_NE);
        doc += "@named{ne}"
               "{Inequality comparison. Takes two operands that must be the same width and returns a Boolean.}";

        ops_.insert("negate",       SymbolicExpr::OP_NEGATE);
        doc += "@named{negate}"
               "{Evauates to the two's complement of the single operand. The result is the same width as the operand.}";

        ops_.insert("or",           SymbolicExpr::OP_OR);
        doc += "@named{or}"
               "{Boolean OR operation. All operands (one or more) are Boolean values, such as relational operators. For "
               "bit-wise OR use the \"bv-or\" operator.}";
               
        ops_.insert("read",         SymbolicExpr::OP_READ);
        doc += "@named{read}"
               "{Memory read operation. Indicates a value read from a memory state. This operator expects two operands: the "
               "memory state, and the address from which a value is read. The address must have the same width as the "
               "memory state's domain, and the result has the same width as the memory state's range (usually eight).}";

        ops_.insert("rol",          SymbolicExpr::OP_ROL);
        doc += "@named{rol}"
               "{Rotate left.  Rotates the bits of the second operand left by the amount specified in the first operand. "
               "The first operand must be a numeric constant interpreted as unsigned modulo the width of the second "
               "operand.  Bits shifted off the left side of the second operand are introduced into the right side.}";

        ops_.insert("ror",          SymbolicExpr::OP_ROR);
        doc += "@named{ror}"
               "{Rotate right.  Rotates the bits of the second operand right by the amount specified in the first operand. "
               "The first operand must be a numeric constant interpreted as unsigned modulo the width of the second "
               "operand.  Bits shifted off the right side of the second operand are introduced into the left side.}";

        ops_.insert("sdiv",         SymbolicExpr::OP_SDIV);
        doc += "@named{sdiv}"
               "{Signed division. Divides the first operand by the second. Both operands are interpreted as signed "
               "values, but they need not have the same width. The result width is the same as the width of the first "
               "operand.}";
        
        ops_.insert("sextend",      SymbolicExpr::OP_SEXTEND);
        doc += "@named{sextend}"
               "{Sign extends the second operand so it has the width specified in the first operand. The first operand "
               "must be an unsigned numeric constant not less than the width of the second operand.}";

        ops_.insert("sge",          SymbolicExpr::OP_SGE);
        doc += "@named{sge}"
               "{Signed greater-than or equal comparison. Takes two operands that must be the same width and returns a "
               "Boolean.}";

        ops_.insert("sgt",          SymbolicExpr::OP_SGT);
        doc += "@named{sgt}"
               "{Signed greater-than comparison. Takes two operands that must be the same width and returns a Boolean.}";

        ops_.insert("shl0",         SymbolicExpr::OP_SHL0);
        doc += "@named{shl0}"
               "{Shift left introducing zeros.  Shifts the bits of the second operand left by the amount specified in the "
               "first operand. The first operand must be a numeric constant interpreted as unsigned modulo the width of the "
               "second operand.  Bits introduced into the right side are cleared.}";

        ops_.insert("shl1",         SymbolicExpr::OP_SHL1);
        doc += "@named{shl1}"
               "{Shift left introducing ones.  Shifts the bits of the second operand left by the amount specified in the "
               "first operand. The first operand must be a numeric constant interpreted as unsigned modulo the width of the "
               "second operand.  Bits introduced into the right side are set.}";

        ops_.insert("shr0",         SymbolicExpr::OP_SHR0);
        doc += "@named{shr0}"
               "{Shift right introducing zeros.  Shifts the bits of the second operand right by the amount specified in the "
               "first operand. The first operand must be a numeric constant interpreted as unsigned modulo the width of the "
               "second operand.  Bits introduced into the left side are cleared.}";

        ops_.insert("shr1",         SymbolicExpr::OP_SHR1);
        doc += "@named{shr1}"
               "{Shift right introducing ones.  Shifts the bits of the second operand right by the amount specified in the "
               "first operand. The first operand must be a numeric constant interpreted as unsigned modulo the width of the "
               "second operand.  Bits introduced into the left side are set.}";

        ops_.insert("sle",          SymbolicExpr::OP_SLE);
        doc += "@named{sle}"
               "{Signed less-than or equal comparison. Takes two operands that must be the same width and returns a "
               "Boolean.}";

        ops_.insert("slt",          SymbolicExpr::OP_SLT);
        doc += "@named{slt}"
               "{Signed less-than comparison. Takes two operands that must be the same width and returns a Boolean.}";

        ops_.insert("smod",         SymbolicExpr::OP_SMOD);
        doc += "@named{smod}"
               "{Signed modulo. Computes the first operand modulo the second. Both operands are interpreted as signed "
               "values, but they need not have the same width. The result width is the same as the width of the second "
               "operand.}";

        ops_.insert("smul",         SymbolicExpr::OP_SMUL);
        doc += "@named{smul}"
               "{Signed multiply. Computes the product of the operands (one or more). The operands are interpreted as signed "
               "values, but they need not have the same width. The result width is the sum of the operand widthds}";

        ops_.insert("udiv",         SymbolicExpr::OP_UDIV);
        doc += "@named{udiv}"
               "{Unsigned division. Divides the first operand by the second. Both operands are interpreted as unsigned "
               "values, but they need not have the same width. The result width is the same as the width of the first "
               "operand.}";

        ops_.insert("uextend",      SymbolicExpr::OP_UEXTEND);
        doc += "@named{uextend}"
               "{Extends or truncates the second operand so it has the width specified in the first operand. The first operand "
               "must be an unsigned numeric constant.}";

        ops_.insert("uge",          SymbolicExpr::OP_UGE);
        doc += "@named{uge}"
               "{Unsigned greater-than or equal comparison. Takes two operands that must be the same width and returns a "
               "Boolean.}";

        ops_.insert("ugt",          SymbolicExpr::OP_UGT);
        doc += "@named{ugt}"
               "{Unsigned greater-than comparison. Takes two operands that must be the same width and returns a Boolean.}";

        ops_.insert("ule",          SymbolicExpr::OP_ULE);
        doc += "@named{ule}"
               "{Unsigned less-than or equal comparison. Takes two operands that must be the same width and returns a "
               "Boolean.}";

        ops_.insert("ult",          SymbolicExpr::OP_ULT);
        doc += "@named{ult}"
               "{Unsigned less-than comparison. Takes two operands that must be the same width and returns a Boolean.}";

        ops_.insert("umod",         SymbolicExpr::OP_UMOD);
        doc += "@named{umod}"
               "{Unsigned modulo. Computes the first operand modulo the second. Both operands are interpreted as unsigned "
               "values, but they need not have the same width. The result width is the same as the width of the second "
               "operand.}";

        ops_.insert("umul",         SymbolicExpr::OP_UMUL);
        doc += "@named{umul}"
               "{Unsigned multiply. Computes the product of the operands (one or more). The operands are interpreted as "
               "unsigned values, but they need not have the same width. The result width is the sum of the operand widthds}";

        ops_.insert("write",        SymbolicExpr::OP_WRITE);
        doc += "@named{write}"
               "{Memory write operation. Indicates writing a value to a memory state. This operator expects three operands: the"
               "memory state, the address to which the value is written, and the value to write. The address must have the "
               "same width as the memory state's domain, and the value must have the same width as the memory state's range "
               "(usually eight). The result of this expression is a new memory state.}";

        ops_.insert("zerop",        SymbolicExpr::OP_ZEROP);
        doc += "@named{zerop}"
               "{Equal to zero.  The result is a Boolean value that is true when the single operand is equal to zero and "
               "false if not zero.}";

        title("Named SMT operators");
        docString(doc);
    }

public:
    static Ptr instance() {
        return Ptr(new SmtOperators);
    }

    virtual SymbolicExpr::Ptr operator()(const SymbolicExprParser::Token &op, const SymbolicExpr::Nodes &args) ROSE_OVERRIDE {
        if (!ops_.exists(op.lexeme()))
            return SymbolicExpr::Ptr();
        return SymbolicExpr::Interior::create(op.width(), ops_[op.lexeme()], args);
    }
};

// Creates symbolic expressions using more C-like operator names
class COperators: public SymbolicExprParser::OperatorExpansion {
protected:
    Sawyer::Container::Map<std::string, SymbolicExpr::Operator> ops_;

    COperators() {
        std::string doc;
        ops_.insert("+",        SymbolicExpr::OP_ADD);
        doc += "@named{+}"
               "{Adds operands together. All operands must have the same width, which is also the width of the result.}";

        ops_.insert("&&",       SymbolicExpr::OP_AND);
        doc += "@named{&&}"
               "{Boolean AND operation. All operands (one or more) are Boolean values, such as relational operators. For "
               "bit-wise AND use the \"bv-and\" operator.}";

        ops_.insert("&",        SymbolicExpr::OP_BV_AND);
        doc += "@named{&}"
               "{Bit-wise AND operation. All operands (one or more) must be the same width, which is also the width of "
               "the result.}";

        ops_.insert("|",        SymbolicExpr::OP_BV_OR);
        doc += "@named{|}"
               "{Bit-wise OR operation. All operands (one or more) must be the same width, which is also the width of "
               "the result.}";

        ops_.insert("^",        SymbolicExpr::OP_BV_XOR);
        doc += "@named{^}"
               "{Bit-wise XOR operation. All operands (one or more) must be the same width, which is also the width of "
               "the result.}";

        ops_.insert("==",       SymbolicExpr::OP_EQ);
        doc += "@named{==}"
               "{Equality comparison. Takes two operands that must be the same width and returns a Boolean.}";

        ops_.insert("~",        SymbolicExpr::OP_INVERT);
        doc += "@named{~}"
               "{Bit-wise invert. This operator takes one operand and returns a value having the same width but with "
               "each bit flipped.}";

        ops_.insert("?",        SymbolicExpr::OP_ITE);
        doc += "@named{?}"
               "{If-then-else.  Takes a Boolean condition (first operand) and two alternatives (second and third operands). "
               "If the condition is true, then evaluates to the second operand, otherwise the third operand. The second "
               "and third operand must have the same width, which is also the width of the result.}";

        ops_.insert("!=",       SymbolicExpr::OP_NE);
        doc += "@named{!=}"
               "{Inequality comparison. Takes two operands that must be the same width and returns a Boolean.}";

        ops_.insert("-",        SymbolicExpr::OP_NEGATE);
        doc += "@named{-}"
               "{Evauates to the two's complement of the single operand. The result is the same width as the operand.}";

        ops_.insert("||",       SymbolicExpr::OP_OR);
        doc += "@named{||}"
               "{Boolean OR operation. All operands (one or more) are Boolean values, such as relational operators. For "
               "bit-wise OR use the \"bv-or\" operator.}";
               
        ops_.insert("<<",       SymbolicExpr::OP_SHL0); // requires escapes since '<' introduces a comment
        doc += "@named{<<}"
               "{Shift left introducing zeros.  Shifts the bits of the second operand left by the amount specified in the "
               "first operand. The first operand must be a numeric constant interpreted as unsigned modulo the width of the "
               "second operand.  Bits introduced into the right side are cleared.}";

        ops_.insert(">>",       SymbolicExpr::OP_SHR0);
        doc += "@named{>>}"
               "{Shift right introducing zeros.  Shifts the bits of the second operand right by the amount specified in the "
               "first operand. The first operand must be a numeric constant interpreted as unsigned modulo the width of the "
               "second operand.  Bits introduced into the left side are cleared.}";

        ops_.insert("/",        SymbolicExpr::OP_UDIV);
        doc += "@named{/}"
               "{Unsigned division. Divides the first operand by the second. Both operands are interpreted as unsigned "
               "values, but they need not have the same width. The result width is the same as the width of the first "
               "operand.}";

        ops_.insert(">=",       SymbolicExpr::OP_UGE);
        doc += "@named{>=}"
               "{Unsigned greater-than or equal comparison. Takes two operands that must be the same width and returns a "
               "Boolean.}";

        ops_.insert(">",        SymbolicExpr::OP_UGT);
        doc += "@named{>}"
               "{Unsigned greater-than comparison. Takes two operands that must be the same width and returns a Boolean.}";

        ops_.insert("<=",       SymbolicExpr::OP_ULE);  // requires escapes since '<' introduces a comment
        doc += "@named{<=}"
               "{Unsigned less-than or equal comparison. Takes two operands that must be the same width and returns a "
               "Boolean.}";

        ops_.insert("<",        SymbolicExpr::OP_ULT);  // requires escapes since '<' introduces a comment
        doc += "@named{<}"
               "{Unsigned less-than comparison. Takes two operands that must be the same width and returns a Boolean.}";

        ops_.insert("%",        SymbolicExpr::OP_UMOD);
        doc += "@named{%}"
               "{Unsigned modulo. Computes the first operand modulo the second. Both operands are interpreted as unsigned "
               "values, but they need not have the same width. The result width is the same as the width of the second "
               "operand.}";

        ops_.insert("*",        SymbolicExpr::OP_UMUL);
        doc += "@named{*}"
               "{Unsigned multiply. Computes the product of the operands (one or more). The operands are interpreted as "
               "unsigned values, but they need not have the same width. The result width is the sum of the operand widthds}";

        title("C-like operators");
        docString(doc);
    }
        
public:
    static Ptr instance() {
        return Ptr(new COperators);
    }

    virtual SymbolicExpr::Ptr operator()(const SymbolicExprParser::Token &op, const SymbolicExpr::Nodes &args) ROSE_OVERRIDE {
        if (!ops_.exists(op.lexeme()))
            return SymbolicExpr::Ptr();
        return SymbolicExpr::Interior::create(op.width(), ops_[op.lexeme()], args);
    }
};

class CanonicalVariable: public SymbolicExprParser::AtomExpansion {
public:
    static Ptr instance() {
        Ptr functor = Ptr(new CanonicalVariable);
        functor->title("Canonical variables");
        functor->docString("Variables are written as the letter \"v\" followed by a unique identification number. The "
                           "variable must have a non-zero explicit width. No check is made to ensure that all occurrences "
                           "of the variable have the same width, although this is normally required.");
        return functor;
    }
    SymbolicExpr::Ptr operator()(const SymbolicExprParser::Token &symbol) {
        boost::smatch matches;
        if (!boost::regex_match(symbol.lexeme(), matches, boost::regex("v(\\d+)")))
            return SymbolicExpr::Ptr();
        if (symbol.width() == 0) {
            throw symbol.syntaxError("variable \"" + StringUtility::cEscape(symbol.lexeme()) + "\""
                                     " must have a non-zero width specified");
        }
        uint64_t varId = rose_strtoull(matches.str(1).c_str(), NULL, 10);
        return SymbolicExpr::makeExistingVariable(symbol.width(), varId);
    }
};


void
SymbolicExprParser::init() {
    appendOperatorExpansion(AbbreviatedOperator::instance());
    appendOperatorExpansion(SmtOperators::instance());
    appendOperatorExpansion(COperators::instance());

    appendAtomExpansion(AbbreviatedAtom::instance());
    appendAtomExpansion(CanonicalVariable::instance());
}

std::string
SymbolicExprParser::docString() const {
    std::string s = "Symbolic expressions are parsed as S-expressions in LISP-like notation. Each S-expression is either "
                    "an atom, or a list of S-expressions.  An atom is a bare number or symbol, like \"-45\", \"v120\", or "
                    "\"+\", and a list is a juxtaposition of S-expressions enclosed in parentheses, like \"(add 1 2)\". "
                    "Since lists are defined recursively they can be nested, like \"(+ 1 (* 2 3))\".  The first "
                    "member of a list is an operator and the remaining members are the operands.  An atom or operator "
                    "can be suffixed with a width in square brackets, like \"(+[32] 1[32] v120[32])\", although specifying "
                    "a width for an operator is usually redundant since operator widths are implied by their operands. "
                    "Comments are enclosed in matching, nested pairs of angle brackets, like \"<<this is a comment> within "
                    "a comment>\". Parentheses, square brackets, and angle-brackets can be escaped with a backslash to "
                    "remove their special meaning, although using this feature reduces human readability.";

    BOOST_FOREACH (const AtomExpansion::Ptr &functor, atomTable_) {
        if (!functor->title().empty() && !functor->docString().empty())
            s += "@named{" + functor->title() + "}{" + functor->docString() + "}";
    }

    BOOST_FOREACH (const OperatorExpansion::Ptr &functor, operatorTable_) {
        if (!functor->title().empty() && !functor->docString().empty())
            s += "@named{" + functor->title() + "}{" + functor->docString() + "}";
    }

    return s;
}

SymbolicExpr::Ptr
SymbolicExprParser::parse(const std::string &input, const std::string &inputName) {
    std::istringstream stream(input);
    return parse(stream, inputName);
}

SymbolicExpr::Ptr
SymbolicExprParser::parse(std::istream &input, const std::string &inputName, unsigned lineNumber, unsigned columnNumber) {
    TokenStream tokens(input, inputName, lineNumber, columnNumber);
    return parse(tokens);
}

struct PartialInternalNode {
    SymbolicExprParser::Token op;
    std::vector<SymbolicExpr::Ptr> operands;
    SymbolicExprParser::Token ltparen;                  // for error messages
    PartialInternalNode(const SymbolicExprParser::Token &op, const SymbolicExprParser::Token &ltparen)
        : op(op), ltparen(ltparen) {}
};

SymbolicExpr::Ptr
SymbolicExprParser::parse(TokenStream &tokens) {
    std::vector<PartialInternalNode> stack;
    while (tokens[0].type() != Token::NONE) {
        switch (tokens[0].type()) {
            case Token::LTPAREN: {
                if (tokens[1].type()!=Token::SYMBOL)
                    throw tokens[0].syntaxError("expected operator after left paren", tokens.name());
                stack.push_back(PartialInternalNode(tokens[1], tokens[0]));
                tokens.shift(2);
                break;
            }
            case Token::SYMBOL: {
                SymbolicExpr::Ptr expr;
                try {
                    BOOST_FOREACH (const AtomExpansion::Ptr &functor, atomTable_) {
                        if ((expr = (*functor)(tokens[0])))
                            break;
                    }
                } catch (const SymbolicExpr::Exception &e) {
                    throw tokens[0].syntaxError(e.what(), tokens.name());
                }
                if (expr == NULL)
                    throw tokens[0].syntaxError("unrecognized symbol: \"" + StringUtility::cEscape(tokens[0].lexeme()) + "\"",
                                                tokens.name());
                tokens.shift();
                if (stack.empty())
                    return expr;
                stack.back().operands.push_back(expr);
                break;
            }
            case Token::BITVECTOR: {
                SymbolicExpr::Ptr leaf = SymbolicExpr::makeConstant(tokens[0].bits());
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
                SymbolicExpr::Ptr expr;
                try {
                    BOOST_FOREACH (const OperatorExpansion::Ptr &functor, operatorTable_) {
                        if ((expr = (*functor)(stack.back().op, stack.back().operands)))
                            break;
                    }
                } catch (const SymbolicExpr::Exception &e) {
                    throw stack.back().ltparen.syntaxError(e.what(), tokens.name());
                }
                if (expr == NULL) {
                    throw stack.back().op.syntaxError("unrecognized function name: \"" +
                                                      StringUtility::cEscape(stack.back().op.lexeme()) + "\"",
                                                      tokens.name());
                }
                stack.pop_back();
                if (stack.empty())
                    return expr;
                stack.back().operands.push_back(expr);
                break;
            }
            case Token::NONE:
                ASSERT_not_reachable("end of input");   // should have been handled above
        }
    }
    if (stack.empty())
        throw SyntaxError("empty input", tokens.name(), tokens.lineNumber(), tokens.columnNumber());
    throw stack.back().ltparen.syntaxError("expression not closed before end of input", tokens.name());
}

void
SymbolicExprParser::appendAtomExpansion(const AtomExpansion::Ptr &functor) {
    ASSERT_not_null(functor);
    atomTable_.push_back(functor);
}

void
SymbolicExprParser::appendOperatorExpansion(const OperatorExpansion::Ptr &functor) {
    ASSERT_not_null(functor);
    operatorTable_.push_back(functor);
}

} // namespace
} // namespace
