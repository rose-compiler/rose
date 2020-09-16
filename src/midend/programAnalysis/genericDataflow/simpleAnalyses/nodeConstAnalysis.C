#include "nodeConstAnalysis.h"
#include <algorithm>

int nodeConstAnalysisDebugLevel=0;

/**********************
 ***** nodeConstLattice *****
 **********************/

// The different levels of this lattice
// this object is uninitialized
const int nodeConstLattice::uninitialized; 
// no information is known about the value of the variable
const int nodeConstLattice::bottom; 
// the value of the variable is known
const int nodeConstLattice::valKnown; 
// this variable holds more values than can be represented using a single value and divisibility
const int nodeConstLattice::top; 

const int nodeConstLattice::noAssign;
const int nodeConstLattice::constVal;
const int nodeConstLattice::multVal;

// returns a copy of this lattice
Lattice* nodeConstLattice::copy() const
{
        return new nodeConstLattice(*this);
}

// overwrites the state of this Lattice with that of that Lattice
void nodeConstLattice::copy(Lattice* that_arg)
{
        nodeConstLattice* that = dynamic_cast<nodeConstLattice*>(that_arg);
        
        this->valID = that->valID;
        this->level = that->level;
}

// computes the meet of this and that and saves the result in this
// returns true if this causes this to change and false otherwise
bool nodeConstLattice::meetUpdate(Lattice* that_arg)
{
        unsigned long  oldValID = valID;
        short oldLevel = level;
        nodeConstLattice* that = dynamic_cast<nodeConstLattice*>(that_arg);

/*printf("nodeConstLattice::meetUpdate\n");
cout << "this: " << str("") << "\n";
cout << "that: " << that->str("") << "\n";*/

        // if this object is uninitialized, just copy the state of that
        if(level==uninitialized)
        {
                if(that->level > uninitialized)
                        copy(that);
        }
        // else, if that is uninitialized, leave this alone
        else if(that->level==uninitialized)
        {
        }
        // if both are initialized, perform the meet
        else
        {
                // if this object is bottom and that is higher than bottom,
                // make this object top since we're at a meet of a set value and an unset value
                if(level==bottom)
                {
                        if(that->level>bottom)
                                //copy(that);
                                setToTop();
                }
                // else, if that is bottom
                else if(that->level==bottom)
                {
                        // if this is higher than bottom, make this object top since we're at a 
                        // meet of a set value and an unset value
                        if(level > bottom)
                                setToTop();
                }
                // else, if both are above bottom, perform the meet
                else
                {
                        // if the two objects have known values 
                        if(level==valKnown && that->level==valKnown)
                        {
                                // if they disagree on their values, move the state 
                                // of this object to top
                                if(valID != that->valID)
                                {
                                        setToTop();
                                }
                                // else, if the two objects agree on their values, we can 
                                // leave this object alone
                        }
                        // else, if either object is top, set this object to top
                        else
                        {
                                if(level!=top)
                                        setToTop();
                        }
                }
        }

        // return whether this object was modified
        return (oldValID != valID) ||
               (oldLevel != level);
}

// Computes the maximum of this node and that, which is just like meet
// except that different values get max-ed, rather than push the result to top
// returns true if this causes this to change and false otherwise
bool nodeConstLattice::maxUpdate(nodeConstLattice& that)
{
        unsigned long  oldValID = valID;
        short oldLevel = level;

/*printf("nodeConstLattice::maxUpdate\n");
cout << "this: " << str("") << "\n";
cout << "that: " << that.str("") << "\n";*/

        // if this object is uninitialized, just copy the state of that
        if(level==uninitialized)
        {
                if(that.level > uninitialized)
                        copy(&that);
        }
        // else, if that is uninitialized, leave this alone
        else if(that.level==uninitialized)
        {
        }
        // if both are initialized, perform the meet
        else
        {
                // if this object is bottom, just copy the state of that
                // (since we know that both objects are initialized)
                if(level==bottom)
                {
                        if(that.level>bottom)
                                copy(&that);
                }
                // else, if that is bottom, leave this alone since this is at least bottom
                else if(that.level==bottom)
                {
                }
                // else, if both are above bottom, perform the meet
                else
                {
                        // if the two objects have known values 
                        if(level==valKnown && that.level==valKnown)
                        {
                                // update valID to the maximum of the two valIDs
                                valID = std::max(valID, that.valID);
                        }
                        // else, if either object is top, set this object to top
                        else
                        {
                                if(level!=top)
                                        setToTop();
                        }
                }
        }

        // return whether this object was modified
        return (oldValID != valID) ||
               (oldLevel != level);
}

// If this lattice is at level valKnown, increments the value by the given amount
// returns true if this causes this to change and false otherwise
bool nodeConstLattice::increment(int val)
{
        if(level == valKnown)
        {
                valID += val;
                return val>0;
        }
        return false;
}

bool nodeConstLattice::operator==(Lattice* that_arg)
{
        nodeConstLattice* that = dynamic_cast<nodeConstLattice*>(that_arg);
        
        return (valID == that->valID) &&
               (level == that->level);
}

short nodeConstLattice::getValConst() const
{ return level; }
        

// Sets the state of this lattice to bottom
// returns true if this causes the lattice's state to change, false otherwise
bool nodeConstLattice::setToBottom()
{
        bool modified = this->level != bottom;
        this->valID = 0;
        level = bottom;
        return modified;
}

// Sets the state of this lattice to the given value.
// returns true if this causes the lattice's state to change, false otherwise
bool nodeConstLattice::set(unsigned long valID)
{
        bool modified = this->level != valKnown || this->valID != valID;
        this->valID = valID;
        level = valKnown;
        return modified;
}

// Sets the state of this lattice to top
// returns true if this causes the lattice's state to change, false otherwise
bool nodeConstLattice::setToTop()
{
        bool modified = this->level != top;
        this->valID = 0;
        level = top;
        return modified;        
}
        
string nodeConstLattice::str(string indent)
{
        ostringstream outs;
        //printf("nodeConstLattice::str() level=%d\n", level);
        if(level == uninitialized)
                outs << indent << "<level: uninitialized>";
        else if(level == bottom)
                outs << indent << "<level: bottom>";
        else if(level == valKnown)
                outs << indent << "<level: valKnown, valID="<<valID<<">";
        else if(level == top)
                outs << indent << "<level: top>";
        return outs.str();
}

/***********************
 ***** nodeConstAnalysis *****
 ***********************/


// generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
//vector<Lattice*> nodeConstAnalysis::genInitState(const Function& func, const DataflowNode& n, const NodeState& state)
void nodeConstAnalysis::genInitState(const Function& func, const DataflowNode& n, const NodeState& state,
                        vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts)
{
        //vector<Lattice*> initLattices;
        map<varID, Lattice*> constVarLattices;
        FiniteVariablesProductLattice* l = new FiniteVariablesProductLattice(true, false, new nodeConstLattice(), constVarLattices, NULL, func, n, state);
        //printf("nodeConstAnalysis::genInitState, returning %p\n", l);
        initLattices.push_back(l);
        
/*printf("nodeConstAnalysis::genInitState() initLattices:\n");
for(vector<Lattice*>::iterator it = initLattices.begin(); 
    it!=initLattices.end(); it++)
{       
        cout << *it << ": " << (*it)->str("    ") << "\n";
}*/
        
        //return initLattices;
}

bool nodeConstAnalysis::transfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo)
{
        bool modified=false;
        
        FiniteVariablesProductLattice* prodLat = dynamic_cast<FiniteVariablesProductLattice*>(*(dfInfo.begin()));
        
        printf("nodeConstAnalysis::transfer\n");
        
        // make sure that all the non-constant Lattices are initialized
        const vector<Lattice*>& lattices = prodLat->getLattices();
        for(vector<Lattice*>::const_iterator it = lattices.begin(); it!=lattices.end(); it++)
                (dynamic_cast<nodeConstLattice*>(*it))->initialize();
        
        SgNode* asgnLHS;
        /*printf("n.getNode() = <%s|%s>\n", n.getNode()->unparseToString().c_str(), n.getNode()->class_name().c_str());
        printf("isSgBinaryOp(n.getNode())=%p\n", isSgBinaryOp(n.getNode()));*/
        
        // If this is an assignment operation, grab the left-hand side
        if((asgnLHS = cfgUtils::getAssignmentLHS(n.getNode())) != NULL)
        {
                // If the expression being assigned is a valid variable expression
                if(varID::isValidVarExp(asgnLHS))
                {
                        varID lhsVar(asgnLHS);
                        if(nodeConstAnalysisDebugLevel>=1)
                                printf("Assignment of variable %s\n", lhsVar.str().c_str());
                        
                        // ---------------------------------------------------------------------------------
                        // Update the left-hand side variable with the max of the lattices of the right-hand 
                        // side variables or set it to 1 if the right hand side has no variable reads (i.e. its a constant)
// WARNING: The RHS MAY CONTAIN FUNCTION CALLS, WHICH WE'RE NOT HANDLING RIGHT NOW
                        set<SgNode*> rhs;
                        cfgUtils::getAssignmentRHS(n.getNode(), rhs);
                        varIDSet rhsReads;
                        //printf("rhs=\n");
                        // Iterate through the SgNodes in rhs and collect all their variable read references
                        for(set<SgNode*>::iterator it=rhs.begin(); it!=rhs.end(); it++)
                        {
                                varIDSet rhsReads_it = getReadVarRefsInSubtree(*it);
                                // copy rhsReads_it into rhsReads
                                for(varIDSet::iterator it2=rhsReads_it.begin(); it2!=rhsReads_it.end(); it2++)
                                        rhsReads.insert(*it2);
                                
                                varIDSet rhsWrites_it = getWriteVarRefsInSubtree(*it);
                                //printf("    <%s|%s> rhsReads_it.size()=%d rhsWrites_it.size()=%d\n", (*it)->unparseToString().c_str(), (*it)->class_name().c_str(), rhsReads_it.size(), rhsWrites_it.size());
                        }
                        nodeConstLattice maxLat;
                        //printf("    maxLat=%s, rhsReads.size()=%d\n", maxLat.str().c_str(), rhsReads.size());
                        if(rhsReads.size()>0)
                        {
                                maxLat.initialize();
                                for(varIDSet::iterator it=rhsReads.begin(); it!=rhsReads.end(); it++)
                                {
                                        nodeConstLattice* rhsVarLat = dynamic_cast<nodeConstLattice*>(prodLat->getVarLattice(func, *it));
                                        ROSE_ASSERT(rhsVarLat != NULL);
                                        printf("    RHS variable %s lattice: %s\n", (*it).str().c_str(), rhsVarLat->str().c_str());
                                        maxLat.maxUpdate(*rhsVarLat);
                                }
                                maxLat.increment(1);
                        }
                        else
                                maxLat.set(/*uids.getUID()*/ (unsigned long)n.getNode());
                        
                        nodeConstLattice* lhsVarLat = dynamic_cast<nodeConstLattice*>(prodLat->getVarLattice(func, lhsVar));
                        ROSE_ASSERT(lhsVarLat != NULL);
                      //lhsVarLat->set(uids.getUID());
                        *lhsVarLat = maxLat;
                        //lhsVarLat->meetUpdate(&maxLat);
                        printf("    Final RHS lattice: %s\n", maxLat.str().c_str());
                        printf("    Final LHS variable %s, lattice: %s\n", lhsVar.str().c_str(), lhsVarLat->str().c_str());
                }
        }
        // If this is a variable declaration
        else if(isSgInitializedName(n.getNode()))
        {
                SgInitializedName* initName = isSgInitializedName(n.getNode());
                varID var(initName);
                
                nodeConstLattice* ncLat = dynamic_cast<nodeConstLattice*>(prodLat->getVarLattice(func, var));
                
                // if this is a scalar that we care about
                if(ncLat)
                {
                        ncLat->set(/*uids.getUID()*/ (unsigned long)n.getNode());
                        if(nodeConstAnalysisDebugLevel>=1)
                                printf("Declaration of variable %s\n", var.str().c_str());
                        modified = ncLat->setToBottom() || modified;
                }
        }
        // If this is a function call to a function that doesn't have a body, assume that all the variables
        // passed by reference were assigned
        else if(isSgFunctionCallExp(n.getNode()))
        {
                Function callee(isSgFunctionCallExp(n.getNode()));
                if(!callee.get_definition())
                {
                        SgInitializedNamePtrList params = callee.get_params();
                        const SgExpressionPtrList& args = isSgFunctionCallExp(n.getNode())->get_args()->get_expressions();
                        ROSE_ASSERT(params.size() == args.size());
                        
                        SgInitializedNamePtrList::const_iterator itParams;
                        SgExpressionPtrList::const_iterator itArgs;
                        for(itParams=params.begin(), itArgs=args.begin(); 
                            itParams!=params.end() && itArgs!=args.end(); 
                            itParams++, itArgs++)
                        {
                                SgInitializedName* param = *itParams;
                                SgExpression*      arg   = *itArgs;
// !!!! THIS IS A VERY COARSE WAY TO DETECT PASSING BY REFERENCE
                                // If the current variable is being passed by reference through a pointer
                                if(isSgPointerType(param->get_type()) && isSgAddressOfOp(arg) && 
                                   varID::isValidVarExp(isSgAddressOfOp(arg)->get_operand()))
                                {
                                        varID var(isSgAddressOfOp(arg)->get_operand());
                                        nodeConstLattice* ncLat = dynamic_cast<nodeConstLattice*>(prodLat->getVarLattice(func, var));
                                        ROSE_ASSERT(ncLat != NULL);
                                        ncLat->set(/*uids.getUID()*/ (unsigned long)n.getNode());
                                }
                                // If the current variable is being passed by reference throgh a reference-typed argument
                                else if(isSgReferenceType(param->get_type()) && varID::isValidVarExp(arg))
                                {
                                        //printf("arg = <%s|%s>\n", arg->unparseToString().c_str(), arg->class_name().c_str());
                                        varID var(arg);
                                        //printf("var = %s, func=%s\n", var.str().c_str(), func.get_name().str());
                                        nodeConstLattice* ncLat = dynamic_cast<nodeConstLattice*>(prodLat->getVarLattice(func, var));
                                        ROSE_ASSERT(ncLat != NULL);
                                        ncLat->set(/*uids.getUID()*/ (unsigned long)n.getNode());
                                }
                        }
                }
        }
        
        return modified;
}

// runs the nodeConstAnalysis on the project and returns the resulting nodeConstAnalysis object
nodeConstAnalysis* runNodeConstAnalysis()
{
        nodeConstAnalysis* nca = new nodeConstAnalysis();
        ContextInsensitiveInterProceduralDataflow ciipd_nca(nca, getCallGraph());
        ciipd_nca.runAnalysis();
        /*UnstructuredPassInterDataflow upipd_nca(nca);
        upipd.runAnalysis();*/
        return nca;
}

// prints the Lattices set by the given nodeConstAnalysis 
void printNodeConstAnalysisStates(nodeConstAnalysis* nca, string indent)
{
        vector<int> factNames;
        vector<int> latticeNames;
        latticeNames.push_back(0);
        printAnalysisStates pas(nca, factNames, latticeNames, printAnalysisStates::below, indent);
        UnstructuredPassInterAnalysis upia_pas(pas);
        upia_pas.runAnalysis();
}
