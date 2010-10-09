#ifndef VarLatticeVector_H
#define VarLatticeVector_H

#include "common.h"
#include "variables.h"


class VarLatticeVector{
public:
	class Lattice
	{
		public :
		Lattice(Lattice &that) {}
		virtual void meet(Lattice& that) { ROSE_ASSERT(!"VarLatticeVector::Lattice::meet()!"); }
		virtual void join(Lattice& that) { ROSE_ASSERT(!"VarLatticeVector::Lattice::join()!"); }
		
		//static virtual Lattice getMinVal() { ROSE_ASSERT(!"VarLatticeVector::Lattice::getMinVal()!"); return *this; }
		virtual Lattice getMinVal() { ROSE_ASSERT(!"VarLatticeVector::Lattice::getMinVal()!"); return *this; }
		//static virtual Lattice getMaxVal() { ROSE_ASSERT(!"VarLatticeVector::Lattice::getMaxVal()!"); return *this; }
		virtual Lattice getMaxVal() { ROSE_ASSERT(!"VarLatticeVector::Lattice::getMaxVal()!"); return *this; }
	};
	
	
	protected:
	Lattice sample;
		
	// maps variables to boolean values
	map<varID, Lattice> varMap;
	
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
	
	VarLatticeVector(Lattice sample, bool initialized=true)
	{
		this->initialized = initialized;
		this->sample = sample;
	}
	
	VarLatticeVector(Lattice sample, varIDSet& vars, bool initialized=true)
	{
		init(vars, sample.getMinVal(), initialized);
	}
	
	/*VarLatticeVector(varIDSet& vars, bool initValue, bool initialized=true)
	{
		boolPlus initVal(initValue ? boolPlus::TrueVal : boolPlus::FalseVal);
		init(vars, initVal, initialized);
	}*/
	
	VarLatticeVector(varIDSet& vars, Lattice initVal, bool initialized=true)
	{
		init(vars, initVal, initialized);
	}
	
	protected:
	void init(varIDSet& vars, Lattice initVal, bool initialized);

	public:
	VarLatticeVector(VarLatticeVector* orig, bool initialized=true)
	{
		sample = orig->sample;
		// copy all variable mappings from orig to this object's varMap
		copyMap(orig->varMap);
		this->vars = vars;
		this->initialized = initialized;
	}

	VarLatticeVector(const VarLatticeVector& orig, bool initialized=true)
	{
		sample = orig.sample;
		// copy all variable mappings from orig to this object's varMap
		copyMap(orig.varMap);
		this->vars = vars;
		this->initialized = initialized;
	}
	
	~VarLatticeVector()
	{
		varMap.clear();
	}

	// copies the data from the given bitvector to this bitvector.
	// returns true if this results in a change in this bitvector or false otherwise
	// (change = among the variables mapped by bitvector src)
	// if there's a variable in srcMap that is not in this, this variable's mapping is copied to this
	// if there's a variable in this that is not in srcMap, it is unchanged
	bool copy(VarLatticeVector& src);
	
protected:
	// copies the contents of the given map into this bitvector's map
	// returns true if the mappings of variables in srcMap are different from
	//    those originally in varMap and returns false otherwise
	// if there's a variable in srcMap that is not in this, this variable's mapping is copied to this
	// if there's a variable in this that is not in srcMap, it is unchanged
	bool copyMap(const map<varID, Lattice> srcMap);

public:
	bool eq ( VarLatticeVector &other );

	
	bool operator != ( VarLatticeVector &other );
	
	bool operator == ( VarLatticeVector &other );

	// returns the current mapping of the given variable
	Lattice getVar(varID var);
		
	// maps the given variable to the given value, 
	// returns true if this causes the bitvector to be modified and false otherwise
	bool setVar(varID var, Lattice newVal);
		
	// returns the current mapping of the given variable
	Lattice& operator[] (varID var);
	
	// sets the mapping all of mapped variables to the minimum lattice value
	// returns true if this causes the bitvector to be modified and false otherwise
	bool setToMin();
	
	// sets the mapping all of mapped variables to the maximum lattice value
	// returns true if this causes the bitvector to be modified and false otherwise
	bool setToMax();
	
	// sets this bitvector to an uninitialized state, erasing any of its state
	void uninitialize();

	// sets this bitmap to map all variables to the given value
	// returns true if this causes the bitvector to be modified and false otherwise
	bool setToVal(Lattice val);
	
public:
	// prints all variables mapped by this bitvector, showing the boolean value that each is mapped to
	void showVarMap(string mess);
	
	void showVarMap(string mess, string indent);
	
	// Computes the meet of this bitvector with the given bitvector, returning this meet
	VarLatticeVector* meetVec(VarLatticeVector &other);

	// Computes the join of this bitvector with the given bitvector, returning this join
	VarLatticeVector* joinVec(VarLatticeVector &other);
	
	// computes the meet of f1 and f2, returning this meet
	static VarLatticeVector *meetVec(VarLatticeVector &f1, VarLatticeVector &f2);
	
	// computes the join of f1 and f2, returning this join
	static VarLatticeVector *joinVec(VarLatticeVector &f1, VarLatticeVector &f2);

	// computes the meet of this bitvector and the given bitvector, updating this bitvector to be that meet
	bool meetUpdate(VarLatticeVector &other);
	
	// computes the join of this bitvector and the given bitvector, updating this bitvector to be that join
	bool joinUpdate(VarLatticeVector &other);
	
	// returns true if the bitvector is =bottom (vector of all mins) and false otherwise
	bool isMin();
	
	// returns true if the bitvector is =top (vector of all maxes) and false otherwise
	bool isMax();

	// returns true if the bitvector maps all of its variables to val and false otherwise
	bool isAllEq(Lattice val);

public:

	// returns the set of variables mapped by this VarLatticeVector
	varIDSet& getMappedVars();
};

#endif
