// #################################################
// ############## CONSTRAINT GRAPHS ################
// #################################################
//#define DEBUG_FLAG_TC
#include "ConstrGraph.h"
#include <sys/time.h>

using namespace cfgUtils;

static int debugLevel=0;
static int profileLevel=1;

/**** Constructors & Destructors ****/
ConstrGraph::ConstrGraph(bool initialized) : 
	arrays(*(new varIDSet())), scalars(*(new varIDSet())), func()
{
	constrChanged=false;
	
	// by default a new constraint graph is set to uninitialized or initialized but not =bottom
	bottom = false;
		
	this->initialized = initialized;
	
	inTransaction=false;
	
	/*this->divL = NULL;
	this->sgnL = NULL;*/
}

ConstrGraph::ConstrGraph(Function func, FiniteVariablesProductLattice* divL, FiniteVariablesProductLattice* sgnL, bool initialized) : 
	      arrays(getVisibleArrays(func)), scalars(getVisibleScalars(func)), func(func)
{
//		std::cout << "ConstrGraph arrays.size()="<<arrays.size()<<"\n";
	constrChanged=false;
	
	// by default a new constraint graph is set to uninitialized or initialized but not =bottom
	bottom = false;
	
	for(varIDSet::iterator curArray = this->arrays.begin(); 
		 curArray != this->arrays.end(); curArray++)
		emptyRange[*curArray] = false;

	// Initially, all variables are "modified"
	for(varIDSet::iterator curArray = this->arrays.begin(); 
		 curArray != this->arrays.end(); curArray++)
	{
		newConstrVars.insert(*curArray);
		modifiedVars.insert(*curArray);
	}
	for(varIDSet::iterator curScalar = this->scalars.begin(); 
		 curScalar != this->scalars.end(); curScalar++)
	{
		newConstrVars.insert(*curScalar);
		modifiedVars.insert(*curScalar);
	}
		
	// add the default constraints: 0 = 1 - 1
	/*setVal(zeroVar, oneVar, 1, 1, -1);
	setVal(oneVar, zeroVar, 1, 1, 1);*/
	
	this->initialized = initialized;
	
	inTransaction=false;
	
	/*this->divL = divL;
	this->sgnL = sgnL;*/
	pair<string, void*> noAnnot("", NULL);
	this->divL[noAnnot] = divL;
	this->sgnL[noAnnot] = sgnL;
}

ConstrGraph::ConstrGraph(Function func, const map<pair<string, void*>, FiniteVariablesProductLattice*>& divL, 
	                      const map<pair<string, void*>, FiniteVariablesProductLattice*>& sgnL, bool initialized) : 
	      arrays(getVisibleArrays(func)), scalars(getVisibleScalars(func)), func(func)
{
//		std::cout << "ConstrGraph arrays.size()="<<arrays.size()<<"\n";
	constrChanged=false;
	
	// by default a new constraint graph is set to uninitialized or initialized but not =bottom
	bottom = false;
	
	for(varIDSet::iterator curArray = this->arrays.begin(); 
		 curArray != this->arrays.end(); curArray++)
		emptyRange[*curArray] = false;

	// Initially, all variables are "modified"
	for(varIDSet::iterator curArray = this->arrays.begin(); 
		 curArray != this->arrays.end(); curArray++)
	{
		newConstrVars.insert(*curArray);
		modifiedVars.insert(*curArray);
	}
	for(varIDSet::iterator curScalar = this->scalars.begin(); 
		 curScalar != this->scalars.end(); curScalar++)
	{
		newConstrVars.insert(*curScalar);
		modifiedVars.insert(*curScalar);
	}
		
	// add the default constraints: 0 = 1 - 1
	/*setVal(zeroVar, oneVar, 1, 1, -1);
	setVal(oneVar, zeroVar, 1, 1, 1);*/
	
	this->initialized = initialized;
	
	inTransaction=false;
	
	this->divL = divL;
	this->sgnL = sgnL;	
}

ConstrGraph::ConstrGraph(const varIDSet& scalars, const varIDSet& arrays, bool initialized) : 
	arrays(*(new varIDSet(arrays))), scalars(*(new varIDSet(scalars))), func()
{
	constrChanged=false;
	
	// by default a new constraint graph is set to uninitialized or initialized but not =bottom
	bottom = false;
	
	for(varIDSet::iterator curArray = this->arrays.begin(); 
		 curArray != this->arrays.end(); curArray++)
		emptyRange[*curArray] = false;

	// Initially, all variables are "modified"
	for(varIDSet::iterator curArray = this->arrays.begin(); 
		 curArray != this->arrays.end(); curArray++)
	{
		newConstrVars.insert(*curArray);
		modifiedVars.insert(*curArray);
	}
	for(varIDSet::iterator curScalar = this->scalars.begin(); 
		 curScalar != this->scalars.end(); curScalar++)
	{
		newConstrVars.insert(*curScalar);
		modifiedVars.insert(*curScalar);
	}
		
	this->initialized = initialized;
	
	inTransaction=false;
	
	/*this->divL = NULL;
	this->sgnL = NULL;*/
}

/*ConstrGraph::ConstrGraph(varIDSet& arrays, varIDSet& scalars, FiniteVariablesProductLattice* divL, bool initialized) : 
	      arrays(arrays), scalars(scalars)
{
//		std::cout << "ConstrGraph arrays.size()="<<arrays.size()<<"\n";
	constrChanged=false;
	
	// by default a new constraint graph is set to uninitialized or initialized but not =bottom
	bottom = false;
	
	for(varIDSet::iterator curArray = this->arrays.begin(); 
		 curArray != this->arrays.end(); curArray++)
		emptyRange[*curArray] = false;
		
	// add the default constraints: 0 = 1 - 1
	setVal(zeroVar, oneVar, 1, 1, -1);
	setVal(oneVar, zeroVar, 1, 1, 1);
	
	this->initialized = initialized;
	
	inTransaction=false;
	
	this->divL = divL;
}*/

ConstrGraph::ConstrGraph(ConstrGraph &that, bool initialized) :
         arrays(that.arrays), scalars(that.scalars), func(that.func)
{
	constrChanged=false;
	copyFrom(that);
	this->initialized = that.initialized;
	inTransaction=false;
	divL = that.divL;
	sgnL = that.sgnL;
}

ConstrGraph::ConstrGraph(const ConstrGraph* that, bool initialized) :
         arrays(that->arrays), scalars(that->scalars), func(that->func)
{
	constrChanged=false;
	copyFrom(*((ConstrGraph*)that));
	this->initialized = that->initialized;
	inTransaction=false;
	
	this->divL = that->divL;
	this->sgnL = that->sgnL;
}

// Creates a constraint graph that contains the given set of inequalities, 
//// which are assumed to correspond to just scalars
ConstrGraph::ConstrGraph(const set<varAffineInequality>& ineqs, Function func, 
                         FiniteVariablesProductLattice* divL, FiniteVariablesProductLattice* sgnL) : 
                         arrays(getVisibleArrays(func)), scalars(getVisibleScalars(func)), func(func)
{
	constrChanged=false;
	initialized=true;
	/*this->divL = divL;
	this->sgnL = sgnL;*/
	pair<string, void*> noAnnot("", NULL);
	this->divL[noAnnot] = divL;
	this->sgnL[noAnnot] = sgnL;
	inTransaction=false;
	bottom = false;
	
	for(set<varAffineInequality>::const_iterator it = ineqs.begin();
	    it!=ineqs.end(); it++)
	{
		/*scalars.insert((*it).getX());
		scalars.insert((*it).getY());*/
		
		assertCond(*it);
		
		newConstrVars.insert(it->getX());
		modifiedVars.insert(it->getX());
		newConstrVars.insert(it->getY());
		modifiedVars.insert(it->getY());
	}

	transitiveClosure();
}

ConstrGraph::ConstrGraph(const set<varAffineInequality>& ineqs, Function func,
	            const map<pair<string, void*>, FiniteVariablesProductLattice*>& divL, 
	            const map<pair<string, void*>, FiniteVariablesProductLattice*>& sgnL) : 
                         arrays(getVisibleArrays(func)), scalars(getVisibleScalars(func)), func(func)
{
	constrChanged=false;
	initialized=true;
	this->divL = divL;
	this->sgnL = sgnL;
	inTransaction=false;
	bottom = false;
	
	for(set<varAffineInequality>::const_iterator it = ineqs.begin();
	    it!=ineqs.end(); it++)
	{
		/*scalars.insert((*it).getX());
		scalars.insert((*it).getY());*/
		
		assertCond(*it);
		
		newConstrVars.insert(it->getX());
		modifiedVars.insert(it->getX());
		newConstrVars.insert(it->getY());
		modifiedVars.insert(it->getY());
	}
	
	transitiveClosure();
}

ConstrGraph::~ConstrGraph ()
{
	arrays.clear();
	emptyRange.clear();
	eraseConstraints(true);
	
	if(debugLevel>=1) cout << "Deleting ConstrGraph "<<this<<"\n";
}

// initializes this Lattice to its default state, if it is not already initialized
void ConstrGraph::initialize()
{
	/*if(!initialized)
		setToBottom();*/
	setToTop(true);
}

/***** The sets of arrays and scalars visible in the given function *****/

map<Function, varIDSet> ConstrGraph::funcVisibleArrays;
map<Function, varIDSet> ConstrGraph::funcVisibleScalars;

// returns the set of arrays visible in this function
varIDSet& ConstrGraph::getVisibleArrays(Function func)
{
	// if we haven't yet computed the visible arrays for this function
	if(funcVisibleArrays.find(func) == funcVisibleArrays.end())
	{
		varIDSet& locals = varSets::getLocalArrays(func);
		varIDSet& globals = varSets::getGlobalArrays(getProject());
		
		varIDSet lgUnion;
		for(varIDSet::iterator it = locals.begin(); it!=locals.end(); it++)
			funcVisibleArrays[func].insert(*it);
		
		for(varIDSet::iterator it = globals.begin(); it!=globals.end(); it++)
			funcVisibleArrays[func].insert(*it);
			
		// insert the control variables
		//funcVisibleScalars[func].insert(allArrays);
	}
	
	return funcVisibleArrays[func];
}

// returns the set of scalars visible in this function
varIDSet& ConstrGraph::getVisibleScalars(Function func)
{
	// if we haven't yet computed the visible arrays for this function
	if(funcVisibleScalars.find(func) == funcVisibleScalars.end())
	{
		varIDSet& locals = varSets::getLocalScalars(func);
		varIDSet& globals = varSets::getGlobalScalars(getProject());
		
		varIDSet lgUnion;
		for(varIDSet::iterator it = locals.begin(); it!=locals.end(); it++)
		{
			// insert the scalar and its divisibility variable
			funcVisibleScalars[func].insert(*it);
			funcVisibleScalars[func].insert(getDivScalar(*it));
		}
		
		for(varIDSet::iterator it = globals.begin(); it!=globals.end(); it++)
		{
			// insert the scalar and its divisibility variable
			funcVisibleScalars[func].insert(*it);
			funcVisibleScalars[func].insert(getDivScalar(*it));
		}
		
		// insert the control variables
		funcVisibleScalars[func].insert(zeroVar);
		//funcVisibleScalars[func].insert(oneVar);
	}
	
	/*printf("scalars for %s()\n", func.get_name().str());
	for(varIDSet::iterator it = funcVisibleScalars[func].begin(); it!=funcVisibleScalars[func].end(); it++)
	{
		cout << "    "<<(*it).str()<<"\n";
	}*/
	
	return funcVisibleScalars[func];
}


// for a given scalar returns the corresponding divisibility scalar
varID ConstrGraph::getDivScalar(const varID& scalar)
{
	varID divScalar("divscalar_"+scalar.str());
	return divScalar;
}

// returns true if the given variable is a divisibility scalar and false otherwise
bool ConstrGraph::isDivScalar(const varID& scalar)
{
	// its a divisibility scalar if its name begins with "divscalar_"
	return scalar.str().find("divscalar_", 0)==0;
}

// Returns a divisibility product lattice that matches the given variable
FiniteVariablesProductLattice* ConstrGraph::getDivLattice(const varID& var)
{
	for(map<pair<string, void*>, FiniteVariablesProductLattice*>::iterator itDiv=divL.begin();
		    itDiv!=divL.end(); itDiv++)
	{
		// If the current annotation matches all annotations
		if(itDiv->first.first=="" && itDiv->first.second==NULL)
			return itDiv->second;
		
		for(map<string, void*>::const_iterator itVar=var.getAnnotations().begin();
		    itVar!=var.getAnnotations().end(); itVar++)
		{
			if((itDiv->first.first=="" || itDiv->first.first==itVar->first) &&
			   (itDiv->first.second==NULL || itDiv->first.second==itVar->second))
				return itDiv->second;
		}
	}
	return NULL;
}

// Returns a sign product lattice that matches the given variable
FiniteVariablesProductLattice* ConstrGraph::getSgnLattice(const varID& var)
{
	for(map<pair<string, void*>, FiniteVariablesProductLattice*>::iterator itSgn=sgnL.begin();
		 itSgn!=sgnL.end(); itSgn++)
	{
		// If the current annotation matches all annotations
		if(itSgn->first.first=="" && itSgn->first.second==NULL)
			return itSgn->second;
		
		for(map<string, void*>::const_iterator itVar=var.getAnnotations().begin();
		    itVar!=var.getAnnotations().end(); itVar++)
		{
			if((itSgn->first.first=="" || itSgn->first.first==itVar->first) &&
			   (itSgn->first.second==NULL || itSgn->first.second==itVar->second))
				return itSgn->second;
		}
	}
	return NULL;
}

// returns whether the given variable is known in this constraint graph to be an array
bool ConstrGraph::isArray(const varID& array) const
{
	// return whether the given variable can be found in this constraint graph's set of arrays
	return arrays.find(array) != arrays.end();
}

// Adds the given variable to the scalars list, returning true if this causes
// the constraint graph to change and false otherwise.
bool ConstrGraph::addScalar(const varID& scalar)
{
	pair<varIDSet::iterator,bool> loc = scalars.insert(scalar);
	return loc.second;
}

// Removes the given variable and its divisibility scalar (if one exists) from the scalars list
// and removes any constraints that involve them. 
// Returning true if this causes the constraint graph to change and false otherwise.
bool ConstrGraph::removeScalar(const varID& scalar)
{
	bool modified=false;
	
	varID divScalar = getDivScalar(scalar);
	
	// Remove the constraints
	modified = eraseVarConstr(divScalar) || modified;
	modified = eraseVarConstr(scalar) || modified;
	
	// Remove the variables from divVariables and scalars
	modified = (divVariables.erase(divScalar) > 0) || modified;
	modified = (scalars.erase(scalar) > 0) || modified;
	
	return modified;
}

// Returns a reference to the constraint graph's set of scalars
const varIDSet& ConstrGraph::getScalars() const
{
	return scalars;
}

// Returns a modifiable reference to the constraint graph's set of scalars
varIDSet& ConstrGraph::getScalarsMod()
{
	return scalars;
}

// For each scalar variable not in noExternal create another variable that is identical except that it has a special
//    annotation that identifies it as the external view onto the value of this variable. The newly 
//    generated external variables will be set to be equal to their original counterparts
//    and will default to have no relations with any other variables, even through transitive closures. 
//    These variables are used to transfer information about variable state from one constraint graph
//    to another. Specially annotated cersions of the external variables will exist in multiple constraint 
//    graphs and will periodically be updated in other graphs from this graph based on how the relationships 
//    between the external variables and their regular counterparts change in this graph.
// If the external variables already exist, their relationships relative to their original counterparts are reset.
// Returns true if this causes the constraint graph to change, false otherwise.
bool ConstrGraph::addScalarExternals(varIDSet noExternal)
{
	bool modified = false;
	
	// Iterate through all the scalars
	for(varIDSet::iterator it=scalars.begin(); it!=scalars.end(); it++)
	{
		varID origVar = *it;
		if(noExternal.find(origVar) != noExternal.end()) continue;

		varID extVar = *it;
		// Annotate the external variable
		extVar.addAnnotation("cg_ext", 0);
		
		// The new variable is equal to the original variable
		modified = assertEq(extVar, origVar) || modified;
		
		// WE DO NOT ADD var TO scalars TO ENSURE THAT transitiveClosure DOES NOT APPLY
		// ANY INFERENCES TO var. THIS WAY THE ONLY CONSTRAINTS THAT WE'LL MAINTAIN ON var
		// WILL BE TO ITS ORIGINAL VARIABLE. THIS REDUCES THE COST OF KEEPING THESE VARIABLES.
		
		externalVars.insert(extVar);
	}
	
	return modified;
}

// Looks over all the external versions of all scalars in this constraint graph and looks for all the same variables
//    in the tgtCFG constraint graph that also have the given annotation. Then, updates the variables in that 
//    from the current relationships in this between the original scalars and their external versions. 
//    Thus, if blah <= var' in that and var' = var + 5 in this (var' is the annotated copy of var) then we'll update 
//    to blah <= var' - 5 so that now blah is related to var's current value in this constraint graph. 
// Returns true if this causes the tgtCG constraint graph to change, false otherwise.
bool ConstrGraph::updateFromScalarExternals(ConstrGraph* tgtCG, string annotName, void* annot)
{
	bool modified=false;
	
	// Iterate through all the scalars
	for(varIDSet::iterator it=scalars.begin(); it!=scalars.end(); it++)
	{
		varID origVar = *it;
		varID extVarThis = *it;
		varID extVarTgt = *it;
		// Annotate the external variable
		extVarThis.addAnnotation("cg_ext", 0);
		extVarTgt.addAnnotation(annotName, annot);
		
		// If the current scalar does have an associated external 
		affineInequality* ineqThis;
		if((ineqThis = getVal(extVarThis, origVar)))
		{
			// Iterate through constraints in tgtCG and update extVar's constraints in tgtCG
			for(map<varID, map<varID, affineInequality> >::iterator itX = tgtCG->vars2Value.begin();
			    itX!=tgtCG->vars2Value.end(); itX++)
			{
				//cout << "    copyVar itX->first="<<itX->first.str()<<"\n";
				// extVar <= constraints
				if(itX->first == extVarTgt)
				{
					// Update all the extVar <= y constraints in tgtCG
					for(map<varID, affineInequality>::iterator itY = itX->second.begin();
					    itY != itX->second.end(); itY++)
					{
						affineInequality& ineqTgt = itY->second;
						
						//      ineqThis                                ineqTgt
						// extVar*a = origVar*b + c          && extVar*a' <= y*b' + c'
						// extVar*a*a' = origVar*b*a' + c*a' && extVar*a'*a <= y*b'*a + c'*a
						//                origVar*b*a' + c*a' <= y*b'*a + c'*a
						//                origVar*b*a' <= y*b'*a + c'*a - c*a'
						// ==> in ineqTgt: extVar*b*a' <= y*b'*a + c'*a - c*a'
						modified = ineqTgt.set(ineqThis->getB()*ineqTgt.getA(), ineqTgt.getB()*ineqThis->getA(),
						                       ineqTgt.getC()*ineqThis->getA() - ineqThis->getC()*ineqTgt.getA(), 
						                       ineqThis->isXZero(), ineqTgt.isYZero(), ineqThis->getXSign(), ineqTgt.getYSign()) || modified;
					
						modifiedVars.insert(itY->first);
						newConstrVars.insert(itY->first);
					}
					modifiedVars.insert(itX->first);
					newConstrVars.insert(itX->first);
				}
				// x <= extVar constraints
				else
				{
					// Update all the x <= extVar constraints in tgtCG
					for(map<varID, affineInequality>::iterator itY = itX->second.begin();
					    itY!=itX->second.end(); itY++)
					{
						affineInequality& ineqTgt = itY->second;
						if(itY->first == extVarTgt)
						{
							//      ineqThis                                ineqTgt
							// extVar*a = origVar*b + c          && y*a' <= extVar*b' + c'
							// extVar*a*b' = origVar*b*b' + c*b' && y*a'*a <= extVar*b'*a + c'*a
							//                y*a'*a <= origVar*b*b' + c*b' + c'*a
							// ==> in ineqTgt: y*a'*a <= extVar*b*b' + c*b' + c'*a
							modified = ineqTgt.set(ineqThis->getA()*ineqTgt.getA(), ineqTgt.getB()*ineqThis->getB(),
							                       ineqTgt.getB()*ineqThis->getC() - ineqThis->getA()*ineqTgt.getC(), 
							                       ineqTgt.isXZero(), ineqThis->isYZero(), ineqTgt.getXSign(), ineqThis->getYSign()) || modified;
							modifiedVars.insert(itY->first);
							newConstrVars.insert(itY->first);
						}
						modifiedVars.insert(itX->first);
						newConstrVars.insert(itX->first);
					}
				}
			}
		}
	}
  return modified;
}

// Returns a reference to the constraint graph's set of scalars
const varIDSet& ConstrGraph::getExternals() const
{
	return externalVars;
}

/***** Copying *****/

// overwrites the state of this Lattice with that of that Lattice
void ConstrGraph::copy(Lattice* that)
{
	copyFrom(*(dynamic_cast<ConstrGraph*>(that)));
}

// returns a copy of this lattice
Lattice* ConstrGraph::copy() const
{
	return new ConstrGraph(this);
}

// returns a copy of this LogicalCond object
LogicalCond* ConstrGraph::copy()
{
	return new ConstrGraph(this);
}

// copies the state of cg to this constraint graph
// returns true if this causes this constraint graph's state to change
bool ConstrGraph::copyFrom(ConstrGraph &that)
{
	bool modified = (bottom != that.bottom);
	bottom = that.bottom;

if(debugLevel>=1)
{	
	if(that.arrays != arrays)
	{
		cout << "!!!!!ConstrGraph::copyFrom() Different arrays:\n";
		cout << "    arrays=";
		for(set<varID>::iterator it=arrays.begin(); it!=arrays.end(); it++)
		{ cout << (*it).str() << " "; }
		cout << "\n";
		
		cout << "    that.arrays=";
		for(set<varID>::iterator it=that.arrays.begin(); it!=that.arrays.end(); it++)
		{ cout << (*it).str() << " "; }
		cout << "\n";
	}
	
	if(that.scalars!= scalars)
	{
		cout << "!!!!!ConstrGraph::copyFrom() Different scalars:\n";
		cout << "    scalars=";
		for(set<varID>::iterator it=scalars.begin(); it!=scalars.end(); it++)
		{ cout << (*it).str() << " "; }
		cout << "\n";
		
		cout << "    that.scalars=";
		for(set<varID>::iterator it=that.scalars.begin(); it!=that.scalars.end(); it++)
		{ cout << (*it).str() << " "; }
		cout << "\n";
	}
	fflush(stdout);
}

	
	// copy the array information from cg to this
	modified = copyArrays(that) || modified;
	
	// copy the constraint information from cg to this		
	modified = copyConstraints(that) || modified;
	
	modifiedVars = that.modifiedVars;
	newConstrVars = that.newConstrVars;
	
/*	modified = (scalars != that.scalars) || modified;
	scalars == that.scalars;
	
	modified = (divVariables != that.divVariables) || modified;
	scalars == that.divVariables;
	
	modified = (arrays != that.arrays) || modified;
	arrays == that.arrays;*/

	// copy the constrChanged from cg since the state of this ConstrGraph
	// object is a direct copy of cg, including the upto-date-ness of the 
	// bottom flag
	constrChanged=that.constrChanged;
	
	return  modified;
}

// Update the state of this constraint graph from that constraint graph, leaving 
//    this graph's original contents alone and only modifying the pairs that are in that.
// Returns true if this causes this constraint graph's state to change
bool ConstrGraph::updateFrom(ConstrGraph &that)
{
	
}

// Copies the given var and its associated constrants from that to this.
// Returns true if this causes this constraint graph's state to change; false otherwise.
bool ConstrGraph::copyVar(const ConstrGraph& that, const varID& var)
{
	//printf("ConstrGraph::copyVar(var=%s)\n", var.str().c_str());
	// Add var to scalars if it isn't already there.
	if(scalars.find(var) == scalars.end())
		scalars.insert(var);
	
	// Iterate over all the var<=x and	x<=var pairs.
	for(map<varID, map<varID, affineInequality> >::const_iterator itX = that.vars2Value.begin();
	    itX!=that.vars2Value.end(); itX++)
	{
		//cout << "    copyVar itX->first="<<itX->first.str()<<"\n";
		// var <= constraints
		if(itX->first == var)
		{
			// Assert in this all the var <= x constraints in that
			for(map<varID, affineInequality>::const_iterator itY = itX->second.begin();
			    itY != itX->second.end(); itY++)
			{
				//cout << "ConstrGraph::copyVar: calling assertCond1("<<itX->first.str()<<", "<<var.str()<<", "<<itY->second.str()<<")\n";
				assertCond(var, itY->first, itY->second);
			}
		}
		// x <= var constraints
		else
		{
			// Assert in this all the x <= var constraints in that
			for(map<varID, affineInequality>::const_iterator itY = itX->second.begin();
			    itY!=itX->second.end(); itY++)
			{
				if(itY->first == var)
				{
					//cout << "ConstrGraph::copyVar: calling assertCond2("<<itX->first.str()<<", "<<var.str()<<", "<<itY->second.str()<<")\n";
					assertCond(itX->first, var, itY->second);
				}
			}
		}
	}
}

/*// returns true if this and cg map the same sets of arrays and false otherwise
	bool mapsSameArrays(ConstrGraph *cg)
	{
		return containsArraySet(this, cg) && containsArraySet(cg, this);
	}

	// determines whether cg1->arrays contains cg2->arrays
	static bool containsArraySet(ConstrGraph *cg1, ConstrGraph *cg2)
	{
		// iterate over cg2->arrays to ensure that all entries in arrays appear in cg1->arrays
		for(varIDSet::iterator curArray = cg2->arrays.begin(); 
			 curArray != cg2->arrays.end(); curArray++)
		{
			// if the current array does not appear in cg1->arrays, 
			// then cg1->arrays does not contain all of cg2->arrays
			if(!cg1->isArray(*curArray))
				return true;
		}
		return false;
	}*/
	
	// determines whether cg->arrays and cg->emptyRange are different 
	// from arrays and emptyRange
	/*bool diffArrays(ConstrGraph *cg)
	{
		// iterate over arrays to ensure that all entries in arrays appear in cg->arrays
		// and they have the same mappings in emptyRange and cg->emptyRange
		for(varIDSet::iterator curArray = arrays.begin(); 
			 curArray != arrays.end(); curArray++)
		{
			// if the current array does not appear in cg->arrays or cg->emptyRange, 
			// then the array information of cg and this is different
			if(!cg->isArray(*curArray) ||
				cg->emptyRange.find(*curArray) == cg->emptyRange.end())
				return true;
			
			// if emptyRange and cg->emptyRange map *curArray to different values,
			// then the array information of cg and this is different
			if(cg->emptyRange[*curArray] != emptyRange[*curArray])
				return true;
		}
		
		// now iterate over cg->arrays to ensure that all entries in cg->arrays appear in arrays
		//    we don't need to check the mappings of emptyRange since if arrays and cg->arrays
		//    do turn out to be equal, the mappings of emptyRange must have been checked in the above loop
		for(varIDSet::iterator curArray = cg->arrays.begin(); 
			 curArray != cg->arrays.end(); curArray++)
		{
			// if the current array does not appear in arrays, 
			// then the array information of cg and this is different
			if(!isArray(*curArray))
				return true;
		}
		
		// if we haven't found any differences, both constraint graphs must have 
		//    the same arrays that are mapped to the same values in their respective
		//    emptyRange maps
		return false;
	}*/
	
// copies the data of cg->emptyRange to emptyRange and 
// returns whether this caused emptyRange to change
bool ConstrGraph::copyArrays(const ConstrGraph &that)
{
	bool modified = false;
	// we don't do copies from ConstrGraph from different functions
	//ROSE_ASSERT(that.arrays == arrays);
	
	// perform the copy
	for(varIDSet::iterator curArray = that.arrays.begin(); 
		 curArray != that.arrays.end(); curArray++)
	{
		modified = (emptyRange[*curArray] != that.emptyRange.find(*curArray)->second) || modified;
		emptyRange[*curArray] = that.emptyRange.find(*curArray)->second;
	}

	// the array information of this has changed
	return modified;
}

// updates the data of cg->emptyRange to emptyRange, leaving 
//    this graph's original emptyRange alone and only modifying the entries that are in that.
// returns whether this caused emptyRange to change
bool ConstrGraph::updateArrays(const ConstrGraph &that)
{
	bool modified = false;
	// we don't do copies from ConstrGraph from different functions
	//ROSE_ASSERT(that.arrays == arrays);
	
	// perform the update
	for(varIDSet::iterator curArray = that.arrays.begin(); 
		 curArray != that.arrays.end(); curArray++)
	{
		modified = arrays.find(*curArray)==arrays.end() || 
		           (emptyRange[*curArray] != that.emptyRange.find(*curArray)->second) || 
		           modified;
		emptyRange[*curArray] = that.emptyRange.find(*curArray)->second;
	}

	// the array information of this has changed
	return modified;
}

// determines whether constraints in cg are different from
// the constraints in this
bool ConstrGraph::diffConstraints(ConstrGraph &that)
{
//Check to see if map::operator== is sufficient for this
	// if these two constraint graphs differ on their bottom-ness
	if(isBottom()!=that.isBottom())
		return true;
	
	// if these two constraint graphs have different numbers of constraints or 
	// map different sets of variables
	if(vars2Value != that.vars2Value ||
		arrays != that.arrays ||
		scalars != that.scalars)
		return true;
	
	return false;
/*		map<varID, map<varID, constraint> >::const_iterator itThis, itThat;
	// iterate over all mapped variables in this
	for(itThis = vars2Value.begin(); itThis != vars2Value.end(); itThis++)
	{
		map<varID, constraint>::const_iterator itThisThis, itThisThat;
		// if the current mapping in this doesn't exist in that, the constraints are not equal
		if((itThisThat = that.vars2Value.find(itThis->first)) == that.vars2Value.end())
			return true;
		// both this and that have a mapping for itThis->first
		else
		{
			map<varID, constraint>
		}
	}

	// iterate over all pairs of variables in that
	for(itThat = that.vars2Value.begin(); itThat != that.vars2Value.end(); itThat++)
	{
		// if the current mapping in this doesn't exist in that, the constraints are not equal
		if(vars2Value.find(itThat->first) == vars2Value.end()
			return true;
	}
	
	// there are no differences in the constraints of these two graphs
	return false;*/
}

// copies the constraints of cg into this constraint graph
// returns true if this causes this constraint graph's state to change
bool ConstrGraph::copyConstraints(ConstrGraph &that)
{
	bool modified;
	// we don't do copies from ConstrGraph from different functions
	//ROSE_ASSERT(that.arrays == arrays);
	//ROSE_ASSERT(that.scalars == scalars);
	
	// this constraint graph will be modified if it is currently uninitialized
	modified = !initialized;
	
	modified = modified || diffConstraints(that);
	
	//vars2Value = that.vars2Value;
	// erase vars2Value
	map<varID, map<varID, affineInequality> >::iterator itX;
	for(itX = vars2Value.begin(); itX!=vars2Value.end(); itX++)
		itX->second.clear();
	vars2Value.clear();
	
	// copy vars2Value from that
	vars2Value = that.vars2Value;
	
	modifiedVars = that.modifiedVars;
	newConstrVars = that.newConstrVars;
	
	initialized = true; // this constraint graph is now definitely initialized
	
	return modified;
}

// updates the constraints of cg into this constraint graph, leaving 
//    this graph's original constraints alone and only modifying the constraints that are in that.
// returns true if this causes this constraint graph's state to change
/*bool ConstrGraph::updateConstraints(ConstrGraph &that)
{
	bool modified;
	// we don't do copies from ConstrGraph from different functions
	ROSE_ASSERT(that.arrays == arrays);
	ROSE_ASSERT(that.scalars == scalars);
	
	// this constraint graph will be modified if it is currently uninitialized
	modified = !initialized;
	
	modified = modified || diffConstraints(that);
	
	//vars2Value = that.vars2Value;
	// erase vars2Value
	map<varID, map<varID, affineInequality> >::iterator itX;
	for(itX = vars2Value.begin(); itX!=vars2Value.end(); itX++)
		itX->second.clear();
	vars2Value.clear();
	
	// copy vars2Value from that
	vars2Value = that.vars2Value;
	
	initialized = true; // this constraint graph is now definitely initialized
	
	return modified;
}*/

/***** Array Range Management *****/
// sets the ranges of all arrays in this constraint graph to not be empty
void ConstrGraph::unEmptyArrayRanges()
{
	for(varIDSet::iterator curArray = arrays.begin(); 
		 curArray != arrays.end(); curArray++)
	{
		emptyRange[*curArray] = false;
	}
	constrChanged=true;
}

// sets the ranges of all arrays in this constraint graph to empty
// noBottomCheck - flag indicating whether this function should do nothing if this isBottom() returns 
//              true (=false) or to not bother checking with isBottom (=true)
void ConstrGraph::emptyArrayRanges(bool noBottomCheck)
{
	for(varIDSet::iterator curArray = arrays.begin(); 
		 curArray != arrays.end(); curArray++)
	{
		emptyRange[*curArray] = true;
		eraseVarConstrNoDiv(*curArray, noBottomCheck);
	}
	constrChanged=true;
}

/**** Erasing ****/
// erases all constraints from this constraint graph
// noBottomCheck - flag indicating whether this function should do nothing if this isBottom() returns 
//              true (=false) or to not bother checking with isBottom (=true)
void ConstrGraph::eraseConstraints(bool noBottomCheck)
{
//		std::cout << "eraseConstraints() dead="<<dead<<" bottom="<<bottom<<"\n";
//		std::cout << "eraseConstraints() isDead()="<<isDead()<<" isBottom()="<<isBottom()<<" dead="<<dead<<" bottom="<<bottom<<"\n";
	// if we're checking and this constraint graph is already bottom, don't bother
	if(noBottomCheck || !isBottom())
	{
		for(map<varID, map<varID, affineInequality> >::iterator it = vars2Value.begin(); it!=vars2Value.end(); it++)
			it->second.clear();
		vars2Value.clear();
		modifiedVars.clear();
		newConstrVars.clear();
		
		constrChanged = true;	
	}
}

// erases all constraints that relate to variable eraseVar and its corresponding divisibility variable 
// from this constraint graph
// returns true if this causes the constraint graph to change and false otherwise
// noBottomCheck - flag indicating whether this function should do nothing if this isBottom() returns 
//              true (=false) or to not bother checking with isBottom (=true)
bool ConstrGraph::eraseVarConstr(const varID& eraseVar, bool noBottomCheck)
{
	bool modified = false;
	varID eraseDivVar = getDivScalar(eraseVar);
	
	// if we're checking and this constraint graph is already bottom, don't bother
	if(noBottomCheck || !isBottom())
	{
		modified = modified || vars2Value[eraseVar].size()>0 || vars2Value[eraseDivVar].size()>0;
		
		// first erase all mappings from eraseVar to other variables
		vars2Value[eraseVar].clear();
		vars2Value[eraseDivVar].clear();
		modifiedVars.insert(eraseVar);
		modifiedVars.insert(eraseDivVar);
						
		// iterate over all variable mappings, erasing links from other variables to eraseVar
		for(map<varID, map<varID, affineInequality> >::iterator curVar = vars2Value.begin(); 
		    curVar != vars2Value.end(); curVar++ )
		{
			modified = curVar->second.find(eraseVar) != curVar->second.end() || modified;
			modified = curVar->second.find(eraseDivVar) != curVar->second.end() || modified;
			
			(curVar->second).erase(eraseVar);
			(curVar->second).erase(eraseDivVar);
			modifiedVars.insert(curVar->first);
		}
	}
	
	constrChanged = constrChanged || modified;
	
	return modified;
}

// erases all constraints that relate to variable eraseVar but not its divisibility variable from 
// this constraint graph
// returns true if this causes the constraint graph to change and false otherwise
// noBottomCheck - flag indicating whether this function should do nothing if this isBottom() returns 
//              true (=false) or to not bother checking with isBottom (=true)
bool ConstrGraph::eraseVarConstrNoDiv(const varID& eraseVar, bool noBottomCheck)
{
	//printf("eraseVarConstrNoDiv eraseVar=%s\n", eraseVar.str().c_str());
	bool modified = false;
	
	// if we're checking and this constraint graph is already bottom, don't bother
	if(noBottomCheck || !isBottom())
	{
		modified = modified || vars2Value[eraseVar].size()>0;
		
		// first erase all mappings from eraseVar to other variables
		vars2Value[eraseVar].clear();
		modifiedVars.insert(eraseVar);
						
		// iterate over all variable mappings, erasing links from other variables to eraseVar
		for(map<varID, map<varID, affineInequality> >::iterator curVar = vars2Value.begin(); 
		    curVar != vars2Value.end(); curVar++ )
		{
			modified = curVar->second.find(eraseVar) != curVar->second.end() || modified;
			(curVar->second).erase(eraseVar);
			modifiedVars.insert(curVar->first);
		}
	}
	
	constrChanged = constrChanged || modified;
	
	return modified;
}

// erases all constraints between eraseVar and scalars in this constraint graph but leave the constraints 
// that relate to its divisibility variable alone
// returns true if this causes the constraint graph to change and false otherwise
// noBottomCheck - flag indicating whether this function should do nothing if this isBottom() returns 
//              true (=false) or to not bother checking with isBottom (=true)
bool ConstrGraph::eraseVarConstrNoDivScalars(const varID& eraseVar, bool noBottomCheck)
{
	bool modified = false;
	
	// if we're checking and this constraint graph is already bottom, don't bother
	if(noBottomCheck || !isBottom())
	{
		modified = modified || vars2Value[eraseVar].size()>0;
		
		// remove all the eraseVar->scalar connections
		for(varIDSet::iterator it=scalars.begin(); it!=scalars.end(); it++)
		{
			modified = vars2Value[eraseVar].erase(*it) > 0 || modified;
		}
		modifiedVars.insert(eraseVar);
						
		// iterate over all variable mappings, erasing links from scalars to eraseVar
		for(map<varID, map<varID, affineInequality> >::iterator curVar = vars2Value.begin(); 
		    curVar != vars2Value.end(); curVar++ )
		{
			// if this is a scalar
			if(scalars.find(curVar->first) != scalars.end())
			{
				modified = (curVar->second).erase(eraseVar) > 0 || modified;
				modifiedVars.insert(curVar->first);
			}
		}
	}
	
	constrChanged = constrChanged || modified;
	
	return modified;
}

// erases the ranges of all array variables
void ConstrGraph::eraseAllArrayRanges()
{
	for(varIDSet::iterator curArray = arrays.begin(); 
		 curArray != arrays.end(); curArray++ )
		// erase the range of each array without the cost of re-checking feasibility
		eraseVarConstrNoDiv(*curArray, true);
	
	// perform the fasibility check, updating the graph's feasibility information
	isFeasible();
}

// Removes any constraints between the given pair of variables
// returns true if this causes the constraint graph to change and false otherwise
bool ConstrGraph::disconnectVars(const varID& x, const varID& y)
{
	bool modified = false;
	// if this constraint graph is already bottom, don't bother
	if(!isBottom())
	{
		if(vars2Value[x].find(y)!=vars2Value[y].end())
			vars2Value[x].erase(y);
		if(vars2Value[y].find(x)!=vars2Value[y].end())
			vars2Value[y].erase(x);
		modifiedVars.insert(x);
		modifiedVars.insert(y);
	}
	
	constrChanged = constrChanged || modified;
	
	return modified;	
}

// Replaces all instances of origVar with newVar. Both are assumed to be scalars.
// returns true if this causes the constraint graph to change and false otherwise
// noBottomCheck - flag indicating whether this function should do nothing if this isBottom() returns 
//              true (=false) or to not bother checking with isBottom (=true)
bool ConstrGraph::replaceVar(const varID& origVar, const varID& newVar, bool noBottomCheck)
{
	bool modified = false;
	varID origDivVar = getDivScalar(origVar);
	varID newDivVar = getDivScalar(newVar);
	
	// If we're checking and this constraint graph is already bottom, don't bother
	if(noBottomCheck || !isBottom())
	{
		//cout << "replaceVar("<<origVar.str()<<", "<<newVar.str()<<");\n";
		
		modified = modified || scalars.find(origVar)!=scalars.end();//vars2Value[origVar].size()>0 || vars2Value[origDivVar].size()>0;

		// First erase the origVar and then re-add it as a scalar with no constraints
		modified = eraseVarConstr(newVar) || modified;
		modified = addScalar(newVar) || modified;

/*		cout << "variables vars2Value ===\n";
		for(map<varID, map<varID, affineInequality> >::iterator it=vars2Value.begin(); it!=vars2Value.end(); it++)
		{ cout << "replaceVar: "<<it->first.str()<<", ==origVar = "<<(it->first == origVar)<<", vars2Value[var].size()="<<vars2Value[it->first].size()<<", vars2Value[origVar].size()="<<vars2Value[origVar].size()<<"==newVar = "<<(it->first == newVar)<<"\n"; }
		
		cout << "variables scalars ===\n";
		for(varIDSet::iterator it=scalars.begin(); it!=scalars.end(); it++)
		{ cout << "replaceVar: "<<(*it).str()<<", ==origVar = "<<((*it) == origVar)<<", vars2Value[var].size()="<<vars2Value[(*it)].size()<<", vars2Value[origVar].size()="<<vars2Value[origVar].size()<<"==newVar = "<<((*it) == newVar)<<"\n"; }
		
		
		cout << "replaceVar: origVar ==== vars2Value["<<origVar.str()<<"].size()="<<vars2Value[origVar].size()<<"\n";
		for(map<varID, affineInequality>::iterator it=vars2Value[origVar].begin(); it!=vars2Value[origVar].end(); it++)
		{ cout << "replaceVar: "<<origVar.str()<<" -> "<<it->first.str()<<" = "<<it->second.str()<<"\n"; }
		*/
		// Next erase all mappings from origVar to other variables
		vars2Value[newVar]=vars2Value[origVar];
		/*cout << "replaceVar: Before ====\n";
		for(map<varID, affineInequality>::iterator it=vars2Value[newVar].begin(); it!=vars2Value[newVar].end(); it++)
		{ cout << "replaceVar: "<<newVar.str()<<" -> "<<it->first.str()<<" = "<<it->second.str()<<"\n"; }*/
		vars2Value[origVar].clear();
		vars2Value[newDivVar]=vars2Value[origDivVar];
		vars2Value[origDivVar].clear();
		
		/*cout << "replaceVar: After ====\n";
		for(map<varID, affineInequality>::iterator it=vars2Value[newVar].begin(); it!=vars2Value[newVar].end(); it++)
		{ cout << "replaceVar: "<<newVar.str()<<" -> "<<it->first.str()<<" = "<<it->second.str()<<"\n"; }*/
		
		// Iterate over all variable mappings, copying links from other variables to origVar
		// to make them into links to newVar
		for(map<varID, map<varID, affineInequality> >::iterator curVar = vars2Value.begin(); 
		    curVar != vars2Value.end(); curVar++ )
		{
			if(curVar->second.find(origVar) != curVar->second.end())
			{
				modified = true;
				(curVar->second)[newVar] = (curVar->second)[origVar];
				(curVar->second).erase(origVar);
			}
			
			if(curVar->second.find(origDivVar) != curVar->second.end())
			{
				modified = true;
				(curVar->second)[newDivVar] = (curVar->second)[origDivVar];
				(curVar->second).erase(origDivVar);
			}
		}
		
		// We don't modify modifiedVars or newConstrVars because the new variable has constraints
		// identical to the old, meaning that this operation has no affect on the transitive closure
	}
	
	constrChanged = constrChanged || modified;
	
	return modified;
}


// Used by copyAnnotVars() and mergeAnnotVars() to identify variables that are interesting
// from their perspective.
bool ConstrGraph::annotInterestingVar(const varID& var, const set<pair<string, void*> >& noCopyAnnots, const set<varID>& noCopyVars,
                                      const string& annotName, void* annotVal)
{
	return !varHasAnnot(var, noCopyAnnots) && noCopyVars.find(var)==noCopyVars.end() && 
		    varHasAnnot(var, annotName, annotVal);
}


// Copies the constrains on all the variables that have the given annotation (srcAnnotName -> srcAnnotVal).
// For each such variable we create a copy variable that is identical except that the
//    (srcAnnotName -> srcAnnotVal) annotation is replaced with the (tgtAnnotName -> tgtAnnotVal) annotation.
// If two variables match the (srcAnnotName -> srcAnnotVal) annotation and the constraint graph has a relation
//    between them, their copies will have the same relation between each other but will have no relation to the
//    original variables. If two variables have a relation and only one is copied, then the copy maintains the 
//    original relation to the non-copied variable.
// A variable matches the given (srcAnnotName -> srcAnnotVal) annotation if this is one of the variable's annotations
//    or if srcAnnotName=="" and the variable has no annotations.
// Avoids copying variables with annotations in the noCopyAnnots set and variables in the noCopyVars set.
// Returns true if this causes the constraint graph to change and false otherwise
bool ConstrGraph::copyAnnotVars(string srcAnnotName, void* srcAnnotVal, 
                                string tgtAnnotName, void* tgtAnnotVal,
                                const set<pair<string, void*> >& noCopyAnnots,
	                             const set<varID>& noCopyVars)
{
	map<varID, map<varID, affineInequality> > xCopyAdditions;
	
	for(map<varID, map<varID, affineInequality> >::iterator itX=vars2Value.begin();
	    itX!=vars2Value.end(); itX++)
	{
		const varID& x = itX->first;
		//cout << "x="<<x.str()<<"\n";
		
		// If x is a matching variable, copy its constraints
		if(!varHasAnnot(x, noCopyAnnots) && noCopyVars.find(x)==noCopyVars.end() && 
		   ((srcAnnotName=="" && x.numAnnotations()==0) || 
		   (x.hasAnnotation(srcAnnotName) && x.getAnnotation(srcAnnotName)==srcAnnotVal)))
		{
			//cout << "    match\n";
			// Create the copy variable, which is identical to x, except with replaced annotations
			varID xCopy(x);
			xCopy.remAnnotation(srcAnnotName);
			xCopy.addAnnotation(tgtAnnotName, tgtAnnotVal);
			
			xCopyAdditions[xCopy] = itX->second;
			modifiedVars.insert(xCopy);
			newConstrVars.insert(xCopy);
			
			// Update xCopy's map to perform copies on the variables inside the map
			map<pair<varID, varID>, affineInequality> yCopyChanges;
			for(map<varID, affineInequality>::iterator itY=xCopyAdditions[xCopy].begin(); itY!=xCopyAdditions[xCopy].end(); itY++)
			{
				const varID& y = itY->first;
				//cout << "y="<<y.str()<<"\n";
				
				// If y is also matching variable, copy the x->y constraint to apply to xCopy->yCopy
				if(!varHasAnnot(y, noCopyAnnots) && noCopyVars.find(y)==noCopyVars.end() && 
		         ((srcAnnotName=="" && y.numAnnotations()==0) || 
		         (y.hasAnnotation(srcAnnotName) && y.getAnnotation(srcAnnotName)==srcAnnotVal)))
				{
					//cout << "    match\n";
					// Create the copy variable, which is identical to y, except with replaced annotations
					varID yCopy(y);
					yCopy.remAnnotation(srcAnnotName);
					yCopy.addAnnotation(tgtAnnotName, tgtAnnotVal);
					
					pair<varID, varID> oldnewVars(y, yCopy);
					yCopyChanges[oldnewVars] = itY->second;
					//itY->first.remAnnotation(srcAnnotName);
					//itY->first.addAnnotation(tgtAnnotName, tgtAnnotVal);
				}
			}
			
			// Perform the changes
			for(map<pair<varID, varID>, affineInequality>::iterator it=yCopyChanges.begin();
			    it!=yCopyChanges.end(); it++)
			{
				//cout << "Erasing "<<xCopy.str()<<" -> "<<it->first.first.str()<<"\n";
				(xCopyAdditions[xCopy]).erase(it->first.first);
				(xCopyAdditions[xCopy])[it->first.second] = it->second;
			}
		}
		// If x is NOT a matching variable, process its relations to other variables
		else
		{
			//cout << "    no match\n";
			map<varID, affineInequality> yCopyAdditions;
			for(map<varID, affineInequality>::iterator itY=itX->second.begin(); itY!=itX->second.end(); itY++)
			{
				const varID& y = itY->first;
				//cout << "y="<<y.str()<<"\n";
				
				// If y is also matching variable, create a new x->CopyY constraints, which is a copy of the x->y constraint
				if(!varHasAnnot(y, noCopyAnnots) && noCopyVars.find(y)==noCopyVars.end() && 
		         ((srcAnnotName=="" && y.numAnnotations()==0) || 
		         (y.hasAnnotation(srcAnnotName) && y.getAnnotation(srcAnnotName)==srcAnnotVal)))
				{
					//cout << "    match\n";
					// Create the copy variable, which is identical to y, except with replaced annotations
					varID yCopy(y);
					yCopy.remAnnotation(srcAnnotName);
					yCopy.addAnnotation(tgtAnnotName, tgtAnnotVal);
					
					yCopyAdditions[yCopy] = itY->second;
				}
			}
			
			// If x has constraints that will be copied
			if(yCopyAdditions.begin() != yCopyAdditions.end())
			{
				modifiedVars.insert(x);
				newConstrVars.insert(x);
			}
			
			// Insert the newly-copied constraints back into itX->second
			for(map<varID, affineInequality>::iterator it=yCopyAdditions.begin(); it!=yCopyAdditions.end(); it++)
				itX->second[it->first] = it->second;
		}
	}
	
	// Insert the newly-copied constraints back into vars2Value
	for(map<varID, map<varID, affineInequality> >::iterator it=xCopyAdditions.begin();
	    it!=xCopyAdditions.end(); it++)
		vars2Value[it->first] = it->second;
	
	// ------------------------------------
	// Add copy scalar variables to scalars
	varIDSet copyScalars;
	for(varIDSet::iterator it=scalars.begin(); it!=scalars.end(); it++)
	{
		const varID& var = *it;
		
		//cout << "copyAnnotVars: var = "<<var.str()<<"\n";
		if(!varHasAnnot(var, noCopyAnnots) && noCopyVars.find(var)==noCopyVars.end() && 
		   ((srcAnnotName=="" && var.numAnnotations()==0) || 
		   (var.hasAnnotation(srcAnnotName) && var.getAnnotation(srcAnnotName)==srcAnnotVal)))
		{
			// Create the copy variable, which is identical to var, except with replaced annotations
			varID varCopy(var);
			varCopy.remAnnotation(srcAnnotName);
			varCopy.addAnnotation(tgtAnnotName, tgtAnnotVal);
			//cout << "      varCopy = "<<varCopy.str()<<"\n";
			
			// Record the copy
			copyScalars.insert(varCopy);
		}
	}
	
	for(varIDSet::iterator it=copyScalars.begin(); it!=copyScalars.end(); it++)
		scalars.insert(*it);
	
	// ----------------------------------
	// Add copy array variables to arrays
	varIDSet copyArrays;
	for(varIDSet::iterator it=arrays.begin(); it!=arrays.end(); it++)
	{
		const varID& var = *it;
		
		if(!varHasAnnot(var, noCopyAnnots) && noCopyVars.find(var)==noCopyVars.end() && 
		   ((srcAnnotName=="" && var.numAnnotations()==0) || 
		   (var.hasAnnotation(srcAnnotName) && var.getAnnotation(srcAnnotName)==srcAnnotVal)))
		{
			// Create the copy variable, which is identical to var, except with replaced annotations
			varID varCopy(var);
			varCopy.remAnnotation(srcAnnotName);
			varCopy.addAnnotation(tgtAnnotName, tgtAnnotVal);
			
			// Record the copy
			copyArrays.insert(varCopy);
		}
	}
	
	for(varIDSet::iterator it=copyArrays.begin(); it!=copyArrays.end(); it++)
		arrays.insert(*it);
		
	// ----------------------------------
	// Add copy divVariables variables to divVariables
	varIDSet copydivVariables;
	for(varIDSet::iterator it=divVariables.begin(); it!=divVariables.end(); it++)
	{
		const varID& var = *it;
		
		if(!varHasAnnot(var, noCopyAnnots) && noCopyVars.find(var)==noCopyVars.end() && 
		   ((srcAnnotName=="" && var.numAnnotations()==0) || 
		   (var.hasAnnotation(srcAnnotName) && var.getAnnotation(srcAnnotName)==srcAnnotVal)))
		{
			// Create the copy variable, which is identical to var, except with replaced annotations
			varID varCopy(var);
			varCopy.remAnnotation(srcAnnotName);
			varCopy.addAnnotation(tgtAnnotName, tgtAnnotVal);
			
			// Record the copy
			copydivVariables.insert(varCopy);
		}
	}
	
	for(varIDSet::iterator it=copydivVariables.begin(); it!=copydivVariables.end(); it++)
		divVariables.insert(*it);
}
		   
// Merges the state of the variables in the constraint graph with the [finalAnnotName -> finalAnnotVal] annotation
//    with the state of the variables with the [remAnnotName -> remAnnotVal]. Each constraint that involves a variable
//    with the former annotation and the same variable with the latter annotation is replaced with the union of the 
//    two constraints and will only involve the variable with the [finalAnnotName -> finalAnnotVal] (latter) annotation.
// All variables with the [remAnnotName -> remAnnotVal] annotation are removed from the constraint graph.
// A variable matches the given (srcAnnotName -> srcAnnotVal) annotation if this is one of the variable's annotations
//    or if srcAnnotName=="" and the variable has no annotations.
// Avoids merging variables with annotations in the noCopyAnnots set and variables in the noCopyVars set.
// Returns true if this causes the constraint graph to change and false otherwise.
// It is assumed that variables that match [finalAnnotName -> finalAnnotVal] differ from variables that match
//    [remAnnotName -> remAnnotVal] in only that annotation.
bool ConstrGraph::mergeAnnotVars(const string& finalAnnotName, void* finalAnnotVal, 
                                 const string& remAnnotName,   void* remAnnotVal,
                                 const set<pair<string, void*> >& noCopyAnnots,
                                 const set<varID>& noCopyVars)
{
	bool modified=false;
	
	// The set of variables whose submaps we'll be deleting because they belong
	// to variables that have the [remAnnotName -> remAnnotVal] annotation
	set<varID> toDeleteX;
	
	for(map<varID, map<varID, affineInequality> >::iterator itX=vars2Value.begin();
	    itX!=vars2Value.end(); itX++)
	{
		const varID& x = itX->first;
		//cout << "x="<<x.str()<<"\n";
		
		// If x matches the final annotation
		if(annotInterestingVar(x, noCopyAnnots, noCopyVars, finalAnnotName, finalAnnotVal))
		{
			// Create a version of x with the final annotations replaced with the rem annotations
			varID xRem(x);
			ROSE_ASSERT( xRem.swapAnnotations(finalAnnotName, finalAnnotVal, remAnnotName, remAnnotVal) );
			//cout << "xRem="<<xRem.str()<<", itX->second.size()="<<itX->second.size()<<"\n";
			
			// Union x's sub-map, unioning any inequalities x<=yFinal and x<=yRem and deleting the latter
			modified = mergeAnnotVarsSubMap(itX->second, finalAnnotName, finalAnnotVal, 
	                                      remAnnotName, remAnnotVal, noCopyAnnots, noCopyVars) || modified;
			
			map<varID, map<varID, affineInequality> >::iterator itXrem = vars2Value.find(xRem);
			// If x's associated rem variable has its own sub-map
			if(itXrem != vars2Value.end())
			{
				//cout << "    both rem and final exist, itXrem->second.size()="<<itXrem->second.size()<<"\n";
				// Iterate through xRem's sub-map, transferring state to x's sub-map
				for(map<varID, affineInequality>::iterator itYRem = itXrem->second.begin(); 
				    itYRem!=itXrem->second.end(); itYRem++)
				{
					const varID& yRem = itYRem->first;
					//cout << "    yRem="<<yRem.str()<<", ineq="<<itYRem->second.str()<<"\n";
					
					// For the current y variable, consider all three possibilities: y has 
					// a final annotation, a rem annotation or neither.
					if(!varHasAnnot(itYRem->first, noCopyAnnots) && noCopyVars.find(itYRem->first)==noCopyVars.end())
					{
						// xRem <= yFinal
						if(varHasAnnot(yRem, finalAnnotName, finalAnnotVal))
						{
							// Union the x <= yFinal inequality with the x <= yRem inequality
							unionXYsubMap(itX->second, yRem, itYRem->second);
						}
						// xRem <= yRem
						else if(varHasAnnot(yRem, remAnnotName, remAnnotVal))
						{
							varID yFinal(yRem);
							ROSE_ASSERT( yFinal.swapAnnotations(remAnnotName, remAnnotVal, finalAnnotName, finalAnnotVal) );
							
							// Update the x->yFinal inequality with this one
							unionXYsubMap(itX->second, yFinal, itYRem->second);
						}
						// xRem <= y (no rem or final annotation)
						else
							// Update the x->y inequality with this one
							unionXYsubMap(itX->second, yRem, itYRem->second);
					}
		   		// xRem <= y (no rem or final annotation)
		   		else
		   			// Update the x->y inequality with this one
						unionXYsubMap(itX->second, yRem, itYRem->second);
				}
				
				modified = modified || (itXrem->second.size()>0);
				// Record xRem's submap for future deletion
				toDeleteX.insert(xRem);
				
				modifiedVars.insert(x);
				modifiedVars.insert(xRem);
				newConstrVars.insert(x);
			}
			// If x's associated rem variable doesn't have its own sub-map, we don't need to
			// do anything since there is nothing to union with the x <= ? inequalities
		}
		// If x matches the rem annotation
		else if(annotInterestingVar(x, noCopyAnnots, noCopyVars, remAnnotName, remAnnotVal))
		{
			// Create a version of x with the rem annotations replaced with the final annotations
			varID xFinal(x);
			ROSE_ASSERT( xFinal.swapAnnotations(remAnnotName, remAnnotVal, finalAnnotName, finalAnnotVal) );
			
			map<varID, map<varID, affineInequality> >::iterator itXfinal = vars2Value.find(xFinal);
			// If x's equivalent final variable does not have its own sub-map
			// (the case where both variables exist is covered above)
			if(itXfinal == vars2Value.end())
			{
				// Copy x's sub-map over under xFinal's name by simply re-annotating x with the final annotation
				//ROSE_ASSERT( (itX->first).swapAnnotations(remAnnotName, remAnnotVal, finalAnnotName, finalAnnotVal) );
				vars2Value[xFinal] = itX->second;
				
				// Union x's sub-map, unioning any inequalities x<=yFinal and x<=yRem and deleting the latter
				mergeAnnotVarsSubMap(/*itX->second**/(vars2Value.find(xFinal))->second, finalAnnotName, finalAnnotVal, 
                                 remAnnotName, remAnnotVal, noCopyAnnots, noCopyVars);
				
				// Remove itXfinal from this sub-map (if its there) to make sure that xFinal doesn't 
				// have a relation to itself
				itX->second.erase(xFinal);
				
				modified = true;
			}
		}
		// If this variable has neither the rem nor the final annotation
		else
		{
			// Union x's sub-map, unioning any inequalities x<=yFinal and x<=yRem and deleting the latter
			modified = mergeAnnotVarsSubMap(itX->second, finalAnnotName, finalAnnotVal, 
	                                      remAnnotName, remAnnotVal, noCopyAnnots, noCopyVars) || modified;
		}
	}
	
	// Delete the sub-maps recorded in toDeleteX
	for(set<varID>::iterator it=toDeleteX.begin(); it!=toDeleteX.end(); it++)
		vars2Value.erase(*it);
		
	// -------------------------------------------
	// Now update scalars, arrays and divVariables
	
	return modified;
}

// Union the current inequality for y in the given subMap of vars2Value with the given affine inequality
// Returns true if this causes a change in the subMap, false otherwise.
bool ConstrGraph::unionXYsubMap(map<varID, affineInequality>& subMap, const varID& y, const affineInequality& ineq)
{
	bool modified = false;
	
//cout << "unionXYsubMap("<<y.str()<<", "<<ineq.str()<<")\n";
	
	// Update the x->y inequality with this one
	map<varID, affineInequality>::iterator loc = subMap.find(y);
	if(loc != subMap.end())
		modified = loc->second.unionUpd(ineq) || modified;
	else
	{
		subMap[y] = ineq;
		modified = true;
	}
	
//cout << "unionXYsubMap() subMap["<<y.str()<<"] = "<<subMap[y].str()<<"\n";
	
	return modified;
}

// Merges the given sub-map of var2Vals, just like mergeAnnotVars. Specifically, for every variable in the subMap
// that has a [remAnnotName -> remAnnotVal] annotation,
// If there exists a corresponding variable that has the [finalAnnotName -> finalAnnotVal] annotation, 
//    their respective inequalities are unioned. This union is left with the latter variable and the former
//    variable's entry in subMap is removed
// If one does not exist, we simply replace the variable's record with an identical one that now belongs
//    to its counterpart with the [finalAnnotName -> finalAnnotVal] annotation.
// Other entries are left alone.
// Returns true if this causes the subMap to change, false otherwise.
bool ConstrGraph::mergeAnnotVarsSubMap(map<varID, affineInequality>& subMap, 
                                       string finalAnnotName, void* finalAnnotVal, 
                                       string remAnnotName,   void* remAnnotVal,
                                       const set<pair<string, void*> >& noCopyAnnots,
                                       const set<varID>& noCopyVars)
{
	bool modified = false;
	
	// The set of variables whose sub-submaps we'll be deleting because they belong
	// to variables that have the [remAnnotName -> remAnnotVal] annotation
	set<varID> toDeleteY;
	
	// Replace all variables that have the rem annotation with equivalent variables that have 
	// the final annotation or perform a union if both are present
	for(map<varID, affineInequality>::iterator itYRem = subMap.begin(); 
	    itYRem!=subMap.end(); itYRem++)
	{
		const varID& y = itYRem->first;
		//cout << "    mergeAnnotVarsSubMap y="<<y.str()<<"\n";
		
		// If the current y matches the rem annotation
		if(annotInterestingVar(y, noCopyAnnots, noCopyVars, remAnnotName, remAnnotVal))
		{
			// Generate the version of y that has the final annotation
			varID yFinal(y);
			ROSE_ASSERT( yFinal.swapAnnotations(remAnnotName, remAnnotVal, finalAnnotName, finalAnnotVal) );
			
			map<varID, affineInequality>::iterator yFinalIt = subMap.find(yFinal);
			//cout << "    mergeAnnotVarsSubMap yFinal="<<yFinal.str()<<", found="<<(yFinalIt != subMap.end())<<"\n";

			// If this sub-map contains both a rem and a final version of the same variable
			if(yFinalIt != subMap.end())
			{
				// Union both their inequalities and place the result into the final variable
				//cout << "        old ineq="<<yFinalIt->second.str()<<"\n";
				yFinalIt->second.unionUpd(itYRem->second);
				//cout << "        new ineq="<<yFinalIt->second.str()<<"\n";
				// Record the rem variable for deletion
				toDeleteY.insert(y);
			}
			// If this sub-map only contains a rem version of this variable
			else
			{
				// Swap its annotation so that it now has the final annotation
				//ROSE_ASSERT( (itYRem->first).swapAnnotations(remAnnotName, remAnnotVal, finalAnnotName, finalAnnotVal) );
				subMap[yFinal] = itYRem->second;
				// Record the rem variable for deletion
				toDeleteY.insert(y);
			}
			
			modified = true;
		}
	}
	
	// Erase all the y entries in x's submap that belong rem variables
	for(set<varID>::iterator it=toDeleteY.begin(); it!=toDeleteY.end(); it++)
		subMap.erase(*it);
	
	// Filter scalars, arrays and divVariables to remove any rem variables. When variables
	// have a rem version but not a final version, we replace the rem version with the final version.
	modified = mergeAnnotVarsSet(scalars, 
	                             finalAnnotName, finalAnnotVal, remAnnotName, remAnnotVal,
	                             noCopyAnnots, noCopyVars) || modified;
	modified = mergeAnnotVarsSet(scalars, 
	                             finalAnnotName, finalAnnotVal, remAnnotName, remAnnotVal,
	                             noCopyAnnots, noCopyVars) || modified;
	modified = mergeAnnotVarsSet(divVariables, 
	                             finalAnnotName, finalAnnotVal, remAnnotName, remAnnotVal,
	                             noCopyAnnots, noCopyVars) || modified;
	
	return modified;	
}

// Support routine for mergeAnnotVars(). Filters out any rem variables in the given set, replacing
// them with their corresponding final versions if those final versions are not already in the set
// Returns true if this causes the set to change, false otherwise.
bool ConstrGraph::mergeAnnotVarsSet(set<varID> varsSet, 
                                    string finalAnnotName, void* finalAnnotVal, 
                                    string remAnnotName,   void* remAnnotVal,
                                    const set<pair<string, void*> >& noCopyAnnots,
                                    const set<varID>& noCopyVars)
{
	bool modified = false;
	
	set<varID> varsToDelete;
	set<varID> varsToInsert;
	
	for(set<varID>::iterator it=varsSet.begin(); it!=varsSet.end(); it++)
	{
		const varID& var = *it;
		
		// If this is a rem variable
		if(annotInterestingVar(var, noCopyAnnots, noCopyVars, remAnnotName, remAnnotVal))
		{
			// Create a version of var with the rem annotations replaced with the final annotations
			varID varFinal(var);
			ROSE_ASSERT( varFinal.swapAnnotations(remAnnotName, remAnnotVal, finalAnnotName, finalAnnotVal) );
			
			// If the corresponding final variable exists in the set
			if(varsSet.find(varFinal)!=varsSet.end())
				// Record that we're going to remove var
				varsToDelete.insert(var);
			else
			{
				varID varFinal(var);
				// Change var's annotation to convert it from rem to final
				ROSE_ASSERT( varFinal.swapAnnotations(remAnnotName, remAnnotVal, finalAnnotName, finalAnnotVal) );
				
				
				// Record that we're going to remove var and insert varFinal
				varsToDelete.insert(var);
				varsToInsert.insert(varFinal);
			}
			
			modified = true;
		}
	}
	
	for(set<varID>::iterator it=varsToDelete.begin(); it!=varsToDelete.end(); it++)
		varsSet.erase(*it);
	
	for(set<varID>::iterator it=varsToInsert.begin(); it!=varsToInsert.end(); it++)
		varsSet.insert(*it);
	
	return modified;
}

// Returns true if the given variable has an annotation in the given set and false otherwise.
// The variable matches an annotation if its name and value directly match or if the variable
// has no annotations and the annotation's name is "".
bool ConstrGraph::varHasAnnot(const varID& var, const set<pair<string, void*> >& annots)
{
	for(set<pair<string, void*> >::const_iterator it=annots.begin(); it!=annots.end(); it++)
	{
		if(varHasAnnot(var, (*it).first, (*it).second))
			return true;
	}
	return false;
}

// Returns true if the given variable has an annotation in the given set and false otherwise.
// The variable matches an annotation if its name and value directly match or if the variable
// has no annotations and the annotName=="".
bool ConstrGraph::varHasAnnot(const varID& var, string annotName, void* annotVal)
{
	// If the annotation matches variables with no annotation and this variable has no annotations
	if(annotName=="" && var.numAnnotations()==0)
		return true;
	
	// If var has the given annotation, which matches matches the given value
	if(var.hasAnnotation(annotName) && var.getAnnotation(annotName)==annotVal)
		return true;
			
	return false;
}

// Returns a constraint graph that only includes the constrains in this constraint graph that involve the
// variables in focusVars and their respective divisibility variables, if any. 
// It is assumed that focusVars only contains scalars and not array ranges.
ConstrGraph* ConstrGraph::getProjection(const varIDSet& focusVars)
{
	ConstrGraph* pCG = new ConstrGraph(func, divL, sgnL);
	
	// focusVars that are inside this->scalars and their respective divisibility variables, if any
	// We record these variables in allFocusVars and only worry about them when extracting
	//    the projection constraints. 
	// // Furthermore, we add these variables to the list of scalars
	// //    and divisibility variables of pCG.
	varIDSet allFocusVars;
	for(varIDSet::iterator it=focusVars.begin(); it!=focusVars.end(); it++)
	{
		const varID& var = *it;
		if(scalars.find(var) != scalars.end())
		{
			addScalar(var);
			allFocusVars.insert(var);
//			pCG->addScalar(var);
			
			varID divVar = getDivScalar(var);
			if(divVariables.find(divVar) != divVariables.end())
			{
				allFocusVars.insert(divVar);
//				pCG->addDivVar(var);
			}
		}
	}
	
	// We simply copy the sets of scalars and divisibility variables from this to pCG
	// We may not need all of them but its simpler this way
	pCG->scalars = scalars;
	pCG->divVariables = divVariables;
	
	// Copy all constraints in vars2Value that pertain to variables in allFocusVars to pCG
	for(map<varID, map<varID, affineInequality> >::iterator itX=vars2Value.begin(); itX!=vars2Value.end(); itX++)
	{
		

		// If itX->first appears in allFocusVars
		if(allFocusVars.find(itX->first) != allFocusVars.end())
		{
			// Copy all of the [itX->first <= ???] conditions to pCG
			pCG->vars2Value[itX->first] = itX->second;
			pCG->modifiedVars.insert(itX->first);
		}
		else
		{
			bool foundMatches=false; // Set to true when we find a variable in itX->second that appears in allFocusVars
			for(map<varID, affineInequality>::iterator itY=itX->second.begin(); itY!=itX->second.end(); itY++)
			{
				// if itY->first appears in allFocusVars
				if(allFocusVars.find(itY->first) != allFocusVars.end())
				{
					if(!foundMatches)
					{
						// Create a map in pCG for itX->first and insert itY->first ==> itY->second into this map
						map<varID, affineInequality> initMap;
						initMap[itY->first] = itY->second;
						pCG->vars2Value[itX->first] = initMap;
						foundMatches = true;
					}
					else
					{
						// Insert itY->first ==> itY->second into the pCG's map of itX->first
						(pCG->vars2Value[itX->first])[itY->first] = itY->second;
					}
					modifiedVars.insert(itY->first);
				}
			}
		}
	}
	
	return pCG;
}

// Creates a new constraint graph that is the disjoint union of the two given constraint graphs.
// The variables in cg1 and cg2 that are not in the noAnnot set, are annotated with cg1Annot and cg2Annot, respectively,
// under the name annotName.
// cg1 and cg2 are assumed to have identical constraints between variables in the noAnnotset.
ConstrGraph* ConstrGraph::joinCG(ConstrGraph* cg1, void* cg1Annot, ConstrGraph* cg2, void* cg2Annot, string annotName, const varIDSet& noAnnot)
{
	ROSE_ASSERT(cg1Annot != cg2Annot);
	ConstrGraph* combo = new ConstrGraph();
	if(debugLevel>=1)
	{
		cout << "joinCG("<<cg1<<", "<<cg1Annot<<", "<<cg2<<", "<<cg2Annot<<", "<<annotName<<", noAnnot: [";
		for(varIDSet::const_iterator it = noAnnot.begin(); it!=noAnnot.end(); )
		{ cout << (*it).str(); it++; if(it!=noAnnot.end()) cout << ", "; }
		cout << "]\n";
		cout << "=== joinCG_copyState1 === \n";
	}
	joinCG_copyState(combo, cg1, cg1Annot, annotName, noAnnot);
	if(debugLevel>=1) cout << "=== joinCG_copyState2 === \n";
	joinCG_copyState(combo, cg2, cg2Annot, annotName, noAnnot);
	if(debugLevel>=1) cout << "=== transitiveClosure === \n";
	combo->transitiveClosure();
	
	return combo;
}

// Copies the per-variable contents of srcCG to tgtCG, while ensuring that in tgtCG all variables that are not
// in noAnnot are annotated with the annotName->annot label. For variables in noAnnot, the function ensures
// that tgtCG does not have inconsistent mappings between such variables.
void ConstrGraph::joinCG_copyState(ConstrGraph* tgtCG, ConstrGraph* srcCG, void* annot, string annotName, const varIDSet& noAnnot)
{
	// === vars2Value ===
	for(map<varID, map<varID, affineInequality> >::iterator itX=srcCG->vars2Value.begin();
	    itX!=srcCG->vars2Value.end(); itX++)
	{
		// Only worry about scalars
		if(srcCG->scalars.find(itX->first) == srcCG->scalars.end()) continue;
		
		varID x = itX->first;
				
		// Annotate x if necessary and add a fresh map for x -> ? mappings in tgtCG->vars2Value, if necessary
		if(noAnnot.find(x) == noAnnot.end())
		{
			//cout << "joinCG_copyState: annotating "<<x.str();
			// Annotate x if it is not already annotated
			if(!x.getAnnotation(annotName))
				x.addAnnotation(annotName, annot);
			//cout << " : "<<x.str()<<"\n";
			map<varID, affineInequality> empty;
			tgtCG->vars2Value[x] = empty;
			
			// Add the annotated variable as a scalar to tgtCG to ensure that transitiveClosure operates on it
			tgtCG->addScalar(x);
		}
		else if(tgtCG->vars2Value.find(x) == tgtCG->vars2Value.end())
		{
			map<varID, affineInequality> empty;
			tgtCG->vars2Value[x] = empty;
		}
		tgtCG->modifiedVars.insert(x);
		tgtCG->newConstrVars.insert(x);
		map<varID, affineInequality>& xToTgt = tgtCG->vars2Value[x];
		
		for(map<varID, affineInequality>::iterator itY=itX->second.begin();
		    itY!=itX->second.end(); itY++)
		{
			// Only worry about scalars
			if(srcCG->scalars.find(itY->first) == srcCG->scalars.end()) continue;
			
			varID y = itY->first;
			// Annotate y if necessary and add a fresh x->y mapping, if necessary
			if(noAnnot.find(y) == noAnnot.end())
			{
				//cout << "joinCG_copyState: annotating     ->"<<y.str();
				// Annotate y if it is not already annotated
				if(!y.getAnnotation(annotName))
					y.addAnnotation(annotName, annot);
				//cout << " : "<<y.str()<<"\n";
				xToTgt[y] = itY->second;
				
				// Add the annotated variable as a scalar to tgtCG to ensure that transitiveClosure operates on it
				tgtCG->addScalar(y);
				
				if(debugLevel>=1) 
					cout << "joinCG_copyState: addingA "<<x.str()<<"->"<<y.str()<<": "<<xToTgt[y].str()<<"\n";
			}
			// // We do not allow disagreements about the value of the x->y mapping 
			// If there are disagreements about the value of the x->y mapping (can only happen if both
			// x and y are in noAnnot and they have different mappings in the various constraint graphs)
			// we take the union of the two options
			else if(xToTgt.find(y) != xToTgt.end())
			{
				/*if(xToTgt[y] != itY->second)
				{
					cout << "x="<<x.str()<<"y="<<y.str()<<"\n";
					for(varIDSet::const_iterator it = noAnnot.begin(); it!=noAnnot.end(); it++)
						cout << "noAnnot: "<<(*it).str()<<"\n";
					ROSE_ASSERT(xToTgt[y] == itY->second);
				}*/
				xToTgt[y] += itY->second;
				if(debugLevel>=1) 
					cout << "joinCG_copyState: unioning "<<x.str()<<"->"<<y.str()<<" from "<<itY->second.str()<<" to "<<xToTgt[y].str()<<"\n";
			}
			else
			{
				xToTgt[y] = itY->second;
				if(debugLevel>=1) 
					cout << "joinCG_copyState: addingB "<<x.str()<<"->"<<y.str()<<": "<<xToTgt[y].str()<<"\n";
			}
			tgtCG->modifiedVars.insert(y);
			tgtCG->newConstrVars.insert(y);
		}
	}

	// === arrays ===
	for(varIDSet::iterator it=srcCG->arrays.begin(); it!=srcCG->arrays.end(); it++)
	{
		varID var = *it;
		if(noAnnot.find(var) == noAnnot.end())
			var.addAnnotation(annotName, annot);
		tgtCG->arrays.insert(var);
	}
	
	// === scalars ===
	for(varIDSet::iterator it=srcCG->scalars.begin(); it!=srcCG->scalars.end(); it++)
	{
		varID var = *it;
		if(noAnnot.find(*it) == noAnnot.end())
			var.addAnnotation(annotName, annot);
		tgtCG->scalars.insert(var);
	}
	
	/*for(varIDSet::iterator it = tgtCG->scalars.begin(); it!=tgtCG->scalars.end(); it++)
		cout << "    var: "<< (*it).str()<<"\n";*/
}

// Replaces all references to variables with the given annotName->annot annotation to references to variables without the annotation
// Returns true if this causes the constraint graph to change and false otherwise
bool ConstrGraph::removeVarAnnot(string annotName, void* annot)
{
	bool modified=false;
	
	// === vars2Value ===
	// Set of vars x whose x->? mappings we'll need to change
	varIDSet changedVarsX;
	
	for(map<varID, map<varID, affineInequality> >::iterator itX=vars2Value.begin();
	    itX!=vars2Value.end(); itX++)
	{
		varID x = itX->first; 
		if(x.getAnnotation(annotName) == annot)
		{
			changedVarsX.insert(x);
			modifiedVars.insert(x); // ???
		}
		
		// Set of vars y for which we'll need to remove the annotation
		varIDSet changedVarsY;
		for(map<varID, affineInequality>::iterator itY=itX->second.begin();
		    itY!=itX->second.end(); itY++)
		{
			varID y = itY->first; 
			if(y.getAnnotation(annotName) == annot)
				changedVarsY.insert(y);
		}
		
		modified = (changedVarsY.size()>0) || modified;
		
		// Change all the x->y mappings for all variables y for which we need to remove the annotation
		for(varIDSet::iterator it=changedVarsY.begin(); it!=changedVarsY.end(); it++)
		{
			varID y = *it;
			// Remove the annotation from y to produce y'
			ROSE_ASSERT(y.remAnnotation(annotName));
			// Move the x->y mapping to become the x->y' mapping
			(itX->second)[y] = (itX->second)[*it];
			(itX->second).erase(*it);
		}
	}	
	
	modified = (changedVarsX.size()>0) || modified;
	
	// Change all the x->? mappings for all variables x for which we need to remove the annotation
	for(varIDSet::iterator it=changedVarsX.begin(); it!=changedVarsX.end(); it++)
	{
		varID x = *it;
		// Remove the annotation from x to produce x'
		ROSE_ASSERT(x.remAnnotation(annotName));
		// Move the x->? mapping to become the x'->? mapping
		vars2Value[x] = vars2Value[*it];
		vars2Value.erase(*it);
	}

	// === arrays ===
	varIDSet newArrays;
	for(varIDSet::iterator it=arrays.begin(); it!=arrays.end(); it++)
	{
		varID var = *it;
		if(var.getAnnotation(annotName) == annot)
			modified = var.remAnnotation(annotName) || modified;
		newArrays.insert(var);
	}
	arrays = newArrays;
	
	// === scalars ===
	varIDSet newScalars;
	for(varIDSet::iterator it=scalars.begin(); it!=scalars.end(); it++)
	{
		varID var = *it;
		if(var.getAnnotation(annotName) == annot)
			modified = var.remAnnotation(annotName) || modified;
		newScalars.insert(var);
	}
	scalars = newScalars;
	
	// === divVariables ===
	varIDSet newDivVariables;
	for(varIDSet::iterator it=divVariables.begin(); it!=divVariables.end(); it++)
	{
		varID var = *it;
		if(var.getAnnotation(annotName) == annot)
			modified = var.remAnnotation(annotName) || modified;
		newDivVariables.insert(var);
	}
	divVariables = newDivVariables;	
	
	return modified;
}

// Replaces all references to variables with the given annotName->annot annotation to 
// references to variables without the annotation
// Returns true if this causes the constraint graph to change and false otherwise
bool ConstrGraph::replaceVarAnnot(string oldAnnotName, void* oldAnnot,
                                  string newAnnotName, void* newAnnot)
{
	bool modified=false;
	
	// === vars2Value ===
	// Set of vars x whose x->? mappings we'll need to change
	varIDSet changedVarsX;
	
	for(map<varID, map<varID, affineInequality> >::iterator itX=vars2Value.begin();
	    itX!=vars2Value.end(); itX++)
	{
		varID x = itX->first; 
		if(x.getAnnotation(oldAnnotName) == oldAnnot)
		{
			changedVarsX.insert(x);
			// This is not a modification that can affect the transitive closure
			//modifiedVars.insert(x);
		}
		
		// Set of vars y for which we'll need to remove the annotation
		varIDSet changedVarsY;
		for(map<varID, affineInequality>::iterator itY=itX->second.begin();
		    itY!=itX->second.end(); itY++)
		{
			varID y = itY->first; 
			if(y.getAnnotation(oldAnnotName) == oldAnnot)
				changedVarsY.insert(y);
		}
		
		modified = (changedVarsY.size()>0) || modified;
		
		// Change all the x->y mappings for all variables y for which we need to remove the annotation
		for(varIDSet::iterator it=changedVarsY.begin(); it!=changedVarsY.end(); it++)
		{
			varID y = *it;
			// Remove the annotation from y to produce y'
			ROSE_ASSERT(y.swapAnnotations(oldAnnotName, oldAnnot, newAnnotName, newAnnot));
			// Move the x->y mapping to become the x->y' mapping
			(itX->second)[y] = (itX->second)[*it];
			(itX->second).erase(*it);
		}
	}	
	
	modified = (changedVarsX.size()>0) || modified;
	
	// Change all the x->? mappings for all variables x for which we need to remove the annotation
	for(varIDSet::iterator it=changedVarsX.begin(); it!=changedVarsX.end(); it++)
	{
		varID x = *it;
		// Remove the annotation from x to produce x'
		ROSE_ASSERT(x.swapAnnotations(oldAnnotName, oldAnnot, newAnnotName, newAnnot));
		// Move the x->? mapping to become the x'->? mapping
		vars2Value[x] = vars2Value[*it];
		vars2Value.erase(*it);
	}

	// === arrays ===
	varIDSet newArrays;
	for(varIDSet::iterator it=arrays.begin(); it!=arrays.end(); it++)
	{
		varID var = *it;
		if(var.getAnnotation(oldAnnotName) == oldAnnot)
			modified = var.swapAnnotations(oldAnnotName, oldAnnot, newAnnotName, newAnnot) || modified;
		newArrays.insert(var);
	}
	arrays = newArrays;
	
	// === scalars ===
	varIDSet newScalars;
	for(varIDSet::iterator it=scalars.begin(); it!=scalars.end(); it++)
	{
		varID var = *it;
		if(var.getAnnotation(oldAnnotName) == oldAnnot)
			modified = var.swapAnnotations(oldAnnotName, oldAnnot, newAnnotName, newAnnot) || modified;
		newScalars.insert(var);
	}
	scalars = newScalars;
	
	// === divVariables ===
	varIDSet newDivVariables;
	for(varIDSet::iterator it=divVariables.begin(); it!=divVariables.end(); it++)
	{
		varID var = *it;
		if(var.getAnnotation(oldAnnotName) == oldAnnot)
			modified = var.swapAnnotations(oldAnnotName, oldAnnot, newAnnotName, newAnnot) || modified;
		newDivVariables.insert(var);
	}
	divVariables = newDivVariables;	
	
	return modified;
}

// For all variables that have a string (tgtAnnotName -> tgtAnnotVal) annotation 
//    (or if tgtAnnotName=="" and the variable has no annotations), add the annotation
//    (newAnnotName -> newAnnotVal).
// Returns true if this causes the constraint graph to change and false otherwise
bool ConstrGraph::addVarAnnot(string tgtAnnotName, void* tgtAnnotVal, string newAnnotName, void* newAnnotVal)
{
	bool modified=false;
	// === vars2Value ===
	// Set of vars x whose x->? mappings we'll need to change
	varIDSet changedVarsX;
	
	for(map<varID, map<varID, affineInequality> >::iterator itX=vars2Value.begin();
	    itX!=vars2Value.end(); itX++)
	{
		varID x = itX->first; 
		if((tgtAnnotName=="" && x.numAnnotations()==0) || x.getAnnotation(tgtAnnotName)==tgtAnnotVal)
			changedVarsX.insert(x);
		
		// This is not a modification that can affect the transitive closure
		//modifiedVars.insert(x);
		
		// Set of vars y for which we'll need to add the annotation
		varIDSet changedVarsY;
		for(map<varID, affineInequality>::iterator itY=itX->second.begin();
		    itY!=itX->second.end(); itY++)
		{
			varID y = itY->first; 
			if((tgtAnnotName=="" && y.numAnnotations()==0) || y.getAnnotation(tgtAnnotName)==tgtAnnotVal)
				changedVarsY.insert(y);
		}
		
		modified = (changedVarsY.size()>0) || modified;
		
		// Change all the x->y mappings for all variables y for which we need to remove the annotation
		for(varIDSet::iterator it=changedVarsY.begin(); it!=changedVarsY.end(); it++)
		{
			varID y = *it;
			// Add the annotation from y to produce y'
			y.addAnnotation(newAnnotName, newAnnotVal);
			
			// Move the x->y mapping to become the x->y' mapping
			(itX->second)[y] = (itX->second)[*it];
			(itX->second).erase(*it);
		}
	}
	
	modified = (changedVarsX.size()>0) || modified;
	
	// Change all the x->? mappings for all variables x for which we need to remove the annotation
	for(varIDSet::iterator it=changedVarsX.begin(); it!=changedVarsX.end(); it++)
	{
		varID x = *it;
		// Add the annotation from x to produce x'
		x.addAnnotation(newAnnotName, newAnnotVal);
			
		// Move the x->? mapping to become the x'->? mapping
		vars2Value[x] = vars2Value[*it];
		vars2Value.erase(*it);
	}

	// === arrays ===
	varIDSet newArrays;
	for(varIDSet::iterator it=arrays.begin(); it!=arrays.end(); it++)
	{
		varID var = *it;
		if((tgtAnnotName=="" && var.numAnnotations()==0) || var.getAnnotation(tgtAnnotName)==tgtAnnotVal)
			modified = var.addAnnotation(newAnnotName, newAnnotVal) || modified;
		newArrays.insert(var);
	}
	arrays = newArrays;
	
	// === scalars ===
	varIDSet newScalars;
	for(varIDSet::iterator it=scalars.begin(); it!=scalars.end(); it++)
	{
		varID var = *it;
		if((tgtAnnotName=="" && var.numAnnotations()==0) || var.getAnnotation(tgtAnnotName)==tgtAnnotVal)
			modified = var.addAnnotation(newAnnotName, newAnnotVal) || modified;
		newScalars.insert(var);
	}
	scalars = newScalars;
	
	// === divVariables ===
	varIDSet newDivVariables;
	for(varIDSet::iterator it=divVariables.begin(); it!=divVariables.end(); it++)
	{
		varID var = *it;
		if((tgtAnnotName=="" && var.numAnnotations()==0) || var.getAnnotation(tgtAnnotName)==tgtAnnotVal)
			modified = var.addAnnotation(newAnnotName, newAnnotVal) || modified;
		newDivVariables.insert(var);
	}
	divVariables = newDivVariables;
	
	return modified;
}

// Adds a new range into this constraint graph 
/*void ConstrGraph::addRange(varID rangeVar)
{
	arrays.insert(rangeVar);
	emptyRange[rangeVar] = false;
}*/

/**** Transfer Function-Related Updates ****/

// updates the constraint graph with the information that x*a = y*b+c
// returns true if this causes the constraint graph to change and false otherwise
bool ConstrGraph::assign(const varAffineInequality& cond)
{
	return assign(cond.getX(), cond.getY(), cond.getA(), cond.getB(), cond.getC());
}

bool ConstrGraph::assign(varID x, varID y, const affineInequality& ineq)
{
	return assign(x, y, ineq.getA(), ineq.getB(), ineq.getC());
}

bool ConstrGraph::assign(varID x, varID y, int a, int b, int c)
{
	//printf("ConstrGraph::assign, x=%s, y=%s, a=%d, b=%d c=%d\n", x.str().c_str(), y.str().c_str(), a, b, c);
	
	bool modified = false;
	map<varID, map<varID, affineInequality> >::iterator mapIter;
		
	// if the graph is already bottom, do nothing more
	if(isBottom())
		return modified;
	
	// we do not support constraints between contents of different arrays
	if(isArray(x) && isArray(y))
		return modified;

	// if x is an array with an empty range, this range cannot be further changed
	if(isArray(x) && emptyRange[x])
		return modified;
		
	// if y is an array with an empty range, this range cannot be further changed
	if(isArray(y) && emptyRange[y])
		return modified;
	
	modifiedVars.insert(x);
	newConstrVars.insert(x);
	
	// x = x*b + c
	if(x == y && a==1)
	{
		//varID divX = getDivScalar(x);
		
		// remove x's divisibility variable from the constraint graph and add the divisibility constraints
		addDivVar(x, true);
		
		// iterate over all other variables i and update all the i -> x constraints
		for(map<varID, map<varID, affineInequality> >::iterator iterI = vars2Value.begin(); 
		    iterI != vars2Value.end(); iterI++)
		{
			//const varID& i = iterI->first;
			/* // don't update the connection between x and its divisibility variable
			if(i == divX) continue;*/
				
			// update all i->x pairs
			for(map<varID, affineInequality>::iterator iterJ = iterI->second.begin();
			    iterJ != iterI->second.end(); iterJ++)
			{
				const varID& j = iterJ->first;
				
				if(j == x)
				{
					// if x and z have a known constraint relationship
					if(iterJ->second.getLevel() == affineInequality::constrKnown)
					{
						affineInequality& constrIX = iterJ->second;
						// original constraint:
						// i*a <= x*b + c AND x' ==> x*b'+c'
						// i*a - c <= x*b
						// new constraint:
						// x'*b = (x*b'+c')*b = x*b'*b + c'*b
						// x'*b >= (i*a-c)*b' + c'*b
						// x'*b >= i*a*b' - c*b' + c'*b
						// i*a*b' <= x'*b + c*b' - c'*b
						
						//cout << "    assign() new constraint: "<<i.str()<<"*"<<(constrIX.getA()*b)<<" <= "<<x.str()<<"*"<<(constrIX.getB())<<" + "<<(constrIX.getC()*b - c*constrIX.getB())<<"\n";
						// update the constraints
						modified = constrIX.set(constrIX.getA()*b, constrIX.getB(), constrIX.getC()*b - c*constrIX.getB()) || modified;
						
						modifiedVars.insert(iterI->first);
						newConstrVars.insert(iterI->first);
					}
				}
			}
		}
		
		// iterate over all of x->z constraints
		map<varID, affineInequality>& xMap = vars2Value[x];
		for(map<varID, affineInequality>::iterator iterZ = xMap.begin(); iterZ != xMap.end(); iterZ++)
		{
			//const varID& z = iterZ->first;
			/* // don't update the connection between x and its divisibility variable
			if(z == divX) continue;*/
				
			affineInequality& constrXZ = iterZ->second;
			
			// if x and z have a known constraint relationship
			if(constrXZ.getLevel() == affineInequality::constrKnown)
			{
				// original constraint:
				// x*a <= z*b + c AND x' = x*b'+c'
				// new constraint:
				// x'*a = (x*b'+c')*a = x*b'*a + c'*a
				// x'*a = x*a*b' + c'*a <= (z*b + c)*b' + c'*a
				// x'*a <= z*b*b' + c*b' + c'*a
				
				//cout << "    assign() new constraint: "<<x.str()<<"*"<<(b*constrXZ.getA())<<" <= "<<z.str()<<"*"<<(constrXZ.getB()*b)<<" + "<<(constrXZ.getC()*b + c*constrXZ.getA())<<"\n";
				// update the constraints
				modified = constrXZ.set(constrXZ.getA(), constrXZ.getB()*b, constrXZ.getC()*b + c*constrXZ.getA()) || modified;
				
				modifiedVars.insert(iterZ->first);
				newConstrVars.insert(iterZ->first);
			}
		}
	}
	// case x*a = y*b + c
	else if(x!=y)
	{
		// disconnect all variables from x
		eraseVarConstr(x);
		
		//cout << "    assign() new constraint: "<<x.str()<<"*"<<a<<" <= "<<y.str()<<"*"<<b<<" + "<<c<<"\n";
		
		// x*a <= y*b + c
		setVal(x, y, a, b, c);
		// y*b <= x*a - c
		setVal(y, x, b, a, 0-c);
		
		modifiedVars.insert(y);
		newConstrVars.insert(y);
		
		modified = true;
	}
	// x*a = x*1
	else if(x==y && b==1 && c==0)
	{
		ROSE_ASSERT(0);
	}
	else
		// x*a = x*b + c
		ROSE_ASSERT(0);
	
	initialized = true; // this constraint graph is now definitely initialized
	
	constrChanged = constrChanged || modified;
	
	return modified;
}

/*// Undoes the i = j + c assignment for backwards analysis
void
ConstrGraph::undoAssignment( quad i, quad j, quad c )
{
	m_quad2str::iterator mapIter, mapIter2;

	// j = j+c 
	// given that i's pre-assignment value is unknown, remove all links
	// from i to anything else
	if ( i != j )
	{
		// iterate over all pairs of variables in vars2Name
		for ( mapIter = vars2Name.begin(); mapIter != vars2Name.end(); mapIter++ ) 
			for ( mapIter2 = vars2Name.begin(); mapIter2 != vars2Name.end(); mapIter2++ ) 
			{
				quad x = mapIter->first, y = mapIter2->first;
				if ( x != y )
					if ( x == i || y == i )
						setVal( x, y, INF );
					else
						setVal( x, y, getVal( x, y ) );
			}
	}
	// i = i + c
	// decrement i by c
	else
	{
#ifdef DEBUG_FLAG2
		cout << "i = i + c ---- reverting: " << vars2Name[i] << "="
				 << vars2Name[i] << "+" << c << "\n";
#endif

		// iterate over all variables
		for ( mapIter = vars2Name.begin(); mapIter != vars2Name.end(); mapIter++ )
		{
			quad y = mapIter->first;
			// change the connection between the current variable and i to account
			// for i being c smaller than before
			if ( y != i )
			{
				quad val = getVal(i, y);
				setVal( i, y, (val == INF ? INF : val - c) );
				val = getVal(y, i);
				setVal( y, i, (val == INF ? INF : val + c) );
			}
		}
	}
	
	initialized = true; // this constraint graph is now definitely initialized
}*/

/*
// kills all links from variable x to every other variable
void
ConstrGraph::killVariable( quad x )
{
	m_quad2str::iterator mapIter;
	for ( mapIter = vars2Name.begin(); mapIter != vars2Name.end(); mapIter++ )
		if (mapIter->first != x)
		{
			if (getVal(mapIter->first, x) != INF)
				setVal(mapIter->first, x, INF);
			if (getVal(x, mapIter->first) != INF)
				setVal(x, mapIter->first, INF);
		}
	
	initialized = true; // this constraint graph is now definitely initialized
}
*/

// add the condition (x*a <= y*b + c) to this constraint graph
// returns true if this causes the constraint graph to change and false otherwise
bool ConstrGraph::assertCond(const varAffineInequality& cond)
{
	/*cout << "assertCond cond.getX()="<<cond.getX().str()<<"\n";
	cout << "assertCond cond.getY()="<<cond.getY().str()<<"\n";*/
	return assertCond(cond.getX(), cond.getY(), cond.getIneq());
}

// add the condition (x*a <= y*b + c) to this constraint graph
// returns true if this causes the constraint graph to change and false otherwise
bool ConstrGraph::assertCond(const varID& x, const varID& y, const affineInequality& ineq)
{
	bool modified = false;
	
	// Note: assertCond doesn't check whether x and y are arrays 
	//   with empty ranges only because setVal() does this already
	affineInequality* constr = getVal(x, y);
	//printf("    assertCond(%s, %s) constr=%p\n", x.str().c_str(), y.str().c_str(), constr);
	// if there is already a constraint between x and y, update it
	if(constr)
	{
		(*constr) *= ineq;
		modified = true;
	}
	// else, create a new constraint
	else
	{
		modified = setVal(x, y, ineq) || modified;
		
		//affineInequality* constrV1V2 = getVal(x, y);
		//printf("x=%s, y=%s, constrXY=%p\n", x.str().c_str(), y.str().c_str(), constrV1V2);
	}
	
	constrChanged = constrChanged || modified;
	return modified;
}

// Add the condition (x*a <= y*b + c) to this constraint graph. The addition is done via a conjunction operator, 
// meaning that the resulting graph will be left with either (x*a <= y*b + c) or the original condition, whichever is stronger.
// returns true if this causes the constraint graph to change and false otherwise
bool ConstrGraph::assertCond(const varID& x, const varID& y, int a, int b, int c)
{
	bool modified = false;
	
	// Note: assertCond doesn't check whether x and y are arrays 
	//   with empty ranges only because setVal() does this already
	affineInequality* constr = getVal(x, y);
//printf("    assertCond(%s, %s, %d, %d, %d) constr=%p\n", x.str().c_str(), y.str().c_str(), a, b, c, constr);
	// if there is already a constraint between x and y, update it
	if(constr)
	{
		affineInequality newConstr(a, b, c, x==zeroVar, y==zeroVar, getVarSign(x), getVarSign(y));
		(*constr) *= newConstr;
		modified = true;
	}
	// else, create a new constraint
	else
	{
		modified = setVal(x, y, a, b, c) || modified;
		
		//affineInequality* constrV1V2 = getVal(x, y);
		//printf("x=%s, y=%s, constrXY=%p\n", x.str().c_str(), y.str().c_str(), constrV1V2);
	}
	
	constrChanged = constrChanged || modified;
	return modified;
}

// add the condition (x*a = y*b + c) to this constraint graph
// returns true if this causes the constraint graph to change and false otherwise
bool ConstrGraph::assertEq(const varAffineInequality& cond)
{
	return assertEq(cond.getX(), cond.getY(), cond.getA(), cond.getB(), cond.getC());
}

bool ConstrGraph::assertEq(varID x, varID y, const affineInequality& ineq)
{
	return assertEq(x, y, ineq.getA(), ineq.getB(), ineq.getC());	
}

bool ConstrGraph::assertEq(const varID& x, const varID& y, int a, int b, int c)
{
	bool modified = false;
	// x*a <= y*b + c
	modified = assertCond(x, y, a, b, c) || modified;
	// y*b <= x*a - c
	modified = assertCond(y, x, b, a, 0-c) || modified;
	
	return modified;
}

// Cuts i*b+c from the given array's range if i*b+c is either at the very bottom
//    or very bottom of its range. In other words, if (i*b+c)'s range overlaps the  
//    array's range on one of its edges, the array's range is reduced by 1 on 
//    that edge
// returns true if this causes the constraint graph to change and false otherwise
bool ConstrGraph::shortenArrayRange(varID array, varID i, int b, int c)
{
	bool modified = false;
	
	//cout << "shortenArrayRange("<<array.str()<<", "<<i.str()<<", "<<c<<")\n";
	
	affineInequality *Ai = getVal(array, i);
	affineInequality *iA = getVal(i, array);
	ROSE_ASSERT(Ai || iA);
	
	modifiedVars.insert(array);
	newConstrVars.insert(array);
	modifiedVars.insert(i);
	newConstrVars.insert(i);
	
	if(Ai)
	{
		// if $A <= i*b + c (i.e. i*b+c is actually at the bottom of array's range)
		if(Ai->getA()==1 && Ai->getB()==b && Ai->getC()==c)
			modified = Ai->setC(c-1) || modified;
	}
	
	if(iA)
	{
		// if i*b <= $A - c (i.e. i*b+c is actually at the bottom of array's range)
		if(iA->getA()==b && iA->getB()==1 && iA->getC()==(0-c))
			modified = iA->setC((0-c) - 1) || modified;
	}
		
	initialized = true; // this constraint graph is now definitely initialized
	
	constrChanged = constrChanged || modified;
	
	return modified;
}

/**** Dataflow Functions ****/


// returns the sign of the given variable
affineInequality::signs ConstrGraph::getVarSign(const varID& var)
{
/*	affineInequality* constrZeroVar = getVal(zeroVar, var);
	affineInequality* constrVarZero = getVal(var, zeroVar);
	
	affineInequality::signs varSign = affineInequality::unknownSgn;

	if(constrZeroVar && constrZeroVar->getC()==0 &&
	   constrVarZero && constrVarZero->getC()==0)
		varSign  = affineInequality::eqZero;
	else if(constrZeroVar && constrZeroVar->getC()<=0)
		varSign = affineInequality::posZero;
	else if(constrVarZero && constrVarZero->getC()<=0)
		varSign = affineInequality::negZero;*/
	
	FiniteVariablesProductLattice* sgnLattice = getSgnLattice(var);
	if(sgnLattice)
	{
		SgnLattice* sign = dynamic_cast<SgnLattice*>(sgnLattice->getVarLattice(func, var));
		if(sign)
		{
			//cout << "    getVarSign() "<<var.str()<<" : "<<sign->str("")<<"\n";
			if(sign->getLevel() == SgnLattice::eqZero)
				return affineInequality::eqZero;
			else if(sign->getLevel() == SgnLattice::sgnKnown)
				if(sign->getSgnState() == SgnLattice::posZero)
					return affineInequality::posZero;
				if(sign->getSgnState() == SgnLattice::negZero)	
					return affineInequality::negZero;
		}
		/*else
			cout << "    getVarSign() "<<var.str()<<" : NULL\n";*/
	}
	
	return affineInequality::unknownSgn;
}

bool ConstrGraph::isEqZero(const varID& var)
{
/*	if(var==zeroVar) return true;
	// a divisibility scalar is =0 if its original variable is =0 and the remainder ==0
	if(isDivScalar(var))
	{
		varID origVar = divVar2OrigVar[var];
		DivLattice* d = dynamic_cast<DivLattice*>(divL->getVarLattice(func, origVar));
		if(d->getLevel() == DivLattice::divKnown)
			return d->getValue()==0;
	}
	else
	{	
		DivLattice* d = dynamic_cast<DivLattice*>(divL->getVarLattice(func, var));
		
		if(d->getLevel() == DivLattice::valKnown)
			return d->getValue()==0;
	}
	return false;*/
	
	/*affineInequality* constrZeroVar = getVal(zeroVar, var);
	affineInequality* constrVarZero = getVal(var, zeroVar);

	if(constrZeroVar && constrZeroVar->getC()==0 &&
	   constrVarZero && constrVarZero->getC()==0)
		return true;

	return false;*/
	
	return eqVars(zeroVar, var);
}

// Returns true if v1*a = v2*b + c and false otherwise
bool ConstrGraph::eqVars(const varID& v1, const varID& v2, int a, int b, int c)
{
	return v1==v2 || (lteVars(v1, v2, a, b, c) && lteVars(v2, v1, b, a, 0-c));
}

// If v1*a = v2*b + c, sets a, b and c appropriately and returns true. 
// Otherwise, returns false.
bool ConstrGraph::isEqVars(const varID& v1, const varID& v2, int& a, int& b, int& c)
{
	// If v1*constrV1V2.getA() <= v2*constrV1V2.getB() + constrV1V2.getC() AND
	//    v1*constrV1V2.getA() >= v2*constrV1V2.getB() + constrV1V2.getC()
	affineInequality* constrV1V2 = getVal(v1, v2);
	if(constrV1V2 && lteVars(v2, v1, constrV1V2->getB(), constrV1V2->getA(), 0-constrV1V2->getC()))
	{
		a = constrV1V2->getA();
		b = constrV1V2->getB();
		c = constrV1V2->getC();
		return true;
	}
	else
		return false;
}

// Returns a list of variables that are equal to var in this constraint graph as a list of pairs
// <x, ineq>, where var*ineq.getA() = x*ineq.getB() + ineq.getC()
map<varID, affineInequality> ConstrGraph::getEqVars(varID var)
{
	map<varID, affineInequality> res;
	for(map<varID, affineInequality>::iterator it=vars2Value[var].begin();
	    it!=vars2Value[var].end(); it++)
	{
		// var*a <= x*b + c
		affineInequality& constrVarX = it->second;
		// x*b <= var*a - c
		affineInequality* constrXVar = getVal(it->first, var);
		if(constrXVar)
			if(constrVarX.getA() == constrXVar->getB() &&
			   constrVarX.getB() == constrXVar->getA() &&
			   constrVarX.getC() == (0-constrXVar->getC()))
			{
				res[it->first] = constrVarX;
			}
	}
	return res;
}

// Returns true if v1*a <= v2*b + c and false otherwise
bool ConstrGraph::lteVars(const varID& v1, const varID& v2, int a, int b, int c)
{
	if(v1==v2) return true;
	
	affineInequality* constrV1V2 = getVal(v1, v2);
	/*if(constrV1V2)
		cout << "lteVars("<<v1.str()<<", "<<v2.str()<<", "<<a<<", "<<b<<", "<<c<<"), constrV1V2="<<constrV1V2->str()<<"\n";
	else
		cout << "lteVars("<<v1.str()<<", "<<v2.str()<<", "<<a<<", "<<b<<", "<<c<<"), constrV1V2=NULL\n";*/
	if(constrV1V2)
		return constrV1V2->getA()==a &&
		       constrV1V2->getB()==b &&
		       // stronger constraints (smaller c) imply looser constraints (larger c)
		       constrV1V2->getC()<=c;
	else
		return false;
}

// Returns true if v1*a < v2*b + c and false otherwise
bool ConstrGraph::ltVars(const varID& v1, const varID& v2, int a, int b, int c)
{
	return lteVars(v1, v2, a, b, c-1);
}

/*********************************
 **** ConstrGraph::leIterator ****
 *********************************/

// Class used to iterate over all the constraints x*a <= y*b + c for a given variable x
ConstrGraph::leIterator::leIterator(const ConstrGraph* parent,
                                    const map<varID, map<varID, affineInequality> >::iterator& curX)
{
	this->parent = parent;
	this->curX = curX;
}

ConstrGraph::leIterator::leIterator(const ConstrGraph* parent, 
                                    const varID& x)
{
	this->parent = parent;	
	this->x = x;
	curX = parent->vars2Value.find(x);
	if(parent->vars2Value.find(x)!=parent->vars2Value.end())
		curY = curX->second.begin();
}

bool ConstrGraph::leIterator::isDone() const
{
	return curX == parent->vars2Value.end() ||
	       curY == curX->second.end();
}

varAffineInequality ConstrGraph::leIterator::operator*() const
{
	ROSE_ASSERT(!isDone());
	varAffineInequality vai(x, curY->first, curY->second);
	return vai;	
}

void ConstrGraph::leIterator::operator ++ ()
{
	if(curX != parent->vars2Value.end())
		curY++;
}

void ConstrGraph::leIterator::operator ++ (int)
{
	if(curX != parent->vars2Value.end())
		curY++;
}

bool ConstrGraph::leIterator::operator==(const leIterator& otherIt) const
{
	return (isDone() && otherIt.isDone()) ||
	       (parent == otherIt.parent &&
	        x      == otherIt.x      &&
	        curX    == otherIt.curX  &&
	        curY    == otherIt.curY);
}

bool ConstrGraph::leIterator::operator!=(const leIterator& otherIt) const
{
	return !(*this == otherIt);
}

// Beginning and end points of the iteration over all constraints x*a <= y*b + c for a 
// given variable x.
ConstrGraph::leIterator ConstrGraph::leBegin(const varID& x)
{
	leIterator lei(this, x);
	return lei;
}

ConstrGraph::leIterator ConstrGraph::leEnd()
{
	leIterator lei(this, vars2Value.end());
	return lei;
}
/*********************************
 **** ConstrGraph::geIterator ****
 *********************************/

ConstrGraph::geIterator::geIterator()
{
	isEnd = true;
}

// Class used to iterate over all the constraints x*a <= y*b + c for a given variable y
ConstrGraph::geIterator::geIterator(const ConstrGraph* parent, const varID& y): parent(parent), y(y)
{
	
	isEnd = false;
	curX = parent->vars2Value.begin();
	curY = curX->second.begin();
	/*cout << "geIterator::geIterator()\n";
	cout << "geIterator::geIterator() curX==vars2Value.end() = "<<(curX==parent->vars2Value.end())<<"\n";
	if(curX!=parent->vars2Value.end())
	{
		cout << "geIterator::geIterator() curX="<<curX->first.str()<<" curX.size()="<<curX->second.size()<<" curY==curX->second.end() = "<<(curY==curX->second.end())<<"\n";
		if(curY!=curX->second.end())
		cout << "geIterator::geIterator() curY="<<curY->first.str()<<" curY.ineq="<<curY->second.str()<<"\n";
	}*/
	//if(curY->first != y)
		advance();
}

ConstrGraph::geIterator::geIterator(const ConstrGraph* parent, const varID& y,
           const map<varID, map<varID, affineInequality> >::iterator& curX,
           const map<varID, affineInequality>::iterator& curY): parent(parent), curX(curX), curY(curY), y(y)
{
	isEnd = false;
}

// Advances curX and curY by one step. Returns false if curX/curY is already at the
// end of parent->vars2Value and true otherwise (i.e. successful step).
bool ConstrGraph::geIterator::step()
{
	if(isEnd) return false;
	// If we've run out of x's, return false because we're done.
	if(curX == parent->vars2Value.end()) return false;
	/*else
	{
		 // Move x and y along until we hit the next non-empty x
		if(curY == curX->second.end())
		{
			do
			{
				// Move on the the beginning of the next x
				curX++;
				curY = curX->second.begin();
			}
			while(curY == curX->second.end());
		}
		else
		{
			// Move on to the next y in the current x
			curY++;
		}*/
		if(curY != curX->second.end())
		{
			// Move on to the next y in the current x
			curY++;
		}
	//}
	
	// If we've run out of y's for the current x, advance forward to the next
	// x that has an associated y. If there are none, return false.
	if(curY == curX->second.end())
	{
		do {
			curX++;
			if(curX == parent->vars2Value.end()) return false;
			curY = curX->second.begin();
		} while(curY == curX->second.end());
		return true;
	}
	// If we have another y for the current x, this is it
	else
		return true;
}

// Move curX/curY to the next x/y pair with a matching y (may leave curX/curY already satisfy this).
// Returns true if there are no more such pairs.
bool ConstrGraph::geIterator::advance()
{
	//cout << "geIterator::advance() isEnd="<<isEnd<<" isDone()="<<isDone()<<"\n";
	if(isEnd) return false;
	if(isDone()) return false;
	
	bool notDone=true;
	while(notDone && (curY==curX->second.end() || curY->first != y))
	{
		notDone = step();
		/*if(curX!=parent->vars2Value.end())
		{
			varAffineInequality vai(getX(), curY->first, curY->second);
			cout << "advance() "<<vai.str()<<"\n";
		}*/
	}
	
	return notDone;
}

bool ConstrGraph::geIterator::isDone() const
{
	/*if(!isEnd)
	{ cout << "geIterator::isDone() isEnd="<<isEnd<<" (curX == parent->vars2Value.end())="<<(curX == parent->vars2Value.end())<<"\n"; }
	if(!isEnd && curX != parent->vars2Value.end())
	{ cout << "geIterator::isDone() curX="<<curX->first.str()<<" (curY == curX->second.end())="<<(curY == curX->second.end())<<"\n"; }
	if(!isEnd && curX != parent->vars2Value.end() && curY != curX->second.end())
	{ cout << "geIterator::isDone() curY="<<curY->first.str()<<"\n"; }*/
	
	return isEnd || curX == parent->vars2Value.end()/* || curY == curX->second.end()*/;
}

const varID& ConstrGraph::geIterator::getX() const
{
	ROSE_ASSERT(!isDone());
	
	return curX->first;
}

varAffineInequality ConstrGraph::geIterator::operator*() const 
{ 
	ROSE_ASSERT(!isDone());
	varAffineInequality vai(getX(), curY->first, curY->second);
	return vai;
}

void ConstrGraph::geIterator::operator++()
{
	step();
	advance();
}

void ConstrGraph::geIterator::operator++(int)
{
	step();
	advance();
}

bool ConstrGraph::geIterator::operator==(const geIterator& otherIt) const
{
//	cout << "isEnd="<<isEnd<<"  otherIt.isEnd="<<otherIt.isEnd<<"\n";
	return (isEnd == otherIt.isEnd) ||
	       (isDone() == otherIt.isDone()) ||
	       (parent == otherIt.parent &&
	        y      == otherIt.y      &&
	        curX   == otherIt.curX   &&
	        curY   == otherIt.curY);
}

bool ConstrGraph::geIterator::operator!=(const geIterator& otherIt) const
{
	return !(*this == otherIt);
}

// Beginning and End points of the iteration over all constraints x*a <= y*b + c for a 
// given variable y.
ConstrGraph::geIterator ConstrGraph::geBegin(const varID& y)
{
	geIterator gei(this, y);
	return gei;
}

ConstrGraph::geIterator ConstrGraph::geEnd()
{
	geIterator gei;
	return gei;
}
	
	
// widens this from that and saves the result in this
// returns true if this causes this to change and false otherwise
bool ConstrGraph::widenUpdate(InfiniteLattice* that_arg)
{
	ConstrGraph* that = dynamic_cast<ConstrGraph*>(that_arg);
	
	// if this constraint graph is Bottom or Uninitialized, the widening is that
	if(isBottom() || !initialized)
	{
/*		bool initOld = initialized;
		initialized = true; // this constraint graph will now definitely be initialized
		
		// this constraint graph has changed if it wasn't initialized until now
		return !initOld;*/
		return copyFrom(*that);
	}
	// if that is Bottom or Uninitialized then we don't need to change this
	else if(that->isBottom() || !that->initialized)
	{
		//return copyFrom( cg );
		//setToBottom();
		//return true;
		return false;
	}
	// if both this and that are not bottom
	else
		return meetwidenUpdate(that, false, false);
}

// Widens this from that and saves the result in this, while ensuring that if a given constraint
// doesn't exist in that, its counterpart in this is not modified
// returns true if this causes this to change and false otherwise
bool ConstrGraph::widenUpdateLimitToThat(InfiniteLattice* that_arg)
{
	ConstrGraph* that = dynamic_cast<ConstrGraph*>(that_arg);
	
	// if this constraint graph is Bottom or Uninitialized, the widening is that
	if(isBottom() || !initialized)
	{
/*		bool initOld = initialized;
		initialized = true; // this constraint graph will now definitely be initialized
		
		// this constraint graph has changed if it wasn't initialized until now
		return !initOld;*/
		return copyFrom(*that);
	}
	// if that is Bottom or Uninitialized then we don't need to change this
	else if(that->isBottom() || !that->initialized)
	{
		//return copyFrom( cg );
		//setToBottom();
		//return true;
		return false;
	}
	// if both this and that are not bottom
	else
		return meetwidenUpdate(that, false, true);
}

// Computes the union of this constraint graph with cg, returning this union
//ConstrGraph *ConstrGraph::unionMe(const ConstrGraph &that)
// computes the meet of this and that and saves the result in this
// returns true if this causes this to change and false otherwise
// The meet is the union of the information from both constraint graphs. If a given variable pair
//    has a constraint in one graph but not the other, the resulting graph has this constraint.
//    If the pair has constraints in both graphs, their constraint in the resulting graph will be 
//    the union of these constraints.
bool ConstrGraph::meetUpdate(Lattice* that_arg)
{
	ConstrGraph* that = dynamic_cast<ConstrGraph*>(that_arg);

	// if this constraint graph is uninitialized, the meet is that
	if(!initialized)
	{
if(debugLevel>=1) cout << "(!initialized)\n";
		return copyFrom(*that);
	}
	// if one graph is strictly looser than the other, the union = the looser graph
	else if(*that <<= *this)
	{
if(debugLevel>=1) cout << "(*that <<= *this)\n";
		// this is already the union
		return false;
	}
	else if(*this <<= *that)
	{
if(debugLevel>=1) cout << "(*this <<= *that)\n";
		return copyFrom(*that);
	}
	// else, iterate over all constraints in both constraint graphs and union them individually
	else
	{
if(debugLevel>=1) cout << "calling meetwidenUpdate\n";
		return meetwidenUpdate(that, true, false);
	}
}

// Meet this and that and saves the result in this, while ensuring that if a given constraint
// doesn't exist in that, its counterpart in this is not modified
// returns true if this causes this to change and false otherwise
bool ConstrGraph::meetUpdateLimitToThat(InfiniteLattice* that_arg)
{
	ConstrGraph* that = dynamic_cast<ConstrGraph*>(that_arg);
	
	// if one graph is strictly looser than the other, the union = the looser graph
	if(*that <<= *this)
	{
		// this is already the union
		return false;
	}
	else if(*this <<= *that)
	{
		return copyFrom(*that);
	}
	// if both this and that are not bottom
	else
		return meetwidenUpdate(that, true, true);
}

// Unified function for meet and widening
// if meet == true, this function computes the meet and if =false, computes the widening
// if limitToThat == true, if a given constraint does not exist in that, this has no effect on the meet/widening
bool ConstrGraph::meetwidenUpdate(ConstrGraph* that, bool meet, bool limitToThat)
{
	bool modified = false;
	affineInequality topIneq;
	topIneq.setToTop();
/*if(meet)
	printf("meetUpdate()\n");	
else
	printf("widenUpdate()\n");	
cout << "   this: "<<str("")<<"\n";
cout << "   that: "<<that->str("")<<"\n";*/
	
/*	// if one graph is strictly looser than the other, the result = the looser graph
	if(*that <<= *this)
	{
		cout << "    this is looser than that, keeping this\n";
		// this is already the union/widening
	}
	else if(*this <<= *that)
	{
		modified = copyFrom(*that) || modified;
	}
	// if both this and that are not bottom
	else
	{*/
		// This constraint graph will be modified if it is currently uninitialized
		modified = !initialized;
		
		initialized = true; // this constraint graph will now definitely be initialized
		
		// Iterate over all constraints in both constraint graphs and union/widen them individually
		//printf("vars2Value.size()=%d, that->vars2Value.size()=%d\n", vars2Value.size(), that->vars2Value.size());
		map<varID, map<varID, affineInequality> >::iterator itThisX, itThatX;
		for(itThisX = vars2Value.begin(), itThatX = that->vars2Value.begin(); 
		    itThisX!=vars2Value.end() && itThatX!=that->vars2Value.end(); )
		{
			//cout << "itThisX = "<<itThisX->first.str()<< "  itThatX = "<<itThatX->first.str()<<"\n";
			
			// If itThisX->first exists in this, but not in that
			if(itThisX->first < itThatX->first)
			{
				//cout << "    thisX="<<itThisX->first.str()<<" No thatX, limitToThat="<<limitToThat<<"\n";
				/*// do nothing, since all <itThisX->first -> ???> constraints in that are assumed to be top*/
				
				// Only bother with this case if we've been asked to
				if(!limitToThat)
				{
					// Change all the <x -> ???> constraints in this to top, since the corresponding constraints in that are =top
					itThisX->second.clear();
					modifiedVars.insert(itThisX->first);
				}
				itThisX++;
			}
			// If itThatX->first exists in that, but not in this
			else if(itThisX->first > itThatX->first)
			{
				//cout << "    No thisX, itThatX="<<itThatX->first.str()<<"\n";
				/*// we know that all <itThatX->first -> ???> constraints in this are bottom (unknown)
				// as such, we need to raise them up to their level in that
				vars2Value[itThatX->first] = itThatX->second;
				modified = true;*/
				
				// All the <x -> ???> constraints in this are =top, so we don't need to change them, since (top union/widen ???) => top
				itThatX++;
			}
			// If, itThisX->first exists in both this and that
			else
			{
				//cout << "    thisX="<<itThisX->first.str()<<" itThatX="<<itThatX->first.str()<<"\n";
				
				//varIDSet to_delete;
				varID x = itThisX->first;
				/*affineInequality::signs xSign = getVarSign(x);
				ROSE_ASSERT(xSign == that->getVarSign(x));*/
				
				// union/widen each <x->???> constraint
				map<varID, affineInequality>::iterator itThisY, itThatY;
				for(itThisY = itThisX->second.begin(), itThatY = itThatX->second.begin();
				    itThisY!=itThisX->second.end() && itThatY!=itThatX->second.end(); )
				{
					varID y = itThisY->first;
					//cout << "        itThisY = "<<itThisY->first.str()<< "  itThatY = "<<itThatY->first.str()<<"\n";
					
					// if itThisY->first exists in this, but not in that
					if(itThisY->first < itThatY->first)
					{
						/*// do nothing, since all <itThisX->first -> itThisY->first> constraints in that are assumed to be bottom*/
						// Only bother with this case if we've been asked to
						if(!limitToThat)
						{
							// change the <x -> y> constraint in this to top, since the corresponding constraint in that is =top
							modified = itThisY->second.setToTop() || modified;
							modifiedVars.insert(itThisY->first);
							modifiedVars.insert(itThisX->first);
// !!! above should be a removal
						}
						
						itThisY++;
					}
					// if itThatY->first exists in that, but not in this
					else if(itThisY->first > itThatY->first)
					{
						/*// we know that the <itThatX->first -> itThatY->first> constraint in this is bottom
						// as such, we need to raise it to its counterpart in that
						itThisX->second[itThatY->first] = itThatY->second;
						modified = true;*/
						
						// The <x -> y> constraint in this is =top, so we don't need to change it, since (top union/widen ???) => top
						itThatY++;
					}
					// else, <itThisX->first -> itThisY->first> exists in both this and that
					else
					{
						// Union
						if(meet)
						{
							//cout << "meetwidenUpdate "<<itThisX->first.str()<<"=>"<<itThisY->first.str()<<" : "<<itThisY->second.str()<<" && "<<itThatY->second.str()<<" => ";
							// union this constraint in this with the corresponding constraint in that
							modified = itThisY->second.unionUpd(itThatY->second) || modified;
							modifiedVars.insert(itThisX->first);
							modifiedVars.insert(itThisY->first);
							newConstrVars.insert(itThisX->first);
							newConstrVars.insert(itThisY->first);
							//cout << itThisY->second.str()<<"\n";
						}
						// Widening
						else
						{
							// widen this constraint in this with the corresponding constraint in that
							if(itThisY->second != itThatY->second)
							{
								//cout <<itThisX->first.str() << " -> "<<itThisY->first.str()<<"\n";
								//cout <<"itThisY->second = "<<itThisY->second.str()<<"\n";
								//cout <<"itThatY->second = "<<itThatY->second.str()<<"\n";
								//itThisY->second ^= itThatY->second;
								if(itThisY->second.semLessThan(itThatY->second, isEqZero(x), isEqZero(y)))
								{
									/* // Before we remove this constraint, first check if the constraint between x and y's 
									// divisibility variables has changed. If one exists and it hasn't changed, simply
									// copy over the x-y constraint from that to this, since the divisibility variables
									// represent the same information as the regular variables
									varID divX = ConstrGraph::getDivScalar(x);
									varID divY = ConstrGraph::getDivScalar(y);
									affineInequality* thisDivXY = getVal(divX, divY);
									affineInequality* thatDivXY = that->getVal(divX, divY);
									
									if(thisDivXY && thatDivXY && 
									   thisDivXY->getLevel()!=affineInequality::top && 
									   thatDivXY->getLevel()!=affineInequality::top &&
									   thisDivXY == thatDivXY)
									{
										itThisY->second = itThatY->second;
									}
									else*/
									itThisY->second.setToTop();
									modifiedVars.insert(itThisX->first);
									modifiedVars.insert(itThisY->first);
								}
	// !!! above may result in a removal if itThisY->second becomes top
								//cout <<"itThisY->second ^ itThatY->second = "<<itThisY->second.str()<<"\n";
								modified = true;
							}
						}
						
						itThisY++;
						itThatY++;
					}
				}
				
				/*// if there exist any constraint for x in that that is not in this
				if(itThatY!=itThatX->second.end())
				{
					// copy them over to this
					for(; itThatY!=itThatX->second.end(); itThatY++)
						itThisX->second[itThatY->first] = itThatY->second;
				}*/
				// if there exist any constraint for x in that that is not in this, leave this
				// alone because the corresponding constraints in this are top and (top widen ???) => top
				
				// if there exist any constraint for x in this that is not in that, set them to top
				for(; itThisY!=itThisX->second.end(); itThisY++)
				{
					//cout << "meetwidenUpdate "<<itThisX->first.str()<<"=>"<<itThisY->first.str()<<" doesn't exist in that\n";
					// Only bother with this case if we've been asked to
					if(!limitToThat)
					{
						itThisY->second.setToTop();
						modifiedVars.insert(itThisX->first);
						modifiedVars.insert(itThisY->first);
					}
				}
				
				//cout << "pre-increment, itThisX==vars2Value.end()="<<(itThisX==vars2Value.end())<<" && itThatX==that->vars2Value.end()="<<(itThatX==that->vars2Value.end())<<"\n";
				itThisX++;
				itThatX++;
			}
			//cout << "bottom, itThisX==vars2Value.end()="<<(itThisX==vars2Value.end())<<" && itThatX==that->vars2Value.end()="<<(itThatX==that->vars2Value.end())<<"\n";
		}
		
//cout << "   this loop end: "<<str("")<<"\n";
//cout << "   modified = "<<modified<<"\n";
		
		/*// if there exist any variables s.t. they have constraints in that but not in this
		if(itThatX!=that->vars2Value.end())
		{
			// copy them over to this
			for(; itThatX!=that->vars2Value.end(); itThatX++)
				vars2Value[itThatX->first] = itThatX->second;
		}*/
		// if there exist any variables s.t. they have constraints in that but not in this, leave them
		// alone because the corresponding constraints in this are top and (top widen ???) => top
		
		// if there exist any variables s.t. they have constraints in this but not in that, set them to top
		if(itThisX!=vars2Value.end() && !limitToThat)
		{
			map<varID, affineInequality>::iterator itThisY;
			for(itThisY = itThisX->second.begin(); itThisY!=itThisX->second.end(); itThisY++)
			{
				// Only bother with this case if we've been asked to
				if(!limitToThat)
				{
					itThisY->second.setToTop();
					modifiedVars.insert(itThisX->first);
					modifiedVars.insert(itThisY->first);
				}
			}
		}
		
		if(meet)
			// iterate over all the arrays in this constraint graph
			for(varIDSet::iterator curArray = arrays.begin(); curArray != arrays.end(); curArray++)
			{
				bool origEmptyRange = emptyRange[*curArray];
				emptyRange[*curArray] = emptyRange[*curArray] && that->emptyRange[*curArray];
				modified = modified || (origEmptyRange != emptyRange[*curArray]);
				// if the array has an empty range in the union, it should have no connections
				// to any other variables in the union (this should have been true before the union, so we're just asserting this)
				if(emptyRange[*curArray])
				{
					// verify this fact
					bool old_constrChanged = constrChanged;
					constrChanged = false;
					eraseVarConstrNoDiv(*curArray);
					// eraseVarConstr should have been a noop
					ROSE_ASSERT(!constrChanged);
					constrChanged = old_constrChanged;
				}
			}
		
		// close if the widening will cause this graph to change
		if (modified)
		{
			constrChanged = true;
			transitiveClosure();
		}
//	}	
	
/*	// Merge the scalars, arrays and divVariables sets of the two objects
	for(varIDSet::const_iterator it=that->scalars.begin(); it!=that->scalars.end(); it++)
		scalars.insert(*it);
	for(varIDSet::const_iterator it=that->arrays.begin(); it!=that->arrays.end(); it++)
		arrays.insert(*it);
	for(varIDSet::const_iterator it=that->divVariables.begin(); it!=that->divVariables.end(); it++)
		divVariables.insert(*it);*/
	
	/*cout << "meetwidenUpdate scalars = ";
	for(varIDSet::iterator it=scalars.begin(); it!=scalars.end(); it++)
	{ cout << (*it).str() << ", "; }
	cout << "\n";*/
	
//cout << "   this final: "<<str("")<<"\n";
//cout << "   modified = "<<modified<<"\n";
	return modified;
}

// <from LogicalCond>
bool ConstrGraph::andUpd(LogicalCond& that_arg)
{
	ConstrGraph* that = dynamic_cast<ConstrGraph*>(&that_arg);
	return andUpd(that);
}

bool ConstrGraph::andUpd(ConstrGraph* that)
{
	bool modified = false;
	
	// add all the constraints in that to this, one by one
	for(map<varID, map<varID, affineInequality> >::iterator itX = that->vars2Value.begin(); 
	    itX != that->vars2Value.end(); itX++)
	{
		for(map<varID, affineInequality>::iterator itY = itX->second.begin();
		    itY!=itX->second.end(); itY++)
		{
			modified = assertCond(itX->first, itY->first, 
			                      itY->second.getA(), itY->second.getB(), itY->second.getC()) || modified;
		}
	}
	
/*	// Merge the scalars, arrays and divVariables sets of the two objects
	for(varIDSet::const_iterator it=that->scalars.begin(); it!=that->scalars.end(); it++)
		scalars.insert(*it);
	for(varIDSet::const_iterator it=that->arrays.begin(); it!=that->arrays.end(); it++)
		arrays.insert(*it);
	for(varIDSet::const_iterator it=that->divVariables.begin(); it!=that->divVariables.end(); it++)
		divVariables.insert(*it);*/
	
	return modified;
}

// <from LogicalCond>
bool ConstrGraph::orUpd(LogicalCond& that_arg)
{
	ConstrGraph* that = dynamic_cast<ConstrGraph*>(&that_arg);

	// if one graph is strictly looser than the other, the union = the looser graph
	if(*that <<= *this)
	{
		// this is already the union
		return false;
	}
	else if(*this <<= *that)
	{
		return copyFrom(*that);
	}
	// else, iterate over all constraints in both constraint graphs and union them individually
	else
	{
		return meetwidenUpdate(that, true, false);
	}
}

// computes the transitive closure of the given constraint graph,
// and updates the graph to be that transitive closure
void ConstrGraph::transitiveClosure()
{
	int numSteps=0, numInfers=0, numFeasibleChecks=0, numLocalClosures=0;
	struct timeval startTime, endTime;
	if(profileLevel>=1)
	{
		cout << "transitiveClosure() <<<\n";
		gettimeofday(&startTime, NULL);
	}
	// don't take transitive closures in the middle of a transaction
	if(inTransaction) return;
	
	initialized = true; // this constraint graph will now definitely be initialized
	
#ifdef DEBUG_FLAG_TC
cout << "Beginning transitive closure\n";
cout << "    Pre-closure: \n" << str("    ") << "\n";
#endif
	//m_quad2str::iterator mIter1, mIter2, mIter3;
	bool modified = true;
	
	// First, compute the transitive closure of all non-array range variables. This determines
	// all the constraints on the current program state.
	
	// iterate until a fixed point is reached
	while (modified)
	{
		modified = false;
		// iterate through every triple of variables
		//for ( mIter1 = vars2Name.begin(); mIter1 != vars2Name.end(); mIter1++ )
		for(varIDSet::iterator itX = newConstrVars.begin(); itX!=newConstrVars.end(); itX++)
		{
//cout << "itX = "<<itX->str()<<"\n";
			varID x = *itX;
			varID divX = getDivScalar(x);
			
			for(varIDSet::iterator itY = scalars.begin(); itY!=scalars.end(); itY++)
			{
				varID y = *itY;
				varID divY = getDivScalar(y);
							
				// if x and y are different variables and they're not both constants
				//    (we don't want to do inference on constants since we can't learn anything more 
				//     about them and we might lose information because we're being conservative)
/*				// and one is not the other's divisibility variable*/
				if(x != y && ((x!=zeroVar && x!=oneVar) || (y!=zeroVar && y!=oneVar))
				   /*&& x!=divY && y!=divX*/)
				{
//cout << "itY = "<<itY->str()<<"\n";
					//quad xy = getVal(x, y);
					affineInequality* constrXY = NULL;
					// skip the rest if there is no x->y constraint
					//if(constrXY)
					{
						//for ( mIter3 = vars2Name.begin(); mIter3 != vars2Name.end(); mIter3++ )
						for(varIDSet::iterator itZ = scalars.begin(); itZ!=scalars.end(); itZ++)
						{
							numSteps++;
//cout << "itZ = "<<itZ->str()<<"\n";
							/*// Skip all array ranges
							if(isArray(mIter2->first))
								continue;
	
							quad z = mIter3->first;*/
							varID z = *itZ;
							// x, y and z are three different variables
							if(z!=x && z!=y)
							{
								if(!constrXY)
									constrXY = getVal(x, y);
									
								//quad xz = getVal(x, z), zy = getVal(z, y);
								affineInequality* constrXZ = getVal(x, z);
								affineInequality* constrZY = getVal(z, y);
								/*if(constrXZ) cout << "                  "<<x.str()<<"->"<<z.str()<<" = " << constrXZ->str(x, z, "") << "\n";
								if(constrZY) cout << "                  "<<z.str()<<"->"<<y.str()<<" = " << constrZY->str(z, y, "") << "\n";*/
								// if the x->z->y path results in a tighter constraint than the
								// x->y path, update the latter to the former
								//if ( xz != INF && zy != INF )
								if(constrXZ && constrXZ->getLevel()==affineInequality::constrKnown && 
									constrZY && constrZY->getLevel()==affineInequality::constrKnown)
								{
									numInfers++;
									affineInequality inferredXY(*constrXZ, *constrZY/*, x==zeroVar, y==zeroVar, 
									                            dynamic_cast<DivLattice*>(divL->getVarLattice(func, x)), 
									                            dynamic_cast<DivLattice*>(divL->getVarLattice(func, y)), z*/);
									//affineInequality *constrXY = getVal(x, y);
									
//printf("transitiveClosure() constrXY=%p\n", &inferredXY, constrXY);

									// if there doesn't exist an x-y constraint in the graph, add it
									if(!constrXY)
									{
#ifdef DEBUG_FLAG_TC
										cout << "    " << x.str() << "->" << y.str() << "\n";
										cout << "        Current = None\n";
										cout << "        Inferred("<<x.str()<<"->"<<z.str()<<"->"<<y.str()<<") = " << inferredXY.str(x, y, "") << "\n";
#endif

										setVal(x, y, inferredXY);
										constrXY = getVal(x, y);
										modified = true;
									}
									// else, if the inferred x-y constraint it strictly tighter than the current x-y constraint
									else if(inferredXY.semLessThan(*constrXY, isEqZero(x), isEqZero(y)))
									{
#ifdef DEBUG_FLAG_TC
										cout << "    " << x.str() << "->" << y.str() << "\n";
										cout << "        Current = " << constrXY->str(x, y, "") << "\n";
										cout << "        Inferred ("<<x.str()<<"->"<<z.str()<<"->"<<y.str()<<") = " << inferredXY.str(x, y, "") << "\n";
#endif
										// replace the current x-y constraint with the inferred one
										constrXY->set(inferredXY);
										modified = true;
									}
								}
							}
						}
					}
				}
			}
		}

		//numFeasibleChecks++;
		//// look for cycles
		//quad r = isFeasible();
		//if(r!=1) break;
	}
	numFeasibleChecks++;
	// look for cycles
	//quad r = isFeasible();
	
	// Reset the variable modification state
	modifiedVars.clear();
	newConstrVars.clear();

#ifdef DEBUG_FLAG_TC
	// check for cycles
	cout << "    transitiveClosure() feasibility r="<<isFeasible()<<"\n\n";
	cout << "    Constraints at the end of base transitiveClosure\n";
	cout << str("    ") << "\n";
#endif

	
	// Now compute the local transitive closures of all array ranges. The critical bit
	//    here is to only modify the constraints of each range relative to other constraint 
	//    variables without modifying the constraints of other variables to each other.
	//    the reason is that if this closure process creates a negative cycle in a 
	//    given array's constraints (i.e. makes its range empty), this cycle should 
	//    not transitively propagate to other variables and make this constraint graph top.
	//    The negative cycle applies to the array range and must be limited to this array range.
	
	// iterate over all arrays
	// !!! Need to incorporate arrays into the newConstrVars logic of variable modification
//	for(varIDSet::iterator curArray = arrays.begin(); 
//		 curArray != arrays.end(); curArray++)
//	{
//		numLocalClosures++;
//		localTransClosure(*curArray);
//	}

#ifdef DEBUG_FLAG_TC
	// check for cycles
	cout << "    transitiveClosure() feasibility ="<<isFeasible()<<"\n\n";
	cout << "    Constraints at the end of full transitiveClosure\n";
	cout << str("    ") << "\n";
#endif
/*
	// return appropriate flags for -1=Dead and 0=Bottom
	if (r == 0)
	{
		delete (d);
		d = NULL;
	}
	if (r == -1)
		d->dead = true;

#ifdef DEBUG_FLAG
	cout << "    Ending transitive closure\n";
#endif
	return r;*/
	if(profileLevel>=1) 
	{
		gettimeofday(&endTime, NULL);
		cout << "transitiveClosure() >>> numSteps="<<numSteps<<", numInfers="<<numInfers<<", numFeasibleChecks="<<numFeasibleChecks<<", numLocalClosures="<<numLocalClosures<<", numVars="<<scalars.size()<<", time="<<((double)((endTime.tv_sec*1000000+endTime.tv_usec)-(startTime.tv_sec*1000000+startTime.tv_usec)))/1000000.0<<"\n";
	}
}


// computes the transitive closure of the given constraint graph,
// focusing on the constraints of scalars that have divisibility variables
// we only bother propagating constraints to each such variable through its divisibility variable
void ConstrGraph::divVarsClosure()
{
	int numVarClosures=0, numFeasibleChecks=0;
	struct timeval startTime, endTime;
	if(profileLevel>=1)
	{
		cout << "divVarsClosure() <<<\n";
		gettimeofday(&startTime, NULL);
	}
	
	// don't take transitive closures in the middle of a transaction
	if(inTransaction) return;
	
	initialized = true; // this constraint graph will now definitely be initialized
	
#ifdef DEBUG_FLAG_TC
cout << "Beginning divVarsClosure\n";
cout << "    Pre-closure: \n" << str("    ") << "\n";
#endif
	bool modified = true;
	
	// First, compute the transitive closure of all non-array range variables. This determines
	// all the constraints on the current program state.
	
	// iterate until a fixed point is reached
	while (modified)
	{
		modified = false;
		// iterate through every triple of variables
		for(varIDSet::iterator itX = divVariables.begin(); itX!=divVariables.end(); itX++)
		{
			varID x = *itX;
			varID divX = getDivScalar(x);
			affineInequality* constrXDivX = getVal(x, divX);
			affineInequality* constrDivXX = getVal(divX, x);
			
			//affineInequality::signs xSign = getVarSign(x);
			
			for(varIDSet::iterator itY = scalars.begin(); itY!=scalars.end(); itY++)
			{
				varID y = *itY;
				//affineInequality::signs ySign = getVarSign(y);
				numVarClosures++;
				modified = divVarsClosure_perY(x, divX, y, constrXDivX, constrDivXX/*, xSign, ySign*/) || modified;
			}
			
			/*for(varIDSet::iterator itY = arrays.begin(); itY!=arrays.end(); itY++)
			{
				varID y = *itY;
				modified = divVarsClosure_perY(x, divX, y, constrXDivX, constrDivXX) || modified;
			}*/
		}

		numFeasibleChecks++;
		// look for cycles
		quad r = isFeasible();
		if(r!=1) break;
	}
	if(profileLevel>=1) 
	{
		gettimeofday(&endTime, NULL);
		cout << "divVarsClosure() >>> numVarClosures="<<numVarClosures<<", numFeasibleChecks="<<numFeasibleChecks<<", numVars="<<scalars.size()<<", time="<<((double)((endTime.tv_sec*1000000+endTime.tv_usec)-(startTime.tv_sec*1000000+startTime.tv_usec)))/1000000.0<<"\n";
	}
}

// The portion of divVarsClosure that is called for every y variable. Thus, given x and x' (x's divisibility variable)
// divVarsClosure_perY() is called for every scalar or array y to infer the x->y connection thru x->x'->y and
// infer the y->x connection thru x->x'->x
bool ConstrGraph::divVarsClosure_perY(const varID& x, const varID& divX, const varID& y, 
                                      affineInequality* constrXDivX, affineInequality* constrDivXX/*,
                                      affineInequality::signs xSign, affineInequality::signs ySign*/)
{
	int numInfers=0;
	struct timeval startTime, endTime;
	if(profileLevel>=1)
	{
		cout << "divVarsClosure_perY("<<x.str()<<", "<<divX.str()<<", "<<y.str()<<") <<<\n";
		gettimeofday(&startTime, NULL);
	}
	bool modified = false;
	
	// if x, divX and y are different variables
	if(x != y && divX != y)
	{
		{
			affineInequality* constrDivXY = getVal(divX, y);
			affineInequality* constrXY = getVal(x, y);
			
			// if the x->divX->y path results in a tighter constraint than the
			// x->y path, update the latter to the former
			if(constrDivXY && constrDivXY->getLevel()==affineInequality::constrKnown)
			{
				affineInequality inferredXY(*constrXDivX, *constrDivXY/*, x==zeroVar, y==zeroVar, getVarSign(x), getVarSign(y)*/);
				
				numInfers++;
			
				// if there doesn't exist an x-y constraint in the graph, add it
				if(!constrXY)
				{
#ifdef DEBUG_FLAG_TC
					cout << "    dvc(x->x'->y): " << x.str() << "->" << y.str() << "\n";
					cout << "    dvc(x->x'->y):     Current = None\n";
					cout << "    dvc(x->x'->y):     Inferred("<<x.str()<<"->"<<divX.str()<<"->"<<y.str()<<") = " << inferredXY.str(x, y, "") << "\n";
#endif
					setVal(x, y, inferredXY);
					modified = true;
				}
				// else, if the inferred x-y constraint it strictly tighter than the current x-y constraint
				else if(inferredXY.semLessThan(*constrXY, isEqZero(x), isEqZero(y)))
				{
#ifdef DEBUG_FLAG_TC
					cout << "    dvc(x->x'->y): " << x.str() << "->" << y.str() << "\n";
					cout << "    dvc(x->x'->y):     Current = " << constrXY->str(x, y, "") << "\n";
					cout << "    dvc(x->x'->y):     Inferred ("<<x.str()<<"->"<<divX.str()<<"->"<<y.str()<<") = " << inferredXY.str(x, y, "") << "\n";
#endif								
					// replace the current x-y constraint with the inferred one
					constrXY->set(inferredXY);
					modified = true;
				}
			}
		}
		
		{
			affineInequality* constrYDivX = getVal(y, divX);
			affineInequality* constrYX = getVal(y, x);
			
			numInfers++;
			
			// if the y->divX->x path results in a tighter constraint than the
			// y->x path, update the latter to the former
			if(constrYDivX && constrYDivX->getLevel()==affineInequality::constrKnown)
			{
				affineInequality inferredYX(*constrYDivX, *constrDivXX/*, y==zeroVar, x==zeroVar, getVarSign(y), getVarSign(x)*/);
			
				// if there doesn't exist an y-x constraint in the graph, add it
				if(!constrYX)
				{
#ifdef DEBUG_FLAG_TC
					cout << "    dvc(y->x'->x): " << y.str() << "->" << x.str() << "\n";
					cout << "    dvc(y->x'->x):     Current = None\n";
					cout << "    dvc(y->x'->x):     Inferred("<<y.str()<<"->"<<divX.str()<<"->"<<x.str()<<") = " << inferredYX.str(y, x, "") << "\n";
#endif
					setVal(y, x, inferredYX);
					modified = true;
				}
				// else, if the inferred y-x constraint it strictly tighter than the current y-x constraint
				else if(inferredYX.semLessThan(*constrYX, isEqZero(y), isEqZero(x)))
				{
#ifdef DEBUG_FLAG_TC
					cout << "    dvc(y->x'->x): " << y.str() << "->" << x.str() << "\n";
					cout << "    dvc(y->x'->x):     Current = " << constrYX->str(x, y, "") << "\n";
					cout << "    dvc(y->x'->x):     Inferred ("<<y.str()<<"->"<<divX.str()<<"->"<<x.str()<<") = " << inferredYX.str(y, x, "") << "\n";
#endif								
					// replace the current y-x constraint with the inferred one
					constrYX->set(inferredYX);
					modified = true;
				}
			}
		}
	}
	if(profileLevel>=1)
	{
		gettimeofday(&endTime, NULL);
		cout << "divVarsClosure_perY("<<x.str()<<", "<<divX.str()<<", "<<y.str()<<") >>> numInfers="<<numInfers<<", numVars="<<scalars.size()<<", time="<<((double)((endTime.tv_sec*1000000+endTime.tv_usec)-(startTime.tv_sec*1000000+startTime.tv_usec)))/1000000.0<<"\n";
	}
	
	return modified;
}

// computes the transitive closure of this constraint graph while modifying 
// only the constraints that involve the given variable
void ConstrGraph::localTransClosure(const varID& tgtVar)                         
{
	// don't take transitive closures in the middle of a transaction
	if(inTransaction) return;
	
	int numSteps=0, numInfers=0, numFeasibleChecks=0;
	struct timeval startTime, endTime;
	if(profileLevel>=1)
	{
		cout << "localTransClosure("<<tgtVar.str()<<") <<<\n";
		gettimeofday(&startTime, NULL);
	}
	
	initialized = true; // this constraint graph will now definitely be initialized
	//affineInequality::signs tgtVarSign = getVarSign(tgtVar);
	
#ifdef DEBUG_FLAG_TC
cout << "    Beginning local closure("<<tgtVar.str()<<")\n";
cout << str("    ") << "\n";
#endif
	//m_quad2str::iterator mIter1, mIter2, mIter3;
	bool modified = true;
	// iterate until a fixed point is reached
	while (modified)
	{
		modified = false;
		// iterate through every pair of variables
		//for ( mIter1 = vars2Name.begin(); mIter1 != vars2Name.end(); mIter1++ )
		//for(varIDSet::iterator itX = scalars.begin(); itX!=scalars.end(); itX++)
		for(varIDSet::iterator itX = newConstrVars.begin(); itX!=newConstrVars.end(); itX++)
		{
			//quad x = mIter1->first;
			varID x = *itX;
			//affineInequality::signs xSign = getVarSign(x);
			
			//for ( mIter2 = vars2Name.begin(); mIter2 != vars2Name.end(); mIter2++ )
			for(varIDSet::iterator itY = scalars.begin(); itY!=scalars.end(); itY++)
			{
				//quad y = mIter2->first;
				varID y = *itY;
				//affineInequality::signs ySign = getVarSign(y);
				
				// if tgtVar, x and y are three different variables
				if(x!=y && tgtVar!=x && tgtVar!=y)
				{
					//quad xy = getVal(x, y);
					affineInequality* constrXY = getVal(x, y);
#ifdef DEBUG_FLAG_TC
					if(constrXY) cout << "                  "<<x.str()<<"->"<<y.str()<<" = " << constrXY->str(x, y, "") << "\n";
					else cout << "                  "<<x.str()<<"->"<<y.str()<<" = None\n";
#endif
					
					if(constrXY && constrXY->getLevel()==affineInequality::constrKnown)
					{
						// examine the constraint chain tgtVar->x->y
						{
							affineInequality* constrTgtX = getVal(tgtVar, x);
#ifdef DEBUG_FLAG_TC
							if(constrTgtX) cout << "                      "<<tgtVar.str()<<"->"<<x.str()<<" = " << constrTgtX->str(tgtVar, x, "") << "\n";
							else cout << "                       "<<tgtVar.str()<<"->"<<x.str()<<" = None\n";
#endif
							numSteps++;
							
							if(constrTgtX && constrTgtX->getLevel()==affineInequality::constrKnown)
							{
								affineInequality inferredTgtY(*constrTgtX, *constrXY/*, tgtVar==zeroVar, y==zeroVar, affineInequality::posZero, getVarSign(y),
								                              dynamic_cast<DivLattice*>(divL->getVarLattice(func, tgtVar)), 
									                           dynamic_cast<DivLattice*>(divL->getVarLattice(func, y)), x*/);
								affineInequality* constrTgtY = getVal(tgtVar, y);
								
								numInfers++;
								
								// if there doesn't exist a constraint between tgtVar and y, add it
								if(!constrTgtY)
								{
									setVal(tgtVar, y, inferredTgtY);
									modified = true;
									
#ifdef DEBUG_FLAG_TC
									cout << "    " << tgtVar.str() << "->" << y.str() << "\n";
									cout << "        Current = None\n";
									cout << "        Inferred ("<<tgtVar.str()<<"->"<<x.str()<<"->"<<y.str()<<") = " << inferredTgtY.str(tgtVar, y, "") << "\n";
#endif
								}
								// if the inferred tgtVar-y constraint is strictly tighter than the current tgtVar-y constraint
								else if(inferredTgtY.semLessThan(*constrTgtY, false, isEqZero(y)))
								{
#ifdef DEBUG_FLAG_TC
									cout << "    " << tgtVar.str() << "->" << y.str()<< "\n";
									cout << "        Current = " << constrTgtY->str(tgtVar, y, "") << "\n";
									cout << "        Inferred ("<<tgtVar.str()<<"->"<<x.str()<<"->"<<y.str()<<")= " << inferredTgtY.str(tgtVar, y, "") << "\n";
#endif
									// replace the current constraint with the inferred one
									constrTgtY->set(inferredTgtY);
									modified = true;			
								}
							}
	
/*							quad tgtX = getVal(tgtVar, x);
							quad tgtY = getVal(tgtVar, y);
							
							// if the tgtVar->x->y path results in a tighter constraint than the
							// tgtVar->y path, update the latter to the former
							if ( xy != INF && tgtX != INF && tgtX + xy < tgtY )
							{
	#ifdef DEBUG_FLAG_TC
								cout << "    " << vars2Name[tgtVar] << "---" << vars2Name[y] << " = " << tgtY << " <- "
										 << tgtX << " + " << xy << "  through "<<vars2Name[x]<<"\n";
	#endif
										setVal(tgtVar, y, tgtX + xy);
										// remember that the fixed point has not been reached yet
										modified = true;
							}*/
						}
						// examine the constraint chain x->y->tgtVar
						{
							affineInequality* constrYTgt = getVal(y, tgtVar);
#ifdef DEBUG_FLAG_TC
							if(constrYTgt) cout << "                      "<<y.str()<<"->"<<tgtVar.str()<<" = " << constrYTgt->str(y, tgtVar, "") << "\n";
							else cout << "                       "<<y.str()<<"->"<<tgtVar.str()<<" = None\n";
#endif
	
							if(constrYTgt && constrYTgt->getLevel()==affineInequality::constrKnown)
							{
								affineInequality inferredXTgt(*constrXY, *constrYTgt/*, x==zeroVar, tgtVar==zeroVar, getVarSign(x), affineInequality::posZero,
								                              dynamic_cast<DivLattice*>(divL->getVarLattice(func, x)), 
									                           dynamic_cast<DivLattice*>(divL->getVarLattice(func, tgtVar)), y*/);
								affineInequality* constrXTgt = getVal(x, tgtVar);
								numInfers++;
								                                                    
								// if there doesn't exist a constraint between x and tgtVar, add it
								if(!constrXTgt)
								{
									setVal(x, tgtVar, inferredXTgt);
									modified = true;
									
#ifdef DEBUG_FLAG_TC
									cout << "    " << x.str() << "->" << tgtVar.str() << "\n";
									cout << "        Current = None\n";
									cout << "        Inferred ("<<x.str()<<"->"<<y.str()<<"->"<<tgtVar.str()<<") = " << inferredXTgt.str(x, tgtVar, "") << "\n";
#endif
								}
								// if the inferred x-tgtVar constraint is strictly tighter than the current x-tgtVar constraint
								else if(inferredXTgt.semLessThan(*constrXTgt, isEqZero(x), false))
								{
#ifdef DEBUG_FLAG_TC
									cout << "    " << x.str() << "->" << tgtVar.str()<< "\n";
									cout << "        Current = " << constrXTgt->str(x, tgtVar, "") << "\n";
									cout << "        Inferred ("<<x.str()<<"->"<<y.str()<<"->"<<tgtVar.str()<<") = " << inferredXTgt.str(x, tgtVar, "") << "\n";
#endif
									// replace the current constraint with the inferred one
									constrXTgt->set(inferredXTgt);
									modified = true;					
								}
							}
							
/*							quad yTgt = getVal(y, tgtVar);
							quad xTgt = getVal(x, tgtVar);
							
							// if the x->y->tgtVar path results in a tighter constraint than the
							// x->tgtVar path, update the latter to the former
							if ( xy != INF && yTgt != INF && xy + yTgt< xTgt )
							{
	#ifdef DEBUG_FLAG_TC
								cout << "    " << vars2Name[x] << "---" << vars2Name[tgtVar] << " = " << xTgt << " <- "
										 << xy << " + " << yTgt << "  through "<<vars2Name[y]<<"\n";
	#endif
										setVal(x, tgtVar, xy+yTgt);
										// remember that the fixed point has not been reached yet
										modified = true;
							}*/
						}
						
						// update tgtVarSign if either x or y are zeroVar (this can be made more precide to cut down on the number of calls to getVarSign
						/*if(x==zeroVar || y==zeroVar)
							tgtVarSign = getVarSign(tgtVar);*/
					}
				}
			}				
		}
		
		//numFeasibleChecks++;
		//// look for any cycles that go through arrays
		//quad r = isFeasible();
		//if(r!=1) break;
	}
	numFeasibleChecks++;
	
	// look for any cycles that go through arrays
	//quad r = isFeasible();
	//if(r!=1) break;
		
	if(profileLevel>=1)
	{
		gettimeofday(&endTime, NULL);
		cout << "localTransClosure("<<tgtVar.str()<<") >>> numSteps="<<numSteps<<", numInfers="<<numInfers<<", numFeasibleChecks="<<numFeasibleChecks<<", numVars="<<scalars.size()<<", time="<<((double)((endTime.tv_sec*1000000+endTime.tv_usec)-(startTime.tv_sec*1000000+startTime.tv_usec)))/1000000.0<<"\n";
	}
}

// searches this constraint graph for cycles. 
// If it finds a negative cycle that does not go through an array variable, it records this 
//    fact and returns 0 (Bottom).
// Otherwise, it returns 1.
// If isFeasible() finds any negative cycles, it updates the state of this constraint graph
//    accordingly, either setting it to top or recording that the range of an array is empty.
int ConstrGraph::isFeasible()
{
	int numConsistenChecks=0, numInconsistentSteps=0;
	struct timeval startTime, endTime;
	// don't do feasibility checks in the middle of a transaction
	if(inTransaction) return 1;
	
	//m_quad2str::iterator mapIter1, mapIter2;
	
	if(profileLevel>=1)
	{
		cout << "isFeasible() <<<\n";
		gettimeofday(&startTime, NULL);
	}
	
	initialized = true; // this constraint graph will now definitely be initialized

//printf("isFeasible()\n");
//cout << "isFeasible() constrChanged="<<constrChanged<<"\n";

	// loop through all the pairs of variables
	//for ( mapIter1 = vars2Name.begin(); mapIter1 != vars2Name.end(); mapIter1++ )
		//for ( mapIter2 = mapIter1, ++mapIter2; mapIter2 != vars2Name.end(); mapIter2++ )
	for(varIDSet::iterator itX = scalars.begin(); itX!=scalars.end(); itX++)
		for(varIDSet::iterator itY = scalars.begin(); itY!=scalars.end(); itY++)
		{
			//quad x = mapIter1->first, y = mapIter2->first;
			varID x = *itX;
			varID y = *itY;
			// for each pair
			if(x != y)
			{
				//quad xy = getVal(x, y), yx = getVal(y, x);
				affineInequality* constrXY = getVal(x, y);
				affineInequality* constrYX = getVal(y, x);
				
				//if(xy != INF && yx != INF)
				if(constrXY && constrXY->getLevel()==affineInequality::constrKnown && 
					constrYX && constrYX->getLevel()==affineInequality::constrKnown)
				{
					numConsistenChecks++;
					// if there is a negative cycle that goes through x and y
					//if(xy < -yx)
					if(!affineInequality::mayConsistent(*constrXY, *constrYX))
					{
						numInconsistentSteps++;
						// if neither x nor y is an array variable
						if(arrays.find(x) == arrays.end() && arrays.find(y) == arrays.end())
						{
							if(debugLevel>=1)
							{
								cout << "Bottom: X:"<< x.str() << " -> Y:" << y.str() <<" = !!!NOT CONSISTENT!!!\n";
								cout << "    "<<x.str()<<"=>"<<y.str()<<" = "<<constrXY->str("")<<"\n";
								cout << "    "<<y.str()<<"=>"<<x.str()<<" = "<<constrYX->str("")<<"\n";
								cout << "CG = "<<str("", false)<<"\n";
							}
							// since there is a negative cycle through non-array variables, 
							// the constraint graph is top
							setToBottom(true);
							
							// reset constrChanged since the state of the constraint graph now 
							// reflects its feasibility status
							constrChanged = false;
							
							if(profileLevel>=1)
							{
								gettimeofday(&endTime, NULL);
								cout << "isFeasible() >>> numConsistenChecks="<<numConsistenChecks<<", numInconsistentSteps="<<numInconsistentSteps<<", numVars="<<scalars.size()<<", time="<<((double)((endTime.tv_sec*1000000+endTime.tv_usec)-(startTime.tv_sec*1000000+startTime.tv_usec)))/1000000.0<<"\n";
							}
							
							return 0;
						}
						// if either x or y are array variables
						else
						{
							if(debugLevel>=1) 
								cout << "    Empty Range: "<< x.str() << "->" << y.str() <<" = "<<constrXY->str("")<< "  <- = "<<constrYX->str("")<<"\n";
							
							// if both x and y are arrays, then we have a problem
							if(arrays.find(x) != arrays.end() &&
								arrays.find(y) != arrays.end())
							{
								cerr << "Error! Negative cycle through a pair of array variables. Arrays should NEVER be related via constraints!\n";
								ROSE_ASSERT(0);
							}
							
							// if x is an array
							if(arrays.find(x) != arrays.end())
							{
								// set its range to empty
								emptyRange[x] = true;
								//remove all constraints that relate to x
								eraseVarConstrNoDiv(x);
							}
							// else, if y is an array
							else if(arrays.find(y) != arrays.end())
							{
								// set its range to empty
								emptyRange[y] = true;
								//remove all constraints that relate to y
								eraseVarConstrNoDiv(y);
							}
						}
					}
				}
			}
		}

	// reset constrChanged since the state of the constraint graph now 
	// reflects its feasibility status
	constrChanged = false;
	
	if(profileLevel>=1)
	{
		gettimeofday(&endTime, NULL);
		cout << "isFeasible() >>> numConsistenChecks="<<numConsistenChecks<<", numInconsistentSteps="<<numInconsistentSteps<<", numVars="<<scalars.size()<<", time="<<((double)((endTime.tv_sec*1000000+endTime.tv_usec)-(startTime.tv_sec*1000000+startTime.tv_usec)))/1000000.0<<"\n";
	}
								
	// this constraint graph is not bottom
	return 1;
}

// creates a divisibility variable for the given variable and adds it to the constraint graph
// If var = r (mod d), then the relationship between x and x' (the divisibility variable)
// will be x = x'*d + r
// returns true if this causes the constraint graph to be modified (it may not if this 
//    information is already in the graph) and false otherwise
bool ConstrGraph::addDivVar(varID var/*, int div, int rem*/, bool killDivVar)
{
	bool modified = false;
	
	FiniteVariablesProductLattice* divLattice = getDivLattice(var);
	if(divLattice)
	{
		varID divVar = getDivScalar(var);
		
		// record that this constraint graph contains divisibility information for var
		divVariables.insert(var);
		//divVar2OrigVar[divVar] = var;
	
		// first, disconnect the divisibility variable from all other variables
		if(killDivVar)
			modified = eraseVarConstrNoDiv(divVar) || modified;
		
		// add the important constraints (other constraints will be recomputed during transitive closure)
		DivLattice* varDivL = dynamic_cast<DivLattice*>(divLattice->getVarLattice(func, var));
		
		// Only bother if we have divisibility information for this variable
		if(varDivL)
		{
			// incorporate this variable's divisibility information (if any)
			if(varDivL->getLevel() == DivLattice::divKnown)
			{
				//modified = addDivVar(var, varDivL->getDiv(), varDivL->getRem()) || modified;
				modified = setVal(var, divVar, 1, varDivL->getDiv(), varDivL->getRem(), getVarSign(var), getVarSign(var)) || modified;
				modified = setVal(divVar, var, varDivL->getDiv(), 1, 0-varDivL->getRem(), getVarSign(var), getVarSign(var)) || modified;
			}
			else if(varDivL->getLevel() != DivLattice::bottom)
			{
				//modified = addDivVar(var, 1, 0) || modified;
				modified = setVal(var, divVar, 1, 1, 0, getVarSign(var), getVarSign(var)) || modified;
				modified = setVal(divVar, var, 1, 1, 0, getVarSign(var), getVarSign(var)) || modified;
			}
				
			scalars.insert(divVar);
		}
		/*else
			printf("WARNING: No divisibility info for variable %s in function %s!\n", var.str().c_str(), func.get_name().str());*/
	}
	
	return modified;
}

// Disconnect this variable from all other variables except its divisibility variable. This is done 
// in order to compute the original variable's relationships while taking its divisibility information 
// into account.
// Returns true if this causes the constraint graph to be modified and false otherwise
bool ConstrGraph::disconnectDivOrigVar(varID var/*, int div, int rem*/)
{
	bool modified = false;
	FiniteVariablesProductLattice* divLattice = getDivLattice(var);
	if(divLattice)
	{
		varID divVar = getDivScalar(var);
	
		// record that this constraint graph constains divisibility information for var
		//divVariables.insert(var);
	
		// first, disconnect var from all scalars
		modified = eraseVarConstrNoDivScalars(var) || modified;
		
		// Add the important constraints (other constraints will be recomputed during transitive closure).
		// We don't update modified since we're assuming that these constraints were in the constraint
		// graph before the eraseVarConstr() call, since they should have been added by the preceding 
		// addDivVar() call.
		DivLattice* varDivL = dynamic_cast<DivLattice*>(divLattice->getVarLattice(func, var));
			
		// incorporate this variable's divisibility information (if any)
		if(varDivL->getLevel() == DivLattice::divKnown)
		{
			//modified = addDivVar(var, varDivL->getDiv(), varDivL->getRem()) || modified;
			modified = setVal(var, divVar, 1, varDivL->getDiv(), varDivL->getRem(), getVarSign(var), getVarSign(var)) || modified;
			modified = setVal(divVar, var, varDivL->getDiv(), 1, 0-varDivL->getRem(), getVarSign(var), getVarSign(var)) || modified;
		}
		else if(varDivL->getLevel() != DivLattice::bottom)
		{
			//modified = addDivVar(var, 1, 0) || modified;
			modified = setVal(var, divVar, 1, 1, 0, getVarSign(var), getVarSign(var)) || modified;
			modified = setVal(divVar, var, 1, 1, 0, getVarSign(var), getVarSign(var)) || modified;
		}
	}
	
	return modified;
}

// Adds a new divisibility lattice, with the associated anotation
// Returns true if this causes the constraint graph to be modified and false otherwise
bool ConstrGraph::addDivL(FiniteVariablesProductLattice* divLattice, string annotName, void* annot)
{
	bool modified = false;
	pair<string, void*> divLAnnot(annotName, annot);
	map<pair<string, void*>, FiniteVariablesProductLattice*>::iterator loc = divL.find(divLAnnot);
	// If we already have a divisibility lattice associated with the given annotation
	if(loc != divL.end())
	{
		// Update the mapping
		modified = loc->second != divLattice;
		loc->second = divLattice;
	}
	else
	{
		// Create a new mapping
		modified = true;
		divL[divLAnnot] = divLattice;
	}
	
	return modified;
}

// Adds a new sign lattice, with the associated anotation
// Returns true if this causes the constraint graph to be modified and false otherwise
bool ConstrGraph::addSgnL(FiniteVariablesProductLattice* sgnLattice, string annotName, void* annot)
{
	bool modified = false;
	pair<string, void*> sgnLAnnot(annotName, annot);
	map<pair<string, void*>, FiniteVariablesProductLattice*>::iterator loc = sgnL.find(sgnLAnnot);
	// If we already have a divisibility lattice associated with the given annotation
	if(loc != sgnL.end())
	{
		// Update the mapping
		modified = loc->second != sgnLattice;
		loc->second = sgnLattice;
	}
	else
	{
		// Create a new mapping
		modified = true;
		sgnL[sgnLAnnot] = sgnLattice;
	}
	
	return modified;
}

/**** State Accessor Functions *****/

// Returns true if this constraint graph includes constraints for the given variable
// and false otherwise
bool ConstrGraph::containsVar(const varID& var)
{
	// First check if there are any var <= x constraints
	if(vars2Value.find(var) != vars2Value.end())
		return true;
	else
	{
		// Now look for x <= var constraints
		for(map<varID, map<varID, affineInequality> >::iterator itX = vars2Value.begin();
		    itX!=vars2Value.end(); itX++)
		{
			if(itX->second.find(var) != itX->second.end())
				return true;
		}
		
		// Finally, check if var is an array with an empty range (i.e. we have known
		// constraints that are not recorded in vars2Value)
		if(emptyRange.find(var) != emptyRange.end())
			return emptyRange[var] == true;
	}
	return false;
}

// returns the x->y constraint in this constraint graph
affineInequality* ConstrGraph::getVal(varID x, varID y)
{
	if(x == y)
		return NULL;

	map<varID, map<varID, affineInequality> >::iterator xIt = vars2Value.find(x);
	// we don't have constraints from x
	if(xIt == vars2Value.end()) return NULL;
	
	map<varID, affineInequality>::iterator yIt = xIt->second.find(y);
	// we don't have an x-y constraint
	if(yIt == xIt->second.end()) return NULL;
		
	return &(yIt->second);
/*
	for(map<varID, constraint>::iterator it = xConstr->second.begin(); it!=xConstr->second.end(); it++)
	{
		// if we've found the right constraint
		if(y == (*it)->getY())
			return &(*it);
	}
	
	initialized = true; // this constraint graph is now definitely initialized
	
	// we don't have an x-y constraint
	return NULL;*/
}

// set the x->y connection in this constraint graph to: x*a <= y*b + c
// return true if this results this constraint graph being changed
// xSign, ySign: the default signs for x and y. If they're set to unknown, setVal computes them on its own using getVarSign.
//     otherwise, it uses the given signs 
bool ConstrGraph::setVal(varID x, varID y, int a, int b, int c, affineInequality::signs xSign, affineInequality::signs ySign)
{
	//cout << "setVal(): "<<x.str()<<"*"<<a<<" <= "<<y.str()<<"*"<<b<<" + "<<c<<"\n";
	// if x or y are arrays with an empty range, their range cannot be further changed
	if((isArray(x) && emptyRange[x]) ||
		(isArray(y) && emptyRange[y]))
		return false;

	initialized = true; // this constraint graph is now definitely initialized

	// only bother adding constrants to this graph if it is not already bottom
	if(!isBottom())
	{
		map<varID, map<varID, affineInequality> >::iterator xIt = vars2Value.find(x);
		xSign = (xSign==affineInequality::unknownSgn? getVarSign(x): xSign);
		ySign = (ySign==affineInequality::unknownSgn? getVarSign(y): ySign);
		affineInequality newConstr(a, b, c, x==zeroVar, y==zeroVar, xSign, ySign);
		
		modifiedVars.insert(x);
		modifiedVars.insert(y);
		newConstrVars.insert(x);
		newConstrVars.insert(y);
							
		// we don't have constraints from x
		if(xIt == vars2Value.end())
		{
			constrChanged = true;
			vars2Value[x][y] = newConstr;
			//cout << "vars2Value[x][y] = " << vars2Value[x][y].str("") << "\n";
			//cout << "newConstr = " << newConstr.str("") << "\n";
			return true;
		}
		
		map<varID, affineInequality>::iterator yIt = xIt->second.find(y);
		// we don't have an x->y constraint
		if(yIt == xIt->second.end())
		{
			vars2Value[x][y] = newConstr;
			constrChanged = true;
			return true;
		}
		
		affineInequality& constrXY = yIt->second;
		constrChanged = constrChanged || (constrXY != newConstr);
		bool modified = constrXY.set(newConstr);
		return modified;
	}
	else
		// we haven't modified the constraint graph
		return false;
}

bool ConstrGraph::setVal(varID x, varID y, const affineInequality& ineq)
{
	return setVal(x, y, ineq.getA(), ineq.getB(), ineq.getC(), ineq.getXSign(), ineq.getYSign());
}

// Sets the state of this constraint graph to Uninitialized, without modifying its contents. Thus, 
//    the graph will register as uninitalized but when it is next used, its state will already be set up.
// Returns true if this causes the constraint graph to be modified and false otherwise.
bool ConstrGraph::setToUninitialized()
{
	bool modified = (initialized == true);
	initialized = false;
	return modified;
}

// sets the state of this constraint graph to Bottom
// noBottomCheck - flag indicating whether this function should do nothing if this isBottom() returns 
//              true (=false) or to not bother checking with isBottom (=true)
void ConstrGraph::setToBottom(bool noBottomCheck)
{
	// this constraint graph will now definitely be initialized 
	// (we must record this now for the sake of isBottom() and isDead())
	initialized = true; 
	
	// erase all the data in this constraint graph
	eraseConstraints(noBottomCheck);
	
	// make all the ranges of arrays empty
	emptyArrayRanges(noBottomCheck);
	
	// Erase the modification state
	modifiedVars.clear();
	newConstrVars.clear();
	
	// remember that this constraint graph is Bottom
	bottom = true;
	
	// reset constrChanged because bottom is now correctly set relative
	// to the state of the constraint graph
	constrChanged = false;
} 

// Sets the state of this constraint graph to top 
// If onlyIfNotInit=true, this is only done if the graph is currently uninitialized
void ConstrGraph::setToTop(bool onlyIfNotInit)
{
	if(!onlyIfNotInit || !initialized)
	{
		// this constraint graph will now definitely be initialized 
		// (we must record this now for the sake of isBottom() and isDead())
		initialized = true; 
		
		// erase all the data in this constraint graph
		eraseConstraints(true);
		
		// make all the ranges of arrays empty
		emptyArrayRanges(true);
		
		// Erase the modification state
		modifiedVars.clear();
		newConstrVars.clear();
		
		// reset constrChanged because bottom is now correctly set relative
		// to the state of the constraint graph
		constrChanged = false;
	}
}

// returns whether the range of the given array is empty
bool ConstrGraph::isEmptyRange(varID array)
{ 
	// if this constraint graph is not initialized, no arrays are known to have an empty range
	if(!initialized)
		return false;
		
	// the range of this array is empty if this fact is recorded inside emptyRange
	// or if it is known that this constraint graph = bottom
	return emptyRange.find(array)->second || isBottom();
}

// returns whether this constraint graph is Bottom
bool ConstrGraph::isBottom()
{
	// if this constraint graph is not initialized, it cannot be bottom or anything in particular
	if(!initialized)
		return false;
		
	// if the constraints have changed since the last time this constraint graph 
	// was checked for dead-ness, update this graph's feasibility state
	if(constrChanged)
	{
		// if we already know that this constraint graph is bottom 
		// then it should not have changed since we learned this fact
		if(bottom)
		{
			if(debugLevel>=1) 
				std::cout << "ERROR: constrChanged=true and bottom=true inside isBottom()!\n";
			ROSE_ASSERT(0);
		}
		
		// check it again and update the state of this constraint graph 
		// to reflect its feasibility status
		isFeasible();
	}

	// return whether this constraint graph is known to be bottom
	return bottom;
}

// sets the state of this constraint graph to Bottom
	// noBottomCheck - flag indicating whether this function should do nothing if this isBottom() returns 
	//              true (=false) or to not bother checking with isBottom (=true)
	/*void setToBottom(bool noBottomCheck=false)
	{
//		std::cout <<"setToBottom() dead="<<dead<<" bottom="<<bottom<<"\n";
		// make sure that this graph is neither Dead or Bottom
		// this will enable eraseConstants to do its work 
		bottom=false;

		// erase all the data in this constraint graph
		eraseConstraints(noBottomCheck);
		
		// record that the ranges of all arrays are not empty in this constraint graph
		unEmptyArrayRanges();
		
		initialized = true; // this constraint graph  is now definitely initialized
		
//		std::cout <<"setToBottom() dead="<<dead<<" bottom="<<bottom<<"\n";
	}*/

/**** String Output *****/

// Returns the string representation of the constraints held by this constraint graph, 
//    with a line for each pair of variables for which the constraint is < bottom. It also prints
//    the names of all the arrays that have empty ranges in this constraint graph
// There is no \n on the last line of output, even if it is a multi-line string
string ConstrGraph::str(string indent)
{
	return str(indent, true);
}

// Returns the string representation of the constraints held by this constraint graph, 
//    with a line for each pair of variables for which the constraint is < bottom. It also prints
//    the names of all the arrays that have empty ranges in this constraint graph
// There is no \n on the last line of output, even if it is a multi-line string
// If useIsBottom=true, isBottom() is used to determine whether the graph is =bottom.
// Otherwise, the bottom variable is checked.
// If useIsBottom=true, isBottom() is used to determine whether the graph is =bottom.
// Otherwise, the bottom variable is checked.
string ConstrGraph::str(string indent, bool useIsBottom)
{
	ostringstream outs;
	
	if(!initialized)
		outs << /*indent << */"ConstrGraph : uninitialized";
	else if( (useIsBottom && isBottom()) || (!useIsBottom && bottom))
		outs << /*indent << */"ConstrGraph : bottom";
/*	else if(vars2Value.size() == 0)
		outs << indent << "ConstrGraph : bottom";*/
	else
	{
		bool needEndl=false; // =true if the previous line was printed and needs a \n before the next line can begin
		if(debugLevel>=1) 
			outs << /*indent << */"ConstrGraph : \n";
//		cout << "vars2Value.size()="<<vars2Value.size()<<"\n";
		for(map<varID, map<varID, affineInequality> >::iterator itX = vars2Value.begin();
		    itX!=vars2Value.end(); itX++)
		{
//outs << indent << "\nvars2Value["<<itX->first.str()<<"].size()="<<vars2Value[itX->first].size()<<"\n";
			for(map<varID, affineInequality>::iterator itY = itX->second.begin();
			    itY!=itX->second.end(); itY++)
			{
				const affineInequality& constr = itY->second;
				if(needEndl) { outs << "\n"; }
				outs << indent << "  " << constr.str(itX->first, itY->first, indent+"    ");
				needEndl = true;
			}
		}
		
		// print out all arrays with empty ranges
/*		for(varIDSet::iterator curArray = arrays.begin();
		    curArray != arrays.end(); curArray++)
		{
			if(emptyRange.find(*curArray)->second)
			{
				if(needEndl) { outs << "\n"; }
				outs << indent << "  Array \""<<(*curArray).str()<<"\" has empty range.";
				needEndl = true;
			}
		}
		outs << "\n";
		
		// print out all scalars
		outs << indent << "      scalars = ";
		for(varIDSet::iterator it=scalars.begin(); it!=scalars.end(); it++)
		{ outs << (*it).str() << ", "; }
		//outs << "\n";*/
	}
	
	return outs.str();
}

/**** Comparison Functions ****/

// two constraint graphs are unequal if there exists some pair of variables
// that have different constraints (to get semantic equivalence apply transitive
// closure first)
bool ConstrGraph::operator != (ConstrGraph &that)
{
	// if either constraint graph is uninitialized, it isn't equal to any other graph
	if(!initialized || !that.initialized)
		return true;
	
	return diffConstraints(that);// || diffArrays(&that);
}

// two graphs are equal if they're not unequal
bool ConstrGraph::operator == (ConstrGraph &that)
{
	return !(*this != that);
}

bool ConstrGraph::operator==(Lattice* that)
{
	return *this == *dynamic_cast<ConstrGraph*>(that);
}

// this is <<= that if the constraints in this are equal to or strictly tighter (tight is bottom, loose is top) 
// than the constraints in that for every pair of variables. As this function does not itself apply 
// transitive closure, the input graphs need to be closed for this to be a semantic <=.
bool ConstrGraph::operator <<= (ConstrGraph &that)
{
	//printf("ConstrGraph::operator <=, initialized=%d, that.initialized=%d\n", initialized, that.initialized);
	
	// if this constraint graph is uninitialized, it is smaller than any other graph
	if(!initialized)
		return true;
	// else, if that is uninitialized, it must be smaller
	else if(!that.initialized)
		return false;
	
	if(isBottom())
		return true;
	else if(that.isBottom())
		return false;
	
	map<varID, map<varID, affineInequality> >::const_iterator itThisX, itThatX;
	for(itThisX = vars2Value.begin(), itThatX = that.vars2Value.begin();
	    itThisX!=vars2Value.end() && itThatX!=that.vars2Value.end(); )
	{
		// if both constraint graphs have constraints for itThisX->first
		if(itThisX->first == itThatX->first)
		{
			/*affineInequality::signs xSign = getVarSign(itThisX->first);
			ROSE_ASSERT(xSign == that.getVarSign(itThisX->first));*/
			varID x = itThisX->first;
			
			map<varID, affineInequality>::const_iterator itThisY, itThatY;
			for(itThisY = itThisX->second.begin(), itThatY = itThatX->second.begin();
			    itThisY!=itThisX->second.end() && itThatY!=itThatX->second.end(); )
			{
				// if both constraint graphs have constraints for <itThisX->first, itThisY->first>
				if(itThisY->first == itThatY->first)
				{
					varID y = itThisY->first;
					
					// if the corresponding <x->y> constraints in that have more information than in this
					if(itThatY->second.semLessThan(itThisY->second, isEqZero(x), isEqZero(y)))
						return false;
					
					// advance both iterators
					itThisY++;
					itThatY++;
				}
				// else, if only that has constraints for <itThisX->first, itThisY->first>
				else
					// advance only itThisY since we have to see if the next variable in itThisX->second 
					// matches itThatY->first
					// (we've assumed this has the same or more pairs than that and we're trying to find a counter-example)
					itThisY++;
			}
			
			// if there are variables that have constraints in itThatX->second but not in itThisX->second
			if(itThisY!=itThisX->second.end())
				// that has some constraints that this does not and is therefore either tighter or non-comparable
				return false;
			
			// advance both iterators
			itThisX++;
			itThatX++;
		}
		// else, if only that has constraints for var->first
		else
		{
			// advance only itThisX since we have to see if the next variable in that matches itThatX->first
			// (we've assumed this has the same or more pairs than that and we're trying to find a counter-example)
			itThisX++;
		}
	}
	
	// if there are variables that have constraints in that but not in this
	if(itThatX!=vars2Value.end())
		// that has some constraints that this does not and is therefore either tighter or non-comparable
		return false;
	
	// this <<= cg only if for all arrays, if an array's range is empty in cg, then 
	// it must be empty in this
	for(varIDSet::iterator curArray = arrays.begin(); 
		 curArray != arrays.end(); curArray++)
	{
		if(that.emptyRange.find(*curArray)->second==true &&
		   emptyRange.find(*curArray)->second==false)
			return false;
	}
	
	// there has been no pair for which that's constraints are tighter than this'
	return true;
}
	
// Returns true if x*b+c MUST be outside the range of y and false otherwise. 
// If two variables are unrelated, it is assumed that there is no information 
// about their relationship and mustOutsideRange() thus proceeds conservatively (returns true).
bool ConstrGraph::mustOutsideRange(varID x, int b, int c, varID y)
{
	// do a transitive closure in case one is overdue
	if(constrChanged) transitiveClosure();
	
	affineInequality* constrXY = getVal(x, y);
	affineInequality* constrYX = getVal(y, x);
	
	if(debugLevel>=1)
	{
		if(debugLevel>=1) 
		{
			cout << "mustOutsideRange("<<x.str()<<"*"<<b<<"+"<<c<<", "<<y.str()<<")\n";
			printf("    constrXY=%p constrYX=%p\n", constrXY, constrYX);
			if(constrXY) cout << "mustOutsideRange() "<<x.str()<<"->"<<y.str()<<"="<<constrXY->str(x, y, "")<<" b="<<b<<" c="<<c<<"\n";
			if(constrYX) cout << "mustOutsideRange() "<<y.str()<<"->"<<x.str()<<"="<<constrYX->str(y, x, "")<<" b="<<b<<" c="<<c<<"\n";
		}
	}
	//cout << str("    ") <<"\n";
		
	ROSE_ASSERT(b==1);
	if(constrXY) ROSE_ASSERT(constrXY->getA()==1 && constrXY->getB()==1);
	if(constrYX) ROSE_ASSERT(constrYX->getA()==1 && constrYX->getB()==1);
	
	// if (x*a' <= y*b' + c'), then 
	// (x*b + c < y) == (x*b*b' + c*b' + c' < y*b' + c' >= x*a') == (x*b*b' + c*b' + c' < x*a')
	// to keep this simple, we only deal with the case where a'=1, b'=1 and b'=1, which reduces the above to:
	// x + c + c' < x
	// c + c' < 0
	if(constrXY) return (constrXY && (constrXY->getC() + c)<0);
	// if (y*a' <= x*b' + c'), then 
	// (x*b + c > y) == (x*b*a' + c*a' > y*a' <= x*b' + c') == (x*b*a' + c*a' > x*b' + c')
	// to keep this simple, we only deal with the case where a'=1, b'=1 and b=1, which reduces the above to:
	// x + c > x + c'
	// c - c' > 0
	if(constrYX) return (constrYX && (c - constrYX->getC())>0);
		
	return false;
	
	
/*	// x<=y+c'
	if((getVal(x,y)!=INF && (getVal(x,y)+c)<0) || 
	       (getVal(y, x)!=INF && getVal(y,x)<c))
		cout << "    must be outside the range\n";
	else cout << "    may be inside the range\n";
		
	return (getVal(x,y)!=INF && (getVal(x,y)+c)<0) || 
	       (getVal(y, x)!=INF && getVal(y,x)<c);*/
}

/*// returns true if x+c MUST be inside the range of y and false otherwise
// If two variables are unrelated, it is assumed that there is no information 
// about their relationship and mustInsideRange() thus proceeds conservatively.
bool ConstrGraph::mustInsideRange(varID x, int b, int c, varID y)
{
	return !mayOutsideRange(x, b, c, y);
}
*/

// returns true if this logical condition must be true and false otherwise
// <from LogicalCond>
bool ConstrGraph::mayTrue()
{
	if(constrChanged) transitiveClosure();
	return !isBottom();
}


/* Transactions */
void ConstrGraph::beginTransaction()
{
	inTransaction = true;
}

void ConstrGraph::endTransaction()
{
	ROSE_ASSERT(inTransaction);
	inTransaction = false;
	transitiveClosure();
	
	/*cout << "scalars = ";
	for(varIDSet::iterator it=scalars.begin(); it!=scalars.end(); it++)
	{ cout << (*it).str() << " "; }
	cout << "\n";*/
}

