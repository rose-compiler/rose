#ifndef Rosebud_Ast_H
#define Rosebud_Ast_H

#include <Rosebud/BasicTypes.h>

#include <list>
#include <memory>
#include <string>
#include <vector>

namespace Rosebud {

/** Abstract syntax tree. */
namespace Ast {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Error types
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Base class for errors related to the AST. */
class Error: public std::runtime_error {
public:
    NodePtr node;
    Error(const std::string &mesg, const NodePtr&);
};

/** Error when attaching a node to a tree and the node is already attached somewhere else.
 *
 *  If the operation were allowed to continue without throwing an exception, the AST would no longer be a tree. */
class AttachmentError: public Error {
public:
    explicit AttachmentError(const NodePtr&);
};

/** Error when attaching a node to a tree would cause a cycle.
 *
 *  If the operation were allowed to continue without throwing an exception, the AST would no longer be a tree. */
class CycleError: public Error {
public:
    explicit CycleError(const NodePtr&);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// General types used by many nodes
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ParentEdge;

// Internal. The only purpose of this class is so that the ChildEdge has permission to change the parent pointer in an
// ParentEdge.
class ParentEdgeAccess {
protected:
    void resetParent(ParentEdge&);
    void setParent(ParentEdge&, Node&);
};

/** Points from a child to a parent in the AST. */
class ParentEdge {
    Node &child_;                                       // child node owning this edge

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
    Node *parent_ = nullptr;

public:
    ParentEdge() = delete;
    explicit ParentEdge(const ParentEdge&) = delete;
    ParentEdge& operator=(const ParentEdge&) = delete;

    ~ParentEdge();
    explicit ParentEdge(Node &child);

    /** Return the parent if there is one, else null. */
    NodePtr operator()() const;
    NodePtr operator->() const;

    bool operator==(const std::shared_ptr<Node>&) const;

    explicit operator bool() const {
        return parent_ != nullptr;
    }
private:
    friend class ParentEdgeAccess;
    void reset();
    void set(Node&);
};

/** A parent-to-child edge in the abstract syntax tree. */
template<class T>
class ChildEdge: protected ParentEdgeAccess {
    Node &parent_;                                      // parent owning this child edge
    std::shared_ptr<T> child_;

public:
    ChildEdge() = delete;
    ChildEdge(const ChildEdge&) = delete;
    ChildEdge& operator=(const ChildEdge&) = delete;

    ~ChildEdge();
    explicit ChildEdge(Node &parent);
    ChildEdge(Node &parent, const std::shared_ptr<T> &child);

    const std::shared_ptr<T>& operator->() const;
    const std::shared_ptr<T>& operator()() const;

    bool operator==(const std::shared_ptr<Node>&) const;

    ChildEdge& operator=(const std::shared_ptr<T> &child);

    explicit operator bool() const {
        return child_ != nullptr;
    }
};

/** Base class for nodes in the abstract syntax tree. */
class Node: public std::enable_shared_from_this<Node> {
public:
    using Ptr = NodePtr;

public:
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

/** Node that points to an ordered sequence of indexable children. */
template<class T>
class ListNode: public Node {
public:
    using Ptr = std::shared_ptr<ListNode>;

private:
    using EdgeVector = std::vector<std::unique_ptr<ChildEdge<T>>>;
    EdgeVector elmts_;

public:
    class iterator {
        friend class ListNode;
        typename EdgeVector::iterator base_;
        iterator() = delete;
        iterator(typename EdgeVector::iterator base)
            : base_(base) {}

    public:
        iterator& operator++() {
            ++base_;
            return *this;
        }

        iterator operator++(int) {
            auto temp = *this;
            ++base_;
            return temp;
        }

        iterator& operator--() {
            --base_;
            return *this;
        }

        iterator operator--(int) {
            auto temp = *this;
            --base_;
            return temp;
        }

        iterator operator+=(size_t n) {
            base_ += n;
            return *this;
        }

        iterator operator-=(size_t n) {
            base_ -= n;
            return *this;
        }

        ChildEdge<T>& operator[](size_t n) {
            ASSERT_not_null(base_[n]);
            return *base_[n];
        }

        ChildEdge<T>& operator*() {
            ASSERT_not_null(*base_);
            return **base_;
        }

        ChildEdge<T>* operator->() {
            ASSERT_not_null(*base_);
            return &**base_;
        }

        iterator& operator=(const iterator &other) {
            base_ = other.base_;
            return *this;
        }

        bool operator==(const iterator &other) const {
            return base_ == other.base_;
        }

        bool operator!=(const iterator &other) const {
            return base_ != other.base_;
        }
    };

protected:
    ListNode() {}

public:
    static std::shared_ptr<ListNode> instance() {
        return std::shared_ptr<ListNode>(new ListNode);
    }

    bool empty() const {
        return elmts_.empty();
    }

    size_t size() const {
        return elmts_.size();
    }

    ChildEdge<T>& push_back(const std::shared_ptr<T>& elmt) {
        elmts_.push_back(std::make_unique<ChildEdge<T>>(*this, elmt));
        return *elmts_.back();
    }

    NodePtr pop_back() {
        ASSERT_forbid(elmts_.empty());
        NodePtr retval = (*elmts_.back())();
        elmts_.pop_back();
        return retval;
    }

    ChildEdge<T>& operator[](size_t i) {
        return *elmts_.at(i);
    }

    ChildEdge<T>& at(size_t i) {
        return *elmts_.at(i);
    }

    iterator begin() {
        return iterator(elmts_.begin());
    }

    //typename const_iterator begin() const {
    //    return elmts_.begin();
    //}

    iterator end() {
        return iterator(elmts_.end());
    }

    //typename const_iterator end() const {
    //    return elmts_.end();
    //}

    ChildEdge<T>& front() {
        ASSERT_forbid(elmts_.empty());
        return *elmts_.front();
    }

    ChildEdge<T>& back() {
        ASSERT_forbid(elmts_.empty());
        return *elmts_.back();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AST nodes specific to Rosebud
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Node that holds a sequence of consecutive tokens from an input file. */
class TokenList: public Node {
public:
    using Ptr = TokenListPtr;

public:
    std::vector<Token> tokens;

protected:
    TokenList();

public:
    static Ptr instance();
    bool empty() const;
    size_t size() const;
    void push_back(const Token &token);
    std::string string();                               // node must have a File ancestor
    std::string string(const FilePtr&);
    std::vector<Token>::iterator begin() {
        return tokens.begin();
    }
    std::vector<Token>::iterator end() {
        return tokens.end();
    }
};

/** Information about C preprocessor conditional compilation directives. */
class CppStack: public Node {
public:
    using Ptr = CppStackPtr;

    enum class Type {
        IF,                                             /** #if, #ifdef, or #ifndef. */
        ELSE,                                           /** #else or #elif */
        END,                                            /** #endif */
        OTHER                                           /** Not a conditional compilation directive. */
    };

    /** Conditional compilation directive. */
    struct Directive {
        Type type;                                      /** Type of conditional compilation directive. */
        Token token;                                    /** Token representing the conditional compilation directive. */
        std::string lexeme;                             /** Cached lexeme. */
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
    CppStack();

public:
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

/** An argument list is a sequence of token sequences. */
using ArgumentList = ListNode<TokenList>;

/** An attribute has a name and optional argument list. */
class Attribute: public Node {
public:
    using Ptr = AttributePtr;
public:
    std::string fqName;                                 // fully qualified name
    std::vector<Token> nameTokens;                      // one or more tokens that form the name
    ChildEdge<ArgumentList> arguments;                  // arguments; non-null if an argument list is present even if it's empty
protected:
    Attribute();
public:
    static Ptr instance();
    static Ptr instance(const std::string &fqName, const std::vector<Token> &nameTokens);
};

/** An ordered sequence of attributes with their arguments. */
using AttributeList = ListNode<Attribute>;

/** Base class for class and property definitions. */
class Definition: public Node {
public:
    using Ptr = DefinitionPtr;
public:
    Token startToken;                                   // the first token of the definition
    std::string name;
    Token nameToken;
    std::string doc;                                    // includes leading indentation for first line
    Token docToken;
    ChildEdge<CppStack> cppStack;
    std::string priorText;                              // non-token text (other than the doxygen comment) prior to definition
protected:
    Definition();
};

/** A property has a name, type, initializer, and attributes. */
class Property: public Definition {
public:
    using Ptr = PropertyPtr;
public:
    ChildEdge<TokenList> cType;
    ChildEdge<TokenList> cInit;
    ChildEdge<AttributeList> attributes;
    Sawyer::Optional<std::string> dataMemberName;
    Sawyer::Optional<std::vector<std::string>> accessorNames;
    Sawyer::Optional<std::vector<std::string>> mutatorNames;

protected:
    Property();

public:
    static Ptr instance();

    AttributePtr findAttribute(const std::string &fqName);
};

/** An ordered sequence of properties. */
using PropertyList = ListNode<Property>;

/** A class has a name and list of properties. */
class Class: public Definition {
public:
    using Ptr = ClassPtr;

    /** A list of visibilities and class names used for class inheritance. */
    using Inheritance = std::vector<std::pair<std::string /*visibility*/, std::string/*name*/>>;
public:
    ChildEdge<PropertyList> properties;
    Inheritance inheritance;
    std::string endText;                                // text from last property to end of class definition

protected:
    Class();

public:
    static Ptr instance();
};

/** An ordered sequence of class definitions. */
using ClassList = ListNode<Class>;

/** An input file. */
class File: public Node {
public:
    using Ptr = FilePtr;

public:
    TokenStream stream_;
    ChildEdge<ClassList> classes;
    std::string endText;                                // text from last class definition to end of file

protected:
    File() = delete;

    File(const std::string &name);

public:
    static Ptr instance(const std::string &name);

    std::string name();

    TokenStream& tokenStream();

    const Token token(size_t position = 0);

    // When checking whether a token's lexeme matches a string, use lexemeMatches instead.
    std::string lexeme(size_t position = 0);
    std::string lexeme(const Token&);

    // Token lexeme matching
    bool matches(const Token&, const char*);
    bool matches(const Token&, const std::string&);
    bool matches(size_t tokenPosition, const char*);
    bool matches(size_t tokenPosition, const std::string&);
    bool matches(const char*);
    bool matches(const std::string&);
    bool matchesAny(size_t tokenPosition, const std::vector<std::string>&);
    bool startsWith(const Token&, const char*);

    Token consume();
    void consume(size_t n);

    std::string content(const std::vector<Token>&, Expand);

    std::string trimmedContent(size_t begin, size_t end);
    std::string trimmedContent(size_t begin, size_t end, const Token &exclude);

    void emitContext(std::ostream&, const Token &first, const Token &locus, const Token &last);
};

/** An ordered list of files. */
using FileList = ListNode<File>;

/** A project is settings and a list of files. */
class Project: public Node {
public:
    using Ptr = ProjectPtr;

public:
    ChildEdge<FileList> files;

protected:
    Project();

public:
    static Ptr instance();

    std::vector<ClassPtr> allClassesFileOrder();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Template implementations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
ChildEdge<T>::operator==(const std::shared_ptr<Node> &other) const {
    return child_ == other;
}

template<class T>
ChildEdge<T>&
ChildEdge<T>::operator=(const std::shared_ptr<T> &child) {
    if (child != child_) {
        // Check for errors
        if (child) {
            if (child->parent)
                throw AttachmentError(child);
            parent_.traverseUpward([&child](const NodePtr &node) {
                if (child == node) {
                    throw CycleError(child);
                } else {
                    return false;
                }
            });
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

} // namespace
} // namespace
#endif
