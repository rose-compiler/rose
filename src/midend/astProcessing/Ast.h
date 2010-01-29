// Author: Markus Schordan, Vienna University of Technology, 2004.
// $Id: Ast.h,v 1.4 2008/01/08 02:56:38 dquinlan Exp $

// WORK IN PROGRESS : only use the public interface (!)
// - currently only the forward iterator is supported
// - copy-by-value will be replaced by references and using const
// - null_iterator may also be added (including encountered null pointers)

#ifndef AST_H
#define AST_H

#include <vector>
#include "roseInternal.h"
//#include "sage3.h"

//! This class allows to use iterators on the AST; it is different to
//! the AstProcessing classes because no visit function needs to be
//! implemented.  

class Ast {
 public:
  typedef SgNode elementType;
  typedef elementType* pointer;
  typedef elementType& reference;
  typedef size_t size_type;
  // no default constructor

  //! the argument is the root node of tree to iterate on
  Ast(SgNode* astNode); 
  SgNode* operator*();

  class iterator {
    private:
    friend class Ast;
    SgNode* findNextNode(SgNode* p);

    protected:
 // DQ (10/24/2004): Swapped order of declaration to avoid compiler warning
    SgNode* node;
    SgNode* startNode;
    iterator(SgNode* x) : node(x),startNode(x) {}
    
    public:
    iterator() {}
    bool operator==(const iterator& x) const { return node == x.node; }
    bool operator!=(const iterator& x) const { return node != x.node; }
    reference operator*() const { return (*node); }
    iterator& operator++();
    iterator operator++(int) {
      iterator tmp = *this;
      ++*this;
      return tmp;
    }
  };

  //! the returned iterator holds a pointer to the root node of the tree to be traversed
  iterator begin() { return iterator(startNode); }
  //! use this method to check for the end of the iteration (do not use '0')
  iterator end() { return iterator(0); }

 protected:
  typedef std::vector<SgNode*> NodeList;
  static NodeList successors(SgNode* astNode);
  static unsigned int numSuccessors(SgNode* astNode);
  static NodeList rightSiblings(SgNode* astNode);
  static unsigned int numRightSiblings(SgNode* astNode);
  static SgNode* nextRightSibling(SgNode* astNode);
  static SgNode* parent(SgNode* astNode);
 private:
  static SgNode* first(NodeList l) { return *(l.begin()); }
  SgNode* startNode;
};

#endif
