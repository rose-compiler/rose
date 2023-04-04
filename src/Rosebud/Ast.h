#ifndef Rosebud_Ast_H
#define Rosebud_Ast_H

#include <Rosebud/BasicTypes.h>

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
// Error types
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Base class for errors related to the AST. */
class Error: public std::runtime_error {
public:
    /** Node that caused the error. */
    NodePtr node;

    /** Construct a new error with the specified message and causing node. */
    Error(const std::string &mesg, const NodePtr&);
};

/** Error when attaching a node to a tree and the node is already attached somewhere else.
 *
 *  If the operation were allowed to continue without throwing an exception, the AST would no longer be a tree. */
class AttachmentError: public Error {
public:
    /** Construct a new error with the node that caused the error. */
    explicit AttachmentError(const NodePtr&);
};

/** Error when attaching a node to a tree would cause a cycle.
 *
 *  If the operation were allowed to continue without throwing an exception, the AST would no longer be a tree. */
class CycleError: public Error {
public:
    /** Construct a new error with the node that caused the error. */
    explicit CycleError(const NodePtr&);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// General types used by many nodes
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ParentEdge;
template<class T> class ChildEdge;

// Internal. The only purpose of this class is so that the ChildEdge has permission to change the parent pointer in an
// ParentEdge.
class ParentEdgeAccess {
protected:
    void resetParent(ParentEdge&);
    void setParent(ParentEdge&, Node&);
};

/** Points from a child to a parent in the AST.
 *
 *  This is the pointer type that points from a node to its parent. Its value is adjusted automatically when the containing
 *  node is attached or detached as a child of some other node. The term "edge" is used instead of "pointer" because the
 *  relationship between child and parent is bidirectional.
 *
 *  A parent edge is always a data member of a node and never instantiated in other circumstances. Thus users don't normally
 *  instantiate these directly, but they do interact with them through the @c parent data member that exists for all node types. */
class ParentEdge {
    Node &child_;                                       // required child node owning this edge

    // The parent pointer is a raw pointer because it is safe to do so, and because we need to know the pointer before the parent is
    // fully constructed.
    //
    // It is safe (never dangling) because the pointer can only be changed by an ChildEdge, the ChildEdge is always a member of an
    // Node, and the parent pointer is only set to point to that node. When the parent is deleted the ChildEdge is deleted and its
    // destructor changes the parent pointer back to null.
    //
    // The parent pointer is needed during construction of the parent when the parent has some ChildEdge data members that are being
    // initialized to point to non-null children. This happens during the parent's construction, before the parent has any shared or
    // weak pointers.
    Node *parent_ = nullptr;                            // optional parent to which this edge points

public:
    // No default constructor and not copyable.
    ParentEdge() = delete;
    explicit ParentEdge(const ParentEdge&) = delete;
    ParentEdge& operator=(const ParentEdge&) = delete;

public:
    ~ParentEdge();                                      // internal use only
    explicit ParentEdge(Node &child);                   // internal use only

public:
    /** Return the parent if there is one, else null.
     *
     * @{ */
    NodePtr operator()() const;
    NodePtr operator->() const;
    /** @} */

    /** Compare the parent pointer to another pointer.
     *
     * @{ */
    bool operator==(const NodePtr&) const;
    bool operator!=(const NodePtr&) const;
    bool operator==(const ParentEdge&) const;
    bool operator!=(const ParentEdge&) const;
    template<class T> bool operator==(const ChildEdge<T>&) const;
    template<class T> bool operator!=(const ChildEdge<T>&) const;
    /** @} */

    /** True if parent is not null. */
    explicit operator bool() const {
        return parent_ != nullptr;
    }

private:
    // Used internally through ParentEdgeAccess when a ChildEdge<T> adjusts the ParentEdge
    friend class ParentEdgeAccess;
    void reset();
    void set(Node&);
};

/** A parent-to-child edge in the abstract syntax tree.
 *
 *  A parent-to-child edge is a pointer-like object that points from an parent node to a child node or nullptr. It is also
 *  responsible for adjusting the child's parent pointer. The term "edge" is used instead of "pointer" because the relationship
 *  between the parent and child is bidirectional.
 *
 *  A child edge is always a data member of a node and never instantiated in other circumstances. Thus users don't normally
 *  instanticate these directly, but they do interact with them to obtain pointers to children from a parent.
 *
 *  A ChildEdge is used to define a data member in the parent that points to a child. For instance, the following binary expression
 *  node has left-hand-side and right-hand-side children that are part of the tree.
 *
 * @code
 *  class BinaryExpression: public Expression {
 *  public:
 *      Ast::ChildEdge<Expression> lhs;
 *      Ast::ChildEdge<Expression> rhs;
 *
 *  protected:
 *      BinaryExpression()
 *          : lhs(*this), rhs(*this) {}
 *
 *      static std::shared_ptr<BinaryExpression> instance() {
 *          return std::shared_ptr<BinaryExpression>(new BinaryExpression);
 *      }
 *  };
 * @endcode */
template<class T>
class ChildEdge: protected ParentEdgeAccess {
    Node &parent_;                                      // required parent owning this child edge
    std::shared_ptr<T> child_;                          // optional child to which this edge points

public:
    // No default constructor and not copyable.
    ChildEdge() = delete;
    ChildEdge(const ChildEdge&) = delete;
    ChildEdge& operator=(const ChildEdge&) = delete;

public:
    ~ChildEdge();

    /** Construct a child edge that belongs to the specified parent.
     *
     *  When constructing a class containing a data member of this type (i.e., a tree edge that points to a child of this node), the
     *  data member must be initialized by passing @c *this as the argument.  See the example in this class documentation.
     *
     *  An optional second argument initializes the child pointer for the edge. The initialization is the same as if the child
     *  had been assigned with @c operator= later. I.e., the child must not already have a parent.
     *
     * @{ */
    explicit ChildEdge(Node &parent);
    ChildEdge(Node &parent, const std::shared_ptr<T> &child);
    /** @} */

    /** Return the child if there is one, else null.
     *
     * @{ */
    const std::shared_ptr<T>& operator->() const;
    const std::shared_ptr<T>& operator()() const;
    /** @} */

    /** Compare the child pointer to another pointer.
     *
     * @{ */
    bool operator==(const std::shared_ptr<Node>&) const;
    bool operator!=(const std::shared_ptr<Node>&) const;
    bool operator==(const ParentEdge&) const;
    bool operator!=(const ParentEdge&) const;
    template<class U> bool operator==(const ChildEdge<U>&) const;
    template<class U> bool operator!=(const ChildEdge<U>&) const;
    /** @} */

    /** Assign a pointer to a child.
     *
     *  If this edge points to an old child then that child is removed and its parent is reset. If the specified new child is
     *  non-null, then it is inserted and its parent pointer set to the parent of this edge.
     *
     *  However, if the new child already has a non-null parent, then no changes are made and a @ref AttachmentError is thrown with
     *  the error's node point to the new child. Otherwise, if the new child is non-null and is the parent or any more distant
     *  ancestor of this edge's node, then a @ref CycleError is thrown. Cycle errors are only thrown if debugging is enabled (i.e.,
     *  the CPP macro @c NDEBUG is undefined).
     *
     *  Attempting to assign one child edge object to another is a compile-time error (its operator= is not declared) because every
     *  non-null child edge points to a child whose parent is non-null, which would trigger an @ref AttachmentError. Therefore only
     *  null child edges could be assigned. But since only null child edges can be assigned, its more concise and clear to assign
     *  the null pointer directly.
     *
     * @{ */
    ChildEdge& operator=(const std::shared_ptr<T> &child);
    ChildEdge& operator=(const ParentEdge&);
    /** @} */

    /** True if child is not null. */
    explicit operator bool() const {
        return child_ != nullptr;
    }
};

/** Base class for nodes in the abstract syntax tree. */
class Node: public std::enable_shared_from_this<Node> {
public:
    /** Shared-ownership pointer to a @ref Node. */
    using Ptr = NodePtr;

public:
    /** Pointer to the parent in the tree.
     *
     *  A node's parent pointer is adjusted automatically when the node is inserted or removed as a child of another node. An
     *  invariant of this design is that whenever node A is a child of node B, then node B is a parent of node A. */
    ParentEdge parent;

public:
    virtual ~Node() {}

protected:
    Node();

public:
    /** Returns a shared pointer to this node. */
    Ptr pointer();

    /** Traverse upward following parent pointers.
     *
     *  The visitor is called for each node from the current node until the root of the tree is reached unless the visitor indicates
     *  that the traversal should end. It does so by returning a value that is true in a Boolean context, and this value becomes the
     *  return value for the entire traversal. */
    template<class Visitor>
    auto traverseUpward(const Visitor &visitor) {
        for (auto node = pointer(); node; node = node->parent()) {
            if (auto result = visitor(node))
                return result;
        }
        return decltype(visitor(NodePtr()))();
    }

    /** Traversal that finds an ancestor of a particular type. */
    template<class T>
    std::shared_ptr<T> findAncestor() {
        return traverseUpward([](const NodePtr &node) -> std::shared_ptr<T> {
                return std::dynamic_pointer_cast<T>(node);
            });
    };
};

/** Node that points to an ordered sequence of indexable children.
 *
 *  This node acts like an @c std::vector except that inserting and erasing children also adjusts the child's parent pointer. */
template<class T>
class ListNode: public Node {
private:
    using EdgeVector = std::vector<std::unique_ptr<ChildEdge<T>>>;

public:
    /** Shared-ownership pointer to nodes of this type. */
    using Ptr = std::shared_ptr<ListNode>;

    /** Type of values stored in this class. */
    using value_type = ChildEdge<T>;

    /** Size type. */
    using size_type = typename EdgeVector::size_type;

    /** Distance between elements. */
    using difference_type = typename EdgeVector::difference_type;

    /** Reference to value. */
    using reference = value_type&;

    /** Reference to cons value. */
    using const_reference = const value_type&;

    /** Pointer to value. */
    using pointer = value_type*;

    /** Pointer to const value. */
    using const_pointer = const value_type*;

private:
    EdgeVector elmts_;

public:
    /** Random access iterator to non-const edges.
     *
     *  Iterators are invalidated in the same situations as for @c std::vector. */
    class iterator {
        friend class ListNode;
        typename EdgeVector::iterator base_;
        iterator() = delete;
        iterator(typename EdgeVector::iterator base)
            : base_(base) {}

    public:
        /** Cause iterator to point to the next edge.
         *
         * @{ */
        iterator& operator++() {
            ++base_;
            return *this;
        }
        iterator operator++(int) {
            auto temp = *this;
            ++base_;
            return temp;
        }
        /** @} */

        /** Cause iterator to point to previous edge.
         *
         * @{ */
        iterator& operator--() {
            --base_;
            return *this;
        }
        iterator operator--(int) {
            auto temp = *this;
            --base_;
            return temp;
        }
        /** @} */

        /** Advance iterator in forward (or backward if negative) direction by @p n edges.
         *
         * @{ */
        iterator& operator+=(difference_type n) {
            base_ += n;
            return *this;
        }
        iterator operator+(difference_type n) const {
            iterator retval = *this;
            retval += n;
            return retval;
        }
        /** @} */

        /** Advance iterator in backward (or forward if negative) direction by @p n edges.
         *
         * @{ */
        iterator& operator-=(difference_type n) {
            base_ -= n;
            return *this;
        }
        iterator operator-(difference_type n) const {
            iterator retval = *this;
            retval -= n;
            return retval;
        }
        /** @} */

        /** Distance between two iterators. */
        difference_type operator-(const iterator &other) const {
            return other.base_ - base_;
        }

        /** Return an edge relative to the current one.
         *
         *  Returns the edge that's @p n edges after (or before if negative) the current edge. */
        ChildEdge<T>& operator[](difference_type n) {
            ASSERT_not_null(base_[n]);
            return *base_[n];
        }

        /** Return a reference to the current edge. */
        ChildEdge<T>& operator*() {
            ASSERT_not_null(*base_);
            return **base_;
        }

        /** Return a pointer to the current edge. */
        ChildEdge<T>* operator->() {
            ASSERT_not_null(*base_);
            return &**base_;
        }

        /** Make this iterator point to the same element as the @ other iterator. */
        iterator& operator=(const iterator &other) {
            base_ = other.base_;
            return *this;
        }

        /** Compare two iterators.
         *
         * @{ */
        bool operator==(const iterator &other) const {
            return base_ == other.base_;
        }
        bool operator!=(const iterator &other) const {
            return base_ != other.base_;
        }
        bool operator<(const iterator &other) const {
            return base_ < other.base_;
        }
        bool operator<=(const iterator &other) const {
            return base_ <= other.base_;
        }
        bool operator>(const iterator &other) const {
            return base_ > other.base_;
        }
        bool operator>=(const iterator &other) const {
            return base_ >= other.base_;
        }
        /** @} */
    };

protected:
    ListNode() {}

public:
    /** Allocating constructor.
     *
     *  Constructs a new node that has no children. */
    static std::shared_ptr<ListNode> instance() {
        return std::shared_ptr<ListNode>(new ListNode);
    }

    /** Test whether vector is empty.
     *
     *  Returns true if this node contains no child edges, null or otherwise. */
    bool empty() const {
        return elmts_.empty();
    }

    /** Number of child edges.
     *
     *  Returns the number of children edges, null or otherwise. */
    size_t size() const {
        return elmts_.size();
    }

    /** Reserve space so the child edge vector can grow without being reallocated. */
    void reserve(size_t n) {
        elmts_.reserve(n);
    }

    /** Reserved capacity. */
    size_t capacity() const {
        return elmts_.capacity();
    }

    /** Insert a child pointer at the end of this node.
     *
     *  If the new element is non-null, then it must satisfy all the requirements for inserting a node as a child of another
     *  node, and its parent pointer will be adjusted automatically. */
    ChildEdge<T>& push_back(const std::shared_ptr<T>& elmt) {
        elmts_.push_back(std::make_unique<ChildEdge<T>>(*this, elmt));
        return *elmts_.back();
    }

    /** Erase a child edge from the end of this node.
     *
     *  If the edge being erased points to a child, then that child's parent pointer is reset. */
    NodePtr pop_back() {
        ASSERT_forbid(elmts_.empty());
        NodePtr retval = (*elmts_.back())();
        elmts_.pop_back();
        return retval;
    }

    /** Return a reference to the nth edge.
     *
     * @{ */
    ChildEdge<T>& operator[](size_t n) {
        return *elmts_.at(n);
    }
    ChildEdge<T>& at(size_t i) {
        return *elmts_.at(i);
    }
    /** @} */

    /** Return an iterator pointing to the first edge. */
    iterator begin() {
        return iterator(elmts_.begin());
    }

    /** Return an iterator pointing to one past the last edge. */
    iterator end() {
        return iterator(elmts_.end());
    }

    /** Return a reference to the first edge. */
    ChildEdge<T>& front() {
        ASSERT_forbid(elmts_.empty());
        return *elmts_.front();
    }

    /** Return a reference to the last edge. */
    ChildEdge<T>& back() {
        ASSERT_forbid(elmts_.empty());
        return *elmts_.back();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AST nodes specific to Rosebud
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------------------------------------------------
/** Node that holds a sequence of consecutive tokens from an input file. */
class TokenList: public Node {
public:
    /** Shared-ownership pointer. */
    using Ptr = TokenListPtr;

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
     *  The no-argument version will search for the file as an ancestor node in the tree.
     *
     * @{ */
    std::string string();                               // node must have a File ancestor
    std::string string(const FilePtr&);
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
/** Ordered list of arguments.
 *
 *  An argument list is a sequence of token sequences. */
using ArgumentList = ListNode<TokenList>;

//------------------------------------------------------------------------------------------------------------------------------
/** An attribute adjusting the details for a definition.
 *
 *  An attribute has a name and optional argument list and is used to adjust the behavior, traits, etc. of a class or property
 *  definition. */
class Attribute: public Node {
public:
    /** Allocating constructor. */
    using Ptr = AttributePtr;

public:
    /** Fully qualified name. */
    std::string fqName;

    /** One or more tokens associated with the name. */
    std::vector<Token> nameTokens;

    /** Attribute arguments.
     *
     *  This tree edge is null unless the attribute has an argument list with zero or more arguments. */
    ChildEdge<ArgumentList> arguments;

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
/** An ordered sequence of attributes with their arguments. */
using AttributeList = ListNode<Attribute>;

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
    ChildEdge<CppStack> cppStack;

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
    ChildEdge<AttributeList> attributes;

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
    ChildEdge<TokenList> cType;

    /** Optional pointer to tokens that define the property's initial value. */
    ChildEdge<TokenList> cInit;

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
/** An ordered sequence of properties. */
using PropertyList = ListNode<Property>;

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
    ChildEdge<PropertyList> properties;

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

protected:
    /** Default constructor used only by derived classes. */
    Class();

public:
    /** Allocating constructor. */
    static Ptr instance();
};

//------------------------------------------------------------------------------------------------------------------------------
/** An ordered sequence of class definitions. */
using ClassList = ListNode<Class>;

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
    ChildEdge<ClassList> classes;

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

    /** Input string for file region.
     *
     *  Returns the content of the file from character position @p begin (inclusive) to @p end (exclusvie). If the @p end is less
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
/** An ordered list of files. */
using FileList = ListNode<File>;

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
    ChildEdge<FileList> files;

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
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Template implementations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T>
bool
ParentEdge::operator==(const ChildEdge<T> &other) const {
    return parent_ == other();
}

template<class T>
bool
ParentEdge::operator!=(const ChildEdge<T> &other) const {
    return parent_ != other();
}

template<class T>
ChildEdge<T>::~ChildEdge() {
    if (child_)
        resetParent(child_->parent);
}

template<class T>
ChildEdge<T>::ChildEdge(Node &parent)
    : parent_(parent) {}

template<class T>
ChildEdge<T>::ChildEdge(Node &parent, const std::shared_ptr<T> &child)
    : parent_(parent), child_(child) {
    if (child) {
        if (child->parent)
            throw AttachmentError(child);
        setParent(child->parent, parent);
    }
}

template<class T>
const std::shared_ptr<T>&
ChildEdge<T>::operator->() const {
    ASSERT_not_null(child_);
    return child_;
}

template<class T>
const std::shared_ptr<T>&
ChildEdge<T>::operator()() const {
    return child_;
}

template<class T>
bool
ChildEdge<T>::operator==(const std::shared_ptr<Node> &ptr) const {
    return child_ == ptr;
}

template<class T>
bool
ChildEdge<T>::operator!=(const std::shared_ptr<Node> &ptr) const {
    return child_ != ptr;
}

template<class T>
bool
ChildEdge<T>::operator==(const ParentEdge &other) const {
    return child_ == other();
}

template<class T>
bool
ChildEdge<T>::operator!=(const ParentEdge &other) const {
    return child_.get() != other();
}

template<class T>
template<class U>
bool
ChildEdge<T>::operator==(const ChildEdge<U> &other) const {
    return child_.get() == other.get();
}

template<class T>
template<class U>
bool
ChildEdge<T>::operator!=(const ChildEdge<U> &other) const {
    return child_.get() != other.get();
}

template<class T>
ChildEdge<T>&
ChildEdge<T>::operator=(const std::shared_ptr<T> &child) {
    if (child != child_) {
        // Check for errors
        if (child) {
            if (child->parent)
                throw AttachmentError(child);
#ifndef NDEBUG
            parent_.traverseUpward([&child](const NodePtr &node) {
                if (child == node) {
                    throw CycleError(child);
                } else {
                    return false;
                }
            });
#endif
        }

        // Unlink the child from the tree
        if (child_) {
            resetParent(child_->parent);
            child_.reset();                             // parent-to-child edge
        }

        // Link new child into the tree
        if (child) {
            setParent(child->parent, parent_);
            child_ = child;
        }
    }
    return *this;
}

template<class T>
ChildEdge<T>&
ChildEdge<T>::operator=(const ParentEdge &parent) {
    return (*this) = parent();
}

} // namespace
} // namespace
#endif
