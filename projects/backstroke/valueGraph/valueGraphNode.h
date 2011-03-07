#ifndef BACKSTROKE_VALUEGRAPHNODE_H
#define	BACKSTROKE_VALUEGRAPHNODE_H

#include <rose.h>
#include <ssa/staticSingleAssignment.h>

namespace Backstroke
{

typedef std::vector<SgInitializedName*> VarName;


struct PhiNodeDependence
{
	enum ControlDependenceType
	{
		cdTrue,
		cdFalse,
		cdCase,
		cdDefault
	};

	PhiNodeDependence(int v)
	: version(v) {}

	//! One version member of the phi function.
	int version;

	//!  The SgNode on which the phi function has a control dependence for the version.
	SgNode* cdNode;

	//! The control dependence type.
	ControlDependenceType cdType;

	//! If the control dependence is cdCase, this is the case value.
	int caseValue;
};

struct VersionedVariable
{
	VersionedVariable() {}
	VersionedVariable(const VarName& varName, int ver, bool pseudoDef = false)
	: name(varName), version(ver), isPseudoDef(pseudoDef) {}

	//! The unique name of this variable.
	VarName name;

	//! The version of this variable in the SSA form.
	int version;

	//! Indicated if this variable is defined by a phi function.
	bool isPseudoDef;

	//! All version it dependes if this variable is a pseudo def.
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
	if (var.name.empty())
		return os << "TEMP";
	return os << StaticSingleAssignment::varnameToString(var.name) << ' ' << var.version;
}


struct ValueGraphNode
{
	ValueGraphNode() {}

	virtual void writeDotString(std::ostream& out) const
	{
		out << "[label=\"\"]";
	}
};

struct TempVariableNode : ValueGraphNode
{
	TempVariableNode(SgType* t) : type(t) {}
	
	virtual void writeDotString(std::ostream& out) const
	{
		out << "[label=\"" << "TEMP" << "\"]";
	}

	SgType* type;
};

struct VariableNode : ValueGraphNode
{
	VariableNode() {}
	VariableNode(const VarName& name, int ver)
	: var(name, ver) {}
	VariableNode(const VersionedVariable& v)
	: var(v) {}

	void setVariable(const VarName& name, int ver)
	{
		var.name = name;
		var.version = ver;
	}

	void setVariable(const VersionedVariable& v)
	{
		var = v;
	}

	virtual void writeDotString(std::ostream& out) const
	{
		out << "[label=\"" << toString() << "\"]";
	}

	std::string toString() const
	{
		std::ostringstream os;
		os << var;
		return os.str();
	}

	//! The variable name with a version attached to this node.
	VersionedVariable var;
};

struct PhiNode : VariableNode
{
	PhiNode(const VersionedVariable& v) : VariableNode(v) {}

	//std::vector<ValueGraphNode*> nodes;

	virtual void writeDotString(std::ostream& out) const
	{
		out << "[label=\""  << "phi\\n" << toString() << "\"]";
	}

};

struct ValueNode : VariableNode
{
	ValueNode(SgValueExp* exp) : VariableNode(), valueExp(exp) {}

	virtual void writeDotString(std::ostream& out) const
	{
		out << "[label=\""  << valueExp->unparseToString() << "\\n" << toString() << "\"]";
	}

	SgValueExp* valueExp;
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


	//! Given a variantT value, return a coresponding operator type.
	static OperatorType getOperatorType(VariantT t);

	OperatorNode(OperatorType t) : ValueGraphNode(), type(t) {}
	OperatorNode(VariantT t) : ValueGraphNode(), type(getOperatorType(t)) {}
	OperatorType type;

	virtual void writeDotString(std::ostream& out) const;
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

inline VariableNode* isVariableNode(ValueGraphNode* node)
{
	return dynamic_cast<VariableNode*>(node);
}

inline TempVariableNode* isTempVariableNode(ValueGraphNode* node)
{
	return dynamic_cast<TempVariableNode*>(node);
}

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

}  // End of namespace Backstroke

#endif	/* BACKSTROKE_VALUEGRAPHNODE_H */

