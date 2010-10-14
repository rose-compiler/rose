#include "VarLatticeVector.h"

void VarLatticeVector::init(varIDSet& vars, boolPlus initVal, bool initialized=true)
{
	// map all variables to initVal
	for(varIDSet::iterator it = vars.begin(); it != vars.end(); it++)
	{
		varMap[*it]=initVal;
	}
	this->vars = vars;
	this->initialized = initialized;
	this->sample = initVal;
}

// copies the data from the given bitvector to this bitvector.
// returns true if this results in a change in this bitvector or false otherwise
// (change = among the variables mapped by bitvector src)
// if there's a variable in srcMap that is not in this, this variable's mapping is copied to this
// if there's a variable in this that is not in srcMap, it is unchanged
bool VarLatticeVector::copy(VarLatticeVector& src)
{
//		cout <<"copy()  src="<<src<<"\n";	

	bool modified = !this->initialized ||
	                (this->initialized != src.initialized);
	
	this->initialized = src.initialized;
	bool modifiedCopy = copyMap(src.varMap);
	
	return modified || modifiedCopy;
}

// copies the contents of the given map into this bitvector's map
// returns true if the mappings of variables in srcMap are different from
//    those originally in varMap and returns false otherwise
// if there's a variable in srcMap that is not in this, this variable's mapping is copied to this
// if there's a variable in this that is not in srcMap, it is unchanged
bool VarLatticeVector::copyMap(const map<varID, VarLatticeVector::Lattice> srcMap)
{
	bool modified = false;		
	
	// copy all variable mappings from orig to this object's varMap
	for(map<varID, VarLatticeVector::Lattice>::const_iterator it = srcMap.begin(); it != srcMap.end(); it++ )
	{
		bool modifiedCopy;
		// if the current variable doesn't exist in this map, we'll add it, so this is a modification
		if(varMap.find(it->first)==varMap.end()) modifiedCopy = true;
		else modifiedCopy = (varMap[it->first]!=it->second);

		modified = modified || modifiedCopy;
		varMap[it->first]=it->second;
	}
	initialized = true; // this bitvector is now definitely initialized
	
	return modified;
}

bool VarLatticeVector::eq(VarLatticeVector &other)
{
	// if a bitvector is uninitialized, it isn't equal to any other bitvector
	if(!initialized || !other.initialized)
	{
//		printf("uninitialized: initialized=%d other.initialized=%d\n", initialized, other.initialized);
		return false;
	}
	
	// verify that all the variables that have a mapping in other.varMap's have 
	// a mapping in varMap and the mapping is in fact to the same value.
	for(map<varID, VarLatticeVector::Lattice>::iterator it = varMap.begin(); it != varMap.end(); it++)
	{
		if(other.varMap[it->first] != varMap[it->first])
		{
//			printf("varMap: other.varMap[%s] (%s) != varMap[%s] (%s)\n", it->first.str().c_str(), other.varMap[it->first].str().c_str(), it->first.str().c_str(), varMap[it->first].str().c_str());
			return false;
		}
	}
	
	// verify the reverse of the above: that all the variables that have 
	// a mapping in varMap's have a mapping in other.varMap and the mapping 
	// is in fact to the same value.
	for(map<varID, VarLatticeVector::Lattice>::iterator it = other.varMap.begin(); it != other.varMap.end(); it++)
	{
		if(other.varMap[it->first] != varMap[it->first])
		{
//			printf("other.varMap: other.varMap[%s] (%s) != varMap[%s] (%s)\n", it->first.str().c_str(), other.varMap[it->first].str().c_str(), it->first.str().c_str(), varMap[it->first].str().c_str());
			return false;
		}
	}
	
	return true;
}


bool VarLatticeVector::operator != (VarLatticeVector &other)
{
	return !(*this).eq(other);
}

bool VarLatticeVector::operator == (VarLatticeVector &other)
{
	return (*this).eq(other);
}

// returns the current mapping of the given variable
VarLatticeVector::Lattice VarLatticeVector::getVar(varID var)
{
	initialized = true; // this bitvector is now definitely initialized
	return varMap[var];
}

// maps the given variable to the given value
// returns true if this causes the bitvector to be modified and false otherwise
bool VarLatticeVector::setVar(varID var, VarLatticeVector::Lattice newVal)
{
	VarLatticeVector::Lattice oldVal = varMap[var];
	varMap[var] = newVal;
	initialized = true; // this bitvector is now definitely initialized
	return oldVal!=newVal;
}

// returns the current mapping of the given variable
VarLatticeVector::Lattice& VarLatticeVector::operator[] (varID var) { 
	initialized = true; // this bitvector is now definitely initialized
	return varMap[var];
}


// sets the mapping all of mapped variables to the minimum lattice value
// returns true if this causes the bitvector to be modified and false otherwise
bool VarLatticeVector::setToMin()
{
	return setToVal(VarLatticeVector::Lattice::getMin());
}

// sets the mapping all of mapped variables to maximum lattice value
// returns true if this causes the bitvector to be modified and false otherwise
bool VarLatticeVector::setToMax()
{
	return setToVal(VarLatticeVector::Lattice::getMin());
}

// sets this bitvector to an uninitialized state, erasing any of its state
void VarLatticeVector::uninitialize()
{
	varMap.clear();
	initialized = false;
}

// sets this bitmap to map all variables to the given value
// returns true if this causes the bitvector to be modified and false otherwise
bool VarLatticeVector::setToVal(VarLatticeVector::Lattice val)
{
	bool modified = !initialized;
	// map all variables to 0
	for(map<varID, VarLatticeVector::Lattice>::iterator it = varMap.begin(); it != varMap.end(); it++)
	{
		VarLatticeVector::Lattice oldVal = varMap[it->first];
		varMap[it->first]=val;
		modified = modified || (oldVal!=val);
	}
	initialized = true; // this bitvector is now definitely initialized
	return modified;
}

// prints all variables mapped by this bitvector, showing the boolean value that each is mapped to
void VarLatticeVector::showVarMap(string mess)
{
	showVarMap(mess, "");
}

void VarLatticeVector::showVarMap(string mess, string indent)
{
	//printf("VarLatticeVector::showVarMap varMap.size()=%d\n", varMap.size());
	cout << indent << mess << "\n";
	for(map<varID, VarLatticeVector::Lattice>::iterator it = varMap.begin(); it != varMap.end(); it++)
	{
		//cout << indent << it->second << " -> \t" << (varMap[it->first]? "True": "False") << "\n";
		printf("%s%15s -> %6s\n", indent.c_str(), ((varID)it->first).str().c_str(), it->second.str().c_str());
		fflush(stdout);
	}
}

// Computes the intersection of this bitvector with the given bitvector, returning this meet
VarLatticeVector* VarLatticeVector::meetVec(VarLatticeVector &other)
{
	VarLatticeVector* newVBV = new VarLatticeVector();
	
	// go through all the variable mappings and for each variable that is shared by both this 
	// bitvector and the given bitvector, compute the meet of their mapped boolean values, saving
	// the results in newVBV
	for(map<varID, VarLatticeVector::boolPlus>::iterator it = other.varMap.begin(); it != other.varMap.end(); it++)
	{
		// if the current variable also exists in this bitvector's varMap
		if(varMap.find(it->first) != varMap.end())
		{
			newVBV->varMap[it->first] = varMap[it->first].meet(other.varMap[it->first]);
/*			printf("newVBV->varMap[%s] = %s\n", (it->first).str().c_str(), newVBV->varMap[it->first].str().c_str());
			printf("varMap[%s] = %s\n", (it->first).str().c_str(), varMap[it->first].str().c_str());
			printf("other.varMap[%s] = %s\n", (it->first).str().c_str(), other.varMap[it->first].str().c_str());*/
		}
	}
	
	return newVBV;
}

// Computes the join of this bitvector with the given bitvector, returning this join
VarLatticeVector* VarLatticeVector::joinVec(VarLatticeVector &other)
{
	VarLatticeVector* newVBV = new VarLatticeVector();
	
	// go through all the variable mappings and for each variable that is shared by both this 
	// bitvector and the given bitvector, compute the join of their mapped boolean values, saving
	// the results in newVBV
	for(map<varID, VarLatticeVector::Lattice>::iterator it = other.varMap.begin(); it != other.varMap.end(); it++)
	{
		// if the current variable also exists in this bitvector's varMap
		if(varMap.find(it->first) != varMap.end())
			newVBV->varMap[it->first] = varMap[it->first].join(other.varMap[it->first]);
	}
	
	return newVBV;
}

// computes the intersection of f1 and f2, returning this meet
VarLatticeVector *VarLatticeVector::meetVec(VarLatticeVector &f1, VarLatticeVector &f2)
{
	return f1.meetVec( f2 );
}

// computes the join of f1 and f2, returning this join
VarLatticeVector *VarLatticeVector::joinVec(VarLatticeVector &f1, VarLatticeVector &f2)
{
	return f1.joinVec( f2 );
}

// computes the meet of this bitvector and the given bitvector, updating this bitvector to be that meet
bool VarLatticeVector::meetUpdate(VarLatticeVector &other)
{
	VarLatticeVector *vecMeet = meetVec(other);
	// if the meet is different from this bitvector's initial value
	if ((*this) != (*vecMeet))
	{
		// copy the new meet into the current bitvector
		copyMap(vecMeet->varMap);
		delete vecMeet;
		return true; // inform the caller that this bitvector has changed
	}
	else
		return false; // inform the caller that this bitvector has not changed
}

// computes the join of this bitvector and the given bitvector, updating this bitvector to be that join
bool VarLatticeVector::joinUpdate(VarLatticeVector &other)
{
	VarLatticeVector *vecJoin = joinVec(other);
	// if the join is different from this bitvector's initial value
	if ((*this) != (*vecJoin))
	{
		// copy the new join into the current bitvector
		copyMap(vecJoin->varMap);
		delete vecJoin;
		return true; // inform the caller that this bitvector has changed
	}
	else
		return false; // inform the caller that this bitvector has not changed
}

// returns true if the bitvector is =bottom (vector of all mins) and false otherwise
bool VarLatticeVector::isMin()
{
	// uninitialized bitvectors are not equal to any other bitvector
	if(!initialized) return false;
	return isAllEq(VarLatticeVector::Lattice::getMin());
}

// returns true if the bitvector is =top (vector of all maxes) and false otherwise
bool VarLatticeVector::isMax()
{
	// uninitialized bitvectors are not equal to any other bitvector
	if(!initialized) return false;
	return isAllEq(VarLatticeVector::Lattice::getMin());
}


// returns true if the bitvector maps all of its variables to val and false otherwise	
bool VarLatticeVector::isAllEq(VarLatticeVector::Lattice val)
{
	// go through all the variable mappings, returning false if any variable maps to true
	for(map<varID, VarLatticeVector::Lattice>::iterator it = varMap.begin(); it != varMap.end(); it++)
	{
		// if the current variable is not mapped to val, return false
		if(varMap[it->first] != val)
			return false;
	}
	
	// if every variable is mapped to val, return true
	return true;
}

// returns the set of variables mapped by this VarLatticeVector
varIDSet& VarLatticeVector::getMappedVars()
{
	return vars;	
}
