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

	// If this object is uninitialized, just copy the state of that
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
//cout << "final: " << str("") << "\n";

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
	if(level == uninitialized && that.level == uninitialized)
	{ return false; }
	else if(level == bottom || that.level == bottom)
	{ return setBot(); }
	else if(level == bottom && that.level == bottom)
	{ return false; }
	else if(that.level == top || that.level == bottom)
		return setTop();
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

// Decrements the state of this object by value
// returns true if this causes the lattice's state to change, false otherwise
bool SgnLattice::minus(long decrement)
{
	return plus(0-decrement);
}

// Decrements the state of this object by the contents of that
// returns true if this causes the lattice's state to change, false otherwise
bool SgnLattice::minus(const SgnLattice& that)
{
	if(level == uninitialized && that.level == uninitialized)
	{ return false; }
	else if(level == bottom || that.level == bottom)
	{ return setBot(); }
	else if(level == bottom && that.level == bottom)
	{ return false; }
	else if(that.level == top || that.level == bottom)
		return setTop();
	else if(level == eqZero)
	{
		if(that.level == sgnKnown)
		{
			ROSE_ASSERT(that.sgnState == posZero || that.sgnState == negZero);
			if(that.sgnState == posZero) return setSgnKnown(negZero);
			else                         return setSgnKnown(posZero);
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

// Negates the state of the object
// returns true if this causes the lattice's state to change, false otherwise
bool SgnLattice::negate()
{
	if(level == sgnKnown)
	{
		ROSE_ASSERT(sgnState == posZero || sgnState == negZero);
		if(sgnState == posZero) return setSgnKnown(negZero);
		else                    return setSgnKnown(posZero);
	}
	
	return false;
}

// Multiplies and/or divides the state of this object by value
// returns true if this causes the lattice's state to change, false otherwise
bool SgnLattice::multdiv(long multiplier)
{
	if(level == sgnKnown)
	{
		// if the multiplier is negative, flip the sign
		if(multiplier < 0)
		{
			if(sgnState==posZero) return setSgnKnown(negZero);
			else if(sgnState==negZero) return setSgnKnown(posZero);
		}
	} else if(level == bottom)
		return setTop();
	
	return false;
}

// Multiplies and/or divides the state of this object by the contents of that
// returns true if this causes the lattice's state to change, false otherwise
bool SgnLattice::multdiv(const SgnLattice& that)
{
	if(level == uninitialized && that.level == uninitialized)
	{ return false; }
	else if(level == bottom || that.level == bottom)
	{ return setBot(); }
	else if(level == bottom && that.level == bottom)
	{ return false; }
	else if(that.level == top || that.level == bottom)
		return setTop();
	else if(that.level == eqZero)
		return setEqZero();
	else if(level == sgnKnown)
	{
		// if the multiplier is negative, flip the sign
		if(that.level==sgnKnown && that.sgnState==negZero)
		{
			if(sgnState==posZero) return setSgnKnown(negZero);
			else if(sgnState==negZero) return setSgnKnown(posZero);
		}
	}
	return false;
}

// Applies a generic complex operation to this and that objects, storing the results in this object
// returns true if this causes the lattice's state to change, false otherwise
bool SgnLattice::complexOp(const SgnLattice& that)
{
	if(level == uninitialized && that.level == uninitialized)
	{ return false; }
	else if(level == bottom || that.level == bottom)
	{ return setBot(); }
	else if(level == bottom && that.level == bottom)
	{ return false; }
	else
		return setTop();
}
	
string SgnLattice::str(string indent)
{
	ostringstream outs;
	//printf("SgnLattice::str() level=%d\n", level);
	if(level == uninitialized)
		outs << "<SgnLattice: uninitialized>";
	else if(level == bottom)
		outs << "<SgnLattice: bottom>";
	else if(level == eqZero)
		outs << "<SgnLattice: eqZero>";
	else if(level == sgnKnown)
		outs << "<SgnLattice: "<<(sgnState==unknown?"unknown":(sgnState==posZero?"posZero":(sgnState==negZero?"negZero":"???")))<<">";
	else if(level == top)
		outs << "<SgnLattice: top>";
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
	//printf("SgnAnalysis::genInitState() n=%p<%s | %s>\n", n.getNode(), n.getNode()->class_name().c_str(), n.getNode()->unparseToString().c_str());
	/*printf("SgnAnalysis::genInitState() state = %p\n", &state);*/
	
	//vector<Lattice*> initLattices;
	//map<varID, Lattice*> constVars;
	//FiniteVariablesProductLattice* l = new FiniteVariablesProductLattice(true, false, new SgnLattice(), constVars, NULL, func, n, state);
	map<varID, Lattice*> emptyM;
	FiniteVarsExprsProductLattice* l = new FiniteVarsExprsProductLattice((Lattice*)new SgnLattice(), emptyM, (Lattice*)NULL, ldva, n, state);
	initLattices.push_back(l);
	/*cout << "    #initLattices="<<initLattices.size()<<":\n";
	for(vector<Lattice*>::const_iterator it=initLattices.begin(); it!=initLattices.end(); it++) {
		cout << "        "<<(*it)<<"\n"; cout.flush();
		cout << "        "<<(*it)->str("            ")<<"\n"; cout.flush();
	}*/
	//return initLattices;
}

bool SgnAnalysis::transfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo)
{
	bool modified=false;
	
	//printf("SgnAnalysis::transfer() n=%p<%s | %s>\n", n.getNode(), n.getNode()->class_name().c_str(), n.getNode()->unparseToString().c_str());
	/*cout << "   #dfInfo="<<dfInfo.size()<<": \n";
	for(vector<Lattice*>::const_iterator it=dfInfo.begin(); it!=dfInfo.end(); it++) {
		cout << "        "<<(*it)->str("            ")<<"\n"; cout.flush();
	}*/
	//FiniteVariablesProductLattice* prodLat = dynamic_cast<FiniteVariablesProductLattice*>(*(dfInfo.begin()));
	FiniteVarsExprsProductLattice* prodLat = dynamic_cast<FiniteVarsExprsProductLattice*>(*(dfInfo.begin()));
	
	// Make sure that all the non-constant Lattices are initialized
	const vector<Lattice*>& lattices = prodLat->getLattices();
	for(vector<Lattice*>::const_iterator it = lattices.begin(); it!=lattices.end(); it++)
		(dynamic_cast<SgnLattice*>(*it))->initialize();
	
//	printf("SgnAnalysis::transfer\n");
// Plain assignment: lhs = rhs
	if(isSgAssignOp(n.getNode())) {
		varID res = SgExpr2Var(isSgExpression(n.getNode()));
		varID lhs = SgExpr2Var(isSgAssignOp(n.getNode())->get_lhs_operand());
		varID rhs = SgExpr2Var(isSgAssignOp(n.getNode())->get_rhs_operand());
		if(sgnAnalysisDebugLevel>=1) {
			cout << "res="<<res.str()<<" lhs="<<lhs.str()<<" rhs="<<rhs.str()<<"\n";
		}
		
		SgnLattice* resLat = dynamic_cast<SgnLattice*>(prodLat->getVarLattice(res));
		SgnLattice* lhsLat = dynamic_cast<SgnLattice*>(prodLat->getVarLattice(lhs));
		SgnLattice* rhsLat = dynamic_cast<SgnLattice*>(prodLat->getVarLattice(rhs));
		
		if(sgnAnalysisDebugLevel>=1) {
			if(resLat) cout << "resLat=\n    "<<resLat->str("    ")<<"\n";
			if(lhsLat) cout << "lhsLat=\n    "<<lhsLat->str("    ")<<"\n";
			if(rhsLat) cout << "rhsLat=\n    "<<rhsLat->str("    ")<<"\n";
		}
		
		// Copy the lattice of the right-hand-side to both the left-hand-side variable and to the assignment expression itself
		if(resLat) // If the left-hand-side contains a live expression or variable
		{ resLat->copy(rhsLat); modified=true; }
		if(lhsLat) // If the left-hand-side contains a live expression or variable
		{ lhsLat->copy(rhsLat); modified=true; }
	// Initializer for a variable
	} else if(isSgAssignInitializer(n.getNode())) {
		varID res = SgExpr2Var(isSgAssignInitializer(n.getNode()));
		varID asgn = SgExpr2Var(isSgAssignInitializer(n.getNode())->get_operand());

		SgnLattice* asgnLat = dynamic_cast<SgnLattice*>(prodLat->getVarLattice(asgn));
		SgnLattice* resLat = dynamic_cast<SgnLattice*>(prodLat->getVarLattice(res));
		if(sgnAnalysisDebugLevel>=1) {
			if(asgnLat) cout << "asgn="<<asgn<<" Lat=    "<<asgnLat->str("    ")<<"\n";
			if(resLat) cout << "res="<<res<<" Lat=    "<<resLat->str("    ")<<"\n";
		}

		// If the result expression is live
		if(resLat) { resLat->copy(asgnLat); modified=true; }
	// Variable Declaration
	} else if(isSgInitializedName(n.getNode())) {
		SgInitializedName* initName = isSgInitializedName(n.getNode());
		varID var(initName);
		SgnLattice* varLat = dynamic_cast<SgnLattice*>(prodLat->getVarLattice(var));
		
		//cout << "DivAnalysis::transfer() isSgInitializedName var="<<var.str()<<" varLat="<<varLat<<"\n";
		
		// if this is a scalar that we care about, initialize it to Bottom
		if(varLat)
		{
			//if(sgnAnalysisDebugLevel>=1) cout << "Variable declaration: "<<var.str()<<", get_initializer()="<<initName->get_initializer()<<"\n";
			// If there was no initializer
			if(initName->get_initializer()==NULL)
				modified = varLat->setBot() || modified;
			else {
				varID init = SgExpr2Var(initName->get_initializer());
				SgnLattice* initLat = dynamic_cast<SgnLattice*>(prodLat->getVarLattice(init));
				//if(sgnAnalysisDebugLevel>=1) cout << "    init="<<init.str()<<" initLat="<<initLat<<"\n";
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
		SgnLattice* resLat = dynamic_cast<SgnLattice*>(prodLat->getVarLattice(res));
		
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
		SgnLattice* resLat = dynamic_cast<SgnLattice*>(prodLat->getVarLattice(res));
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
		SgnLattice *resLat, *arg1Lat, *arg2Lat;
		
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
			arg1Lat = dynamic_cast<SgnLattice*>(prodLat->getVarLattice(arg1));
			arg2Lat = dynamic_cast<SgnLattice*>(prodLat->getVarLattice(arg2));
		} else if(isSgUnaryOp(n.getNode())) {
			arg1 = SgExpr2Var(isSgUnaryOp(n.getNode())->get_operand());
			arg1Lat = dynamic_cast<SgnLattice*>(prodLat->getVarLattice(arg1));
			// Unary Update
			if(isSgMinusMinusOp(n.getNode()) || isSgPlusPlusOp(n.getNode())) {
				arg2Lat = new SgnLattice(1);
			}
			//cout << "res="<<res.str()<<" arg1="<<arg1.str()<<" arg1Lat="<<arg1Lat<<", arg2Lat="<<arg2Lat<<"\n";
		}
		resLat = dynamic_cast<SgnLattice*>(prodLat->getVarLattice(res));
		//cout << "transfer B, resLat="<<resLat<<"\n";
		
		// If the result expression is dead but the left-hand-side of the expression is live,
		// update the left-hand-side with the result
		if(resLat==NULL && 
			(isSgPlusAssignOp(n.getNode()) || isSgMinusAssignOp(n.getNode()) ||
			 isSgMultAssignOp(n.getNode()) || isSgDivAssignOp(n.getNode()) ||
			 isSgModAssignOp(n.getNode())) &&
			prodLat->getVarLattice(lhs)!=NULL)
		{ resLat = dynamic_cast<SgnLattice*>(prodLat->getVarLattice(lhs)); }
		
		//cout << "transfer C, resLat="<<resLat<<"\n";
		// If the result or left-hand-side expression as well as the arguments are live
		if(resLat && arg1Lat && arg2Lat) {
			// ADDITION / SUBTRACTION
			if(isSgPlusAssignOp(n.getNode())  || isSgAddOp(n.getNode()) ||
		      isSgMinusAssignOp(n.getNode()) || isSgMinusOp(n.getNode()) ||
		      isSgMinusMinusOp(n.getNode())  || isSgPlusPlusOp(n.getNode())) {
		      // Addition
				if(isSgPlusAssignOp(n.getNode()) || isSgAddOp(n.getNode()) || isSgPlusPlusOp(n.getNode())) {
					resLat->copy(arg1Lat); modified=true;
					modified = resLat->plus(*arg2Lat) || modified;
				// Subtraction
				} else {
					resLat->copy(arg1Lat); modified=true;
					modified = resLat->minus(*arg2Lat) || modified;
				}
			// Negation
			} else if(isSgMinusOp(n.getNode())) {
				resLat->copy(arg1Lat); modified=true;
				modified = resLat->negate() || modified;
			// PLUS SIGN
			} else if(isSgUnaryAddOp(n.getNode())) {
				resLat->copy(arg1Lat); modified=true;
			// MULTIPLICATION
			} else if(isSgMultAssignOp(n.getNode()) || isSgMultiplyOp(n.getNode())) {
				if(sgnAnalysisDebugLevel>=1) printf("   case i = j * k\n");
				/*printf("arg1Lat = %s\n", arg1Lat->str().c_str());
				printf("arg2Lat = %s\n", arg2Lat->str().c_str());*/
				resLat->copy(arg1Lat); modified=true;
				modified = resLat->multdiv(*arg2Lat) || modified;
			// DIVISION
			} else if(isSgDivAssignOp(n.getNode()) || isSgDivideOp(n.getNode())) {
				if(sgnAnalysisDebugLevel>=1) printf("   case i = j / k\n");
			
				resLat->copy(arg1Lat); modified=true;
				modified = resLat->multdiv(*arg2Lat) || modified;
			// MODULUS
			} else if(isSgModAssignOp(n.getNode()) || isSgModOp(n.getNode())) {
				if(sgnAnalysisDebugLevel>=1) printf("   case i = j %% k\n");
				
				resLat->copy(arg1Lat); modified=true;
				modified = resLat->complexOp(*arg2Lat) || modified;
			}
		
			// If there is a left-hand side, copy the final lattice to the lhs variable
			if(isSgPlusAssignOp(n.getNode()) || isSgMinusAssignOp(n.getNode()) ||
				isSgMultAssignOp(n.getNode()) || isSgDivAssignOp(n.getNode()) ||
				isSgModAssignOp(n.getNode())) {
				// If we didn't use the lhs lattice as resLat, copy resLat into lhsLat
				//cout << "prodLat->getVarLattice("<<res.str()<<")="<<prodLat->getVarLattice(res)<<"\n";
				if(prodLat->getVarLattice(res)!=NULL) {
					SgnLattice* lhsLat = dynamic_cast<SgnLattice*>(prodLat->getVarLattice(lhs));
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
	
	return modified;
	//cout << "transfer C\n";
	
	// // Make sure that all the non-constant Lattices are initialized
	// const vector<Lattice*>& lattices = prodLat->getLattices();
	// for(vector<Lattice*>::const_iterator it = lattices.begin(); it!=lattices.end(); it++)
	// {
	// 	// !!! Now initialize() is a noop on initialized lattices
	// 	if((dynamic_cast<SgnLattice*>(*it))->getLevel() == SgnLattice::uninitialized)
	// 		(dynamic_cast<SgnLattice*>(*it))->initialize();
	// }
	// 
	// //SgNode* asgn;
	// if(cfgUtils::isAssignment(n.getNode()))
	// {
	// 	varID i, j, k;
	// 	bool negJ, negK;
	// 	long c;
	// 	short op;
	// 	if(cfgUtils::parseAssignment(n.getNode(), op, i, j, negJ, k, negK, c))
	// 	{
	// 		/*printf("cfgUtils::parseAssignment(asgn, op, i, j, c) = 1\n");
	// 		printf("        op=%s i=%s, j=%s%s, k=%s%s, c=%d\n", 
	// 		       (op==cfgUtils::add? "+": (op==cfgUtils::subtract? "-": (op==cfgUtils::mult? "*": (op==cfgUtils::divide? "/": (op==cfgUtils::none? "~": "???"))))),
	// 		       i.str().c_str(), negJ?"-":"", j.str().c_str(), negK?"-":"", k.str().c_str(), c);*/
	// 		
	// 		SgnLattice* iLattice = dynamic_cast<SgnLattice*>(prodLat->getVarLattice(func, i));
	// 		SgnLattice newILattice;
	// 		
	// 		if(op==cfgUtils::none)
	// 		{
	// 			// i = c
	// 			if(j==zeroVar && k==zeroVar)
	// 				newILattice.set(c);
	// 			// i = j;
	// 			else if(k==zeroVar)
	// 			{
	// 				SgnLattice* jLattice = dynamic_cast<SgnLattice*>(prodLat->getVarLattice(func, j));
	// 				newILattice.copy(jLattice);
	// 				newILattice.mult(negJ?-1:1);
	// 			}
	// 		}
	// 		else if(op==cfgUtils::add)	
	// 		{
	// 			if(sgnAnalysisDebugLevel>=1){
	// 							printf("          add, k==zeroVar = %d\n", k==zeroVar);
	// 			}
	// 			// i = j + c
	// 			if(k==zeroVar)
	// 			{
	// 				SgnLattice* jLattice = dynamic_cast<SgnLattice*>(prodLat->getVarLattice(func, j));
	// 				newILattice.copy(jLattice);
	// 				newILattice.mult(negJ?-1:1);
	// 				newILattice.plus(c);
	// 			}
	// 			// i = j + k + c
	// 			else
	// 			{
	// 				SgnLattice* jLattice = dynamic_cast<SgnLattice*>(prodLat->getVarLattice(func, j));
	// 				SgnLattice* kLattice = dynamic_cast<SgnLattice*>(prodLat->getVarLattice(func, k));
	// 				
	// 				newILattice.copy(jLattice);
	// 				newILattice.mult(negJ?-1:1);
	// 				newILattice.plus(*kLattice);
	// 				newILattice.mult(negK?-1:1);
	// 				newILattice.plus(c);
	// 			}
	// 		}
	// 		else if(op==cfgUtils::mult || op==cfgUtils::divide)	
	// 		{
	// 			if(sgnAnalysisDebugLevel>=1){
	// 							printf("operation: mult\n");
	// 			}
	// 			// i = j * c OR i = j / c
	// 			if(k==oneVar)
	// 			{
	// 				SgnLattice* jLattice = dynamic_cast<SgnLattice*>(prodLat->getVarLattice(func, j));
	// 				newILattice.copy(jLattice);
	// 				newILattice.mult(negJ?-1:1);
	// 				newILattice.mult(c);
	// 			}
	// 			// i = j * k * c OR i = j / k / c
	// 			else
	// 			{
	// 				if(sgnAnalysisDebugLevel>=1){
	// 									printf("   case i = j * k * c\n");
	// 				}
	// 				SgnLattice* jLattice = dynamic_cast<SgnLattice*>(prodLat->getVarLattice(func, j));
	// 				SgnLattice* kLattice = dynamic_cast<SgnLattice*>(prodLat->getVarLattice(func, k));
	// 				
	// 				newILattice.copy(jLattice);
	// 				newILattice.mult(negJ?-1:1);
	// 				newILattice.mult(*kLattice);
	// 				newILattice.mult(negK?-1:1);
	// 				newILattice.mult(c);
	// 			}
	// 		}
	// 		modified = iLattice->copyMod(&newILattice) || modified;
	// 	}
	// }
	// else if(isSgInitializedName(n.getNode()))
	// {
	// 	SgInitializedName* initName = isSgInitializedName(n.getNode());
	// 	varID var(initName);
	// 	
	// 	SgnLattice* varLattice = dynamic_cast<SgnLattice*>(prodLat->getVarLattice(func, var));
	// 	
	// 	// if this is a scalar that we care about
	// 	if(varLattice)
	// 	{
	// 		if(sgnAnalysisDebugLevel>=1){
	// 					printf("Variable declaration: %s\n", var.str().c_str());
	// 		}
	// 		modified = varLattice->setBot() || modified;
	// 	}
	// }
	// 
	// return modified;
}

// prints the Lattices set by the given SgnAnalysis 
void printSgnAnalysisStates(SgnAnalysis* sa, string indent)
{
	vector<int> factNames;
	vector<int> latticeNames;
	latticeNames.push_back(0);
	printAnalysisStates pas(sa, factNames, latticeNames, printAnalysisStates::below, indent);
	UnstructuredPassInterAnalysis upia_pas(pas);
	upia_pas.runAnalysis();
}
