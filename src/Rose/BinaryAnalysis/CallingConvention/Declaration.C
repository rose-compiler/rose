#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/CallingConvention/Declaration.h>

#include <Rose/Affirm.h>
#include <Rose/BinaryAnalysis/CallingConvention/Definition.h>
#include <Rose/BinaryAnalysis/CallingConvention/Exception.h>
#include <Rose/BinaryAnalysis/CallingConvention/StoragePool.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/StringUtility/Escape.h>

#include <SgAsmFloatType.h>
#include <SgAsmIntegerType.h>
#include <SgAsmPointerType.h>
#include <SgAsmVoidType.h>

#include <Cxx_GrammarDowncast.h>

#include <Sawyer/Clexer.h>
#include <Sawyer/StaticBuffer.h>

#include <boost/lexical_cast.hpp>

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

    decl->computeArgumentLocations();
    decl->computeReturnLocations();
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

void
Declaration::computeArgumentLocations() {
    Definition::Ptr cc = notnull(callingConvention_);
    Allocator::Ptr allocator = notnull(cc->argumentValueAllocator());
    allocator->reset();
    argumentLocations_.clear();

    for (size_t argIndex = 0; argIndex < arguments_.size(); ++argIndex) {
        SgAsmType *argType = arguments_[argIndex].first;
        const std::string &argName = arguments_[argIndex].second;
        const std::vector<ConcreteLocation> locations = allocator->allocate(argType);

        // Check for errors
        if (locations.empty())
            throw AllocationError::cannotAllocateArgument(argIndex, argName, argType, toString());
#ifndef NDEBUG
        if (cc->stackParameterOrder() != StackParameterOrder::RIGHT_TO_LEFT) {
            for (const ConcreteLocation &loc: locations) {
                if (loc.type() == ConcreteLocation::RELATIVE)
                    ASSERT_not_implemented("stack-based locations are only handled for right-to-left (C-style) arguments");
            }
        }
#endif

        argumentLocations_.push_back(locations);
    }
}

void
Declaration::computeReturnLocations() {
    if (returnType_ && !isSgAsmVoidType(returnType_)) {
        Definition::Ptr cc = notnull(callingConvention_);
        Allocator::Ptr allocator = notnull(cc->returnValueAllocator());
        allocator->reset();
        const std::vector<ConcreteLocation> locations = allocator->allocate(returnType_);
        if (locations.empty())
            throw AllocationError::cannotAllocateReturn(returnType_, toString());
        returnLocations_ = locations;
    }
}

Sawyer::Result<std::vector<ConcreteLocation>, std::string>
Declaration::argumentLocation(const size_t index) const {
    if (index > arguments_.size())
        return Sawyer::makeError("argument #" + boost::lexical_cast<std::string>(index) + " is out-of-bounds"
                                 " in declaration \"" + toString() + "\"");
    if (index >= argumentLocations_.size())
        return Sawyer::makeError("argument #" + boost::lexical_cast<std::string>(index) + " is not allocated"
                                 " in declaration \"" + toString() + "\"");
    return Sawyer::makeOk(argumentLocations_[index]);
}

Sawyer::Result<std::vector<ConcreteLocation>, std::string>
Declaration::argumentLocation(const std::string &name) const {
    for (size_t i = 0; i < arguments_.size(); ++i) {
        if (arguments_[i].second == name)
            return argumentLocation(i);
    }
    return Sawyer::makeError("argument \"" + StringUtility::cEscape(name) + "\" not found in declaration \"" + toString() + "\"");
}

Sawyer::Result<std::vector<ConcreteLocation>, std::string>
Declaration::returnLocation() const {
    ASSERT_not_null(returnType_);
    if (isSgAsmVoidType(returnType_)) {
        return Sawyer::makeOk(std::vector<ConcreteLocation>());
    } else if (returnLocations_.empty()) {
        return Sawyer::makeError("return value is not allocated in declaration \"" + toString() + "\"");
    } else {
        return Sawyer::makeOk(returnLocations_);
    }
}

} // namespace
} // namespace
} // namespace

#endif
