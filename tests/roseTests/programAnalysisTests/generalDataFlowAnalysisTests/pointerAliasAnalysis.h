#ifndef POINTERALIAS_ANALYSIS_H
#define POINTERALIAS_ANALYSIS_H

/*
#include "genericDataflowCommon.h"
#include "analysisCommon.h"
#include "analysis.h"
#include "dataflow.h"
#include "latticeFull.h"
*/
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <algorithm>
#include "VariableStateTransfer.h"


extern int pointerAliasAnalysisDebugLevel;

//! A struct to hold the information about an alias node
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
        bool addVar(const varID& var);
        bool remVar(const varID& var);
        bool exists(const varID& var);
        
        std::string str(std::string);
        bool widenUpdate(InfiniteLattice* that);

        void setAliases(varID al);
        void clearAliases();
        void setAliasRelation(std::pair < AliasRelationNode, AliasRelationNode > alRel);
        set< std::pair<AliasRelationNode, AliasRelationNode> > getAliasRelations();
        set<varID> getAliases();
private:
        void printLattice(set<varID> vars);
        template <typename T> bool search(set<T> thisSet, T value);
};




class pointerAliasAnalysisTransfer : public VariableStateTransfer<pointerAliasLattice>
{
    private:
          using VariableStateTransfer<pointerAliasLattice>::getLattices;

    public:
          void visit(SgIntVal *sgn);
          void visit(SgFunctionCallExp *sgn);
          void visit(SgAssignStatement *sgn);
          void visit(SgAssignOp *sgn);
          void visit(SgAssignInitializer *sgn);
          void visit(SgConditionalExp *sgn);
          void visit(SgConstructorInitializer *sgn);
          void visit(SgReturnStmt *sgn);  
        
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
