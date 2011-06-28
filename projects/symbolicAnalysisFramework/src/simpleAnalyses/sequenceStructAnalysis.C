#include "sequenceStructAnalysis.h"

int sequenceStructAnalysisDebugLevel=0;

// ############################
// ##### SeqStructLattice #####
// ############################

// Initializes the lattice to level=uninitialized
SeqStructLattice::SeqStructLattice(ConstrGraph* cg, const DataflowNode& n, string indent) : cg(cg), n(n)
{
	ROSE_ASSERT(cg);
	init(indent);
	s=0;
	level = uninitialized;
}

// Initializes the lattice to level=startKnown and this->vInit=this->vFin=vInit
SeqStructLattice::SeqStructLattice(ConstrGraph* cg, const DataflowNode& n, const varID& vInit, int startOffset, string indent) : cg(cg), n(n)
{
	ROSE_ASSERT(cg);
	init(indent);
	setToStartKnown(vInit, startOffset);
}
// Initializes the lattice to level=startKnown and this->vInit=this->vFin=init
SeqStructLattice::SeqStructLattice(ConstrGraph* cg, const DataflowNode& n, int initVal, string indent) : cg(cg), n(n)
{
	ROSE_ASSERT(cg);
	init(indent);
	setToStartKnown(initVal);
}

// Initializes the lattice to level=seqKnown and this->vInit=initV this->vFin=finV, this->s=s
SeqStructLattice::SeqStructLattice(ConstrGraph* cg, const DataflowNode& n, const varID& initV, const varID& finV, int s, string indent) : cg(cg), n(n)
{
	ROSE_ASSERT(cg);
	init(indent);
	cg->assign(vInit, initV, 1, 1, 0);
	cg->assign(vFin,  finV,  1, 1, 0);
	this->s = s;
	level = seqKnown;
}
// Initializes the lattice to level=seqKnown and this->vInit=initV this->vFin=finV, this->s=s
SeqStructLattice::SeqStructLattice(ConstrGraph* cg, const DataflowNode& n, int initV, int finV, int s, string indent) : cg(cg), n(n)
{
	ROSE_ASSERT(cg);
	init(indent);
	cg->assign(vInit, zeroVar, 1, 1, initV);
	cg->assign(vFin,  zeroVar, 1, 1, finV);
	this->s = s;
	level = seqKnown;
}

// Common initialization code for SeqStructLattice
void SeqStructLattice::init(string indent)
{
	ostringstream vInitName;
	vInitName << "vInit_SeqStructLattice_" << this;
	vInit.init(vInitName.str());
	
	ostringstream vFinName;
	vFinName << "vFin_SeqStructLattice_" << this;
	vFin.init(vFinName.str());
}

// Copy constructor
SeqStructLattice::SeqStructLattice(const SeqStructLattice& that, string indent) : n(that.n)
{
	copyFrom(that, indent);
}

// Copies the state from That sequence to This. Returns true if this causes This to change and false otherwise.
bool SeqStructLattice::copyFrom(const SeqStructLattice& that, string indent)
{
	init(indent);
	bool modified = false;
	
	modified = (level != that.level) || modified;
	level = that.level;
	
	modified = (cg != that.cg) || modified;
	cg = that.cg;
	
	if(level == startKnown || level == seqKnown) {
		modified = cg->assign(vInit, that.vInit, 1, 1, 0) || modified;
		modified = cg->assign(vFin, that.vFin, 1, 1, 0)   || modified;
		
		modified = (s != that.s) || modified;
		s = that.s;
	}
	
	return modified;
}

// Initializes this Lattice to its default state, if it is not already initialized
void SeqStructLattice::initialize()
{
	if(level == uninitialized)
		level = bottom;
}

// Returns a copy of this lattice
Lattice* SeqStructLattice::copy() const
{
	return new SeqStructLattice(*this);
}

// Overwrites the state of this Lattice with that of that Lattice
void SeqStructLattice::copy(Lattice* that)
{
	copyFrom(*dynamic_cast<SeqStructLattice*>(that), "");
}

// Called by analyses to create a copy of this lattice. However, if this lattice maintains any 
//    information on a per-variable basis, these per-variable mappings must be converted from 
//    the current set of variables to another set. This may be needed during function calls, 
//    when dataflow information from the caller/callee needs to be transferred to the callee/calleer.
// We do not force child classes to define their own versions of this function since not all
//    Lattices have per-variable information.
// varNameMap - maps all variable names that have changed, in each mapping pair, pair->first is the 
//              old variable and pair->second is the new variable
// func - the function that the copy Lattice will now be associated with
void SeqStructLattice::remapVars(const map<varID, varID>& varNameMap, const Function& newFunc)
{
	// Do nothing here since each SeqStructLattice will always be used in conjuction with
	// a ConstrGraph and this call on the ConstrGraph will perform the appropriate replacement
	// since the only meaningful relationships that vInit and vFin have are through the ConstrGraph.
}

// Called by analyses to copy over from the that Lattice dataflow information into this Lattice.
// that contains data for a set of variables and incorporateVars must overwrite the state of just
// those variables, while leaving its state for other variables alone.
// We do not force child classes to define their own versions of this function since not all
//    Lattices have per-variable information.
void SeqStructLattice::incorporateVars(Lattice* that_arg)
{
	SeqStructLattice* that = dynamic_cast<SeqStructLattice*>(that_arg);
	ROSE_ASSERT(cg == that->cg);
	cg->assign(vInit, that->vInit, 1, 1, 0);
	cg->assign(vFin,  that->vFin , 1, 1, 0);
	s = that->s;
}

// Returns a Lattice that describes the information known within this lattice
// about the given expression. By default this could be the entire lattice or any portion of it.
// For example, a lattice that maintains lattices for different known variables and expression will 
// return a lattice for the given expression. Similarly, a lattice that keeps track of constraints
// on values of variables and expressions will return the portion of the lattice that relates to
// the given expression. 
// It it legal for this function to return NULL if no information is available.
// The function's caller is responsible for deallocating the returned object
Lattice* SeqStructLattice::project(SgExpression* expr)
{
	// This lattice only refers to virtual variables
	return new SeqStructLattice(*this);
}

// The inverse of project(). The call is provided with an expression and a Lattice that describes
// the dataflow state that relates to expression. This Lattice must be of the same type as the lattice
// returned by project(). unProject() must incorporate this dataflow state into the overall state it holds.
// Call must make an internal copy of the passed-in lattice and the caller is responsible for deallocating it.
// Returns true if this causes this to change and false otherwise.
bool SeqStructLattice::unProject(SgExpression* expr, Lattice* exprState)
{
	bool modified = false;
	SeqStructLattice* that = dynamic_cast<SeqStructLattice*>(exprState);
	ROSE_ASSERT(cg == that->cg);
	modified = cg->assign(vInit, that->vInit, 1, 1, 0) || modified;
	modified = cg->assign(vFin,  that->vFin , 1, 1, 0) || modified;
	modified = (s!=that->s) || modified;
	s = that->s;
	return modified;
}

// computes the meet of this and that and saves the result in this
// returns true if this causes this to change and false otherwise
bool SeqStructLattice::meetUpdate(Lattice* that_arg)
{
	string indent="";
	SeqStructLattice* that = dynamic_cast<SeqStructLattice*>(that_arg);
	bool modified = false;
	
	// That is assumed to be a predecessor of This
	
	if(level <= bottom) {
		if(that->level > level)
			modified = copyFrom(*that) || modified;
	} else if(that->level <= bottom) {
		
	} else if(that->level == top) {
		if(level < that->level) {
			modified = setToTop() || modified;
		}
	} else {
		// If the application always executes the predecessor before this current node and is not part of a loop
		if(dominates(that->n, this->n) && !dominates(this->n, that->n)) {
			if(*this != that)
				modified = setToTop() || modified;
		// If the application must execute the current node before the predecessor in a loop
		} else if(dominates(this->n, that->n)) {
			int a, b, c;
			// If the current state is a constant and the state carried by the predecessor from around the loop is a different constant
			if(level==startKnown && that->level==startKnown && 
			      cg->eqVars(this->vInit, that->vInit, indent+"    ") && 
			      cg->eqVars(this->vFin,  that->vFin,  a, b, c, indent+"    ") && a==1 && b==1) { 
				modified = cg->assign(this->vFin, that->vFin, 1, 1, 0) || modified;
				modified = (s!=c) || modified;
				s = c;
			} else if(level==startKnown && that->level==seqKnown && 
			      cg->eqVars(this->vInit, that->vInit, indent+"    ") && 
			      cg->eqVars(this->vFin,  that->vFin,  a, b, c, indent+"    ") && a==1 && b==1) { 
				modified = cg->assign(this->vFin, that->vFin, 1, 1, 0) || modified;
				modified = (s!=that->s) || modified;
				s = that->s;
			} else
				modified = setToTop() || modified;
		}
	}
	
	return modified;
}

// Set the level of this object to Bottom.
// Return true if this causes ths Lattice to change and false otherwise.
bool SeqStructLattice::setToBottom()
{
	bool modified = false;
	
	modified = (level != bottom) || modified;
	level = bottom;
	
	modified = cg->removeVar(vInit) || modified;
	modified = cg->removeVar(vFin)  || modified;
	
	modified = (s != 0) || modified;
	s = 0;
	
	return modified;
}

// Set the level of this object to Top.
// Return true if this causes ths Lattice to change and false otherwise.
bool SeqStructLattice::setToTop()
{
	bool modified = false;
	
	modified = (level != top);
	level = top;
	
	modified = cg->removeVar(vInit) || modified;
	modified = cg->removeVar(vFin)  || modified;
	
	modified = (s != 0) || modified;
	s = 0;
	
	return modified;	
}

// Set the level of this object to startKnown, with the given variable starting point (vInit + startOffset).
// Return true if this causes ths Lattice to change and false otherwise.
bool SeqStructLattice::setToStartKnown(varID vInit, int initOffset)
{
	bool modified = false;

	modified = cg->assign(this->vInit, vInit, 1, 1, initOffset) || modified;
	modified = cg->assign(this->vFin,  this->vInit, 1, 1, 0)     || modified;
	
	modified = (s!=0 || modified);
	s=0;
	
	modified = (level!=startKnown || modified);
	level = startKnown;
	
	return modified;
}

// Set the level of this object to startKnown, with the given constant starting point.
// Return true if this causes ths Lattice to change and false otherwise.
bool SeqStructLattice::setToStartKnown(int initVal)
{
	bool modified = false;
	
	modified = cg->assign(this->vInit, zeroVar, 1, 1, initVal) || modified;
	modified = cg->assign(this->vFin, this->vInit, 1, 1, 0)    || modified;
	
	modified = (s!=0 || modified);
	s=0;
	
	modified = (level!=startKnown || modified);
	level = startKnown;
	
	return modified;
}

// Set the level of this object to seqKnown, with the given final point and stride
// Return true if this causes ths Lattice to change and false otherwise.
bool SeqStructLattice::setToSeqKnown(varID vFin, int finOffset, int stride)
{
	bool modified = false;

	modified = cg->assign(this->vFin, vFin, 1, 1, finOffset) || modified;
	
	modified = (s!=stride || modified);
	s=stride;
	
	modified = (level!=seqKnown || modified);
	level = seqKnown;
	
	return modified;
}
	

// Return this lattice's level
SeqStructLattice::seqLevel SeqStructLattice::getLevel() const
{
	return level;
}

bool SeqStructLattice::operator==(Lattice* that_arg)
{
	SeqStructLattice* that = dynamic_cast<SeqStructLattice*>(that_arg);
	return (level == that->level) && 
	       (cg    == that->cg) && 
	       (dom   == that->dom) && 
	       (n     == that->n) &&
	       cg->eqVars(this->vInit, that->vInit, "    ") && 
	       cg->eqVars(this->vFin,  that->vFin,  "    ") && 
	       (s     == that->s);
}
		
// The string that represents this object
// If indent!="", every line of this string must be prefixed by indent
// The last character of the returned string should not be '\n', even if it is a multi-line string.
string SeqStructLattice::str(string indent)
{
	ostringstream oss;
	
	if(level == uninitialized)   oss << "SeqStructLattice: uninitialized>";
	else if(level == bottom)     oss << "SeqStructLattice: bottom>";
	else if(level == startKnown) oss << "SeqStructLattice: startKnown: vInit="<<vInit<<", vFin="<<vFin<<">";
	else if(level == seqKnown)   oss << "SeqStructLattice: seqKnown: vInit="<<vInit<<", vFin="<<vFin<<", s="<<s<<">";
	else if(level == top)        oss << "SeqStructLattice: top>";

	return oss.str();
}

/***********************
 ***** DivAnalysis *****
 ***********************/

// Generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
//vector<Lattice*> DivAnalysis::genInitState(const Function& func, const DataflowNode& n, const NodeState& state)
void SeqStructAnalysis::genInitState(const Function& func, const DataflowNode& n, const NodeState& state,
                                     vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts)
{
	// Get the initial state from the ConstrGraphAnalysis, which will create a constraint graph in initLattices and nothing in initFacts
	cgAnalysis->genInitState(func, n, state, initLattices, initFacts);
	ROSE_ASSERT(initLattices.size()==0 && dynamic_cast<ConstrGraph*>(initLattices[0]) && initFacts.size()==0);
	ConstrGraph* cg = dynamic_cast<ConstrGraph*>(initLattices[0]);
	
	// Create a SeqStructLattice for every live variable in the application
	map<varID, Lattice*> emptyM;
	FiniteVarsExprsProductLattice* l = new FiniteVarsExprsProductLattice((Lattice*)new SeqStructLattice(cg, n), emptyM/*genConstVarLattices()*/, 
	                                                                     (Lattice*)NULL, ldva, /*func, */n, state);
	
	// Get all the array reference expressions in the application and add a SeqStructLattice for each one
	ROSE_ASSERT(func.get_definition());
	Rose_STL_Container<SgNode*> arrayRefs = NodeQuery::querySubTree(func.get_definition()->get_body(), V_SgPntrArrRefExp);
	for(Rose_STL_Container<SgNode*>::iterator aref=arrayRefs.begin(); aref!=arrayRefs.end(); aref++) {
		ROSE_ASSERT(isSgExpression(*aref));
		l->addVar(SgExpr2Var(isSgExpression(*aref)));
	}
	
	//printf("DivAnalysis::genInitState, returning %p\n", l);
	initLattices.push_back(l);
	
	/*printf("SeqStructAnalysis::genInitState() initLattices:\n");
	for(vector<Lattice*>::iterator it = initLattices.begin(); 
	    it!=initLattices.end(); it++)
	{	
		cout << *it << ": " << (*it)->str("    ") << "\n";
	}*/
}

bool SeqStructAnalysis::transfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo)
{
	bool modified=false;
	
	ConstrGraph* cg = dynamic_cast<ConstrGraph*>(state.getLatticeAbove(cgAnalysis, 0));
	
	ROSE_ASSERT(dfInfo.size() == 1);
	FiniteVarsExprsProductLattice* prodLat = dynamic_cast<FiniteVarsExprsProductLattice*>(*(dfInfo.begin()));
	
	// Make sure that all the lattices are initialized
	const vector<Lattice*>& lattices = prodLat->getLattices();
	for(vector<Lattice*>::const_iterator it = lattices.begin(); it!=lattices.end(); it++)
		(dynamic_cast<DivLattice*>(*it))->initialize();
	
	
	if(isSgPntrArrRefExp(n.getNode()) || isSgAssignOp(n.getNode())) {
		// Variable that corresponds to the value referenced by the array index or the new value of the assigned variable
		varID refVar;
		// The SeqStructLattice associated with refVar
		SeqStructLattice* ssL;
		
		if(isSgPntrArrRefExp(n.getNode())) {
			ssL = dynamic_cast<SeqStructLattice*>(prodLat->getVarLattice(SgExpr2Var(isSgPntrArrRefExp(n.getNode()))));
			ROSE_ASSERT(ssL);
			refVar = SgExpr2Var(isSgPntrArrRefExp(n.getNode())->get_rhs_operand());
		} else if(isSgAssignOp(n.getNode())) {
			ssL = dynamic_cast<SeqStructLattice*>(prodLat->getVarLattice(SgExpr2Var(isSgAssignOp(n.getNode())->get_lhs_operand())));
			// If the lhs of this assignment is live
			if(ssL) refVar = SgExpr2Var(isSgPntrArrRefExp(n.getNode())->get_rhs_operand());
			// Otherwise, we're done
			else    goto DONE;
		}
		
		if(ssL->getLevel() == SeqStructLattice::bottom) {
			modified = ssL->setToStartKnown(refVar, 0) || modified;
		} else if(ssL->getLevel() == SeqStructLattice::top) {
			// Do nothing, we can't represent this sequence
		} else if(ssL->getLevel() == SeqStructLattice::startKnown) {
			// If the last-known access was a fixed stride away from this access
			int a, b, c;
			if(cg->isEqVars(refVar, ssL->vInit, a, b, c) && a==1 && b==1) {
				modified = ssL->setToSeqKnown(refVar, 0, c) || modified;
			// Otherwise, this sequence is too complex to be represented
			} else
				modified = ssL->setToTop() || modified;
		// If we've identified a sequence after analyzing multiple instances of this array access
		} else if(ssL->getLevel() == SeqStructLattice::seqKnown) {
			// If the stride of this access is equal to what was previously computed
			int a, b, c;
			if(cg->isEqVars(refVar, ssL->vInit, a, b, c) && a==1 && b==1 && c==ssL->s) {
				modified = ssL->setToSeqKnown(refVar, 0, ssL->s) || modified;
			// If the stride is different from what we've identified, this sequence is too complex to be represented
			} else 
				modified = ssL->setToTop() || modified;
		}
	}
	
	DONE:
	
	// Finally, call the transfer function of the ConstrGraph to make sure that the current statement's effects are incorporated
	modified = cgAnalysis->transfer(func, n, state, dfInfo) || modified;
	
	return modified;
}

// Prints the Lattices set by the given SeqStructAnalysis 
void printSeqStructAnalysisStates(SeqStructAnalysis* ssa, string indent)
{
	vector<int> factNames;
	vector<int> latticeNames;
	latticeNames.push_back(0);
	latticeNames.push_back(1);
	printAnalysisStates pas(ssa, factNames, latticeNames, printAnalysisStates::below, indent);
	UnstructuredPassInterAnalysis upia_pas(pas);
	upia_pas.runAnalysis();
}
