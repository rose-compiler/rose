// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://gitlab.com/charger7534/sawyer.git.




#include <Sawyer/DocumentMarkup.h>

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>

namespace Sawyer {
namespace Document {
namespace Markup {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Error handling
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ErrorLocation::Frame::Frame(TokenStream &where, const std::string &mesg) {
    if (where.atEof()) {
        boost::tie(lineIdx, offset) = where.locationEof();
    } else {
        boost::tie(lineIdx, offset) = where.location(where.current().begin());
    }

    name = mesg + std::string(mesg.empty()?"":" ") +
           "at " + where.name() +
           ":" + boost::lexical_cast<std::string>(lineIdx+1) +
           "." + boost::lexical_cast<std::string>(offset+1);
    input = where.lineString(lineIdx);
};

SAWYER_EXPORT std::string
ErrorLocation::toString() const {
    std::string retval;
    BOOST_FOREACH (const Frame &frame, frames_) {
        retval += (retval.empty() ? "" : "\n") + frame.name;
        std::string s = boost::trim_right_copy(frame.input);
        size_t offset = std::min(frame.offset, s.size());

        // If there's lots of stuff after the curor, replace it with "..."
        static const size_t MAX_BEFORE_CURSOR = 50;     // arbitrary amount context to show before cursor
        static const size_t MAX_AFTER_CURSOR = 20;      // arbitrary amount to show after the cursor
        static const double ELLIPSIS_PLACEMENT = 0.5;   // where to position ellipsis before the cursor

        if (offset < s.size() && s.size() - offset > MAX_AFTER_CURSOR)
            s = s.substr(0, offset + MAX_AFTER_CURSOR) + "...";

        if (offset > MAX_BEFORE_CURSOR) {
            size_t n1 = round(MAX_BEFORE_CURSOR * ELLIPSIS_PLACEMENT);
            size_t n2 = MAX_BEFORE_CURSOR - n1;
            s = s.substr(0, n1) + "..." + s.substr(offset-n2);
            offset = n1 + 3 + n2;
        }

        retval += "\n       |" + s + "\n       |" + std::string(offset, '-') + "^\n";
    }

    return retval;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      TokenStream
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool
isValidNameCharacter(int c) {
    return isalnum(c) || strchr("_-", c);
}

SAWYER_EXPORT Token
TokenStream::scanNextToken(const Container::LineVector &content, size_t &at /*in,out*/) {
    size_t begin = at;

    while (1) {
        int c = content.character(at);
        if (EOF == c) {
            if (at > begin)
                return Token(TOK_DATA, begin, at);
            return Token();

        } else if (CHAR_AT == c && isalpha(content.character(at+1))) {
            if (at > begin)
                return Token(TOK_DATA, begin, at);
            ++at;
            while (isValidNameCharacter(content.character(at)))
                ++at;
            return Token(TOK_FUNCTION, begin, at);

        } else if (CHAR_AT == c) {
            c = content.character(++at);
            if (EOF == c)
                return Token(TOK_DATA, begin, at);
            ++at;

        } else if (CHAR_LEFT == c) {
            if (at > begin)
                return Token(TOK_DATA, begin, at);
            return Token(TOK_LEFT, begin, ++at);

        } else if (CHAR_RIGHT == c) {
            if (at > begin)
                return Token(TOK_DATA, begin, at);
            return Token(TOK_RIGHT, begin, ++at);

        } else if ('\n' == c) {
            size_t nLineFeeds = 1, i = at + 1;
            while (isspace(c = content.character(i))) {
                if ('\n' == c)
                    ++nLineFeeds;
                ++i;
            }
            if (nLineFeeds > 1) {
                if (at > begin)
                    return Token(TOK_DATA, begin, at);
                at = i;
                return Token(TOK_BLANK_LINE, begin, i);
            } else {
                at = i;
            }

        } else {
            ++at;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Function
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SAWYER_EXPORT const std::string&
Function::name() const {
    return name_;
}

SAWYER_EXPORT bool
Function::isMacro() const {
    return isMacro_;
}

SAWYER_EXPORT Function::Ptr
Function::arg(const std::string &name) {
    ASSERT_require(nOptionalArgs() == 0);
    formals_.push_back(FormalArg(name));
    return sharedFromThis();
}

SAWYER_EXPORT Function::Ptr
Function::arg(const std::string &name, const std::string &dflt) {
    formals_.push_back(FormalArg(name, dflt));
    return sharedFromThis();
}

SAWYER_EXPORT Function::Ptr
Function::ellipsis(size_t n) {
    ellipsis_ = n;
    return sharedFromThis();
}

SAWYER_EXPORT size_t
Function::nRequiredArgs() const {
    size_t retval = 0;
    for (size_t i = 0; i<formals_.size() && formals_[i].isRequired(); ++i)
        ++retval;
    return retval;
}

SAWYER_EXPORT size_t
Function::nOptionalArgs() const {
    size_t retval = 0;
    for (size_t i = formals_.size(); i > 0 && formals_[i-1].isOptional(); --i)
        ++retval;
    return retval;
}

SAWYER_EXPORT size_t
Function::nAdditionalArgs() const {
    return ellipsis_;
}

SAWYER_EXPORT size_t
Function::maxArgs() const {
    return nRequiredArgs() + nOptionalArgs() + nAdditionalArgs();
}

SAWYER_EXPORT void
Function::validateArgs(std::vector<std::string> &actuals /*in,out*/, TokenStream &/*tokens*/) const {
    if (actuals.size() < nRequiredArgs()) {
        throw SyntaxError("not enough arguments for \"" + name() + "\""
                          "; got " + boost::lexical_cast<std::string>(actuals.size()) +
                          " but need " + (nOptionalArgs() + nAdditionalArgs() > 0 ? "at least " : "") +
                          boost::lexical_cast<std::string>(nRequiredArgs()));
    }

    if (actuals.size() > maxArgs()) {
        throw SyntaxError("too many arguments for \"" + name() + "\""
                          "; got " + boost::lexical_cast<std::string>(actuals.size()) +
                          " but only " + boost::lexical_cast<std::string>(maxArgs()) + " allowed");
    }

    for (size_t i = actuals.size(); i < formals_.size(); ++i)
        actuals.push_back(*formals_[i].dflt);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Predefined functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SAWYER_EXPORT std::string
StaticContent::eval(Grammar&, const std::vector<std::string> &args) {
    ASSERT_always_require(args.empty());                // so args is always used
    return resultString_;
}

SAWYER_EXPORT std::string
Error::eval(Grammar&, const std::vector<std::string> &args) {
    ASSERT_require(args.size() == 1);
    throw SyntaxError(args[0]);
}

SAWYER_EXPORT std::string
Quote::eval(Grammar&, const std::vector<std::string> &args) {
    return boost::join(args, "");
}

SAWYER_EXPORT std::string
Eval::eval(Grammar &grammar, const std::vector<std::string> &args) {
    std::string s = grammar.unescape(boost::join(args, ""));
    return grammar(s);
}

SAWYER_EXPORT std::string
IfEq::eval(Grammar &grammar, const std::vector<std::string> &args) {
    ASSERT_require(args.size() == 4);
    std::string v1 = grammar.unescape(args[0]);
    std::string v2 = grammar.unescape(args[1]);
    return grammar(v1==v2 ? args[2] : args[3]);
}

SAWYER_EXPORT std::string
Concat::eval(Grammar &/*grammar*/, const std::vector<std::string> &args) {
    return boost::join(args, "");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Reflow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SAWYER_EXPORT Reflow&
Reflow::operator++() {
    ++indentLevel_;
    return *this;
}

SAWYER_EXPORT Reflow&
Reflow::operator--() {
    if (indentLevel_ > 0)
        --indentLevel_;
    return *this;
}

SAWYER_EXPORT void
Reflow::emitIndentation() {
    if (0 == column_) {
        for (size_t i=0; i<indentLevel_; ++i)
            out_ <<indentation_;
        column_ = indentation_.size() * indentLevel_;
        spaces_ = "";
    }
}

SAWYER_EXPORT void
Reflow::emitAccumulated() {
    if (nonspaces_.empty()) {
        // emit nothing
    } else if (column_ == 0) {
        emitIndentation();
        out_ <<nonspaces_;
        column_ += nonspaces_.size();
    } else if (column_ + spaces_.size() + nonspaces_.size() > pageWidth_) {
        emitNewLine();
        emitIndentation();
        out_ <<nonspaces_;
        column_ += nonspaces_.size();
    } else {
        out_ <<spaces_ <<nonspaces_;
        column_ += spaces_.size() + nonspaces_.size();
    }
    spaces_ = nonspaces_ = "";
}

SAWYER_EXPORT Reflow&
Reflow::lineBreak() {
    emitAccumulated();
    if (column_ > 0)
        emitNewLine();
    return *this;
}

SAWYER_EXPORT void
Reflow::emitNewLine() {
    out_ <<"\n";
    column_ = 0;
}

SAWYER_EXPORT Reflow&
Reflow::operator()(const std::string &s) {
    BOOST_FOREACH (char ch, s) {
        if ('\n'==ch) {
            ch = ' ';
            ++nLineFeeds_;
            if (2 == nLineFeeds_) {
                emitAccumulated();
                emitNewLine();
                emitNewLine();
            }
        } else {
            nLineFeeds_ = 0;
        }

        if (isspace(ch)) {
            if (!nonspaces_.empty())
                emitAccumulated();
            spaces_ += ch;
        } else {
            nonspaces_ += ch;
        }
    }
    return *this;
}

SAWYER_EXPORT std::string
Reflow::toString() {
    emitAccumulated();
    if (column_ != 0)
        emitNewLine();
    return out_.str();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Grammar
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SAWYER_EXPORT Grammar&
Grammar::with(const Function::Ptr &f) {
    functions_.insert(f->name(), f);
    return *this;
}

SAWYER_EXPORT std::string
Grammar::operator()(const std::string &s) {
    static size_t callLevel = 0;

    struct CallLevel {
        size_t &callLevel;
        CallLevel(size_t &n): callLevel(n) { ++callLevel; }
        ~CallLevel() { ASSERT_require(callLevel>0); --callLevel; }
    } called(callLevel);

    ErrorLocation eloc;
    try {
        TokenStream tokens(s);
        ErrorLocation::Trap t(eloc, tokens, 1==callLevel ? "top level" : "recursive eval");
        std::string retval = unescape(eval(tokens, eloc));
        t.passed();
        ASSERT_require(callLevel > 0);
        return retval;
    } catch (const SyntaxError &e) {
        throw SyntaxError(e.what() + std::string("\n") + eloc.toString());
    }
}

SAWYER_EXPORT std::string
Grammar::eval(TokenStream &tokens, ErrorLocation &eloc) {
    std::string retval;
    while (!tokens.atEof()) {
        retval += evalArgument(tokens, eloc, LEAVE);
        if (tokens.isa(TOK_RIGHT))
            throw SyntaxError("unexpected end-of-argument");
    }
    return retval;
}

// class method
SAWYER_EXPORT std::string
Grammar::unescape(const std::string &s) {
    std::ostringstream out(std::ios::out | std::ios::binary);
    std::ostream_iterator<char, char> oi(out);
    boost::regex re("(@([@{}]))|(@~)");
    const char *fmt = "(?1\\2)(?3)";
    boost::regex_replace(oi, s.begin(), s.end(), re, fmt, boost::match_default|boost::format_all);
    return out.str();
}

// class method
SAWYER_EXPORT std::string
Grammar::escape(const std::string &s) {
    std::string retval;
    BOOST_FOREACH (char ch, s) {
        if (strchr("@{}", ch)) {
            retval += std::string("@") + ch;
        } else {
            retval += ch;
        }
    }
    return retval;
}

SAWYER_EXPORT std::string
Grammar::readArgument(TokenStream &tokens, ErrorLocation &/*eloc*/, bool requireRight) const {
    std::string retval;
    size_t depth = 0;
    for (/*void*/; !tokens.atEof(); tokens.consume()) {
        if (tokens.isa(TOK_LEFT)) {
            ++depth;
        } else if (tokens.isa(TOK_RIGHT)) {
            if (0 == depth)
                break;
            --depth;
        }
        retval += tokens.lexeme();
    }
    if (requireRight) {
        if (!tokens.isa(TOK_RIGHT))
            throw SyntaxError("end-of-argument expected");
        tokens.consume();
    }
    if (depth > 0)
        throw SyntaxError("expected end-of-argument marker");
    return retval;
}

SAWYER_EXPORT std::string
Grammar::evalArgument(TokenStream &tokens, ErrorLocation &eloc, bool requireRight) {
    std::string retval, data;
    size_t depth = 0;
    while (!tokens.atEof()) {
        if (tokens.isa(TOK_LEFT)) {
            ++depth;
            retval += tokens.lexeme();
            tokens.consume();
        } else if (tokens.isa(TOK_RIGHT)) {
            if (0 == depth)
                break;
            --depth;
            retval += tokens.lexeme();
            tokens.consume();
        } else if (tokens.isa(TOK_FUNCTION)) {
            retval += evalFunction(tokens, eloc);
        } else {
            retval += tokens.lexeme();
            tokens.consume();
        }
    }
    if (requireRight) {
        if (!tokens.isa(TOK_RIGHT))
            throw SyntaxError("end-of-argument expected");
        tokens.consume();
    }
    if (depth > 0)
        throw SyntaxError("expected end-of-argument marker");
    return retval;
}

SAWYER_EXPORT std::string
Grammar::evalFunction(TokenStream &tokens, ErrorLocation &eloc) {
    ASSERT_require(tokens.isa(TOK_FUNCTION));
    std::string funcName = tokens.lexeme();
    ASSERT_require(funcName.size() >= 2 && '@' == funcName[0]);
    funcName = funcName.substr(1);
    tokens.consume();

    // Get the function declaration
    const Function::Ptr func = functions_.getOrDefault(funcName);
    if (!func)
        throw SyntaxError("function \"" + funcName + "\" is not declared");

    // Parse the actual arguments
    std::vector<std::string> actuals;
    while (tokens.isa(TOK_LEFT)) {
        tokens.consume();
        if (func->isMacro()) {
            actuals.push_back(readArgument(tokens, eloc, CONSUME));
        } else {
            actuals.push_back(evalArgument(tokens, eloc, CONSUME));
        }
    }
    func->validateArgs(actuals, tokens);

    ErrorLocation::Trap t(eloc, tokens, "in function \"" + funcName + "\"");
    std::string retval = func->eval(*this, actuals);
    t.passed();
    return retval;
}

} // namespace
} // namespace
} // namespace
