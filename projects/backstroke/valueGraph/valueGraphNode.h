#ifndef BACKSTROKE_VALUEGRAPHNODE2_H
#define	BACKSTROKE_VALUEGRAPHNODE2_H

#include "types.h"
#include <rose.h>
#include <boost/lexical_cast.hpp>

namespace Backstroke
{

typedef std::vector<SgInitializedName*> VarName;


struct PhiNodeDependence
{
//	enum ControlDependenceType
//	{
//		cdTrue,
//		cdFalse,
//		cdCase,
//		cdDefault
//	};

	PhiNodeDependence(int v)
	: version(v) {}

	//! One version member of the phi function.
	int version;

	//!  The SgNode on which the phi function has a control dependence for the version.
	SgNode* cdNode;

	////! The control dependence type.
	//ControlDependenceType cdType;

	////! If the control dependence is cdCase, this is the case value.
	//int caseValue;
};


struct VersionedVariable
{
	VersionedVariable() {}
	VersionedVariable(const VarName& varName, int ver, bool pseudoDef = false)
	: name(varName), version(ver), isPseudoDef(pseudoDef) {}

	std::string toString() const;
	
	//! The unique name of this variable.
	VarName name;

	//! The version of this variable in the SSA form.
	int version;

	//! Indicated if this variable is defined by a phi function.
	bool isPseudoDef;

	//! All versions it dependes if this variable is a pseudo def.
	std::vector<PhiNodeDependence> phiVersions;
};


inline bool operator == (const VersionedVariable& var1, const VersionedVariable& var2)
{
	return var1.name == var2.name && var1.version == var2.version;
}

inline bool operator < (const VersionedVariable& var1, const VersionedVariable& var2)
{
	return var1.name < var2.name ||
		(var1.name == var2.name && var1.version < var2.version);
}

inline std::ostream& operator << (std::ostream& os, const VersionedVariable& var)
{
	return os << var.toString();
}


/**********************************************************************************************************/
// Value Graph Nodes


struct ValueGraphNode
{
	ValueGraphNode() {}

	virtual std::string toString() const
	{ return ""; }

    virtual int getCost() const
    { return 0; }
};

struct PhiNode : ValueGraphNode
{
    enum GateType
    {
        phi,
        mu,
        eta
    };

	PhiNode(const VersionedVariable& v) : var(v), dagIndex(0), type(phi) {}

	//std::vector<ValueGraphNode*> nodes;

	virtual std::string toString() const
	{
		return "PHI\\n" + var.toString();
	}

    virtual int getCost() const;

    SgType* getType() const
    { return var.name.back()->get_type(); }

    //! The versioned variable it contains.
	VersionedVariable var;

    //! The DAG index.
    int dagIndex;

    //! The type of this gate function
    GateType type;
};

struct ValueNode : ValueGraphNode
{
	ValueNode() : astNode(NULL), cost(0) {}
	ValueNode(SgNode* node) : astNode(node), cost(0) {}
	//ValueNode(SgValueExp* exp) : valueExp(exp), type(NULL), cost(0) {}

	virtual std::string toString() const;

    virtual int getCost() const;

	bool isTemp() const { return vars.empty(); }

    //! If the AST node is a value expression, it is avaiable.
	bool isAvailable() const { return isSgValueExp(astNode) != NULL; }

    SgType* getType() const;

    //! The AST node which defines the variables.
    SgNode* astNode;
    
	//SgValueExp* valueExp;
	//SgType* type;
	std::vector<VersionedVariable> vars;
    int cost;
};

struct OperatorNode : ValueGraphNode
{
	enum OperatorType
	{
		otAdd,
		otSubtract,
		otMultiply,
		otDivide,
		otMod,
		//otAssign,
		otGreaterThan,
		otLessThan,
		otUnknown
	};

	OperatorNode(OperatorType t) : ValueGraphNode(), type(t) {}
	OperatorNode(VariantT t) : ValueGraphNode(), type(getOperatorType(t)) {}

	//! Given a variantT value, return a coresponding operator type.
	static OperatorType getOperatorType(VariantT t);
	
	virtual std::string toString() const;
	
	OperatorType type;
};

#if 0
struct UnaryOperaterNode : OperatorNode
{
	enum OperationType
	{
		otPlus,
		otMinus
	} type;

	ValueGraphNode* operand;
};


struct BinaryOperaterNode : OperatorNode
{
	enum OperaterType
	{
		otAdd,
		otMinus,
		otMultiply,
		otDevide,
		otMod,
		otAssign,
	} type;

	BinaryOperaterNode(OperaterType t) : type(t) {}
	ValueGraphNode* lhsOperand;
	ValueGraphNode* rhsOperand;
};
#endif

struct FunctionCallNode : ValueGraphNode
{

};

/**********************************************************************************************************/
// Value Graph Edges

struct ValueGraphEdge
{
	ValueGraphEdge() : cost(0), dagIndex(0) {}
	ValueGraphEdge(int c) : cost(c), dagIndex(0) {}

    virtual ~ValueGraphEdge() {}

	virtual std::string toString() const;

    //! The cost attached on this edge. The cost may come from state saving,
    //! or operations.
	int cost;

    //! A CFG may be seperated into several DAGs, and each DAG have its own path
    //! numbers. This index represents which DAG the following paths belong to.
    int dagIndex;

    //! All paths this relationship exists.
    PathSet paths;
};

//! An edge coming from an operator node.
struct OrderedEdge : ValueGraphEdge
{
	OrderedEdge(int idx) : index(idx) {}

	virtual std::string toString() const
	{ return "index:" + boost::lexical_cast<std::string>(index); }

	int index;
};

/**********************************************************************************************************/
// Utility functions

//inline TempVariableNode* isTempVariableNode(ValueGraphNode* node)
//{
//	return dynamic_cast<TempVariableNode*>(node);
//}

inline PhiNode* isPhiNode(ValueGraphNode* node)
{
	return dynamic_cast<PhiNode*>(node);
}

inline OperatorNode* isOperatorNode(ValueGraphNode* node)
{
	return dynamic_cast<OperatorNode*>(node);
}

inline ValueNode* isValueNode(ValueGraphNode* node)
{
	return dynamic_cast<ValueNode*>(node);
}

inline OrderedEdge* isOrderedEdge(ValueGraphEdge* edge)
{
	return dynamic_cast<OrderedEdge*>(edge);
}

}  // End of namespace Backstroke

#endif	/* BACKSTROKE_VALUEGRAPHNODE2_H */

