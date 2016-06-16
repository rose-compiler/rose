#include "taintAnalysis.h"

using namespace rose;

int taintanalysis_debug_level = 2;

void TaintLattice::initialize()
{
    // not sure if this function is required
}


// return a new copy of this lattice
Lattice* TaintLattice::copy() const
{
    return new TaintLattice(*this);
}

// copy that lattice into this
void TaintLattice::copy(Lattice *that_arg)
{
    TaintLattice *that = dynamic_cast<TaintLattice*>(that_arg);
    this->level = that->level;
}

// compute the meet of this and that
bool TaintLattice::meetUpdate(Lattice *that_arg)
{
    TaintLattice* that = dynamic_cast<TaintLattice*>(that_arg);    
    bool modified = false;
    /*
    if(this->level == bottom) {
        if(that->level == bottom) {
            // do nothing
        }
        // replace all else if by single else later
        else if(that->level == top) {
            this->level = top;
            modified = true;
        }
        else if(that->level == taintyes) {
            this->level = taintyes;
            modified = true;
        }
        else if(that->level == taintno) {
            this->level = taintno;
            modified = true;
        }
    }
    else if(this->level == taintyes) {
        if(that->level == top) {
            this->level = top;
            modified = true;
        }
        else {
            // do nothing for other cases
        }
    }
    else if(this->level == taintno) {
        if(that->level == top) {
            this->level = top;
            modified = true;
        }
        else if(that->level == taintyes) {
            this->level = taintyes;
            modified = true;
        }
        else {
            // do nothing for other cases
        }
    }
    // this-level is top
    else {
        // really do nothing
    }
    */
    
    if(that->level == bottom && this->level == bottom) {
        // do nothing
    }
    else if(that->level != bottom && this->level == bottom) {
        this->setLevel(that->getLevel());
        modified = true;
    }
    // verify this case
    else if(that->level == taintyes && this->level == taintno) {
        this->setTop();
        modified = true;
    }
    // verify this case
    else if(that->level == taintno && this->level == taintyes) {
        this->setTop();
        modified = true;
    }
    else if(that->level == top && this->level != top) {
        this->setTop();
        modified = true;
    }
    else {
        // do nothing
    }

    return modified;    
}

TaintLattice::latticeLevel TaintLattice::getLevel()
{
    return this->level;
}

void TaintLattice::setTainted()
{
    this->level = taintyes;
}

void TaintLattice::setUntainted()
{
    this->level = taintno;
}

void TaintLattice::setTop()
{
    this->level = top;
}

void TaintLattice::setBottom()
{
    this->level = bottom;
}

void TaintLattice::setLevel(latticeLevel _level)
{
    this->level = _level;
}

bool TaintLattice::operator==(Lattice *that_arg)
{
    TaintLattice *that = dynamic_cast<TaintLattice*>(that_arg);
    return (that->level == this->level);
}

std::string TaintLattice::str(std::string indent)
{
    ostringstream outs;
    if(level == taintyes) {
        outs << indent << "[TaintLattice : tainted]";
    }
    else if(level == taintno) {
        outs << indent << "TaintLattice : untainted]";
    }
    else if(level == top) {
        outs << indent << "[TaintLattice : top]";
    }
    else {
        outs << indent << "[TaintLattice : bottom]";
    }
    return outs.str();
}

/***************************************
 ****** TaintAnalysisTransfer **********
 **************************************/

TaintAnalysisTransfer::TaintAnalysisTransfer(const Function& func, const DataflowNode& n, NodeState& state, const std::vector<Lattice*>& dfInfo) : VariableStateTransfer<TaintLattice>(func, n, state, dfInfo, taintanalysis_debug_level)
{
}

bool TaintAnalysisTransfer::finish()
{
    return modified;
}

// update _this lattice
bool TaintAnalysisTransfer::evaluateAndSetTaint(TaintLattice* _this, TaintLattice* _that)
{
    ROSE_ASSERT(_that != NULL);
    bool modified = false;

    TaintLattice::latticeLevel _thatlevel = _that->getLevel();
    TaintLattice::latticeLevel _thislevel = _this->getLevel();

    if(_thatlevel == TaintLattice::bottom && _thislevel == TaintLattice::bottom) {
        // do nothing
    }
    // copy that level if this is bottom
    else if(_thatlevel != TaintLattice::bottom && _thislevel == TaintLattice::bottom) {
        _this->setLevel(_thatlevel);
        modified = true;
    }
    // now _this is taint
    else if(_thatlevel == TaintLattice::taintyes && _thislevel == TaintLattice::taintno) {
        _this->setLevel(_thatlevel);
        modified = true;
    }
    else if(_thatlevel == TaintLattice::top && _thislevel != TaintLattice::top) {
        _this->setTop();
        modified = true;
    }
    else {
        // do nothing
    }

    return modified;
}
    

bool TaintAnalysisTransfer::transferTaint(SgBinaryOp* sgn)
{
    bool modified = false;
    ROSE_ASSERT(sgn != NULL);
    TaintLattice* lhs_lat = getLattice(dynamic_cast<SgExpression*> (sgn->get_lhs_operand()));      // unsafe ??
    TaintLattice* rhs_lat = getLattice(dynamic_cast<SgExpression*> (sgn->get_rhs_operand()));
    TaintLattice* res_lattice = getLattice(dynamic_cast<SgExpression*> (sgn));
    // not a simple copy - do a meet to copy lattice information from right - left    
    if(lhs_lat) {
        modified = evaluateAndSetTaint(lhs_lat, rhs_lat);
        /*
        lhs_lat->copy(rhs_lat); modified |= true;
        */
    }
    if(res_lattice) {
        modified = evaluateAndSetTaint(res_lattice, rhs_lat);
        /*
        res_lattice->copy(rhs_lat); modified |= true;
        */
    }
    return modified;
}

void TaintAnalysisTransfer::visit(SgFunctionCallExp* sgn)
{
    ROSE_ASSERT(sgn != NULL);

    // NOTE: This is applicable only for statically resolvable functions
    SgFunctionDeclaration* fn_decl = sgn->getAssociatedFunctionDeclaration();
    TaintLattice* res_lattice = getLattice(sgn);
    ROSE_ASSERT(res_lattice != NULL);
    /*
    if(!this->functionsinfile->isFunctionInFile(fn_decl)) {
        modified = true;
        res_lattice->setTainted();
    }
    */
    SecureFunctionType* attr = dynamic_cast<SecureFunctionType*> (sgn->getAttribute("SECURE_TYPE") );
    if(! attr->isSecure() ) {
        res_lattice->setTainted();
    }
    else {
        res_lattice->setUntainted();
    }
    modified = true;
}

void TaintAnalysisTransfer::visit(SgIntVal* sgn)
{
    ROSE_ASSERT(sgn != NULL);
    TaintLattice* res_lattice = getLattice(sgn);
    ROSE_ASSERT(res_lattice);
    modified = true;
    res_lattice->setUntainted();
}

void TaintAnalysisTransfer::visit(SgValueExp* sgn)
{
    ROSE_ASSERT(sgn != NULL);
    TaintLattice* res_lattice = getLattice(sgn);
    ROSE_ASSERT(res_lattice);
    modified = true;
    res_lattice->setUntainted();
}
    

void TaintAnalysisTransfer::visit(SgAssignOp* sgn)
{
    modified = transferTaint(sgn);
}

void TaintAnalysisTransfer::visit(SgPlusAssignOp* sgn)
{
    modified = transferTaint(sgn);
}

void TaintAnalysisTransfer::visit(SgMinusAssignOp* sgn)
{
    modified = transferTaint(sgn);
}

void TaintAnalysisTransfer::visit(SgMultAssignOp* sgn)
{
    modified = transferTaint(sgn);
}

void TaintAnalysisTransfer::visit(SgDivAssignOp* sgn)
{
    modified = transferTaint(sgn);
}

void TaintAnalysisTransfer::visit(SgModAssignOp* sgn)
{
    modified = transferTaint(sgn);
}


// not sure if the following are required
// following op come under SgCompoundAssignOp requiring to transfer taint value ?
void TaintAnalysisTransfer::visit(SgAndAssignOp* sgn)
{
    modified = transferTaint(sgn);
}

void TaintAnalysisTransfer::visit(SgExponentiationAssignOp* sgn)
{
    modified = transferTaint(sgn);
}

void TaintAnalysisTransfer::visit(SgIntegerDivideAssignOp* sgn)
{
    modified = transferTaint(sgn);
}

void TaintAnalysisTransfer::visit(SgIorAssignOp* sgn)
{
    modified = transferTaint(sgn);
}

void TaintAnalysisTransfer::visit(SgLshiftAssignOp* sgn)
{
    modified = transferTaint(sgn);
}

void TaintAnalysisTransfer::visit(SgRshiftAssignOp* sgn)
{
    modified = transferTaint(sgn);
}

void TaintAnalysisTransfer::visit(SgXorAssignOp* sgn)
{
    modified = transferTaint(sgn);
}




/***************************************
 ******* taint Analysis ****************
 **************************************/

void TaintAnalysis::genInitState(const Function& func, const DataflowNode& n, const NodeState& state,
                                 vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts)
{
    map<varID, Lattice*> emptyM;
    //FiniteVariablesProductLattice *l = new FiniteVariablesProductLattice(true, true, (Lattice*) new TaintLattice(), emptyM, (Lattice*) NULL,
    //                                                                     func, n, state);
    FiniteVarsExprsProductLattice *l = new FiniteVarsExprsProductLattice((Lattice*) new TaintLattice(), emptyM, (Lattice*) NULL, ldva, n, state);
    initLattices.push_back(l);
}


// identity transfer function for now
bool TaintAnalysis::transfer(const Function& func, const DataflowNode& node, NodeState& state, const vector<Lattice*>& dfInfo)
{
    assert(0);
    return false;
}

boost::shared_ptr<IntraDFTransferVisitor>
TaintAnalysis::getTransferVisitor(const Function& func, const DataflowNode& n, NodeState& state, const std::vector<Lattice*>& dfInfo)
{
    return boost::shared_ptr<IntraDFTransferVisitor>(new TaintAnalysisTransfer(func, n, state, dfInfo));
}    

/*****************************************
 ******** FuntionType Annotation *********
 *****************************************/

void SecureFunctionTypeTraversal::visit(SgNode* sgn)
{
    if(isSgFunctionCallExp(sgn)) {
        SgFunctionDeclaration* fndecl = isSgFunctionCallExp(sgn)->getAssociatedFunctionDeclaration();
        string filename = fndecl->get_file_info()->get_filename();
        if(filename != "compilerGenerated") {
            StringUtility::FileNameClassification classification;
            classification = StringUtility::classifyFileName(filename, this->sourcedir, this->trustedlibs);
            //StringUtility::FileNameLocation filetypeclassification = classification.getLocation();
            //cout << sgn->unparseToString() <<  filename << " : "  << classification.getLocation() << ", " <<  classification.getLibrary() << endl;
            bool isSecure;
            if(untrustedFunctions.find(fndecl->get_name().getString()) != untrustedFunctions.end()) {
                isSecure = false;
            }                 
                
            else if(classification.isLibraryCode() || classification.isUserCode()) {
                isSecure = true;
            }

            else {
                isSecure = false;
            }
            AstAttribute* attr = dynamic_cast<AstAttribute*> ( new SecureFunctionType(isSecure) );
            sgn->setAttribute("SECURE_TYPE", attr);            
        }
    }
}

void SecureFunctionTypeTraversalTest::visit(SgNode* sgn)
{
    if(isSgFunctionCallExp(sgn)) {
        SecureFunctionType* attr = dynamic_cast<SecureFunctionType*> (sgn->getAttribute("SECURE_TYPE") );
        ROSE_ASSERT(attr != NULL);

        cout << sgn->unparseToString() << ": " ; (attr->isSecure()) ? cout << "trusted\n" : cout << "untrusted\n";
    }
}
