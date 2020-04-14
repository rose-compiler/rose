#ifndef INTRA_PROC_ALIAS_ANALYSIS_H
#define INTRA_PROC_ALIAS_ANALYSIS_H
#include "IntraProcDataFlowAnalysis.h"
#include "customFilteredCFG.h"
#include "ClassHierarchyGraph.h"
#include "CallGraph.h"
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <algorithm>

#define foreach BOOST_FOREACH
#define reverse_foreach BOOST_REVERSE_FOREACH


using namespace boost;
using namespace std;

//! A struct to hold the information about an alias node
struct AliasRelationNode{
    //! The VariableSymbol participating in aliasing
    SgVariableSymbol *var;
    //! Dereference Level 
    //!  *a  +1
    //! **a  +2
    //!   a   0
    //!  &a  -1
    int derefLevel;

    bool operator==(const AliasRelationNode &that) const {
        assert(this != NULL);
        return (this->var == that.var && this->derefLevel == that.derefLevel);
    }
    
};


struct AliasCfgFilter
{
        /** Determines if the provided CFG node should be traversed during Alias Analysis
         *
         * @param cfgn The node in question.
         * @return Whether it should be traversed.
         */
        bool operator() (CFGNode cfgn) const
        {
                SgNode *node = cfgn.getNode();
                switch (node->variantT())
                {
                    case V_SgBasicBlock:
                        return cfgn == node->cfgForBeginning()  || cfgn == node->cfgForBeginning();
                    case V_SgAssignStatement:
                    case V_SgAssignOp:
                    case V_SgAssignInitializer:
                        //return (cfgn == node->cfgForBeginning());
                        return (cfgn == node->cfgForEnd());
                    case V_SgConditionalExp:
                                return (cfgn.getIndex() == 1);
                    case V_SgConstructorInitializer:
                    case V_SgFunctionCallExp:
                    case V_SgReturnStmt:
                                return (cfgn == node->cfgForBeginning());
                                //return (cfgn == node->cfgForEnd());
                     default:
                        return false;
                        
                }
        }
};
// Base class for CompactRepresentation
class CompReprBase{


public:
    CompReprBase() {
    }
    //! Given a VariableSymbol and Dereference level compute the Aliases
    virtual void computeAliases (SgVariableSymbol *var, int derefLevel, vector<SgGraphNode *> &) = 0;
    
    //! Given a GraphNode and Dereference level compute the Aliases
    virtual void computeAliases (SgGraphNode *node, int derefLevel, vector<SgGraphNode *> &) = 0;
    
    //! Add a must alias relation
    virtual void addMustAliasRelation(const AliasRelationNode &left, const AliasRelationNode &right) = 0;
    
    //! Add a may alias relation eg. conditionals
    virtual void addMayAliasRelation(const AliasRelationNode &left, const AliasRelationNode &right) = 0;
    
    //! compute hash of the graph
    virtual unsigned long getHash() const = 0;
    //! Get the Graph
    virtual SgIncidenceDirectedGraph * getGraph() const = 0;
    
    //! Merge two CompRepr
    virtual void merge(const CompReprBase &) = 0;
    
    //! Print to Dot file
    virtual void toDot(const std::string& file_name) = 0;

protected:
    SgIncidenceDirectedGraph *graph;
    unsigned long hash;
    /**
     * Protected Destructor to make sure the client code can't delete it 
     */
 // DQ (12/6/2016): Aded virtual to fix compiler warning that we want to considered an error: -Wdelete-non-virtual-dtor
    virtual ~CompReprBase(){
        delete graph;
        graph = NULL;
    }
};

// The actual Compact Representation. It's implemented
// as a graph(SgIncidenceDirectedGraph).
class CompactRepresentation : public CompReprBase{
    
    //! A Map to hold the SgNode to SgGraphNode mapping
    boost::unordered_map<SgNode *,SgGraphNode *> all_nodes;
    
    //! Get Graph Node from SgNode
    SgGraphNode * getGraphNode(SgNode *node);
    
    //! Merge a graph with this compact representation
    void merge(SgIncidenceDirectedGraph *thatGraph);
    
    //! Update the Hash
    void updateHash(SgNode *from, SgNode *to);
    
    //! Initialize CompactRepresentation
    void init();
    
    //! add a Graph Edge
    void addEdge(SgGraphNode *g_from, SgGraphNode *g_to);
    
    //! Process Nodes
    void processNodes(std::ostream & o, SgGraphNode* n, std::set<SgGraphNode*>& explored);
    
    //! Print Nodes and Edges
    void printNodePlusEdges(std::ostream & o, SgGraphNode* node);
    
    //! Print Nodes
    void printNode(std::ostream & o, SgGraphNode* node);
    
    //! Print Edges
    void printEdge(std::ostream & o, SgDirectedGraphEdge* edge, bool isInEdge);
    
    
public:
    //! Get the Mapping from SgNode to SgGraphNode
    boost::unordered_map<SgNode *,SgGraphNode *>  getNodesMapping(){ return all_nodes;}
    CompactRepresentation() {  init(); }
    
    //! Get the Graph
    SgIncidenceDirectedGraph * getGraph() const{ return graph; }
    
    //! Get the Hash
    unsigned long getHash() const {  return hash; }    
    
    //! Copy constructor for CompactRepresentation
    CompactRepresentation(const CompactRepresentation& copy);
    
    //! Assignment operator overload for Compactrepresentation
    CompactRepresentation& operator=(const CompactRepresentation& p);
    
    //! Given a VariableSymbol and Dereference level return list of aliases
    void computeAliases (SgVariableSymbol *var, int derefLevel, vector<SgGraphNode *> &nodes);
    
    //! Given a graph node and Dereference level return list of aliases
    void computeAliases (SgGraphNode *node, int derefLevel, vector<SgGraphNode *> &);
    
    //! Add a Must Relation to CompactRepresntation
    void addMustAliasRelation(const AliasRelationNode &left, const AliasRelationNode &right);
    
    //! Add a May Relation to CompactRepresntation
    void addMayAliasRelation(const AliasRelationNode &left, const AliasRelationNode &right);
    
    //! Merge two CompactRepresentations
    void merge(const CompReprBase &that);
    
    //! == Operator overload for CompactRepresentation. Implemented using hashing for efficiency
    bool operator==(const CompactRepresentation &that) const;
    
    //! != Operator overload for CompactRepresentation. Implemented using hashing for efficiency
    bool operator !=(const CompactRepresentation &that) const;
    
    //! Prints CompactRepresentation to Dot File.
    void toDot(const std::string& file_name);
};

//! A Shared Pointer Wrapper for CompactRepresentation
class CompReprPtr  {
    //! The shared pointer holding th CompactRepresentation
     boost::shared_ptr<CompReprBase> ptr;
public:
    //! Constructor
    CompReprPtr() { ptr = boost::shared_ptr<CompReprBase>(); }
    
    //! Constructor
    CompReprPtr(CompactRepresentation *repr)   { ptr = boost::shared_ptr<CompReprBase>(repr);  }
    
    //! get the actual pointer to compact representation
    CompReprBase * get() const{ return ptr.get();  }
    
    // Equality operator overload. Compares two CompactRepresentations
    bool operator==(const CompReprPtr &that) const;
    
    // Not Equality operator overload. Compares two CompactRepresentations
    bool operator!=(const CompReprPtr &that) const {  return !(*this == that);   }
    
    // += operator overload
    void operator+=(const CompReprPtr &that) const;
};

//! Helper class to hold Alias Information
class AliasInfoGenerator {
    //!  IN of every CFG Node
    boost::unordered_map<SgGraphNode *, CompReprPtr> ins;
    
    //!  OUT of every CFG Node
    boost::unordered_map<SgGraphNode *, CompReprPtr> outs;
    
    //! All the Return Stmts
    std::vector <AliasRelationNode > returnStmts;
    
    //! All the AliasRelations for every CFG Node 
    boost::unordered_map<SgGraphNode *, std::vector <std::pair<AliasRelationNode, AliasRelationNode> > >aliasRelations;    
    
public :
    AliasInfoGenerator();
    
    //! Initialize 
    void init(SgGraphNode *n);
    
    //! Get Entry Data For a CFG Node
    CompReprPtr getEntryData(SgGraphNode *node);
    
    //! Set Entry Data For a CFG Node
    void setEntryData(SgGraphNode *node, CompReprPtr en) {    ins[node] = en;    }
    
    //! Get Exit Data For a CFG Node
    CompReprPtr getExitData(SgGraphNode *node);
    
    //! Set Exit Data For a CFG Node
    void setExitData(SgGraphNode *node, CompReprPtr en) {  outs[node] = en;   }
    
    //! Get alias relations for a CFG Node
    std::vector <std::pair<AliasRelationNode, AliasRelationNode> > getAliasRelations(SgGraphNode *node);
    
    //! Adds a new alias relation for a CFG Node
    void addNewAliasRelation(SgGraphNode *node, std::pair<AliasRelationNode, AliasRelationNode> a_relation) ;
    
    //! Adds a return statement. Return Statements are handled separately from others because of backward propagation 
    void addReturnStmt(AliasRelationNode node) { returnStmts.push_back(node);  }
    
    //! Get all the return statements
    std::vector<AliasRelationNode> getReturnStmts(){ return returnStmts; } 
    
};

namespace ProcessExpression {
    //! Helper Function to process Left Hand Side of an Expression
    void processLHS(SgNode *node, struct AliasRelationNode &arNode) ;
    
    //! Helper Function to process Right Hand Side of an Expression
    void processRHS(SgNode *node, struct AliasRelationNode &arNode) ;
}

//! Helper class to collect Alias Relations
class CollectAliasRelations {
      //! CFG for a Function
      StaticCFG::CFG *cfg;
      
      //! AliasInfoGenerator to hold the Alias Relations
      AliasInfoGenerator *g;
      
      //! Process a Node and collect the alias relations
      void processNode(SgGraphNode *);
  public:
      enum COLOR {WHITE=0, GREY, BLACK};
      enum TRAVERSAL_TYPE {TOPOLOGICAL=0, NON_TOPOLOGICAL};
      
      CollectAliasRelations(StaticCFG::CFG *_cfg, AliasInfoGenerator *_g ) : cfg(_cfg), g(_g) { }
      void run();
  private:
      //! recursively Collect Alias Information from the CFG Nodes
      void recursiveCollect(SgGraphNode *, boost::unordered_map<SgGraphNode*, CollectAliasRelations::COLOR> &);
};

//! IntraProcedurial DataFlow Analysis to compute exit and entry from all the function which
class IntraProcAliasAnalysis :
        public IntraProcDataFlowAnalysis <SgGraphNode, CompReprPtr> {

    //! AliasInfoGenerator to hold Alias Information
    AliasInfoGenerator *gen;
    
    //! Class Hierarchy Instance
    ClassHierarchyWrapper *classHierarchy;

protected:
    //! The CFG for the function
    StaticCFG::CustomFilteredCFG<AliasCfgFilter> *cfg;
    
    //! The Call Graph Builder for interation computatoin
    CallGraphBuilder *cgBuilder;
    
    //! List of all the CFGNodes in topological order
    vector<SgGraphNode *> cfgNodes;
    
   //typedef FilteredCFGNode<AliasCfgFilter> FilteredCfgNode;
   //typedef FilteredCFGEdge<AliasCfgFilter> FilteredCfgEdge;

    //! Entry CompactRepresentation of the function
   CompReprPtr entry;
   
   //! Exit CompactRepresentation of the function
   CompReprPtr exit;
   
   unsigned long checkPointHash;

   //! Constructs the CFG
   virtual void buildCFG() ;
   
   //! A mapping to hold SgFunctionDeclaration to IntraProcAliasAnalysis 
   boost::unordered_map<SgFunctionDeclaration *, IntraProcAliasAnalysis *> &mapping;
   
   //! A mapping to hold function resolve data
   boost::unordered_map<SgExpression*, std::vector<SgFunctionDeclaration*> > &resolver;
   
   //! Retrieve Alias Relations from Function Parameters. Map data between actual and formal parameters.
   //! Handles return values as well.
   void getFunctionParametersAliasRelations(SgFunctionCallExp *f_exp, SgFunctionDeclaration *funcDecl, 
        std::vector <std::pair<AliasRelationNode, AliasRelationNode> > &arg_relations,
        std::vector <std::pair<AliasRelationNode, AliasRelationNode> > &return_relations);
   
   //! Retrieve Alias Relations from Constructor Parameters. 
   void getConstructorParametersAliasRelations(SgConstructorInitializer *f_exp, SgFunctionDeclaration *funcDecl, 
        std::vector <std::pair<AliasRelationNode, AliasRelationNode> > &arg_relations );
   
   //! Given a callSite and type of object add appropriate virtual function declaration
   bool addVirtualFunction(SgType *type, SgFunctionCallExp *funcExp);
   
   //! Given a CallSite update with all the virtual function calls
   bool updateVirtualFunctionInCallGraph (SgFunctionCallExp *funcCall, CompReprPtr &callSiteIN);
   
   // Get all the aliases 
    void getAliases(CompReprPtr &ptr, AliasRelationNode &node, std::vector<SgVariableSymbol*>& aliases);
    
public:
    //ReachingDefinitionAnalysis(SgNode *head) : DataFlowAnalysis<ReachingDefNode, ReachingDefinitions>(head),  g(0) {
    IntraProcAliasAnalysis(SgNode *head, ClassHierarchyWrapper *_classHierarchy, CallGraphBuilder *_cgB, 
                boost::unordered_map<SgFunctionDeclaration *, IntraProcAliasAnalysis *> &mapping,
                boost::unordered_map<SgExpression*, std::vector<SgFunctionDeclaration*> > &resolver);
    
    //! Get the Entry CompactRepresentation Graph for the function
    CompReprPtr getFunctionEntry() {return entry;}
    
    //! Get the Exit CompactRepresentation Graph for the function
    CompReprPtr getFunctionExit() {return exit;}
    
    //! Get the return statements
    std::vector<AliasRelationNode> getReturnStmts () { return gen->getReturnStmts(); }
    
    //! Set Function Entry CompactRepresentation
    void setFunctionEntry(CompReprPtr &n);
    
    //! Set Function Exit CompactRepresentation
    void setFunctionExit(CompReprPtr &n);
    
    //! run and check whether any changes have been made
    bool runCheck();

    //! Perform the intra procedural analysis
    virtual void run() ;
    
    //! Initialized the Analysis
    void init() { }
    
 // DQ (12/6/2016): Aded virtual to fix compiler warning that we want to considered an error: -Wdelete-non-virtual-dtor
    //! Destructor
    virtual ~IntraProcAliasAnalysis();
    
    //! Meet_data for intra procedural dataflow analysis. This function is called when two path
    //! merges in CFG
    CompReprPtr meet_data( const CompReprPtr& d1, const CompReprPtr& d2);

    //! Get all the CFG Nodes
    virtual std::vector<SgGraphNode *> getAllNodes() {    return cfgNodes;  }
    
    //! Get all the Predecessors of a CFG Node
    virtual std::vector<SgGraphNode *> getPredecessors(SgGraphNode *n);
    
    //! Get the IN data of a CFG Node
    CompReprPtr getCFGInData(SgGraphNode *a) {     return gen->getEntryData(a);   }
    
    //! Get the OUT data of a CFG Node
    CompReprPtr getCFGOutData(SgGraphNode *a) {      return gen->getExitData(a);  }
    
    //! Set the IN data of a CFG Node
    void setCFGInData(SgGraphNode*a, CompReprPtr &b) {      gen->setEntryData(a, b);  }
    
    //! Apply the Transfer function to a node
    //! For an expression ‚Äö√Ñ√∫pi = qj‚Äö√Ñ√π
    //! OutC =TransferFunction(InC)
    //!      = (InC ‚Äö√Ñ√¨ Must(Alias(p,i))  U (*a, b)where  a ≈í¬µ Alias(p,i) and b ≈í¬µ Alias(q,j);

    void applyCFGTransferFunction(SgGraphNode* s);

};

#endif
