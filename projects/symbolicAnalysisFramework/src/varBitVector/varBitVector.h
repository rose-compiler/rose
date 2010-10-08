#ifndef VARBITVECTOR_H
#define VARBITVECTOR_H

#include "common.h"
#include "variables.h"

class VarBitVector{
public:
	class boolPlus
	{
		int val;
		
		public:
                // Liao 10/7/2010, follow convention for static class variables
		const static int UnsetVal; // = -1;
		const static int FalseVal; // = 0;
		const static int TrueVal;  // = 1;
			
		boolPlus() { val = UnsetVal; }
		
		boolPlus(bool initVal)
		{
			val = (initVal? TrueVal: FalseVal);
		}
		
		boolPlus(int initVal)
		{
			val = initVal;
		}
	
		boolPlus(const boolPlus &initVal)
		{
			val = initVal.val;
		}
			
		void set(int newVal)
		{
			val = newVal;
		}
	
		void set(bool newVal)
		{
			val = newVal? TrueVal: FalseVal;
		}
		
		bool isUnset() const
		{
			return val==UnsetVal;
		}
		
		bool isFalse() const
		{
			return val==FalseVal;
		}
		
		bool isTrue() const
		{
			return val==TrueVal;
		}
		
		bool operator == (const boolPlus &that) const
		{
			return val == that.val;
		}

		bool operator == (const bool &that) const
		{
			boolPlus thatVal(that);
			return (*this) == thatVal;
		}

		bool operator != (const boolPlus &that) const
		{
			return !(*this == that);
		}

		bool operator != (const bool &that) const
		{
			return !(*this == that);
		}
		
		bool operator < (const boolPlus &that) const
		{
			if(val == that.val)
				return false;
			
			if(val==UnsetVal) return true;
			if(that.val==UnsetVal) return false;
			if(val==false) return true;
			if(that.val==false) return false;
			
			ROSE_ASSERT(!"ERROR: impossible case for boolPlus::<!");
		}
		bool operator < (const bool &that) const
		{
			boolPlus thatVal(that);
			return (*this) < thatVal;
		}

		bool operator > (const boolPlus &that) const
		{
			return !(*this < that);
		}
		bool operator > (const bool &that) const
		{
			return !(*this < that);
		}
		
		bool operator >= (const boolPlus &that) const
		{
			return (*this > that) || (*this == that);
		}
		bool operator >= (const bool &that) const
		{
			return (*this > that) || (*this == that);
		}
		
		bool operator <= (const boolPlus &that) const
		{
			return (*this < that) || (*this == that);
		}
		bool operator <= (const bool &that) const
		{
			return (*this < that) || (*this == that);
		}
		
		boolPlus operator && (const boolPlus &that) const
		{
			boolPlus res;
			if(*this <= that) res.val = val;
			else if(that <= *this) res.val = that.val;
			else ROSE_ASSERT(!"ERROR in boolPlus::&&. Impossible case!");
			
			return res;
		}
		boolPlus operator && (const bool &that) const
		{
			boolPlus thatVal(that);
			return (*this) && thatVal;
		}
		
		boolPlus operator || (const boolPlus &that) const
		{
			boolPlus res;
			if(*this >= that) res.val = val;
			else if(that >= *this) res.val = that.val;
			else ROSE_ASSERT(!"ERROR in boolPlus::&&. Impossible case!");
			
			return res;
		}
		bool operator || (const bool &that) const
		{
			boolPlus thatVal(that);
			boolPlus ret = (*this) || thatVal;
			ROSE_ASSERT(ret.val != UnsetVal);
			
			if(ret.val == TrueVal) return true;
			else return false;
		}
		
		string str()
		{
			return (val==UnsetVal? "Unset":
			       (val==FalseVal? "False":
			       (val==TrueVal? "True": "???"
			       )));
		}
	};
	
	static boolPlus Unset;
	static boolPlus False;
	static boolPlus True;
	
	protected:
	// maps variables to boolean values
	map<varID, boolPlus> varMap;
	
	// set of variables mapped by this map
	varIDSet vars;
	
	// flag indicating whether this vector should be considered initialized (its 
	//    variable mapping is real) or uninitialized (the variable mapping 
	//    does not correspond to a real state). Uninitialized bitvectors behave
	//    just like regular bit-vectors but they are not equal to any other bitvector
	//    until they are initialized. Any operation that modifies or reads the state  
	//    of the bitvector (not including comparisons or other operations that don't
	//    access individual variable mappings) causes it to become initialized 
	//    (if it wasn't already). An uninitialized bitvector is neither =top nor =bottom.
	//    printing a bitvector's contents does not make it initialized
	bool initialized;
	
	public:
	
	VarBitVector(bool initialized=true)
	{
		this->initialized = initialized;
	}
	
	VarBitVector(varIDSet& vars, bool initialized=true)
	{
		boolPlus unsetVal(boolPlus::UnsetVal);
		init(vars, unsetVal, initialized);
	}
	
	/*VarBitVector(varIDSet& vars, bool initValue, bool initialized=true)
	{
		boolPlus initVal(initValue ? boolPlus::TrueVal : boolPlus::FalseVal);
		init(vars, initVal, initialized);
	}*/
	
	VarBitVector(varIDSet& vars, boolPlus initVal, bool initialized=true)
	{
		init(vars, initVal, initialized);
	}
	
	protected:
	void init(varIDSet& vars, boolPlus initVal, bool initialized);

	public:
	VarBitVector(VarBitVector* orig, bool initialized=true)
	{
		// copy all variable mappings from orig to this object's varMap
		copyMap(orig->varMap);
		this->vars = vars;
		this->initialized = initialized;
	}

	VarBitVector(const VarBitVector& orig, bool initialized=true)
	{
		// copy all variable mappings from orig to this object's varMap
		copyMap(orig.varMap);
		this->vars = vars;
		this->initialized = initialized;
	}
	
	~VarBitVector()
	{
		varMap.clear();
	}

	// copies the data from the given bitvector to this bitvector.
	// returns true if this results in a change in this bitvector or false otherwise
	// (change = among the variables mapped by bitvector src)
	// if there's a variable in srcMap that is not in this, this variable's mapping is copied to this
	// if there's a variable in this that is not in srcMap, it is unchanged
	bool copy(VarBitVector& src);
	
protected:
	// copies the contents of the given map into this bitvector's map
	// returns true if the mappings of variables in srcMap are different from
	//    those originally in varMap and returns false otherwise
	// if there's a variable in srcMap that is not in this, this variable's mapping is copied to this
	// if there's a variable in this that is not in srcMap, it is unchanged
	bool copyMap(const map<varID, boolPlus> srcMap);

public:
	bool eq ( VarBitVector &other );

	
	bool operator != ( VarBitVector &other );
	
	bool operator == ( VarBitVector &other );

	// returns the current mapping of the given variable
	boolPlus getVar(varID var);
	
	// maps the given variable to the given value, 
	// returns true if this causes the bitvector to be modified and false otherwise
	bool setVar(varID var, bool val);
	
	// maps the given variable to the given value, 
	// returns true if this causes the bitvector to be modified and false otherwise
	bool setVar(varID var, boolPlus newVal);
		
	// returns the current mapping of the given variable
	boolPlus& operator[] (varID var);
	
	// sets the mapping all of mapped variables to UnsetVal
	// returns true if this causes the bitvector to be modified and false otherwise
	bool setToUnset();
	
	// sets the mapping all of mapped variables to false
	// returns true if this causes the bitvector to be modified and false otherwise
	bool setToBot();
	
	// sets the mapping all of mapped variables to false
	// returns true if this causes the bitvector to be modified and false otherwise
	bool setToTop();
	
	// sets this bitvector to an uninitialized state
	// if eraseVarMap==true, uninitialize also clears the variable mappings, including the sets of the variables being mapped
	// otherwise, it leaves varMap alone, and simply resets the initialized flag
	void uninitialize(bool eraseVarMap=true);

	// sets this bitmap to map all variables to the given value
	// returns true if this causes the bitvector to be modified and false otherwise
	bool setToVal(bool val);
	
	// sets this bitmap to map all variables to the given value
	// returns true if this causes the bitvector to be modified and false otherwise
	bool setToVal(boolPlus val);
	
public:
	// prints all variables mapped by this bitvector, showing the boolean value that each is mapped to
	void showVarMap(string mess);
	
	void showVarMap(string mess, string indent);
	
	// Computes the intersection of this bitvector with the given bitvector, returning this intersection
	// If either vector is un-initialized, the intersection is just the other vector. If both veectors are uninitialized, 
	// then the result is an uninitialized vector the contents of which are the intersection of the two input vectors.
	VarBitVector* intersectVec(VarBitVector &other);

	// Computes the union of this bitvector with the given bitvector, returning this intersection
	// If either vector is un-initialized, the union is just the other vector. If both veectors are uninitialized, 
	// then the result is an uninitialized vector the contents of which are the union  of the two input vectors.
	VarBitVector* unionVec(VarBitVector &other);
	
	// computes the intersection of f1 and f2, returning this intersection
	static VarBitVector *intersectVec(VarBitVector &f1, VarBitVector &f2);
	
	// computes the union of f1 and f2, returning this union
	static VarBitVector *unionVec(VarBitVector &f1, VarBitVector &f2);

	// computes the intersection of this bitvector and the given bitvector, updating this bitvector to be that intersection
	bool intersectUpdate(VarBitVector &other);
	
	// computes the union of this bitvector and the given bitvector, updating this bitvector to be that union
	bool unionUpdate(VarBitVector &other);
	
	// returns true if the bitvector is =bottom (vector of all falses) and false otherwise
	bool isBot();
	
	// returns true if the bitvector is =top (vector of all trues) and false otherwise
	bool isTop();

protected:
	// returns true if the bitvector maps all of its variables to val and false otherwise
	bool isAllEq(bool val);

	// returns true if the bitvector maps all of its variables to val and false otherwise
	bool isAllEq(boolPlus val);
public:

	// returns the set of variables mapped by this VarBitVector
	varIDSet& getMappedVars();
};

#endif
