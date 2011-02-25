
#ifndef MULTI_GRAPH_CREATE
#define MULTI_GRAPH_CREATE

#include <stdlib.h>
#include <assert.h>
/*
An element (could be a node or an edge) which can belong to more than one graphs.
*/
class MultiGraphCreate;
class MultiGraphElem 
{
  MultiGraphCreate *gc; // The owner graph of this element
  int count;// Reference count of this element, >0 means shared by more than one graphs
 public:
  MultiGraphElem( MultiGraphCreate *c) : gc(c) { count = 0; }
  virtual ~MultiGraphElem() { assert( count == 0); }

  bool UnlinkGraphCreate( const MultiGraphCreate *c)
       { // Decrement the reference count if not the original graph
         if (gc != c) 
            count--;
         else // (gc==c) Unlink the original 'owner' graph
            gc = 0;
         // Delete this elment if it is not used in any graph   
         if (gc == 0 && count == 0) {
             delete this;
            return true;
         }
         return false;
       }
  int LinkGraphCreate(const MultiGraphCreate* c)
    { 
      if (gc != c) ++count;
      return count; 
    }
  MultiGraphCreate* GetGraphCreate() const { return gc; }
  virtual std::string toString() const { return ""; }
 friend class MultiGraphCreate;
};

/* Class Hierarchy for MultiGraphCreate. 

MultiGraphCreate
* BaseGraphCreate
** DAGBaseGraphImpl
** DGBaseGraphImpl

* VirtualGraphCreateTemplate
** CallGraphCreate

** CFGImplTEmplate
*** DataFlowAnalysis
**** ReachingDefinitionAnalysis
*** DefaultCFGImpl

** CompSliceRegistry

** DAGCreate

** DefUseChain
*** DefaultDUChain
*** ValuePropagate

** DepInfoGraphCreate
*** CompSliceDepGraphCreate
*** DepCompAstRefGraphCreate
**** DepCompAstRefDAG
*** LoopTreeDepGraphCreate

** DepInfoSetGraphCreate

** GroupGraphCreate
*** SCCGraphCreate
**** TransAnalSCCCreate

** TransDepGraphCreate

*/
class MultiGraphCreate 
{
 protected:
  virtual ~MultiGraphCreate() {}
  bool UnlinkElem( MultiGraphElem *n) { return n->UnlinkGraphCreate(this); }
  bool LinkElem( MultiGraphElem *n) { return n->LinkGraphCreate(this); }
  bool UnlinkElem( void* n) {  return false; }
  bool LinkElem(void* n) { return false; }
 public:
  // Check if this MultiGraphCreate owns the element
  bool ContainElem( const MultiGraphElem* e) const
          { return e->GetGraphCreate() == this; }
};

//A template class implementing mulitiGraphElem and storing arbitrary information.
template <class Info>
class MultiGraphElemTemplate : public MultiGraphElem
{
  Info info;
 protected:
  virtual ~MultiGraphElemTemplate() {}
 public:
  MultiGraphElemTemplate(MultiGraphCreate *c, const Info&  _info)
    : MultiGraphElem(c), info(_info) {}
  Info&  GetInfo() { return info; }
  Info GetInfo() const { return info; }
  // Liao, 2/22/2008, some instantiated objects do not have this member function
 // std::string toString() const { return info.toString(); }
};

#endif
