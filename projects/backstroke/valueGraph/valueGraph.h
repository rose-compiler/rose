#ifndef BACKSTROKE_VALUE_GRAPH
#define BACKSTROKE_VALUE_GRAPH

#include <ssa/staticSingleAssignment.h>
#include <boost/graph/adjacency_list.hpp>
#include <boost/bind.hpp>

namespace Backstroke
{

typedef std::vector<SgInitializedName*> VarName;

struct VariableWithVersion
{
	VariableWithVersion() {}
	VariableWithVersion(const VarName& varName, int ver)
	: name(varName), version(ver) {}
	
	VarName name;
	int version;
};

inline bool operator == (const VariableWithVersion& var1, const VariableWithVersion& var2)
{
	return var1.name == var2.name && var1.version == var2.version;
}

inline bool operator < (const VariableWithVersion& var1, const VariableWithVersion& var2)
{
	return var1.name < var2.name ||
		(var1.name == var2.name && var1.version < var2.version);
}

inline std::ostream& operator << (std::ostream& os, const VariableWithVersion& var)
{
	return os << StaticSingleAssignment::varnameToString(var.name) << ' ' << var.version;
}


struct ValueGraphNode
{
	ValueGraphNode() : isTemp(true) {}
	ValueGraphNode(const VarName& name, int ver)
	: isTemp(false), var(name, ver) {}

	void setVariable(const VarName& name, int ver)
	{
		var.name = name;
		var.version = ver;
		isTemp = false;
	}

	virtual void writeDotString(std::ostream& out) const = 0;
	
	//! Indicate if this node is a temporary variable node.
	bool isTemp;

	//! The variable name with a version attached to this node.
	VariableWithVersion var;

};

struct PhiNode : ValueGraphNode
{
	std::vector<ValueGraphNode*> nodes;

	virtual void writeDotString(std::ostream& out) const
	{
		out << "[label=\""  << "phi\\n" << var << "\"]";
	}

};

struct ValueNode : ValueGraphNode
{
	ValueNode(SgValueExp* exp) : valueExp(exp) {}

	virtual void writeDotString(std::ostream& out) const
	{
		out << "[label=\""  << valueExp->unparseToString() << "\\n" << var << "\"]";
	}

	SgValueExp* valueExp;
};

enum OperaterType
{
	otAdd,
	otMinus,
	otMultiply,
	otDevide,
	otMod,
	otAssign,
};

struct OperaterNode : ValueGraphNode
{
	OperaterNode(OperaterType t) : type(t) {}
	OperaterType type;

	virtual void writeDotString(std::ostream& out) const
	{
		out << "[label=\""  << type << "\\n" << var << "\"]";
	}
};

#if 0
struct UnaryOperaterNode : OperaterNode
{
	enum OperationType
	{
		otPlus,
		otMinus
	} type;

	ValueGraphNode* operand;
};


struct BinaryOperaterNode : OperaterNode
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


struct ValueGraphEdge
{
	//virtual void writeDotString(std::ostream& out) const = 0;
};

class ValueGraph : public boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
		ValueGraphNode*, ValueGraphEdge*>
{
public:
	typedef boost::graph_traits<ValueGraph>::vertex_descriptor Vertex;
	typedef boost::graph_traits<ValueGraph>::edge_descriptor Edge;

	typedef StaticSingleAssignment SSA;
	typedef SSA::VarName VarName;

//	typedef CFG<VirtualCFG::InterestingNode,
//			VirtualCFG::InterestingEdge> CFG;
//	typedef CDG<CFG> CDG;
	
	ValueGraph() {}

	void build(SgFunctionDefinition* funcDef);

	void toDot(const std::string& filename) const;

private:
	void writeGraphNode(std::ostream& out, const Vertex& node) const
	{
		(*this)[node]->writeDotString(out);
	}

	void writeGraphEdge(std::ostream& out, const Edge& edge) const
	{
		//(*this)[edge]->writeDotString();
	}
	
};

} // End of namespace Backstroke



#endif // BACKSTROKE_VALUE_GRAPH