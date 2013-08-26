#ifndef CALL_GRAPH_TRAVERSE_H
#define CALL_GRAPH_TRAVERSE_H

#include "CallGraph.h"
#include <map>
#include <set>
#include <list>
#include <string>
#include <utility>

namespace fuse {

/* !!! NOTE: TraverseCallGraphDataflow LIMITED TO NON-RECURSIVE PROGRAMS (I.E. CONTROL FLOW GRAPHS WITH NO CYCLES) !!! */

class Function
{
  protected:
  SgFunctionDeclaration* decl;
  
  public:
  Function();

  Function(std::string name);

  Function(SgFunctionDeclaration* sample);

  Function(SgFunctionDefinition* sample);

  Function(SgFunctionCallExp* funcCall);
  
  public:
  void init(SgFunctionDeclaration* sample);

  public:
  Function(const Function &that);

  Function(const Function *that);

  // Returns whether this Function object has been initialized
  bool isInitialized() const;
  
  // returns a unique SgFunctionDeclaration* that is the canonical AST node that represents the given function
  static SgFunctionDeclaration* getCanonicalDecl(SgFunctionDeclaration* decl);

  bool eq(const Function &that) const;

  bool operator == (const Function &that) const;
  bool operator != (const Function &that) const;

  protected:
  bool lessThan(const Function &that) const;
  public:
  bool operator < (const Function &that) const;
  bool operator > (const Function &that) const;
  bool operator <= (const Function &that) const;
  bool operator >= (const Function &that) const;

  SgName get_name() const;

  // returns this function's definition or NULL of it does not have one
  SgFunctionDefinition* get_definition() const;

  // returns one of this function's declarations. it is guaranteed to be the same each time get_declaration 
  // is called and to be the canonical declaration
  SgFunctionDeclaration* get_declaration() const;

  // returns the file_info of the definition or one of the declarations if there is no definition
  Sg_File_Info* get_file_info() const;

  // Returns the parameters of this function if it is known and NULL if it is not
  SgInitializedNamePtrList* get_args() const;
  
  // Returns the parameters of this function if it is known and NULL if it is not
  SgFunctionParameterList* get_params() const;
  
  // Returns the Function object that refers to the function that contains the given SgNode
  static Function getEnclosingFunction(SgNode* n, bool includingSelf=true);

  std::string str(std::string indent="") const;
};

// extension of the generic function class that also contains all the SgGraphNodes that refer to the function
class CGFunction : public Function
{
  std::set<SgGraphNode*> cgNodes;
  SgIncidenceDirectedGraph* graph;
  
  public:
  CGFunction(std::string name, SgIncidenceDirectedGraph* graph);
  
  CGFunction(SgFunctionDeclaration* sample, SgIncidenceDirectedGraph* graph);
  
  CGFunction(SgGraphNode* sample, SgIncidenceDirectedGraph* graph);
  
  CGFunction(const CGFunction &that);
  
  CGFunction(const CGFunction *that);
  
  protected:
  // initializes the cgNodes set
  void initCGNodes();
  
  public: 
  bool operator == (const CGFunction &that) const;
  bool operator != (const CGFunction &that) const;
  
  bool operator < (const CGFunction &that) const;
  bool operator > (const CGFunction &that) const;
  bool operator <= (const CGFunction &that) const;
  bool operator >= (const CGFunction &that) const;
  
  // Traverses the callers or callees of this function
  class iterator
  {
    // direction in which the iterator is going
    //SgIncidenceDirectedGraph::EdgeDirection iterDir;
    // side of an edge that we're going to be following
    //SgIncidenceDirectedGraph::EdgeDirection edgeDir;
    
    // Direction in which the iterator is going (fw: from callers to callees, bw: from callees to callers)
    public:
    //typedef enum direction {fw=0, bw=1};
    enum direction {fw=0, bw=1};
    protected:
    direction dir;
    
    std::set<SgGraphNode*>::iterator itn;
    std::set<SgDirectedGraphEdge*> edges; // The current SgGraphNode's (*itn) incoming or outgoing edges
    std::set<SgDirectedGraphEdge*>::iterator ite; // The current edge in edges
      
    // The set the SgGraphNodes that have already been visited by this iterator
    // used to eliminate duplicates
    std::set<SgGraphNode*> visitedCGNodes;
      
    const CGFunction* func;
    
    // =true if this iterator has reached the end of all the edges and =false if not
    bool finished;
      
    public:
    iterator();
      
    iterator(const CGFunction* const func, direction dir);
    
    // Returns a reference to CGFunction that matches the current SgGraphNode that this iterator refers to,
    // pulling the reference from the given set of CGFunctions
    const CGFunction* getTarget(std::set<CGFunction> &functions);
    
    // Returns the function that this iterator is currently referring to
    Function getTarget();
    
    void operator ++( int );
    
    // If the current <itn, ite> pair refers to a specific CallGraph node, does nothing.
    // otherwise, advances the <itn, ite> pair until it does refer to a specific CallGraph node
    void advanceToNextValidPoint();
    
    bool operator == (const iterator& that);
    
    bool operator != (const iterator& that);
  };
  
  // Returns an iterator that traverses the callees of this function
  iterator callees() const;
  iterator successors() const;
  
  // Returns an iterator that traverses all the callers of this function
  iterator callers() const;
  iterator predecessors() const;
  
  iterator end() const;
};

class TraverseCallGraph
{
  protected:
  SgIncidenceDirectedGraph* graph;
  // maps each SgFunctionDefinition to its associated SgGraphNodes
  //   (there may be more than one such node for a given SgFunctionDefinition)
  //map<SgFunctionDefinition*, std::set<SgGraphNode*> > decl2CFNode;
  
  // The set of functions in this program
  std::set<CGFunction> functions;
  
  // The number of functions that call each given function
  //std::map<SgFunctionDefinition*, int> numCallers;
  std::map<const CGFunction*, int> numCallers;
  
  // set of all the SgFunctionDefinition for all functions that are not called from other functions
  //set<SgFunctionDefinition*> noPred;
  // Set of functions that are not called from other functions.
  // Just contains pointers to the CGFunction objects inside the functions set.
  std::set<const CGFunction*> noPred;
  
  public:
  TraverseCallGraph(SgIncidenceDirectedGraph* graph);
  
  // Returns a pointer to a CGFunction that matches the given declaration.
  // The memory of the object persists for the entire lifetime of this TraverseCallGraph object.
  const CGFunction* getFunc(SgFunctionDeclaration* decl);
  
  // Returns a pointer to a CGFunction object that matches the given Function object.
  // The memory of the object persists for the entire lifetime of this TraverseCallGraph object.
  const CGFunction* getFunc(const Function& func);
};

/* Un-ordered traversal of the call graph */
class TraverseCallGraphUnordered : public TraverseCallGraph
{
  public:
  TraverseCallGraphUnordered(SgIncidenceDirectedGraph* graph);
  
  void traverse();
    
  virtual void visit(const CGFunction* func)=0;
  
  virtual ~TraverseCallGraphUnordered();
};

template <class InheritedAttribute>
class TraverseCallGraphTopDown : public TraverseCallGraph
{
  class funcRecord
  {
    public:
    // the inherited attributes passed down from callers
    std::list<InheritedAttribute> fromCallers;
  };
  
  public:
  TraverseCallGraphTopDown(SgIncidenceDirectedGraph* graph);
  
  void traverse();
  
  virtual InheritedAttribute visit(const CGFunction* func, std::list<InheritedAttribute>& fromCallers)=0;
  
  virtual InheritedAttribute defaultAttrVal() { InheritedAttribute val; return val; }
  
  protected:
  void traverse_rec(const CGFunction* fd, 
        std::map<const CGFunction*, funcRecord> &visitRecords, 
        std::set<std::pair<const CGFunction*, const CGFunction*> > &touchedEdges,
        InheritedAttribute &fromCaller);

  public:
  virtual ~TraverseCallGraphTopDown();
};


template <class SynthesizedAttribute>
class TraverseCallGraphBottomUp : public TraverseCallGraph
{
  public:
  TraverseCallGraphBottomUp(SgIncidenceDirectedGraph* graph);
  
  void traverse();
  
  virtual SynthesizedAttribute visit(const CGFunction* func, std::list<SynthesizedAttribute> fromCallees)=0;
  
  virtual SynthesizedAttribute defaultAttrVal() { SynthesizedAttribute val; return val; }

  protected:
  SynthesizedAttribute traverse_rec(const CGFunction* fd, 
      std::map<const CGFunction*, SynthesizedAttribute> &visitRecords, 
      std::set<std::pair<const CGFunction*, const CGFunction*> > &touchedEdges);
  
  public:
  virtual ~TraverseCallGraphBottomUp();
};

// CallGraph traversal useful for inter-procedural dataflow analyses because it starts
// at the functions that are not called by any other function and allows such
// analyses to keep adding more nodes depending on how function dataflow information changes
class TraverseCallGraphDataflow : public TraverseCallGraph
{
  public:
  // list of functions that still remain to be processed;
  std::list<const CGFunction*> remaining;
    
  TraverseCallGraphDataflow(SgIncidenceDirectedGraph* graph);
  
  void traverse();
  
  virtual void visit(const CGFunction* func)=0;
  
  // adds func to the back of the remaining list, if its not already there
  void addToRemaining(const CGFunction* func);
  
  virtual ~TraverseCallGraphDataflow();
};

/*********************************************************
 ***         numCallersAnnotator     ***
 *** Annotates every function's SgFunctionDefinition   ***
 *** node with a numCallersAttribute that contains the ***
 *** number of functions that call the given function. ***
 *********************************************************/
 
// The attribute that is associated with each function's declaration to
// record its number of callers.
class numCallersAttribute : public AstAttribute
{
  int numCallers;
  
  public: 
  numCallersAttribute()
  {
    numCallers = 0;
  }
  
  numCallersAttribute(int numCallers)
  {
    this->numCallers = numCallers;
  }
  
  numCallersAttribute(numCallersAttribute& that)
  {
    this->numCallers = that.numCallers;
  }
  
  int getNumCallers()
  {
    return numCallers;
  }
};

// Uses the numCallersAnnotator class to annotate every function's SgFunctionDefinition node 
// with a numCallersAttribute that contains the number of functions that call the given function.
void annotateNumCallers(SgIncidenceDirectedGraph* graph);

// Returns the number of functions that call this function or 0 if the function is compiler-generated.
int getNumCallers(const Function* fDecl);

} // namespace fuse {

#endif
