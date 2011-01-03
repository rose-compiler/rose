#ifndef BACKSTROKE_VALUE_GRAPH
#define BACKSTROKE_VALUE_GRAPH

#include <ssa/staticSingleAssignment.h>
#include <boost/graph/adjacency_list.hpp>
#include <boost/bind.hpp>

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

struct VariableWithVersion
{
	VariableWithVersion() {}
	VariableWithVersion(const VarName& varName, int ver, bool pseudoDef = false)
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
	ValueGraphNode(const VariableWithVersion& v)
	: isTemp(false), var(v) {}

	void setVariable(const VarName& name, int ver)
	{
		var.name = name;
		var.version = ver;
		isTemp = false;
	}

	void setVariable(const VariableWithVersion& v)
	{
		var = v;
		isTemp = false;
	}

	virtual void writeDotString(std::ostream& out) const
	{
		if (isTemp)
			out << "[label=\"" << "TEMP" << "\"]";
		else
			out << "[label=\"" << toString() << "\"]";
	}

	std::string toString() const
	{
		if (isTemp)
			return "TEMP";
		else
		{
			std::ostringstream os;
			os << var;
			return os.str();
		}
	}
	
	//! Indicate if this node is a temporary variable node.
	bool isTemp;

	//! The variable name with a version attached to this node.
	VariableWithVersion var;

};

struct PhiNode : ValueGraphNode
{
	PhiNode(const VariableWithVersion& v) : ValueGraphNode(v) {}
	
	std::vector<ValueGraphNode*> nodes;

	virtual void writeDotString(std::ostream& out) const
	{
		out << "[label=\""  << "phi\\n" << toString() << "\"]";
	}

};

struct ValueNode : ValueGraphNode
{
	ValueNode(SgValueExp* exp) : ValueGraphNode(), valueExp(exp) {}

	virtual void writeDotString(std::ostream& out) const
	{
		out << "[label=\""  << valueExp->unparseToString() << "\\n" << toString() << "\"]";
	}

	SgValueExp* valueExp;
};

struct OperaterNode : ValueGraphNode
{
	enum OperaterType
	{
		otAdd,
		otMinus,
		otMultiply,
		otDevide,
		otMod,
		otAssign,
	};

	OperaterNode(OperaterType t) : ValueGraphNode(), type(t) {}
	OperaterType type;

	virtual void writeDotString(std::ostream& out) const
	{
		std::string label;
		switch (type)
		{
			case otAdd:
				label = "+";
				break;
			case otMultiply:
				label = "Mul";
				break;
			default:
				break;
		}

		out << "[label=\""  << label << "\\n" << toString() << "\"]";
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

private:
	//! The SSA form of the function definition.
	SSA ssa_;
	
	//! A map from SgNode to vertex of Value Graph.
	std::map<SgNode*, Vertex> nodeVertexMap_;

	//! A map from variable with version to vertex of Value Graph.
	std::map<VariableWithVersion, Vertex> varVertexMap_;

//	typedef CFG<VirtualCFG::InterestingNode,
//			VirtualCFG::InterestingEdge> CFG;
//	typedef CDG<CFG> CDG;
public:
	
	ValueGraph() 
	: ssa_(SageInterface::getProject())
	{
		ssa_.run(false);
	}

	void build(SgFunctionDefinition* funcDef);

	void toDot(const std::string& filename) const;

private:
	//! This function set or add a def node to the value graph. If the variable defined is assigne by a 
	//! node with a temporary variable, just set the name and version to the temporary one.
	//! Or else, build a new graph node and add an edge from this new node to the target node.
	void setNewDefNode(SgNode* defNode, Vertex useVertex);

	void writeGraphNode(std::ostream& out, const Vertex& node) const
	{
		(*this)[node]->writeDotString(out);
	}

	void writeGraphEdge(std::ostream& out, const Edge& edge) const
	{
		//(*this)[edge]->writeDotString();
	}

	/** Given a SgNode, return its variable name and version.
	 * 
	 *  @param node A SgNode which is the variable.
	 *  @param isUse Inidicate if the variable is a use or a def.
	 */
	VariableWithVersion getVariableWithVersion(SgNode* node, bool isUse = true) const;

	static Vertex nullVertex()
	{ return boost::graph_traits<ValueGraph>::null_vertex(); }
	
};

} // End of namespace Backstroke



#endif // BACKSTROKE_VALUE_GRAPH