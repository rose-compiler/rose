/* Author: Markus Schordan, 2004, 2012.
   ROSE AST Interface, providing:
   - AST iterator: equivalent to the AST SimpleProcessing traversal,
     but provides an STL-compliant iterator for traversing the AST nodes
     in preorder. In difference to the SimpleProcessing traversal for
     the iterator it can be selected whether null values should be traversed
     or not (by using the function withoutNullValues/withNullValues). By
     default null values are not traversed.
     The iterator also provides a number of auxiliary functions for querying
     some structural properties of the AST w.r.t. to the position in the
     traversal (e.g. is_at_first_child, is_at_last_child, is_at_root, 
     parent, etc.)

 */

#ifndef MYAST_H
#define MYAST_H

#include <stack>
#include "roseInternal.h"

//! This class allows to use iterators on the AST.

class MyAst {
 public:
  typedef SgNode elementType;
  typedef elementType* pointer;
  typedef elementType& reference;
  typedef size_t size_type;

  // no default constructor
  //! the argument is the root node of the AST subtree to iterate on
  MyAst(SgNode* astNode); 

  //! operator '*' allows to access AST nodes we iterate on (including null values)
  //SgNode* operator*();

  class iterator {
  public:
    iterator();
    iterator(SgNode* x);

    bool operator==(const iterator& x) const;
    bool operator!=(const iterator& x) const;
    SgNode* operator*() const;
    iterator& operator++(); // prefix
    iterator operator++(int); // postfix

    // MS 02/26/2012:
    //! Allows to skip children on a forward iteration 
    //! with operator++. The children are skipped only for the immediate next
    //! forward iteration (this is useful when used in the iterator idiom).
    //! This function is specific to tree iteration and allows to take tree
    //! structure into account although we are traversing the tree nodes as a sequence
    void skipChildrenOnForward();

    //! This function allows to filter null values, such that a dereferenced
    //! iterator never returns a null value.
    //! By default this mode is OFF. The function returns the iterator itself and can therefore
    //! be called together with begin (e.g. Ast::iterator i=myast.begin().withoutNullValues();)
    iterator& withoutNullValues();
    iterator& withNullValues();

    //! auxiliary functions utilizing the iterator's maintained context
    SgNode* parent() const;
    bool is_at_root() const;
    bool is_at_first_child() const;
    bool is_at_last_child() const;
    bool is_past_the_end() const;
    std::string current_node_id() const;
    std::string parent_node_id() const;

    //! info functions
    void print_top_element() const;
    int stack_size() const;

  protected:
    SgNode* _startNode;
    bool _skipChildrenOnForward;
    bool _withNullValues;

  private:
    static const int ROOT_NODE_INDEX=-2;
    friend class MyAst;
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

  // ast access functions
  SgFunctionDefinition* findFunctionByName(std::string name);

 protected:
  static SgNode* parent(SgNode* astNode);
 private:
  SgNode* _startNode;
};

#endif
