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

    bool isNull() const { return name.empty(); }
    SgType* getType() const { return name.back()->get_type(); }
    SgExpression* getVarRefExp() const;
	
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
	ValueGraphNode(SgNode* node = NULL) : astNode(node) {}

	virtual std::string toString() const
	{ return ""; }

    virtual int getCost() const
    { return 0; }

    //! The corresponding AST node of this value graph node.
    SgNode* astNode;
};

//! A value node can hold a lvalue and a rvalue.
struct ValueNode : ValueGraphNode
{    
	explicit ValueNode(SgNode* node = NULL) : ValueGraphNode(node) {}
    ValueNode(const VersionedVariable& v, SgNode* node = NULL)
    : ValueGraphNode(node), var(v) {}

	virtual std::string toString() const;
    virtual int getCost() const;

    //void addVariable(const VersionedVariable& newVar);

	bool isTemp() const { return var.isNull(); }

    //! If the AST node is a value expression, it is avaiable.
	bool isAvailable() const { return isSgValueExp(astNode) != NULL; }

    //! Get the type of the value.
    SgType* getType() const;
    

//    SgNode* getNode() const { return astNode; }
//    void setNode(SgNode* node)  { astNode = node; }

    //! All variables sharing the same value.
	VersionedVariable var;

    //! The unique name of this value node in VG which becomes
    //! the name of the corresponding variable
    std::string str;
};

//! This node represents a phi node in the SSA form CFG. The AST node inside (actually
//! from its parent class ValueNode) only describe the place of this Phi node in the CFG.
struct PhiNode : ValueNode
{
    enum GateType
    {
        phi,
        mu,
        eta
    };

	PhiNode(const VersionedVariable& v, SgNode* node)
    : ValueNode(v, node), dagIndex(0)/*, type(phi)*/ {}

	//std::vector<ValueGraphNode*> nodes;

	virtual std::string toString() const
	{ return "PHI_" + var.toString(); }

    virtual int getCost() const;

//    SgType* getType() const
//    { return var.name.back()->get_type(); }

    //! The DAG index.
    int dagIndex;

    //! The type of this gate function
    //GateType type;
};

//! An operator node represents a unary or binary operation. It only has one in edge,
//! and one or two out edges, for unary and binary operation separately.
struct OperatorNode : ValueGraphNode
{
    static std::map<VariantT, std::string> typeStringTable;
    static void buildTypeStringTable();

	//OperatorNode(OperatorType t) : ValueGraphNode(), type(t) {}
	OperatorNode(VariantT t, SgNode* node = NULL);

	virtual std::string toString() const;
	
	VariantT type;
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
	ValueGraphEdge(int cst, int dagIdx, const PathSet& pths)
    : cost(cst), dagIndex(dagIdx), paths(pths) {}

    virtual ~ValueGraphEdge() {}

	virtual std::string toString() const;
    
    virtual ValueGraphEdge* clone() 
    { return new ValueGraphEdge(*this); }

    //! The cost attached on this edge. The cost may come from state saving,
    //! or operations.
	int cost;

    //! A CFG may be seperated into several DAGs, and each DAG have its own path
    //! numbers. This index represents which DAG the following paths belong to.
    int dagIndex;

    //! All paths on which this relationship exists.
    PathSet paths;
};

//! An edge coming from an operator node.
struct OrderedEdge : ValueGraphEdge
{
	explicit OrderedEdge(int idx) : index(idx) {}

	virtual std::string toString() const
	{ return boost::lexical_cast<std::string>(index); }
    
    virtual OrderedEdge* clone() 
    { return new OrderedEdge(*this); }

	int index;
};

#if 0
//! An edge coming from a phi node.
struct PhiEdge : ValueGraphEdge
{
    PhiEdge(const std::set<ReachingDef::FilteredCfgEdge>* edges)
    : ValueGraphEdge(0, dagIdx, paths), visiblePathNum(visibleNum) {}
    //! A set of edges indicating where the target def comes from in CFG.
    std::set<ReachingDef::FilteredCfgEdge> cfgEdges;
};
#endif

//! An edge going to the root node.
struct StateSavingEdge : ValueGraphEdge
{
	//StateSavingEdge() : visiblePathNum(0) {}
    StateSavingEdge(int cost, int dagIdx, int visibleNum,
        const PathSet& paths, SgNode* killerNode)
    :   ValueGraphEdge(cost, dagIdx, paths), 
        visiblePathNum(visibleNum), killer(killerNode) {}

	virtual std::string toString() const;
    
    virtual StateSavingEdge* clone() 
    { return new StateSavingEdge(*this); }

	int visiblePathNum;
    SgNode* killer;
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

inline StateSavingEdge* isStateSavingEdge(ValueGraphEdge* edge)
{
	return dynamic_cast<StateSavingEdge*>(edge);
}

}  // End of namespace Backstroke

#endif	/* BACKSTROKE_VALUEGRAPHNODE2_H */

