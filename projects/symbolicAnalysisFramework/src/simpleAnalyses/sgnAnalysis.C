#include "sgnAnalysis.h"

int sgnAnalysisDebugLevel=0;

/**********************
 ***** SgnLattice *****
 **********************/

// returns a copy of this lattice
Lattice* SgnLattice::copy() const
{
	return new SgnLattice(*this);
}

// overwrites the state of this Lattice with that of that Lattice
void SgnLattice::copy(Lattice* that_arg)
{
	SgnLattice* that = dynamic_cast<SgnLattice*>(that_arg);
	
	this->sgnState = that->sgnState;
	this->level = that->level;
}

// overwrites the state of this Lattice with that of that Lattice
// returns true if this causes this lattice to change and false otherwise
bool SgnLattice::copyMod(Lattice* that_arg)
{
	SgnLattice* that = dynamic_cast<SgnLattice*>(that_arg);
	bool modified = (this->sgnState != that->sgnState) || (this->level != that->level);
	
	this->sgnState = that->sgnState;
	this->level = that->level;
	
	return modified;
}


// computes the meet of this and that and saves the result in this
// returns true if this causes this to change and false otherwise
bool SgnLattice::meetUpdate(Lattice* that_arg)
{
	long  oldSgnState = sgnState;
	short oldLevel = level;
	SgnLattice* that = dynamic_cast<SgnLattice*>(that_arg);

/*printf("SgnLattice::meetUpdate\n");
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
		// if this object is bottom, just copy the state of that
		// (since we know that both objects are initialized
		if(level==bottom)
		{
			if(that->level>bottom)
				copy(that);
		}
		// else, if that is bottom, leave this alone since this is at least bottom
		else if(that->level==bottom)
		{
		}
		// else, if both are above bottom, perform the meet
		else
		{
			// if either object is = top, the result is top
			if(that->level == top)
				setTop();
			else if(level!=top)
			{
				// we now know that both objects are either eqZero or sgnKnown
				
				// if the two objects are equal to 0
				if(level==eqZero && that->level==eqZero)
				{
					// we don't have anything to change, since level==eqZero
				}
				// if only one is equal to zero, the result is the other object
				else if(that->level!=eqZero)
					setSgnKnown(that->sgnState);
				else if(level!=eqZero)
				{
					// we don't have anything to change, since level==sgnKnown
				}
				else
				{
					// both objects should be at level sgnKnown
					ROSE_ASSERT(level == sgnKnown && that->level == sgnKnown);
					
					// if their signs do not agree, this object is raised to top
					if(sgnState != that->sgnState)
						setTop();
				}
			}
		}
	}

	// return whether this object was modified
	return (oldSgnState != sgnState) ||
	       (oldLevel != level);
}

bool SgnLattice::operator==(Lattice* that_arg)
{
	SgnLattice* that = dynamic_cast<SgnLattice*>(that_arg);
	
	return (sgnState == that->sgnState) &&
	       (level == that->level);
}

// returns the current state of this object
SgnLattice::sgnStates SgnLattice::getSgnState() const
{ return sgnState; }

SgnLattice::sgnLevels SgnLattice::getLevel() const
{ return level; }
	

// Sets the state of this lattice to bottom
// returns true if this causes the lattice's state to change, false otherwise
bool SgnLattice::setBot()
{
	bool modified = this->level != bottom;
	this->sgnState = unknown;
	level = bottom;
	return modified;
}

// Sets the state of this lattice to eqZero.
// returns true if this causes the lattice's state to change, false otherwise
bool SgnLattice::setEqZero()
{
	bool modified = this->level != eqZero;
	this->sgnState = unknown;
	level = eqZero;
	return modified;
}

// Sets the state of this lattice to sgnKnown, with the given sign.
// returns true if this causes the lattice's state to change, false otherwise
bool SgnLattice::setSgnKnown(SgnLattice::sgnStates sgnState)
{
	bool modified = this->level != sgnKnown || this->sgnState != sgnState;
	this->sgnState = sgnState;
	level = sgnKnown;
	return modified;
}

// Sets the state of this lattice to sgnKnown, with the sign of the given value.
// returns true if this causes the lattice's state to change, false otherwise
bool SgnLattice::set(int val)
{
	bool modified=false;
	if(val==0)
	{
		modified = this->level != eqZero;
		level = eqZero;
	}
	else if(val>0)
	{
		modified = this->level != sgnKnown || this->sgnState != posZero;
		sgnState = posZero;
		level = sgnKnown;
	}
	else if(val>0)
	{
		modified = this->level != sgnKnown || this->sgnState != negZero;
		sgnState = negZero;
		level = sgnKnown;
	}
	return modified;
}

// Sets the state of this lattice to top
// returns true if this causes the lattice's state to change, false otherwise
bool SgnLattice::setTop()
{
	bool modified = this->level != top;
	this->sgnState = unknown;
	level = top;
	return modified;	
}

// Increments the state of this object by value
// returns true if this causes the lattice's state to change, false otherwise
bool SgnLattice::plus(long increment)
{
	if(level == eqZero)
	{
		// if the increment is non-zero, the variable's value is !=0, so we simply record the variable's new sign
		if(increment != 0)
		{
			if(increment > 0) return setSgnKnown(posZero);
			else if(increment < 0) return setSgnKnown(negZero);
		}
	}
	else if(level == sgnKnown)
	{
		// if the increment moves in the opposite direction from the current sign, raise the sign info to top
		if((increment>0 && sgnState==negZero) ||
		   (increment<0 && sgnState==posZero))
			return setTop();
	}
	
	return false;
}

// Increments the state of this object by the contents of that
// returns true if this causes the lattice's state to change, false otherwise
bool SgnLattice::plus(const SgnLattice& that)
{
	if(that.level == top || that.level == bottom)
		setTop();
	else if(level == eqZero)
	{
		if(that.level == sgnKnown)
		{
			return setSgnKnown(that.sgnState);
		}
	}
	else if(level == sgnKnown)
	{
		// if the increment moves in the opposite direction from the current sign, raise the sign info to top
		if(that.level == sgnKnown && sgnState!=that.sgnState)
			return setTop();
	}
	return false;
}

// Multiplies the state of this object by value
// returns true if this causes the lattice's state to change, false otherwise
bool SgnLattice::mult(long multiplier)
{
	if(level == sgnKnown)
	{
		// if the multiplier is negative, flip the sign
		if(multiplier < 0)
			if(sgnState==posZero) return setSgnKnown(negZero);
			else if(sgnState==negZero) return setSgnKnown(posZero);
	}
	
	return false;
}

// Multiplies the state of this object by the contents of that
// returns true if this causes the lattice's state to change, false otherwise
bool SgnLattice::mult(const SgnLattice& that)
{
	if(that.level == top || that.level == bottom)
		setTop();
	else if(that.level == eqZero)
		return setEqZero();
	else if(level == sgnKnown)
	{
		// if the multiplier is negative, flip the sign
		if(that.level==sgnKnown && that.sgnState==negZero)
			if(sgnState==posZero) return setSgnKnown(negZero);
			else if(sgnState==negZero) return setSgnKnown(posZero);
	}
	return false;
}
	
string SgnLattice::str(string indent)
{
	ostringstream outs;
	//printf("SgnLattice::str() level=%d\n", level);
	if(level == uninitialized)
		outs << indent << "<SgnLattice: uninitialized>";
	else if(level == bottom)
		outs << indent << "<SgnLattice: bottom>";
	else if(level == eqZero)
		outs << indent << "<SgnLattice: eqZero>";
	else if(level == sgnKnown)
		outs << indent << "<SgnLattice: "<<(sgnState==unknown?"unknown":(sgnState==posZero?"posZero":(sgnState==negZero?"negZero":"???")))<<">";
	else if(level == top)
		outs << indent << "<SgnLattice: top>";
	return outs.str();
}

/***********************
 ***** SgnAnalysis *****
 ***********************/

map<varID, Lattice*> SgnAnalysis::constVars;
bool SgnAnalysis::constVars_init=false;

// generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
//vector<Lattice*> SgnAnalysis::genInitState(const Function& func, const DataflowNode& n, const NodeState& state)
void SgnAnalysis::genInitState(const Function& func, const DataflowNode& n, const NodeState& state,
                        vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts)
{
/*	printf("SgnAnalysis::genInitState() n=%p<%s | %s>\n", n.getNode(), n.getNode()->class_name().c_str(), n.getNode()->unparseToString().c_str());
	printf("SgnAnalysis::genInitState() state = %p\n", &state);*/
	
	//vector<Lattice*> initLattices;
	map<varID, Lattice*> constVars;
	FiniteVariablesProductLattice* l = new FiniteVariablesProductLattice(true, false, new SgnLattice(), constVars, NULL, func, n, state);
	initLattices.push_back(l);
	
	//return initLattices;
}

bool SgnAnalysis::transfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo)
{
	bool modified=false;
	
	FiniteVariablesProductLattice* prodLat = dynamic_cast<FiniteVariablesProductLattice*>(*(dfInfo.begin()));
	
//	printf("SgnAnalysis::transfer\n");
	
	// make sure that all the non-constant Lattices are initialized
	const vector<Lattice*>& lattices = prodLat->getLattices();
	for(vector<Lattice*>::const_iterator it = lattices.begin(); it!=lattices.end(); it++)
	{
		// !!! Now initialize() is a noop on initialized lattices
		if((dynamic_cast<SgnLattice*>(*it))->getLevel() == SgnLattice::uninitialized)
			(dynamic_cast<SgnLattice*>(*it))->initialize();
	}
	
	//SgNode* asgn;
	if(cfgUtils::isAssignment(n.getNode()))
	{
		varID i, j, k;
		bool negJ, negK;
		long c;
		short op;
		if(cfgUtils::parseAssignment(n.getNode(), op, i, j, negJ, k, negK, c))
		{
			/*printf("cfgUtils::parseAssignment(asgn, op, i, j, c) = 1\n");
			printf("        op=%s i=%s, j=%s%s, k=%s%s, c=%d\n", 
			       (op==cfgUtils::add? "+": (op==cfgUtils::subtract? "-": (op==cfgUtils::mult? "*": (op==cfgUtils::divide? "/": (op==cfgUtils::none? "~": "???"))))),
			       i.str().c_str(), negJ?"-":"", j.str().c_str(), negK?"-":"", k.str().c_str(), c);*/
			
			SgnLattice* iLattice = dynamic_cast<SgnLattice*>(prodLat->getVarLattice(func, i));
			SgnLattice newILattice;
			
			if(op==cfgUtils::none)
			{
				// i = c
				if(j==zeroVar && k==zeroVar)
					newILattice.set(c);
				// i = j;
				else if(k==zeroVar)
				{
					SgnLattice* jLattice = dynamic_cast<SgnLattice*>(prodLat->getVarLattice(func, j));
					newILattice.copy(jLattice);
					newILattice.mult(negJ?-1:1);
				}
			}
			else if(op==cfgUtils::add)	
			{
if(sgnAnalysisDebugLevel>=1){
				printf("          add, k==zeroVar = %d\n", k==zeroVar);
}
				// i = j + c
				if(k==zeroVar)
				{
					SgnLattice* jLattice = dynamic_cast<SgnLattice*>(prodLat->getVarLattice(func, j));
					newILattice.copy(jLattice);
					newILattice.mult(negJ?-1:1);
					newILattice.plus(c);
				}
				// i = j + k + c
				else
				{
					SgnLattice* jLattice = dynamic_cast<SgnLattice*>(prodLat->getVarLattice(func, j));
					SgnLattice* kLattice = dynamic_cast<SgnLattice*>(prodLat->getVarLattice(func, k));
					
					newILattice.copy(jLattice);
					newILattice.mult(negJ?-1:1);
					newILattice.plus(*kLattice);
					newILattice.mult(negK?-1:1);
					newILattice.plus(c);
				}
			}
			else if(op==cfgUtils::mult || op==cfgUtils::divide)	
			{
if(sgnAnalysisDebugLevel>=1){
				printf("operation: mult\n");
}
				// i = j * c OR i = j / c
				if(k==oneVar)
				{
					SgnLattice* jLattice = dynamic_cast<SgnLattice*>(prodLat->getVarLattice(func, j));
					newILattice.copy(jLattice);
					newILattice.mult(negJ?-1:1);
					newILattice.mult(c);
				}
				// i = j * k * c OR i = j / k / c
				else
				{
if(sgnAnalysisDebugLevel>=1){
					printf("   case i = j * k * c\n");
}
					SgnLattice* jLattice = dynamic_cast<SgnLattice*>(prodLat->getVarLattice(func, j));
					SgnLattice* kLattice = dynamic_cast<SgnLattice*>(prodLat->getVarLattice(func, k));
					
					newILattice.copy(jLattice);
					newILattice.mult(negJ?-1:1);
					newILattice.mult(*kLattice);
					newILattice.mult(negK?-1:1);
					newILattice.mult(c);
				}
			}
			modified = iLattice->copyMod(&newILattice) || modified;
		}
	}
	else if(isSgInitializedName(n.getNode()))
	{
		SgInitializedName* initName = isSgInitializedName(n.getNode());
		varID var(initName);
		
		SgnLattice* varLattice = dynamic_cast<SgnLattice*>(prodLat->getVarLattice(func, var));
		
		// if this is a scalar that we care about
		if(varLattice)
		{
if(sgnAnalysisDebugLevel>=1){
			printf("Variable declaration: %s\n", var.str().c_str());
}
			modified = varLattice->setBot() || modified;
		}
	}
	
	return modified;
}

// prints the Lattices set by the given SgnAnalysis 
void printSgnAnalysisStates(SgnAnalysis* sa, string indent)
{
	vector<int> factNames;
	vector<int> latticeNames;
	latticeNames.push_back(0);
	printAnalysisStates pas(sa, factNames, latticeNames, indent);
	UnstructuredPassInterAnalysis upia_pas(pas);
	upia_pas.runAnalysis();
}
