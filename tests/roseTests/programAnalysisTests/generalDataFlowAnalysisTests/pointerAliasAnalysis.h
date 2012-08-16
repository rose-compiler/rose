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
    aliasDerefCount : A struct that holds the alias information which includes the deference 
                Count for each pointer variable. E.g., int *p = &q would maintain a pair like so:
                pair( <p(SgVarSymbol), p(varID), 1> , < q(SgVariableSymbol), q(varID), -1 > ).
                aliasDerefCounts are used to create compact representation graphs for the pointers at 
                each CFG Node, by creating/removing an edge between the varID's of the left and right aliasDerefCounts
*/
struct aliasDerefCount{
    //! The VariableSymbol participating in aliasing
    SgVariableSymbol* var;
    varID vID;
    //! Dereference Level 
    //!  *a  +1
    //! **a  +2
    //!   a   0
    //!  &a  -1
    int derefLevel;

    bool operator==(const aliasDerefCount &that) const {
        assert(this != NULL);
        return (this->var == that.var && this->vID == that.vID && this->derefLevel == that.derefLevel);
    }

    bool operator<(const aliasDerefCount &that) const{
        assert(this != NULL);
        return true;
    }
};


/*
    Lattices:   Per variable lattices using the FiniteVarsExprProductLattice
                Each variable maintains a set<varID> and set< pair<aliasDerefCount,aliasDerefCount> >
                The set of all per-variable lattices is an abstraction of the compact representation graph, i.e, a structure to maintain
                the pointer alias information at each CFG Node.

    Example:    Consider the code:
                    int **x;
                    int *p,*q;
                    int a;
                    p = &a;
                    q = p;
                    x = &p;

                The aliases at the end of this piece of code would look something like this:
                p -> a
                q -> a
                x -> p

                The compact representation graph would be a graph containing an edge from 'x' to 'p', 'p' to 'a' and an edge from 'q' to 'a'

                To represent it as a lattice, we have per variable lattices like so:
                p: {a}
                q: {a}
                x: {p}
                where each variable such as 'p' and 'q' here contain a set<varID> as it aliases.

                Since each variable pointer is stored with a derefernce count in aliasDerefCount, we use that information to traverse the per variable lattices using a recursive algorithm called "computeAliases". For ex: derefernce 2 for variable x gives {a}
*/
class pointerAliasLattice : public FiniteLattice
{
protected:
        //per variable aliases
        set<varID> aliasedVariables;

        //Set of all alias relations per CFG Node. These relations denote the graph edges in the compact represntation graph for pointers
        set< std::pair<aliasDerefCount, aliasDerefCount> > aliasRelations;

public:
        pointerAliasLattice(){};
        void initialize();
        Lattice* copy()const ;
        void copy(Lattice* that);
        bool operator==(Lattice*);
        
        bool meetUpdate(Lattice* that);
        std::string str(std::string);

        void setAliasedVariables(varID al);
        void clearAliasedVariables();
        void setAliasRelation(std::pair < aliasDerefCount, aliasDerefCount > alRel);
        set< std::pair<aliasDerefCount, aliasDerefCount> > getAliasRelations();
        set<varID> getAliasedVariables();
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
          //Visit function to apply "transfer" on the specified SgNode in CFG
          void visit(SgFunctionCallExp *sgn);
          void visit(SgAssignOp *sgn);
          void visit(SgAssignInitializer *sgn);
          void visit(SgConstructorInitializer *sgn);
        
          bool finish();
          pointerAliasAnalysisTransfer(const Function& func, const DataflowNode& n, NodeState& state, const std::vector<Lattice*>& dfInfo);
    private:
         
          //processes LHS of an expression 'node' and populates 'arNode' with the varID and its derefCount
          void processLHS(SgNode *node, struct aliasDerefCount &arNode);


          //processes RHS of an expression 'node' and populates 'arNode' with the varID and its derefCount
          void processRHS(SgNode *node, struct aliasDerefCount &arNode);
          
          //Updates the 'aliasedVariables' set by establishing an relation('edge' in compact representation graph) between 'aliasRelations' pair. 'isMust' denotes may or must alias
          bool updateAliases(set< std::pair<aliasDerefCount, aliasDerefCount> > aliasRelations,int isMust);

          //Recursive function to traverse the per-variable lattices to compute Aliases for 'var' at deref count of 'derefLevel'
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
