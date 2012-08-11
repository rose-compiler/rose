#ifndef POINTERALIAS_ANALYSIS_H
#define POINTERALIAS_ANALYSIS_H

#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <algorithm>
#include "VariableStateTransfer.h"


extern int pointerAliasAnalysisDebugLevel;

/*
    Pointer Alias Analysis
    AliasRelationNode : A struct that holds the alias information which includes the deference 
                Count for each pointer variable. E.g., int *p = &q would maintain a pair like so:
                pair( <p(SgVarSymbol), p(varID), 1> , < q(SgVariableSymbol), q(varID), -1 > ).
                AliasRelationNodes are used to create compact representation graphs for the pointers at 
                each CFG Node, by creating/removing an edge between the varID's of the left and right AliasRelationNodes
*/
struct AliasRelationNode{
    //! The VariableSymbol participating in aliasing
    SgVariableSymbol* var;
    varID vID;
    //! Dereference Level 
    //!  *a  +1
    //! **a  +2
    //!   a   0
    //!  &a  -1
    int derefLevel;

    bool operator==(const AliasRelationNode &that) const {
        assert(this != NULL);
        return (this->var == that.var && this->vID == that.vID && this->derefLevel == that.derefLevel);
    }

    bool operator<(const AliasRelationNode &that) const{
        assert(this != NULL);
        return true;
    }
};


/*
    Lattices:   Per variable lattices using the FiniteVarsExprProductLattice
                Each variable maintains a set<varID> and set< pair<AliasRelationNode,AliasRelationNode> >
                The set of all per-variable lattices is an abstraction of the compact representation graph, i.e, a structure to maintain
                the pointer alias information at each CFG Node.
*/
class pointerAliasLattice : public FiniteLattice
{
public:
        // Possible levels of this compact representation(CR) graph.
        typedef enum {
                // Uninitialized CR graph. Graph is empty
                bottom=0,
                //Full graph with Points-to information
                top
                }levels;
protected:
        levels level;
        //per variable aliases
        set<varID> aliases;

        //Set of all alias relations per CFG Node
        set< std::pair<AliasRelationNode, AliasRelationNode> > aliasRelations;

public:
        pointerAliasLattice():level(bottom){};
        void initialize();
        Lattice* copy()const ;
        void copy(Lattice* that);
        bool operator==(Lattice*);
        
        bool meetUpdate(Lattice* that);
        std::string str(std::string);

        void setAliases(varID al);
        void clearAliases();
        void setAliasRelation(std::pair < AliasRelationNode, AliasRelationNode > alRel);
        set< std::pair<AliasRelationNode, AliasRelationNode> > getAliasRelations();
        set<varID> getAliases();
private:
        template <typename T> bool search(set<T> thisSet, T value);
};



/*
    Transfer:   We define visit functions for SgFunctinCallExp, SgAssignOp, SgAssignInitializer, SgConstructorInitializer
                i.e., the CFG nodes that could potentially update any pointers.
                processLHS() and processRHS() functions are used to find the AliasRelations at a CFG node, using the LHS and RHS of 
                the given expression. 
                updateAliases() is used to update the compact representation graph, which is nothing but the set of Aliases at each CFG node.
                computeAliases() is used to compute the Aliases for a given variable, using the compact representation graph 
*/
class pointerAliasAnalysisTransfer : public VariableStateTransfer<pointerAliasLattice>
{
    private:
          using VariableStateTransfer<pointerAliasLattice>::getLattices;

    public:
          void visit(SgFunctionCallExp *sgn);
          void visit(SgAssignOp *sgn);
          void visit(SgAssignInitializer *sgn);
          void visit(SgConstructorInitializer *sgn);
        
          bool finish();
          pointerAliasAnalysisTransfer(const Function& func, const DataflowNode& n, NodeState& state, const std::vector<Lattice*>& dfInfo);
    private:
          
          void processLHS(SgNode *node, struct AliasRelationNode &arNode);
          void processRHS(SgNode *node, struct AliasRelationNode &arNode);
          bool updateAliases(set< std::pair<AliasRelationNode, AliasRelationNode> > aliasRelations,int isMust);
          void computeAliases(pointerAliasLattice *lat, varID var, int derefLevel, set<varID> &result);
}; 



class pointerAliasAnalysis : public IntraFWDataflow
{
protected:
    LiveDeadVarsAnalysis* ldva;

public:
    pointerAliasAnalysis(LiveDeadVarsAnalysis* ldva);
    void genInitState(const Function& func, const DataflowNode& n, const NodeState& state,std::vector<Lattice*>& initLattices, std::vector<NodeFact*>& initFacts);
    bool transfer(const Function& func, const DataflowNode& n, NodeState& state, const std::vector<Lattice*>& dfInfo);
    boost::shared_ptr<IntraDFTransferVisitor> getTransferVisitor(const Function& func, const DataflowNode& 
n, NodeState& state, const std::vector<Lattice*>& dfInfo);
};

#endif
