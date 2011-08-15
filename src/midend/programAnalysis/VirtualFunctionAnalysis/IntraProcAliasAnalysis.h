#ifndef INTRA_PROC_ALIAS_ANALYSIS_H
#define INTRA_PROC_ALIAS_ANALYSIS_H

#include "sage3basic.h"

#include "IntraProcDataFlowAnalysis.h"
#include "BitVectorRepr.h"
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include "customFilteredCFG.h"
#include <boost/unordered_map.hpp>
#include <algorithm>
#include "ClassHierarchyGraph.h"
#include "CallGraph.h"

#define foreach BOOST_FOREACH
#define reverse_foreach BOOST_REVERSE_FOREACH


using namespace boost;
using namespace std;
typedef BitVectorRepr AliasDefinitions;


struct AliasRelationNode{
    SgVariableSymbol *var;
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
class CompReprBase{


public:
    CompReprBase() {
    }
    virtual void computeAliases (SgVariableSymbol *var, int derefLevel, vector<SgGraphNode *> &) = 0;
    virtual void computeAliases (SgGraphNode *node, int derefLevel, vector<SgGraphNode *> &) = 0;
    virtual void addMustAliasRelation(const AliasRelationNode &left, const AliasRelationNode &right) = 0;
    virtual void addMayAliasRelation(const AliasRelationNode &left, const AliasRelationNode &right) = 0;
    virtual unsigned long getHash() const = 0;
    virtual SgIncidenceDirectedGraph * getGraph() const = 0;
    virtual void merge(const CompReprBase &) = 0;
    virtual void toDot(const std::string& file_name) = 0;

protected:
    SgIncidenceDirectedGraph *graph;
    unsigned long hash;
    /**
     * Protected Destructor to make sure the client code can't delete it 
     */
    ~CompReprBase(){
        delete graph;
        graph = NULL;
    }
};

class CompactRepresentation : public CompReprBase{
    
    
    unordered_map<SgNode *,SgGraphNode *> all_nodes;
    
    SgGraphNode * getGraphNode(SgNode *node);
    void merge(SgIncidenceDirectedGraph *thatGraph);
    void updateHash(SgNode *from, SgNode *to);
    void init();
    void addEdge(SgGraphNode *g_from, SgGraphNode *g_to);
    void processNodes(std::ostream & o, SgGraphNode* n, std::set<SgGraphNode*>& explored);
    void printNodePlusEdges(std::ostream & o, SgGraphNode* node);
    void printNode(std::ostream & o, SgGraphNode* node);
    void printEdge(std::ostream & o, SgDirectedGraphEdge* edge, bool isInEdge);
    
    
public:
    unordered_map<SgNode *,SgGraphNode *>  getNodesMapping(){ return all_nodes;}
    CompactRepresentation() {  init(); }
    SgIncidenceDirectedGraph * getGraph() const{ return graph; }
    unsigned long getHash() const {  return hash; }    
    CompactRepresentation(const CompactRepresentation& copy);
    CompactRepresentation& operator=(const CompactRepresentation& p);
    void computeAliases (SgVariableSymbol *var, int derefLevel, vector<SgGraphNode *> &nodes);
    void computeAliases (SgGraphNode *node, int derefLevel, vector<SgGraphNode *> &);
    void addMustAliasRelation(const AliasRelationNode &left, const AliasRelationNode &right);
    void addMayAliasRelation(const AliasRelationNode &left, const AliasRelationNode &right);
    void merge(const CompReprBase &that);
    bool operator==(const CompactRepresentation &that) const;
    bool operator !=(const CompactRepresentation &that) const;
    void toDot(const std::string& file_name);
};

class CompReprPtr  {
     boost::shared_ptr<CompReprBase> ptr;
public:
    CompReprPtr() { ptr = boost::shared_ptr<CompReprBase>(); }
    CompReprPtr(CompactRepresentation *repr)   { ptr = boost::shared_ptr<CompReprBase>(repr);  }
    CompReprBase * get() const{ return ptr.get();  }
    void addAliasRelation(const AliasRelationNode &left, const AliasRelationNode &right);
    bool operator==(const CompReprPtr &that) const;
    bool operator!=(const CompReprPtr &that) const {  return !(*this == that);   }
    void operator+=(const CompReprPtr &that) const;
};


class AliasInfoGenerator {
    /*  IN of every CFG Node*/
    unordered_map<SgGraphNode *, CompReprPtr> ins;
    
    /*  OUT of every CFG Node*/
    unordered_map<SgGraphNode *, CompReprPtr> outs;
    
    /* All the Return Stmts*/
    std::vector <AliasRelationNode > returnStmts;
    
    /* All the AliasRelations for every CFG Node */
    unordered_map<SgGraphNode *, std::vector <std::pair<AliasRelationNode, AliasRelationNode> > >aliasRelations;    
    
public :
    AliasInfoGenerator();
    void init(SgGraphNode *n);
    CompReprPtr getEntryData(SgGraphNode *node) {   return ins.at(node);    }
    void setEntryData(SgGraphNode *node, CompReprPtr en) {    ins[node] = en;    }
    CompReprPtr getExitData(SgGraphNode *node) {  return outs.at(node);    }
    void setExitData(SgGraphNode *node, CompReprPtr en) {  outs[node] = en;   }
    std::vector <std::pair<AliasRelationNode, AliasRelationNode> > getAliasRelations(SgGraphNode *node);
    void addNewAliasRelation(SgGraphNode *node, std::pair<AliasRelationNode, AliasRelationNode> a_relation) ;
    void addReturnStmt(AliasRelationNode node) { returnStmts.push_back(node);  }
    std::vector<AliasRelationNode> getReturnStmts(){ return returnStmts; } 
    
};

class CollectAliasRelations {

      StaticCFG::CFG *cfg;
      vector<SgGraphNode *> nodes;
      AliasInfoGenerator *g;
      void processNode(SgGraphNode *);
  public:
      enum COLOR {WHITE=0, GREY, BLACK};
      enum TRAVERSAL_TYPE {TOPOLOGICAL=0, NON_TOPOLOGICAL};
      
      CollectAliasRelations(StaticCFG::CFG *_cfg, AliasInfoGenerator *_g ) : cfg(_cfg), g(_g) { }
      vector<SgGraphNode *> getCFGNodes() {  return nodes;  }
      static void processLHS(SgNode *node, struct AliasRelationNode &arNode) ;
      static void processRHS(SgNode *node, struct AliasRelationNode &arNode) ;
      void run();
  private:
      void recursiveCollect(SgGraphNode *, unordered_map<SgGraphNode*, CollectAliasRelations::COLOR> &);
};


class IntraProcAliasAnalysis :
        public IntraProcDataFlowAnalysis <SgGraphNode, CompReprPtr> {
    
    CollectAliasRelations *car;
    AliasInfoGenerator *gen;
    ClassHierarchyWrapper *classHierarchy;
    void processLeftSide(SgNode *, AliasRelationNode &);
    
    

protected:

    SgIncidenceDirectedGraph  *graph;
    StaticCFG::CustomFilteredCFG<AliasCfgFilter> *cfg;
    CallGraphBuilder *cgBuilder;
    vector<SgGraphNode *> cfgNodes;
    
   typedef FilteredCFGNode<AliasCfgFilter> FilteredCfgNode;
   typedef FilteredCFGEdge<AliasCfgFilter> FilteredCfgEdge;

   CompReprPtr entry;
   CompReprPtr exit;
   
   unsigned long checkPointHash;

   virtual void buildCFG() ;
    
   boost::unordered_map<SgFunctionDeclaration *, IntraProcAliasAnalysis *> &mapping;
   boost::unordered_map<SgExpression*, std::vector<SgFunctionDeclaration*> > &resolver;
   void getFunctionParametersAliasRelations(SgFunctionCallExp *f_exp, SgFunctionDeclaration *funcDecl, 
        std::vector <std::pair<AliasRelationNode, AliasRelationNode> > &arg_relations,
        std::vector <std::pair<AliasRelationNode, AliasRelationNode> > &return_relations);
   void getConstructorParametersAliasRelations(SgConstructorInitializer *f_exp, SgFunctionDeclaration *funcDecl, 
        std::vector <std::pair<AliasRelationNode, AliasRelationNode> > &arg_relations );
   bool addVirtualFunction(SgType *type, SgFunctionCallExp *funcExp);
   bool updateVirtualFunctionInCallGraph (SgFunctionCallExp *funcCall, CompReprPtr &callSiteIN);
    
public:
    //ReachingDefinitionAnalysis(SgNode *head) : DataFlowAnalysis<ReachingDefNode, ReachingDefinitions>(head),  g(0) {
    IntraProcAliasAnalysis(SgNode *head, ClassHierarchyWrapper *_classHierarchy, CallGraphBuilder *_cgB, 
                boost::unordered_map<SgFunctionDeclaration *, IntraProcAliasAnalysis *> &mapping,
                boost::unordered_map<SgExpression*, std::vector<SgFunctionDeclaration*> > &resolver);
    
    CompReprPtr getFunctionEntry() {return entry;}
    CompReprPtr getFunctionExit() {return exit;}
    std::vector<AliasRelationNode> getReturnStmts () { return gen->getReturnStmts(); }
    void setFunctionEntry(CompReprPtr &n);
    void setFunctionExit(CompReprPtr &n);
    bool runCheck();

    virtual void run() ;
    void init() { }
    ~IntraProcAliasAnalysis();
    void getAliases(CompReprPtr &ptr, AliasRelationNode &node, std::vector<SgVariableSymbol*>& aliases);
    CompReprPtr meet_data( const CompReprPtr& d1, const CompReprPtr& d2);
    virtual CompReprPtr get_empty_data() const {   return CompReprPtr(new CompactRepresentation()); }
    virtual std::vector<SgGraphNode *> getAllNodes() {    return cfgNodes;  }
    virtual std::vector<SgGraphNode *> getPredecessors(SgGraphNode *n);
    CompReprPtr getCFGInData(SgGraphNode *a) {     return gen->getEntryData(a);   }
    CompReprPtr getCFGOutData(SgGraphNode *a) {      return gen->getExitData(a);  }
    void setCFGInData(SgGraphNode*a, CompReprPtr &b) {      gen->setEntryData(a, b);  }
    void applyCFGTransferFunction(SgGraphNode* s);

};

#endif
