#include <sage3basic.h>

#include <BinarySymbolicExprParser.h>
#include <BinarySmtSolver.h>
#include <Sawyer/BitVector.h>
#include <Sawyer/Map.h>
#include <SymbolicSemantics2.h>
#include <integerOps.h>
#include <rose_strtoull.h>
#include <sstream>

#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

using namespace Sawyer::Message::Common;

namespace Rose {
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

void
SymbolicExprParser::SubstitutionError::print(std::ostream &out) const {
    if (what() && *what()) {
        out <<what();
    } else {
        out <<"substitution error";
    }
}

std::ostream&
operator<<(std::ostream &out, const SymbolicExprParser::SubstitutionError &error) {
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

SymbolicExpr::Type
SymbolicExprParser::TokenStream::consumeType() {
    // '['
    consumeWhiteSpaceAndComments();
    unsigned startLine = lineNumber_;
    unsigned startColumn = columnNumber_;
    if (nextCharacter() != '[') {
        throw SymbolicExprParser::SyntaxError("expected '[' to start a type specification",
                                              name_, startLine, startColumn);
    }
    consumeCharacter();

    // Type name or width constant
    consumeWhiteSpaceAndComments();
    SymbolicExpr::Type retval;
    if (isdigit(nextCharacter())) {
        // integer width
        std::string s;
        while (isdigit(nextCharacter()))
            s += consumeCharacter();
        // optional "->N"
        if (nextCharacter() == '-') {
            size_t width1 = boost::lexical_cast<size_t>(s);
            consumeCharacter();
            if (nextCharacter() != '>')
                throw SymbolicExprParser::SyntaxError("expected '->' in type specification", name_, lineNumber_, columnNumber_);
            consumeCharacter();
            consumeWhiteSpaceAndComments();
            s = "";
            unsigned ln = lineNumber_, cn = columnNumber_;
            while (isdigit(nextCharacter()))
                s += consumeCharacter();
            if (s.empty())
                throw SymbolicExprParser::SyntaxError("expected decimal integer domain width after '->'", name_, ln, cn);
            size_t width2 = boost::lexical_cast<size_t>(s);
            consumeWhiteSpaceAndComments();
            retval = SymbolicExpr::Type::memory(width1, width2);
        } else {
            retval = SymbolicExpr::Type::integer(boost::lexical_cast<size_t>(s));
        }
    } else if (nextCharacter() == 'u') {
        // integer followed by width
        consumeCharacter();
        if (!isdigit(nextCharacter()))
            throw SymbolicExprParser::SyntaxError("invalid type specification", name_, startLine, startColumn);
        std::string s;
        while (isdigit(nextCharacter()))
            s += consumeCharacter();
        retval = SymbolicExpr::Type::integer(boost::lexical_cast<size_t>(s));
    } else if (nextCharacter() == 'f') {
        // floating point followed by width
        consumeCharacter();
        if (!isdigit(nextCharacter()))
            throw SymbolicExprParser::SyntaxError("invalid type specification", name_, startLine, startColumn);
        std::string s;
        while (isdigit(nextCharacter()))
            s += consumeCharacter();
        if ("32" == s) {
            retval = SymbolicExpr::Type::floatingPoint(8, 24);
        } else if ("64" == s) {
            retval = SymbolicExpr::Type::floatingPoint(11, 53);
        } else {
            throw SymbolicExprParser::SyntaxError("invalid type specification", name_, startLine, startColumn);
        }
    } else {
        throw SymbolicExprParser::SyntaxError("invalid type specification", name_, startLine, startColumn);
    }
    consumeWhiteSpaceAndComments();

    // ']'
    if (nextCharacter() == EOF) {
        throw SymbolicExprParser::SyntaxError("end of input reached inside '[' type expression",
                                              name_, startLine, startColumn);
    }
    if (nextCharacter() != ']')
        throw SymbolicExprParser::SyntaxError("missing closing ']' in type specification", name_, startLine, startColumn);
    consumeCharacter();
    return retval;
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
            return Token(Token::LTPAREN, SymbolicExpr::Type(), "(", startLine, startColumn);
        case ')':
            consumeCharacter();
            return Token(Token::RTPAREN, SymbolicExpr::Type(), ")", startLine, startColumn);
        case '[':
            throw SymbolicExprParser::SyntaxError("unexpected width specification", name_, startLine, startColumn);
        case ']':
            throw SymbolicExprParser::SyntaxError("found ']' outside width specification", name_, startLine, startColumn);
        default: {
            std::string s = consumeTerm();
            boost::smatch matches;
            if (boost::regex_match(s, matches, hexLiteralRe)) {
                SymbolicExpr::Type exprType = consumeType();
                if (exprType.typeClass() == SymbolicExpr::Type::MEMORY)
                    throw SymbolicExprParser::SyntaxError("type of hex literal must be scalar", name_, startLine, startColumn);
                Sawyer::Container::BitVector bv(exprType.nBits());
                bv.fromHex(matches.str(1));         // hex digits without leading "0x"
                return Token(bv, exprType, s, startLine, startColumn);
            } else if (boost::regex_match(s, signedDecimalLiteralRe)) {
                int n = boost::lexical_cast<int>(s);
                consumeWhiteSpaceAndComments();
                SymbolicExpr::Type exprType = nextCharacter() == '[' ? consumeType() : SymbolicExpr::Type::integer(8*sizeof(int));
                if (exprType.typeClass() == SymbolicExpr::Type::MEMORY)
                    throw SymbolicExprParser::SyntaxError("type of decimal literal must be scalar", name_, startLine, startColumn);
                Sawyer::Container::BitVector bv(exprType.nBits());
                bv.fromInteger(IntegerOps::signExtend2((uint64_t)n, 8*sizeof(int), 64));
                return Token(bv, exprType, s, startLine, startColumn);
            } else {
                consumeWhiteSpaceAndComments();
                SymbolicExpr::Type exprType = nextCharacter() == '[' ? consumeType() : SymbolicExpr::Type::none();
                return Token(Token::SYMBOL, exprType, s, startLine, startColumn);
            }
        }
    }
    return Token();
}

void
SymbolicExprParser::TokenStream::fillTokenList(size_t idx) {
    for (size_t i = tokens_.size(); i <= idx; ++i) {
        Token token = scan();
        if (Token::NONE == token.tokenType())
            return;
        tokens_.push_back(token);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      OperatorExpansion
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SymbolicExprParser::OperatorExpansion::OperatorExpansion(const SmtSolverPtr &solver)
    : solver(solver) {}

SymbolicExprParser::OperatorExpansion::~OperatorExpansion() {}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      SMT operators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Generates symbolic expressions for the SMT operators
class SmtOperators: public SymbolicExprParser::OperatorExpansion {
protected:
    Sawyer::Container::Map<std::string, SymbolicExpr::Operator> ops_;

    explicit SmtOperators(const SmtSolverPtr &solver)
        : SymbolicExprParser::OperatorExpansion(solver) {
        std::string doc;
        ops_.insert("add",          SymbolicExpr::OP_ADD);
        doc += "@named{add}"
               "{Adds operands together. All operands must have the same width, which is also the width of the result.}";

        ops_.insert("and",          SymbolicExpr::OP_AND);
        doc += "@named{and}"
               "{Conjunction operation. All operands (one or more) must be the same width. Boolean values are 1 bit.}";

        ops_.insert("asr",          SymbolicExpr::OP_ASR);
        doc += "@named{asr}"
               "{Arithmetic shift right. The second operand is interpreted as a signed value which is shifted right by "
               "the unsigned first argument. The result has the same width as the second operand.}";

        ops_.insert("bv-and",       SymbolicExpr::OP_AND); // [Robb Matzke 2017-11-14]: deprecated; use "and" instead.
        ops_.insert("bv-or",        SymbolicExpr::OP_OR); // [Robb Matzke 2017-11-14]: deprecated; use "or" instead.
        ops_.insert("bv-xor",       SymbolicExpr::OP_XOR); // [Robb Matzke 2017-11-14]: deprecated; use "xor" instead

        ops_.insert("concat",       SymbolicExpr::OP_CONCAT);
        doc += "@named{concat}"
               "{Concatenate operands to produce a result whose width is the sum of the widths of the operands. The "
               "operands are in bit-endian order: the first operand becomes the most significant bits of the result and "
               "the last operand becomes the least significant bits. The operands can be any scalar type and the result "
               "will be an integer type (bit vector).}";

        ops_.insert("convert",      SymbolicExpr::OP_CONVERT);
        doc += "@named{convert}"
               "{Converts the argument to the type of the operand. For instance, a 32-bit integer -1 "
               "could be converted to a 64-bit floating-point -1.0, which has an entirely different bit pattern. See also, "
               "\"reinterpret\".}";

        ops_.insert("eq",           SymbolicExpr::OP_EQ);
        doc += "@named{eq}"
               "{Equality comparison. Takes two operands that must be the same width and returns a Boolean.}";

        ops_.insert("extract",      SymbolicExpr::OP_EXTRACT);
        doc += "@named{extract}"
               "{Extract part of a value. The bits of the third operand are numbered from zero (least significant) through "
               "N-1 (most significant) and those bits specified by the first operand (low bit) and second operand "
               "(one past high bit) are returned.  The width of the return value is the difference between the second "
               "and first operands, both of which must have constant numeric values. The third operand can be any scalar "
               "type and the result is always an integer (bit vector) type.}";

        ops_.insert("invert",       SymbolicExpr::OP_INVERT);
        doc += "@named{invert}"
               "{Bit-wise invert. This operator takes one operand and returns a value having the same width but with "
               "each bit flipped. Since ROSE Boolean values are 1-bit vectors, use \"invert\"; 2's complement negation "
               "of a 1-bit value is a no-op. See also \"not\" and \"!\".}";

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

        ops_.insert("not",       SymbolicExpr::OP_INVERT);
        doc += "@named{not}"
               "{Bit-wise invert. This is an alias for \"invert\".}";

        ops_.insert("or",           SymbolicExpr::OP_OR);
        doc += "@named{or}"
               "{Disjunction operation. All operands (one or more) must be the same width. Boolean values are 1 bit.}";

        ops_.insert("read",         SymbolicExpr::OP_READ);
        doc += "@named{read}"
               "{Memory read operation. Indicates a value read from a memory state. This operator expects two operands: the "
               "memory state, and the address from which a value is read. The address must have the same width as the "
               "memory state's domain, and the result has the same width as the memory state's range (usually eight).}";

        ops_.insert("reinterpret", SymbolicExpr::OP_REINTERPRET);
        doc += "@named{reinterpret}"
               "{Reinterprets the argument as some other type without changing the bit pattern of the value. For instance, "
               "reinterpreting the 32-bit integer constant -1 as a 32-bit floating-point type will result in NaN since they "
               "can have the same bit pattern.  The source and destination types must have the same width.}";

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

        ops_.insert("set",          SymbolicExpr::OP_SET);
        doc += "@named{set}"
               "{A set of expressions.  This expression can evaluate to any of its arguments.}";

        ops_.insert("sextend",      SymbolicExpr::OP_SEXTEND);
        doc += "@named{sextend}"
               "{Sign extends the second operand so it has the width specified in the first operand. The first operand "
               "must be an unsigned numeric constant not less than the width of the second operand. The second operand "
               "must be an integer type.}";

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
               "must be an unsigned numeric constant, the second operand must be of scalar type, and the result will be "
               "an integer (bit vector) type.}";

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

        ops_.insert("xor",       SymbolicExpr::OP_XOR);
        doc += "@named{xor}"
               "{Exclusive disjunction operation. All operands (one or more) must be the same width, which is also the width of "
               "the result. Booleans are 1 bit.}";

        ops_.insert("zerop",        SymbolicExpr::OP_ZEROP);
        doc += "@named{zerop}"
               "{Equal to zero.  The result is a Boolean value that is true when the single operand is equal to zero and "
               "false if not zero.}";

        title("Named SMT operators");
        docString(doc);
    }

public:
    static Ptr instance(const SmtSolverPtr &solver) {
        return Ptr(new SmtOperators(solver));
    }

    virtual SymbolicExpr::Ptr
    immediateExpansion(const SymbolicExprParser::Token &op, const SymbolicExpr::Nodes &args) ROSE_OVERRIDE {
        if (!ops_.exists(op.lexeme()))
            return SymbolicExpr::Ptr();
        return SymbolicExpr::Interior::instance(op.exprType(), ops_[op.lexeme()], args, solver);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      C-like operators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Creates symbolic expressions using more C-like operator names
class COperators: public SymbolicExprParser::OperatorExpansion {
protected:
    Sawyer::Container::Map<std::string, SymbolicExpr::Operator> ops_;

    explicit COperators(const SmtSolverPtr &solver)
        : SymbolicExprParser::OperatorExpansion(solver) {
        std::string doc;
        ops_.insert("+",        SymbolicExpr::OP_ADD);
        doc += "@named{+}"
               "{Adds operands together. All operands must have the same width, which is also the width of the result.}";

        ops_.insert("&&",       SymbolicExpr::OP_AND);
        doc += "@named{&&}"
               "{Conjunction operation. All operands (one or more) must be the same width. Note that ROSE does not "
               "distinguish between Boolean types and 1-bit vectors, therefore \"&&\" and \"&\" are the same thing.}";

        ops_.insert("&",        SymbolicExpr::OP_AND);
        doc += "@named{&}"
               "{Conjunction operation. All operands (one or more) must be the same width. Note that ROSE does not "
               "distinguish between Boolean types and 1-bit vectors, therefore \"&&\" and \"&\" are the same thing.}";

        ops_.insert("^",        SymbolicExpr::OP_XOR);
        doc += "@named{^}"
               "{Exclusive disjunction operation. All operands (one or more) must be the same width, which is also the width "
               "of the result. Note that ROSE does not distinguish between Boolean types and 1-bit vectors.}";

        ops_.insert("==",       SymbolicExpr::OP_EQ);
        doc += "@named{==}"
               "{Equality comparison. Takes two operands that must be the same width and returns a Boolean.}";

        ops_.insert("~",        SymbolicExpr::OP_INVERT);
        doc += "@named{~}"
               "{Bit-wise invert. This operator takes one operand and returns a value having the same width but with "
               "each bit flipped. Since ROSE does not distinguish between Boolean and 1-bit vectors, the \"~\" and \"!\" "
               "operators do the same thing.}";

        ops_.insert("!",        SymbolicExpr::OP_INVERT);
        doc += "@named{~}"
               "{Bit-wise invert. This operator takes one operand and returns a value having the same width but with "
               "each bit flipped. Since ROSE does not distinguish between Boolean and 1-bit vectors, the \"~\" and \"!\" "
               "operators do the same thing.}";

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
               "{Disjunction operation. All operands (one or more) must be the same width, which is also the width of "
               "the result. Note that ROSE does not distinguish between Boolean types and 1-bit vectors, therefore "
               "\"||\" and \"|\" are the same thing.}";

        ops_.insert("|",        SymbolicExpr::OP_OR);
        doc += "@named{|}"
               "{Disjunction operation. All operands (one or more) must be the same width, which is also the width of "
               "the result. Note that ROSE does not distinguish between Boolean types and 1-bit vectors, therefore "
               "\"||\" and \"|\" are the same thing.}";

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
    static Ptr instance(const SmtSolverPtr &solver) {
        return Ptr(new COperators(solver));
    }

    virtual SymbolicExpr::Ptr
    immediateExpansion(const SymbolicExprParser::Token &op, const SymbolicExpr::Nodes &args) ROSE_OVERRIDE {
        if (!ops_.exists(op.lexeme()))
            return SymbolicExpr::Ptr();
        return SymbolicExpr::Interior::instance(op.exprType(), ops_[op.lexeme()], args, solver);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Canonical variables
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CanonicalVariable: public SymbolicExprParser::AtomExpansion {
public:
    static Ptr instance() {
        Ptr functor = Ptr(new CanonicalVariable);
        functor->title("Canonical variables");
        functor->docString("Variables are written as the letter \"v\" or \"m\" followed by a unique identification number. "
                           "Variables whose name starts with \"v\" are scalar and must have an explicit width (number in "
                           "square brackets after the variable name).  Variables that start with \"m\" are memory states "
                           "and must be followed by a domain (address) width, a \"->\" arrow, and a range (value) width "
                           "in square bracket (or just a domain width, in which case the range width is assumed to be "
                           "eight). No check is made to ensure that all occurrences of a variable have the same "
                           "width, although this is normally required.");
        return functor;
    }
    SymbolicExpr::Ptr immediateExpansion(const SymbolicExprParser::Token &symbol) {
        boost::smatch matches;
        if (!boost::regex_match(symbol.lexeme(), matches, boost::regex("[vm](\\d+)")))
            return SymbolicExpr::Ptr();
        if (symbol.exprType().nBits() == 0) {
            throw symbol.syntaxError("variable \"" + StringUtility::cEscape(symbol.lexeme()) + "\""
                                     " must have a non-zero width specified");
        }
        uint64_t varId = rose_strtoull(matches.str(1).c_str(), NULL, 10);
        if (symbol.lexeme()[0] == 'v') {
            if (symbol.exprType().typeClass() == SymbolicExpr::Type::MEMORY) {
                throw symbol.syntaxError("scalar variable \"" + StringUtility::cEscape(symbol.lexeme()) + "\""
                                         " should have scalar type");
            }
            return SymbolicExpr::makeVariable(symbol.exprType(), varId);
        } else {
            ASSERT_require(symbol.lexeme()[0] == 'm');
            if (symbol.exprType().typeClass() != SymbolicExpr::Type::MEMORY) {
                throw symbol.syntaxError("memory variable \"" + StringUtility::cEscape(symbol.lexeme()) + "\""
                                         " should have memory type");
            }
            return SymbolicExpr::makeVariable(symbol.exprType(), varId);
        }
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Boolean literals
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class BooleanConstant: public SymbolicExprParser::AtomExpansion {
public:
    static Ptr instance() {
        Ptr functor = Ptr(new BooleanConstant);
        functor->title("Boolean constants");
        functor->docString("Boolean constants named \"true\" and \"false\" are equivalent to the more cumbersome numeric "
                           "constants \"1[1]\" and \"0[1]\".");
        return functor;
    }
    SymbolicExpr::Ptr immediateExpansion(const SymbolicExprParser::Token &symbol) {
        if (symbol.lexeme() == "true") {
            return SymbolicExpr::makeBooleanConstant(true);
        } else if (symbol.lexeme() == "false") {
            return SymbolicExpr::makeBooleanConstant(false);
        } else {
            return SymbolicExpr::Ptr();
        }
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Immediate register substitutions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
SymbolicExprParser::defineRegisters(const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr &ops) {
    atomTable_.push_back(RegisterToValue::instance(ops));
}

SymbolicExprParser::RegisterSubstituter::Ptr
SymbolicExprParser::defineRegisters(const RegisterDictionary *regdict) {
    RegisterSubstituter::Ptr retval = RegisterSubstituter::instance(regdict);
    atomTable_.push_back(retval);
    return retval;
}

// class method
SymbolicExprParser::RegisterToValue::Ptr
SymbolicExprParser::RegisterToValue::instance(const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr &ops) {
    Ptr functor = Ptr(new RegisterToValue(ops));
    functor->title("Registers");
    std::string doc = "Register locations are specified by just mentioning the name of the register. Register names "
                      "are usually lower case, such as \"eax\", \"rip\", etc.";
    functor->docString(doc);
    return functor;
}

SymbolicExpr::Ptr
SymbolicExprParser::RegisterToValue::immediateExpansion(const Token &token) {
    using namespace Rose::BinaryAnalysis::InstructionSemantics2;
    BaseSemantics::RegisterStatePtr regState = ops_->currentState()->registerState();
    const RegisterDescriptor *regp = regState->get_register_dictionary()->lookup(token.lexeme());
    if (NULL == regp)
        return SymbolicExpr::Ptr();
    if (token.exprType().nBits() != 0 && token.exprType().nBits() != regp->nBits()) {
        throw token.syntaxError("invalid register width (specified=" + StringUtility::numberToString(token.exprType().nBits()) +
                                ", actual=" + StringUtility::numberToString(regp->nBits()) + ")");
    }
    if (token.exprType().typeClass() == SymbolicExpr::Type::MEMORY)
        throw token.syntaxError("register width must be scalar");
    BaseSemantics::SValuePtr regValue = regState->peekRegister(*regp, ops_->undefined_(regp->nBits()), ops_.get());
    return SymbolicSemantics::SValue::promote(regValue)->get_expression();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Delayed register substitutions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// class method
SymbolicExprParser::RegisterSubstituter::Ptr
SymbolicExprParser::RegisterSubstituter::instance(const RegisterDictionary *regdict) {
    ASSERT_not_null(regdict);
    Ptr functor = Ptr(new RegisterSubstituter(regdict));
    functor->title("Registers");
    std::string doc = "Register locations are specified by just mentioning the name of the register. Register names "
                      "are usually lower case, such as \"eax\", \"rip\", etc. If the register name is suffixed with "
                      "\"_0\", then the value is read from the lazy initial state if it exists, or is an error if the "
                      "initial state doesn't exist.";
    functor->docString(doc);
    return functor;
}

SymbolicExpr::Ptr
SymbolicExprParser::RegisterSubstituter::immediateExpansion(const Token &token) {
    using namespace Rose::BinaryAnalysis::InstructionSemantics2;
    ASSERT_not_null(regdict_);

    // Look up either the full name, or without the "_0" suffix
    std::string registerName = token.lexeme();
    const RegisterDescriptor *regp = regdict_->lookup(registerName);
    if (!regp && boost::ends_with(registerName, "_0"))
        regp = regdict_->lookup(boost::erase_tail_copy(registerName, 2));

    if (NULL == regp)
        return SymbolicExpr::Ptr();
    if (token.exprType().nBits() != 0 && token.exprType().nBits() != regp->nBits()) {
        throw token.syntaxError("invalid register width (specified=" + StringUtility::numberToString(token.exprType().nBits()) +
                                ", actual=" + StringUtility::numberToString(regp->nBits()) + ")");
    }
    if (token.exprType().typeClass() == SymbolicExpr::Type::MEMORY)
        throw token.syntaxError("register width must be scalar");

    SymbolicExpr::Ptr retval;
    if (reg2var_.forward().getOptional(*regp).assignTo(retval))
        return retval;

    retval = SymbolicExpr::makeIntegerVariable(regp->nBits(), token.lexeme());
    reg2var_.insert(*regp, retval);
    return retval;
}

SymbolicExpr::Ptr
SymbolicExprParser::RegisterSubstituter::delayedExpansion(const SymbolicExpr::Ptr &src, const SymbolicExprParser *parser) {
    ASSERT_not_null(src);
    ASSERT_not_null(parser);
    ASSERT_not_null(ops_);

    namespace SS = Rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics;
    namespace BS = Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;
    Sawyer::Message::Stream debug(SymbolicExprParser::mlog[DEBUG]);

    RegisterDescriptor reg;
    if (reg2var_.reverse().getOptional(src).assignTo(reg)) {
        // Earlier (in immediateExpansion), we set the temporary variable's comment to be the original variable (register)
        // name including any "_0" suffix. Now, if we're expanding an "_0" register we should read from the original state
        // rather than the current state.
        BS::RegisterStatePtr regState;
        if (boost::ends_with(src->comment(), "_0")) {
            if (!ops_->initialState()) {
                std::ostringstream ss;
                ss <<"no initial state from which to read register"
                   <<" \"" <<StringUtility::cEscape(src->comment()) <<"\"";
                throw SubstitutionError(ss.str());
            }
            regState = ops_->initialState()->registerState();
        } else {
            regState = ops_->currentState()->registerState();
        }

        // Read the register
        SS::SValuePtr regval = SS::SValue::promote(regState->readRegister(reg, ops_->undefined_(reg.nBits()), ops_.get()));
        SAWYER_MESG(debug) <<"register substitution: " <<src->comment() <<" = " <<*regval <<"\n";
        return regval->get_expression();
    }
    return src;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Delayed memory substitutions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SymbolicExprParser::MemorySubstituter::Ptr
SymbolicExprParser::MemorySubstituter::instance(const SmtSolver::Ptr &solver /*=NULL*/) {
    Ptr functor = Ptr(new MemorySubstituter(solver));
    functor->title("Memory");
    functor->docString("Memory can be read using the \"memory\" function. A size should be specified as part of the operator "
                       "name, and the argument is the address (possibly symbolic) that should be read. The actual read is "
                       "delayed until the expression is evaluated, although expressions involving memory undergo simplifications "
                       "during the parse (e.g., performing an exclusive-or whose operands are both the same memory location "
                       "will result in zero at parse time without ever reading the memory location).");
    return functor;
}

SymbolicExpr::Ptr
SymbolicExprParser::MemorySubstituter::immediateExpansion(const Token &func, const SymbolicExpr::Nodes &operands) {
    if (func.lexeme() != "memory") {
        return SymbolicExpr::Ptr();
    } else if (operands.size() != 1) {
        throw func.syntaxError("wrong number of arguments for \"memory\""
                               "(specified=" + StringUtility::numberToString(operands.size()) + ", required=1)");
    } else if (func.exprType().nBits() % 8 != 0) {
        throw func.syntaxError("invalid memory width (specified=" + StringUtility::numberToString(func.exprType().nBits()) +
                               ", required multiple of 8)");
    } else {
        SymbolicExpr::Ptr retval = SymbolicExpr::makeIntegerVariable(func.exprType().nBits(), "memory-ref");
        exprToMem_.insert(retval, operands[0]);
        return retval;
    }
}

SymbolicExpr::Ptr
SymbolicExprParser::MemorySubstituter::delayedExpansion(const SymbolicExpr::Ptr &src, const SymbolicExprParser *parser) {
    ASSERT_not_null(src);
    ASSERT_not_null(parser);
    ASSERT_not_null(ops_);
    using namespace Rose::BinaryAnalysis::InstructionSemantics2;
    Sawyer::Message::Stream debug(SymbolicExprParser::mlog[DEBUG]);

    if (SymbolicExpr::Ptr addrExpr = exprToMem_.getOrDefault(src)) {
        addrExpr = parser->delayedExpansion(addrExpr);
        SymbolicSemantics::SValuePtr addr = SymbolicSemantics::SValue::promote(ops_->undefined_(addrExpr->nBits()));
        addr->set_expression(addrExpr);
        BaseSemantics::SValuePtr dflt = ops_->undefined_(src->nBits());
         BaseSemantics::SValuePtr mem = ops_->readMemory(RegisterDescriptor(), addr, dflt, ops_->boolean_(true));
        SAWYER_MESG(debug) <<"memory substitution: (memory[" <<src->nBits() <<"] " <<*addr <<") -> " <<*mem <<"\n";
        return SymbolicSemantics::SValue::promote(mem)->get_expression();
    } else {
        return src;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Named variable substitutions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// class method
SymbolicExprParser::TermPlaceholders::Ptr
SymbolicExprParser::TermPlaceholders::instance() {
    Ptr functor = Ptr(new TermPlaceholders);
    functor->title("Terms");
    functor->docString("Any variable.");
    return functor;
}

SymbolicExpr::Ptr
SymbolicExprParser::TermPlaceholders::immediateExpansion(const Token &token) {
    SymbolicExpr::Ptr retval;
    if (name2var_.forward().getOptional(token.lexeme()).assignTo(retval))
        return retval;
    if (token.exprType().nBits() == 0)
        throw token.syntaxError("non-zero variable width required");
    retval = SymbolicExpr::makeIntegerVariable(token.exprType().nBits());
    name2var_.insert(token.lexeme(), retval);
    return retval;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      SymbolicExprParser
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Sawyer::Message::Facility SymbolicExprParser::mlog;

// class method
void
SymbolicExprParser::initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::SymbolicExprParser");
        mlog.comment("parsing symbolic expressions");
    }
}

// Throws an exception for functions named "..."
class AbbreviatedOperator: public SymbolicExprParser::OperatorExpansion {
protected:
    explicit AbbreviatedOperator(const SmtSolverPtr &solver)
        : SymbolicExprParser::OperatorExpansion(solver) {}

public:
    static Ptr instance(const SmtSolverPtr &solver) {
        return Ptr(new AbbreviatedOperator(solver));            // undocumented
    }
    SymbolicExpr::Ptr immediateExpansion(const SymbolicExprParser::Token &op, const SymbolicExpr::Nodes &args) {
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
    SymbolicExpr::Ptr immediateExpansion(const SymbolicExprParser::Token &symbol) {
        if (symbol.lexeme() == "...")
            throw symbol.syntaxError("input is an abbreviated expression; parts are missing");
        return SymbolicExpr::Ptr();
    }
};

SymbolicExprParser::SymbolicExprParser() {
    init();
}

SymbolicExprParser::SymbolicExprParser(const SmtSolverPtr &solver)
    : solver_(solver) {
    init();
}

SymbolicExprParser::~SymbolicExprParser() {}

void
SymbolicExprParser::init() {
    appendOperatorExpansion(AbbreviatedOperator::instance(solver_));
    appendOperatorExpansion(SmtOperators::instance(solver_));
    appendOperatorExpansion(COperators::instance(solver_));

    appendAtomExpansion(AbbreviatedAtom::instance());
    appendAtomExpansion(CanonicalVariable::instance());
    appendAtomExpansion(BooleanConstant::instance());
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
    TokenStream tokens(stream, inputName);
    SymbolicExpr::Ptr expr = parse(tokens);
    if (tokens[0].tokenType() != Token::NONE)
        throw SyntaxError("additional text after end of expression",
                          tokens.name(), tokens[0].lineNumber(), tokens[0].columnNumber());
    return expr;
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
    while (tokens[0].tokenType() != Token::NONE) {
        switch (tokens[0].tokenType()) {
            case Token::LTPAREN: {
                if (tokens[1].tokenType() != Token::SYMBOL)
                    throw tokens[0].syntaxError("expected operator after left paren", tokens.name());
                stack.push_back(PartialInternalNode(tokens[1], tokens[0]));
                tokens.shift(2);
                break;
            }
            case Token::SYMBOL: {
                SymbolicExpr::Ptr expr;
                try {
                    BOOST_FOREACH (const AtomExpansion::Ptr &functor, atomTable_) {
                        if ((expr = functor->immediateExpansion(tokens[0])))
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
                SymbolicExpr::Ptr leaf = SymbolicExpr::makeConstant(tokens[0].exprType(), tokens[0].bits());
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
                        if ((expr = functor->immediateExpansion(stack.back().op, stack.back().operands)))
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

struct DelayedSubber {
    const SymbolicExprParser *parser;

    explicit DelayedSubber(const SymbolicExprParser *parser)
        : parser(parser) {}

    SymbolicExpr::Ptr operator()(SymbolicExpr::Ptr expr, const SmtSolver::Ptr &solver/*=NULL*/) {
        BOOST_FOREACH (SymbolicExprParser::AtomExpansion::Ptr expander, parser->atomTable()) {
            expr = expander->delayedExpansion(expr, parser);
            ASSERT_not_null(expr);
        }
        BOOST_FOREACH (SymbolicExprParser::OperatorExpansion::Ptr expander, parser->operatorTable()) {
            expr = expander->delayedExpansion(expr, parser);
            ASSERT_not_null(expr);
        }
        return expr;
    }
};

SymbolicExpr::Ptr
SymbolicExprParser::delayedExpansion(const SymbolicExpr::Ptr &expr) const {
    DelayedSubber subber(this);
    return SymbolicExpr::substitute(expr, subber, solver_);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Command-line parsing
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// class method
std::string
SymbolicExprParser::SymbolicExprCmdlineParser::docString() {
    return SymbolicExprParser().docString();
}

Sawyer::CommandLine::ParsedValue
SymbolicExprParser::SymbolicExprCmdlineParser::operator()(const char *input, const char **rest,
                                                          const Sawyer::CommandLine::Location &loc) {
    SymbolicExpr::Ptr expr;
    try {
        expr = SymbolicExprParser().parse(input, "command-line");
    } catch (const SyntaxError &e) {
        *rest = input && *input ? input+1 : input;
        std::ostringstream ss;
        e.print(ss);
        ss <<"\n  input: " <<input
           <<"\n  here---" <<std::string(e.columnNumber, '-') <<"^";
        throw std::runtime_error(ss.str());
    }

    *rest = input + strlen(input);
    return Sawyer::CommandLine::ParsedValue(expr, loc, input, valueSaver());
}

SymbolicExprParser::SymbolicExprCmdlineParser::Ptr
SymbolicExprParser::symbolicExprParser(SymbolicExpr::Ptr &storage) {
    return SymbolicExprCmdlineParser::instance(Sawyer::CommandLine::TypedSaver<SymbolicExpr::Ptr>::instance(storage));
}

SymbolicExprParser::SymbolicExprCmdlineParser::Ptr
SymbolicExprParser::symbolicExprParser(std::vector<SymbolicExpr::Ptr> &storage) {
    return SymbolicExprCmdlineParser::instance(Sawyer::CommandLine::TypedSaver<std::vector<SymbolicExpr::Ptr> >::instance(storage));
}

SymbolicExprParser::SymbolicExprCmdlineParser::Ptr
SymbolicExprParser::symbolicExprParser() {
    return SymbolicExprCmdlineParser::instance();
}


} // namespace
} // namespace
