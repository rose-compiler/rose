#include <Rosebud/Ast.h>

#include <Rosebud/Utility.h>

#include <Sawyer/Interval.h>
#include <Sawyer/IntervalSet.h>
#include <Sawyer/Message.h>

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/range/adaptors.hpp>

#include <regex>

using namespace Sawyer::Message::Common;

namespace Rosebud {
namespace Ast {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Errors
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Error::Error(const std::string &mesg, const NodePtr &node)
    : std::runtime_error(mesg), node(node) {}

AttachmentError::AttachmentError(const NodePtr &node)
    : Error("node is already attached to the AST", node) {}

CycleError::CycleError(const NodePtr &node)
    : Error("insertion of node would cause a cycle in the AST", node) {}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ParentEdge
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ParentEdge::~ParentEdge() {
    ASSERT_require(parent_ == nullptr);
}

ParentEdge::ParentEdge(Node &child)
    : child_(child) {}

NodePtr
ParentEdge::operator()() const {
    if (parent_) {
        return parent_->shared();
    } else {
        return {};
    }
}

NodePtr
ParentEdge::operator->() const {
    ASSERT_not_null(parent_);
    return parent_->shared();
}

void
ParentEdge::reset() {
    parent_ = nullptr;
}

void
ParentEdge::set(Node &parent) {
    parent_ = &parent;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ParentEdgeAccess
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
ParentEdgeAccess::resetParent(ParentEdge &e) {
    e.reset();
}

void
ParentEdgeAccess::setParent(ParentEdge &e, Node &parent) {
    e.set(parent);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Node
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Node::Node()
    : parent(*this) {}

NodePtr
Node::shared() {
    return shared_from_this();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TokenList
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TokenList::TokenList() {}

TokenList::Ptr
TokenList::instance() {
    return Ptr(new TokenList);
}

bool
TokenList::empty() const {
    return tokens.empty();
}

size_t
TokenList::size() const {
    return tokens.size();
}

void
TokenList::push_back(const Token &token) {
    tokens.push_back(token);
}

std::string
TokenList::string() {
    return string(findAncestor<File>());
}

std::string
TokenList::string(const FilePtr &file) {
    ASSERT_not_null(file);
    return file->content(tokens, Expand::INTER);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CppStack
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CppStack::CppStack() {}

CppStack::Ptr
CppStack::instance() {
    return Ptr(new CppStack);
}

CppStack::Type
CppStack::type(const std::string &directive) {
    if (std::regex_search(directive, std::regex("^[ \\t]*#[ \\t]*if"))) {
        return Type::IF;
    } else if (std::regex_search(directive, std::regex("^[ \\t]*#[ \\t]*(else|elif)"))) {
        return Type::ELSE;
    } else if (std::regex_search(directive, std::regex("^[ \\t]*#[ \\t]*endif"))) {
        return Type::END;
    } else {
        return Type::OTHER;
    }
}

bool
CppStack::process(const File::Ptr &file, const Token &token, Stack &stack) {
    ASSERT_not_null(file);
    if (token.type() != Sawyer::Language::Clexer::TOK_CPP)
        return false;

    const std::string lexeme = boost::trim_copy(file->lexeme(token));
    switch (type(lexeme)) {
        case Type::IF:
            stack.push_back(Level{Directive{Type::IF, token, lexeme}});
            break;
        case Type::END:
            if (stack.empty() || stack.back().front().type != Type::IF) {
                stack.push_back(Level{Directive{Type::END, token, lexeme}});
            } else {
                stack.pop_back();
            }
            break;
        case Type::ELSE:
            if (stack.empty() || stack.back().front().type == Type::END) {
                stack.push_back(Level{Directive{Type::ELSE, token, lexeme}});
            } else {
                stack.back().push_back(Directive{Type::ELSE, token, lexeme});
            }
            break;
        case Type::OTHER:
            return false;
    }
    return true;
}

bool
CppStack::printError(const File::Ptr &file) const {
    ASSERT_not_null(file);
    if (stack.empty()) {
        return false;
    } else if (stack.back().empty()) {
        ASSERT_not_reachable("stack elements must never be empty");
    } else if (Type::END == stack.back().front().type) {
        message(ERROR, file, stack.back().front().token, "unmatched \"#endif\"");
        return true;
    } else if (Type::ELSE == stack.back().front().type && 1 == stack.back().size()) {
        message(ERROR, file, stack.back().front().token, "unmatched \"#else\" or \"#elif\"");
        return true;
    } else {
        return false;
    }
}

void
CppStack::emitOpen(std::ostream &out) {
    for (const Level &level: stack) {
        for (const Directive &directive: level)
            out <<directive.lexeme <<"\n";
    }
}

void
CppStack::emitClose(std::ostream &out) {
    std::regex cppHashRe("^[ \\t]*#[ \\t]*");
    for (const Level &level: boost::adaptors::reverse(stack)) {
        std::smatch parts;
        ASSERT_forbid(level.empty());
        if (std::regex_search(level.back().lexeme, parts, cppHashRe)) {
            out <<parts.str(0) <<"endif\n";
        } else {
            out <<"#endif\n";
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Attribute
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Attribute::Attribute()
    : arguments(*this) {}

Attribute::Ptr
Attribute::instance() {
    return Ptr(new Attribute);
}

Attribute::Ptr
Attribute::instance(const std::string &fqName, const std::vector<Token> &nameTokens) {
    auto retval = instance();
    retval->fqName = fqName;
    retval->nameTokens = nameTokens;
    return retval;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Definition
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Definition::Definition()
    : cppStack(*this, CppStack::instance()) {}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Property
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Property::Property()
    : cType(*this), cInit(*this), attributes(*this, AttributeList::instance()) {}

Property::Ptr
Property::instance() {
    return Ptr(new Property);
}

Attribute::Ptr
Property::findAttribute(const std::string &fqName) {
    for (const auto &attr: *attributes()) {
        if (attr->fqName == fqName)
            return attr();
    }
    return {};
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Class::Class()
    : properties(*this, PropertyList::instance()) {}


Class::Ptr
Class::instance() {
    return Ptr(new Class);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// File
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

File::File(const std::string &name)
    : stream_(name), classes(*this, ClassList::instance()) {}

File::Ptr
File::instance(const std::string &name) {
    return Ptr(new File(name));
}

std::string
File::name() {
    return stream_.fileName();
}

TokenStream&
File::tokenStream() {
    return stream_;
}

const Token
File::token(size_t position) {
    return stream_[position];
}

std::string
File::lexeme(size_t position) {
    return lexeme(stream_[position]);
}

std::string
File::lexeme(const Token &token) {
    return stream_.lexeme(token);
}

bool
File::matches(const Token &token, const char *s) {
    ASSERT_not_null(s);
    return stream_.matches(token, s);
}

bool
File::matches(const Token &token, const std::string &s) {
    return matches(token, s.c_str());
}

bool
File::matches(size_t tokenPosition, const char *s) {
    return matches(token(tokenPosition), s);
}

bool
File::matches(size_t tokenPosition, const std::string &s) {
    return matches(token(tokenPosition), s.c_str());
}

bool
File::matches(const char *s) {
    return matches(token(0), s);
}

bool
File::matches(const std::string &s) {
    return matches(token(0), s.c_str());
}

bool
File::matchesAny(size_t at, const std::vector<std::string> &lexemes) {
    for (const std::string &lexeme: lexemes) {
        if (matches(at, lexeme))
            return true;
    }
    return false;
}

bool
File::startsWith(const Token &token, const char *s) {
    ASSERT_not_null(s);
    return stream_.startsWith(token, s);
}

Token
File::consume() {
    Token t = token(0);
    stream_.consume();
    return t;
}

void
File::consume(size_t n) {
    stream_.consume(n);
}

std::string
File::content(const std::vector<Token> &tokens, Expand expand) {
    std::string retval;
    for (const Token &token: tokens) {
        switch (expand) {
            case Expand::INTER:
                expand = Expand::PRIOR;
                // fall through...
            case Expand::NONE:
                retval += stream_.content().contentAsString(token.begin(), token.end());
                break;
            case Expand::PRIOR:
                retval += stream_.content().contentAsString(token.prior(), token.end());
                break;
        }
    }
    return retval;
}

std::string
File::trimmedContent(size_t begin, size_t end) {
    return trimmedContent(begin, end, Token());
}

std::string
File::trimmedContent(size_t begin, size_t end, const Token &exclude) {
    using Interval = Sawyer::Container::Interval<size_t>;
    using IntervalSet = Sawyer::Container::IntervalSet<Interval>;
    IntervalSet where;

    if (begin < end)
        where |= Interval::hull(begin, end-1);
    if (exclude.begin() < exclude.end())
        where -= Interval::hull(exclude.begin(), exclude.end() - 1);

    std::string s;
    for (const Interval &interval: where.intervals())
        s += stream_.content().contentAsString(interval.least(), interval.greatest() + 1);

    std::vector<std::string> lines = splitIntoLines(s);
    trimBlankLines(lines);
    return boost::join(lines, "\n");
}

void
File::emitContext(std::ostream &out, const Token &first, const Token &locus, const Token &last) {
    stream_.emit(out, name(), first, locus, last, "");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Project
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Project::Project()
    : files(*this, FileList::instance()) {}

Project::Ptr
Project::instance() {
    return Ptr(new Project);
}

std::vector<Class::Ptr>
Project::allClassesFileOrder() {
    std::vector<Class::Ptr> retval;
    for (const auto &file: *files()) {
        for (const auto &c: *file->classes())
            retval.push_back(c());
    }
    return retval;
}




} // namespace
} // namespace
