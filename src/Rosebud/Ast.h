#ifndef Rosebud_Ast_H
#define Rosebud_Ast_H

#include <Rosebud/BasicTypes.h>

#include <Sawyer/Cached.h>
#include <Sawyer/Tree.h>

#include <list>
#include <memory>
#include <string>
#include <vector>

namespace Rosebud {

/** Abstract syntax tree.
 *
 *  This is the tree representation of the abstract syntactic structure of the Rosebud input. Each node of the tree denotes
 *  a construct appearing in an input file.
 *
 *  The root of the tree is a @ref Project node, which points to a list of @ref File nodes. Each file points to a list of
 *  (usually one) @ref Class nodes. A class is a type of (inherits from) @ref Definition and points to a list of @ref Property
 *  nodes. Each property has a data type, initial value, and points to a list of @ref Attribute nodes. Each attribute has a
 *  list of arguments that are of each a @ref TokenList node. A token list is simply a list of locations in the input file.
 *
 *  All nodes of the AST are derived from the @ref Node type which, among other things, provides a parent pointer. The parent
 *  pointer in a node is adjusted automatically when the node is assigned as a child of another node. All nodes are reference
 *  counted and are deleted automatically when they're no longer referenced.
 *
 *  Since nodes are always constructed in the heap and reference counted, their normal C++ constructors are protected. Instead, use
 *  the static @c instance member functions to construct nodes. Every node also has a @c Ptr type which is a @c std::shared_ptr<T>
 *  for that class of node.
 *
 *  For examples showing how nodes and pointers to children and parents work, see the unit Rosebud AST unit tests. Here's a
 *  quick preview:
 *
 * @code
 *  auto fileList = FileList::instance();
 *
 *  auto foo = File::instance("foo.h");
 *  auto bar = File::instance("bar.h");
 *
 *  fileList->push_back(foo);
 *  ASSERT_require(fileList->at(0) == foo);
 *  ASSERT_require(foo->parent == fileList);
 *
 *  fileList->at(0) == bar;
 *  ASSERT_require(foo->parent == nullptr);
 *  ASSERT_require(bar->parent == fileList);
 *
 *  fileList->pop_back();
 *  ASSERT_require(bar->parent == nullptr);
 * @endcode */
namespace Ast {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AST nodes specific to Rosebud
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------------------------------------------------
/** Base class of all AST nodes for Rosebud. */
class Node: public Sawyer::Tree::Vertex<Node> {
public:
    virtual ~Node() {}

    using Ptr = NodePtr;

    template<class T>
    std::shared_ptr<T> findAncestor() {
        return traverseReverse<T>([](const std::shared_ptr<T> &node, TraversalEvent) {
            return node;
        });
    }
};

//------------------------------------------------------------------------------------------------------------------------------
/** Node that holds a sequence of consecutive tokens from an input file. */
class TokenList: public Node {
public:
    /** Shared-ownership pointer. */
    using Ptr = TokenListPtr;

private:
    Sawyer::Cached<std::string> string_;                // cached string representation

public:
    /** The ordered tokens. */
    std::vector<Token> tokens;

protected:
    /** Default constructor used only by derived classes. */
    TokenList();

public:
    /** Allocating constructor. */
    static Ptr instance();

    /** True if there are no tokens in this node. */
    bool empty() const;

    /** Number of tokens. */
    size_t size() const;

    /** Insert a token at the end of the list. */
    void push_back(const Token &token);

    /** Return the text for all tokens in this list.
     *
     *  The string representation is cached so the same value is always returned. This node must have a @ref File ancestor unless
     *  a file is provided or the string is already cached. If a string is provided as an argument, it overrides the cached value.
     *
     * @{ */
    std::string string();
    std::string string(const FilePtr&);
    void string(const std::string&);
    /** @} */

    /** Iterators.
     *
     * @{ */
    std::vector<Token>::iterator begin() {
        return tokens.begin();
    }
    std::vector<Token>::iterator end() {
        return tokens.end();
    }
    /** @} */
};

//------------------------------------------------------------------------------------------------------------------------------
/** Information about C preprocessor conditional compilation directives. */
class CppStack: public Node {
public:
    /** Shared-ownership pointer. */
    using Ptr = CppStackPtr;

    /** Type of CPP directive. */
    enum class Type {
        IF,                                             /**< #if, #ifdef, or #ifndef. */
        ELSE,                                           /**< #else or #elif */
        END,                                            /**< #endif */
        OTHER                                           /**< Not a conditional compilation directive. */
    };

    /** Conditional compilation directive. */
    struct Directive {
        Type type;                                      /**< Type of conditional compilation directive. */
        Token token;                                    /**< Token representing the conditional compilation directive. */
        std::string lexeme;                             /**< Cached lexeme. */
    };

    /** The CPP directives that belong to the same level.
     *
     *  E.g., a matching #if, #elif, and #else. The first element of the pair indicates the type of directive in the first element
     *  of the list. The list is never empty. */
    using Level = std::list<Directive>;

    /** A stack of nested conditional compilation directives.
     *
     *  The earlier entries enclose the later entries. */
    using Stack = std::vector<Level>;

public:
    /** The stack of nested conditional compilation directives. */
    Stack stack;

protected:
    /** Default constructor used only by derived classes. */
    CppStack();

public:
    /** Allocating constructor. */
    static Ptr instance();

    /** Type of directive. */
    static Type type(const std::string &directive);

    /** Adjust the stack based on the C preprocessor directive.
     *
     *  Returns true if the stack was adjusted, false otherwise. */
    static bool process(const FilePtr&, const Token &directive, Stack &runningStack);

    /** Print an error message for the top of the stack if there's something wrong with it.
     *
     *  Returns true if an error was printed. */
    bool printError(const FilePtr &file) const;

    /** Emit all control flow directives in this stack. */
    void emitOpen(std::ostream&);

    /** Emit matching #endif for all control flow directives in this stack. */
    void emitClose(std::ostream&);
};

//------------------------------------------------------------------------------------------------------------------------------
/** A node that holds a list of arguments. */
class ArgumentList: public Node {
public:
    /** Shared-ownership pointer. */
    using Ptr = ArgumentListPtr;

public:
    EdgeVector<TokenList> elmts;

protected:
    ArgumentList();

public:
    /** Allocating constructor. */
    static Ptr instance();
};

//------------------------------------------------------------------------------------------------------------------------------
/** An attribute adjusting the details for a definition.
 *
 *  An attribute has a name and optional argument list and is used to adjust the behavior, traits, etc. of a class or property
 *  definition. */
class Attribute: public Node {
public:
    /** Shared-ownership pointer. */
    using Ptr = AttributePtr;

public:
    /** Fully qualified name. */
    std::string fqName;

    /** One or more tokens associated with the name. */
    std::vector<Token> nameTokens;

    /** Attribute arguments.
     *
     *  This tree edge is null unless the attribute has an argument list with zero or more arguments. */
    Edge<ArgumentList> arguments;

protected:
    /** Default constructor used only by derived classes. */
    Attribute();
public:

    /** Allocating constructor.
     *
     *  The no-argument constructor creates a node with no name. If a name is provided, it should be a fully qualified name
     *  and the @p nameTokens should be a list of input tokens for the name. The input tokens can point to the unqualified
     *  name.
     *
     * @{ */
    static Ptr instance();
    static Ptr instance(const std::string &fqName, const std::vector<Token> &nameTokens);
    /** @} */
};

//------------------------------------------------------------------------------------------------------------------------------
/** Base class for class and property definitions. */
class Definition: public Node {
public:
    /** Shared-ownership pointer. */
    using Ptr = DefinitionPtr;

public:
    /** Token at the start of the definition. */
    Token startToken;

    /** Unqualified name for the definition. */
    std::string name;

    /** Token for the definition's unqualified name. */
    Token nameToken;

    /** Doxygen documentation comment.
     *
     *  The doxygen comment is a multi-line comment string. The first line may contain spaces for leading indentation. */
    std::string doc;

    /** Token associated with the Doxygen comment.
     *
     *  Even though the @ref doc string contains leading spaces, the token does not. */
    Token docToken;

    /** C preprocessor pending conditional compilation directives.
     *
     *  This is a non-null child that contains information about the CPP conditional compilation directives that have been started
     *  but not yet closed. */
    Edge<CppStack> cppStack;

    /** Input text before the definition.
     *
     *  This is all the input text that appears before this definition and is not associated with any other definition. If there is
     *  a @ref docToken, then its text is excluded from the prior text. Furthermore, white space at the ends of lines is removed,
     *  consecutive blank lines are replaced with a single blank line, and leading and trailing blank lines are removed. */
    std::string priorText;

    /** Token describing the location of the prior text.
     *
     *  This is a generated token of type TOK_OTHER that encloses the @ref priorText value. */
    Token priorTextToken;

    /** Non-null pointer to the list of attributes controlling this property. */
    EdgeVector<Attribute> attributes;

protected:
    /** Default constructor used only by derived classes. */
    Definition();

public:
    /** Finds an attribute with the specified fully qualified name.
     *
     *  Returns a non-null pointer to the attribute if found, null otherwise. */
    AttributePtr findAttribute(const std::string &fqName);
};

//------------------------------------------------------------------------------------------------------------------------------
/** Represents a class property definition.
 *
 *  A property is somewhat like a class data member, except it has accessors and mutators. This node represents a property
 *  definition. */
class Property: public Definition {
public:
    /** Shared-ownership pointer. */
    using Ptr = PropertyPtr;

public:
    /** Optional pointer to tokens that define the property type. */
    Edge<TokenList> cType;

    /** Optional pointer to tokens that define the property's initial value. */
    Edge<TokenList> cInit;

    /** Optional data member name override.
     *
     *  This usually comes from the "Rosebud::data" attribute, but can be set directly. If there is no data member name, then the
     *  name is chosen by the backend code generator. */
    Sawyer::Optional<std::string> dataMemberName;

    /** Optional override for accessor names.
     *
     *  This usually comes from the "Rosebud::accessors" attribute, but can be set directly. An empty vector means that no accessors
     *  will be generated. Lack of a vector means that the backend should choose accessor names. */
    Sawyer::Optional<std::vector<std::string>> accessorNames;

    /** Optional override for mutator names.
     *
     *  This usually comes from the "Rosebud::mutators" attribute, but can be set directly. An empty vector means that no mutators
     *  will be generated. Lack of a vector means that the backend should choose mutator names. */
    Sawyer::Optional<std::vector<std::string>> mutatorNames;

protected:
    /** Default constructor used only by derived classes. */
    Property();

public:
    /** Allocating constructor. */
    static Ptr instance();
};

//------------------------------------------------------------------------------------------------------------------------------
/** Represents a class definition. */
class Class: public Definition {
public:
    /** Shared-ownership pointer. */
    using Ptr = ClassPtr;

    /** A list of access specifiers and class names used for class inheritance. */
    using Inheritance = std::vector<std::pair<std::string /*visibility*/, std::string/*name*/>>;

public:
    /** Non-null list of zero or more properties. */
    EdgeVector<Property> properties;

    /** Information about base classes. */
    Inheritance inheritance;

    /** Text after the last member definition.
     *
     *  This is the input text that appears after the last property definition and before the end of the entire class definition.
     *  It ends just before the closing curly brace for the class definition. If the class has no properties, then this is the
     *  entire body of the class definition (everthing between the opening and closing curly braces, excluding the braces).
     *
     *  The text is modified by removing trailing white space from each line, replacing consecutive blank lines with a single blank
     *  line, and removing leading and trailing blank lines. */
    std::string endText;

    /** Token that encloses endText.
     *
     *  This generated token of type TOK_OTHER encloses the file content that produced `endText`. */
    Token endTextToken;

    /** If non-empty, overrides the tag name for this type. */
    std::string tag;

    /** If non-empty, the qualified namespace in which this class is defined. */
    std::string qualifiedNamespace;

protected:
    /** Default constructor used only by derived classes. */
    Class();

public:
    /** Allocating constructor. */
    static Ptr instance();
};

//------------------------------------------------------------------------------------------------------------------------------
/** An input file. */
class File: public Node {
public:
    /** Shared-ownership pointer. */
    using Ptr = FilePtr;

private:
    TokenStream stream_;

public:
    /** Non-null list of zero or more class definitions. */
    EdgeVector<Class> classes;

    /** Text after the last class definition until the end of the file.
     *
     *  The text is modified by removing trailing white space from each line, replacing consecutive blank lines with a single blank
     *  line, and removing leading and trailing blank lines. */
    std::string endText;

    /** Token that encloses endText.
     *
     *  This generated token of type TOK_OTHER encloses the file content that produced `endText`. */
    Token endTextToken;

protected:
    File() = delete;

    /** Constructor used only by derived classes. */
    File(const std::string &name);

public:
    /** Allocating constructor.
     *
     *  The specified name must be the name of an existing text file that can be opened for reading. */
    static Ptr instance(const std::string &name);

    /** Name of the input file. */
    std::string name();

    /** Token stream for the file.
     *
     *  These tokens are consumed by parsing, but the stream itself is left here as a data member because it still contains
     *  useful information such as the file name, the content of the file as text by which tokens can be converted to strings,
     *  and information about line and column numbers. */
    TokenStream& tokenStream();

    /** Current or future token.
     *
     *  Returns the current token, or one of the future tokens. */
    const Token token(size_t lookAhead = 0);

    /** Text associated with a token.
     *
     *  Returns the input text that caused the current or future token.
     *
     *  Attention: when testing whether a token matches some string, use one of the matching functions if possible instead of
     *  obtaining a string and comparing it to another string. The matching functions are more efficient.
     *
     * @{ */
    std::string lexeme(size_t position = 0);
    std::string lexeme(const Token&);
    /** @} */

    /** Test whether a token matches a string.
     *
     *  Returns true if the specified token matches a string. A token can be passed as an argument, or specified as a future
     *  token (@p lookAhead > 1), or the current token (no argument, or @p lookAhead == 0).
     *
     * @{ */
    bool matches(const Token&, const char*);
    bool matches(const Token&, const std::string&);
    bool matches(size_t lookAhead, const char*);
    bool matches(size_t lookAhead, const std::string&);
    bool matches(const char*);
    bool matches(const std::string&);
    /** @} */

    /** Test whether a token matches any text.
     *
     *  Returns true if the specified token matches any of the supplied strings. */
    bool matchesAny(size_t tokenPosition, const std::vector<std::string>&);

    /** Test whether a token starts with a certain string.
     *
     *  Returns true if the specified token begins with the specified characters. */
    bool startsWith(const Token&, const char*);

    /** Consume tokens.
     *
     *  Consume the current token, or @p n tokens. This is used during parsing, so by time a backend code generator is called all
     *  the tokens have presumably been consumed and the stream is sitting at the special EOF token. Consuming the EOF token is
     *  permissible, but has no effect.
     *
     * @{ */
    Token consume();
    void consume(size_t n);
    /** @} */

    /** Input string for token list.
     *
     *  Returns a string for the specified tokens by concatenating the strings for the individual tokens. Depending on the second
     *  argument, the returned string includes either just the token text, the text between the start of the first token and the end
     *  of the last token, or the same plus the non-token text before the first token. */
    std::string content(const std::vector<Token>&, Expand);

    /** Part of file.
     *
     *  Returns some text from the file from the starting byte offset (inclusive) to the ending byte offset (exclusive). */
    std::string content(size_t begin, size_t end);

    /** Input string for file region.
     *
     *  Returns the content of the file from character position @p begin (inclusive) to @p end (exclusive). If the @p end is less
     *  than or equal to @p begin, then an empty string is returned. The text is modified by removing trailing white space from each
     *  line, replacing consecutive blank lines with a single blank line, and removing leading and trailing blank lines. If an @p
     *  exclude token is specified, then its text is not included in the returned string.
     *
     * @{ */
    std::string trimmedContent(size_t begin, size_t end, Token &outputToken);
    std::string trimmedContent(size_t begin, size_t end, const Token &exclude, Token &outputToken);
    /** @} */

    /** Emit the context for a diagnostic message.
     *
     *  The input lines corresponding to the tokens are emitted to standard error. The part of that input corresponding to the
     *  tokens is marked by underlining it, and the start of the @p locus token is marked in a special way. */
    void emitContext(std::ostream&, const Token &first, const Token &locus, const Token &last);
};

//------------------------------------------------------------------------------------------------------------------------------
/** Root of an AST for one or more input files.
 *
 *  The project represents all the input files that were parsed. */
class Project: public Node {
public:
    /** Shared-ownership pointer. */
    using Ptr = ProjectPtr;

public:
    /** Non-null list of input files. */
    EdgeVector<File> files;

protected:
    /** Default constructor used only by derived classes. */
    Project();

public:
    /** Allocating constructor. */
    static Ptr instance();

    /** Return all classes defined in the input.
     *
     *  The returned classes are in the order they appear in a depth-first traversal of the AST, which is the same order they were
     *  encountered during parsing. */
    std::vector<ClassPtr> allClassesFileOrder();

    /** Find a class by name.
     *
     *  If the class name exists, then return a pointer to the class, otherwise return null. */
    ClassPtr findClassByName(const std::string&);
};

} // namespace
} // namespace
#endif
