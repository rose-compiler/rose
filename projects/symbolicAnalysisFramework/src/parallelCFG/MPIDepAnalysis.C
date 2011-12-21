#include "MPIDepAnalysis.h"

int MPIDepAnalysisDebugLevel = 2;

void MPIDepLattice::copy(Lattice* _that)
{
    MPIDepLattice* that = dynamic_cast<MPIDepLattice*> (_that);
    this->level = that->level;
    this->MPIDep = that->MPIDep;
}

bool MPIDepLattice::operator==(Lattice* _that)
{
    MPIDepLattice* that = dynamic_cast<MPIDepLattice*> (_that);
    return (that->level == this->level);
}

bool MPIDepLattice::meetUpdate(Lattice* _that)
{
    bool modified = false;
    MPIDepLattice* that = dynamic_cast<MPIDepLattice*> (_that);
    
    if(this->level == bottom && that->level != bottom) {
        this->level = that->level;
        modified = true;
    }
    else if(this->level != top && that->level == top) {
        this->level = top;
        modified = true;
    }
    else if(this->level == yes && that->level == no) {
        this->level = top;
        modified = true;
    }
    else if(this->level == no && that->level == yes) {
        this->level = top;
        modified = true;
    }
    else {
        // do nothing
    }
    
    return modified;
}

std::string MPIDepLattice::str(std::string indent)
{
    ostringstream outs;
    if(level == bottom) {
        outs << indent << "[MPIDepLattice : bottom]";
    }
    else if(level == top) {
        outs << indent << "[MPIDepLattice : top]";
    }
    else if(level == yes) {
        outs << indent << "[MPIDepLattice : yes]";
    }
    else {
        outs << indent << "[MPIDepLattice : no]";
    }

    return outs.str();
}

////////////////////////////////////////////////////
/////       MPIDepAnalysisTransfer       ///////////
////////////////////////////////////////////////////

MPIDepAnalysisTransfer::MPIDepAnalysisTransfer (const Function& func,
                                                const DataflowNode& n,
                                                NodeState& state,
                                                const std::vector<Lattice*>& dfInfo
    ) : VariableStateTransfer<MPIDepLattice>(func, n, state, dfInfo, MPIDepAnalysisDebugLevel /* debug level */ )
{ }

bool MPIDepAnalysisTransfer::finish()
{
    return modified;
}

void MPIDepAnalysisTransfer::visit(SgFunctionCallExp* sgn)
{    
    ROSE_ASSERT(sgn != NULL);
    // Get arguments associated with function calls
    vector<SgExpression*> sgexprptrlist = (sgn->get_args())->get_expressions();
    Function callee(sgn);
    
    if(callee.get_name().getString() == "MPI_Comm_rank" ||
       callee.get_name().getString() == "MPI_Comm_size") {
        //NOTE: for now look only at second argument
        // MPI internal variables are passed by refernce
        // need support to distinguish mpi vars from program vars
        SgExpression* arg1 = *(++(sgexprptrlist.begin()));
        assert(arg1 != NULL);
        if(isSgAddressOfOp(arg1) && varID::isValidVarExp( (isSgAddressOfOp(arg1))->get_operand())) {
                varID dep_var = SgExpr2Var( (isSgAddressOfOp(arg1))->get_operand());
                MPIDepLattice* res_lattice = dynamic_cast<MPIDepLattice*> ( getLattice (dep_var));
                // NOTE: res_lattice can be NULL
                // Why is it not initialized ?
                if(res_lattice) {
                    res_lattice->setToYes();
                    modified = true;
                }
            }       
    }           
}

void MPIDepAnalysis::genInitState(const Function& func, const DataflowNode& n, const NodeState& state,
                                 vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts)
{
    map<varID, Lattice*> emptyM;
    
    FiniteVarsExprsProductLattice *l = new FiniteVarsExprsProductLattice((Lattice*) new MPIDepLattice(), emptyM, (Lattice*) NULL, ldva, n, state);
    initLattices.push_back(l);
}

bool MPIDepAnalysis::transfer(const Function& func, const DataflowNode& node, NodeState& state, const vector<Lattice*>& dfInfo)
{
    assert(0);
    return false;
}

boost::shared_ptr<IntraDFTransferVisitor>
MPIDepAnalysis::getTransferVisitor(const Function& func, const DataflowNode& n, NodeState& state, const std::vector<Lattice*>& dfInfo)
{
    return boost::shared_ptr<IntraDFTransferVisitor>(new MPIDepAnalysisTransfer(func, n, state, dfInfo));
}
