#include "pCFGIteratorTransfer.h"

bool pCFGIteratorTransfer::isMpiDepCond(SgIfStmt* sgif)
{
    // check if the if condition is rank dependant
    // get the cond_expr
    SgExpression* cond_expr = (isSgExprStatement(sgif->get_conditional()))->get_expression();
    ROSE_ASSERT(cond_expr != NULL);
    varID cond_expr_var = SgExpr2Var(cond_expr);
    //ROSE_ASSERT(cond_expr_var != NULL);
    // get dataflow node, node state corresponding to mpidep analysis
    NodeState* mpidep_state = NodeState::getNodeState( pcfg_node.getCurNode(pSet), 0);
    ROSE_ASSERT(mpidep_state != NULL);
    vector<Lattice*>& mpi_dfInfo = mpidep_state->getLatticeBelowMod(mda);

    FiniteVarsExprsProductLattice* prodLat = dynamic_cast<FiniteVarsExprsProductLattice*> (*(mpi_dfInfo.begin()));
    MPIDepLattice* dep_lattice = dynamic_cast<MPIDepLattice*> (prodLat->getVarLattice(cond_expr_var));

    ROSE_ASSERT(dep_lattice != NULL);

    if(dep_lattice->getLevel() == MPIDepLattice::yes)
        return true;

    // return false otherwise
    return false;
}


void pCFGIteratorTransfer::visit(SgIfStmt* sgif)
{
    
    if(isMpiDepCond(sgif)) {
        // pcfgSplitAnnotation* annotation = dynamic_cast<pcfgSplitAnnotation*> (sgif->getAttribute("pCFGAnnotation"));
        //TODO: stick this annotation to pCFGState maybe ??

        //we need to split process set
        isSplitPSet = true;
        //set others two false
        // isDeadPSet = false;
        // isSplitPNode = false;
        // isBlockPSet =false;
        // isMergePSet = false;

        // do a outEdges() split
        // populate splitPSetNodes
        const DataflowNode& dfNode = pcfg_node.getCurNode(pSet);
        vector<DataflowEdge> dfEdges = dfNode.outEdges();
        vector<DataflowEdge>::iterator dfEdgeI;
        //split size is dfEdges.size()
        for(dfEdgeI = dfEdges.begin(); dfEdgeI != dfEdges.end(); dfEdgeI++) {
            DataflowNode splitTarget = (*dfEdgeI).target();
            splitPSetNodes.push_back(splitTarget);
        }        
    }
  
}

void pCFGIteratorTransfer::visit(SgFunctionCallExp* sgfcexp)
{
    Function callee(sgfcexp);
    //TODO: add other mpi blocking calls
    if(callee.get_name().getString() == "MPI_Send" ||
       callee.get_name().getString() == "MPI_Recv") {
        isBlockPSet = true;        
    }
}

void pCFGIteratorTransfer::visit(SgPragmaDeclaration* sgpragma)
{
    // we only care about merge annotation
    Annotation* annotation = dynamic_cast<Annotation*> (sgpragma->getAttribute("pCFGAnnotation"));
    if(annotation) { // there could be other annotations
        if(annotation->getType() == pcfg_merge) {
            isMergePSet = true;
        }
    }
}
