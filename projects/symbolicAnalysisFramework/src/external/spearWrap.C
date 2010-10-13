#include "spearWrap.h"

/*************
 * SpearType *
 *************/
SpearType::SpearType(int numBits) { nb = numBits; }
	
SpearType::SpearType(const SpearType& type) { nb = type.nb; }

// returns the Spear name of this type
string SpearType::typeName() const
{
	ostringstream ss;
	ss << "i" << nb;
	return ss.str();
}

// returns the number of bits in this type
int SpearType::numBits() const
{
	return nb;
}

// returns the Spear name of a type with this many bits
string SpearType::typeName(int numBits)
{
	ostringstream ss;
	ss << "i" << numBits;
	return ss.str();
}

/************
 * SpearVar *
 ************/
 
SpearVar::SpearVar(string name, SpearType type, int seUID): type(type)
{
	ostringstream ss;
	ss << name << "_" << seUID;
	this->name = ss.str();
}

SpearVar::SpearVar(string name, int numBits, int seUID): type(numBits)
{
	ostringstream ss;
	ss << name << "_" << seUID;
	this->name = ss.str();
}

SpearVar::SpearVar(varID var, SpearType type, int seUID): type(type)
{
	ostringstream ss;
	ss << var.str() << "_" << seUID;
	this->name = ss.str();
}

SpearVar::SpearVar(varID var, int numBits, int seUID): type(numBits)
{
	ostringstream ss;
	ss << var.str() << "_" << seUID;
	this->name = ss.str();
}

SpearVar::SpearVar(string name, SpearType type): type(type)
{
	ostringstream ss;
	ss << name;
	this->name = ss.str();
}

SpearVar::SpearVar(string name, int numBits): type(numBits)
{
	ostringstream ss;
	ss << name;
	this->name = ss.str();
}

SpearVar::SpearVar(varID var, SpearType type): type(type)
{
	ostringstream ss;
	ss << var.str();
	this->name = ss.str();
}

SpearVar::SpearVar(varID var, int numBits): type(numBits)
{
	ostringstream ss;
	ss << var.str();
	this->name = ss.str();
}

// Returns the declaration of this variable
string SpearVar::varDecl()
{
	ostringstream ss;
	ss << name << ":" << type.typeName();
	return ss.str();
}

// Returns the variable's name
string SpearVar::getName()
{
	return name;
}

// Returns the type of this variable
SpearType SpearVar::getType()
{
	return type;
}

/**************
 * SpearConst *
 **************/
SpearConst::SpearConst(int val, SpearType type): val(val), type(type) {}

SpearConst::SpearConst(int val, int numBits): val(val), type(numBits) {}

// Returns the declaration of this constant
string SpearConst::varDecl()
{
	// constants don't need to be declared
	return "";
}

// Returns the constant's name
string SpearConst::getName()
{
	ostringstream ss;
	ss << val << ":" << type.typeName();
	return ss.str();
}

// Returns the type of this variable
SpearType SpearConst::getType()
{
	return type;
}

/***********
 * SpearOp *
 ***********/
#ifdef THREADED
pthread_mutex_t SpearOp::SOMutex = PTHREAD_MUTEX_INITIALIZER;
#endif

const int SpearOp::None            ;
const int SpearOp::AndOp           ;
const int SpearOp::OrOp            ;
const int SpearOp::XorOp           ;
const int SpearOp::NotOp           ;
const int SpearOp::Implies         ;
const int SpearOp::Equal           ;
const int SpearOp::NotEqual        ;
const int SpearOp::UnsLTE          ;
const int SpearOp::SgnLTE          ;
const int SpearOp::UnsLT           ;
const int SpearOp::SgnLT           ;
const int SpearOp::UnsGTE          ;
const int SpearOp::SgnGTE          ;
const int SpearOp::UnsGT           ;
const int SpearOp::SgnGT           ;
const int SpearOp::IfThenElse      ;
const int SpearOp::Add             ;
const int SpearOp::Subtract        ;
const int SpearOp::Multiply        ;
const int SpearOp::UnsDivide       ;
const int SpearOp::SgnDivide       ;
const int SpearOp::UnsRemain       ;
const int SpearOp::SgnRemain       ;
const int SpearOp::LeftShift       ;
const int SpearOp::LogRightShift   ;
const int SpearOp::ArithRightShift ;
const int SpearOp::Truncate        ;
const int SpearOp::SgnExtend       ;
const int SpearOp::ZeroExtend      ;
const int SpearOp::Concatenate     ;
const int SpearOp::Extract         ;

bool SpearOp::int2strInitialized = false;
map<int, string> SpearOp::int2str;

string SpearOp::opStr(int op)
{
	#ifdef THREADED
	pthread_mutex_lock(&SOMutex);
	#endif

	if(!int2strInitialized)
	{
		int2strInitialized = true;
		int2str[None]            = "???";
		int2str[AndOp]           = "&";
		int2str[OrOp]            = "|";
		int2str[XorOp]           = "^";
		int2str[NotOp]           = "~";
		int2str[Implies]         = "=>";
		int2str[Equal]           = "=";
		int2str[NotEqual]        = "/=";
		int2str[UnsLTE]          = "ule";
		int2str[SgnLTE]          = "sle";
		int2str[UnsLT]           = "ult";
		int2str[SgnLT]           = "slt";
		int2str[UnsGTE]          = "uge";
		int2str[SgnGTE]          = "sge";
		int2str[UnsGT]           = "ugt";
		int2str[SgnGT]           = "sgt";
		int2str[IfThenElse]      = "ite";
		int2str[Add]             = "+";
		int2str[Subtract]        = "-";
		int2str[Multiply]        = "*";
		int2str[UnsDivide]       = "/u";
		int2str[SgnDivide]       = "/s";
		int2str[UnsRemain]       = "%u";
		int2str[SgnRemain]       = "%s";
		int2str[LeftShift]       = "<<";
		int2str[LogRightShift]   = ">>l";
		int2str[ArithRightShift] = ">>a";
		int2str[Truncate]        = "trun";
		int2str[SgnExtend]       = "sext";
		int2str[ZeroExtend]      = "zext";
		int2str[Concatenate]     = "conc";
		int2str[Extract]         = "extr";
	}
	#ifdef THREADED
	pthread_mutex_unlock(&SOMutex);
	#endif
	
	return int2str[op];
}

string SpearOp::opStr() const
{
	return SpearOp::opStr(op);
}

/*************
 * SpearExpr *
 *************/
int SpearExpr::globalUID=0;
#ifdef THREADED
pthread_mutex_t SpearExpr::SEMutex = PTHREAD_MUTEX_INITIALIZER;
#endif
