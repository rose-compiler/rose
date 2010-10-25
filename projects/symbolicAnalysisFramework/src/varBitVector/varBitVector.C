#include "varBitVector.h"

// Liao 10/7/2010, real definition of static class member variables
const int VarBitVector::boolPlus::UnsetVal = -1;
const int VarBitVector::boolPlus::FalseVal = 0;
const int VarBitVector::boolPlus::TrueVal = 1;

VarBitVector::boolPlus VarBitVector::Unset(VarBitVector::boolPlus::UnsetVal);
VarBitVector::boolPlus VarBitVector::False(VarBitVector::boolPlus::FalseVal);
VarBitVector::boolPlus VarBitVector::True (VarBitVector::boolPlus::TrueVal);

void VarBitVector::init(varIDSet& vars, boolPlus initVal, bool initialized=true)
{
	// map all variables to initVal
	for(varIDSet::iterator it = vars.begin(); it != vars.end(); it++)
	{
		varMap[*it]=initVal;
	}
	this->vars = vars;
	this->initialized = initialized;
}

// copies the data from the given bitvector to this bitvector.
// returns true if this results in a change in this bitvector or false otherwise
// (change = among the variables mapped by bitvector src)
// if there's a variable in srcMap that is not in this, this variable's mapping is copied to this
// if there's a variable in this that is not in srcMap, it is unchanged
bool VarBitVector::copy(VarBitVector& src)
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
bool VarBitVector::copyMap(const map<varID, VarBitVector::boolPlus> srcMap)
{
	bool modified = false;		
	
	// copy all variable mappings from orig to this object's varMap
	for(map<varID, VarBitVector::boolPlus>::const_iterator it = srcMap.begin(); it != srcMap.end(); it++ )
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

bool VarBitVector::eq(VarBitVector &other)
{
	// if a bitvector is uninitialized, it isn't equal to any other bitvector
	if(!initialized || !other.initialized)
	{
//		printf("uninitialized: initialized=%d other.initialized=%d\n", initialized, other.initialized);
		return false;
	}
	
	// verify that all the variables that have a mapping in other.varMap's have 
	// a mapping in varMap and the mapping is in fact to the same value.
	for(map<varID, VarBitVector::boolPlus>::iterator it = varMap.begin(); it != varMap.end(); it++)
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
	for(map<varID, VarBitVector::boolPlus>::iterator it = other.varMap.begin(); it != other.varMap.end(); it++)
	{
		if(other.varMap[it->first] != varMap[it->first])
		{
//			printf("other.varMap: other.varMap[%s] (%s) != varMap[%s] (%s)\n", it->first.str().c_str(), other.varMap[it->first].str().c_str(), it->first.str().c_str(), varMap[it->first].str().c_str());
			return false;
		}
	}
	
	return true;
}


bool VarBitVector::operator != (VarBitVector &other)
{
	return !(*this).eq(other);
}

bool VarBitVector::operator == (VarBitVector &other)
{
	return (*this).eq(other);
}

// returns the current mapping of the given variable
VarBitVector::boolPlus VarBitVector::getVar(varID var)
{
	initialized = true; // this bitvector is now definitely initialized
	return varMap[var];
}

// maps the given variable to the given value
// returns true if this causes the bitvector to be modified and false otherwise
bool VarBitVector::setVar(varID var, bool val)
{
	VarBitVector::boolPlus newVal(val);
	return setVar(var, newVal);
}

// maps the given variable to the given value
// returns true if this causes the bitvector to be modified and false otherwise
bool VarBitVector::setVar(varID var, VarBitVector::boolPlus newVal)
{
	VarBitVector::boolPlus oldVal = varMap[var];
	varMap[var] = newVal;
	initialized = true; // this bitvector is now definitely initialized
	return oldVal!=newVal;
}

// returns the current mapping of the given variable
VarBitVector::boolPlus& VarBitVector::operator[] (varID var) { 
	initialized = true; // this bitvector is now definitely initialized
	return varMap[var];
}


// sets the mapping all of mapped variables to UnsetVal
// returns true if this causes the bitvector to be modified and false otherwise
bool VarBitVector::setToUnset()
{
	return setToVal(VarBitVector::boolPlus::UnsetVal);
}

// sets the mapping all of mapped variables to false
// returns true if this causes the bitvector to be modified and false otherwise
bool VarBitVector::setToBot()
{
	return setToVal(VarBitVector::boolPlus::FalseVal);
}

// sets the mapping all of mapped variables to false
// returns true if this causes the bitvector to be modified and false otherwise
bool VarBitVector::setToTop()
{
	return setToVal(VarBitVector::boolPlus::TrueVal);
}

// sets this bitvector to an uninitialized state
// if eraseVarMap==true, uninitialize also clears the variable mappings, including the sets of the variables being mapped
// otherwise, it leaves varMap alone, and simply resets the initialized flag
void VarBitVector::uninitialize(bool eraseVarMap)
{
	if(eraseVarMap)
		varMap.clear();
	initialized = false;
}

// sets this bitmap to map all variables to the given value
// returns true if this causes the bitvector to be modified and false otherwise
bool VarBitVector::setToVal(bool val)
{
	VarBitVector::boolPlus newVal(val);
	setToVal(newVal);	
}

// sets this bitmap to map all variables to the given value
// returns true if this causes the bitvector to be modified and false otherwise
bool VarBitVector::setToVal(VarBitVector::boolPlus val)
{
	bool modified = !initialized;
	// map all variables to 0
	for(map<varID, VarBitVector::boolPlus>::iterator it = varMap.begin(); it != varMap.end(); it++)
	{
		VarBitVector::boolPlus oldVal = varMap[it->first];
		varMap[it->first]=val;
		modified = modified || (oldVal!=val);
	}
	initialized = true; // this bitvector is now definitely initialized
	return modified;
}

// prints all variables mapped by this bitvector, showing the boolean value that each is mapped to
void VarBitVector::showVarMap(string mess)
{
	showVarMap(mess, "");
}

void VarBitVector::showVarMap(string mess, string indent)
{
	//printf("VarBitVector::showVarMap varMap.size()=%d\n", varMap.size());
	cout << indent << mess << "\n";
	if(!initialized) cout << indent << "[ininitialized]\n";
	for(map<varID, VarBitVector::boolPlus>::iterator it = varMap.begin(); it != varMap.end(); it++)
	{
		//cout << indent << it->second << " -> \t" << (varMap[it->first]? "True": "False") << "\n";
		printf("%s%15s -> %6s\n", indent.c_str(), ((varID)it->first).str().c_str(), it->second.str().c_str());
		fflush(stdout);
	}
}

// Computes the intersection of this bitvector with the given bitvector, returning this intersection
// If either vector is un-initialized, the intersection is just the other vector. If both veectors are uninitialized, 
// then the result is an uninitialized vector the contents of which are the intersection of the two input vectors.
VarBitVector* VarBitVector::intersectVec(VarBitVector &other)
{
	VarBitVector* newVBV = new VarBitVector();
	
	// go through all the variable mappings and for each variable that is shared by both this 
	// bitvector and the given bitvector, compute the AND of their mapped boolean values, saving
	// the results in newVBV
	for(map<varID, VarBitVector::boolPlus>::iterator it = other.varMap.begin(); it != other.varMap.end(); it++)
	{
		// if the current variable also exists in this bitvector's varMap
		if(varMap.find(it->first) != varMap.end())
		{
			if(initialized && other.initialized)
				newVBV->varMap[it->first] = varMap[it->first] && other.varMap[it->first];
			else if(initialized)
				newVBV->varMap[it->first] = varMap[it->first];
			else if(other.initialized)
				newVBV->varMap[it->first] = other.varMap[it->first];
			else
				newVBV->varMap[it->first] = varMap[it->first] && other.varMap[it->first];
/*			printf("newVBV->varMap[%s] = %s\n", (it->first).str().c_str(), newVBV->varMap[it->first].str().c_str());
			printf("varMap[%s] = %s\n", (it->first).str().c_str(), varMap[it->first].str().c_str());
			printf("other.varMap[%s] = %s\n", (it->first).str().c_str(), other.varMap[it->first].str().c_str());*/
		}
	}
	newVBV->initialized = initialized || other.initialized;
	
	return newVBV;
}

// Computes the union of this bitvector with the given bitvector, returning this union
// If either vector is un-initialized, the union is just the other vector. If both veectors are uninitialized, 
// then the result is an uninitialized vector the contents of which are the union  of the two input vectors.
VarBitVector* VarBitVector::unionVec(VarBitVector &other)
{
	VarBitVector* newVBV = new VarBitVector();
	
	// go through all the variable mappings and for each variable that is shared by both this 
	// bitvector and the given bitvector, compute the AND of their mapped boolean values, saving
	// the results in newVBV
	for(map<varID, VarBitVector::boolPlus>::iterator it = other.varMap.begin(); it != other.varMap.end(); it++)
	{
		// if the current variable also exists in this bitvector's varMap
		if(varMap.find(it->first) != varMap.end())
		{
			if(initialized && other.initialized)
				newVBV->varMap[it->first] = varMap[it->first] || other.varMap[it->first];
			else if(initialized)
				newVBV->varMap[it->first] = varMap[it->first];
			else if(other.initialized)
				newVBV->varMap[it->first] = other.varMap[it->first];
			else
				newVBV->varMap[it->first] = varMap[it->first] || other.varMap[it->first];
		}
	}
	newVBV->initialized = initialized || other.initialized;
	
	return newVBV;
}

// computes the intersection of f1 and f2, returning this intersection
VarBitVector *VarBitVector::intersectVec(VarBitVector &f1, VarBitVector &f2)
{
	return f1.intersectVec( f2 );
}

// computes the union of f1 and f2, returning this union
VarBitVector *VarBitVector::unionVec(VarBitVector &f1, VarBitVector &f2)
{
	return f1.unionVec( f2 );
}

// computes the intersection of this bitvector and the given bitvector, updating this bitvector to be that intersection
bool VarBitVector::intersectUpdate(VarBitVector &other)
{
	VarBitVector *inter = intersectVec(other);
	// if the intersection is different from this bitvector's initial value
	if ((*this) != (*inter))
	{
		// copy the new intersection into the current bitvector
		copyMap(inter->varMap);
		delete inter;
		return true; // inform the caller that this bitvector has changed
	}
	else
		return false; // inform the caller that this bitvector has not changed
}

// computes the union of this bitvector and the given bitvector, updating this bitvector to be that union
bool VarBitVector::unionUpdate(VarBitVector &other)
{
	VarBitVector *vecUnion = unionVec(other);
	// if the union is different from this bitvector's initial value
	if ((*this) != (*vecUnion))
	{
		// copy the new union into the current bitvector
		copyMap(vecUnion->varMap);
		delete vecUnion;
		return true; // inform the caller that this bitvector has changed
	}
	else
		return false; // inform the caller that this bitvector has not changed
}

// returns true if the bitvector is =bottom (vector of all falses) and false otherwise
bool VarBitVector::isBot()
{
	// uninitialized bitvectors are not equal to any other bitvector
	if(!initialized) return false;
	return isAllEq(VarBitVector::boolPlus::FalseVal);
}

// returns true if the bitvector is =top (vector of all trues) and false otherwise
bool VarBitVector::isTop()
{
	// uninitialized bitvectors are not equal to any other bitvector
	if(!initialized) return false;
	return isAllEq(VarBitVector::boolPlus::TrueVal);
}

// returns true if the bitvector maps all of its variables to val and false otherwise
bool VarBitVector::isAllEq(bool val)
{
	VarBitVector::boolPlus testVal(val);
	return isAllEq(testVal);
}

// returns true if the bitvector maps all of its variables to val and false otherwise	
bool VarBitVector::isAllEq(VarBitVector::boolPlus val)
{
	// go through all the variable mappings, returning false if any variable maps to true
	for(map<varID, VarBitVector::boolPlus>::iterator it = varMap.begin(); it != varMap.end(); it++)
	{
		// if the current variable is not mapped to val, return false
		if(varMap[it->first] != val)
			return false;
	}
	
	// if every variable is mapped to val, return true
	return true;
}

// returns the set of variables mapped by this VarBitVector
varIDSet& VarBitVector::getMappedVars()
{
	return vars;	
}
