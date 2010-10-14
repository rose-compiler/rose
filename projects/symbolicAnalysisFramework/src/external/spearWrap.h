#ifndef SPEAR_WRAP_H
#define SPEAR_WRAP_H

#include "common.h"
#include "cfgUtils.h"
#include "logical.h"
#include "variables.h"


#ifdef THREADED
#include <pthread.h>
#endif

class SpearType
{
	int nb;
	
	public:
	SpearType(int numBits);
	
	SpearType(const SpearType& type);
	
	// returns the Spear name of this type
	string typeName() const;
	
	// returns the number of bits in this type
	int numBits() const;
	
	// returns the Spear name of a type with this many bits
	static string typeName(int numBits);
};

// Represents a Spear variable or constant, including its name and type
class SpearAbstractVar
{
	public:
	
	// Returns the variable's name
	virtual string getName()=0;
	
	// Returns the declaration of this variable
	virtual string varDecl()=0;
	
	// Returns the type of this variable
	virtual SpearType getType()=0;
	
	bool operator==(SpearAbstractVar& that)
	{ return getName()==that.getName() && varDecl()==that.varDecl(); }
	
	bool operator<=(SpearAbstractVar& that)
	{
		return getName()<that.getName() ||
		       (getName()==that.getName() && varDecl()<that.varDecl()) || 
		       (getName()==that.getName() && varDecl()==that.varDecl());
	}
	
	bool operator<(SpearAbstractVar& that)
	{
		return getName()<that.getName() ||
		       (getName()==that.getName() && varDecl()<that.varDecl());
	}
};

// Represents a Spear variable, including its name and type
class SpearVar: public virtual SpearAbstractVar
{
	string    name;
	SpearType type;
	
	public:
	SpearVar(string varName, SpearType type, int seUID);
	
	SpearVar(string varName, int numBits, int seUID);
	
	SpearVar(varID var, SpearType type, int seUID);
	
	SpearVar(varID var, int numBits, int seUID);
	
	SpearVar(string varName, SpearType type);
	
	SpearVar(string varName, int numBits);
	
	SpearVar(varID var, SpearType type);
	
	SpearVar(varID var, int numBits);
	
	// Returns the declaration of this variable
	string varDecl();
	
	// Returns the variable's name
	string getName();
	
	// Returns the type of this variable
	SpearType getType();
};

// Represents a Spear constant, including its value and type
class SpearConst: public virtual SpearAbstractVar
{
	int val;
	SpearType type;
	
	public:
	SpearConst(int val, SpearType type);
	
	SpearConst(int val, int numBits);
	
	// Returns the declaration of this constant
	string varDecl();
	
	// Returns the constant's name
	string getName();
	
	// Returns the type of this variable
	SpearType getType();
};

class SpearOp
{
	#ifdef THREADED
	static pthread_mutex_t SOMutex;
	#endif
	
	public:
	// Possible Spear operations
	static const int None            = -1;
	static const int AndOp           = 0;
	static const int OrOp            = 1;
	static const int XorOp           = 2;
	static const int NotOp           = 3;
	static const int Implies         = 4;
	static const int Equal           = 5;
	static const int NotEqual        = 6;
	static const int UnsLTE          = 7;
	static const int SgnLTE          = 8;
	static const int UnsLT           = 9;
	static const int SgnLT           = 10;
	static const int UnsGTE          = 11;
	static const int SgnGTE          = 12;
	static const int UnsGT           = 13;
	static const int SgnGT           = 14;
	static const int IfThenElse      = 15;
	static const int Add             = 16;
	static const int Subtract        = 17;
	static const int Multiply        = 18;
	static const int UnsDivide       = 19;
	static const int SgnDivide       = 20;
	static const int UnsRemain       = 21;
	static const int SgnRemain       = 22;
	static const int LeftShift       = 23;
	static const int LogRightShift   = 24;
	static const int ArithRightShift = 25;
	static const int Truncate        = 26;
	static const int SgnExtend       = 27;
	static const int ZeroExtend      = 28;
	static const int Concatenate     = 29;
	static const int Extract         = 30;
	
	private:
	static bool int2strInitialized;
	static map<int, string> int2str;
	public:
	static string opStr(int op);
	
	private:
	int op;
	public:
	SpearOp(int op): op(op) {}
	int getOp() const { return op; }
	int setOp(int op) { this->op = op; }
	bool isOp(int op) const { return this->op == op; }
	bool operator == (const SpearOp& that) { return op == that.op; }
	string opStr() const;
};

// Parent class for objects that represent logical expressions that can be solved using Spear
class SpearExpr
{
	private:
	static int globalUID;
	int uid;
	
	#ifdef THREADED
	static pthread_mutex_t SEMutex;
	#endif
	
	public:
	SpearExpr()
	{
		#ifdef THREADED
		pthread_mutex_lock(&SEMutex);
		#endif

		uid = globalUID;
		globalUID++;

		#ifdef THREADED
		pthread_mutex_unlock(&SEMutex);
		#endif
	}
	
	protected:
	int getUID() { return uid; }

	public:	
	// returns a list of the names of all the variables needed to represent the logical 
	// expression in this object
	virtual const list<SpearAbstractVar*>& getVars()=0;
	
	// returns the variable that represents the result of this expression
	virtual SpearAbstractVar* getOutVar()=0;
	
	// returns the SPEAR Format expression that represents the constraint in this object
	virtual const string& getExpr()=0;	
	
	// Returns a human-readable string representation of this expression.
	// Every line of this string must be prefixed by indent.
	// The last character of the returned string should not be '\n', even if it is a multi-line string.
	virtual string str(string indent="")=0;
	
	virtual SpearExpr* copy()=0;
};

#endif
