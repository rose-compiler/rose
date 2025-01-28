// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://gitlab.com/charger7534/sawyer.git.




#ifndef Sawyer_Document_Markup_H
#define Sawyer_Document_Markup_H

#include <Sawyer/Exception.h>
#include <Sawyer/Lexer.h>
#include <Sawyer/LineVector.h>
#include <Sawyer/Map.h>
#include <Sawyer/Sawyer.h>

#include <boost/algorithm/string/join.hpp>
#include <boost/regex.hpp>

namespace Sawyer {

/** Documentation-related entities.
 *
 *  As far as C++ support libraries go, %Sawyer has rather extensive capabilities for generating documentation on the fly. This
 *  namespace encapsulates most of that functionality. */
namespace Document {

/** A system for embedding function calls in text.
 *
 *  This namespace has functions for marking up text with function calls. A function call looks like
 *  @@<em>foo</em>{<em>arg1</em>}{<em>arg2</em>}... where <em>foo</em> is the name of the function and its arguments are each
 *  enclosed in curly braces.  A function declaration for "foo" must be inserted into the grammar before "@@foo" can be
 *  recognized. */
namespace Markup {

class Grammar;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Token
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum TokenType {
    TOK_DATA,                                           // anything that's not markup
    TOK_FUNCTION,                                       // function name, like "code" scanned from "@code"
    TOK_LEFT,                                           // unescaped left paren (escaped are part of TOK_DATA)
    TOK_RIGHT,                                          // unescaped right paren
    TOK_BLANK_LINE                                      // one or more lines with no non-white-space
};

typedef Lexer::Token<TokenType> Token;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      TokenStream
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SAWYER_EXPORT TokenStream: public Lexer::TokenStream<Token> {
    static const char CHAR_LEFT = '{';
    static const char CHAR_RIGHT = '}';
    static const char CHAR_AT = '@';

public:
    explicit TokenStream(const std::string &s): Lexer::TokenStream<Token>(s) {}
    Token scanNextToken(const Container::LineVector &content, size_t &at /*in,out*/);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Function
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Base class for markup functions. */
class SAWYER_EXPORT Function: public SharedObject, public SharedFromThis<Function> {
public:
    /** Reference-counting pointer to markup function. */
    typedef SharedPointer<Function> Ptr;

private:
    // Declaration for a formal argument
    struct FormalArg {
        std::string name;                               // name of argument for diagnostics and debugging
        Optional<std::string> dflt;                     // default value

        explicit FormalArg(const std::string &name, const Optional<std::string> &dflt = Sawyer::Nothing())
            : name(name), dflt(dflt) {}

        bool isRequired() const { if (dflt) return false; else return true; }
        bool isOptional() const { return !isRequired(); }
    };

    std::string name_;                                  // name of function for diagnostics and debugging
    bool isMacro_;                                      // if true, then don't evaluate args before calling
    std::vector<FormalArg> formals_;                    // declarations of formal arguments
    size_t ellipsis_;                                   // max number of additional args

protected:
    explicit Function(const std::string &name, bool evalArgs = true)
        : name_(name), isMacro_(!evalArgs), ellipsis_(0) {}

public:
    /** Function name.
     *
     *  The name does not include the "@" character. */
    const std::string& name() const;

    /** Whether declaration is for a macro.
     *
     *  A macro differs from a function in only one regard: its arguments are not evaluated before invoking the macro. */
    bool isMacro() const;

    /** Declare a required argument.
     *
     *  Required arguments must come before optional arguments. */
    Ptr arg(const std::string &name);

    /** Declare an optional argument.
     *
     *  Optional arguments have a default value that will be passed as the actual argument if that actual argument isn't
     *  present at the call site. Optional arguments must come after all required arguments. For example, if a function
     *  declares one required argument and one optional argument with the default value "xxx", then if the function is called
     *  as "@foo{aaa}{bbb}" then "bbb" is passed as the actual argument, and if called as "@foo{aaa}" with no second argument
     *  then "xxx" is passed as the actual argument. */
    Ptr arg(const std::string &name, const std::string &dflt);

    /** Declare additional arguments.
     *
     *  A function can take up to @p n additional arguments. If these arguments are present at the call site then they're
     *  passed to the function, otherwise the function's actual argument list is shorter. */
    Ptr ellipsis(size_t n = (size_t)(-1));

    /** Number of required arguments. */
    size_t nRequiredArgs() const;

    /** Number of optional arguments.
     *
     *  If an optional argument is not present at the call site, then an actual argument is created with the declared default
     *  value and passed to the function. Optional arguments are different than "additioanl" arguments. */
    size_t nOptionalArgs() const;

    /** Max number of additional arguments.
     *
     *  This is the maximum number of additional arguments which, if present at the call site, are passed to the function and
     *  which if not present are not passed.  This is different than optional arguments. */
    size_t nAdditionalArgs() const;

    /** Max number of actual arguments possible. */
    size_t maxArgs() const;

    /** Check and adjust actual arguments.
     *
     *  Checks that the actual argument list has enough values to satisfy the functions required arguments, then adds any
     *  default values necessary to pad out the optional arguments. Throws an exception if there is not the right number of
     *  arguments. The token stream is only for error location information. */
    void validateArgs(std::vector<std::string> &actuals /*in,out*/, TokenStream&) const;

    /** How to evaluate this function or macro. */
    virtual std::string eval(Grammar&, const std::vector<std::string> &actuals) = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Errors and exceptions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Information about the location of an exception. */
class SAWYER_EXPORT ErrorLocation {
    friend class Grammar;

    // Stack frame where error occurred
    struct Frame {
        std::string name;                               // like "in @foo at name_of_input"
        size_t lineIdx;                                 // zero-origin line number
        size_t offset;                                  // beginning position within the line
        std::string input;                              // line of input causing error
        Frame(TokenStream &where, const std::string &mesg);
    };

    // Use the destructor to add a record to the stack frame. We could have used try/catch but that would interfere with
    // debugging the exception.  The alternative is that each function that could add a stack frame does so like this:
    //     void some_function(ErrorLocation &eloc, TokenStream &tokens, ...) {
    //         Trap trap(eloc, tokens);
    //         do_something_that_might_throw();
    //         trap.passed();
    //     }
    //
    // Any failure to reach trap.passed() will add a stack frame to the location.
    class Trap {
        ErrorLocation &eloc_;
        TokenStream &tokens_;
        std::string mesg_;
        bool passed_;

    public:
        Trap(ErrorLocation &eloc, TokenStream &tokens, const std::string &mesg)
            : eloc_(eloc), tokens_(tokens), mesg_(mesg), passed_(false) {}
        ~Trap() { if (!passed_) eloc_.push(Frame(tokens_, mesg_)); }
        void passed() { passed_ = true; }
    };

private:
    std::vector<Frame> frames_;

    void push(const Frame &frame) {
        frames_.push_back(frame);
    }

public:
    /** Convert an error location to an error string. */
    std::string toString() const;
};

/** Syntax error when parsing markup. */
class SAWYER_EXPORT SyntaxError: public Sawyer::Exception::SyntaxError {
    ErrorLocation eloc_;
public:
    ~SyntaxError() throw () {}

    /** Syntax error. */
    SyntaxError(const std::string &mesg)
        : Sawyer::Exception::SyntaxError(mesg) {}

    /** Set an error location. */
    void errorLocation(const ErrorLocation &eloc) {
        eloc_ = eloc;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Predefined functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** A function that inserts a string.
 *
 *  When this function is seen in markup it is replaced by a string that is set when the function is declared. */
class SAWYER_EXPORT StaticContent: public Function {
    std::string resultString_;
protected:
    StaticContent(const std::string &name, const std::string &resultString)
        : Function(name), resultString_(resultString) {}
public:
    /** Create a new instance. See class for details. */
    static Ptr instance(const std::string &name, const std::string str) {
        return Ptr(new StaticContent(name, str));
    }
    std::string eval(Grammar&, const std::vector<std::string> &args) override;
};

/** Function that generates an error message.
 *
 *  This function generates an error. If given an argument, the argument is the text of the error message, otherwise it
 *  uses the default value set at declaration time. */
class SAWYER_EXPORT Error: public Function {
protected:
    Error(const std::string &name): Function(name) {}
public:
    /** Create a new instance. See class for details. */
    static Ptr instance(const std::string &name, const std::string dfltMesg = "error") {
        return Ptr(new Error(name))->arg("message", dfltMesg);
    }
    std::string eval(Grammar&, const std::vector<std::string> &args) override;
};

/** Function that quotes its arguments.
 *
 *  This function quotes its arguments. Any function calls in the arguments are not expanded, but curly braces must still
 *  balance correctly in order for the argument to be parsed. */
class SAWYER_EXPORT Quote: public Function {
protected:
    Quote(const std::string &name): Function(name, false) {}
public:
    /** Create a new instance. See class for details. */
    static Ptr instance(const std::string &name) {
        return Ptr(new Quote(name))->ellipsis();
    }
    std::string eval(Grammar&, const std::vector<std::string> &args) override;
};

/** Evaluate arguments a second time.
 *
 *  All arguments are evaluated once (like for normal functions) and are concatenated. The concatenation is then evaluated
 *  recursively and its result becomes the result of this function.  Escaped characters in the concatenation are unescaped
 *  before it is evaluated. */
class SAWYER_EXPORT Eval: public Function {
protected:
    Eval(const std::string &name): Function(name) {}
public:
    /** Create a new instance. See class for details. */
    static Ptr instance(const std::string &name) {
        return Ptr(new Eval(name))->ellipsis();
    }
    std::string eval(Grammar &grammar, const std::vector<std::string> &args) override;
};

/** An "if" statement.
 *
 *  This function compares its first two arguments and if they are equal it evaluates the third argument, otherwise it
 *  evaluates the fourth argument. The fourth argument is optional and defaults to the emtpy string. */
class SAWYER_EXPORT IfEq: public Function {
protected:
    IfEq(const std::string &name): Function(name, false) {}
public:
    static Ptr instance(const std::string &name) {
        return Ptr(new IfEq(name))->arg("val1")->arg("val2")->arg("if_part")->arg("else_part", "");
    }
    std::string eval(Grammar &grammar, const std::vector<std::string> &args) override;
};

/** No-operation.
 *
 *  This function simply concatenates its arguments. */
class SAWYER_EXPORT Concat: public Function {
protected:
    Concat(const std::string &name): Function(name) {}
public:
    static Ptr instance(const std::string &name) {
        return Ptr(new Concat(name))->ellipsis();
    }
    std::string eval(Grammar &grammar, const std::vector<std::string> &args) override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Reflow
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** A class that can reflow and indent paragraphs.
 *
 *  This class acts as an input stream in that it processes calls to its function operator to accumulate text. The accumulated
 *  text is indented according to the current indentation level, and reflowed so that it fills the current page width. Isolated
 *  linefeeds are ignored, and consecutive linefeeds (no matter how many) result in a single blank line in the output. */
class SAWYER_EXPORT Reflow {
    size_t indentLevel_;
    std::string indentation_;                           // string for one level of indentation
    std::ostringstream out_;
    size_t column_;
    std::string spaces_;                                // accumulated white space
    std::string nonspaces_;                             // non-spaces following accumulated space
    size_t pageWidth_;
    size_t nLineFeeds_;                                 // number of consecutive linefeeds

public:
    /** Construct a reflow filter.
     *
     *  This reflow filter will use the specified page width, measured in characters. */
    explicit Reflow(size_t pageWidth = 80)
        : indentLevel_(0), indentation_("    "), column_(0), pageWidth_(pageWidth), nLineFeeds_(0) {}

    /** Property: Page width.
     *
     *  The page width is usually set in the constructor, but it can also be modified through this property. Modifying the page
     *  width during operation is allowed, but will only affect data streamed in after the change.  A minimum page width of 20
     *  columns is enforced.
     *
     * @{ */
    size_t pageWidth() const { return pageWidth_; }
    Reflow& pageWidth(size_t n) { pageWidth_ = std::max(n, (size_t)20); return *this; }
    /** @} */

    /** Property: Indentation string.
     *
     *  This is the string emitted for each level of indentation. The default is four spaces. TAB characters should be avoided
     *  since they may interfere with column calculations.
     *
     * @{ */
    const std::string& indentationString() const { return indentation_; }
    Reflow& indentationString(const std::string &s) { indentation_ = s; return *this; }
    /** @} */

    /** Increase or decrease indentation.
     *
     *  Indentation is always increased or decreased one level at a time. The indentation level will never go below zero.
     *
     *  If the indentation is changed in the middle of a line it will affect only subsequent lines.
     *
     * @{ */
    Reflow& operator++();
    Reflow& operator--();
    /** @} */

    /** Insert a line break.
     *
     *  A line break causes subsequent input to appear on the next line. Inserting multiple line breaks and/or paragraph
     *  separators (two or more consecutive linefeeds) does not produce extra blank lines in the output. */
    Reflow& lineBreak();

    /** Insert text.
     *
     *  The specified string is inserted into the reflow engine. */
    Reflow& operator()(const std::string &s);

    /** Extract the reflowed string.
     *
     *  Returns the text that has been inserted so far. */
    std::string toString();

private:
    void emitIndentation();               // indent if we're at the beginning of a line. Also discards accumulated white space.
    void emitAccumulated();               // optionally indent and emit accumulated space and non-space
    void emitNewLine();                   // advance to the next line without emitting accumulated text or indentation.
};
    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Grammar
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Grammar declaration. */
class SAWYER_EXPORT Grammar {
    Container::Map<std::string, Function::Ptr> functions_;         // functions indexed by their names
    static const bool CONSUME = true;
    static const bool LEAVE = false;

public:
    virtual ~Grammar() {}

    /** Insert function.
     *
     *  Also removes any previously declared function with the same name. */
    Grammar& with(const Function::Ptr&);

    /** Evaluate an entire string. */
    virtual std::string operator()(const std::string &s);

    /** Expand escape sequences "@@", "@{" and "@}". */
    static std::string unescape(const std::string &s);

    /** Escape all special "@", "{", and "}". */
    static std::string escape(const std::string &s);
    
private:
    // Evaluate an entire token stream. Throws an exception if an error occurs.
    std::string eval(TokenStream &tokens, ErrorLocation&);

    // Returns a string up to (and possibly including) the next CHAR_RIGHT that is not balanced by a CHAR_LEFT encountered
    // during the scanning.  The TOK_RIGHT is consumed if requireRight is set, and an error is thrown if that token is not
    // found.
    std::string readArgument(TokenStream&, ErrorLocation&, bool requireRight) const;

    // Parse one argument by parsing up to (and possibly including) the next unbalanced TOK_RIGHT.  The TOK_RIGHT is consumed
    // if requireRight is set, and an error is thrown if that token is not found.
    std::string evalArgument(TokenStream&, ErrorLocation&, bool requireRight);
    
    // Parse one function.  The current token should be a TOK_FUNCTION.
    std::string evalFunction(TokenStream&, ErrorLocation&);
};

} // namespace
} // namespace
} // namespace

#endif
