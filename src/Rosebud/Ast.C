#include <Rosebud/Ast.h>

#include <Rosebud/Utility.h>

#include <Sawyer/AllocatingBuffer.h>
#include <Sawyer/Interval.h>
#include <Sawyer/IntervalSet.h>
#include <Sawyer/Message.h>

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem.hpp>
#include <boost/range/adaptors.hpp>

#include <fstream>
#include <regex>

using namespace Sawyer::Message::Common;

namespace Rosebud {
namespace Ast {

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
    return tokens.empty() && (!string_.isCached() || string_.get().empty());
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
TokenList::string(const FilePtr &file) {
    ASSERT_not_null(file);
    if (!string_.isCached())
        string_ = file->content(tokens, Expand::INTER);
    return string_.get();
}

std::string
TokenList::string() {
    if (!string_.isCached()) {
        auto file = findAncestor<File>();
        ASSERT_not_null(file);
        string_ = file->content(tokens, Expand::INTER);
    }
    return string_.get();
}

void
TokenList::string(const std::string &s) {
    string_ = s;
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
// ArgumentList
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ArgumentList::ArgumentList()
    : elmts(*this) {}

ArgumentList::Ptr
ArgumentList::instance() {
    return Ptr(new ArgumentList);
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
    : cppStack(*this, CppStack::instance()), attributes(*this) {}

Attribute::Ptr
Definition::findAttribute(const std::string &fqName) {
    for (const auto &attr: attributes) {
        if (attr->fqName == fqName)
            return attr();
    }
    return {};
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Property
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Property::Property()
    : cType(*this), cInit(*this) {}

Property::Ptr
Property::instance() {
    return Ptr(new Property);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Class::Class()
    : properties(*this) {}


Class::Ptr
Class::instance() {
    return Ptr(new Class);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// File
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Read the file the hard way with extra data copying because boost::iostreams::mapped_file holds the file descriptor open even
// though mmap doesn't require that. On macOS, this results in the process eventually having too many open file descriptors since
// there's one per parsed Rosebud input file.
static Sawyer::Container::Buffer<size_t, char>::Ptr
readFile(const boost::filesystem::path &name) {
    std::ifstream input(name.c_str());
    if (!input)
        throw std::runtime_error("cannot open \"" + name.string() + "\" for reading");
    size_t nChars = boost::filesystem::file_size(name);
    auto retval = Sawyer::Container::AllocatingBuffer<size_t, char>::instance(nChars);
    std::vector<char> buf(4096);
    for (size_t at = 0; at < nChars; /*void*/) {
        input.read(buf.data(), buf.size());
        const size_t nRead = input.gcount();
        if (nRead > 0) {
            const size_t nWritten = retval->write(buf.data(), at, nRead);
            ASSERT_always_require(nWritten == nRead);
            at += nRead;
        } else {
            ASSERT_always_require(at == retval->size());
            break;
        }
    }
    return retval;
}

File::File(const std::string &name)
    :
#if 1
    // Copy the file into memory. It turns out this is almost as fast as mapping the file.
      stream_(name, readFile(name)),
#else
    // Map the file into memory. This is fast, but we use boost::iostreams::mapped_file which holds the file open for the lifetime
    // of the mapped_file object. This can result in an EMFILE error (the per-process limit on number of open file descriptors has
    // been reached).
      stream_(name),
#endif
      classes(*this) {}


File::Ptr
File::instance(const std::string &name) {
    std::ifstream test(name.c_str());
    if (!test) {
        message(FATAL, "unable to open file for reading: \"" + name + "\"");
        return {};
    } else {
        return Ptr(new File(name));
    }
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
File::content(size_t begin, size_t end) {
    return stream_.content().contentAsString(begin, end);
}

std::string
File::trimmedContent(size_t begin, size_t end, Token &outputToken) {
    return trimmedContent(begin, end, Token(), outputToken);
}

std::string
File::trimmedContent(size_t begin, size_t end, const Token &exclude, Token &outputToken) {
    using Interval = Sawyer::Container::Interval<size_t>;
    using IntervalSet = Sawyer::Container::IntervalSet<Interval>;
    IntervalSet where;

    if (begin < end)
        where |= Interval::hull(begin, end-1);
    if (exclude.begin() < exclude.end())
        where -= Interval::hull(exclude.begin(), exclude.end() - 1);

    // FIXME[Robb Matzke 2023-04-03]: it would be nice if this token were a little more accurate
    if (where.isEmpty()) {
        outputToken = Token();
    } else {
        outputToken = Token(Sawyer::Language::Clexer::TOK_OTHER,
                            where.hull().least(), where.hull().least(), where.hull().greatest());
    }

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
    : files(*this) {}

Project::Ptr
Project::instance() {
    return Ptr(new Project);
}

std::vector<Class::Ptr>
Project::allClassesFileOrder() {
    std::vector<Class::Ptr> retval;
    for (const auto &file: files) {
        for (const auto &c: file->classes)
            retval.push_back(c());
    }
    return retval;
}

Class::Ptr
Project::findClassByName(const std::string &name) {
    for (const auto &file: files) {
        for (const auto &c: file->classes) {
            if (name == c->name)
                return c();
        }
    }
    return Class::Ptr();
}

} // namespace
} // namespace
