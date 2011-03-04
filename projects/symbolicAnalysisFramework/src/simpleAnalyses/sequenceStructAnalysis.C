#include "sequenceStructAnalysis.h"

int seqStructAnalysisDebugLevel=0;

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
SeqStructLattice::SeqStructLattice(ConstrGraph* cg, const DataflowNode& n, const varID& vInit, string indent) : cg(cg), n(n)
{
	ROSE_ASSERT(cg);
	init(indent);
	cg->assign(this->vInit, vInit, 1, 1, 0);
	cg->assign(this->vFin,  this->vInit, 1, 1, init);
	s=0;
	level = startKnown;
}
// Initializes the lattice to level=startKnown and this->vInit=this->vFin=init
SeqStructLattice::SeqStructLattice(ConstrGraph* cg, const DataflowNode& n, int init, string indent) : cg(cg), n(n)
{
	ROSE_ASSERT(cg);
	init(indent);
	cg->assign(vInit, zeroVar, 1, 1, init);
	cg->assign(this->vFin,  this->vInit, 1, 1, init);
	s=0;
	level = startKnown;
}

// Initializes the lattice to level=seqKnown and this->vInit=vInit this->vFin=vInit, this->s=s
SeqStructLattice::SeqStructLattice(ConstrGraph* cg, const DataflowNode& n, const varID& init, const varID& fin, int s, string indent) : cg(cg), n(n)
{
	ROSE_ASSERT(cg);
	init(indent);
	cg->assign(vInit, init, 1, 1, 0);
	cg->assign(vFin,  fin,  1, 1, 0);
	this->s = s;
	level = seqKnown;
}
// Initializes the lattice to level=seqKnown and this->vInit=init this->vFin=fin, this->s=s
SeqStructLattice::SeqStructLattice(ConstrGraph* cg, const DataflowNode& n, int init, int fin, int s, string indent) : cg(cg), n(n)
{
	ROSE_ASSERT(cg);
	init(indent);
	cg->assign(vInit, zeroVar, 1, 1, init);
	cg->assign(vFin,  zeroVar, 1, 1, fin);
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
SeqStructLattice::SeqStructLattice(SeqStructLattice& that, string indent)
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
	cg->assign(vInit, that->vInit);
	cg->assign(vFin,  that->vFin);
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
	modified = cg->assign(vInit, that->vInit) || modified;
	modified = cg->assign(vFin,  that->vFin) || modified;
	modified = (s!=that->s) || modified;
	s = that->s;
	return modfied;
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
			modified = copyFrom(that) || modified;
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
}

// Set the level of this object to Bottom
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

// Set the level of this object to Top
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

bool SeqStructLattice::operator==(Lattice* that);
		
// The string that represents this object
// If indent!="", every line of this string must be prefixed by indent
// The last character of the returned string should not be '\n', even if it is a multi-line string.
string SeqStructLattice::str(string indent);