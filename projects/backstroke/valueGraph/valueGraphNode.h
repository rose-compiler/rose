#ifndef BACKSTROKE_VALUEGRAPHNODE2_H
#define	BACKSTROKE_VALUEGRAPHNODE2_H

#include "types.h"
#include <slicing/backstrokeCDG.h>
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

	explicit PhiNodeDependence(int v)
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
	explicit ValueGraphNode(SgNode* node = NULL) : astNode(node) {}

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
    explicit ValueNode(const VersionedVariable& v, SgNode* node = NULL)
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
//    enum GateType
//    {
//        phi,
//        mu,
//        eta
//    };

	PhiNode(const VersionedVariable& v, SgNode* node)
    : ValueNode(v, node) {}

	//std::vector<ValueGraphNode*> nodes;

	virtual std::string toString() const
    { return "PHI_" + var.toString(); }

    virtual int getCost() const;

//    SgType* getType() const
//    { return var.name.back()->get_type(); }

//    //! The DAG index.
//    int dagIndex;
    
    //bool mu;

    //! The type of this gate function
    //GateType type;
};

//! A Mu node is a special phi node which has a data dependence through a back 
//! edge in a loop. This node is placed on the loop header node in CFG.
struct MuNode : PhiNode
{
    MuNode(const VersionedVariable& v, SgNode* node)
    : PhiNode(v, node), dagIndex(0), isCopy(false) {}
    
    explicit MuNode(const PhiNode& phiNode) 
    : PhiNode(phiNode), dagIndex(0), isCopy(false) {}
    	
    virtual std::string toString() const
	{ 
        std::string str = "MU_" + var.toString() + "\\n" 
                + boost::lexical_cast<std::string>(dagIndex);
        return isCopy ? str + "\\nAVAILABLE" : str;
    }

    //! The DAG index.
    int dagIndex;
    
    //! If it is a copy.
    bool isCopy;
};

//! An operator node represents a unary or binary operation. It only has one in edge,
//! and one or two out edges, for unary and binary operation separately.
struct OperatorNode : ValueGraphNode
{
    static std::map<VariantT, std::string> typeStringTable;
    static void buildTypeStringTable();

	//OperatorNode(OperatorType t) : ValueGraphNode(), type(t) {}
	explicit OperatorNode(VariantT t, SgNode* node = NULL);

	virtual std::string toString() const;
	
	VariantT type;
};

struct FunctionCallNode: ValueGraphNode
{
    typedef boost::tuple<std::string, std::string, std::string> FunctionNamesT;
    
    explicit FunctionCallNode(SgFunctionCallExp* funcCall, bool isRvs = false);
    
    SgFunctionCallExp* getFunctionCallExp() const
    { return isSgFunctionCallExp(astNode); }
    
    //! Returns if a parameter is needed in the reverse functino call.
    bool isNeededByInverse(SgInitializedName* initName) const
    { return true; }
    
    FunctionNamesT getFunctionNames() const;
    
    virtual std::string toString() const;
    
    SgFunctionDeclaration* funcDecl;
    std::string funcName;
    
    //! If this function call node is the reverse one.
    bool isReverse;
    
    //! If this function is declared as virtual.
    bool isVirtual;
    
    //! If this function is declared as const.
    bool isConst;
    
    //! If this function call is from std library.
    bool isStd;
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

/**********************************************************************************************************/
// Value Graph Edges

struct ValueGraphEdge
{
    ValueGraphEdge() : cost(0) {}
    ValueGraphEdge(int cst, const PathInfo& pths)
    : cost(cst), paths(pths) {}
    
    ValueGraphEdge(int cst, const PathInfo& pths, const ControlDependences& cd)
    : cost(cst), paths(pths), controlDependences(cd) {}

    virtual ~ValueGraphEdge() {}

	virtual std::string toString() const;
    
    virtual ValueGraphEdge* clone() 
    { return new ValueGraphEdge(*this); }

    //! The cost attached on this edge. The cost may come from state saving,
    //! or operations.
	int cost;
    
    //! All paths on which this relationship exists.
    PathInfo paths;
    
    //! All immediate control dependences representing conditions in VG.
    ControlDependences controlDependences;
};

//! An edge coming from an operator node.
struct OrderedEdge : ValueGraphEdge
{
	explicit OrderedEdge(int idx) : index(idx) {}

	virtual std::string toString() const
	{ return boost::lexical_cast<std::string>(index) 
            + "\\n" + ValueGraphEdge::toString(); }
    
    virtual OrderedEdge* clone() 
    { return new OrderedEdge(*this); }

	int index;
};

#if 1
//! An edge coming from a phi node.
struct PhiEdge : ValueGraphEdge
{
    PhiEdge(int cst, const PathInfo& pths, const ControlDependences& cd)
    : ValueGraphEdge(cst, pths, cd), muEdge(false) {}
    //PhiEdge(const std::set<ReachingDef::FilteredCfgEdge>* edges)
    //: ValueGraphEdge(0, dagIdx, paths), visiblePathNum(visibleNum) {}
    //! A set of edges indicating where the target def comes from in CFG.
    //std::set<ReachingDef::FilteredCfgEdge> cfgEdges;
    
    virtual std::string toString() const
    {
        std::string str = ValueGraphEdge::toString();
        return muEdge ? str + "\\nMU" : str;
    }
    
    bool muEdge;
};
#endif

//! An edge going to the root node.
struct StateSavingEdge : ValueGraphEdge
{
	//StateSavingEdge() : visiblePathNum(0) {}
//    StateSavingEdge(int cost, int dagIdx, int visibleNum,
//        const PathSet& paths, SgNode* killerNode)
//    :   ValueGraphEdge(cost, dagIdx, paths), 
//        visiblePathNum(visibleNum), killer(killerNode) {}
    
    StateSavingEdge(int cost, const PathInfo& paths, const ControlDependences& cd,
                    SgNode* killerNode, bool isKillerScope = false)
    :   ValueGraphEdge(cost, paths, cd), 
        killer(killerNode), 
        scopeKiller(isKillerScope),
        varStored(false) 
    {}
    
    StateSavingEdge(int cost, const PathInfo& paths, const ControlDependences& cd,
                    const std::map<int, PathSet> visiblePaths, 
                    SgNode* killerNode, bool isKillerScope = false)
    :   ValueGraphEdge(cost, paths, cd), 
        visiblePaths(visiblePaths), 
        killer(killerNode), 
        scopeKiller(isKillerScope),
        varStored(false)
    {}
    
    
	virtual std::string toString() const;
    
    virtual StateSavingEdge* clone() 
    { return new StateSavingEdge(*this); }

	//int visiblePathNum;
    std::map<int, PathSet> visiblePaths;
    SgNode* killer;
    
    //! Indicate if the killer is a scope or not. If the killer is a scope, the SS
    //! statement is inserted at the end of the scope. Or else, it is inserted before
    //! the killer statement.
    bool scopeKiller;
    
    //! If state saving is done. It is needed since a SS edge in a loop is traversed
    //! more than once by difference DAGs.
    bool varStored;
    
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

inline MuNode* isMuNode(ValueGraphNode* node)
{
	return dynamic_cast<MuNode*>(node);
}

inline FunctionCallNode* isFunctionCallNode(ValueGraphNode* node)
{
	return dynamic_cast<FunctionCallNode*>(node);
}

inline OrderedEdge* isOrderedEdge(ValueGraphEdge* edge)
{
	return dynamic_cast<OrderedEdge*>(edge);
}

inline StateSavingEdge* isStateSavingEdge(ValueGraphEdge* edge)
{
	return dynamic_cast<StateSavingEdge*>(edge);
}

inline PhiEdge* isPhiEdge(ValueGraphEdge* edge)
{
	return dynamic_cast<PhiEdge*>(edge);
}

}  // End of namespace Backstroke

#endif	/* BACKSTROKE_VALUEGRAPHNODE2_H */

