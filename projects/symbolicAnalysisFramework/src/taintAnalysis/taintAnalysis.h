#ifndef _TAINTANALYSIS_H
#define _TAINTANALYSIS_H

#include "genericDataflowCommon.h"
#include "VirtualCFGIterator.h"
#include "cfgUtils.h"
#include "CallGraphTraverse.h"
#include "analysisCommon.h"
#include "analysis.h"
#include "dataflow.h"
#include "latticeFull.h"
#include "liveDeadVarAnalysis.h"
#include "printAnalysisStates.h"
#include "VariableStateTransfer.h"

#include <set>
#include "string_functions.h"

extern int taintAnalysisDebugLevel;

/*********************************************
 *********************************************
 Naive Taint Analysis Implementation
 Author: Sriram Aananthakrishnan
 email : sriram@cs.utah.edu
 
 Taint Identification:
  user code or library code are not taint sources
  Taint source can be specified explicitly using SecureFunctionTraversal::addToUntrustedFunc()

 No support for pointers and references yet. It understands only variables
 3 ways to taint a var:
 1. x = taint_func() - handled
 2. pass ref: taint_func(var &x) - not handled
 3. pass ptrs: taint_func(var *x) - not handled

 taint flow into other variables handled
 *********************************************
 *********************************************/

class TaintLattice : public FiniteLattice
{      
    public:
    /*-----------------------------------
    -------- Taint Lattice -------------
    ------------------------------------
    top
    |
    taintyes
    |
    taintno
    |
    bottom
    ------------------------------------
  */
    
// levels of taint lattice
    typedef enum {
        // no information about the variable
        bottom,

        // variable is tainted
        taintyes,

        // variable is not tainted
        taintno,

        // either tainted or not tainted on different paths
        top } latticeLevel;

    private:
    // tells whether the variable is tainted or not
    // only level is required for taint analysis
    latticeLevel level;

    public:
    
    // default constructor
    TaintLattice()
    {
        level = bottom;
    }

    // copy constructor
    TaintLattice(const TaintLattice& that)
    {
        this->level = that.level;
    }

    void initialize();

    // overridden method to return copy of this lattice
    Lattice* copy() const;

    // overridden method to copy from lattice 'that
    void copy(Lattice* that);

    // overridden to compute meet of this and that    
    bool meetUpdate(Lattice* that);

    // overridden comparison operator
    bool operator==(Lattice* that);

    // get level
    latticeLevel getLevel();
    
    // setlevel
    void setTainted();

    void setUntainted();

    void setTop();

    void setBottom();

    void setLevel(latticeLevel);
    
    // debug print
    std::string str(std::string indent="");

};

class TaintAnalysisTransfer : public VariableStateTransfer<TaintLattice>
{
    public:
    bool evaluateAndSetTaint(TaintLattice*, TaintLattice*);

    void visit(SgFunctionCallExp*);
    void visit(SgIntVal*);

    // constants are untainted
    // visit(SgValueExp*) can be used to mark untainted for all constants ?
    void visit(SgValueExp*);
    void visit(SgAssignOp*);

    // Can all the following compound assign op functions be replaced by visit(SgCompoundAssignOp*) ??
    void visit(SgPlusAssignOp*);
    void visit(SgMinusAssignOp*);
    void visit(SgMultAssignOp*);
    void visit(SgDivAssignOp*);
    void visit(SgModAssignOp*);
    void visit(SgAndAssignOp*);
    void visit(SgExponentiationAssignOp*);
    void visit(SgIntegerDivideAssignOp*);
    void visit(SgIorAssignOp*);
    void visit(SgLshiftAssignOp*);
    void visit(SgRshiftAssignOp*);
    void visit(SgXorAssignOp*);

    bool transferTaint(SgBinaryOp*);
    
    bool finish();
    TaintAnalysisTransfer(const Function& func, const DataflowNode& n, NodeState& state, const std::vector<Lattice*>& dfInfo);
};

class FunctionsInFile;

class TaintAnalysis : public IntraFWDataflow
{
    protected:
    // required by FiniteVarsExprsProductLattice
    // creates a lattice for every var
    static map<varID, Lattice*> taintVars;
    LiveDeadVarsAnalysis *ldva;
    FunctionsInFile *functionsinfile;
    

    public:
    TaintAnalysis(LiveDeadVarsAnalysis* ldva_arg) : IntraFWDataflow()
    {
        this->ldva=ldva_arg;
    }

    TaintAnalysis(LiveDeadVarsAnalysis *ldva_arg, FunctionsInFile *functionsinfile_arg) : IntraFWDataflow()
    {
        this->ldva = ldva_arg;
        this->functionsinfile = functionsinfile_arg;
    }

    //TaintAnalysis() : IntraFWDataflow()
    //{
        //this->ldva = ldva_arg;
    //}

    // generates the initial lattice state for given the function func with dataflownode 
    void genInitState(const Function& func, const DataflowNode& n, const NodeState& state,
                      vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts);

    // transfer function that maps current state to next state
    bool transfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo);

    // returns an instance of object that has transfer functions to map from current state to next state
    boost::shared_ptr<IntraDFTransferVisitor> getTransferVisitor(const Function& func, const DataflowNode& node, NodeState& state, const std::vector<Lattice*>& dfInfo);
  
};

// identify the type of each function
// library or user defined
class SecureFunctionType: public AstAttribute
{
    bool trusted;
    public:
    SecureFunctionType(bool trusted_arg) 
    {
        trusted = trusted_arg;
    }

    bool isSecure()
    {
        return trusted;
    }    
};

class SecureFunctionTypeTraversal : public AstSimpleProcessing
{
    string sourcedir;
    map<string, string> trustedlibs;
    set<string> untrustedFunctions;

    public:
    SecureFunctionTypeTraversal(string _sourcedir)
    {
        sourcedir = _sourcedir;
    }

    // default constructor
    SecureFunctionTypeTraversal()
    {
        sourcedir = "";
    }

    void visit(SgNode* n);

    void addToTrustedLib(string location, string name) 
    {
        trustedlibs[location] = name;
    }
    void addToUntrustedFunc(string funcname)
    {
        untrustedFunctions.insert(funcname);
    }
};

class SecureFunctionTypeTraversalTest : public AstSimpleProcessing
{
    public:
    void visit(SgNode*);
};


void printTaintAnalysisStates(TaintAnalysis* ta, string indent="");
                    
#endif
