/*************************************************************
 * Copyright: (C) 2012 Markus Schordan                       *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "sage3basic.h"
#include "RoseAst.h"

/* AST functions */

RoseAst::RoseAst(SgNode* astNode):_startNode(astNode) {
  /* only initializer list required */
}

SgNode* 
RoseAst::parent(SgNode* astNode) {
  return astNode->get_parent();
}

SgFunctionDefinition*
RoseAst::findFunctionByName(std::string name) {
  for(RoseAst::iterator i=begin();i!=end();++i) {
    if(SgFunctionDefinition* fundef=isSgFunctionDefinition(*i)) {
      if(SgFunctionDeclaration* fundecl=fundef->get_declaration()) {
    SgName fname=fundecl->get_name();
    if(fname.getString()==name)
      return fundef;
      }
    }
  }
  return 0; // no function found with this name
}

std::list<SgFunctionDeclaration*> RoseAst::findFunctionDeclarationsByName(std::string name) {
  std::list<SgFunctionDeclaration*> fdecl_list;

  for(RoseAst::iterator i=begin();i!=end();++i) {
    if(SgFunctionDeclaration* fundecl = isSgFunctionDeclaration(*i)) {
      SgName fname=fundecl->get_name();
      if(fname.getString() == name)
        fdecl_list.push_back(fundecl);
    }
  }

  return fdecl_list;
}

SgNode* RoseAst::iterator::parent() const {
  return _stack.top().node;
}

/* iterator functions */

RoseAst::iterator::iterator()
  :
  _startNode(0), // 0 is not traversed, due to the empty stack the default iterator is a past-the-end iterator
  _skipChildrenOnForward(false),
  _withNullValues(false) // default: we do not traverse null values
{
}

RoseAst::iterator::iterator(SgNode* x)
  : 
  _startNode(x), 
  _skipChildrenOnForward(false),
  _withNullValues(false)
{
  stack_element e;
  e.node=x;
  e.index=ROOT_NODE_INDEX; // only root node has this index
  _stack.push(e);
}

int RoseAst::iterator::stack_size() const { return _stack.size(); }
bool RoseAst::iterator::is_past_the_end() const { return _stack.size()==0; }

bool RoseAst::iterator::operator==(const iterator& x) const { 
  if(is_past_the_end() != x.is_past_the_end())
    return false;
  if(is_past_the_end() && x.is_past_the_end()) 
    return true;
  // this check ensures that 0 values work for trees. For DAGs we would need to compare the entire stack (i.e. context). Hence, this comparison
  // is guaranteed to work for trees, but not for DAGs. For DAGs the comparison might be true although the traversal is at different positions at
  // shared nodes. This is not problematic for comparisons with the past-the-end comparison, but may be problematic when different DAGs are compared.
  if(_stack.top().node!=x._stack.top().node ||_stack.top().index!=x._stack.top().index)
    return false;
  // mode must be the same otherwise iterators are different as
  // ++ may go to different elements. Therefore i==j -> *i==*j is not violated
  // because in case of different modes we define == such that i==j does not hold.
  if(_skipChildrenOnForward!=x._skipChildrenOnForward)
    return false;
  return true;
}

bool RoseAst::iterator::operator!=(const iterator& x) const { 
  return !(*this==x);
}

SgNode* RoseAst::iterator::operator*() const { 
  if(_stack.size()==0)
    throw std::out_of_range("Ast::iterator: past-the-end access");
  // access node by index
  stack_element e=_stack.top();
  if(e.index==ROOT_NODE_INDEX) {
    return e.node;
  } else {
    return access_node_by_parent_and_index(e.node,e.index);
  }
}

int 
RoseAst::iterator::num_children(SgNode* p) const {
  if(p)
    return p->get_numberOfTraversalSuccessors();
  else
    return 0;
}

RoseAst::iterator RoseAst::iterator::operator++(int) {
  iterator tmp = *this;
  ++*this;
  return tmp;
}

RoseAst::iterator
RoseAst::begin() { 
  return iterator(_startNode);
}

RoseAst::iterator
RoseAst::end() { 
  return iterator();
}

std::string
RoseAst::iterator::current_node_id() const {
  stack_element e=_stack.top();
  std::stringstream ss;
  if(operator*()==0) {
    ss << e.node << ":"; // to make ids of null values unique we need to additionally provide the parent context
    ss << e.index << ":";
    ss << "null";
  } else {
    ss << operator*();
  }
  return ss.str();
}

std::string
RoseAst::iterator::parent_node_id() const {
  std::stringstream ss;
  ss << parent(); // MS: a parent cannot be null, therefore the address is sufficient.
  return ss.str();
}

void
RoseAst::iterator::print_top_element() const {
  if(_stack.size()==0) {
    std::cout << "STACK-TOP-ELEMENT: none (empty)" << std::endl;
  } else {
    stack_element e=_stack.top();
    std::cout << "STACK-TOP-ELEMENT: " << "(" << e.node << "," << e.index << ")" << std::endl;
  }
}

RoseAst::iterator&
RoseAst::iterator::operator++() {
  // check if we are already past the end
  if(is_past_the_end())
    return *this;
  assert(_stack.size()>0);
  stack_element e=_stack.top();
  _stack.pop();
  
  SgNode* new_node=0;
  if(e.index==ROOT_NODE_INDEX) {
    if(num_children(e.node)==0)
      return *this;
    else {
      new_node=e.node;
      goto root_node_has_children;
    }
  };
  // check if we are at a null node (nothing to push and nothing to skip)
  if(e.node==0) {
    return *this;
  }
  new_node=access_node_by_parent_and_index(e.node,e.index);
 root_node_has_children:
  if(new_node==0) {
    return *this;
  }
  if(!_skipChildrenOnForward) {
    int numChildren=num_children(new_node);
    // we need to push children in reverse order on the stack (if they are != null)
    for(int new_index=numChildren-1;new_index>=0;new_index--) {
      stack_element new_e;
      new_e.node=new_node;
      new_e.index=new_index;
      if(new_e.node!=0) {
    if(_withNullValues) {
      // if we visit null-nodes we can push anything
      _stack.push(new_e);
    } else {
      if(access_node_by_parent_and_index(new_e.node,new_e.index)!=0) {
        _stack.push(new_e);
      } else {
        // we are not visiting null nodes therefore we do not push null nodes on the stack
      }
    }
      } else {
    // a null node has no children: nothing to do.
      }
    }
  } else {
    /* we have skipped children (because we do not put them on the stack)
       since we do this only once, we set the flag back to false
    */
    _skipChildrenOnForward=false;
  }
  return *this;
}

SgNode*
RoseAst::iterator::access_node_by_parent_and_index(SgNode* p, int index) const {
  int numChildren=num_children(p);
  if(index>=numChildren) {
    std::stringstream ss;
    ss << "Ast::iterator internal error: memorized index out of bounds (violation: ";
    ss << index << "<" <<numChildren;
    ss << ")";
    throw std::out_of_range(ss.str());
  }
  SgNode* node=p->get_traversalSuccessorByIndex(index);
  return node;
}

void RoseAst::iterator::skipChildrenOnForward() {
  _skipChildrenOnForward=true;
} 

RoseAst::iterator& 
RoseAst::iterator::withoutNullValues() {
  if(_stack.size()!=1 && (_stack.top().node!=_startNode))
    throw "Ast::iterator: unallowed mode change.";
  _withNullValues=false; 
  return *this;
} 

RoseAst::iterator& 
RoseAst::iterator::withNullValues() {
  if(_stack.size()!=1 && (_stack.top().node!=_startNode))
    throw "Ast::iterator: unallowed mode change.";
  _withNullValues=true; 
  return *this;
} 

bool
RoseAst::iterator::is_at_root() const {
  return !is_past_the_end() && _stack.size()>0 && _stack.top().node==_startNode;
}

bool RoseAst::iterator::is_at_first_child() const {
  return _stack.top().index==0;
}

bool RoseAst::iterator::is_at_last_child() const {
  stack_element e=_stack.top();
  return e.index==num_children(e.node)-1 && e.index!=ROOT_NODE_INDEX;
}

bool RoseAst::isSubType(VariantT DerivedClassVariant, VariantT BaseClassVariant) { 
  /* access the byte containing the information and mask and extract the bit that holds the value */
  return rose_ClassHierarchyCastTable[DerivedClassVariant][BaseClassVariant >> 3] & (1 << (BaseClassVariant & 7));
}
