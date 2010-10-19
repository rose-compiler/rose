#include "divAnalysis.h"

int divAnalysisDebugLevel=0;

// CURRENTLY THE DIVISIBILITY ANALYSIS CANNOT HANDLE NEGATIVE NUMBERS!

// GCD implementation from Wikipedia
long gcd(long u, long v)
{
	int shift;
	
	/* GCD(0,x) := x */
	if (u == 0 || v == 0)
		return u | v;
	
	/* Let shift := lg K, where K is the greatest power of 2
	dividing both u and v. */
	for (shift = 0; ((u | v) & 1) == 0; ++shift) {
		u >>= 1;
		v >>= 1;
	}
	
	while ((u & 1) == 0)
		u >>= 1;
	
	/* From here on, u is always odd. */
	do {
		while ((v & 1) == 0)  /* Loop X */
		v >>= 1;
		
		/* Now u and v are both odd, so diff(u, v) is even.
		Let u = min(u, v), v = diff(u, v)/2. */
		if (u <= v) {
			v -= u;
		} else {
			int diff = u - v;
			u = v;
			v = diff;
		}
		v >>= 1;
	} while (v != 0);
	
	return u << shift;
}

/**********************
 ***** DivLattice *****
 **********************/

// The different levels of this lattice
// this object is uninitialized
const int DivLattice::uninitialized; 
// no information is known about the value of the variable
const int DivLattice::bottom; 
// the value of the variable is known
const int DivLattice::valKnown; 
// value is unknown but the divisibility (div and rem) of this variable is known 
const int DivLattice::divKnown; 
// this variable holds more values than can be represented using a single value and divisibility
const int DivLattice::top; 


// returns a copy of this lattice
Lattice* DivLattice::copy() const
{
	return new DivLattice(*this);
}

// overwrites the state of this Lattice with that of that Lattice
void DivLattice::copy(Lattice* that_arg)
{
	DivLattice* that = dynamic_cast<DivLattice*>(that_arg);
	
	this->value = that->value;
	this->div   = that->div;
	this->rem   = that->rem;
	this->level = that->level;
}

// returns true if the given value matches the given div, rem combo and false otherwise
bool DivLattice::matchDiv(long value, long div, long rem)
{
	return (value % div) == rem;
}

// Takes two lattices at level divKnown. If the two objects have matching div, rem pairs, returns
// true and sets div and rem to those mathching values. Otherwise, returns false;
bool DivLattice::matchDiv(DivLattice* one, DivLattice* two, long& div, long& rem)
{
	// if these object have the same div
	if(one->div == two->div)
	{
		// if they have the same rem, we're cool
		if(one->rem == two->rem)
		{
			div = one->div;
			rem = one->rem;
			return true;
		}
		// else, if the rems are different
		else
		{
			int divremgcd;
			// if one rem is 0 and the other 0 shares factors with the common div, those 
			// factors can be the new common div
			if(one->rem == 0 && (divremgcd = gcd(one->div, two->rem))>1)
			{
				div = divremgcd;
				rem = 0;
			}
			else if(two->rem == 0 && (divremgcd = gcd(one->div, one->rem))>1)
			{
				div = divremgcd;
				rem = 0;
			}
			// if the above doesn't work, the're not compatible
			else
				return false;
		}
	}
	// else, if these object have the different divs
	else
	{
		// but they're compatible because they have no remainders
		// and their divs are multiples of the same number
		if(one->rem==0 && two->rem==0)
		{
			if(one->div > two->div && one->div % two->div==0)
			{
				// the common div belongs to two
				div = two->div;
				rem = 0;
				return true;
			}
			else if(two->div > one->div && two->div % one->div==0)
			{
				// the common div belongs to one
				div = one->div;
				rem = 0;
				return true;
			}
			// else, they're not compatible
			else
				return false;
		}
		// else, if they're not compatible
		else return false;
	}
}

// computes the meet of this and that and saves the result in this
// returns true if this causes this to change and false otherwise
bool DivLattice::meetUpdate(Lattice* that_arg)
{
	long  oldValue = value;
	long  oldDiv = div;
	long  oldRem = rem;
	short oldLevel = level;
	DivLattice* that = dynamic_cast<DivLattice*>(that_arg);

/*printf("DivLattice::meetUpdate\n");
cout << "this: " << str("") << "\n";
cout << "that: " << that->str("") << "\n";*/

	// if this object is uninitialized, just copy the state of that
	if(level==uninitialized)
	{
		if(that->level > uninitialized)
			copy(that);
		goto Done;
	}
	// else, if that is uninitialized, leave this alone
	else if(that->level==uninitialized)
	{
		goto Done;
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
			goto Done;
		}
		// else, if that is bottom, leave this alone since this is at least bottom
		else if(that->level==bottom)
		{
			goto Done;
		}
		// else, if both are above bottom, perform the meet
		else
		{
			// if the two objects have known values 
			if(level==valKnown && that->level==valKnown)
			{
				// if they disagree on their values, move the state 
				// of this object to divKnown
				if(value != that->value)
				{
					div = gcd(value, that->value);
					rem = 0;
					value = -1;
					
					// if the gcd is > 1, then we know a useful divisor and the level is divKnown
					// otherwise, the divisor info is useless and the level becomes top
					if(div != 1)
						level = divKnown;
					else
						level = top;
					goto Done;
				}
				// else, if the two objects agree on their values, we can 
				// leave this object alone
				else
					goto Done;
			}
			else if(level==valKnown && that->level==divKnown)
			{
				// if this can be divided by that->div to get that->rem as the remainder,
				// we can use that->div, that->rem to represent both objects
				if(value%that->div == that->rem)
				{
					value=0;
					div = that->div;
					rem = that->rem;
					level = divKnown;
					goto Done;
				}
				// otherwise, the two objects are not compatible
			}
			else if(level==divKnown && that->level==valKnown)
			{
				// if this can be divided by that->div to get that->rem as the remainder,
				// we can use div, rem to represent both objects
				if(that->value%div == rem)
				{
					// leave this object alone
					goto Done;
				}
				// otherwise, the two objects are not compatible
			}
			else if(level==divKnown && that->level==divKnown)
			{
				long newDiv, newRem;
				bool match = matchDiv(this, that, newDiv, newRem);
				if(match)
				{
					div = newDiv;
					rem = newRem;
					goto Done;
				}
			}
		}
	}
	
	// if we haven't hit a case that goes to a non-top level, make this object top
	div = 1;
	rem = 0;
	value = 0;
	level = top;
	
	// the various cases that do not result in this object becoming top goto this label
	Done:

	// return whether this object was modified
	return (oldValue != value) ||
	       (oldDiv != div) ||
	       (oldRem != rem) ||
	       (oldLevel != level);
}

// computes the meet of this and that and returns the result
/*Lattice* meet(Lattice* that)
{
	DivLattice* newDL = new DivLattice(*this);
	newDL->meetUpdate(that);
}*/

bool DivLattice::operator==(Lattice* that_arg)
{
	DivLattice* that = dynamic_cast<DivLattice*>(that_arg);
	
	return (value == that->value) &&
	       (div == that->div) &&
	       (rem == that->rem) &&
	       (level == that->level);
}

/*// widens this from that and saves the result in this
// returns true if this causes this to change and false otherwise
bool DivLattice::widenUpdate(InfiniteLattice* that)
{
	return meetUpdate(that);
}*/

// returns the current state of this object
long DivLattice::getValue() const
{ return value; }

long DivLattice::getDiv() const
{ return div; }

long DivLattice::getRem() const
{ return rem; }

short DivLattice::getLevel() const
{ return level; }
	

// Sets the state of this lattice to bottom
// returns true if this causes the lattice's state to change, false otherwise
bool DivLattice::setBot()
{
	bool modified = this->level != bottom;
	this->value = 0;
	div = -1;
	rem = -1;
	level = bottom;
	return modified;
}

// Sets the state of this lattice to the given value.
// returns true if this causes the lattice's state to change, false otherwise
bool DivLattice::set(long value)
{
	bool modified = this->level != valKnown || this->value != value;
	this->value = value;
	div = -1;
	rem = -1;
	level = valKnown;
	return modified;
}

// Sets the state of this lattice to the given div/rem state.
// returns true if this causes the lattice's state to change, false otherwise
bool DivLattice::set(long div, long rem)
{
	bool modified = this->level != divKnown || this->div != div || this->rem != rem;
	value = 0;
	this->div = div;
	this->rem = rem;
	level = divKnown;
	return modified;
}

// Sets the state of this lattice to top
// returns true if this causes the lattice's state to change, false otherwise
bool DivLattice::setTop()
{
	bool modified = this->level != top;
	this->value = 0;
	div = 1;
	rem = 0;
	level = top;
	return modified;	
}

// Increments the state of this object by value
// returns true if this causes the lattice's state to change, false otherwise
bool DivLattice::incr(long increment)
{
	if(level == valKnown)
		value += increment;
	else if(level == divKnown)
		rem = (rem+increment)/div;
	else
		return false;
	
	return increment!=0;
}

// Multiplies the state of this object by value
// returns true if this causes the lattice's state to change, false otherwise
bool DivLattice::mult(long multiplier)
{
	if(level == valKnown)
		value *= multiplier;
	else if(level == divKnown)
	{
		// (m * div + rem) * c ~ m * (div * c) + rem*c
		div *= multiplier;
		rem *= multiplier;
	}
	else
		return false;
	
	return multiplier!=1;
}
	
string DivLattice::str(string indent)
{
	ostringstream outs;
	//printf("DivLattice::str() level=%d\n", level);
	if(level == uninitialized)
		outs << indent << "<level: uninitialized>";
	else if(level == bottom)
		outs << indent << "<level: bottom>";
	else if(level == valKnown)
		outs << indent << "<level: valKnown, val="<<value<<">";
	else if(level == divKnown)
		outs << indent << "<level: divKnown, div="<<div<<", rem="<<rem<<">";
	else if(level == top)
		outs << indent << "<level: top>";
	return outs.str();
}

/***********************
 ***** DivAnalysis *****
 ***********************/

map<varID, Lattice*> DivAnalysis::constVars;
bool DivAnalysis::constVars_init=false;

/*// generates the initial variable-specific lattice state for a dataflow node
Lattice* DivAnalysis::genInitVarState(const Function& func, const DataflowNode& n, const NodeState& state)
{
	DivLattice* newLattice = new DivLattice();
	/*printf("DivLattice::genInitNonVarState() newLattice=%p\n", newLattice);
	printf("DivLattice::genInitNonVarState() newLattice=%s\n", newLattice->str().c_str());* /
	return newLattice;
}

// generates the initial non-variable-specific lattice state for a dataflow node
Lattice* DivAnalysis::genInitNonVarState(const Function& func, const DataflowNode& n, const NodeState& state)
{
	return NULL;
}*/

// generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
//vector<Lattice*> DivAnalysis::genInitState(const Function& func, const DataflowNode& n, const NodeState& state)
void DivAnalysis::genInitState(const Function& func, const DataflowNode& n, const NodeState& state,
                        vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts)
{
	//vector<Lattice*> initLattices;
	FiniteVariablesProductLattice* l = new FiniteVariablesProductLattice(true, false, new DivLattice(), genConstVarLattices(), NULL, func, n, state);
	//printf("DivAnalysis::genInitState, returning %p\n", l);
	initLattices.push_back(l);
	
/*printf("DivAnalysis::genInitState() initLattices:\n");
for(vector<Lattice*>::iterator it = initLattices.begin(); 
    it!=initLattices.end(); it++)
{	
	cout << *it << ": " << (*it)->str("    ") << "\n";
}*/
	
	//return initLattices;
}

// Returns a map of special constant variables (such as zeroVar) and the lattices that correspond to them
// These lattices are assumed to be constants: it is assumed that they are never modified and it is legal to 
//    maintain only one copy of each lattice may for the duration of the analysis.
map<varID, Lattice*>& DivAnalysis::genConstVarLattices() const
{
	if(!constVars_init)
	{
		DivLattice* newL = new DivLattice();
		newL->set(0);
		constVars[zeroVar] = newL;
		
		newL = new DivLattice();
		newL->set(1);
		constVars[oneVar] = newL;

		constVars_init = true;
		/*
		cout << "constVars:\n";
		for(map<varID, Lattice*>::iterator it = constVars.begin(); it!=constVars.end(); it++)
		{ cout << it->first.str() << ": " << it->second->str("") << "\n"; }
		
		printf("oneVar == zeroVar = %d\n", oneVar == zeroVar);
		printf("oneVar < zeroVar = %d\n", oneVar < zeroVar);
		printf("zeroVar < oneVar = %d\n", zeroVar < oneVar);*/
	}

	return constVars;
}

bool DivAnalysis::transfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo)
{
	bool modified=false;
	
	FiniteVariablesProductLattice* prodLat = dynamic_cast<FiniteVariablesProductLattice*>(*(dfInfo.begin()));
	
	//printf("DivAnalysis::transfer, n.getNode()=<%s | %s>\n", n.getNode()->class_name().c_str(), n.getNode()->unparseToString().c_str());
	
	// make sure that all the non-constant Lattices are initialized
	const vector<Lattice*>& lattices = prodLat->getLattices();
	for(vector<Lattice*>::const_iterator it = lattices.begin(); it!=lattices.end(); it++)
	{
		// !!! Now initialize() is a noop on initialized lattices
		if((dynamic_cast<DivLattice*>(*it))->getLevel() == DivLattice::uninitialized)
			(dynamic_cast<DivLattice*>(*it))->initialize();
	}
	
	//SgNode* asgn;
	//printf("cfgUtils::isAssignment(n.getNode()) = %p\n", cfgUtils::isAssignment(n.getNode()));
	if(cfgUtils::isAssignment(n.getNode()))
	{
		varID i, j, k;
		bool negJ, negK;
		long c;
		short op;
		if(cfgUtils::parseAssignment(n.getNode(), op, i, j, negJ, k, negK, c))
		{
if(divAnalysisDebugLevel>=1){
			printf("cfgUtils::parseAssignment(asgn, op, i, j, c) = 1\n");
			printf("        op=%s i=%s, j=%s%s, k=%s%s, c=%d\n", 
			       (op==cfgUtils::add? "+": (op==cfgUtils::subtract? "-": (op==cfgUtils::mult? "*": (op==cfgUtils::divide? "/": (op==cfgUtils::none? "~": "???"))))),
			       i.str().c_str(), negJ?"-":"", j.str().c_str(), negK?"-":"", k.str().c_str(), c);
}
			
			DivLattice* iLattice = dynamic_cast<DivLattice*>(prodLat->getVarLattice(func, i));
			
			if(op==cfgUtils::none)
			{
				// i = c
				if(j==zeroVar && k==zeroVar)
					iLattice->set(c);
				// i = j;
				else if(k==zeroVar)
				{
					DivLattice* jLattice = dynamic_cast<DivLattice*>(prodLat->getVarLattice(func, j));
					iLattice->copy(jLattice);
				}
			}
			else if(op==cfgUtils::add)	
			{
if(divAnalysisDebugLevel>=1){
				printf("          add, k==zeroVar = %d\n", k==zeroVar);
}
				// i = j + c
				if(k==zeroVar)
				{
					DivLattice* jLattice = dynamic_cast<DivLattice*>(prodLat->getVarLattice(func, j));
					
					if(jLattice->getLevel() == DivLattice::bottom)
						modified = iLattice->setBot() || modified;
					else if(jLattice->getLevel() == DivLattice::valKnown) 
						modified = iLattice->set(jLattice->getValue() + c) || modified;
					else if(jLattice->getLevel() == DivLattice::divKnown)
						modified = iLattice->set(jLattice->getDiv(), (jLattice->getRem()+c)%jLattice->getDiv()) || modified;
					else
						modified = iLattice->setTop() || modified;				}
				// i = j + k + c
				else
				{
					DivLattice* jLattice = dynamic_cast<DivLattice*>(prodLat->getVarLattice(func, j));
					DivLattice* kLattice = dynamic_cast<DivLattice*>(prodLat->getVarLattice(func, k));
					
					/*printf("jLattice = %s\n", jLattice->str().c_str());
					printf("kLattice = %s\n", kLattice->str().c_str());*/
					
					if(jLattice->getLevel() == DivLattice::bottom || kLattice->getLevel() == DivLattice::bottom)
						modified = iLattice->setBot() || modified;
					else if(jLattice->getLevel() == DivLattice::valKnown && kLattice->getLevel() == DivLattice::valKnown)
						modified = iLattice->set(jLattice->getValue() + kLattice->getValue() + c) || modified;
					else if(jLattice->getLevel() == DivLattice::valKnown && kLattice->getLevel() == DivLattice::divKnown)
					{
						if(DivLattice::matchDiv(jLattice->getValue(), kLattice->getDiv(), kLattice->getRem()))
						{
if(divAnalysisDebugLevel>=1){
							printf("lattices match\n");
}
							modified = iLattice->set(kLattice->getDiv(), kLattice->getRem()) || modified;
							modified = iLattice->incr(c) || modified;
						}
						else
						{
if(divAnalysisDebugLevel>=1){
							printf("lattices don't match, j.val(%d)/k.div(%d)=%d != k.rem(%d)\n", jLattice->getValue(), kLattice->getDiv(), jLattice->getValue()/kLattice->getDiv(), kLattice->getRem());
}
							
							modified = iLattice->setTop() || modified;
						}
					}
					else if(jLattice->getLevel() == DivLattice::divKnown && kLattice->getLevel() == DivLattice::valKnown)
					{
						if(DivLattice::matchDiv(kLattice->getValue(), jLattice->getDiv(), jLattice->getRem()))
						{
							modified = iLattice->set(jLattice->getDiv(), jLattice->getRem()) || modified;
							modified = iLattice->incr(+c) || modified;
						}
						else
							modified = iLattice->setTop() || modified;
					}
					else if(jLattice->getLevel() == DivLattice::divKnown && kLattice->getLevel() == DivLattice::divKnown)
					{
						long newDiv, newRem;
						bool match = DivLattice::matchDiv(jLattice, kLattice, newDiv, newRem);
						if(match)
						{
							modified = iLattice->set(newDiv, newRem) || modified;
							modified = iLattice->incr(+c) || modified;
						}
						else
							modified = iLattice->setTop() || modified;
					}
					else
						modified = iLattice->setTop() || modified;
				}
			}
			else if(op==cfgUtils::mult)	
			{
if(divAnalysisDebugLevel>=1){
				printf("operation: mult\n");
}
				// i = j * c
				if(k==oneVar)
				{
if(divAnalysisDebugLevel>=1){
					printf("   case i = j * c\n");
}
					DivLattice* jLattice = dynamic_cast<DivLattice*>(prodLat->getVarLattice(func, j));
					
if(divAnalysisDebugLevel>=1){
					printf("   i=%s, j=%s\n", i.str().c_str(), j.str().c_str());
					printf("   iLattice=%s  jLattice=%s\n", iLattice->str().c_str(), jLattice->str().c_str());
}
					
					if(jLattice->getLevel() == DivLattice::bottom)
						modified = iLattice->setBot() || modified;
					else if(jLattice->getLevel() == DivLattice::valKnown) 
						modified = iLattice->set(jLattice->getValue() * c) || modified;
					else if(jLattice->getLevel() == DivLattice::divKnown)
						modified = iLattice->set(jLattice->getDiv() * c, jLattice->getRem() * c) || modified;
					else
						modified = iLattice->setTop() || modified;
				}
				// i = j * k * c
				else
				{
if(divAnalysisDebugLevel>=1){
					printf("   case i = j * k * c\n");
}
					DivLattice* jLattice = dynamic_cast<DivLattice*>(prodLat->getVarLattice(func, j));
					DivLattice* kLattice = dynamic_cast<DivLattice*>(prodLat->getVarLattice(func, k));
					
					/*printf("jLattice = %s\n", jLattice->str().c_str());
					printf("kLattice = %s\n", kLattice->str().c_str());*/
					
					if(jLattice->getLevel() == DivLattice::bottom || kLattice->getLevel() == DivLattice::bottom)
						modified = iLattice->setBot() || modified;
					else if(jLattice->getLevel() == DivLattice::valKnown && kLattice->getLevel() == DivLattice::valKnown)
						modified = iLattice->set(jLattice->getValue() * kLattice->getValue() * c) || modified;
					else if(jLattice->getLevel() == DivLattice::valKnown && kLattice->getLevel() == DivLattice::divKnown)
					{
						// (m*k.div + k.rem)*j.val*c = (m*(k.div*j.val*c) + k.rem*j.val*c) = 
						modified = iLattice->set(kLattice->getDiv(), kLattice->getRem()) || modified;
						modified = iLattice->mult(jLattice->getValue()) || modified;
						modified = iLattice->mult(c) || modified;
						
					}
					else if(jLattice->getLevel() == DivLattice::divKnown && kLattice->getLevel() == DivLattice::valKnown)
					{
						// (m*j.div + j.rem)*k.val*c = (m*(j.div*k.val*c) + j.rem*k.val*c) = 
						modified = iLattice->set(jLattice->getDiv(), jLattice->getRem()) || modified;
						modified = iLattice->mult(kLattice->getValue()) || modified;
						modified = iLattice->mult(c) || modified;
					}
					else if(jLattice->getLevel() == DivLattice::divKnown && kLattice->getLevel() == DivLattice::divKnown)
					{
						modified = iLattice->setTop() || modified;
					}
					else
						modified = iLattice->setTop() || modified;
				}
			}
			
			else if(op==cfgUtils::divide)	
			{
if(divAnalysisDebugLevel>=1){
				printf("operation: divide\n");
}
				// i = j / c
				if(k==oneVar)
				{
if(divAnalysisDebugLevel>=1){
					printf("   case i = j / c\n");
}
					DivLattice* jLattice = dynamic_cast<DivLattice*>(prodLat->getVarLattice(func, j));
					
if(divAnalysisDebugLevel>=1){
					printf("   i=%s, j=%s\n", i.str().c_str(), j.str().c_str());
					printf("   iLattice=%s  jLattice=%s\n", iLattice->str().c_str(), jLattice->str().c_str());
}
					
					if(jLattice->getLevel() == DivLattice::bottom)
						modified = iLattice->setBot() || modified;
					else if(jLattice->getLevel() == DivLattice::valKnown) 
						modified = iLattice->set(jLattice->getValue() / c) || modified;
					else if(jLattice->getLevel() == DivLattice::divKnown && 
					        (jLattice->getDiv()%c == 0)&& (jLattice->getRem()%c == 0))
						modified = iLattice->set(jLattice->getDiv() / c, jLattice->getRem() / c) || modified;
					else
						modified = iLattice->setTop() || modified;
				}
				// i = j / k
				else if(c==1)
				{
if(divAnalysisDebugLevel>=1){
					printf("   case i = j * k * c\n");
}
					DivLattice* jLattice = dynamic_cast<DivLattice*>(prodLat->getVarLattice(func, j));
					DivLattice* kLattice = dynamic_cast<DivLattice*>(prodLat->getVarLattice(func, k));
					
					/*printf("jLattice = %s\n", jLattice->str().c_str());
					printf("kLattice = %s\n", kLattice->str().c_str());*/
					
					if(jLattice->getLevel() == DivLattice::bottom || kLattice->getLevel() == DivLattice::bottom)
						modified = iLattice->setBot() || modified;
					else if(jLattice->getLevel() == DivLattice::valKnown && kLattice->getLevel() == DivLattice::valKnown)
						modified = iLattice->set(jLattice->getValue() * kLattice->getValue() / c) || modified;
					else if(jLattice->getLevel() == DivLattice::valKnown && kLattice->getLevel() == DivLattice::divKnown &&
					       kLattice->getRem()==0 && jLattice->getValue()%kLattice->getDiv() == 0)
						modified = iLattice->set(jLattice->getValue()/kLattice->getDiv()) || modified;
					else if(jLattice->getLevel() == DivLattice::divKnown && kLattice->getLevel() == DivLattice::valKnown &&
					        (jLattice->getDiv()%kLattice->getValue() == 0) && (jLattice->getRem()%kLattice->getValue() == 0))
						modified = iLattice->set(jLattice->getDiv()/kLattice->getValue(), jLattice->getRem()/kLattice->getValue()) || modified;
					else if(jLattice->getLevel() == DivLattice::divKnown && kLattice->getLevel() == DivLattice::divKnown &&
					        kLattice->getRem()==0 && (jLattice->getDiv()%kLattice->getDiv() == 0) && (jLattice->getRem()%kLattice->getDiv() == 0))
					{
						modified = iLattice->set(jLattice->getDiv()/kLattice->getDiv(), jLattice->getRem()/kLattice->getDiv()) || modified;
					}
					else
						modified = iLattice->setTop() || modified;
				}
			}
			
			goto Done;
		}
	}
	else if(isSgInitializedName(n.getNode()))
	{
		SgInitializedName* initName = isSgInitializedName(n.getNode());
		varID var(initName);
		
		DivLattice* varLattice = dynamic_cast<DivLattice*>(prodLat->getVarLattice(func, var));
		
		// if this is a scalar that we care about
		if(varLattice)
		{
if(divAnalysisDebugLevel>=1){
			printf("Variable declaration: %s\n", var.str().c_str());
}
			modified = varLattice->setBot() || modified;
			goto Done;
		}
	}
	
	Done:
	return modified;
}

// prints the Lattices set by the given DivAnalysis 
void printDivAnalysisStates(DivAnalysis* da, string indent)
{
	vector<int> factNames;
	vector<int> latticeNames;
	latticeNames.push_back(0);
	printAnalysisStates pas(da, factNames, latticeNames, indent);
	UnstructuredPassInterAnalysis upia_pas(pas);
	upia_pas.runAnalysis();
}
