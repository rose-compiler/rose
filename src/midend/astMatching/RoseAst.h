
#ifndef ROSE_AST_H
#define ROSE_AST_H

/*************************************************************
 * Copyright: (C) 2012 Markus Schordan                       *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include <stack>
#include "roseInternal.h"

/*! 

  \author Markus Schordan
  \date 2012
  \brief ROSE AST Interface providing an AST iterator and some AST specific functions.

 */
class RoseAst {
 public:
  typedef SgNode elementType;
  typedef elementType* pointer;
  typedef elementType& reference;
  typedef size_t size_type;

  // no default constructor

  //! Defines the root node of an AST subtree. All operations are specific to this subtree.
  RoseAst(SgNode* astNode); 

  //! operator '*' allows to access AST nodes we iterate on (including null values)
  //SgNode* operator*();

  /*! \brief AST iterator: STL-compliant forward-iterator for traversing AST nodes
     in preorder. 
     \author Markus Schordan
     \date 2012
     \details For the iterator it can be selected whether null values should be traversed
     or not (by using the function withoutNullValues/withNullValues). By
     default null values are traversed.
     The iterator also provides a number of auxiliary functions for querying
     some structural properties of the AST w.r.t. to the position in the
     traversal (e.g. is_at_first_child, is_at_last_child, is_at_root, 
     parent, etc.)
     Subtrees can be exluded from traversal with the function skipSubtreeOnForward.
     \note Comparison of iterators is also correct for null values. Only if two iterators refer to the same (identical) null value, they are equal, otherwise they are not. If they refer to different null values they are different. Hence, different null values in the AST are treated like different nodes. This is necessary to allow STL algorithms to work properly on the AST.

  */
  class iterator {
  public:
    iterator();
    iterator(SgNode* x);

    bool operator==(const iterator& x) const;
    bool operator!=(const iterator& x) const;
    SgNode* operator*() const;
    iterator& operator++(); // prefix
    iterator operator++(int); // postfix

    /*! 
      \brief Allows to skip children on a forward iteration 
      with operator++. 
      
      \details The children are skipped only for the immediate next
      forward iteration (this is useful when used in the iterator idiom).
      This function is specific to tree iteration and allows to take tree
      structure into account although we are traversing the tree nodes as a sequence
      
      \date 2012
      \author Markus Schordan
    */
    void skipChildrenOnForward();

    /*! \brief This function allows to filter null values, such that a dereferenced
     iterator never returns a null value.

     \details By default this mode is OFF. The function returns the iterator itself and can therefore
      be called together with begin (e.g. Ast::iterator i=myast.begin().withoutNullValues();)
      \author Markus Schordan
      \date 2012
    */
    iterator& withoutNullValues();
    iterator& withNullValues();

    //! returns the parent AST node relative to the *iteration*.
    SgNode* parent() const;
    //! true if iterator is pointing to root node of AST to be traversed.
    bool is_at_root() const;
    /*! \brief true if iterator is at first child of parent node.
      \details We need this function for example if we want to print the iterated AST as term (with braces). This allows to query whether we are at the first child node and need to print a "(".
    */
    bool is_at_first_child() const;
    /*! \brief true if iterator is at last child of parent node.
      \details We need this function for example if we want to print the iterated AST as term (with braces). This allows to query whether we are at the last child node (and need to print a ")" )
    */
    bool is_at_last_child() const;

    //! \internal
    bool is_past_the_end() const;
    //! \internal
    std::string current_node_id() const;
    //! \internal
    std::string parent_node_id() const;
    //! \internal
    void print_top_element() const;

    //! info function
    int stack_size() const;

  protected:
    SgNode* _startNode;
    bool _skipChildrenOnForward;
    bool _withNullValues;

  private:
    static const int ROOT_NODE_INDEX=-2;
    friend class RoseAst;
    typedef struct {SgNode* node; int index;} stack_element;
    std::stack<stack_element> _stack;
    SgNode* access_node_by_parent_and_index(SgNode* p, int index) const;

    // not necessary with a children iterator
    int num_children(SgNode* p) const;
  };


  //! the returned iterator holds a pointer to the root node of the tree to be traversed
  iterator begin();
  //! use this method to check for the end of the iteration
  iterator end();

  // ast access function
  SgFunctionDefinition* findFunctionByName(std::string name);

  // determines based on VariantT whether a ROSE-AST node is a subtype of another node type.
  static bool isSubType(VariantT DerivedClassVariant, VariantT BaseClassVariant);

 protected:
  static SgNode* parent(SgNode* astNode);
 private:
  SgNode* _startNode;
};

#endif
