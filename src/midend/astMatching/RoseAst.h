
#ifndef ROSE_AST_H
#define ROSE_AST_H

/*************************************************************
 * Copyright: (C) 2012 Markus Schordan                       *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include <stack>
#include "roseInternal.h"

/** Interface for iterating over an AST.
 *
 *  The iterator follows the STL iterator pattern and is implemented as a depth-first, pre-order traversal that mtaintsins its
 *  own stack. The iterator performs the exact same traversal as the traversal classes in ROSE (it is using the same underlying
 *  information).
 *
 * @code
 *  #include <RoseAst.h>
 *  SgNode *node = .... // any subtree
 *
 *  RoseAst ast(node);
 *  for (RoseAst::iterator i = ast.begin(); i != ast.end(); ++i)
 *      std::cout <<"We are here: " <<(*i)->class_name() <<"\n";
 * @endcode */
class RoseAst {
 public:
  typedef SgNode elementType;
  typedef elementType* pointer;
  typedef elementType& reference;
  typedef size_t size_type;

  // no default constructor

  /** Defines the starting node for traversal.
   *
   *  The traversal is over a subtree rooted at the specified @p root node. All operations are specific to this subtree, and
   *  the traversal does not iterate above this @p root. */
  RoseAst(SgNode* astNode); 

  /** AST iterator.
   *
   *  This is an STL-compliant forward iterator for traversing AST nodes in depth-first pre-order. The @ref withoutNullValues
   *  and @ref withNullValues functions control whether the traversal follows null child pointers (the default is that null
   *  pointers are not followed). Entire subtrees can be exluded from traversal with the @ref skipSubtreeOnForward function.
   *
   *  The iterator also provides a number of auxiliary functions for querying some structural properties of the AST w.r.t. the
   *  position in the traversal (e.g., @ref is_at_first_child, @ref is_at_last_child, @ref is_at_root, @ref parent, etc.).
   *
   *  @note Comparison of iterators is also correct for null values. Only if two iterators refer to the same (identical) null
   *  value, they are equal, otherwise they are not. If they refer to different null values they are different. Hence,
   *  different null values in the AST are treated like different nodes. This is necessary to allow STL algorithms to work
   *  properly on the AST. */
  class iterator {
  public:

    /** Default constructor.
     *
     *  Constructs an iterator that compares equal to the @ref end iterator. */
    iterator();

    /** Construct an iterator pointing to a particular AST node. */
    iterator(SgNode* x,bool withNullValues, bool withTemplates);

    /** Test iterator equality.
     *
     *  Two iterators are equal if they point to the same node or they're both @ref end iterators. Two iterators that are
     *  pointing to null nodes are equal if and only if they refer to the same (identical) null value. If they refer to
     *  different null values they are different. Hence, different null values in the AST are treated like different
     *  nodes. This is necessary to allow STL algorithms to work.
     *
     *  Additionally, even if two iterators are pointing to the same AST node they will be equal only if they're in the same
     *  @ref skipChildrenOnForward state. */
    bool operator==(const iterator& x) const;

    /** Test iterator inequality.
     *
     *  This is just a convenince function that inverts the test for equality. */
    bool operator!=(const iterator& x) const;

    /** Dereference an iterator.
     *
     *  Dereference an iterator to obtain the AST node to which it points. Dereferencing an end iterator will throw an
     *  <code>std::out_of_range</code> exception.  If null AST pointers are being followed, the return value can be a null
     *  node (see @ref withNullValues and @ref withoutNullValues). */
    SgNode* operator*() const;

    /** Advance the iterator.
     *
     *  Advances the iterator to the next AST node in a depth-first pre-order search.  If the @ref skipChildrenOnForward
     *  flag is set then all children of the current node are skipped and the @ref skipChildrenOnForward flag is cleared.  If
     *  the @ref withoutNullValues flag is set, then all null child pointers are skipped but that flag is not cleared.
     *
     * @{ */
    iterator& operator++(); // prefix
    iterator operator++(int); // postfix
    /** @} */

    /** Cause children to be skipped on the next advancement.
     *
     *  This function marks the iterator so that it's next increment operator will advance over the current node's children
     *  without visiting them. The children are skipped only for the immediate next forward iteration (this is useful when used
     *  in the iterator idiom). This function is specific to tree iteration and allows to take tree structure into account
     *  although we are traversing the tree nodes as a sequence. */
    void skipChildrenOnForward();

    /** Mode to enable or disable skipping null child pointers.
     *
     *  The @ref withoutNullValues mode causes the iterator to skip over null child pointers, while the @ref withNullValues
     *  mode causse null child pointers to be followed followed.  The mode affects subsequent advancement of the iterator, not
     *  its current value.  In other words, in the @ref withNullValues mode the iterator dereference operator will always
     *  return non-null values after its next increment. The default mode is @ref withoutNullValues.
     *
     *  The function returns this iterator and can therefore be called together with @ref begin, like this:
     *
     *  @code
     *  RoseAst::iterator i = ast.begin().withNullValues();
     *  @endcode
     *
     * @{ */
    iterator& withoutNullValues();
    iterator& withNullValues();
    /** @} */

    iterator& withoutTemplates();
    iterator& withTemplates();
    
    /** Parent AST node relative to the iteration.
     *
     *  Returns the parent relative to the current iterator position, which might differ from the @c get_parent property of the
     *  current node. For instance, @ref parent can return a non-null pointer when the current node is null. */
    SgNode* parent() const;

    /** Test whether iterator is pointing to root node of AST to be traversed.
     *
     *  Returns true if and only if the current node of this iterator is also the root of the traversed subtree. */
    bool is_at_root() const;

    /**  Test whether iterator is at the first child of its parent.
     *
     *   Returns true if the current node of this iterator is the first child of the parent node. 
     */
    bool is_at_first_child() const;

    /** Test whether iterator as at the last child of its parent.
     *
     *  Returns true if the current node of this iterator is the last child of the parent node. 
     */
    bool is_at_last_child() const;

    // internal
    bool is_past_the_end() const;
    // internal
    std::string current_node_id() const;
    // internal
    std::string parent_node_id() const;
    // internal
    void print_top_element() const;

    /** Depth of traversal. */
    int stack_size() const;
    
  protected:
    SgNode* _startNode;
    bool _skipChildrenOnForward;
    bool _withNullValues;
    bool _withTemplates;

  private:
    static const int ROOT_NODE_INDEX=-2;
    friend class RoseAst;
    typedef struct {SgNode* node; int index;} stack_element;
    std::stack<stack_element> _stack;
    SgNode* access_node_by_parent_and_index(SgNode* p, int index) const;

    // not necessary with a children iterator
    int num_children(SgNode* p) const;
  };

  /** Iterator positioned at root of subtree.
   *
   *  The returned iterator holds a pointer to the root node of the subtree to be traversed. The root node is specified in the
   *  constructor. */
  iterator begin();

  /** Iterator positioned at the end of the traversal.
   *
   *  The returned iterator is positioned one past the last node in the depth-first pre-order traversal. */
  iterator end();

  // ast access function
  SgFunctionDefinition* findFunctionByName(std::string name);
  std::list<SgFunctionDeclaration*> findFunctionDeclarationsByName(std::string name);

/** determines whether a node is used to represent the root node of a template instantiation */
  static bool isTemplateInstantiationNode(SgNode* node);

/** determines whether a node is used to represent the root node of a template. */
  static bool isTemplateNode(SgNode* node);

/** determines based on VariantT whether a ROSE-AST node is a subtype of another node type.*/
static bool isSubTypeOf(VariantT DerivedClassVariant, VariantT BaseClassVariant);
/** deprecated, use isSubTypeOf instead */
static bool isSubType(VariantT DerivedClassVariant, VariantT BaseClassVariant);
 void setWithNullValues(bool flag);
 void setWithTemplates(bool flag);
 
 protected:
  static SgNode* parent(SgNode* astNode);
 private:
  SgNode* _startNode;
  bool _withNullValues;
  bool _withTemplates;
};

#endif
