#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/CallingConvention/Declaration.h>

#include <Rose/Affirm.h>
#include <Rose/BinaryAnalysis/CallingConvention/Definition.h>
#include <Rose/BinaryAnalysis/CallingConvention/Exception.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/StringUtility/Escape.h>

#include <SgAsmFloatType.h>
#include <SgAsmIntegerType.h>
#include <SgAsmPointerType.h>
#include <SgAsmVoidType.h>

#include <Cxx_GrammarDowncast.h>

#include <Sawyer/Clexer.h>
#include <Sawyer/StaticBuffer.h>

namespace Rose {
namespace BinaryAnalysis {
namespace CallingConvention {

// Token stream for the parse language
using TokenStream = Sawyer::Language::Clexer::TokenStream;

// A non-null type with optional name
using TypeNamePair = std::pair<SgAsmType*, std::string>;

// Parse a type and return the type or an error message.
static Sawyer::Result<SgAsmType*, std::string>
parseType(TokenStream &tokens, const Definition::Ptr &cc) {
    ASSERT_not_null(cc);
    using namespace Sawyer::Language::Clexer;
    SgAsmType *type = nullptr;

    // Required type name, like 'void', 'u8', 'i32', etc.
    if (tokens[0].type() != TOK_WORD)
        return Sawyer::makeError("type name expected");
    if (tokens.matches(tokens[0], "void")) {
        type = SgAsmVoidType::instance();
    } else if (tokens.matches(tokens[0], "u8")) {
        type = SgAsmIntegerType::instanceUnsigned(cc->byteOrder(), 8);
    } else if (tokens.matches(tokens[0], "u16")) {
        type = SgAsmIntegerType::instanceUnsigned(cc->byteOrder(), 16);
    } else if (tokens.matches(tokens[0], "u32")) {
        type = SgAsmIntegerType::instanceUnsigned(cc->byteOrder(), 32);
    } else if (tokens.matches(tokens[0], "u64")) {
        type = SgAsmIntegerType::instanceUnsigned(cc->byteOrder(), 64);
    } else if (tokens.matches(tokens[0], "i8")) {
        type = SgAsmIntegerType::instanceSigned(cc->byteOrder(), 8);
    } else if (tokens.matches(tokens[0], "i16")) {
        type = SgAsmIntegerType::instanceSigned(cc->byteOrder(), 16);
    } else if (tokens.matches(tokens[0], "i32")) {
        type = SgAsmIntegerType::instanceSigned(cc->byteOrder(), 32);
    } else if (tokens.matches(tokens[0], "i64")) {
        type = SgAsmIntegerType::instanceSigned(cc->byteOrder(), 64);
    } else if (tokens.matches(tokens[0], "f32")) {
        type = SgAsmFloatType::instanceIeee32(cc->byteOrder());
    } else if (tokens.matches(tokens[0], "f64")) {
        type = SgAsmFloatType::instanceIeee64(cc->byteOrder());
    } else {
        return Sawyer::makeError("unrecognized type \"" + tokens.lexeme(tokens[0]) + "\"");
    }
    tokens.consume();

    // Optional '*' to indicate pointer
    while (tokens.matches(tokens[0], "*")) {
        type = SgAsmPointerType::instance(cc->byteOrder(), cc->bitsPerWord(), type);
        tokens.consume();
    }

    return Sawyer::makeOk(type);
}

// Parse a type followed by an optional name, or return an error string.
static Sawyer::Result<TypeNamePair, std::string>
parseTypeNamePair(TokenStream &tokens, const Definition::Ptr &cc) {
    ASSERT_not_null(cc);
    using namespace Sawyer::Language::Clexer;
    if (const auto type = parseType(tokens, cc)) {
        if (tokens[0].type() == TOK_WORD) {
            const std::string name = tokens.lexeme(tokens[0]);
            tokens.consume();
            return Sawyer::makeOk(std::make_pair(*type, name));
        } else {
            return Sawyer::makeOk(std::make_pair(*type, ""));
        }
    } else {
        return Sawyer::makeError(type.unwrapError());
    }
}

// Parse a type followed by an optional argument name, or return an error string.
static Sawyer::Result<TypeNamePair, std::string>
parseArgTypeNamePair(TokenStream &tokens, const Definition::Ptr &cc) {
    ASSERT_not_null(cc);
    const auto pair = parseTypeNamePair(tokens, cc);
    if (pair && isSgAsmVoidType(pair->first))
        return Sawyer::makeError("type cannot be \"void\"");
    return pair;
}

static ParseError
parseError(const std::string &mesg, TokenStream &tokens) {
    std::ostringstream ss;
    tokens.emit(ss, tokens.fileName(), tokens[0], mesg);
    return ParseError(ss.str());
}

Declaration::~Declaration() {}

Declaration::Declaration(const Definition::Ptr &cc)
    : callingConvention_(cc) {
    ASSERT_not_null(cc);
}

Declaration::Ptr
Declaration::instance(const Definition::Ptr &cc, const std::string &sourceCode) {
    Ptr decl = Ptr(new Declaration(cc));
    using namespace Sawyer::Language::Clexer;
    decl->sourceCode_ = sourceCode;

    auto buffer = Sawyer::Container::StaticBuffer<size_t, char>::instance(sourceCode.data(), sourceCode.size());
    TokenStream tokens("declaration", buffer);

    // Return type and optional function name
    const auto retType = parseTypeNamePair(tokens, cc);
    if (!retType)
        throw parseError(retType.unwrapError(), tokens);
    decl->returnType_ = retType->first;
    decl->name(retType->second);

    // Start of argument list
    if (!tokens.matches(tokens[0], "("))
        throw parseError("\"(\" expected before argument list", tokens);
    tokens.consume();

    // Argument list
    if (!tokens.matches(tokens[0], ")")) {
        while (true) {
            const auto argType = parseArgTypeNamePair(tokens, cc);
            if (!argType) {
                throw parseError(argType.unwrapError() + " for argument #" +
                                 boost::lexical_cast<std::string>(decl->arguments().size()), tokens);
            }

            // Check that argument name is unique
            if (!argType->second.empty()) {
                for (const auto &otherArg: decl->arguments()) {
                    if (otherArg.second == argType->second)
                        throw parseError("duplicate argument name", tokens);
                }
            }

            decl->arguments_.push_back(*argType);
            if (!tokens.matches(tokens[0], ","))
                break;
            tokens.consume();                           // the comma
        }
    }

    // End of argument list
    if (!tokens.matches(tokens[0], ")"))
        throw parseError("\")\" expected at end of argument list", tokens);
    tokens.consume();
    if (!tokens[0].type() == TOK_EOF)
        throw parseError("extra text after end of argument list", tokens);

    return decl;
}

const std::string&
Declaration::name() const {
    return name_;
}

void
Declaration::name(const std::string &s) {
    name_ = s;
}

const std::string&
Declaration::comment() const {
    return comment_;
}

void
Declaration::comment(const std::string &s) {
    comment_ = s;
}

const std::string&
Declaration::toString() const {
    return sourceCode_;
}

Definition::Ptr
Declaration::callingConvention() const {
    return notnull(callingConvention_);
}

SgAsmType*
Declaration::returnType() const {
    return notnull(returnType_);
}

size_t
Declaration::nArguments() const {
    return arguments_.size();
}

SgAsmType*
Declaration::argumentType(size_t index) const {
    ASSERT_require(index < nArguments());
    return notnull(arguments_[index].first);
}

const std::string&
Declaration::argumentName(size_t index) const {
    ASSERT_require(index < nArguments());
    return arguments_[index].second;
}

const std::vector<TypeNamePair>&
Declaration::arguments() const {
    return arguments_;
}

Sawyer::Result<ConcreteLocation, std::string>
Declaration::argumentLocation(const size_t index) const {
    const Definition::Ptr &cc = callingConvention_;
    std::vector<ConcreteLocation> inputParameters = cc->inputParameters();



    if (index > arguments_.size()) {
        return Sawyer::makeError("argument #" + boost::lexical_cast<std::string>(index) + " is out of range"
                                 " for \"" + toString() + "\"");
    } else if (index < cc->inputParameters().size()) {

        ConcreteLocation loc = cc->inputParameters()[index];
        loc.registerDictionary(callingConvention()->registerDictionary());
        return Sawyer::makeOk(loc);

    } else if (cc->stackParameterOrder() == StackParameterOrder::UNSPECIFIED) {
        return Sawyer::makeError("declaration has too many arguments for calling convention");

    } else if (cc->stackParameterOrder() == StackParameterOrder::RIGHT_TO_LEFT /*C order*/) {
        const uint64_t stackOlderDirection = cc->stackDirection() == StackDirection::GROWS_DOWN ? 1 : -1;
        int64_t stackOffset = stackOlderDirection * cc->nonParameterStackSize();
        for (size_t argno = cc->inputParameters().size(); argno < index; ++argno)
            stackOffset += stackOlderDirection * arguments_[argno].first->get_nBytes();
        ConcreteLocation loc(cc->stackPointerRegister(), stackOffset);
        loc.registerDictionary(callingConvention()->registerDictionary());
        return Sawyer::makeOk(loc);

    } else {
        ASSERT_require(cc->stackParameterOrder() == StackParameterOrder::LEFT_TO_RIGHT /*Pascal order*/);
        ASSERT_not_implemented("Pascal-order stack-based argument passing");

    }
}

Sawyer::Result<ConcreteLocation, std::string>
Declaration::argumentLocation(const std::string &name) const {
    for (size_t i = 0; i < arguments_.size(); ++i) {
        if (arguments_[i].second == name)
            return argumentLocation(i);
    }
    return Sawyer::makeError("argument \"" + StringUtility::cEscape(name) + "\" not found in declaration \"" + toString() + "\"");
}

} // namespace
} // namespace
} // namespace

#endif
