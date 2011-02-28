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
// true and sets div and rem to those mathching values. Otherwise, returns false.
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
				return true;
			}
			else if(two->rem == 0 && (divremgcd = gcd(one->div, one->rem))>1)
			{
				div = divremgcd;
				rem = 0;
				return true;
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

// Takes two lattices at level divKnown. If the two objects have div, rem pairs that make it
// possible to add or subtract them them and produce div/rem information where div>1, 
// returns true and sets div and rem to correspond to the sum of these values.
// Otherwise, returns false.
// plus - true if the caller want to see one+two and false if one-two
bool DivLattice::matchDivAddSubt(DivLattice* one, DivLattice* two, long& div, long& rem, bool plus)
{
	// If these object have the same div
	if(one->div == two->div)
	{
		cout << "DivLattice::matchDivAddSubt() one="<<one->str("")<<" two="<<two->str("")<<"\n";
		div = one->div;
		if(plus) rem = (one->rem + two->rem)%div;
		else     rem = (one->rem - two->rem + div)%div;
		return true;
	}
	// Else, if these object have the different divs
	else
	{
		div = gcd(one->div, two->div);
		// If their divisors have a factor in common, their sum can be expressed using a divisor that has this factor
		if(div>1) {
			if(plus) rem = (one->rem%div + two->rem%div)%div;
			else     rem = (one->rem%div - two->rem%div + div)%div;
			return true;
		}
		// Else, if they're not compatible
		else
			return false;
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
		//cout << "    level="<<level<<" that->level="<<that->level<<"\n";
		if(that->level > uninitialized)
			copy(that);
		//cout << "    level="<<level<<" that->level="<<that->level<<"\n";
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
					
					// If the gcd is > 1, then we know a useful divisor and the level is divKnown
					// otherwise, the divisor info is useless and the level becomes top
					if(div != 1) {
						level = divKnown;
						rem = 0;
						value = -1;
					} else {
						// If we cannot find a good divisor, we guess that the variable's new value
						// was produced via an iteration where the variable's value is repeatedly
						// incremented by a constant. If this is the case, the divisor is the difference
						// between the two values. If this guess is wrong, the next time we update
						// this lattice, we'll update it to top.
						level = divKnown;
						div = that->value - value;
						rem = value % div;
					}
					goto Done;
				}
				// else, if the two objects agree on their values, we can 
				// leave this object alone
				else
					goto Done;
			}
			else if(level==valKnown && that->level==divKnown)
			{
				//cout << "(level==valKnown && that->level==divKnown) value%that->div="<<(value%that->div)<<" that->rem="<<that->rem<<"\n";
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
				//cout << "(level==divKnown && that->level==valKnown) that->value%div="<<that->value%div<<" == rem="<<rem<<"\n";
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
	
	/*cout << "operator == ("<<str()<<", "<<that->str()<<"): "<<((value == that->value) &&
	       (div == that->div) &&
	       (rem == that->rem) &&
	       (level == that->level))<<"\n";*/
	
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
	printf("DivLattice::genInitNonVarState() newLattice=%p\n", newLattice);
	printf("DivLattice::genInitNonVarState() newLattice=%s\n", newLattice->str().c_str());
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
	map<varID, Lattice*> emptyM;
	FiniteVarsExprsProductLattice* l = new FiniteVarsExprsProductLattice((Lattice*)new DivLattice(), emptyM/*genConstVarLattices()*/, 
	                                                                     (Lattice*)NULL, ldva, /*func, */n, state);
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
/*map<varID, Lattice*>& DivAnalysis::genConstVarLattices() const
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
		/ *
		cout << "constVars:\n";
		for(map<varID, Lattice*>::iterator it = constVars.begin(); it!=constVars.end(); it++)
		{ cout << it->first.str() << ": " << it->second->str("") << "\n"; }
		
		printf("oneVar == zeroVar = %d\n", oneVar == zeroVar);
		printf("oneVar < zeroVar = %d\n", oneVar < zeroVar);
		printf("zeroVar < oneVar = %d\n", zeroVar < oneVar);* /
	}

	return constVars;
}*/

bool DivAnalysis::transfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo)
{
	bool modified=false;
	
	FiniteVarsExprsProductLattice* prodLat = dynamic_cast<FiniteVarsExprsProductLattice*>(*(dfInfo.begin()));
	
	//cout << "transfer A prodLat="<<prodLat<<"="<<prodLat->str("    ")<<"\n";
	// Make sure that all the lattices are initialized
	//prodLat->initialize();
	const vector<Lattice*>& lattices = prodLat->getLattices();
	for(vector<Lattice*>::const_iterator it = lattices.begin(); it!=lattices.end(); it++)
		(dynamic_cast<DivLattice*>(*it))->initialize();
	
	// Plain assignment: lhs = rhs
	if(isSgAssignOp(n.getNode())) {
		varID res = SgExpr2Var(isSgExpression(n.getNode()));
		varID lhs = SgExpr2Var(isSgAssignOp(n.getNode())->get_lhs_operand());
		varID rhs = SgExpr2Var(isSgAssignOp(n.getNode())->get_rhs_operand());
		if(divAnalysisDebugLevel>=1) {
			cout << "res="<<res.str()<<" lhs="<<lhs.str()<<" rhs="<<rhs.str()<<"\n";
		}
		
		DivLattice* resLat = dynamic_cast<DivLattice*>(prodLat->getVarLattice(res));
		DivLattice* lhsLat = dynamic_cast<DivLattice*>(prodLat->getVarLattice(lhs));
		DivLattice* rhsLat = dynamic_cast<DivLattice*>(prodLat->getVarLattice(rhs));
		
		if(divAnalysisDebugLevel>=1) {
			if(resLat) cout << "resLat=\n    "<<resLat->str("    ")<<"\n";
			if(lhsLat) cout << "lhsLat=\n    "<<lhsLat->str("    ")<<"\n";
			if(rhsLat) cout << "rhsLat=\n    "<<rhsLat->str("    ")<<"\n";
		}
		
		// Copy the lattice of the right-hand-side to both the left-hand-side variable and to the assignment expression itself
		if(resLat) // If the left-hand-side contains a live expression or variable
		{ resLat->copy(rhsLat); modified = true; }
		if(lhsLat) // If the left-hand-side contains a live expression or variable
		{ lhsLat->copy(rhsLat); modified = true; }
	// Initializer for a variable
	} else if(isSgAssignInitializer(n.getNode())) {
		varID res = SgExpr2Var(isSgAssignInitializer(n.getNode()));
		varID asgn = SgExpr2Var(isSgAssignInitializer(n.getNode())->get_operand());

		DivLattice* asgnLat = dynamic_cast<DivLattice*>(prodLat->getVarLattice(asgn));
		DivLattice* resLat = dynamic_cast<DivLattice*>(prodLat->getVarLattice(res));
		if(divAnalysisDebugLevel>=1) {
			if(asgnLat) cout << "asgnLat=    "<<asgnLat->str("    ")<<"\n";
			if(resLat) cout << "resLat=    "<<resLat->str("    ")<<"\n";
		}

		// If the result expression is live
		if(resLat) { resLat->copy(asgnLat); modified = true; }
	// Variable Declaration
	} else if(isSgInitializedName(n.getNode())) {
		SgInitializedName* initName = isSgInitializedName(n.getNode());
		varID var(initName);
		DivLattice* varLat = dynamic_cast<DivLattice*>(prodLat->getVarLattice(var));
		
		//cout << "DivAnalysis::transfer() isSgInitializedName var="<<var.str()<<" varLat="<<varLat<<"\n";
		
		// if this is a scalar that we care about, initialize it to Bottom
		if(varLat)
		{
			//if(divAnalysisDebugLevel>=1) cout << "Variable declaration: "<<var.str()<<", get_initializer()="<<initName->get_initializer()<<"\n";
			// If there was no initializer
			if(initName->get_initializer()==NULL)
				modified = varLat->setBot() || modified;
			else {
				varID init = SgExpr2Var(initName->get_initializer());
				DivLattice* initLat = dynamic_cast<DivLattice*>(prodLat->getVarLattice(init));
				//if(divAnalysisDebugLevel>=1) cout << "    init="<<init.str()<<" initLat="<<initLat<<"\n";
				if(initLat) {
					varLat->copy(initLat);
					modified = true;
				}
			}
		}
	// Integral Numeric Constants
	} else if(isSgLongLongIntVal(n.getNode())         || isSgLongIntVal(n.getNode()) || 
	          isSgIntVal(n.getNode())                 || isSgShortVal(n.getNode()) ||
	          isSgUnsignedLongLongIntVal(n.getNode()) || isSgUnsignedLongVal(n.getNode()) || 
	          isSgUnsignedIntVal(n.getNode())         || isSgUnsignedShortVal(n.getNode())) {
		varID res = SgExpr2Var(isSgExpression(n.getNode()));
		DivLattice* resLat = dynamic_cast<DivLattice*>(prodLat->getVarLattice(res));
		
		// If the result expression is live
/// !!! ADD MODIFICATION DETECTION
		if(resLat) {
			     if(isSgLongLongIntVal(n.getNode()))         modified = resLat->set(isSgLongLongIntVal(n.getNode())->get_value())         || modified;
			else if(isSgLongIntVal(n.getNode()))             modified = resLat->set(isSgLongIntVal(n.getNode())->get_value())             || modified;
			else if(isSgIntVal(n.getNode()))                 modified = resLat->set(isSgIntVal(n.getNode())->get_value())                 || modified;
			else if(isSgShortVal(n.getNode()))               modified = resLat->set(isSgShortVal(n.getNode())->get_value())               || modified;
			else if(isSgUnsignedLongLongIntVal(n.getNode())) modified = resLat->set(isSgUnsignedLongLongIntVal(n.getNode())->get_value()) || modified;
			else if(isSgUnsignedLongVal(n.getNode()))        modified = resLat->set(isSgUnsignedLongVal(n.getNode())->get_value())        || modified;
			else if(isSgUnsignedIntVal(n.getNode()))         modified = resLat->set(isSgUnsignedIntVal(n.getNode())->get_value())         || modified;
			else if(isSgUnsignedShortVal(n.getNode()))       modified = resLat->set(isSgUnsignedShortVal(n.getNode())->get_value())       || modified;
		}
	// Non-integral Constants
	} else if(isSgValueExp(n.getNode())) {
		varID res = SgExpr2Var(isSgExpression(n.getNode()));
		DivLattice* resLat = dynamic_cast<DivLattice*>(prodLat->getVarLattice(res));
		// If the result expression is live
		if(resLat) modified = resLat->setTop() || modified;
	// Arithmetic Operations
	} else if(isSgPlusAssignOp(n.getNode())  || isSgAddOp(n.getNode()) ||
	          isSgMinusAssignOp(n.getNode()) || isSgMinusOp(n.getNode()) ||
	          isSgModAssignOp(n.getNode())   || isSgModOp(n.getNode()) ||
	          isSgMultAssignOp(n.getNode())  || isSgMultiplyOp(n.getNode()) ||
	          isSgDivAssignOp(n.getNode())   || isSgDivideOp(n.getNode()) ||
	          isSgMinusMinusOp(n.getNode())  || isSgMinusOp(n.getNode()) || 
	          isSgPlusPlusOp(n.getNode())    || isSgUnaryAddOp(n.getNode())
	          // Mod and exponentiation
	          ) {
		varID lhs, arg1, arg2;
		varID res = SgExpr2Var(isSgExpression(n.getNode()));
		DivLattice *resLat, *arg1Lat, *arg2Lat;
		
		// Set up the information on the arguments and target of the arithmetic operation
		if(isSgBinaryOp(n.getNode())) {
			if(isSgPlusAssignOp(n.getNode()) || isSgMinusAssignOp(n.getNode()) ||
			   isSgModAssignOp(n.getNode())  || isSgMultAssignOp(n.getNode()) ||
			   isSgDivAssignOp(n.getNode())) {
				lhs = SgExpr2Var(isSgBinaryOp(n.getNode())->get_lhs_operand());
				arg1 = lhs;
				arg2 = SgExpr2Var(isSgBinaryOp(n.getNode())->get_rhs_operand());
				//cout << "lhs="<<lhs.str()<<" arg1="<<arg1.str()<<" arg2="<<arg2.str()<<"\n";
			} else {
				arg1 = SgExpr2Var(isSgBinaryOp(n.getNode())->get_lhs_operand());
				arg2 = SgExpr2Var(isSgBinaryOp(n.getNode())->get_rhs_operand());
			}
			arg1Lat = dynamic_cast<DivLattice*>(prodLat->getVarLattice(arg1));
			arg2Lat = dynamic_cast<DivLattice*>(prodLat->getVarLattice(arg2));
		} else if(isSgUnaryOp(n.getNode())) {
			arg1 = SgExpr2Var(isSgUnaryOp(n.getNode())->get_operand());
			arg1Lat = dynamic_cast<DivLattice*>(prodLat->getVarLattice(arg1));
			// Unary Update
			if(isSgMinusMinusOp(n.getNode()) || isSgPlusPlusOp(n.getNode())) {
				arg2Lat = new DivLattice(1);
			}
			//cout << "res="<<res.str()<<" arg1="<<arg1.str()<<" arg1Lat="<<arg1Lat<<", arg2Lat="<<arg2Lat<<"\n";
		}
		resLat = dynamic_cast<DivLattice*>(prodLat->getVarLattice(res));
		//cout << "transfer B, resLat="<<resLat<<"\n";
		
		// If the result expression is dead but the left-hand-side of the expression is live,
		// update the left-hand-side with the result
		if(resLat==NULL && 
			(isSgPlusAssignOp(n.getNode()) || isSgMinusAssignOp(n.getNode()) ||
			 isSgMultAssignOp(n.getNode()) || isSgDivAssignOp(n.getNode()) ||
			 isSgModAssignOp(n.getNode())) &&
			prodLat->getVarLattice(lhs)!=NULL)
		{ resLat = dynamic_cast<DivLattice*>(prodLat->getVarLattice(lhs)); }
		
		//cout << "transfer C, resLat="<<resLat<<"\n";
		// If the result or left-hand-side expression as well as the arguments are live
		if(resLat && arg1Lat && arg2Lat) {
			// ADDITION / SUBTRACTION
			if(isSgPlusAssignOp(n.getNode())  || isSgAddOp(n.getNode()) ||
		      isSgMinusAssignOp(n.getNode()) || isSgMinusOp(n.getNode()) ||
		      isSgMinusMinusOp(n.getNode())  || isSgPlusPlusOp(n.getNode())) {
		      // Either one Bottom or Uninitialized
		      if(//arg1Lat->getLevel() == DivLattice::uninitialized || arg2Lat->getLevel() == DivLattice::uninitialized ||
		         arg1Lat->getLevel() == DivLattice::bottom        || arg2Lat->getLevel() == DivLattice::bottom) {
					modified = resLat->setBot() || modified;
				// Both ValKnown
				} else if(arg1Lat->getLevel() == DivLattice::valKnown && arg2Lat->getLevel() == DivLattice::valKnown) {
					// Addition
					if(isSgPlusAssignOp(n.getNode()) || isSgAddOp(n.getNode()) || isSgPlusPlusOp(n.getNode()))
						modified = resLat->set(arg1Lat->getValue() + arg2Lat->getValue()) || modified;
					// Subtraction
					else
						modified = resLat->set(arg1Lat->getValue() - arg2Lat->getValue()) || modified;
				// Arg1 ValKnown, Arg2 DivKnown
				} else if(arg1Lat->getLevel() == DivLattice::valKnown && arg2Lat->getLevel() == DivLattice::divKnown) {
					// Addition
					if(isSgPlusAssignOp(n.getNode()) || isSgAddOp(n.getNode()) || isSgPlusPlusOp(n.getNode()))
						modified = resLat->set(arg2Lat->getDiv(), 
						                       (arg1Lat->getValue() + arg2Lat->getRem()) %
						                           arg2Lat->getDiv()) || modified;
					// Subtraction
					else
						modified = resLat->set(arg2Lat->getDiv(), 
						                      ((arg1Lat->getValue()%arg2Lat->getDiv()) - arg2Lat->getRem() + arg2Lat->getDiv()) %
						                          arg2Lat->getDiv()) || modified;
				}
				// Arg1 DivKnown, Arg2 ValKnown
				else if(arg1Lat->getLevel() == DivLattice::divKnown && arg2Lat->getLevel() == DivLattice::valKnown) {
					// Addition
					if(isSgPlusAssignOp(n.getNode()) || isSgAddOp(n.getNode()) || isSgPlusPlusOp(n.getNode()))
						modified = resLat->set(arg1Lat->getDiv(), 
						                       (arg2Lat->getValue() + arg1Lat->getRem()) %
						                           arg1Lat->getDiv()) || modified;
					// Subtraction
					else
						modified = resLat->set(arg1Lat->getDiv(), 
						                      (arg1Lat->getRem() - (arg2Lat->getValue()%arg1Lat->getDiv()) + arg2Lat->getDiv()) %
						                          arg1Lat->getDiv()) || modified;
				}
				// Both DivKnown
				else if(arg1Lat->getLevel() == DivLattice::divKnown && arg2Lat->getLevel() == DivLattice::divKnown) {
					long newDiv, newRem;
					// True for addition, false for subtraction
					bool plus = (isSgPlusAssignOp(n.getNode()) || isSgAddOp(n.getNode()) || isSgPlusPlusOp(n.getNode()));
					
					//if(DivLattice::matchDiv(arg1Lat, arg2Lat, newDiv, newRem))
					if(DivLattice::matchDivAddSubt(arg1Lat, arg2Lat, newDiv, newRem, plus)) {
						modified = resLat->set(newDiv, newRem) || modified;
					} else
						modified = resLat->setTop() || modified;
				}
				// Else => Top
				else
					modified = resLat->setTop() || modified;
			// Negation
			} else if(isSgMinusOp(n.getNode())) {
				resLat->copy(arg1Lat); modified = true;
				if(resLat->getLevel() == DivLattice::valKnown)
					modified = resLat->set(0-resLat->getValue()) || modified;
				else if(resLat->getLevel() == DivLattice::divKnown)
					modified = resLat->set(resLat->getDiv(),
					                       (resLat->getDiv()-resLat->getRem())%resLat->getDiv()) || modified;
			// PLUS SIGN
			} else if(isSgUnaryAddOp(n.getNode())) {
				resLat->copy(arg1Lat); modified = true;
			// MULTIPLICATION
			} else if(isSgMultAssignOp(n.getNode()) || isSgMultiplyOp(n.getNode())) {
				if(divAnalysisDebugLevel>=1) printf("   case i = j * k\n");
				/*printf("arg1Lat = %s\n", arg1Lat->str().c_str());
				printf("arg2Lat = %s\n", arg2Lat->str().c_str());*/
				
				// Both Bottom
				if(arg1Lat->getLevel() == DivLattice::bottom || arg2Lat->getLevel() == DivLattice::bottom)
					modified = resLat->setBot() || modified;
				// Both ValKnown
				else if(arg1Lat->getLevel() == DivLattice::valKnown && arg2Lat->getLevel() == DivLattice::valKnown)
					modified = resLat->set(arg1Lat->getValue() * arg2Lat->getValue()) || modified;
				// Arg1 ValKnown, Arg2 DivKnown, Arg1
				else if(arg1Lat->getLevel() == DivLattice::valKnown && arg2Lat->getLevel() == DivLattice::divKnown)
				{
					// (m*k.div + k.rem)*j.val*c = (m*(k.div*j.val*c) + k.rem*j.val*c) = 
					modified = resLat->set(arg2Lat->getDiv(), arg2Lat->getRem()) || modified;
					modified = resLat->mult(arg1Lat->getValue()) || modified;
				}
				// Arg1 DivKnown, Arg2 ValKnown
				else if(arg1Lat->getLevel() == DivLattice::divKnown && arg2Lat->getLevel() == DivLattice::valKnown)
				{
					// (m*j.div + j.rem)*k.val*c = (m*(j.div*k.val*c) + j.rem*k.val*c) = 
					modified = resLat->set(arg1Lat->getDiv(), arg1Lat->getRem()) || modified;
					modified = resLat->mult(arg2Lat->getValue()) || modified;
				}
				// Both DivKnown => Top
				else if(arg1Lat->getLevel() == DivLattice::divKnown && arg2Lat->getLevel() == DivLattice::divKnown)
				{
					modified = resLat->setTop() || modified;
				}
				// Else => Top
				else
					modified = resLat->setTop() || modified;
			// DIVISION
			} else if(isSgDivAssignOp(n.getNode()) || isSgDivideOp(n.getNode())) {
				if(divAnalysisDebugLevel>=1) printf("   case i = j / k\n");
			
				// Both Bottom
				if(arg1Lat->getLevel() == DivLattice::bottom || arg2Lat->getLevel() == DivLattice::bottom)
					modified = resLat->setBot() || modified;
				// Both ValKnown
				else if(arg1Lat->getLevel() == DivLattice::valKnown && arg2Lat->getLevel() == DivLattice::valKnown)
					modified = resLat->set(arg1Lat->getValue() / arg2Lat->getValue()) || modified;
				// Arg1 ValKnown, Arg2 DivKnown, Arg1 divisible by Arg2 Div
				else if(arg1Lat->getLevel() == DivLattice::valKnown && arg2Lat->getLevel() == DivLattice::divKnown &&
				        arg2Lat->getRem()==0 && arg1Lat->getValue()%arg2Lat->getDiv() == 0)
					modified = resLat->set(arg1Lat->getValue()/arg2Lat->getDiv()) || modified;
				// Arg1 DivKnown, Arg2 ValKnown, Arg1 Div/Rem divisible by Arg2
				else if(arg1Lat->getLevel() == DivLattice::divKnown && arg2Lat->getLevel() == DivLattice::valKnown &&
				        (arg1Lat->getDiv()%arg2Lat->getValue() == 0) && (arg1Lat->getRem()%arg2Lat->getValue() == 0))
					modified = resLat->set(arg1Lat->getDiv()/arg2Lat->getValue(), arg1Lat->getRem()/arg2Lat->getValue()) || modified;
				// Both DivKnown, Arg1 Div/Rem divisible by Arg2 Div
				else if(arg1Lat->getLevel() == DivLattice::divKnown && arg2Lat->getLevel() == DivLattice::divKnown &&
				        arg2Lat->getRem()==0 && (arg1Lat->getDiv()%arg2Lat->getDiv() == 0) && (arg1Lat->getRem()%arg2Lat->getDiv() == 0))
					modified = resLat->set(arg1Lat->getDiv()/arg2Lat->getDiv(), arg1Lat->getRem()/arg2Lat->getDiv()) || modified;
				// Else => Top
				else
					modified = resLat->setTop() || modified;
			// MODULUS
			} else if(isSgModAssignOp(n.getNode()) || isSgModOp(n.getNode())) {
				if(divAnalysisDebugLevel>=1) printf("   case i = j %% k\n");
				
				// Both Bottom
				if(arg1Lat->getLevel() == DivLattice::bottom || arg2Lat->getLevel() == DivLattice::bottom)
					modified = resLat->setBot() || modified;
				// Both ValKnown
				else if(arg1Lat->getLevel() == DivLattice::valKnown && arg2Lat->getLevel() == DivLattice::valKnown)
					modified = resLat->set(arg1Lat->getValue() % arg2Lat->getValue()) || modified;
				// Arg1 ValKnown, Arg2 DivKnown
				else if(arg1Lat->getLevel() == DivLattice::valKnown && arg2Lat->getLevel() == DivLattice::divKnown &&
				        arg2Lat->getRem()==0 && arg1Lat->getValue()%arg2Lat->getDiv() == 0)
					modified = resLat->setTop() || modified;
				// Arg1 DivKnown, Arg2 ValKnown, Arg1 Div divisible by Arg2
				else if(arg1Lat->getLevel() == DivLattice::divKnown && arg2Lat->getLevel() == DivLattice::valKnown &&
				        (arg1Lat->getDiv()%arg2Lat->getValue() == 0))
					modified = resLat->set(arg1Lat->getRem() % arg2Lat->getValue()) || modified;
				// Both DivKnown
				else if(arg1Lat->getLevel() == DivLattice::divKnown && arg2Lat->getLevel() == DivLattice::divKnown &&
				        arg2Lat->getRem()==0 && (arg1Lat->getDiv()%arg2Lat->getDiv() == 0) && (arg1Lat->getRem()%arg2Lat->getDiv() == 0))
					modified = resLat->setTop() || modified;
				// Else => Top
				else
					modified = resLat->setTop() || modified;
			}
		
			// If there is a left-hand side, copy the final lattice to the lhs variable
			if(isSgPlusAssignOp(n.getNode()) || isSgMinusAssignOp(n.getNode()) ||
				isSgMultAssignOp(n.getNode()) || isSgDivAssignOp(n.getNode()) ||
				isSgModAssignOp(n.getNode())) {
				// If we didn't use the lhs lattice as resLat, copy resLat into lhsLat
				//cout << "prodLat->getVarLattice("<<res.str()<<")="<<prodLat->getVarLattice(res)<<"\n";
				if(prodLat->getVarLattice(res)!=NULL) {
					DivLattice* lhsLat = dynamic_cast<DivLattice*>(prodLat->getVarLattice(lhs));
					//cout << "prodLat->getVarLattice("<<lhs.str()<<")="<<lhsLat<<"\n";
					if(lhsLat) // If the left-hand-side contains an identifiable variable
						lhsLat->copy(resLat);
				}
			}
		}
		
		// Deallocate newly-created objects
		if(isSgMinusMinusOp(n.getNode()) || isSgPlusPlusOp(n.getNode()))
			delete arg2Lat;
	} else
		return false;
	//cout << "transfer C\n";
	
	return modified;
}

// prints the Lattices set by the given DivAnalysis 
void printDivAnalysisStates(DivAnalysis* da, string indent)
{
	vector<int> factNames;
	vector<int> latticeNames;
	latticeNames.push_back(0);
	printAnalysisStates pas(da, factNames, latticeNames, printAnalysisStates::below, indent);
	UnstructuredPassInterAnalysis upia_pas(pas);
	upia_pas.runAnalysis();
}
